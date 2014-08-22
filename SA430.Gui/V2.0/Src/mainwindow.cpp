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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProgressDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QColor>
#include <QColorDialog>
#include <QUrl>
#include <QDesktopServices>

#include "appReportCsv.h"
#include "appSettings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
 ui->setupUi(this);

 deviceCtrl     = new drvSA430();
 plotCtrl       = new appPlot(ui->plotwindow);
 statusbarCtrl  = new appStatusBar(ui->statusBar);
 Settings       = new appSettings;
 mHwUsbInfo     = new QStringListModel(this);
 mHwDevInfo     = new QStringListModel(this);
 timedateCtrl   = new QTimer(this);

 sa430FwUpdater = new appFwUpdater(this);

 Status.Spectrum.flagModeContinouse = false;
 Status.Spectrum.flagActiveFrqValues= false;
 Status.Spectrum.flagUndoFrqValues  = false;

 initGui();
 initGuiEvents();
 initPlotCtrl();


 statusbarCtrl->SetDllGuiVersion(deviceCtrl->GetDllVer(),0x0200);

 // Load Connected Device List

 guiUpdateDeviceList();

 ui->bttnRfModeEasyRf->setChecked(true);
 ui->rbRfRange_389_464->setChecked(true);
 ui->actionDeviceFwUpdate->setEnabled(false);

}

MainWindow::~MainWindow()
{
 delete deviceCtrl;
 delete plotCtrl;
 delete statusbarCtrl;
 delete Settings;
 delete mHwUsbInfo;
 delete mHwDevInfo;
 delete ui;
}

void MainWindow::initGui(void)
{
 // Select hardware tab as start tab
 ui->tabToolBox->setCurrentWidget(ui->tabHardware);
 // Maker
 // Marker Menu Item
 ui->actionMarkerOnOffM0->setChecked(false);
 ui->actionMarkerOnOffM1->setChecked(false);
 ui->actionMarkerOnOffM2->setChecked(false);
 ui->actionMarkerOnOffM3->setChecked(false);
 // Marker Trace Items
 ui->cbMarker0TraceSelected->setCurrentIndex(0);
 ui->cbMarker1TraceSelected->setCurrentIndex(0);
 ui->cbMarker2TraceSelected->setCurrentIndex(0);
 // Traces
 QPixmap pm(32,14);
 pm.fill(Qt::red);
 ui->bttnTraceColor0->setIcon(QIcon(pm));
 guiTraceColorSet(TRACE_0,QColor(Qt::red));
 pm.fill(Qt::green);
 ui->bttnTraceColor1->setIcon(QIcon(pm));
 guiTraceColorSet(TRACE_1,QColor(Qt::green));
 pm.fill(Qt::yellow);
 ui->bttnTraceColor2->setIcon(QIcon(pm));
 guiTraceColorSet(TRACE_2,QColor(Qt::yellow));
 pm.fill(Qt::blue);
 ui->bttnTraceColor3->setIcon(QIcon(pm));
 guiTraceColorSet(TRACE_3,QColor(Qt::blue));

 ui->cbTraceMode0->setCurrentIndex(T_MODE_OFF);
 ui->cbTraceMode1->setCurrentIndex(T_MODE_OFF);
 ui->cbTraceMode2->setCurrentIndex(T_MODE_OFF);
 ui->cbTraceMode3->setCurrentIndex(T_MODE_OFF);

 ui->bttnTraceClear0->setEnabled(false);
 ui->bttnTraceClear1->setEnabled(false);
 ui->bttnTraceClear2->setEnabled(false);
 ui->bttnTraceClear3->setEnabled(false);

 ui->bttnTraceHold0->setEnabled(false);
 ui->bttnTraceHold1->setEnabled(false);
 ui->bttnTraceHold2->setEnabled(false);
 ui->bttnTraceHold3->setEnabled(false);

 // Hardware
 ui->bttnHwConnect->setEnabled(false);

 strHwUsbInfoList.clear();
 mHwUsbInfo->setStringList(strHwUsbInfoList);
 ui->lvHwUsbInfo->setModel(mHwUsbInfo);
 ui->grpHwUsbInfo->setVisible(true);

 strHwDevInfoList.clear();
 mHwDevInfo->setStringList(strHwDevInfoList);
 ui->lvHwDevInfo->setModel(mHwDevInfo);
 ui->grpHwDevInfo->setVisible(false);
 // Freuqency Settings
 // Reference Level
 ui->cbRefDcLevelValue->setCurrentIndex(ui->cbRefDcLevelValue->count()-1);
 // Resolution Bandwidth
 ui->cbResolutionBandWidthValue->setCurrentIndex(ui->cbResolutionBandWidthValue->count()-1);
 ui->rbRfRange_389_464->setChecked(true);

 // Marker
 ui->cbMarker0TraceSelected->setCurrentIndex(4);
 ui->cbMarker1TraceSelected->setCurrentIndex(4);
 ui->cbMarker2TraceSelected->setCurrentIndex(4);
 ui->MarkerDial->setMaximum(48);
 ui->MarkerDial->setMinimum(0);

 // ToolBox TabControl
 ui->tabMarker->setEnabled(false);
 ui->tabScreen->setEnabled(true);
 ui->tabTraces->setEnabled(false);
 ui->tabRfSettings->setEnabled(false);
 // Main Tool Bar
 // Create Action
 actionConnect      = new QAction(QIcon(":/img/48Connect"   ),tr("&Connect"), this);
 actionDisconnect   = new QAction(QIcon(":/img/48Disconnect"),tr("&Disconnect"), this);
 actionSpectrumStart= new QAction(QIcon(":/img/48Play"      ),tr("&Start"  ), this);
 actionSpectrumStop = new QAction(QIcon(":/img/48Stop"      ),tr("&Stop"   ), this);
 actionSave         = new QAction(QIcon(":/img/48Save"      ),tr("&Save"   ), this);
 actionPrint        = new QAction(QIcon(":/img/48Print"     ),tr("&Print"  ), this);
 // Tool Tip Assignment
 actionConnect->setStatusTip(tr("Connect selected Device"));
 actionDisconnect->setStatusTip(tr("Close selected Device"));
 actionSpectrumStart->setStatusTip(tr("Start Spectrum"));
 actionSpectrumStop->setStatusTip(tr("Stop Spectrum"));
 actionSave->setStatusTip(tr("Save"));
 actionPrint->setStatusTip(tr("Print/Export Dialog Windows"));
 // Graph

 // Register Action to ToolBar
 ui->mainToolBar->addAction(actionConnect);
 ui->mainToolBar->addAction(actionDisconnect);
 ui->mainToolBar->addSeparator();
 ui->mainToolBar->addAction(actionSpectrumStart);
 ui->mainToolBar->addAction(actionSpectrumStop);
 ui->mainToolBar->addSeparator();
 ui->mainToolBar->addAction(actionSave);
 ui->mainToolBar->addAction(actionPrint);
 // Init State
 actionConnect->setEnabled(true);
 actionDisconnect->setEnabled(false);
 actionSpectrumStart->setEnabled(false);
 actionSpectrumStop->setEnabled(false);
 // Init Trace
 ui->txtTrace0_Label->hide();
 ui->txtTrace0_Mode->hide();
 ui->txtTrace1_Label->hide();
 ui->txtTrace1_Mode->hide();
 ui->txtTrace2_Label->hide();
 ui->txtTrace2_Mode->hide();
 ui->txtTrace3_Label->hide();
 ui->txtTrace3_Mode->hide();
 // Others
 eventTimeDateUpdateTick();
 timedateCtrl->start(1000);


}

void MainWindow::initGuiEvents(void)
{
 // Application Specific Events

 // Radio Button Grid Select Events
 connect(ui->rbGridModeOff,SIGNAL(clicked()),this,SLOT(eventGridMode_Off()));
 connect(ui->rbGridModeLine,SIGNAL(clicked()),this,SLOT(eventGridMode_Lines()));
 connect(ui->rbGridModeDotted,SIGNAL(clicked()),this,SLOT(eventGridMode_Dotted()));
 // Menu Entry Grid Select Events
 connect(ui->actionGridOff,SIGNAL(triggered()),this,SLOT(eventGridMode_Off()));
 connect(ui->actionGridLines,SIGNAL(triggered()),this,SLOT(eventGridMode_Lines()));
 connect(ui->actionGridDotted,SIGNAL(triggered()),this,SLOT(eventGridMode_Dotted()));

 connect(ui->actionExportPng,SIGNAL(triggered()),this,SLOT(eventExport()));
 connect(ui->actionPrint,SIGNAL(triggered()),this,SLOT(eventPrint()));
 connect(ui->actionCapture,SIGNAL(triggered()),this,SLOT(eventCapture()));
 connect(ui->actionDeviceFwUpdate,SIGNAL(triggered()),this,SLOT(eventFirmwarUpdate()));

 connect(ui->actionHelpManual,SIGNAL(triggered()),this,SLOT(eventHelpOpenUsersGuide()));
 connect(ui->actionSupportLink,SIGNAL(triggered()),this,SLOT(eventHelpOpenSupportLink()));


 // Tab ToolBox Events
 connect(ui->bttnFrqCenterMode,SIGNAL(clicked()),this,SLOT(eventFrqModeCenterRangeChange()));
 connect(ui->bttnFrqRangeMode,SIGNAL(clicked()),this,SLOT(eventFrqModeCenterRangeChange()));
 // Tab ToolBox Marker
 connect(ui->actionMarkerAllOff,SIGNAL(triggered()),this,SLOT(eventMarkerAllOff()));
 connect(ui->actionMarkerAllOn,SIGNAL(triggered()),this,SLOT(eventMarkerAllOn()));

 connect(ui->cbMarker0TraceSelected,SIGNAL(currentIndexChanged(int)),this,SLOT(eventMarker0SetTrace(int)));
 connect(ui->cbMarker1TraceSelected,SIGNAL(currentIndexChanged(int)),this,SLOT(eventMarker1SetTrace(int)));
 connect(ui->cbMarker2TraceSelected,SIGNAL(currentIndexChanged(int)),this,SLOT(eventMarker2SetTrace(int)));

 connect(ui->actionMarkerOnOffM0,SIGNAL(toggled(bool)),ui->grpMarker0,SLOT(setChecked(bool)));
 connect(ui->actionMarkerOnOffM1,SIGNAL(toggled(bool)),ui->grpMarker1,SLOT(setChecked(bool)));
 connect(ui->actionMarkerOnOffM2,SIGNAL(toggled(bool)),ui->grpMarker2,SLOT(setChecked(bool)));

 connect(ui->bttnMarkerMoveM0,SIGNAL(clicked()),this,SLOT(eventSelectMarker0()));
 connect(ui->bttnMarkerMoveM1,SIGNAL(clicked()),this,SLOT(eventSelectMarker1()));
 connect(ui->bttnMarkerMoveM2,SIGNAL(clicked()),this,SLOT(eventSelectMarker2()));

 connect(ui->MarkerDial,SIGNAL(valueChanged(int)),this,SLOT(eventMarkerMove(int)));
 // Tab ToolBox Traces
 connect(ui->cbTraceMode0,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace0ModeChanged(int)));
 connect(ui->cbTraceMode1,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace1ModeChanged(int)));
 connect(ui->cbTraceMode2,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace2ModeChanged(int)));
 connect(ui->cbTraceMode3,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace3ModeChanged(int)));

 connect(ui->bttnTraceExport0,SIGNAL(clicked()),this,SLOT(eventTrace0Export()));
 connect(ui->bttnTraceExport1,SIGNAL(clicked()),this,SLOT(eventTrace1Export()));
 connect(ui->bttnTraceExport2,SIGNAL(clicked()),this,SLOT(eventTrace2Export()));
 connect(ui->bttnTraceExport3,SIGNAL(clicked()),this,SLOT(eventTrace3Export()));

 connect(ui->bttnTraceColor0,SIGNAL(clicked()),this,SLOT(eventTrace0ColorChange()));
 connect(ui->bttnTraceColor1,SIGNAL(clicked()),this,SLOT(eventTrace1ColorChange()));
 connect(ui->bttnTraceColor2,SIGNAL(clicked()),this,SLOT(eventTrace2ColorChange()));
 connect(ui->bttnTraceColor3,SIGNAL(clicked()),this,SLOT(eventTrace3ColorChange()));

 connect(ui->bttnTraceHold0,SIGNAL(toggled(bool)),this,SLOT(eventTrace0HoldTrigger(bool)));
 connect(ui->bttnTraceHold1,SIGNAL(toggled(bool)),this,SLOT(eventTrace1HoldTrigger(bool)));
 connect(ui->bttnTraceHold2,SIGNAL(toggled(bool)),this,SLOT(eventTrace2HoldTrigger(bool)));
 connect(ui->bttnTraceHold3,SIGNAL(toggled(bool)),this,SLOT(eventTrace3HoldTrigger(bool)));

 connect(ui->bttnTraceClear0,SIGNAL(clicked()),this,SLOT(eventTrace0Clear()));
 connect(ui->bttnTraceClear1,SIGNAL(clicked()),this,SLOT(eventTrace1Clear()));
 connect(ui->bttnTraceClear2,SIGNAL(clicked()),this,SLOT(eventTrace2Clear()));
 connect(ui->bttnTraceClear3,SIGNAL(clicked()),this,SLOT(eventTrace3Clear()));


 // Tab ToolBox Frequency Settings
 connect(ui->rbRfRange_300_348,SIGNAL(clicked()),this,SLOT(eventFrqRangeChanged()));
 connect(ui->rbRfRange_389_464,SIGNAL(clicked()),this,SLOT(eventFrqRangeChanged()));
 connect(ui->rbRfRange_779_928,SIGNAL(clicked()),this,SLOT(eventFrqRangeChanged()));

 connect(ui->spFrqCenterValue,SIGNAL(editingFinished()),this,SLOT(eventFrqCenterChanged()));
 connect(ui->spFrqSpanValue,SIGNAL(editingFinished()),this,SLOT(eventFrqSpanChanged()));
 connect(ui->spFrqStartValue,SIGNAL(editingFinished()),this,SLOT(eventFrqStartChanged()));
 connect(ui->spFrqStopValue,SIGNAL(editingFinished()),this,SLOT(eventFrqStopChanged()));

 connect(ui->bttnFrqSettingLoad,SIGNAL(clicked()),this,SLOT(eventFrqLoad()));
 connect(ui->bttnFrqSettingSave,SIGNAL(clicked()),this,SLOT(eventFrqSave()));
 connect(ui->bttnFrqSettingUndo,SIGNAL(clicked()),this,SLOT(eventFrqUndo()));

 connect(ui->bttnFrqSet,SIGNAL(clicked()),this,SLOT(eventFrqSet()));
 connect(ui->bttnFrqStop,SIGNAL(clicked()),this,SLOT(eventFrqSpectrumStop()));
 // Tab ToolBox Hardware
 connect(ui->bttnHwDeviceListScan,SIGNAL(clicked()),this,SLOT(eventUpdateDeviceList()));
 connect(ui->cbDeviceList,SIGNAL(currentIndexChanged(int)),this,SLOT(eventDeviceListIndexChanged(int)));
 connect(ui->bttnHwConnect,SIGNAL(toggled(bool)),this,SLOT(eventDeviceStartStop(bool)));

 // Tab Screen
 connect(ui->txtScreenDisplayLabel,SIGNAL(textChanged(QString)),this,SLOT(eventScreenCommentTextChanged(QString)));
 // ToolBar
 connect(actionConnect      ,SIGNAL(triggered()),this,SLOT(eventDeviceConnect())   );
 connect(actionDisconnect   ,SIGNAL(triggered()),this,SLOT(eventDeviceDisconnect()));
 connect(actionPrint        ,SIGNAL(triggered()),this,SLOT(eventCapture())         );
 connect(actionSpectrumStart,SIGNAL(triggered()),this,SLOT(eventFrqSet())          );
 connect(actionSpectrumStop ,SIGNAL(triggered()),this,SLOT(eventFrqSpectrumStop()) );

 // Others
 connect(timedateCtrl,SIGNAL(timeout()),this,SLOT(eventTimeDateUpdateTick()));
 connect(ui->actionAppExit,SIGNAL(triggered()),this,SLOT(close()));
 //SA430Driver Events
 connect(deviceCtrl,SIGNAL(signalConnected()),this,SLOT(eventSA430DeviceConnected()));
 connect(deviceCtrl,SIGNAL(signalDisconnected()),this,SLOT(eventSA430DeviceDisconnected()));
 connect(deviceCtrl,SIGNAL(signalErrorMsg(QString)),this,SLOT(eventSA430ErrorMsg(QString)));
 connect(deviceCtrl,SIGNAL(signalSpectrumReceived()),this,SLOT(eventSA430SpectrumReceived()));
 connect(deviceCtrl,SIGNAL(signalNewParameterSet(bool,int)),this,SLOT(eventSA430NewParameterSet(bool,int)));
 connect(deviceCtrl,SIGNAL(signalDeviceUpdateRequired(QString)),this,SLOT(eventSA430FirmwareUpdateRequired(QString)));

}

void MainWindow::initPlotCtrl(void)
{
 // Grid
 guiSetGridMode(GRID_DOTTED);
 // Markers
 plotCtrl->MarkerOff(M_0);
 plotCtrl->MarkerOff(M_1);
 plotCtrl->MarkerOff(M_2);
 // Traces
 plotCtrl->TraceSetMode(TRACE_0,T_MODE_OFF);
 plotCtrl->TraceSetMode(TRACE_1,T_MODE_OFF);
 plotCtrl->TraceSetMode(TRACE_2,T_MODE_OFF);
 plotCtrl->TraceSetMode(TRACE_3,T_MODE_OFF);

 plotCtrl->TraceOff(TRACE_0);
 plotCtrl->TraceOff(TRACE_1);
 plotCtrl->TraceOff(TRACE_2);
 plotCtrl->TraceOff(TRACE_3);

}

// Private Slot Defintion


// App Events
void MainWindow::eventAppClose(void)
{

}

// Driver Events
void MainWindow::eventSA430DeviceConnected(void)
{
 bool done = false;
 bool flagUsbInfoOk = false;
 bool flagDevInfoOk = false;
 bool flagCalDataOk = false;

 sa430UsbDevice   usbInfo;
 sDeviceInfo      devInfo;
 sCalibrationData calData;

 if(deviceCtrl->deviceGetUsbInfo(&usbInfo))
 {
  flagUsbInfoOk = true;
  if(deviceCtrl->deviceGetDevInfo(&devInfo))
  {
   flagDevInfoOk = true;
   if(deviceCtrl->deviceGetCalData(&calData))
   {
    flagCalDataOk = true;
    done = true;
   };
  };
 };

 if(done)
 {
  statusbarCtrl->SetDeviceConnected(&usbInfo);
  guiDisplayHwUsbInfo(&usbInfo);
  guiDisplayHwDevInfo(&devInfo);
  guiDisplayHwCalData(&calData);
  ui->tabMarker->setEnabled(true);
  ui->tabTraces->setEnabled(true);
  ui->tabRfSettings->setEnabled(true);
  actionSpectrumStart->setEnabled(true);
  actionSpectrumStop->setEnabled(true);

  if(guiFwUpdateIfRequired(&usbInfo,&devInfo))
  {// Firmware Update Successfully Done

  };
 }
 else
 {

 };
}

void MainWindow::eventSA430DeviceDisconnected(void)
{
 statusbarCtrl->SetDeviceDisconnected();
 ui->bttnHwConnect->setEnabled(true);
 ui->bttnHwConnect->setChecked(false);
 ui->bttnHwDeviceListScan->setEnabled(true);
 ui->cbDeviceList->setEnabled(true);
 guiUpdateDeviceList();
 guiDisplayHwDevInfo(NULL);

 ui->tabMarker->setEnabled(false);
 ui->tabTraces->setEnabled(false);
 ui->tabRfSettings->setEnabled(false);
 ui->tabToolBox->setCurrentIndex(0);
 actionSpectrumStart->setEnabled(false);
 actionSpectrumStop->setEnabled(false);

}

void MainWindow::eventSA430FirmwareUpdateRequired(QString ComPort)
{
 if(!ComPort.isEmpty())
 {
  guiFwUpdate(ComPort);
 };
}

void MainWindow::eventSA430NewParameterSet(bool Done, int SpecId)
{
 int Steps;
 sFrqValues        FrqCorrected;
 sCalibrationData  CalibrationData;
 if(Done)
 {
  ActiveSpecId = SpecId;
  deviceCtrl->spectrumGetParameter(&ActiveSpecParameter,&FrqCorrected);
  deviceCtrl->deviceGetCalData(&CalibrationData);
  plotCtrl->SetNewSpectrumParameter(SpecId,&FrqCorrected,&CalibrationData);
  Steps = ((FrqCorrected.FrqStop-FrqCorrected.FrqStart)/(FrqCorrected.FrqStepWidth/1000))+1;
  ui->MarkerDial->setMinimum(0);
  ui->MarkerDial->setMaximum(Steps);
  guiDisplayFrqSettings(&FrqCorrected);
 }
 else
 {
  guiDisplayFrqSettings(NULL);
  plotCtrl->SetNewSpectrumParameter(SpecId,NULL,NULL);
 };
}

void MainWindow::eventSA430SpectrumReceived(void)
{
 sSpectrum spectrum;
 sMarkerInfo minfo;
 if(deviceCtrl->spectrumGetData(&spectrum))
 {
  plotCtrl->SetSpectrumData(&spectrum);
  plotCtrl->MarkerUpdatePos();
  plotCtrl->MarkerGetInfo(&minfo);
  guiDisplayMarkerInfo(&minfo);
  if(Status.Spectrum.flagModeContinouse)
   deviceCtrl->spectrumTriggerOn();
  else
   deviceCtrl->spectrumTriggerOff();
 };
}

void MainWindow::eventSA430ErrorMsg(QString Msg)
{
 QMessageBox msgBox(this);
 msgBox.setText("Device Driver Warning!");
 msgBox.setInformativeText(Msg);
 msgBox.setStandardButtons(QMessageBox::Ok);
 msgBox.setDefaultButton(QMessageBox::Ok);
 msgBox.exec();
}


// Help Events
void MainWindow::eventHelpOpenUsersGuide(void)
{
 QUrl link;

 if(QFile::exists("SLAU371.pdf"))
 {
  link = QUrl::fromLocalFile("SLAU371.pdf");
  if(link.isValid())
  {
   QDesktopServices::openUrl(link);
  }
  else
  {// Link not valid
   // Add Handler
  };
 }
 else
 {// File not Found
  // add Handler
 };

}

void MainWindow::eventHelpOpenSupportLink(void)
{
 if(!QDesktopServices::openUrl(QUrl("http://e2e.ti.com/", QUrl::StrictMode)))
 {// Failed to Open
  // Add Handler
 };
}


// FirmwarUpdate Events
void MainWindow::eventFirmwarUpdate(void)
{
 sa430FwUpdater->exec();
}


// Frequency Settings
void MainWindow::eventFrqStartChanged(void)
{
 if(ui->spFrqStartValue->value()>ui->spFrqStopValue->value())
 {
  ui->spFrqStartValue->setValue(ui->spFrqStopValue->value());
 };
}

void MainWindow::eventFrqStopChanged(void)
{
 if(ui->spFrqStopValue->value()<ui->spFrqStartValue->value())
 {
  ui->spFrqStopValue->setValue(ui->spFrqStartValue->value());
 };
}

void MainWindow::eventFrqCenterChanged(void)
{
}

void MainWindow::eventFrqSpanChanged(void)
{

}


// Date& Timer Events
void MainWindow::eventTimeDateUpdateTick(void)
{
 if(ui->cbDisplayTimeDateOn->isChecked())
 {
  ui->txtGraphDate->setText(QDateTime::currentDateTime().toString());
 }
 else
 {
  ui->txtGraphDate->setText("            ");
 };
}

// Hardware Events
void MainWindow::eventUpdateDeviceList(void)
{
 if(ui->cbDeviceList->count()>0)
  ui->cbDeviceList->clear();
 guiUpdateDeviceList();
}

void MainWindow::eventDeviceListIndexChanged(int index)
{
 sa430UsbDevice Info;
 if(index>=0)
 {
  Info = DeviceList.at(index);
  if(Info.Connected)
  {
   ui->bttnHwConnect->setEnabled(true);
  }
  else
  {
   ui->bttnHwConnect->setEnabled(false);
  };
  guiDisplayHwUsbInfo(&Info);
 }
 else
 {
  ui->bttnHwConnect->setEnabled(false);
 };
}

void MainWindow::eventDeviceStartStop(bool On)
{
 if(On)
 {// Event: Connect
  if(DeviceList.isEmpty())
  {
   QMessageBox::information(this,"SA430 Device Driver","No Device Found/Detected... Scan again for connected Devices!",QMessageBox::Ok);
  }
  else
  {
   sa430UsbDevice devinfo = DeviceList.at(ui->cbDeviceList->currentIndex());
   if(deviceCtrl->deviceConnect(&devinfo))
   {
    ui->bttnHwDeviceListScan->setEnabled(false);
    actionConnect->setEnabled(false);
    actionDisconnect->setEnabled(true);
    ui->cbDeviceList->setEnabled(false);
   };
  };
 }
 else
 {// Event: Disconnect
  deviceCtrl->deviceDisconnect();
  ui->bttnHwDeviceListScan->setEnabled(true);
  ui->bttnHwConnect->setChecked(false);
  actionConnect->setEnabled(true);
  actionDisconnect->setEnabled(false);
  ui->cbDeviceList->setEnabled(true);
 };
}

void MainWindow::eventDeviceConnect(void)
{
 ui->bttnHwConnect->setChecked(true);
}

void MainWindow::eventDeviceDisconnect(void)
{
 ui->bttnHwConnect->setChecked(false);
}


void MainWindow::guiShowDeviceInfo(sa430UsbDevice *device)
{
 if(!device)
 {

 };
}


// Marker event
void MainWindow::eventGridMode_Off(void)
{
 guiSetGridMode(GRID_OFF);
}

void MainWindow::eventGridMode_Lines(void)
{
 guiSetGridMode(GRID_LINES);
}

void MainWindow::eventGridMode_Dotted(void)
{
 guiSetGridMode(GRID_DOTTED);
}

void MainWindow::eventFrqModeCenterRangeChange(void)
{
 if(ui->swFrqModeCenterRange->currentIndex()==0)
 {//Mode FSpan/2<-FCenter->FSpan/2
  ui->swFrqModeCenterRange->setCurrentIndex(1);
  ui->bttnFrqRangeMode->setEnabled(true);
  ui->bttnFrqCenterMode->setEnabled(false);
  guiFrqStartStop2CenterSpan();
 }
 else
 {// Mode FStart<->FStop
  ui->swFrqModeCenterRange->setCurrentIndex(0);
  ui->bttnFrqRangeMode->setEnabled(false);
  ui->bttnFrqCenterMode->setEnabled(true);
  guiFrqCenterSpan2StartStop();
 };
}
// Marker Event Function Defintion
void MainWindow::eventMarker0OnOff(bool flagOn)
{
 if(flagOn)
 {
  guiMarkerSetTrace(M_0,(eTrace)ui->cbMarker0TraceSelected->currentIndex());
 }
 else
 {
  guiMarkerSetTrace(M_0,TRACE_OFF);
 };
}

void MainWindow::eventMarker1OnOff(bool flagOn)
{
 if(flagOn)
 {
  guiMarkerSetTrace(M_1,(eTrace)ui->cbMarker1TraceSelected->currentIndex());
 }
 else
 {
  guiMarkerSetTrace(M_1,TRACE_OFF);
 };
}

void MainWindow::eventMarker2OnOff(bool flagOn)
{
 if(flagOn)
 {
  guiMarkerSetTrace(M_2,(eTrace)ui->cbMarker2TraceSelected->currentIndex());
 }
 else
 {
  guiMarkerSetTrace(M_2,TRACE_OFF);
 };
}
void MainWindow::eventMarkerPeakOnOff(bool flagOn)
{
 //guiMarkerOnOff(M_PEAK,flagOn);
}

void MainWindow::eventMarker0SetTrace(int index)
{
 guiMarkerSetTrace(M_0,(eTrace)index);
}

void MainWindow::eventMarker1SetTrace(int index)
{
 guiMarkerSetTrace(M_1,(eTrace)index);
}

void MainWindow::eventMarker2SetTrace(int index)
{
 guiMarkerSetTrace(M_2,(eTrace)index);
}


void MainWindow::eventMarkerAllOn(void)
{
 ui->actionMarkerOnOffM0->setChecked(true);
 ui->actionMarkerOnOffM1->setChecked(true);
 ui->actionMarkerOnOffM2->setChecked(true);
}

void MainWindow::eventMarkerAllOff(void)
{
 ui->actionMarkerOnOffM0->setChecked(false);
 ui->actionMarkerOnOffM1->setChecked(false);
 ui->actionMarkerOnOffM2->setChecked(false);
}


void MainWindow::eventMarkerMove(int index)
{
 sMarkerInfo minfo;
 if(plotCtrl->MarkerSetActivePos(index) == index)
 {
  plotCtrl->MarkerGetInfo(&minfo);
  guiDisplayMarkerInfo(&minfo);
 };
}

void MainWindow::eventSelectMarker0(void)
{
 guiMarkerSelect(M_0);
}

void MainWindow::eventSelectMarker1(void)
{
 guiMarkerSelect(M_1);
}

void MainWindow::eventSelectMarker2(void)
{
 guiMarkerSelect(M_2);
}

void MainWindow::guiMarkerSelect(eMarker Marker)
{
 plotCtrl->MarkerSetActive(Marker);
 ui->MarkerDial->setValue(plotCtrl->MarkerGetPos(Marker));
 switch(Marker)
 {
  case M_0:
     ui->bttnMarkerMoveM0->setChecked(true );
     ui->bttnMarkerMoveM1->setChecked(false);
     ui->bttnMarkerMoveM2->setChecked(false);
    break;
  case M_1:
     ui->bttnMarkerMoveM0->setChecked(false);
     ui->bttnMarkerMoveM1->setChecked(true );
     ui->bttnMarkerMoveM2->setChecked(false);
    break;
  case M_2:
     ui->bttnMarkerMoveM0->setChecked(false);
     ui->bttnMarkerMoveM1->setChecked(false);
     ui->bttnMarkerMoveM2->setChecked(true );
    break;
  case M_OFF:
  case M_PEAK:
  case M_ALL:
     break;
   default:
     break;
 };

}

bool MainWindow::guiMarkerSetTrace(eMarker Marker, eTrace TraceNr)
{
 bool done = false;

 if(plotCtrl->MarkerSetTrace(Marker,TraceNr))
 {
  switch(Marker)
  {
   case M_0:
      ui->bttnMarkerMoveM0->setEnabled(true);
      if(ui->cbMarker1TraceSelected->currentIndex()!=M_OFF)
      {
       ui->rbMarker0MakeDeltaM1->setChecked(false);
       ui->rbMarker0MakeDeltaM1->setEnabled(true);
      };
     break;
   case M_1:
      ui->bttnMarkerMoveM1->setEnabled(true);
      if(ui->cbMarker0TraceSelected->currentIndex()!=M_OFF)
      {
       ui->rbMarker0MakeDeltaM1->setChecked(false);
       ui->rbMarker0MakeDeltaM1->setEnabled(true);
      };
      if(ui->cbMarker2TraceSelected->currentIndex()!=M_OFF)
      {
       ui->rbMarker2MakeDeltaM1->setChecked(false);
       ui->rbMarker2MakeDeltaM1->setEnabled(true);
      };

     break;
   case M_2:
      ui->bttnMarkerMoveM2->setEnabled(true);
      if(ui->cbMarker1TraceSelected->currentIndex()!=M_OFF)
      {
       ui->rbMarker2MakeDeltaM1->setChecked(false);
       ui->rbMarker2MakeDeltaM1->setEnabled(true);
      };
     break;
   case M_OFF:
   case M_PEAK:
   case M_ALL:
      break;
    default:
      break;
  };
  done = true;
 }
 else
 {
  switch(Marker)
  {
   case M_0:
       ui->cbMarker0TraceSelected->setCurrentIndex(M_OFF);
       ui->bttnMarkerMoveM0->setChecked(false);
       ui->bttnMarkerMoveM0->setEnabled(false);
       ui->rbMarker0MakeDeltaM1->setChecked(false);
       ui->rbMarker0MakeDeltaM1->setEnabled(true);
      break;
   case M_1:
       ui->cbMarker1TraceSelected->setCurrentIndex(M_OFF);
       ui->bttnMarkerMoveM1->setChecked(false);
       ui->bttnMarkerMoveM1->setEnabled(false);
       ui->rbMarker0MakeDeltaM1->setChecked(false);
       ui->rbMarker0MakeDeltaM1->setEnabled(false);
       ui->rbMarker2MakeDeltaM1->setChecked(false);
       ui->rbMarker2MakeDeltaM1->setEnabled(false);
      break;
   case M_2:
       ui->cbMarker2TraceSelected->setCurrentIndex(M_OFF);
       ui->bttnMarkerMoveM2->setChecked(false);
       ui->bttnMarkerMoveM2->setEnabled(false);
       ui->rbMarker2MakeDeltaM1->setChecked(false);
       ui->rbMarker2MakeDeltaM1->setEnabled(true);
      break;
  case M_OFF:
  case M_PEAK:
  case M_ALL:
     break;
   default:
     break;
  };
  done = false;
 };

 return(done);
}

void MainWindow::guiMarkerSetTraceUpdate(eTrace TraceNr)
{
 //Marker 0
 if(ui->cbMarker0TraceSelected->currentIndex() == TraceNr)
 {
  guiMarkerSetTrace(M_0,TraceNr);
 };
 //Marker 1
 if(ui->cbMarker1TraceSelected->currentIndex() == TraceNr)
 {
  guiMarkerSetTrace(M_1,TraceNr);
 };
 //Marker 1
 if(ui->cbMarker2TraceSelected->currentIndex() == TraceNr)
 {
  guiMarkerSetTrace(M_2,TraceNr);
 };
}


void MainWindow::guiTraceColorSelect(eTrace TraceNr)
{
 QColor color;

 color = QColorDialog::getColor(plotCtrl->TraceGetColor(TraceNr), this,"Select Trace Color");
 if(color.isValid())
 {
  guiTraceColorSet(TraceNr,color);
 };
}

void MainWindow::guiTraceColorSet(eTrace TraceNr, QColor newColor)
{
 QPixmap pm(32,14);
 QString txtStyleSheet;

 plotCtrl->TraceSetColor(TraceNr,newColor);
 pm.fill(newColor);
 txtStyleSheet = QString("background-color: rgb(0, 0, 0);\ncolor: rgb(%0 ,%1 ,%2);")
                    .arg(newColor.toRgb().red()  )
                    .arg(newColor.toRgb().green())
                    .arg(newColor.toRgb().blue() );
 switch(TraceNr)
 {
  case TRACE_0:
     ui->bttnTraceColor0->setIcon(QIcon(pm));
     ui->txtTrace0_Label->setStyleSheet(txtStyleSheet);
     ui->txtTrace0_Mode->setStyleSheet(txtStyleSheet);
    break;
  case TRACE_1:
     ui->bttnTraceColor1->setIcon(QIcon(pm));
     ui->txtTrace1_Label->setStyleSheet(txtStyleSheet);
     ui->txtTrace1_Mode->setStyleSheet(txtStyleSheet);
    break;
  case TRACE_2:
     ui->bttnTraceColor2->setIcon(QIcon(pm));
     ui->txtTrace2_Label->setStyleSheet(txtStyleSheet);
     ui->txtTrace2_Mode->setStyleSheet(txtStyleSheet);
    break;
  case TRACE_3:
     ui->bttnTraceColor3->setIcon(QIcon(pm));
     ui->txtTrace3_Label->setStyleSheet(txtStyleSheet);
     ui->txtTrace3_Mode->setStyleSheet(txtStyleSheet);
    break;
  case TRACE_UNDEFINED:
     break;

    default:
        break;
 };
}


void MainWindow::guiTraceModeChange(eTrace TraceNr, eTraceMode trcMode)
{
 bool flagClrWrite = false;
 QString txtMode;
 switch(trcMode)
 {
  case T_MODE_UNDEFINED:
  case T_MODE_OFF:
        // Switch Trace enabled Marker
        if(ui->cbMarker0TraceSelected->currentIndex() == TraceNr)
         guiMarkerSetTrace(M_0,TRACE_OFF);
        if(ui->cbMarker1TraceSelected->currentIndex() == TraceNr)
         guiMarkerSetTrace(M_1,TRACE_OFF);
        if(ui->cbMarker2TraceSelected->currentIndex() == TraceNr)
         guiMarkerSetTrace(M_2,TRACE_OFF);
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        // Switch Off continouse Spec Trigger if not required
        if(
              ui->cbTraceMode0->currentIndex() == T_MODE_OFF
           && ui->cbTraceMode1->currentIndex() == T_MODE_OFF
           && ui->cbTraceMode2->currentIndex() == T_MODE_OFF
           && ui->cbTraceMode3->currentIndex() == T_MODE_OFF
          )
        {
         Status.Spectrum.flagModeContinouse = false;
        };
        txtMode.append("OFF");

       break;
  case T_MODE_CLEARWRITE:
        flagClrWrite = true;
        txtMode.append("ACT");
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        guiMarkerSetTraceUpdate(TraceNr);
       break;
  case T_MODE_MAXHOLD:
        txtMode.append("MAX");
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        guiMarkerSetTraceUpdate(TraceNr);
       break;
  case T_MODE_AVERAGE:
        txtMode.append("AVG");
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        guiMarkerSetTraceUpdate(TraceNr);
       break;

    default:
        break;
 };


 if(trcMode==T_MODE_OFF)
 {
  switch(TraceNr)
  {
   case TRACE_0:
     ui->bttnTraceClear0->setDisabled(true);
     ui->bttnTraceColor0->setDisabled(true);
     ui->bttnTraceExport0->setDisabled(true);
     ui->txtTrace0_Label->hide();
     ui->txtTrace0_Mode->hide();
     ui->bttnTraceHold0->setEnabled(false);
    break;
   case TRACE_1:
     ui->bttnTraceClear1->setDisabled(true);
     ui->bttnTraceColor1->setDisabled(true);
     ui->bttnTraceExport1->setDisabled(true);
     ui->txtTrace1_Label->hide();
     ui->txtTrace1_Mode->hide();
     ui->bttnTraceHold1->setEnabled(false);
    break;
   case TRACE_2:
     ui->bttnTraceClear2->setDisabled(true);
     ui->bttnTraceColor2->setDisabled(true);
     ui->bttnTraceExport2->setDisabled(true);
     ui->txtTrace2_Label->hide();
     ui->txtTrace2_Mode->hide();
     ui->bttnTraceHold2->setEnabled(false);
    break;
   case TRACE_3:
     ui->bttnTraceClear3->setDisabled(true);
     ui->bttnTraceColor3->setDisabled(true);
     ui->bttnTraceExport3->setDisabled(true);
     ui->txtTrace3_Label->hide();
     ui->txtTrace3_Mode->hide();
     ui->bttnTraceHold3->setEnabled(false);

    break;
      default:
        break;
  };
 }
 else
 {
  switch(TraceNr)
  {
   case TRACE_0:
    if(flagClrWrite)
     ui->bttnTraceClear0->setDisabled(true);
    else
     ui->bttnTraceClear0->setDisabled(false);
    ui->bttnTraceColor0->setDisabled(false);
    ui->bttnTraceExport0->setDisabled(false);
    ui->txtTrace0_Mode->setText(txtMode);
    ui->txtTrace0_Label->show();
    ui->txtTrace0_Mode->show();
    ui->bttnTraceHold0->setEnabled(true);
    break;
   case TRACE_1:
     if(flagClrWrite)
      ui->bttnTraceClear1->setDisabled(true);
     else
       ui->bttnTraceClear1->setDisabled(false);
     ui->bttnTraceClear1->setDisabled(false);
     ui->bttnTraceColor1->setDisabled(false);
     ui->bttnTraceExport1->setDisabled(false);
     ui->txtTrace1_Mode->setText(txtMode);
     ui->txtTrace1_Label->show();
     ui->txtTrace1_Mode->show();
     ui->bttnTraceHold1->setEnabled(true);
    break;
   case TRACE_2:
      if(flagClrWrite)
       ui->bttnTraceClear2->setDisabled(true);
      else
       ui->bttnTraceClear2->setDisabled(false);
      ui->bttnTraceClear2->setDisabled(false);
      ui->bttnTraceColor2->setDisabled(false);
      ui->bttnTraceExport2->setDisabled(false);
      ui->txtTrace2_Mode->setText(txtMode);
      ui->txtTrace2_Label->show();
      ui->txtTrace2_Mode->show();
      ui->bttnTraceHold2->setEnabled(true);
     break;
   case TRACE_3:
      if(flagClrWrite)
       ui->bttnTraceClear3->setDisabled(true);
      else
       ui->bttnTraceClear3->setDisabled(false);
     ui->bttnTraceClear3->setDisabled(false);
     ui->bttnTraceColor3->setDisabled(false);
     ui->bttnTraceExport3->setDisabled(false);
     ui->txtTrace3_Mode->setText(txtMode);
     ui->txtTrace3_Label->show();
     ui->txtTrace3_Mode->show();
     ui->bttnTraceHold3->setEnabled(true);
    break;
     default:
       break;
  };
 };

}

// Trace Event Function Defintion
void MainWindow::eventTrace0ModeChanged(int mode)
{
 if(ui->bttnTraceHold0->isChecked())
 {
  ui->bttnTraceHold0->setChecked(false);
 };
 guiTraceModeChange(TRACE_0,(eTraceMode)mode);
}

void MainWindow::eventTrace1ModeChanged(int mode)
{
 if(ui->bttnTraceHold1->isChecked())
 {
  ui->bttnTraceHold1->setChecked(false);
 };
 guiTraceModeChange(TRACE_1,(eTraceMode)mode);
}
void MainWindow::eventTrace2ModeChanged(int mode)
{
 if(ui->bttnTraceHold2->isChecked())
 {
  ui->bttnTraceHold2->setChecked(false);
 };
 guiTraceModeChange(TRACE_2,(eTraceMode)mode);
}

void MainWindow::eventTrace3ModeChanged(int mode)
{
 if(ui->bttnTraceHold3->isChecked())
 {
  ui->bttnTraceHold3->setChecked(false);
 };
 guiTraceModeChange(TRACE_3,(eTraceMode) mode);
}

void MainWindow::eventTrace0ColorChange(void)
{
 guiTraceColorSelect(TRACE_0);
}

void MainWindow::eventTrace1ColorChange(void)
{
 guiTraceColorSelect(TRACE_1);
}

void MainWindow::eventTrace2ColorChange(void)
{
 guiTraceColorSelect(TRACE_2);
}

void MainWindow::eventTrace3ColorChange(void)
{
 guiTraceColorSelect(TRACE_3);
}

void MainWindow::eventTrace0Export(void)
{
 guiTraceExport(TRACE_0,(eTraceMode) ui->cbTraceMode0->currentIndex());
}

void MainWindow::eventTrace1Export(void)
{
 guiTraceExport(TRACE_1,(eTraceMode) ui->cbTraceMode1->currentIndex());
}

void MainWindow::eventTrace2Export(void)
{
 guiTraceExport(TRACE_2,(eTraceMode) ui->cbTraceMode2->currentIndex());
}

void MainWindow::eventTrace3Export(void)
{
 guiTraceExport(TRACE_3,(eTraceMode) ui->cbTraceMode3->currentIndex());
}


void MainWindow::eventTrace0HoldTrigger(bool On)
{
 if(On)
 {
  guiTraceHoldTrigger(TRACE_0);
 }
 else
 {
  guiTraceHoldReset(TRACE_0);
 };
}

void MainWindow::eventTrace1HoldTrigger(bool On)
{
 if(On)
 {
  guiTraceHoldTrigger(TRACE_1);
 }
 else
 {
  guiTraceHoldReset(TRACE_1);
 };
}

void MainWindow::eventTrace2HoldTrigger(bool On)
{
 if(On)
 {
  guiTraceHoldTrigger(TRACE_2);
 }
 else
 {
  guiTraceHoldReset(TRACE_2);
 };
}

void MainWindow::eventTrace3HoldTrigger(bool On)
{
 if(On)
 {
  guiTraceHoldTrigger(TRACE_3);
 }
 else
 {
  guiTraceHoldReset(TRACE_3);
 };
}

void MainWindow::eventTrace0Clear(void)
{
 guiTraceClear(TRACE_0);
}

void MainWindow::eventTrace1Clear(void)
{
 guiTraceClear(TRACE_1);
}

void MainWindow::eventTrace2Clear(void)
{
 guiTraceClear(TRACE_2);
}

void MainWindow::eventTrace3Clear(void)
{
 guiTraceClear(TRACE_3);
}

// Frequency Settings Events
void MainWindow::eventFrqRangeChanged(void)
{
 if(ui->rbRfRange_300_348->isChecked())
 {
  // Frq Start<->Start Limit
  ui->spFrqStartValue->setMinimum(300);
  ui->spFrqStartValue->setMaximum(348);
  ui->spFrqStopValue->setMinimum(300);
  ui->spFrqStopValue->setMaximum(348);
  // Frq Span/2<-FCenter->Span/2 Limit
  ui->spFrqCenterValue->setMinimum(300);
  ui->spFrqCenterValue->setMaximum(348);
  ui->spFrqSpanValue->setMinimum(0.1);
  ui->spFrqSpanValue->setMaximum(48);
 }
 else
 if(ui->rbRfRange_389_464->isChecked())
 {
  // Frq Start<->Start Limit
  ui->spFrqStartValue->setMinimum(389);
  ui->spFrqStartValue->setMaximum(464);
  ui->spFrqStopValue->setMinimum(389);
  ui->spFrqStopValue->setMaximum(464);
  // Frq Span/2<-FCenter->Span/2 Limit
  ui->spFrqCenterValue->setMinimum(389);
  ui->spFrqCenterValue->setMaximum(464);
  ui->spFrqSpanValue->setMinimum(0.1);
  ui->spFrqSpanValue->setMaximum(75);
 }
 else
 if(ui->rbRfRange_779_928->isChecked())
 {
  // Frq Start<->Start Limit
  ui->spFrqStartValue->setMinimum(779);
  ui->spFrqStartValue->setMaximum(928);
  ui->spFrqStopValue->setMinimum(779);
  ui->spFrqStopValue->setMaximum(928);
  // Frq Span/2<-FCenter->Span/2 Limit
  ui->spFrqCenterValue->setMinimum(779);
  ui->spFrqCenterValue->setMaximum(928);
  ui->spFrqSpanValue->setMinimum(0.1);
  ui->spFrqSpanValue->setMaximum(74.5);
 };
}

void MainWindow::eventFrqSet(void)
{
 bool done = false;
 sFrqValues newFrqValues;

 if(guiFrqGetActualSettings(&newFrqValues))
 {
  guiFrqCalcEasyRfSettings(&newFrqValues);
  if(deviceCtrl->spectrumSetParameter(&newFrqValues))
  {
   // Switch off Trace Hold
   if(ui->bttnTraceHold0->isChecked())
     ui->bttnTraceHold0->setChecked(false);
   if(ui->bttnTraceHold1->isChecked())
     ui->bttnTraceHold1->setChecked(false);
   if(ui->bttnTraceHold2->isChecked())
     ui->bttnTraceHold2->setChecked(false);
   if(ui->bttnTraceHold3->isChecked())
     ui->bttnTraceHold3->setChecked(false);
   // If no Traces are active switch On Trc0->ClrWrite and Trc1->MaxHold
   if(
         ui->cbTraceMode0->currentIndex()==T_MODE_OFF
      && ui->cbTraceMode1->currentIndex()==T_MODE_OFF
      && ui->cbTraceMode2->currentIndex()==T_MODE_OFF
      )
   {
    ui->cbTraceMode0->setCurrentIndex(T_MODE_CLEARWRITE);
    ui->cbTraceMode1->setCurrentIndex(T_MODE_MAXHOLD);
   };
   // Update Status Info
   // Status Continous Mode Flag
   Status.Spectrum.flagModeContinouse = newFrqValues.flagModeContinous;
   // Statuc Save Active Frq Settings
   if(Status.Spectrum.flagActiveFrqValues == true)
   {// Save Undo Frq Settings
    Status.Spectrum.undoFrqValues     = Status.Spectrum.activFrqValues;
    Status.Spectrum.flagUndoFrqValues = true;
   };
   // Save New and Active frq Settings
   Status.Spectrum.flagActiveFrqValues = true;
   Status.Spectrum.activFrqValues      = newFrqValues;
   done = true;
  };
 };
}


void MainWindow::eventFrqSpectrumStop(void)
{
 Status.Spectrum.flagModeContinouse = false;
}

void MainWindow::eventFrqSave(void)
{
 bool done = false;
 QString     strFileName;
 sFrqSetting FrqSettings;

 strFileName = QFileDialog::getSaveFileName(this,tr("Save Frq Profile"),QDir::currentPath(), tr("Frq Profile Files (*.xml)"));

 if(strFileName.isEmpty())
   return;

 FrqSettings.Name = strFileName;

 if(guiFrqGetActualSettings(&FrqSettings.Values))
 {
  if(Settings->SaveFrqSettings(&FrqSettings))
  {
   done = true;
  };
 };
}

void MainWindow::eventFrqLoad(void)
{
 QString FileName;
 sFrqSetting newFrqSettings;
 FileName = QFileDialog::getOpenFileName(this,tr("Load Frq Profile"),QDir::currentPath(), tr("Frq Profile Files (*.xml)"));


 if(!FileName.isEmpty())
 {
  if(Settings->LoadFrqSettings(FileName,&newFrqSettings))
  {
   guiLoadFrqProfile(&newFrqSettings);
  };
 };
}

void MainWindow::eventFrqUndo(void)
{
 QUrl link;

 if(QFile::exists("SLAU371_MSP-SA430-SUB1GHZ.pdf"))
 {
  link = QUrl::fromLocalFile("SLAU371_MSP-SA430-SUB1GHZ.pdf");
  if(link.isValid())
  {
   QDesktopServices::openUrl(link);
  }
  else
  {// Link not valid
   // Add Handler
  };
 }
 else
 {// File not Found
  // add Handler
 };

 //QDesktopServices::openUrl(QUrl("http://www.bm-innovations.com", QUrl::TolerantMode));

 if(Status.Spectrum.flagUndoFrqValues)
 {

 };
}

// Div. Tool Event Function Defintion


void MainWindow::eventScreenCommentTextChanged(QString Text)
{
 plotCtrl->SetTitel(&Text);
}


void MainWindow::eventExport(void)
{
 plotCtrl->Export();
}

void MainWindow::eventPrint(void)
{
 plotCtrl->Print();
}

void MainWindow::eventCapture(void)
{
// QProgressDialog progress("Capture Application Window ...","Test", 0, 0, this);
 QString format = "png";
 QString initialPath = QDir::currentPath() + "/untitled." + format;
 QPixmap capture;

 capture = QPixmap::grabWidget(this);
 QString fileName = QFileDialog::getSaveFileName(this, "Save As",initialPath,"PNG Files (*.png);;All Files (*)");
if(!fileName.isEmpty())
 if(!capture.save(fileName,format.toAscii()))
  {
   fileName=fileName;
  };

}


// Private Funtion Defintion


void MainWindow::guiUpdateDeviceList(void)
{
 sa430UsbDevice deviceItem;
 unsigned short NrOfDevices=0;

 // Search Number Of Connected Devices
 ui->cbDeviceList->clear();
 DeviceList.clear();
 if(sa430FindDevices(NrOfDevices))
 {
  for(unsigned short index=0;index<NrOfDevices;index++)
  {// Get for each device the Device Details
   if(sa430GetDeviceInfo(index,&deviceItem))
   {// Add the information to the device list
    DeviceList.append(deviceItem);
   };
  };
  // Update Hardware device List field
  foreach(sa430UsbDevice item,DeviceList)
  {
   if(!item.Connected)
    {
     ui->cbDeviceList->addItem(QString("%0 - Not Connected").arg(item.PortName));
    }
   else
    {
     ui->cbDeviceList->addItem(QString("%0 - Connected").arg(item.PortName));
    };
  };
 };
 // Set index to first connected device
 for(int index=0;index<DeviceList.count();index++)
 {
  if(DeviceList.at(index).Connected)
   ui->cbDeviceList->setCurrentIndex(index);
 };
}

void MainWindow::guiSetGridMode(eGridMode mode)
{
  switch(mode)
  {
   case GRID_OFF:
         if(!ui->rbGridModeOff->isChecked())
         {
          ui->rbGridModeOff->setChecked(true);
          ui->rbGridModeLine->setChecked(false);
          ui->rbGridModeDotted->setChecked(false);
         };
         ui->actionGridOff->setChecked(true);
         ui->actionGridDotted->setChecked(false);
         ui->actionGridLines->setChecked(false);
         plotCtrl->GridOff();
        break;
   case GRID_LINES:
         if(!ui->rbGridModeLine->isChecked())
         {
          ui->rbGridModeOff->setChecked(false);
          ui->rbGridModeLine->setChecked(true);
          ui->rbGridModeDotted->setChecked(false);
         };
         ui->actionGridOff->setChecked(false);
         ui->actionGridDotted->setChecked(false);
         ui->actionGridLines->setChecked(true);
         plotCtrl->GridLines();
        break;
   case GRID_DOTTED:
         if(!ui->rbGridModeDotted->isChecked())
         {
          ui->rbGridModeOff->setChecked(false);
          ui->rbGridModeLine->setChecked(false);
          ui->rbGridModeDotted->setChecked(true);
         };
         ui->actionGridOff->setChecked(false);
         ui->actionGridDotted->setChecked(true);
         ui->actionGridLines->setChecked(false);
         plotCtrl->GridDotted();
        break;
    default:
       break;
  }
}

void MainWindow::guiSetTraceMode(eTrace trace, eTraceMode mode)
{
 trace = trace;
 mode = mode;
}

bool MainWindow::guiSaveFrqProfile(sFrqSetting &tmpFrqSet)
{

 return(true);
}

bool MainWindow::guiLoadFrqProfile(sFrqSetting *newFrqSetting)
{
 bool done = false;

 if(newFrqSetting)
 {
  // Frq Expert Mode
  ui->bttnRfModeEasyRf->setChecked(newFrqSetting->Values.flagModeEasyRf);
  // Frq CenterSpan/StartStop Mode
  if(newFrqSetting->Values.flagModeStartStop)
  {// Mode StartStop
   ui->bttnFrqCenterMode->setEnabled(true);
   ui->bttnFrqRangeMode->setEnabled(false);
   // Frq ModeCenterSpan / StartStop Tab
   ui->swFrqModeCenterRange->setCurrentIndex(0);
  }
  else
  {// Mode CenterSpan
   ui->bttnFrqCenterMode->setEnabled(true);
   ui->bttnFrqRangeMode->setEnabled(false);
   // Frq ModeCenterSpan / StartStop Tab
   ui->swFrqModeCenterRange->setCurrentIndex(1);
  };
  // Frq FrqRange
  switch(newFrqSetting->Values.FrqRange)
  {
   case FRQRANGE_300_348:
       ui->rbRfRange_300_348->setChecked(true);
      break;
   case FRQRANGE_389_464:
       ui->rbRfRange_389_464->setChecked(true);
      break;
   case FRQRANGE_779_928:
      ui->rbRfRange_779_928->setChecked(true);
      break;
   case FRQRANGE_UNDEFINED:
      break;
   default:
      break;
  };
  eventFrqRangeChanged();
  // Frq Values Start<->Stop
  ui->spFrqStartValue->setValue(newFrqSetting->Values.FrqStart);
  ui->spFrqStopValue->setValue( newFrqSetting->Values.FrqStop );
  // Frq Values Span<-Center->Span
  ui->spFrqCenterValue->setValue(newFrqSetting->Values.FrqCenter);
  ui->spFrqSpanValue->setValue(  newFrqSetting->Values.FrqSpan  );
  // Frq Single Continous Mode
  ui->rbSweepModeContinous->setChecked(newFrqSetting->Values.flagModeContinous);
  // Frq StepWidth
  ui->spFrqStepwidthValue->setValue(newFrqSetting->Values.FrqStepWidth);
  // Frq DcLevel Index
  ui->cbRefDcLevelValue->setCurrentIndex(newFrqSetting->Values.RefDcLevelIndex);
  // Frq RBW Index
  ui->cbResolutionBandWidthValue->setCurrentIndex(newFrqSetting->Values.RBWIndex);

  done = true;
 };

 return(done);
}

bool MainWindow::guiDisplayHwUsbInfo(sa430UsbDevice *usbInfo)
{
 bool done = false;
    strHwUsbInfoList.clear();
 if(usbInfo)
 {
  strHwUsbInfoList.append(QString("Connected:"));
  if(usbInfo->Connected)
   strHwUsbInfoList.append(QString("   Yes"));
  else
   strHwUsbInfoList.append(QString("   No "));
  strHwUsbInfoList.append(QString("USBSerNr:"));
  strHwUsbInfoList.append(QString("   %0").arg(usbInfo->SerialNr));
  strHwUsbInfoList.append(QString("USBDescr:"));
  strHwUsbInfoList.append(QString("   %0").arg(usbInfo->DevDesc));
  strHwUsbInfoList.append(QString("ComPort :"));
  strHwUsbInfoList.append(QString("   %0").arg(usbInfo->PortName));
  done = true;
 };

 mHwUsbInfo->setStringList(strHwUsbInfoList);

 return(done);
}

bool MainWindow::guiDisplayHwDevInfo(sDeviceInfo *devInfo)
{
 strHwDevInfoList.clear();
 QStringList  listIDNInfo;
 QString      idnText;
 if(devInfo)
 {
  listIDNInfo = devInfo->IDN.split(",");
  strHwDevInfoList.append(QString("Product :"));
  strHwDevInfoList.append(QString("  SerNr."));
  strHwDevInfoList.append(QString("    %0").arg(devInfo->SerialNumber));
  strHwDevInfoList.append(QString("  Core Version"));
  strHwDevInfoList.append(QString("     %0.%1").arg((unsigned char) (devInfo->CoreVersion>>8),0,16).arg((unsigned char)(devInfo->CoreVersion&0xff)));
  strHwDevInfoList.append(QString("  Spec Version"));
  strHwDevInfoList.append(QString("     %0.%1").arg((unsigned char) (devInfo->SpecVersion>>8),0,16).arg((unsigned char)(devInfo->SpecVersion&0xff)));
  strHwDevInfoList.append(QString("  IDN Info"));
  for(int index=0;index<listIDNInfo.count();index++)
  {
   idnText = listIDNInfo.at(index);
   if(idnText.size()>24)
   {
    idnText.resize(21);
    idnText.append("...");
   };
   strHwDevInfoList.append(QString("     %0").arg(idnText));
  };
  ui->grpHwDevInfo->setVisible(true);
 }
 else
 {
  ui->grpHwDevInfo->setVisible(false);
 };

 mHwDevInfo->setStringList(strHwDevInfoList);

 return(true);
}

bool MainWindow::guiDisplayHwCalData(sCalibrationData *calData)
{

 strHwDevInfoList.append(QString("Calibration :"));

 if(calData)
 {
  // Status
  strHwDevInfoList.append(QString("  Status"));
  strHwDevInfoList.append(QString("     Loaded"));
  // Date
  strHwDevInfoList.append(QString("  Date"));
  calData->CalDate[15] = 0;
  strHwDevInfoList.append(QString("     %0").arg(QString(&calData->CalDate[0])));
  // Format Version
  strHwDevInfoList.append(QString("  Format Version"));
  strHwDevInfoList.append(QString("     V%0.%1").arg((unsigned short)calData->CalFormatVer>>8,0,16).arg((unsigned short)calData->CalFormatVer & 0xff ,0,16));

  ui->grpHwDevInfo->setVisible(true);
 }
 else
 {
  ui->grpHwDevInfo->setVisible(false);
 };

 mHwDevInfo->setStringList(strHwDevInfoList);

 return(true);
}



bool MainWindow::guiDisplayFrqSettings(sFrqValues *FrqSettings)
{
 bool done = false;
 if(FrqSettings)
 {
  if(ui->bttnFrqRangeMode)
  {//FStartStop
   // Label
   ui->txtFrqStartCenter_Label->setText(QString("Start"));
   ui->txtFrqStopSpan_Label->setText(QString("Stop"));
   // Value
   ui->txtFrqStartCenter_Value->setText(s32ToString((long)(FrqSettings->FrqStart*1000000)));
   ui->txtFrqStopSpan_Value->setText(s32ToString((long)(FrqSettings->FrqStop*1000000)));
  }
  else
  {
   //FCenterSpan
   // Label
   ui->txtFrqStartCenter_Label->setText(QString("Center"));
   ui->txtFrqStopSpan_Label->setText(QString("Span"));
   // Value
   ui->txtFrqStartCenter_Value->setText(QString("%0").arg((unsigned long)(FrqSettings->FrqCenter*1000000)));
   ui->txtFrqStopSpan_Value->setText(QString("%0").arg((unsigned long)(FrqSettings->FrqSpan*1000000)));
  };
  ui->txtFrqFSW_Value->setText(QString("%0").arg(FrqSettings->FrqStepWidth,3,'f',3));
  ui->txtFrqRBW_Value->setText(QString("%0").arg(FrqSettings->RBW,3,'f',3));
  ui->txtFrqRefLvl_Value->setText(QString("%0").arg(FrqSettings->RefDcLevel));
  done = true;
 }
 else
 {
  if(ui->bttnFrqRangeMode)
  {//FStartStop
   // Label
   ui->txtFrqStartCenter_Label->setText(QString("Start"));
   ui->txtFrqStopSpan_Label->setText(QString("Stop"));
   // Value
   ui->txtFrqStartCenter_Value->setText(s32ToString((long)(FrqSettings->FrqStart*1000000)));
   ui->txtFrqStopSpan_Value->setText(s32ToString((long)(FrqSettings->FrqStop*1000000)));
  }
  else
  {
   //FCenterSpan
   // Label
   ui->txtFrqStartCenter_Label->setText("Center");
   ui->txtFrqStopSpan_Label->setText("Span");
   // Value
   ui->txtFrqStartCenter_Value->setText(" --- ");
   ui->txtFrqStopSpan_Value->setText(" --- ");
  };

  ui->txtFrqStartCenter_Value->setText(" --- ");
  ui->txtFrqStopSpan_Value->setText(" --- ");
  ui->txtFrqRBW_Value->setText(" --- ");
  ui->txtFrqFSW_Value->setText(" --- ");
  ui->txtFrqRefLvl_Value->setText(" --- ");
 };

 return(done);
}

bool MainWindow::guiDisplayMarkerInfo(sMarkerInfo *mInfo)
{
 bool done = false;
 bool flagMarker0Delta = false;
 bool flagMarker2Delta = false;
 double deltaMHz = 0;
 double deltadBm = 0;

 QLabel *ptrMarkerdBm[3]={ui->txtGraphMarker0_Value_dBm,ui->txtGraphMarker1_Value_dBm,ui->txtGraphMarker2_Value_dBm};
 QLabel *ptrMarkerMHz[3]={ui->txtGraphMarker0_Value_MHz,ui->txtGraphMarker1_Value_MHz,ui->txtGraphMarker2_Value_MHz};
 QLabel *ptrMarkerLabel[3]={ui->txtGraphMarker0_Label,ui->txtGraphMarker1_Label,ui->txtGraphMarker2_Label};
 QLabel *ptrMarkerUnitdBm[3]={ui->txtGraphMarker0_Unit_dBm,ui->txtGraphMarker1_Unit_dBm,ui->txtGraphMarker2_Unit_dBm};

 if(mInfo)
 {// Init Display Information
  // Check Delta Mode
  if(mInfo->On[0] && mInfo->On[1])
   if(ui->rbMarker0MakeDeltaM1->isEnabled())
    if(ui->rbMarker0MakeDeltaM1->isChecked())
     flagMarker0Delta = true;

  if(mInfo->On[2] && mInfo->On[1])
   if(ui->rbMarker2MakeDeltaM1->isEnabled())
    if(ui->rbMarker2MakeDeltaM1->isChecked())
     flagMarker2Delta = true;
  //Update Display
  // Marker 0
  if(mInfo->On[0])
  {
   if(flagMarker0Delta)
   {
    if(mInfo->dBmValue[0] > mInfo->dBmValue[1])
    {
     deltadBm = mInfo->dBmValue[0] - mInfo->dBmValue[1];
    }
    else
    {
     deltadBm =(mInfo->dBmValue[1] - mInfo->dBmValue[0]);
     deltadBm = -deltadBm;
    };
    if(mInfo->FrqValue[0] > mInfo->FrqValue[1])
    {
     deltaMHz = mInfo->FrqValue[0] - mInfo->FrqValue[1];
    }
    else
    {
     deltaMHz = (mInfo->FrqValue[1] - mInfo->FrqValue[0]);
     deltaMHz = -deltaMHz;
    };
    ptrMarkerLabel[0]->setText("M0* ");
    ptrMarkerUnitdBm[0]->setText("dB ");
    ptrMarkerdBm[0]->setText(QString("%0").arg(deltadBm,0,'f',1));
    ptrMarkerMHz[0]->setText(s32ToString((long)(deltaMHz*1000000)));
   }
   else
   {
    ptrMarkerLabel[0]->setText("M0");
    ptrMarkerUnitdBm[0]->setText("dBm");
    ptrMarkerdBm[0]->setText(QString("%0").arg(mInfo->dBmValue[0],0,'f',1));
    ptrMarkerMHz[0]->setText(s32ToString((long)(mInfo->FrqValue[0]*1000000)));
   };
  }
  else
  {
   ptrMarkerLabel[0]->setText("M0");
   ptrMarkerUnitdBm[0]->setText("dBm");
   ptrMarkerdBm[0]->setText(QString(" --- "));
   ptrMarkerMHz[0]->setText(QString(" --- "));
  };
  // Marker 1
  if(mInfo->On[1])
  {
   ptrMarkerdBm[1]->setText(QString("%0").arg(mInfo->dBmValue[1],0,'f',1));
   ptrMarkerMHz[1]->setText(s32ToString((long)(mInfo->FrqValue[1]*1000000)));
  }
  else
  {
   ptrMarkerdBm[1]->setText(QString(" --- "));
   ptrMarkerMHz[1]->setText(QString(" --- "));
  };
  // Marker 2
  if(mInfo->On[2])
  {
   if(flagMarker2Delta)
   {
    if(mInfo->dBmValue[2] > mInfo->dBmValue[1])
    {
     deltadBm = mInfo->dBmValue[2] - mInfo->dBmValue[1];
    }
    else
    {
     deltadBm = (mInfo->dBmValue[1] - mInfo->dBmValue[2]);
     deltadBm = -deltadBm;
    };
    if(mInfo->FrqValue[2] > mInfo->FrqValue[1])
    {
     deltaMHz = mInfo->FrqValue[2] - mInfo->FrqValue[1];
    }
    else
    {
     deltaMHz = (mInfo->FrqValue[1] - mInfo->FrqValue[2]);
     deltaMHz = -deltaMHz;
    };
    ptrMarkerLabel[2]->setText("M2* ");
    ptrMarkerUnitdBm[2]->setText("dB ");
    ptrMarkerdBm[2]->setText(QString("%0").arg(deltadBm,0,'f',1));
    ptrMarkerMHz[2]->setText(s32ToString((long)((deltaMHz)*1000000)));
   }
   else
   {
    ptrMarkerLabel[2]->setText("M2");
    ptrMarkerUnitdBm[2]->setText("dBm");
    ptrMarkerdBm[2]->setText(QString("%0").arg(mInfo->dBmValue[2],0,'f',1));
    ptrMarkerMHz[2]->setText(s32ToString((long)(mInfo->FrqValue[2]*1000000)));
   };
  }
  else
  {
   ptrMarkerLabel[2]->setText("M2");
   ptrMarkerUnitdBm[2]->setText("dBm");
   ptrMarkerdBm[2]->setText(QString(" --- "));
   ptrMarkerMHz[2]->setText(QString(" --- "));
  };
 };

 return(done);
}

void MainWindow::guiUpdateFrq(sFrqValues *FrqSetting)
{
 guiDisplayFrqSettings(FrqSetting);
}


QString MainWindow::s32ToString(long value)
{
 QString txtValue = QString::number(value);
 int length       = txtValue.length();
 if(value>0)
 {
  if(length>3)
  {
   txtValue.insert(txtValue.length()-3,'.');
  };
  if(length>7)
  {
   txtValue.insert(txtValue.length()-7,'.');
  };
 }
 else
 {
  if(length>4)
  {
   txtValue.insert(txtValue.length()-3,'.');
  };
  if(length>8)
  {
   txtValue.insert(txtValue.length()-7,'.');
  };
 }
 return(txtValue);
}

bool MainWindow::guiTraceExport(eTrace TraceNr,eTraceMode TraceMode)
{
 bool         done = false;
 QString      strFileName;
 QVector<double> DataFrq;
 QVector<double> DatadBm;
 appReportCsv csvWriter;

 if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
  if(Status.Spectrum.flagActiveFrqValues)
   if(TraceMode != T_MODE_OFF)
   {
    strFileName = QFileDialog::getSaveFileName(this,tr("Save Trace Curve Data"),QDir::currentPath(), tr("Trace Curve File(*.csv)"));
    if(!strFileName.isEmpty())
     if(plotCtrl->TraceGetData(TraceNr,&DataFrq,&DatadBm))
      if(csvWriter.WriteCurveCsv(strFileName,&Status.Spectrum.activFrqValues,&DataFrq,&DatadBm))
      {
       done = true;
      };
   };

 return(done);
}

bool MainWindow::guiTraceHoldTrigger(eTrace TraceNr)
{
 bool done = false;

 if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
 {
  plotCtrl->TraceHoldTrigger(TraceNr);
  done = true;
 };

 return(done);
}

bool MainWindow::guiTraceHoldReset(eTrace TraceNr)
{
 bool done = false;

 if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
 {
  plotCtrl->TraceHoldReset(TraceNr);
  done = true;
 };

 return(done);
}

bool MainWindow::guiTraceClear(eTrace TraceNr)
{
 bool done = false;

 if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
 {
  plotCtrl->TraceClear(TraceNr);
  done = true;
 };

 return(done);
}

bool MainWindow::guiFrqGetActualSettings(sFrqValues *actualFrqValues)
{
 bool done = false;

 if(actualFrqValues)
 {
  // Check Frequency Range
  if(ui->rbRfRange_300_348->isChecked())
   actualFrqValues->FrqRange = FRQRANGE_300_348;
  else
  if(ui->rbRfRange_389_464->isChecked())
   actualFrqValues->FrqRange = FRQRANGE_389_464;
  else
  if(ui->rbRfRange_779_928->isChecked())
   actualFrqValues->FrqRange = FRQRANGE_779_928;

  // Check Spectrum Parameters
  if(!ui->bttnFrqRangeMode->isEnabled())
  {// FStart <-> FStop
   actualFrqValues->flagModeStartStop = true;
   guiFrqStartStop2CenterSpan();
  }
  else
  {// FSpan<-FCenter->FSpan
   actualFrqValues->flagModeStartStop = false;
   guiFrqCenterSpan2StartStop();
  };
  actualFrqValues->FrqStart  = ui->spFrqStartValue->value();
  actualFrqValues->FrqStop   = ui->spFrqStopValue->value();
  actualFrqValues->FrqCenter = ui->spFrqCenterValue->value();
  actualFrqValues->FrqSpan   = ui->spFrqSpanValue->value();

  // Resolution Band Width
  actualFrqValues->RBWIndex        = ui->cbResolutionBandWidthValue->currentIndex();
  // Reference Level
  actualFrqValues->RefDcLevelIndex = ui->cbRefDcLevelValue->currentIndex();
  // Easy Rf and Expert Mode
  if(ui->bttnRfModeEasyRf->isChecked())
  {// Export Mode Yes -> Take Step width from input control
   actualFrqValues->flagModeEasyRf  = true;
   actualFrqValues->FrqStepWidth    = ui->spFrqStepwidthValue->value();
  }
  else
  {// Export Mode No  -> Auto Calculate StepWidth
   actualFrqValues->flagModeEasyRf  = false;
   actualFrqValues->FrqStepWidth    = ui->spFrqStepwidthValue->value();
  };
  // Continous and Single Mode
  if(ui->rbSweepModeContinous->isChecked())
  {
   actualFrqValues->flagModeContinous   = true;
  }
  else
  {
   actualFrqValues->flagModeContinous   = false;
  };
  done = true;
 };

 return(done);
}

bool MainWindow::guiFrqCalcEasyRfSettings(sFrqValues *FrqValues)
{
 bool done = false;
 double initFrqPoints = (double)(180);
 double minRbwStep    = (double)(0.1); //
 double maxRbwStep    = (double)(0.5); //
 double RBW           = 0;
 int    RBWIndex      = 0;
 double fSpan         = 0;
 double fStep         = 0;

 if(FrqValues->flagModeEasyRf)
 {
  fSpan = FrqValues->FrqStop - FrqValues->FrqStart;
  fStep = (fSpan / initFrqPoints)*1000;
  RBW   = fStep*minRbwStep;
  // Search closest available RBW Value
  for(RBWIndex=0;RBWIndex<15;RBWIndex++)
  {
   if(RBW<= FrqTableFRBW[RBWIndex].FrqKHz)
    break;
  };
  // Adjust fStep
  if(fStep>maxRbwStep*FrqTableFRBW[RBWIndex].FrqKHz)
  {
   fStep = maxRbwStep*FrqTableFRBW[RBWIndex].FrqKHz;
  };
  // Save New Frq Values
  FrqValues->FrqStepWidth = fStep;
  FrqValues->RBWIndex     = RBWIndex;
  FrqValues->RBW          = FrqTableFRBW[RBWIndex].FrqKHz;
  // Update Gui Controls
  ui->spFrqStepwidthValue->setValue(fStep);
  ui->cbResolutionBandWidthValue->setCurrentIndex(RBWIndex);
  done = true;
 };


 return(done);
}


void MainWindow::guiFrqCenterSpan2StartStop(void)
{
 double fstart      = 0;
 double fstop       = 0;
 double fmin        = ui->spFrqStartValue->minimum();
 double fmax        = ui->spFrqStartValue->maximum();
 double fcenter     = ui->spFrqCenterValue->value();
 double fspan       = ui->spFrqSpanValue->value();
 double fspanside   = fspan/2;

 // Calculate fspan and fcenter
 // Check upper Frq limit
 if((fcenter + fspanside) <= fmax)
 {// Clip max Frequency
  fstop = fcenter + fspanside;
 }
 else
 {
  fstop = fmax;
 };
 // Check lower Frq limit
 if((fcenter - fspanside)> fmin)
 {// Clip min Frequency
  fstart =  fcenter - fspanside;
 }
 else
 {
  fstart = fmin;
 };

 // Update Gui Controls
 ui->spFrqStartValue->setValue(fstart);
 ui->spFrqStopValue->setValue(fstop);

}

void MainWindow::guiFrqStartStop2CenterSpan(void)
{
 double fstart  = ui->spFrqStartValue->value();
 double fstop   = ui->spFrqStopValue->value();
 double fcenter = 0;
 double fspan   = 0;

 // Calculate fspan and fcenter
 fspan   = fstop - fstart;
 fcenter = fstart + (fspan/2);

 // Update Gui Controls
 ui->spFrqCenterValue->setValue(fcenter);
 ui->spFrqSpanValue->setValue(fspan);

}


bool MainWindow::guiFwUpdateIfRequired(sa430UsbDevice *UsbInfo, sDeviceInfo *DevInfo)
{
 bool done = true;

 // Parameter Check
 if(UsbInfo && DevInfo)
 {
  // Check Firmware Version
  if(DevInfo->CoreVersion>=0x0209 && DevInfo->SpecVersion!=0xffff)
   if(DevInfo->SpecVersion<=0x0204 && DevInfo->SpecVersion!=0xffff)
   {
    appFwUpdater updater(this);
    deviceCtrl->deviceDisconnect();
    updater.SetParameter(UsbInfo->PortName,"SA430_FW0205_SPEC.bin");
    switch(updater.exec())
    {
     case appFwUpdater::Done:
        QMessageBox::information(this, tr("Auto Firmware Updater"),tr("The SA430 Spectrum Firmware is now up to date\n Connect the SA430 again to proceed"),QMessageBox::Ok);
        done = true;
       break;
     case appFwUpdater::WRONGARGC:
     case appFwUpdater::WRONGARGV:
     case appFwUpdater::FILEMISSING:
     case appFwUpdater::FILECORRUPT:
     case appFwUpdater::COMPORTISSUE:
     case appFwUpdater::NODEVICERESPONSE:
     case appFwUpdater::CONNECTIONLOST:
        QMessageBox::critical(this, tr("Auto Firmware Updater"),tr("The Firmware failed\n Connect the SA430 to proceed"),QMessageBox::Ok,QMessageBox::NoButton);
       break;
     case appFwUpdater::ERASEPTFAIL:
     case appFwUpdater::WRITEPTFAIL:
     case appFwUpdater::ERASEFTFAIL:
     case appFwUpdater::WRITEFTFAIL:
     case appFwUpdater::ERASECODEFAIL:
     case appFwUpdater::WRITECODEFAIL:
     case appFwUpdater::UPDPTFAIL:
     case appFwUpdater::FLASHREADFAILED:
       QMessageBox::warning(this, tr("Auto Firmware Updater"),tr("The SA430 Spectrum Firmware Update failed\n Contact the TI support"),QMessageBox::Ok,QMessageBox::NoButton);
       break;

     default:
        break;
    };
   };
 };

 return(done);
}

bool MainWindow::guiFwUpdate(QString ComPort)
{
 bool done = true;
 appFwUpdater updater(this);

 // Parameter Check
 if(!ComPort.isEmpty())
 {
  deviceCtrl->deviceDisconnect();
  updater.SetParameter((char*)ComPort.toStdString().c_str(),"SA430_FW0205_SPEC.bin");
  switch(updater.exec())
  {
   case appFwUpdater::Done:
        QMessageBox::information(this, tr("Auto Firmware Updater"),tr("The SA430 Spectrum Firmware is now up to date\n Connect the SA430 again to proceed"),QMessageBox::Ok);
        done = true;
       break;
   case appFwUpdater::WRONGARGC:
   case appFwUpdater::WRONGARGV:
   case appFwUpdater::FILEMISSING:
   case appFwUpdater::FILECORRUPT:
   case appFwUpdater::COMPORTISSUE:
   case appFwUpdater::NODEVICERESPONSE:
   case appFwUpdater::CONNECTIONLOST:
        QMessageBox::critical(this, tr("Auto Firmware Updater"),tr("The SA430 Spectrum Firmware Update failed\n Connect the SA430 to proceed"),QMessageBox::Ok,QMessageBox::NoButton);
       break;
   case appFwUpdater::ERASEPTFAIL:
   case appFwUpdater::WRITEPTFAIL:
   case appFwUpdater::ERASEFTFAIL:
   case appFwUpdater::WRITEFTFAIL:
   case appFwUpdater::ERASECODEFAIL:
   case appFwUpdater::WRITECODEFAIL:
   case appFwUpdater::UPDPTFAIL:
   case appFwUpdater::FLASHREADFAILED:
        QMessageBox::warning(this, tr("Auto Firmware Updater"),tr("The SA430 Spectrum Firmware Update failed\n Contact the TI support"),QMessageBox::Ok,QMessageBox::NoButton);
       break;

     default:
        break;
  };
 };

 return(done);
}

