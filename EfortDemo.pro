#-------------------------------------------------
#
# Project created by QtCreator 2022-10-26T15:59:21
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release){
   TARGET = EftDemod
}
else{
   TARGET = EftDemo
}

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        EftDemo.cpp \
        roboxcomwidget.cpp \
        testtwocontroller.cpp \
        monitorthread.cpp

HEADERS += \
        EftDemo.h \
        roboxcomwidget.h \
        testtwocontroller.h \
        monitorthread.h \
        syschange.h

FORMS += \
        EftDemo.ui \
        roboxcomwidget.ui

RESOURCES += \
        EftDemo.qrc


contains(QT_ARCH, i386) {
win32:DESTDIR	= $$PWD/bin/x86
unix:DESTDIR    = $$PWD/bin/linux

#win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/x86/ -lEftSdkd
#else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/x86/ -lEftSdk
#else:unix: LIBS += -L$$PWD/../libs/lib/ -lEftSdk -L$$PWD/..libs/lib/ -lrlibcpp.bcc -L$$PWD/../libs/lib/ -lrlibcpp.tool
#}
win32:CONFIG(debug, debug|release): LIBS += \
    -L$$PWD/../../libs/x86/ -lEftSdkd
else:win32:CONFIG(release, debug|release): LIBS += \
    -L$$PWD/../../libs/x86/ -lEftSdk
else:unix: LIBS += \
    -L$$PWD/../../libs/linux/ -lEftSdk \
    -L$$PWD/../../libs/linux/ -lrlibcpp.bcc \
    -L$$PWD/../../libs/linux/ -lrlibcpp.tool \
    -L$$PWD/../../libs/linux/ -llog4cpp
}
else {
win32:DESTDIR	= $$PWD/bin/x64
unix:DESTDIR    = $$PWD/bin/linux

#win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/x64/ -lEftSdkd
#else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/x64/ -lEftSdk
#else:unix: LIBS += -L$$PWD/../libs/lib/ -lEftSdk -L$$PWD/..libs/lib/ -lrlibcpp.bcc -L$$PWD/../libs/lib/ -lrlibcpp.tool
#}
win32:CONFIG(debug, debug|release): LIBS += \
    -L$$PWD/../../libs/x64/ -lEftSdkd
else:win32:CONFIG(release, debug|release): LIBS += \
    -L$$PWD/../../libs/x64/ -lEftSdk
else:unix: LIBS += \
    -L$$PWD/../../libs/linux/ -lEftSdk \
    -L$$PWD/../../libs/linux/ -lrlibcpp.bcc \
    -L$$PWD/../../libs/linux/ -lrlibcpp.tool \
    -L$$PWD/../../libs/linux/ -llog4cpp
}

win32:INCLUDEPATH += $$PWD/../../include
win32:DEPENDPATH += $$PWD/../../include

unix:INCLUDEPATH += $$PWD/../../include
unix:DEPENDPATH += $$PWD/../../include

QMAKE_LFLAGS += -no-pie
