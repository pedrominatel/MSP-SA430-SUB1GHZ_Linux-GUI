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
#include <windows.h>
#include <string>
#include <dbt.h>
#include "cEvent.h"
#include "cThread.h"
#include "cRegAccess.h"

using namespace std;


#pragma once

static const GUID GuidDevInterfaceList[] =
{
{ 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } },
{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
{ 0x4d1e55b2, 0xf16f, 0x11Cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }
};

/*!
 \brief Usb Detect Class

 \class cUsbDetect cUsbDetect.h "cUsbDetect.h"
*/
class cUsbDetect
{
public:
        /*!
         \brief Constructor

         \fn cUsbDetect
         \param void
        */
        cUsbDetect(void);
        /*!
         \brief Destructor

         \fn ~cUsbDetect
         \param void
        */
        virtual ~cUsbDetect(void);
        /*!
         \brief Traces dedicated comport for unplugg event

         \fn On
         \param strComPort
        */
        bool On(std::string strComPort);
        /*!
         \brief Switch off tracing the dedicated comport

         \fn Off
         \param void
        */
        bool Off(void);
        /*!
         \brief Checks if unplugg event happened

         \fn EventUnplugged
         \param void
        */
        bool EventUnplugged(void);
        /*!
         \brief Check if active port is connected

         \fn IsActivePortConnected
         \param void
        */
        bool IsActivePortConnected(void);
        /*!
         \brief Verifies if given string is equal to the active comport

         \fn IsSame
         \param strTest
        */
        bool IsSame(std::string *strTest);

    
private:
	//Private Variables
        std::string strComPortName;
        std::string strComPortSerial;
        bool flagCreated;
    const char *className;
        WNDCLASSA wincl;
        cRegAccess RegAccess;

        cEvent *eUnpluggedEvent;
        HWND  hparent;
        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
        HDEVNOTIFY hDeviceNotify;
        TThread<cUsbDetect> *ThreadHandle;
        volatile bool flagExitThread;

	// Private Function Decleration

        /*!
        \brief run

        \fn run
        \param void
        */
        void run(void);
};
