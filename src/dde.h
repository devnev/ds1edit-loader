#ifndef DDE_H_INCLUDED
#define DDE_H_INCLUDED

#include <QObject>
#include <QString>
#include <tchar.h>

class DdeServer;

extern const TCHAR * const ServiceName;
extern const TCHAR * const TopicName;


class DdeInterface : public QObject
{
	Q_OBJECT

	bool started;

public:
	DdeInterface(QObject* parent);
	virtual ~DdeInterface();

	/*/// Make sure server is started
	void startup();
	/// Release server use
	void shutdown();
	/// Make sure server is stopped
	void forceShutdown();*/

signals:
	void open(const QString& fileName);
	void exit();
	void run();

private:
	friend class DdeServer;

	void emitExit() { emit exit(); }
	void emitRun() { emit run(); }
	void emitOpen(const QString& fileName) { emit open(fileName); }
};

#endif // DDE_H_INCLUDED
