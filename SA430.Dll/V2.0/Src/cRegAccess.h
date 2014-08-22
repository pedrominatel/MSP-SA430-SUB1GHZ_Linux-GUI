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
#include <Windows.h>
#include <string>
#include <list>
#include <vector>
using namespace std;

#include "sa430TypeDef.h"

#pragma once

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

/*!
 \brief

 \class cRegAccess cRegAccess.h "cRegAccess.h"
*/
class cRegAccess
{
public:
        /*!
         \brief Constructor

         \fn cRegAccess
         \param void
        */
        cRegAccess(void);
        /*!
         \brief Destructor

         \fn ~cRegAccess
         \param void
        */
        virtual ~cRegAccess(void);
        /*!
         \brief Compare the USB Device information list with given comport name

         \fn GetPortSerial
         \param port
         \param UsbSerial
        */
        bool GetPortSerial(std::string port, std::string & UsbSerial);
        /*!
         \brief Load the active comport list from the Windows OS Registry

         \fn LoadComPortList
         \param comPortList
        */
        bool LoadComPortList(lComPortList *comPortList);
        /*!
         \brief Return all installed SA430 Device informations

         \fn LoadRegDeviceList
         \param usbDeviceList
        */
        bool LoadRegDeviceList(vUsbDeviceList *usbDeviceList);
    /*!
     \brief Returns the dedicated SA430 USB device information

     \fn LoadRegDeviceDetails
     \param item
    */
    bool LoadRegDeviceDetails(vUsbDeviceList::iterator item);
private:
        /*!
         \brief

         \class _sRegEntry cRegAccess.h "cRegAccess.h"
        */
        typedef struct sRegEntry
	{
         std::string strKey;    /*!< TODO */
         std::string strValue;  /*!< TODO */
        }sRegEntry;
        /*!
         \brief Returns the specific Windows Registry Key value(s)

         \fn RegGetKeyValues
         \param hKey
         \param regEntryList
        */
        bool RegGetKeyValues(HKEY hKey,list<sRegEntry> *regEntryList);
};
