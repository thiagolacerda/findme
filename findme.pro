#-------------------------------------------------
#
# Project created by QtCreator 2011-01-05T17:45:56
#
#-------------------------------------------------

QT       += core gui network
#xml network

TARGET = findme
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

CONFIG += mobility
MOBILITY = location \
            messaging

symbian {
    TARGET.UID3 = 0xeb22583b
    TARGET.CAPABILITY += Location \
                        ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        UserEnvironment

    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000

    #LIBS += -lprofileengine
}

maemo5 {
    QT += dbus
}

#RESOURCES += \
#    findme.qrc
