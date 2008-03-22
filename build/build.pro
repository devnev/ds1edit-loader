TEMPLATE = app
INCLUDEPATH += .
TARGET = ds1edit_loader
HEADERS += ../src/loaderdialog.h ../src/iniparser.h ../src/loaderparams.h ../src/selectwantedfiledialog.h \
	../src/ds1info.h ../src/parseerror.h ../src/exception.h ../src/stacktrace.h ../src/editorinterface.h ../src/dt1listdialog.h \
	../src/editorconfigdialog.h ../src/loaderconfigdialog.h ../src/datasync.h ../src/mapdialog.h ../src/aboutdialog.h \
	../src/maplistwidget.h ../src/mapsnotfounddialog.h ../src/initialconfigdialog.h
SOURCES += ../src/loaderdialog.cpp ../src/main.cpp ../src/iniparser.cpp ../src/loaderparams.cpp \
	../src/selectwantedfiledialog.cpp ../src/stacktrace.cpp ../src/editorinterface.cpp ../src/dt1listdialog.cpp \
	../src/editorconfigdialog.cpp ../src/loaderconfigdialog.cpp ../src/datasync.cpp ../src/mapdialog.cpp ../src/aboutdialog.cpp \
	../src/maplistwidget.cpp ../src/mapsnotfounddialog.cpp ../src/initialconfigdialog.cpp
CONFIG += release warn_on qt thread
DESTDIR = ../bin
RESOURCES = ../ds1editloader.qrc
RC_FILE = ../ds1editloader.rc
DEFINES += UNICODE _UNICODE NDEBUG
win32 {
	HEADERS += ../src/dde.h
	SOURCES += ../src/dde.cpp
}
