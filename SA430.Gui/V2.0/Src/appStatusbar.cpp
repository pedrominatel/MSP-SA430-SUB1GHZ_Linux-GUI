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
#include "appStatusbar.h"

appStatusBar::appStatusBar(QStatusBar *statusBar)
{
 if(!statusBar)
   return;

 sbCtrl     = statusBar;

 timerCtrl  = new QTimer();

 Init();

 sbCtrl->addWidget(&txtStatusConnect);
 sbCtrl->addWidget(&txtStatusDllVer);
 sbCtrl->addWidget(&txtTime);

 connect(timerCtrl,SIGNAL(timeout()), this, SLOT(eventTimerTick()));
 timerCtrl->start(1000);
}


appStatusBar::~appStatusBar()
{
 timerCtrl->stop();
}

void appStatusBar::SetDllGuiVersion(unsigned short VerDll,unsigned short VerGui)
{
 unsigned char verDllMajor;
 unsigned char verDllMinor;
 unsigned char verGuiMajor;
 unsigned char verGuiMinor;

 verDllMajor = (unsigned char)(VerDll>>8);
 verDllMinor = (unsigned char)(VerDll&0xff);

 verGuiMajor = (unsigned char)(VerGui>>8);
 verGuiMinor = (unsigned char)(VerGui&0xff);

 txtStatusDllVer.setText(QString("Dll V%0.%1, GUI V%2.%3").arg(verDllMajor,2,16).arg(verDllMinor,2,16).arg(verGuiMajor,2,16).arg(verGuiMinor,2,16));
 sbCtrl->update();
}


void appStatusBar::SetDeviceConnected(sa430UsbDevice *DevInfo)
{
 txtStatusConnect.setText(QString("\t Device is connected to Port:%0 \t").arg(DevInfo->PortName));
 sbCtrl->update();
}

void appStatusBar::SetDeviceDisconnected(void)
{
 txtStatusConnect.setText("\t Device is NOT connected \t");
 sbCtrl->update();
}


// Private Slot Definition
void appStatusBar::eventTimerTick(void)
{
 Update();
}


// Private Function Defintion
void appStatusBar::Init(void)
{
 txtStatusConnect.setText("\t Device is NOT connected \t");
 txtStatusConnect.setAlignment(Qt::AlignHCenter);
 txtStatusConnect.setMinimumWidth(100);
 txtStatusDllVer.setText("\t Dll V--.--, GUI V--.--\t");
 txtStatusDllVer.setAlignment(Qt::AlignHCenter);
 txtStatusDllVer.setMinimumWidth(100);
 txtTime.setText(QString("\t Time: %0 \t").arg(QTime::currentTime().toString()));
 txtTime.setAlignment(Qt::AlignHCenter);
 txtTime.setMinimumWidth(100);
}

void appStatusBar::Update(void)
{
 txtTime.setText(QString("\t Time: %0 \t").arg(QTime::currentTime().toString()));
}

