QT       -= core gui


TEMPLATE  = lib

CONFIG    += warn_on
CONFIG    += thread
CONFIG	  += dll

LIBS      += -lsetupapi

DEFINES += SA430_EXPORTS
DEFINES -= UNICODE
DEFINES += "WINVER=0x0500"

SOURCES += sa430.cpp \
    cUsbDetect.cpp \
    cThread.cpp \
    cRegAccess.cpp \
    cMutex.cpp \
    cEvent.cpp \
    cDriver.cpp \
    cDeviceDriver.cpp \
    dllmain.cpp

HEADERS += sa430.h\
    cUsbDetect.h \
    cThread.h \
    cRegAccess.h \
    cMutex.h \
    cEvent.h \
    cDriver.h \
    cDeviceDriver.h \
    sa430TypeDef.h

CONFIG(debug, debug|release){
    TARGET    = sa430dbg
   }else{
    TARGET    = sa430
   }
