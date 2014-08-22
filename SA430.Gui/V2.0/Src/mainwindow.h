/* --COPYRIGHT--,BSD
 * Copyright (c) 2011, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QStringListModel>
#include <QDateTime>
#include <QTimer>

#include "appTypedef.h"
#include "appPlot.h"
#include "appStatusbar.h"
#include "drvSA430.h"
#include "appSettings.h"
#include "appFwUpdater.h"

namespace Ui {
    class MainWindow;
}


typedef struct sStatusSpectrum
{
 bool           flagModeContinouse;
 bool           flagActiveFrqValues;
 bool           flagUndoFrqValues;
 sFrqValues     activFrqValues;
 sFrqValues     undoFrqValues;
}sStatusSpectrum;

typedef struct sStatus
{
 sStatusSpectrum Spectrum;
}sStatus;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    // App Events
    void eventAppClose(void);

    // Driver Events
    void eventSA430DeviceConnected(void);
    void eventSA430DeviceDisconnected(void);
    void eventSA430FirmwareUpdateRequired(QString ComPort);
    void eventSA430NewParameterSet(bool Done, int SpecId);
    void eventSA430SpectrumReceived(void);
    void eventSA430ErrorMsg(QString Msg);

    // Help Events
    void eventHelpOpenUsersGuide(void);
    void eventHelpOpenSupportLink(void);

    // FirmwarUpdate Events
    void eventFirmwarUpdate(void);
    // Frequency Settings
    void eventFrqStartChanged(void);
    void eventFrqStopChanged(void);
    void eventFrqCenterChanged(void);
    void eventFrqSpanChanged(void);
    // Date& Timer Events
    void eventTimeDateUpdateTick(void);
    // Hardware Events
    void eventUpdateDeviceList(void);
    void eventDeviceListIndexChanged(int index);
    void eventDeviceStartStop(bool On);
    void eventDeviceConnect(void);
    void eventDeviceDisconnect(void);
    // Grid Events
    void eventGridMode_Off(void);
    void eventGridMode_Lines(void);
    void eventGridMode_Dotted(void);

    void eventFrqModeCenterRangeChange(void);
    // Marker Events
    void eventMarker0OnOff(bool flagOn);
    void eventMarker1OnOff(bool flagOn);
    void eventMarker2OnOff(bool flagOn);
    void eventMarkerPeakOnOff(bool flagOn);
    void eventMarkerAllOn(void);
    void eventMarkerAllOff(void);
    void eventMarkerMove(int index);
    void eventMarker0SetTrace(int index);
    void eventMarker1SetTrace(int index);
    void eventMarker2SetTrace(int index);
    void eventSelectMarker0(void);
    void eventSelectMarker1(void);
    void eventSelectMarker2(void);
    // Trace Events
    void eventTrace0ModeChanged(int mode);
    void eventTrace1ModeChanged(int mode);
    void eventTrace2ModeChanged(int mode);
    void eventTrace3ModeChanged(int mode);
    void eventTrace0ColorChange(void);
    void eventTrace1ColorChange(void);
    void eventTrace2ColorChange(void);
    void eventTrace3ColorChange(void);
    void eventTrace0Export(void);
    void eventTrace1Export(void);
    void eventTrace2Export(void);
    void eventTrace3Export(void);
    void eventTrace0HoldTrigger(bool On);
    void eventTrace1HoldTrigger(bool On);
    void eventTrace2HoldTrigger(bool On);
    void eventTrace3HoldTrigger(bool On);
    void eventTrace0Clear(void);
    void eventTrace1Clear(void);
    void eventTrace2Clear(void);
    void eventTrace3Clear(void);

    // Frqency Settings Events
    void eventFrqRangeChanged(void);

    void eventFrqSet(void);
    void eventFrqSpectrumStop(void);
    void eventFrqSave(void);
    void eventFrqLoad(void);
    void eventFrqUndo(void);
    // Div. Tool Events
    void eventScreenCommentTextChanged(QString Text);
    void eventExport(void);
    void eventPrint(void);
    void eventCapture(void);
private:
    Ui::MainWindow  *ui;

    appFwUpdater    *sa430FwUpdater;

    QTimer          *timedateCtrl;
    drvSA430        *deviceCtrl;
    appPlot         *plotCtrl;
    appStatusBar    *statusbarCtrl;
    appSettings     *Settings;
    sStatus         Status;
    QList<sa430UsbDevice> DeviceList;

    QStringList      strHwUsbInfoList;
    QStringListModel *mHwUsbInfo;

    QStringList      strHwDevInfoList;
    QStringListModel *mHwDevInfo;

    sFrqValues ActiveSpecParameter;
    int        ActiveSpecId;

    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionSpectrumStart;
    QAction *actionSpectrumStop;
    QAction *actionSave;
    QAction *actionPrint;
    QAction *actionOpenUsersGuide;
    QAction *actionOpenSupportLink;


    void initGui(void);
    void initGuiEvents(void);
    void initPlotCtrl(void);

    void guiUpdateDeviceList(void);
    void guiUpdateFrq(sFrqValues *FrqSetting);
    void guiShowDeviceInfo(sa430UsbDevice *device);
    void guiSetGridMode(eGridMode mode);
    void guiSetTraceMode(eTrace trace, eTraceMode mode);
    bool guiSaveFrqProfile(sFrqSetting &tmpFrqSet);
    bool guiLoadFrqProfile(sFrqSetting *newFrqSetting);
    bool guiDisplayHwUsbInfo(sa430UsbDevice *usbInfo);
    bool guiDisplayHwDevInfo(sDeviceInfo *devInfo);
    bool guiDisplayHwCalData(sCalibrationData *calData);
    bool guiDisplayFrqSettings(sFrqValues *FrqSettings);
    bool guiDisplayMarkerInfo(sMarkerInfo *mInfo);

    void guiMarkerSelect(eMarker Marker);
    bool guiMarkerSetTrace(eMarker Marker, eTrace TraceNr);
    void guiMarkerSetTraceUpdate(eTrace TraceNr);
    void guiTraceColorSelect(eTrace TraceNr);
    void guiTraceColorSet(eTrace TraceNr, QColor newColor);
    void guiTraceModeChange(eTrace TraceNr, eTraceMode trcMode);
    bool guiTraceExport(eTrace TraceNr,eTraceMode TraceMode);
    bool guiTraceHoldTrigger(eTrace TraceNr);
    bool guiTraceHoldReset(eTrace TraceNr);
    bool guiTraceClear(eTrace TraceNr);

    bool guiFrqGetActualSettings(sFrqValues *actualFrqValues);
    bool guiFrqCalcEasyRfSettings(sFrqValues *FrqValues);
    void guiFrqCenterSpan2StartStop(void);
    void guiFrqStartStop2CenterSpan(void);

    QString s32ToString(long value);

    bool guiFwUpdateIfRequired(sa430UsbDevice *UsbInfo, sDeviceInfo *DevInfo);
    bool guiFwUpdate(QString ComPort);
};

#endif // MAINWINDOW_H
