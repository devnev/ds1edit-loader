TEMPLATE = subdirs
INCLUDEPATH += .
TARGET = ds1edit_loader
SUBDIRS += build
CONFIG += release \
          warn_on \
          qt \
          thread
DESTDIR = bin
RC_FILE = ds1editloader.rc
