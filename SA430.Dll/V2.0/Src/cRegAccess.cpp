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
#include "sa430TypeDef.h"
#include "sa430.h"
#include "cdevicedriver.h"
#include "cUsbDetect.h"
#include "cRegAccess.h"
#include <string>
#include <list>
#include <vector>

using namespace std;

/*!
 \brief Constructor

 \fn cRegAccess::cRegAccess
 \param void
*/
cRegAccess::cRegAccess(void)
{

}

/*!
 \brief Destructor

 \fn cRegAccess::~cRegAccess
 \param void
*/
cRegAccess::~cRegAccess(void)
{
}


/*!
 \brief Load the active comport list from the Windows OS Registry

 \fn cRegAccess::LoadComPortList
 \param comPortList
 \return bool
*/
bool cRegAccess::LoadComPortList(lComPortList *comPortList)
{
 bool ok = false;
 HKEY hKey;

 if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",0,KEY_READ,&hKey)!=ERROR_SUCCESS)
 {
  ok = false;
 }
 else
 {
  DWORD cSubKeys=0;           // number of subkeys
  DWORD cbMaxSubKey;          // longest subkey size
  DWORD cchMaxClass;          // longest class string
  DWORD cValues;              // number of values for key
  DWORD cchMaxValue;          // longest value name
  DWORD cbMaxValueData;       // longest value data
  DWORD cbSecurityDescriptor; // size of security descriptor
  DWORD MaxDataSize=0;
  DWORD i;
  DWORD retCode;
  TCHAR achValue[MAX_VALUE_NAME];
  DWORD cchValue = MAX_VALUE_NAME;
  sComPortDescr ComPortDescr;

  comPortList->clear();

  retCode = RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeys,&cbMaxSubKey,&cchMaxClass,&cValues,&cchMaxValue,&cbMaxValueData,&cbSecurityDescriptor,NULL);       // last write time
  if(cValues)
  {
   for(i=0, retCode=ERROR_SUCCESS; i<cValues; i++)
   {
    ComPortDescr.Name.clear();
    ComPortDescr.Nr.clear();
    cchValue = MAX_VALUE_NAME;
    achValue[0] = '\0';
    retCode = RegEnumValue(hKey, i,achValue,&cchValue,NULL,NULL,NULL,&MaxDataSize);
    ComPortDescr.Name.append(&achValue[0]);
    ComPortDescr.Nr.resize(MaxDataSize);
    retCode = RegEnumValue(hKey, i,achValue,&cchValue,NULL,NULL,(unsigned char *)&ComPortDescr.Nr[0],&MaxDataSize);
    if(retCode == ERROR_SUCCESS || retCode==ERROR_MORE_DATA)
    {
     comPortList->push_back(ComPortDescr);
     ok = true;
    };
   };
  };
 };
 RegCloseKey(hKey);

 return(ok);
}

/*!
 \brief Return all installed SA430 Device informations

 \fn cRegAccess::LoadRegDeviceList
 \param usbDeviceList
 \return bool
*/
bool cRegAccess::LoadRegDeviceList(vUsbDeviceList *usbDeviceList)
{
 bool ok = false;
 HKEY hKey;

 usbDeviceList->clear();

 if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum\\USB\\Vid_2047&Pid_0005",0,KEY_READ,&hKey)!=ERROR_SUCCESS)
 {
  ok = false;
 }
 else
 {
  TCHAR achKey[MAX_KEY_LENGTH];   // buffer for subkey name
  TCHAR achClass[MAX_PATH] = TEXT("");  // buffer for class name
  DWORD cchClassName = MAX_PATH;  // size of class string
  DWORD cSubKeys=0;               // number of subkeys
  DWORD cbName;                   // size of name string
  DWORD cbMaxSubKey;              // longest subkey size
  DWORD cchMaxClass;              // longest class string
  DWORD cValues;              // number of values for key
  DWORD cchMaxValue;          // longest value name
  DWORD cbMaxValueData;       // longest value data
  DWORD cbSecurityDescriptor; // size of security descriptor
  DWORD i;
  DWORD retCode;
  sUsbDeviceInfo usbDeviceInfo;

  usbDeviceList->clear();
    
  retCode = RegQueryInfoKey(hKey,achClass,&cchClassName,NULL,&cSubKeys,&cbMaxSubKey,&cchMaxClass,&cValues,&cchMaxValue,&cbMaxValueData,&cbSecurityDescriptor,NULL);       // last write time
  if(cSubKeys)
  {
   for(i=0;i<cSubKeys;i++)
   {
    cbName = MAX_KEY_LENGTH;
    retCode = RegEnumKeyEx(hKey, i,achKey, &cbName,NULL,NULL, NULL,NULL);
    if(retCode == ERROR_SUCCESS)
    {
     usbDeviceInfo.SerialNr.clear();
     usbDeviceInfo.SerialNr.append(&achKey[0]);
     usbDeviceList->push_back(usbDeviceInfo);
     ok = true;
    };
   };
  };
 };
 RegCloseKey(hKey);

 return(ok);
}

/*!
 \brief Compare the USB Device information list with given comport name

 \fn cRegAccess::GetPortSerial
 \param port
 \param UsbSerial
 \return bool
*/
bool cRegAccess::GetPortSerial(std::string port, std::string & UsbSerial)
{
 bool ok = false;
 int result;
 vUsbDeviceList UsbDeviceList;
 LoadRegDeviceList(&UsbDeviceList);

 for(vector<sUsbDeviceInfo>::iterator iComPort=UsbDeviceList.begin();iComPort<UsbDeviceList.end();iComPort++)
 {
  LoadRegDeviceDetails(iComPort);
  result = iComPort->PortName.find(port.data());
  if(result==0)
  {
   UsbSerial = iComPort->SerialNr;
   ok = true;
   break;
  };
 };
 return(ok);
}


/*!
 \brief Returns the dedicated SA430 USB device information

 \fn cRegAccess::LoadRegDeviceDetails
 \param item
 \return bool
*/
bool cRegAccess::LoadRegDeviceDetails(vUsbDeviceList::iterator item)
{
 bool ok = false;
 HKEY hKey;
 list<sRegEntry> RegEntryList;
 std::string strReg;

 strReg = "SYSTEM\\CurrentControlSet\\Enum\\USB\\Vid_2047&Pid_0005\\";
 strReg += item->SerialNr;
 if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,strReg.c_str(),0,KEY_READ,&hKey)!=ERROR_SUCCESS)
 {// No installed device found
  return(false);
 };
 if(!RegGetKeyValues(hKey,&RegEntryList))
 {// No Reg Entry found
  return(false);
 };
 for(list<sRegEntry>::iterator itemReg = RegEntryList.begin(); itemReg != RegEntryList.end(); itemReg++)
 {
  if(itemReg->strKey.compare("DeviceDesc")==0)
  {
   item->DevDesc = itemReg->strValue;
   ok = true;
  }
  else
  if(itemReg->strKey.compare("LocationInformation")==0)
  {
   item->LocationInformation = itemReg->strValue;
   ok=true;
   break;
  };
 };
 RegCloseKey(hKey);

 if(ok)
 {
  ok = false;
  strReg = "SYSTEM\\CurrentControlSet\\Enum\\USB\\Vid_2047&Pid_0005\\";
  strReg += item->SerialNr;
  strReg += "\\Device Parameters";
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,strReg.c_str(),0,KEY_READ,&hKey)!=ERROR_SUCCESS)
  {// No dedicated key found
   return(false);
  };
  RegEntryList.clear();
  if(!RegGetKeyValues(hKey,&RegEntryList))
  {// No Reg Entry found
   return(false);
  };
  for(list<sRegEntry>::iterator itemReg = RegEntryList.begin(); itemReg != RegEntryList.end(); itemReg++)
  {
   if(itemReg->strKey.compare("PortName")==0)
   {
    item->PortName= itemReg->strValue;
    ok = true;
    break;
   };
  };
  RegCloseKey(hKey);
 };

 return(ok);
}


/*!
 \brief Returns the specific Windows Registry Key value(s)

 \fn cRegAccess::RegGetKeyValues
 \param hKey
 \param regEntryList
 \return bool
*/
bool cRegAccess::RegGetKeyValues(HKEY hKey,list<sRegEntry> *regEntryList)
{
 bool ok = false;
 TCHAR achClass[MAX_PATH] = TEXT("");  // buffer for class name 
 DWORD cchClassName = MAX_PATH;  // size of class string 
 DWORD cSubKeys=0;               // number of subkeys 
 DWORD cbMaxSubKey;              // longest subkey size 
 DWORD cchMaxClass;              // longest class string 
 DWORD cValues;              // number of values for key 
 DWORD cchMaxValue;          // longest value name 
 DWORD cbMaxValueData;       // longest value data 
 DWORD cbSecurityDescriptor; // size of security descriptor 
 DWORD MaxDataSize=0;
 DWORD i;
 DWORD retCode; 
 DWORD cchValue = MAX_VALUE_NAME; 
 TCHAR  achValue[MAX_VALUE_NAME];
 sRegEntry RegEntry;
 
 retCode = RegQueryInfoKey(hKey,achClass,&cchClassName,NULL,&cSubKeys,&cbMaxSubKey,&cchMaxClass,&cValues,&cchMaxValue,&cbMaxValueData,&cbSecurityDescriptor,NULL);       // last write time 
 if(cValues) 
 {
  for(i=0,retCode=ERROR_SUCCESS;i<cValues;i++)
  {
   cchValue = MAX_VALUE_NAME;
   achValue[0] = '\0';
   retCode = RegEnumValue(hKey, i,achValue,&cchValue,NULL,NULL,NULL,&MaxDataSize);
   RegEntry.strKey.clear();
   RegEntry.strKey.append(&achValue[0]);
   RegEntry.strValue.resize(MaxDataSize);
   retCode = RegEnumValue(hKey, i,achValue,&cchValue,NULL,NULL,(unsigned char *)&RegEntry.strValue[0],&MaxDataSize);
   if(retCode == ERROR_SUCCESS || retCode==ERROR_MORE_DATA)
   {
    regEntryList->push_back(RegEntry);
    ok = true;
   };
  };
 };

 return(ok);
}


