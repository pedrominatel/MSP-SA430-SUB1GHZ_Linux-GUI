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
// sa430.cpp : Defines the exported functions for the DLL application.
//

#include "sa430TypeDef.h"
#include "sa430.h"
#include "cDeviceDriver.h"
#include "cUsbDetect.h"
#include "cRegAccess.h"
#include <string>
#include <list>
#include <vector>

using namespace std;

#define DLL_VERSION		((unsigned short)(0x0200))


volatile bool           flagInit      = false; /*!< TODO */
volatile bool           flagConnected = false; /*!< TODO */

cDeviceDriver           *Device    = NULL; /*!< TODO */
cUsbDetect		*UsbDetect = NULL; /*!< TODO */
cRegAccess		RegAccess; /*!< TODO */


lComPortList            ComPortList; /*!< TODO */
vUsbDeviceList          UsbDeviceList; /*!< TODO */

/*!
 \brief Returns SA430 Interface Dll Version

 \fn sa430GetDllVersion
 \param void
 \return unsigned short
*/
SA430_API unsigned short API_CALL sa430GetDllVersion(void)
{
 return(DLL_VERSION);
}


/*!
 \brief Returns number of detected and installed devices on the current PC

 \fn sa430FindDevices
 \param NumDevices
 \return bool
*/
SA430_API bool API_CALL sa430FindDevices(unsigned short &NumDevices)
{
 bool ok = false;

 ComPortList.clear(); 
 sUsbDeviceInfo usbdeviceInfo;
 
 NumDevices = 0;
 
 RegAccess.LoadComPortList(&ComPortList);
 
 if(RegAccess.LoadRegDeviceList(&UsbDeviceList))
  { 
   for(vUsbDeviceList::iterator itemDevice= UsbDeviceList.begin(); itemDevice!= UsbDeviceList.end(); itemDevice++)
     {
      if(RegAccess.LoadRegDeviceDetails(itemDevice))
 	   {
	    NumDevices++;
		if(!ComPortList.empty())
		 for(lComPortList::iterator itemCom = ComPortList.begin(); itemCom!= ComPortList.end(); itemCom++)  
		   {
                     if(itemDevice->PortName.compare(itemCom->Nr)==0 && itemDevice->PortName.size()!=0)
		      { 
			   itemDevice->Connected = true;
		       ok = true;
			   break;
		      }
		     else
		      {
		       itemDevice->Connected = false;
			   ok = true;
		      };
		   };
	    };
	 };
  };
 NumDevices = UsbDeviceList.size();
 return(ok);
}

/*!
 \brief Returns SA430 USB Interface Device Information

 \fn sa430GetDeviceInfo
 \param DevNr
 \param DevInfo
 \return bool
*/
SA430_API bool API_CALL sa430GetDeviceInfo(unsigned short DevNr, sa430UsbDevice *DevInfo)
{
 bool ok = false;
 if(DevInfo && DevNr<UsbDeviceList.size())
  {
   strcpy(&DevInfo->SerialNr[0],UsbDeviceList.at(DevNr).SerialNr.c_str());
   strcpy(&DevInfo->DevDesc[0],UsbDeviceList.at(DevNr).DevDesc.c_str());
   strcpy(&DevInfo->LocationInformation[0],UsbDeviceList.at(DevNr).LocationInformation.c_str());
   strcpy(&DevInfo->PortName[0],UsbDeviceList.at(DevNr).PortName.c_str());
   DevInfo->Connected = UsbDeviceList.at(DevNr).Connected;
   ok = true;
  };
  
 return(ok);
}

/*!
 \brief Init the SA430 Interface Dll

 \fn sa430Init
 \param void
 \return bool
*/
SA430_API bool API_CALL sa430Init(void)
{
 if(flagInit)
   return(false);

 flagConnected = false;
 
 if(!UsbDetect)
   UsbDetect = new cUsbDetect();

 if(!Device)
   Device    = new cDeviceDriver();

 if(Device && UsbDetect)
  {
   if(Device->IsInit())
    {
     flagInit = true;
     return(true);
    };
  };

 return(false);
}

/*!
 \brief Checks if all Objects are created and successfully initialized

 \fn sa430IsInit
 \param void
 \return bool
*/
SA430_API bool API_CALL sa430IsInit(void)
{
 return(flagInit);
}

/*!
 \brief Delete all generated objects

 \fn sa430DeInit
 \param void
*/
SA430_API void API_CALL sa430DeInit(void)
{
 flagInit      = false;
 flagConnected = false;

 if(Device)
  {
   if(Device->IsOpen())
     Device->Close();
   delete Device;
   Device = NULL;
  };
 if(UsbDetect)
  {
   UsbDetect->Off();
   delete UsbDetect;
   UsbDetect = NULL;
  };

}

/*!
 \brief Returns the current device status structure sa430Status

 \fn sa430GetStatus
 \param Status
 \return bool
*/
SA430_API bool API_CALL sa430GetStatus(sa430Status &Status)
{
 if(Device)
  {
   if(UsbDetect)
   {
    Status.flagUsbRemoval = UsbDetect->EventUnplugged();
    if(Status.flagUsbRemoval)
    {
     sa430Disconnect();
     Status.flagUsbRemoval  = true;
    };
   }
   else
   {
    Status.flagUsbRemoval  = false;
   };

   Status.flagInit			= flagInit;
   Status.flagOpen			= flagConnected;
   Status.flagDrvError			= !Device->IsDrvOk();
   Status.flagFrameError		= false;
   Status.flagFrameReceived		= Device->HasFrameReceived();
   Status.flagFrameErrorCrc		= Device->HasFrameCrcError();
   Status.flagFrameErrorTimeOut         = Device->HasFrameTimeoutError();
  }
 else
  {
   Status.flagInit			= false;
   Status.flagOpen			= false;
   Status.flagDrvError			= false;
   Status.flagFrameError		= false;
   Status.flagFrameReceived		= false;
   Status.flagFrameErrorCrc		= false;
   Status.flagFrameErrorTimeOut  = false;
   Status.flagUsbRemoval	 = false;
  };

 return(true);
}

/*!
 \brief Connect dedicated device, given by the comportname

 \fn sa430Connect
 \param strPort
 \return bool
*/
SA430_API bool API_CALL sa430Connect(const char *strPort)
{
 bool ok = false;

 if(flagConnected)
   return(false);

 if(!Device)
  return(false);
 
 ok = Device->Open(strPort);

 if(ok)
  {
   if(UsbDetect->On(strPort))
   {
    ok = true;
	flagConnected = true;
   }
   else
   {
    ok = false;
   };
  };

 return(ok);
}

/*!
 \brief Checks if the comport was successfully opened

 \fn sa430IsConnected
 \param void
 \return bool
*/
SA430_API bool API_CALL sa430IsConnected(void)
{
 bool ok = false;

 if(!flagInit)
   return(false);

 if(Device)
  {
   ok = Device->IsOpen();
  };

 if(ok)
  {
   flagConnected = true;
  };

 return(ok);
}

/*!
 \brief Disconnect active comport

 \fn sa430Disconnect
 \param void
 \return bool
*/
SA430_API bool API_CALL sa430Disconnect(void)
{
 bool ok = false;

 if(flagInit && flagConnected)
  {
   if(Device)
    {
     ok = Device->Close();
    };
  };

 if(ok)
  {
   UsbDetect->Off();
   flagConnected = false;
  };

 return(ok);
}



/*!
 \brief Checks if comport has received new frames from the SA430 device in the meantime

 \fn sa430IsFrameAvailable
 \param void
 \return bool
*/
SA430_API bool API_CALL sa430IsFrameAvailable(void)
{
 bool ok = false;

 if(flagInit && flagConnected)
  {
   if(Device)
    {
     ok = !Device->IsFrameFifoEmpty();
    };
  };

 return(ok);
}

/*!
 \brief Return the actuall number of received frames within the Frame Fifo Buffer

 \fn sa430GetFrameFifoSize
 \param Size
 \return bool
*/
SA430_API bool API_CALL sa430GetFrameFifoSize(unsigned short &Size)
{
 bool ok = false;

 if(flagInit && flagConnected)
  {
   if(Device)
    {
     ok = Device->GetFrameFifoSize(Size);
    };
  };

 return(ok);
}

/*!
 \brief Return the topmost frame from the Frame Fifo Buffer

 \fn sa430GetFrame
 \param Frame
 \return bool
*/
SA430_API bool API_CALL sa430GetFrame(sa430Frame *Frame)
{
 bool ok = false;
 sFrame srcFrame;
 if(flagInit && flagConnected)
  {
   if(Device)
    {
     if(Device->GetFrame(&srcFrame))
	 {
	  Frame->Cmd		= srcFrame.Cmd;
	  Frame->Crc		= srcFrame.Crc;
	  Frame->Length	= srcFrame.Length;
	  for(unsigned int index=0;index<srcFrame.Data.size();index++)
	  {
	   Frame->Data[index] = srcFrame.Data[index];
	  };
	  ok = true;
	 };
    };
  };

 return(ok);
}

/*!
 \brief Sends a Frame to the SA430 Device

 \fn sa430SendCmd
 \param Cmd
 \param Data
 \param Size
 \return bool
*/
SA430_API bool API_CALL sa430SendCmd(unsigned char Cmd, unsigned char *Data, unsigned short Size)
{
 bool ok = false;

 if(flagInit && flagConnected)
  {
   if(Device)
    {
     ok = Device->SendFrame(Cmd,Data,(unsigned long)Size);
    };
  };

 return(ok);
}



