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
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SA430_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SA430_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef SA430_H
#define SA430_H

#include <string>
using namespace std;

#ifdef SA430_EXPORTS
#define SA430_API __declspec(dllexport)
#else
#define SA430_API __declspec(dllimport)
#endif

//#define API_CALL   __stdcall
//#define API_CALL   __cdecl
#define API_CALL


#ifdef __cplusplus
extern "C"
{
#endif

#define FRAME_MAX_DATA_LENGTH	255

/*!
 \brief

 \class _sa430UsbDevice sa430.h "sa430.h"
*/
/*!
\brief
\typedef sa430UsbDevice
*/
typedef struct sa430UsbDevice
 {
  char SerialNr[50];            /*!< TODO */
  char PortName[50];            /*!< TODO */
  char DevDesc[50];             /*!< TODO */
  char LocationInformation[50]; /*!< TODO */
  bool  Connected;              /*!< TODO */
 }sa430UsbDevice;

/*!
 \brief

 \class sa430Frame sa430.h "sa430.h"
*/
typedef struct sa430Frame
{
 unsigned char  Cmd;            /*!< TODO */
 unsigned char  Length;         /*!< TODO */
 unsigned char  Data[FRAME_MAX_DATA_LENGTH]; /*!< TODO */
 unsigned short Crc;            /*!< TODO */
}sa430Frame;


/*!
 \brief

 \class sa430Status sa430.h "sa430.h"
*/
typedef struct sa430Status
{
 bool flagInit;                 /*!< TODO */
 bool flagOpen;                 /*!< TODO */
 bool flagUsbRemoval;           /*!< TODO */
 bool flagDrvError;             /*!< TODO */
 bool flagFrameReceived;        /*!< TODO */
 bool flagFrameError;           /*!< TODO */
 bool flagFrameErrorCrc;        /*!< TODO */
 bool flagFrameErrorTimeOut;    /*!< TODO */
}sa430Status;

/*!
 \brief Returns SA430 Interface Dll Version

 \fn sa430GetDllVersion
 \param void
*/
SA430_API unsigned short API_CALL sa430GetDllVersion(void);

/*!
 \brief Returns number of detected and installed devices on the current PC

 \fn sa430FindDevices
 \param NumDevices
*/
SA430_API bool API_CALL sa430FindDevices(unsigned short &NumDevices);
/*!
 \brief Returns SA430 USB Interface Device Information

 \fn sa430GetDeviceInfo
 \param DevNr
 \param DevInfo
*/
SA430_API bool API_CALL sa430GetDeviceInfo(unsigned short DevNr, sa430UsbDevice *DevInfo);

/*!
 \brief Init the SA430 Interface Dll

 \fn sa430Init
 \param void
*/
SA430_API bool API_CALL sa430Init(void);
/*!
 \brief Checks if all Objects are created and successfully initialized

 \fn sa430IsInit
 \param void
*/
SA430_API bool API_CALL sa430IsInit(void);
/*!
 \brief Delete all generated objects

 \fn sa430DeInit
 \param void
*/
SA430_API void API_CALL sa430DeInit(void);

/*!
 \brief Returns the current device status structure sa430Status

 \fn sa430GetStatus
 \param Status
*/
SA430_API bool API_CALL sa430GetStatus(sa430Status &Status);

/*!
 \brief Connect dedicated device, given by the comportname

 \fn sa430Connect
 \param strPort
*/
SA430_API bool API_CALL sa430Connect(const char *strPort);
/*!
 \brief Checks if the comport was successfully opened

 \fn sa430IsConnected
 \param void
*/
SA430_API bool API_CALL sa430IsConnected(void);
/*!
 \brief Disconnect active comport

 \fn sa430Disconnect
 \param void
*/
SA430_API bool API_CALL sa430Disconnect(void);

/*!
 \brief Checks if comport has received new frames from the SA430 device in the meantime

 \fn sa430IsFrameAvailable
 \param void
*/
SA430_API bool API_CALL sa430IsFrameAvailable(void);
/*!
 \brief Return the actuall number of received frames within the Frame Fifo Buffer

 \fn sa430GetFrameFifoSize
 \param Size
*/
SA430_API bool API_CALL sa430GetFrameFifoSize(unsigned short &Size);
/*!
 \brief Return the topmost frame from the Frame Fifo Buffer

 \fn sa430GetFrame
 \param Frame
*/
SA430_API bool API_CALL sa430GetFrame(sa430Frame *Frame);

/*!
 \brief Sends a Frame to the SA430 Device

 \fn sa430SendCmd
 \param Cmd
 \param Data
 \param Size
*/
SA430_API bool API_CALL sa430SendCmd(unsigned char Cmd, unsigned char *Data, unsigned short Size);

#ifdef __cplusplus
}
#endif

#endif // QTSPA430_H
