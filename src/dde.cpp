
#include "dde.h"
#include <QRegExp>
#include <tchar.h>
#include <windows.h>
#include <ddeml.h>
#include <set>
#include <cstring>
#include <QFileInfo>
#include "stacktrace.h"

const TCHAR * const ServiceName = TEXT("Ds1editLoader");
const TCHAR * const TopicName = TEXT("System");
// support dde commands: Open(filename), Run() (, Exit())

#ifdef NDEBUG
#define DEBUG_ONLY(exp) ((void)0)
#else
#define DEBUG_ONLY(exp) exp
#endif

#ifdef UNICODE
static const int CODEPAGE = CP_WINUNICODE;
#else
static const int CODEPAGE = CP_WINANSI;
#endif

/*class DdeString
{
	HSZ ddeString;
	bool owner;
public:
	DdeString(const char* string) : ddeString(NULL), owner(false) {
		if (string)
			ddeString = DdeCreateStringHandleA(
				DdeInstance::getId(), string, CP_WINANSI);
		owner = (ddeString != NULL);
	}
	~DdeString() {
		if (DdeString && owner)
			DdeFreeStringHandle(DdeInstance::getId, ddeString);
	}
	HSZ release() { owner = false; return ddeString; }
	void disown() { owner = false; }
	operator HSZ () const { return DdeString; }
};

class DdeData
{
	HDDEDATA ddeData;
	bool owner;
public:
	DdeData(void* data, size_t size) : ddeData(NULL), owner(false) {
		if (data)
			ddeData = DdeCreateDataHandle(
				DdeInstance::getId(),
				data, size, CP_WINANSI);
		owner = (ddeData != NULL);
	}
	~DdeData() {
		if (ddeData && owner)
			DdeFreeStringHandle(DdeInstance::getId, ddeData);
	}
	HDDEDATA release() { owner = false; return ddeData; }
	void disown() { owner = false; }
	operator HDDEDATA () const { return ddeData; }
};*/

/*class WindowsMimeAcessor : private QWindowsMime
{
public:
	static QWindowsMime *converterToMime(const QString &mimeType, IDataObject *pDataObj)
	{ return QWindowsMime::converterToMime(mimeType, pDataObj); }
	static QStringList allMimesForFormats(IDataObject *pDataObj)
	{ return QWindowsMime::allMimesForFormats(pDataObj); }
	static QWindowsMime *converterFromMime(const FORMATETC &formatetc, const QMimeData *mimeData);
	{ return QWindowsMime::converterFromMime(formatetc, mimeData); }
	static QVector<FORMATETC> allFormatsForMime(const QMimeData *mimeData)
	{ return QWindowsMime::allFormatsForMime(mimeData); }
};*/

class DdeServer
{
	DWORD idInst;
	static DdeServer serverInstance;

	static HDDEDATA CALLBACK DdeCallback(
		UINT uType,
		UINT uFmt,
		HCONV hconv,
		HSZ hsz1,
		HSZ hsz2,
		HDDEDATA hdata,
		DWORD dwData1,
		DWORD dwData2
	);
	HDDEDATA handleCommand(const QString& command);

	int startupCount;

	HSZ hServiceName, hTopicName;

	void emitExit();
	void emitRun();
	void emitOpen(const QString& fileName);

public:
	DdeServer() : idInst(0), startupCount(0) { }
	~DdeServer() { if (idInst) DdeUninitialize(idInst); }
	void startup();
	void shutdown();
	void forceShutdown();

	static DdeServer& instance() { return serverInstance; };

	typedef std::set<DdeInterface*> InterfaceList;
	InterfaceList interfaces;
};

DdeServer DdeServer::serverInstance;

void DdeServer::startup()
{
	FNTRACE("", "DdeServer", "startup", "");

	if (startupCount == 0)
	{
		DEBUG_ONLY(std::cout << "Starting up DDE server\n");
		if (DdeInitialize(&idInst, DdeServer::DdeCallback, APPCLASS_STANDARD |
			CBF_FAIL_ADVISES | CBF_FAIL_POKES | CBF_FAIL_REQUESTS | CBF_FAIL_SELFCONNECTIONS |
			CBF_SKIP_ALLNOTIFICATIONS, 0) == 0)
			DEBUG_ONLY(std::cout << "DDE initialized\n");

		std::auto_ptr<TCHAR> string(new TCHAR[_tcslen(ServiceName)+1]);
		_tcscpy(string.get(), ServiceName);
		DEBUG_ONLY(std::cout << "Allocating service name\n");
		hServiceName = DdeCreateStringHandle(idInst, string.get(), CODEPAGE);
		DEBUG_ONLY(if (hServiceName) std::cout << "Service name allocated\n");

		string.reset(new TCHAR[_tcslen(TopicName)]);
		_tcscpy(string.get(), TopicName);
		DEBUG_ONLY(std::cout << "Allocating topic name\n");
		hTopicName = DdeCreateStringHandle(idInst, string.get(), CODEPAGE);
		DEBUG_ONLY(if (hTopicName) std::cout << "Topic name allocated\n");

		if (DdeNameService(idInst, hServiceName, 0, DNS_REGISTER | DNS_FILTERON))
			DEBUG_ONLY(std::cout << "Service names registered\n");
	}
	++startupCount;
}

void DdeServer::shutdown()
{
	FNTRACE("", "DdeServer", "shutdown", "");

	if (--startupCount < 0) startupCount = 0;
	if (startupCount == 0 && idInst)
	{
		if (DdeNameService(idInst, 0, 0, DNS_UNREGISTER))
			DEBUG_ONLY(std::cout << "Service names unregistered\n");
		if (DdeUninitialize(idInst))
			DEBUG_ONLY(std::cout << "DDE uninitialized\n");
		idInst = 0;
	}
}

void DdeServer::forceShutdown()
{
	FNTRACE("", "DdeServer", "forceShutdown", "");

	if (DdeNameService(idInst, 0, 0, DNS_UNREGISTER))
		DEBUG_ONLY(std::cout << "Service names unregistered\n");
	if (DdeUninitialize(idInst))
		DEBUG_ONLY(std::cout << "DDE uninitialized\n");
	idInst = 0;
	startupCount = 0;
}

HDDEDATA CALLBACK DdeServer::DdeCallback(UINT uType, UINT uFmt, HCONV hconv,
    HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
    DWORD dwData1, DWORD dwData2)
{
	FNTRACE("", "DdeServer", "DdeCallback", "");

	switch(uType)
	{
	case XTYP_CONNECT:
		DEBUG_ONLY(std::cout << "Connection Attempt\n");
		if (hsz2 == DdeServer::instance().hServiceName &&
			hsz1 == DdeServer::instance().hTopicName)
		{
			DEBUG_ONLY(std::cout << "Connected\n");
			return (HDDEDATA) TRUE;
		}
		return (HDDEDATA) FALSE;

	case XTYP_WILDCONNECT:
		DEBUG_ONLY(std::cout << "Wildcard Connection Attempt\n");
		if ((!hsz1 && !hsz2) ||
			hsz2 == DdeServer::instance().hServiceName ||
			hsz1 == DdeServer::instance().hTopicName)
		{
			DEBUG_ONLY(std::cout << "Wildcard match\n");
			HSZPAIR matchList[] = {
				{ DdeServer::instance().hServiceName,
				  DdeServer::instance().hTopicName },
				{ NULL,
				  NULL }
			};
			return DdeCreateDataHandle(DdeServer::instance().idInst,
				reinterpret_cast<BYTE*>(matchList), sizeof(matchList), 0, NULL, 0, 0);
		}
		return (HDDEDATA) NULL;

	case XTYP_EXECUTE:
		DEBUG_ONLY(std::cout << "Execution Attempt\n");
		if (hsz1 == DdeServer::instance().hTopicName)
		{
			DWORD length = DdeGetData(hdata, NULL, 0, 0);
			// convert length to units of sizeof(TCHAR), rounded up
			length = (length + (length%sizeof(TCHAR)))/sizeof(TCHAR);
			std::auto_ptr<TCHAR> string(new TCHAR[length]);
			DdeGetData(hdata, reinterpret_cast<BYTE*>(string.get()), length*sizeof(TCHAR), 0);
			QString command = QString::fromWCharArray(string.get());
			DdeFreeDataHandle(hdata);
			DEBUG_ONLY(std::cout << "Execution Command (length " << length << "): " << command.toStdString() << '\n');
			return DdeServer::instance().handleCommand(command);
		}
		break;

	default:
		return (HDDEDATA) NULL;
	}
	return (HDDEDATA) NULL;
}

HDDEDATA DdeServer::handleCommand(const QString& command)
{
	FNTRACE("", "DdeServer", "handleCommand", command);

	static QRegExp simpleCmd("\\s*([a-zA-Z_][a-zA-Z0-9_]*)\\s*(\\(\\s*\\)\\s*)?");
	static QRegExp paramsCmd("\\s*([a-zA-Z_][a-zA-Z0-9_]*)\\s*\\(\\s*(.*)\\s*\\)\\s*");
	if (simpleCmd.exactMatch(command))
	{ // parameterless command
		QString name = simpleCmd.cap(1);
		if (name.compare("Exit", Qt::CaseInsensitive) == 0)
		{
			emitExit();
			DEBUG_ONLY(std::cout << "Exit command acknowledged\n");
		}
		else if (name.compare("Run", Qt::CaseInsensitive) == 0)
		{
			emitRun();
			DEBUG_ONLY(std::cout << "Run command acknowledged\n");
		}
		else
			return (HDDEDATA) DDE_FNOTPROCESSED;
		return (HDDEDATA) DDE_FACK;
	}
	else if (paramsCmd.exactMatch(command))
	{
		QString name = paramsCmd.cap(1);
		QString params = paramsCmd.cap(2);
		if (name.compare("Open", Qt::CaseInsensitive) == 0)
		{
			if (params.at(0) == '"' && params.at(params.length()-1) == '"')
				params.remove(0, 1).chop(1); // remove quotes
			emitOpen(QFileInfo(params).absoluteFilePath());
			DEBUG_ONLY(std::cout << "Open command acknowledged\n");
			return (HDDEDATA) DDE_FACK;
		}
		else
			return (HDDEDATA) DDE_FNOTPROCESSED;
	}
	else
		return (HDDEDATA) DDE_FNOTPROCESSED;
}

void DdeServer::emitExit()
{
	FNTRACE("", "DdeServer", "emitExit", "");

	for (InterfaceList::iterator iter = interfaces.begin();
		iter != interfaces.end(); ++iter)
		(*iter)->emitExit();
}

void DdeServer::emitRun()
{
	FNTRACE("", "DdeServer", "emitRun", "");

	for (InterfaceList::iterator iter = interfaces.begin();
		iter != interfaces.end(); ++iter)
		(*iter)->emitRun();
}

void DdeServer::emitOpen(const QString& fileName)
{
	FNTRACE("", "DdeServer", "emitOpen", fileName);

	for (InterfaceList::iterator iter = interfaces.begin();
		iter != interfaces.end(); ++iter)
		(*iter)->emitOpen(fileName);
}

DdeInterface::DdeInterface(QObject* parent)
: QObject(parent)
{
	FNTRACE("", "DdeInterface", "DdeInterface", QString("parent %1 0").arg(parent?"!=":"=="));

	DdeServer::instance().interfaces.insert(this);
	DdeServer::instance().startup();
}

DdeInterface::~DdeInterface()
{
	FNTRACE("", "DdeInterface", "~DdeInterface", "");

	DdeServer::instance().interfaces.erase(this);
	DdeServer::instance().shutdown();
}

/*/// Make sure server is started
void DdeInterface::startup();
/// Release server use
void DdeInterface::shutdown();
/// Make sure server is stopped
void DdeInterface::forceShutdown();*/

/*class DdeInstance
{
	DWORD idInst;
	DdeInstance();
	~DdeInstance();
	bool initializeInstance(CallbackFilters filters = CallbackFilters());
	bool initialized;

	static std::auto_ptr<DdeInstance> instance;
	static DdeInstance* getInstance() {
		if (!instance) instance.reset(new DdeInstance);
		return instance.get();
	}

public:

	enum CallbackFilter {
		FailAllServerActions = CBF_FAIL_ALLSVRXACTIONS,
		FailAdvises = CBF_FAIL_ADVISES,
		FailConnect = CBF_FAIL_CONNECTIONS,
		FailExecute = CBF_FAIL_EXECUTES,
		FailPokes = CBF_FAIL_POKES,
		FailRequests = CBF_FAIL_REQUESTS,
		FailSelfConnect = CBF_FAIL_SELFCONNECTIONS,
		SkipAllNotifications = CBF_SKIP_ALLNOTIFICATIONS,
		SkipConnectConfirms = CBF_SKIP_CONNECT_CONFIRMS,
		SkipDisconnects = CBF_SKIP_DISCONNECTS,
		SkipRegistrations = CBF_SKIP_REGISTRATIONS,
		SkipUnregistrations = CBF_SKIP_UNREGISTRATIONS
	};
	Q_DECLARE_FLAGS(CallbackFilters, CallbackFilter);

	static DWORD* getId() {
		return &(getInstance()->idInst);
	}
	static bool isInitialized() {
		return instance.get() != 0 && instance->initialized;
	}
	void initialize() {
		getInstance();
	}
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DdeInstance::CallbackFilters)

DdeInstance::DdeInstance()
: initialized(false), idInst(0)
{
	initialized = initializeInstance();
}

DdeInstance::~DdeInstance()
{
	DdeUninitialize(idInst);
	idInst = 0;
}

bool DdeInstance::initializeInstance()
{
	return DdeInitialize(&idInst, ddecallback,
		APPCLASS_STANDARD |
		FailAdvises | FailPokes | FailSelfConnect |
		SkipAllNotifications)
		== DMLERR_NO_ERROR;
}

class DdeConversation
{
	HCONV hConversation;
	QString serviceName, topicName;
public:
	DdeConversation(const QString& service = QString(), const QString& topic = QString());
	~DdeConversation();
	void connect(const QString& service = QString(), const QString& topic = QString());
	void disconnect();
	bool isConnected() { return hConversation != 0; }
};

DdeConversation::DdeConversation(const QString& service, const QString& topic)
{
	DdeInstance::initialize();
	connect(service, topic);
}

DdeConversation::~DdeConversation()
{
	disconnect();
}

void disconnect()
{
	DdeDisconnect(hConversation);
	hConversation = 0;
}

void DdeConversation::connect(const QString& service, const QString& topic)
{
	if (service.length() >= 255 || topic.length() >= 255)
		return;
	DdeString serviceString(service.isEmpty() ?
		0 : service.toStdString().c_str());
	DdeString topicString(topic.isEmpty() ?
		0 : topic.toStdString().c_str());
	hConversation = DdeConnect(DdeInstance::getId, serviceString.release(), topicString.release(), 0);
}*/
