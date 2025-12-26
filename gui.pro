QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = w27cxxx_flasher
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        chipinfo.cpp \
        logicinfo.cpp \
        dialogseldip.cpp \
        dialogbuf28id.cpp \
        dialogerase.cpp \
        dialogwrite.cpp \
        dialogwrite28.cpp \
        dialogwrite28f.cpp \
        dialogselvpp.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        chipinfo.h \
        logicinfo.h \
        dialogseldip.h \
        dialogbuf28id.h \
        dialogerase.h \
        dialogwrite.h \
        dialogwrite28.h \
        dialogwrite28f.h \
        dialogselvpp.h \
        mainwindow.h \
        testsram.h

FORMS += \
        chipinfo.ui \
        logicinfo.ui \
        dialogseldip.ui \
        dialogbuf28id.ui \
        dialogerase.ui \
        dialogwrite.ui \
        dialogwrite28.ui \
        dialogwrite28f.ui \
        dialogselvpp.ui \
        mainwindow.ui

# Icon for Windows
win32:RC_FILE = icon.rc

DISTFILES += \
    icon.rc
