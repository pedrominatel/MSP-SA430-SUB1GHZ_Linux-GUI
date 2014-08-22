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
#ifndef APPFWUPDATER_H
#define APPFWUPDATER_H

#include <QDialog>
#include <QProcess>
#include <QTimer>
#include <QStringListModel>

namespace Ui {
    class appFwUpdater;
}

class appFwUpdater : public QDialog
{
    Q_OBJECT

 public:
    explicit appFwUpdater(QWidget *parent = 0);
    ~appFwUpdater();

    enum
    {
     Done           =  1,
     WRONGARGC      = -8,   // Wrong number of arguments passed to fwUpdater
     WRONGARGV      = -9,   // Can not understand passed arguments
     FILEMISSING    = -3,   // FW update file not found
     FILECORRUPT    = -4,   // FW update file inconsistent
     COMPORTISSUE   = -5,   // Unable to open COM port
     NODEVICERESPONSE = -6, // Can send data, but device is not answering
     CONNECTIONLOST   = -7, // Connection lost during update. Connect device again and try to restart fwupdater
     // all the following is one issue: Can not write fw
     // It is only made more granular for debugging
     ERASEPTFAIL    = -10,   // Unable to erase the PT part
     WRITEPTFAIL    = -11,   // Unable to write the PT part
     ERASEFTFAIL    = -12,   // Unable to erase the FT part
     WRITEFTFAIL    = -13,   // Unable to write the FT part
     ERASECODEFAIL  = -14,   // Unable to erase the code part
     WRITECODEFAIL  = -15,   // Unable to write the code part
     UPDPTFAIL      = -16,   // Update of PT part failed
     FLASHREADFAILED= -17   // Read of Flash failed
    };

    bool SetParameter(char *ComPort ,const char *FileName);
private slots:
    void eventStartUpdate(void);

    void eventCmdEerror(QProcess::ProcessError Error);
    void eventCmdFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void eventCmdReadyReadStandardError(void);
    void eventCmdReadyReadStandardOutput(void);
    void eventCmdStarted(void);
    void eventCmdStateChanged(QProcess::ProcessState newState );

    void eventTimerTick(void);
 private:
    Ui::appFwUpdater *ui;
    QProcess         *Cmd;
    QString           cmdProgName;
    QStringList       argList;
    QString           argComPort;
    QString           argFileName;
    QTimer           *Timer;
    QStringList       slMonitor;
    QStringListModel *mMonitor;
};

#endif // APPFWUPDATER_H
