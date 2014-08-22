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
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <dbt.h>
#include <setupapi.h>
#include "cevent.h"
#include "cRegAccess.h"
#include "cthread.h"
#include "sa430TypeDef.h"

using namespace std;

#include "cUsbDetect.h"

HWND  hwnd = NULL;

static std::string strSerial;

/*!
 \brief

 \fn WindowProcedure
 \param hWnd
 \param uiMsg
 \param wParam
 \param lParam
*/
static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

/*!
 \brief

 \fn cUsbDetect::On
 \param strComPort
 \return bool
*/
bool cUsbDetect::On(std::string strComPort)
{
 bool ok = false;
 
 if(!flagCreated || strComPort.empty())
  return(false);

 if(RegAccess.GetPortSerial(strComPort,strComPortSerial))
  ok = true;

 if(ok)
 {
  strComPortName   = strComPort;
  ok = true;
 };

 return(ok);
}

/*!
 \brief

 \fn cUsbDetect::Off
 \param void
 \return bool
*/
bool cUsbDetect::Off(void)
{
 bool ok = false;

 if(!flagCreated)
   {
    ok = false;
   }
  else
   {
    strComPortName.clear();
	strComPortSerial.clear();
	eUnpluggedEvent->Reset();
    ok = true;
   };

 return(ok);
}



/*!
 \brief

 \fn cUsbDetect::cUsbDetect
 \param void
*/
cUsbDetect::cUsbDetect(void)
{
 flagCreated = false;

 // Init Static Variables
 className = "Ports";
 // Init Wincl Structure
 ZeroMemory(&wincl, sizeof(wincl));
 wincl.hInstance     = GetModuleHandle(0);
 wincl.lpszClassName = className;
 wincl.lpfnWndProc   = WindowProcedure;

 // Init Notofication Filter
 ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
 NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
 NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
 NotificationFilter.dbcc_classguid	= GuidDevInterfaceList[0];
 NotificationFilter.dbcc_name[0]	= '\0';

 hDeviceNotify = NULL;
 hwnd		   = NULL;
 hparent	   = HWND_MESSAGE;
 eUnpluggedEvent = NULL;

 // Init Thread
 flagExitThread = false;
 
 // Create and Init Objects
 eUnpluggedEvent = new cEvent(false);

 if(eUnpluggedEvent)
  {
   RegisterClass(&wincl);
   ThreadHandle   = new TThread<cUsbDetect>(*this,&cUsbDetect::run);
    if(ThreadHandle)
     {
      flagCreated = true;
     };
   };

 if(flagCreated)
  {
   ThreadHandle->StartAndWait();
   if(ThreadHandle->IsRunning())
     flagCreated = true;
    else
     flagCreated = false;  
  };
}

/*!
 \brief

 \fn cUsbDetect::~cUsbDetect
 \param void
*/
cUsbDetect::~cUsbDetect(void)
{
 PostMessage(hwnd,WM_USER+3,NULL,NULL);
 flagExitThread = true;
 ThreadHandle->WaitUntilTerminate();
 delete ThreadHandle;
 delete eUnpluggedEvent;
}

/*!
 \brief

 \fn WindowProcedure
 \param hWnd
 \param uiMsg
 \param wParam
 \param lParam
 \return LRESULT
*/
static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  PDEV_BROADCAST_DEVICEINTERFACE pdbch=NULL;
 switch (uiMsg)
  {
   case WM_DEVICECHANGE:
		 switch(wParam)
		  {
		   case DBT_DEVICEARRIVAL:
                        PostMessage(hwnd,WM_USER+1,wParam,lParam);
                       break;
		   case DBT_DEVICEREMOVECOMPLETE:
                        pdbch = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
                        strSerial = std::string(pdbch->dbcc_name);
                        PostMessage(hwnd,WM_USER+2,wParam,lParam);
                       break;
		   default:
                       break;
		 };
                break;
   case WM_QUIT:
		 PostMessage(hwnd,WM_USER+3,wParam,lParam);
                break;
    default:
                 return DefWindowProc(hWnd, uiMsg, wParam, lParam);
		break;
  };

 return(0);
}


/*!
 \brief

 \fn cUsbDetect::run
*/
void cUsbDetect::run()
{
 int Ret;
 MSG Msg;
 
 bool flagOk = false;
 hparent = HWND_MESSAGE;
 hwnd = CreateWindowEx(WS_EX_TOPMOST, className, className,0, 0, 0, 0, 0, hparent, 0, 0, 0);
 if(hwnd)
  {
   hDeviceNotify = RegisterDeviceNotification(hwnd, &NotificationFilter,DEVICE_NOTIFY_WINDOW_HANDLE);
   if(hDeviceNotify)
    {
     flagOk = true;
    };
  };

 if(flagOk)
 do
 {
  Ret=GetMessage(&Msg,hwnd,0,0);
  if(Ret == -1 || Ret == 0)
   {
    flagExitThread = true;
    break;
   };
   switch(Msg.message)
   {
    case WM_USER+1:
		 // Pluggin of a usb device can be handled here
		break;
	case WM_USER+2:
		Ret = strSerial.find(strComPortSerial.data()); 
		if(Ret>0)
			{
			 strSerial.clear();
			 eUnpluggedEvent->Signal();
			}; 
	   break;
    case WM_USER+3:
		flagExitThread = true;
       break;
	 default:
	    DispatchMessage(&Msg);
	   break;
   };

 }while(!flagExitThread);
}


/*!
 \brief

 \fn cUsbDetect::IsActivePortConnected
 \param void
 \return bool
*/
bool cUsbDetect::IsActivePortConnected(void)
{
 bool ok = false;
 HDEVINFO			hDevInfo	  = INVALID_HANDLE_VALUE;
 HKEY				hDeviceRegKey; 
 int				DeviceIndex   = 0;
 SP_DEVINFO_DATA	DeviceInfoData;
 char pszPortName[256];
 DWORD dwSize = sizeof(pszPortName);
 DWORD dwType = 0;

 hDevInfo = SetupDiGetClassDevs(&GuidDevInterfaceList[0],NULL,NULL,DIGCF_ALLCLASSES | DIGCF_PRESENT);

 ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
 DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
 DeviceIndex = 0;
    
 while (SetupDiEnumDeviceInfo(hDevInfo,DeviceIndex,&DeviceInfoData)) 
 {
  DeviceIndex++;
  hDeviceRegKey = SetupDiOpenDevRegKey(hDevInfo, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
  //Read in the name of the port

  if((RegQueryValueEx(hDeviceRegKey,"PortName", NULL, &dwType, reinterpret_cast<LPBYTE>(pszPortName), &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
  {
   if(strComPortName.compare(pszPortName)== 0)
   {
    ok = true;
    RegCloseKey(hDeviceRegKey);
    break;
   };
  };
  RegCloseKey(hDeviceRegKey);
 };
 if(hDevInfo)
 {
  SetupDiDestroyDeviceInfoList(hDevInfo);
 };

 return(ok);
}

/*!
 \brief

 \fn cUsbDetect::EventUnplugged
 \param void
 \return bool
*/
bool cUsbDetect::EventUnplugged(void)
{
 if(strComPortName.empty())
 {
  return(false);
 };

 return(eUnpluggedEvent->Check());
}

/*!
 \brief

 \fn cUsbDetect::IsSame
 \param strTest
 \return bool
*/
bool cUsbDetect::IsSame(std::string *strTest)
{
 if(strComPortName.compare(strTest->c_str())==0)
 {
  return(true);
 };

 return(false);
}
