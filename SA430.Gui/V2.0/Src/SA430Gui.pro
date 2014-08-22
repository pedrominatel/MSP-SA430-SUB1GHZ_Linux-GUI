#-------------------------------------------------
#
# Project created by QtCreator 2011-04-13T11:58:03
#
#-------------------------------------------------

QT       += core gui xml

TEMPLATE = app

# ---- Qwt Lib and includes setup -----------
INCLUDEPATH += C:/QtSDK/qwt-6.0.0/src
INCLUDEPATH += ../sa430

RC_FILE = Manifest.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    appgrid.cpp \
    appPlot.cpp \
    appStatusbar.cpp \
    appMarker.cpp \
    drvSA430.cpp \
    appSettings.cpp \
    cEvent.cpp \
    appCurve.cpp \
    appConstData.cpp \
    appReportCsv.cpp \
    appFwUpdater.cpp

HEADERS  += mainwindow.h \
    appgrid.h \
    appPlot.h \
    appStatusbar.h \
    appMarker.h \
    drvSA430.h \
    appSettings.h \
    appTypedef.h \
    cEvent.h \
    appCurve.h \
    appReportCsv.h \
    appFwUpdater.h

FORMS    += mainwindow.ui \
    appFwUpdater.ui




CONFIG(debug, debug|release){
    LIBS += ../Build/debug/libsa430dbg.a
    LIBS += C:/QtSDK/qwt-6.0.0/lib/libqwtd.a
    TARGET = SA430GuiDbg
   }else{
    LIBS += ../Build/release/libsa430.a
   LIBS += C:/QtSDK/qwt-6.0.0/lib/libqwt.a
   TARGET = SA430Gui
   }

RESOURCES += \
    Resource.qrc

OTHER_FILES += \
    Manifest.rc




