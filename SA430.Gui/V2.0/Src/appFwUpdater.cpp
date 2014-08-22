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
#include "appFwUpdater.h"
#include "ui_appFwUpdater.h"

#include <QDir>
#include <QFile>


appFwUpdater::appFwUpdater(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::appFwUpdater)
{
 // Generate Objects
 Cmd        = new QProcess(this);
 Timer      = new QTimer(this);
 mMonitor   = new QStringListModel(this);


 // Timer
 Timer->stop();
 Cmd->setTextModeEnabled(true);

 ui->setupUi(this);

 // Init Objects
 // Cmd
 // Monitor
 mMonitor->setStringList(slMonitor);
 // Connect Events
 connect(Cmd,SIGNAL(error(QProcess::ProcessError)),this,SLOT(eventCmdEerror(QProcess::ProcessError)));
 connect(Cmd,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(eventCmdFinished(int,QProcess::ExitStatus)));
 connect(Cmd,SIGNAL(readyReadStandardError()),this,SLOT(eventCmdReadyReadStandardError()));
 connect(Cmd,SIGNAL(readyReadStandardOutput()),this,SLOT(eventCmdReadyReadStandardOutput()));
 connect(Cmd,SIGNAL(started()),this,SLOT(eventCmdStarted()));
 connect(Cmd,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(eventCmdStateChanged(QProcess::ProcessState)));

 connect(Timer,SIGNAL(timeout()),this,SLOT(eventTimerTick()));
}

appFwUpdater::~appFwUpdater()
{
 delete ui;
 delete mMonitor;
 delete Timer;
 delete Cmd;
}

// Public Function Definition
bool appFwUpdater::SetParameter(char *ComPort ,const char *FileName)
{
 bool done = false;

 argComPort.clear();
 argFileName.clear();
 argList.clear();

 if(ComPort && FileName)
 {
  argComPort = QString(ComPort);
  argFileName= QString(FileName);
  // Assign Argument Arg0: ComPort Arg1: FileName
  argList << argComPort << argFileName;
  QTimer::singleShot(50, this, SLOT(eventStartUpdate()));
  done = true;
 };

 return(done);
}



// Private Slot Function Definition
void appFwUpdater::eventStartUpdate(void)
{
 QString appDir;

 appDir = QDir::currentPath();
 if(QFile::exists("sa430FwUpdate.exe"))
  if(QFile::exists(argFileName))
  {
      Timer->start(25);
   Cmd->start("sa430FwUpdate.exe",argList);
  };

 appDir = appDir;
}

void appFwUpdater::eventCmdEerror(QProcess::ProcessError Error)
{
 switch(Error)
 {
  case QProcess::FailedToStart:
       slMonitor.append("Error: Failed to start");
    break;
  case QProcess::Crashed:
     slMonitor.append("Error: Crashed");
    break;
  case QProcess::Timedout:
     slMonitor.append("Error: Timeout");
    break;
  case QProcess::WriteError:
     slMonitor.append("Error: Write Error");
    break;
  case QProcess::ReadError:
     slMonitor.append("Error: Read Error");
    break;
  case QProcess::UnknownError:
     slMonitor.append("Error: Uknown Error");
    break;
   default:
     break;
 };
}

void appFwUpdater::eventCmdFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
 switch(exitStatus)
 {
  case QProcess::NormalExit:
     slMonitor.append("Finished Normal: NotRunning");
     slMonitor.append(QString("Finished Normal: Exit Code -> %0").arg(exitCode));
    break;
  case QProcess::CrashExit:
     slMonitor.append("Finished Crashed: NotRunning");
     slMonitor.append(QString("Finished Crashed: Exit Code -> %0").arg(exitCode));

    break;
   default:
       break;
 };
 this->done(exitCode);
}

void appFwUpdater::eventCmdReadyReadStandardError(void)
{
  slMonitor.append(QString(Cmd->readAll()).split('\n'));
  mMonitor->setStringList(slMonitor);
}

void appFwUpdater::eventCmdReadyReadStandardOutput(void)
{
 slMonitor.append(QString(Cmd->readAll()).split('\n'));
   mMonitor->setStringList(slMonitor);
}

void appFwUpdater::eventCmdStarted(void)
{
 slMonitor.append("Start: Event");
}

void appFwUpdater::eventCmdStateChanged(QProcess::ProcessState newState )
{
  slMonitor.append("StateChanged: Event");
}


void appFwUpdater::eventTimerTick(void)
{
 if(ui->progressBar->value()==100)
 {
  ui->progressBar->setValue(0);
 }
 else
 {
  ui->progressBar->setValue(ui->progressBar->value()+1);
 };
}
