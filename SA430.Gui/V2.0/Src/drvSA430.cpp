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
#include "drvSA430.h"
#include <math.h>


drvSA430::drvSA430()
{
 Status.flagInit             = false;
 flagThreadExit              = false;

 Status.flagSpecTrigger      = false;
 Status.flagSpecIsBusy       = false;
 Status.flagSpecNewParameter = false;

 Status.flagCalDataLoaded    = false;

 signalDeviceOpen            = new cEvent(true);

 signalSpecIsBusy            = new cEvent(true);
 signalSpecNewParameter      = new cEvent(true);
 signalSpecTrigger           = new cEvent(true);

 currentSpectrumId = 0;
 DecoderSpectrumBuffer.clear();
 SpectrumBuffer.clear();

 sa430Init();
 if(sa430IsInit())
 {
  Status.flagInit = true;
  // Start Thread
  this->start();
 };
}

drvSA430::~drvSA430()
{
 if(Status.flagInit)
 {
  flagThreadExit = true;
  while(!this->isFinished());
  sa430DeInit();
  Status.flagInit = false;
 };

 delete signalDeviceOpen;
 delete signalSpecIsBusy;
 delete signalSpecNewParameter;
 delete signalSpecTrigger;
}


// Public Function Defintion
unsigned short drvSA430::GetDllVer(void)
{
 return(sa430GetDllVersion());
}

bool drvSA430::deviceSearch(unsigned short &NumDevs)
{
 bool ok = false;
 DrvAccess.lock();
 ok = sa430FindDevices(NumDevs);
 DrvAccess.unlock();
 return(ok);
}

bool drvSA430::deviceGetInfo(unsigned short DevNr,sa430UsbDevice *DevInfo)
{
 bool ok = false;
 DrvAccess.lock();
 ok = sa430GetDeviceInfo(DevNr,DevInfo);
 DrvAccess.unlock();
 return(ok);
}



bool drvSA430::deviceConnect(sa430UsbDevice *usbInfo)
{
 if(!usbInfo)
   return(false);

 if(!sa430IsInit())
 {
  emit signalErrorMsg("SA430 DLL Not initialized !!");
  return(false);
 };

 if(sa430IsConnected())
 {
  emit signalErrorMsg("One Device is already Connected!!");
  return(false);
 };

 if(sa430Connect(usbInfo->PortName))
 {
  Status.activeUsbInterface = *usbInfo;
  Status.flagDevConnected   = true;

  signalSpecTrigger->Reset();
  signalSpecNewParameter->Reset();
  signalSpecIsBusy->Reset();

  Status.flagSpecTrigger       = false;
  Status.flagSpecIsBusy        = false;
  Status.flagSpecNewParameter  = false;


  currentSpectrumId     = 0;
  DecoderSpectrumBuffer.clear();
  SpectrumBuffer.clear();

  signalDeviceOpen->Signal();
  return(true);
 }
 else
 {
  deviceDisconnect();
  emit signalErrorMsg(QString("Failed to open Port: %0").arg(usbInfo->PortName));
 };
 return(false);
}


bool drvSA430::deviceGetDevInfo(sDeviceInfo *devInfo)
{
 bool done = false;
 if(!devInfo || !Status.flagDevConnected)
  return(done);

 *devInfo = Status.activeDeviceInfo;

 done = true;

 return(done);
}

bool drvSA430::deviceGetUsbInfo(sa430UsbDevice *usbInfo)
{
 bool done = false;

 if(!usbInfo || !Status.flagDevConnected)
   return(done);

 *usbInfo = Status.activeUsbInterface;

 done = true;

 return(done);
}

bool drvSA430::deviceGetCalData(sCalibrationData *calData)
{
 bool done = false;

 if(!calData || !Status.flagDevConnected)
   return(done);

 if(Status.flagCalDataLoaded)
 {
  *calData = Status.activeCalData;
  done = true;
 };

 return(done);
}


bool drvSA430::deviceDisconnect(void)
{
 bool done = false;

 if(signalDeviceOpen->Check())
 {
  if(sa430IsConnected())
   sa430Disconnect();
  signalDeviceOpen->Reset();
  Status.flagDevConnected = false;
  Status.flagCalDataLoaded= false;
  emit signalDisconnected();
  done = true;
 };

 return(done);
}

bool drvSA430::spectrumTriggerOn(void)
{
 bool done = false;
 Status.flagSpecTrigger = true;

 return(done);
}

bool drvSA430::spectrumTriggerOff(void)
{
 bool done = false;
 Status.flagSpecTrigger = false;

 return(done);
}

bool drvSA430::spectrumSetParameter(sFrqValues *FrqValues)
{
 bool done = false;
 if(signalDeviceOpen->Check())
 {
  Status.activeFrqValues = *FrqValues;

  Status.flagSpecNewParameter = true;
  done = true;
 };

 return(done);
}

bool drvSA430::spectrumGetParameter(sFrqValues *FrqValues, sFrqValues *FrqCorrected)
{
 bool done = false;
 if(signalDeviceOpen->Check())
 {
  *FrqValues    = Status.activeFrqValues;
  *FrqCorrected = Status.activeFrqValuesCorrected;
  done = true;
 };

 return(done);
}


bool drvSA430::spectrumGetData(sSpectrum *Spectrum)
{
 bool done = false;
 if(!Spectrum || SpectrumBuffer.isEmpty())
   return(done);

 *Spectrum =   SpectrumBuffer.first();
 SpectrumBuffer.pop_front();
 done = true;

 return(done);
}


// Public Signals Function Definition

// Public Slot Function Definiton

// Protected Function Defintion


// Private Function Defintion


// Private SA430 Function Defintion
bool drvSA430::IsFrameAvailable(void)
{
 bool ok = false;
 DrvAccess.lock();
 ok = sa430IsFrameAvailable();
 DrvAccess.unlock();
 return(ok);
}

bool drvSA430::GetFrame(sa430Frame *frame)
{
 bool ok = false;
 DrvAccess.lock();
 ok = sa430GetFrame(frame);
 DrvAccess.unlock();
 return(ok);
}



// Private Task Function Defintion
bool drvSA430::stateInit(void)
{
 if(sa430IsInit())
  {
   qDebug()<<"drvSA430: Init";
   State = STATE_OPEN;
  }
 else
  {
   qDebug()<<"drvSA430: Failed to Init";
   emit signalErrorMsg("sa430Drv: Failed to init Interface Dll");
  };
 return(false);
}



bool drvSA430::stateOpen(void)
{
 if(signalDeviceOpen->CheckSignal(1))
 {
  qDebug()<<"drvSA430: Is Open";
  State = STATE_SETUP;
 };
 return(false);
}

bool drvSA430::stateSetup(void)
{
 bool done = false;

 // Clear Settings
 Status.activeDeviceInfo.CoreVersion = 0;
 Status.activeDeviceInfo.ProdVersion = 0;
 Status.activeDeviceInfo.SpecVersion = 0;
 Status.activeDeviceInfo.FrqXtal     = 0;
 Status.activeDeviceInfo.SerialNumber= 0;
 Status.activeDeviceInfo.IDN.clear();
 // Core Function First
 if(cmdGetCoreVersion(&Status.activeDeviceInfo.CoreVersion))
 {
  if(cmdGetHwID(&Status.activeDeviceInfo.SerialNumber))
  {
   if(cmdGetIDN(&Status.activeDeviceInfo.IDN))
   {
    // Spectrum Function Last
    if(cmdModeSpectrumInit())
    {
     if(cmdGetSpecVersion(&Status.activeDeviceInfo.SpecVersion))
     {
      if(FwVersionIsOk())
      {
       if(cmdLoadCalData(&Status.activeCalData))
       {
        Status.flagCalDataLoaded = true;
        done = true;
       }
       else
       {
        emit signalErrorMsg("Failed to load Calibration Data");
       };
      }
      else
      {
       emit signalErrorMsg("Wrong Firmware/Core Version");
      };
     };
    }
    else
    {
     if(cmdGetIDN(&Status.activeDeviceInfo.IDN))
     {
      emit signalDeviceUpdateRequired(QString(Status.activeUsbInterface.PortName));
     };
    };
   };
  };
 };

 if(done)
 {
  cmdBlink();
  State = STATE_RUN;
  emit signalConnected();
 }
 else
 {
  State = STATE_OPEN;
  deviceDisconnect();
 };

 return(done);
}

bool drvSA430::stateExit(void)
{

 return(false);
}

bool drvSA430::stateRun(void)
{
 if(!Status.flagSpecIsBusy)
 {// Ready to set new spectrum parameter
  if(Status.flagSpecNewParameter)
  {
   if(cmdSetFrq(&Status.activeFrqValues,&Status.activeFrqValuesCorrected))
   {// Successfully set new spectrum parameter
    DecoderSpectrumBuffer.clear();
    currentSpectrumId++;
    emit signalNewParameterSet(true,currentSpectrumId);
    Status.flagSpecNewParameter=false;
    cmdGetSpectrum();
    Status.flagSpecIsBusy=true;
   }
   else
   {// Failed to set new spectrum parameter
    emit signalNewParameterSet(false,0);
    emit signalErrorMsg("Failed to set new spectrum parameter !!");
   };
   Status.flagSpecNewParameter = false;
  }
  else
  {// Ready to trigger new spectrum if requested
   if(Status.flagSpecTrigger)
   {
    Status.flagSpecTrigger = false;
    Status.flagSpecIsBusy  = true;
    cmdGetSpectrum();

   };
  };
 }
 else
 {// Wait for requested spectrum data
  if(sa430IsFrameAvailable())
   {
    DecoderFrame.Cmd = 0xff;
    DecoderFrame.Crc = 0;
    DecoderFrame.Length = 0;
    memset(&DecoderFrame.Data[0],0,FRAME_MAX_DATA_LENGTH);
    if(sa430GetFrame(&DecoderFrame))
    {
     switch(DecoderFrame.Cmd)
     {
      case CMD_GETSPECNOINIT:
             DecoderSpectrumBuffer.append(DecoderFrame);
            break;
      case CMD_GETLASTERROR:
             if(DecoderFrame.Length==2)
             {// End of requeted spectrum
              //  if(DecoderFrame.Data[0]==0)
              //   if(DecoderFrame.Data[1]==0)
              //{
                Status.flagSpecIsBusy = false;
                specSave(&DecoderSpectrumBuffer);
                DecoderSpectrumBuffer.clear();
              // };
             };
         break;
        default:
          break;
     };
    };
   };
 };
 return(false);
}

void drvSA430::run(void)
{
 flagThreadExit = false;
 State= STATE_INIT;
 sa430Status Status;

 qDebug()<<"drvSA430: Start Driver Thread";
 do
 {
  switch(State)
  {
   case STATE_INIT:
              stateInit();
            break;
   case STATE_OPEN:
              stateOpen();
            break;
   case STATE_SETUP:
              stateSetup();
           break;
   case STATE_RUN:
              stateRun();
            break;
   case STATE_EXIT:
              qDebug()<<"drvSA430: Exit Driver Task";
              stateExit();
            break;

        default:
            break;
  };
 if(!signalDeviceOpen->Check())
    State = STATE_OPEN;


 if(sa430GetStatus(Status))
 {
  if(Status.flagUsbRemoval)
  {
    deviceDisconnect();
    emit signalErrorMsg("Device removed from USB Interface !!");
   //emit signalDisconnected();
   State = STATE_OPEN;
  };
 };

 this->msleep(1);
}while(!flagThreadExit);

 qDebug()<<"drvSA430: Exit Driver Thread";

}


// SA430 Command Function Definition


bool drvSA430::cmdWaitForConfirmation(unsigned char Cmd,unsigned long ms)
{
 bool done = false;
 sa430Frame rcvFrame;

 ms = ms;

 do
 {
  if(sa430IsFrameAvailable())
   if(sa430GetFrame(&rcvFrame))
    if(rcvFrame.Cmd == Cmd && rcvFrame.Length == 0)
     done = true;

  if(ms==0 && !done)
   return(false);
  usleep(1000);
  ms--;
 }while(!done);

 return(done);
}

bool drvSA430::cmdWaitForData(unsigned char Cmd,sa430Frame *dataFrame, unsigned long ms)
{
 bool done = false;
 sa430Frame tmpFrame;

 do
 {
  if(sa430IsFrameAvailable())
   if(sa430GetFrame(&tmpFrame))
    if(tmpFrame.Cmd == Cmd)
     if(tmpFrame.Length)
     {
      *dataFrame = tmpFrame;
      done = true;
     };
  if(ms==0 && !done)
   return(false);
  usleep(1000);
  ms--;
 }while(!done);

 return(done);
}

bool drvSA430::cmdSetU8(unsigned char Cmd,unsigned char U8)
{
 bool done = false;

 unsigned char  u8 = U8;

 if(sa430SendCmd(Cmd,&u8,1))
 {
  if(cmdWaitForConfirmation(Cmd,100))
  {
   done = true;
  }
 };

 if(!done)
 {
  U8 = 0;
 }
 else
 {
  U8 = u8;
 };

 return(done);
}

bool drvSA430::cmdSetU16(unsigned char Cmd,unsigned short U16)
{
 bool done = false;

 unsigned char  u8[2];

 u8[0] = (unsigned char) (U16>>8    );
 u8[1] = (unsigned char) (U16 & 0xff);

 if(sa430SendCmd(Cmd,&u8[0],2))
  if(cmdWaitForConfirmation(Cmd,100))
   done = true;

 if(!done)
  U16 = 0;

 return(done);
}

bool drvSA430::cmdGetU16(unsigned char Cmd, unsigned short *U16)
{
 bool done = false;
 sa430Frame  rxData;

 *U16 = 0;

 if(sa430SendCmd(Cmd,NULL,0))
  if(cmdWaitForConfirmation(Cmd,100))
   if(cmdWaitForData(Cmd,&rxData,100))
   {
    *U16  = rxData.Data[0] << 8;
    *U16 |= rxData.Data[1];
    done = true;
   };

 return(done);
}

bool drvSA430::cmdSetU32(unsigned char Cmd,unsigned long U32)
{
 bool done = false;
 unsigned char u8[4];

 u8[0] =(unsigned char)(U32>>24);
 u8[1] =(unsigned char)(U32>>16);
 u8[2] =(unsigned char)(U32>> 8);
 u8[3] =(unsigned char)(U32);

 if(sa430SendCmd(Cmd,&u8[0],4))
  if(cmdWaitForConfirmation(Cmd,100))
   done = true;

 return(done);
}

bool drvSA430::cmdGetU32(unsigned char Cmd, unsigned long *U32)
{
 bool done = false;
 sa430Frame  rxData;

 *U32 = 0;

 if(sa430SendCmd(Cmd,NULL,0))
  if(cmdWaitForConfirmation(Cmd,100))
   if(cmdWaitForData(Cmd,&rxData,100))
   {
    *U32  = (unsigned long)(((unsigned char)rxData.Data[0]) << 24);
    *U32 |= (unsigned long)(((unsigned char)rxData.Data[1]) << 16);
    *U32 |= (unsigned long)(((unsigned char)rxData.Data[2]) << 8);
    *U32 |= (unsigned long)(((unsigned char)rxData.Data[3]));
    done = true;
   };

 return(done);
}

bool drvSA430::CmdSetX(unsigned char Cmd, unsigned char *ptrData, int Size)
{
 bool done = false;

 if(sa430SendCmd(Cmd,ptrData,Size))
 {
  if(cmdWaitForConfirmation(Cmd,100))
  {
   done = true;
  };
 };

 return(done);
}

// SA430 Upper API Calls Function Definition

bool drvSA430::cmdBlink(void)
{
 bool done = false;

 if(sa430SendCmd(CMD_BLINKLED,NULL,0))
 {
  if(cmdWaitForConfirmation(CMD_BLINKLED,100))
  {
   done = true;
  };
 };
 return(done);
}

bool drvSA430::cmdSync(void)
{
 bool done = false;

 if(sa430SendCmd(CMD_SYNC,NULL,0))
  if(cmdWaitForConfirmation(CMD_SYNC,100))
   done = true;

 return(done);
}

bool drvSA430::cmdGetTLV(QByteArray &TlvData)
{
 bool done = false;
 sa430Frame  rxData;

 TlvData.clear();

 if(sa430SendCmd(CMD_GETCHIPTLV,NULL,0))
  if(cmdWaitForConfirmation(CMD_GETCHIPTLV,100))
   if(cmdWaitForData(CMD_GETCHIPTLV,&rxData,100))
   {
    for(int index=0;index<rxData.Length;index++)
     TlvData.append((char)rxData.Data[index]);
    done = true;
   };

 return(done);
}

bool drvSA430::cmdGetTemp(QByteArray &TempData)
{
 bool done = false;
 sa430Frame  rxData;

 TempData.clear();

 if(sa430SendCmd(CMD_GETTEMP,NULL,0))
  if(cmdWaitForConfirmation(CMD_GETTEMP,100))
   if(cmdWaitForData(CMD_GETTEMP,&rxData,100))
   {
    for(int index=0;index<rxData.Length;index++)
     TempData.append((char)rxData.Data[index]);
    done = true;
   };

 return(done);
}

bool drvSA430::cmdGetXTal(unsigned long *XTal)
{
 bool done = false;

 if(cmdGetU32(CMD_GETFXTAL,XTal))
  done = true;

 return(done);
}

bool drvSA430::cmdGetHwID(unsigned long *HwID)
{
 bool done = false;

 if(cmdGetU32(CMD_GETHWSERNR,HwID))
  done = true;

 return(done);
}

bool drvSA430::cmdGetCoreVersion(unsigned short *VerCore)
{
 bool done = false;

 *VerCore = 0;

 if(cmdGetU16(CMD_GETCOREVER,VerCore))
  done = true;

 return(done);
}


bool drvSA430::cmdGetSpecVersion(unsigned short *VerSpec)
{
 bool done = false;

 *VerSpec = 0;

 if(cmdGetU16(CMD_GETSPECVER,VerSpec))
  done = true;

 return(done);
}

bool drvSA430::cmdGetProdVersion(unsigned short *VerProd)
{
 bool done = false;

 *VerProd = 0;

 if(cmdGetU16(CMD_GETPRODVER,VerProd))
  done = true;

 return(done);
}

bool drvSA430::cmdGetIDN(QString *strIDN)
{
 bool done = false;
 sa430Frame rcvFrame;

 strIDN->clear();

 if(sa430SendCmd(CMD_GETIDN,NULL,0))
 {
  if(cmdWaitForConfirmation(CMD_GETIDN,100))
  {
   if(cmdWaitForData(CMD_GETIDN,&rcvFrame,100))
   {
    rcvFrame.Data[rcvFrame.Length]=0;
    strIDN->append((const char*)&rcvFrame.Data[0]);
    done = true;
   };
  };
 };

 return(done);
}

bool drvSA430::cmdModeSpectrumInit(void)
{
 bool done = false;

 if(sa430SendCmd(CMD_INITPARAMETER,NULL,0))
 {
  if(cmdWaitForConfirmation(CMD_INITPARAMETER,100))
  {
   done = true;
  };
 };

 return(done);
}

bool drvSA430::cmdGetSpectrum(void)
{
 bool done = false;

 if(sa430SendCmd(CMD_GETSPECNOINIT,NULL,0))
 {
  if(cmdWaitForConfirmation(CMD_GETSPECNOINIT,100))
  {
   done = true;
  };
 };

 return(done);
}


bool drvSA430::cmdLoadCalData(sCalibrationData *CalData)
{
 bool          done = false;
 QByteArray  dataBuffer;
 sProgHeader ProgHeader;

 if(!CalData)
  return(done);

 dataBuffer.clear();

 // Try to load Calibration ProgHeader
 if(cmdFlashRead(CALDATA_FLASH_START,&dataBuffer,(unsigned short)sizeof(sProgHeader)))
 {
  ProgHeader.MemStartAddr = (unsigned short)(dataBuffer[0] + (dataBuffer[1]<<8));
  ProgHeader.MemLength    = (unsigned short)(dataBuffer[2] + (dataBuffer[3]<<8));
  ProgHeader.MemType      = (unsigned short)(dataBuffer[4] + (dataBuffer[5]<<8));
  ProgHeader.TypeVersion  = (unsigned short)(dataBuffer[6] + (dataBuffer[7]<<8));
  ProgHeader.Crc16        = (unsigned short)(dataBuffer[8] + (dataBuffer[9]<<8));
  // Verify Header For Valid Type
  if(ProgHeader.MemType==PROGTYPE_CALC)
  {
   done = true;
  };
 };

 if(done)
 {
  done = false;
  dataBuffer.clear();

  // Try to load Calibration Data itself
  if(cmdFlashRead((unsigned short)(CALDATA_FLASH_START+sizeof(sProgHeader)),&dataBuffer,(unsigned short)sizeof(sCalibrationData)))
  {
   SetGetDataPtr(&dataBuffer);
   // Get Index 1: Calibration File Format Version
   CalData->CalFormatVer    = GetDataU16();

   // Get Index 2: Calibration Date Information
   for(int index=0;index<16;index++)
    CalData->CalDate[index] = GetDataU8();

   // Get Index 3: Calibration Software Version
   CalData->CalSwVer        = GetDataU16();

   // Get Index 4: Calibration Production Side
   CalData->CalProdSide     = GetDataU8();

   // Get Index 5: Calibration FrqRange Start Stop NrOfSamples
   for(int frqrangeIndex=0;frqrangeIndex<3;frqrangeIndex++)
   {
    CalData->CalFrqRange[frqrangeIndex].FStart   = GetDataU32();
    CalData->CalFrqRange[frqrangeIndex].FStop    = GetDataU32();
    CalData->CalFrqRange[frqrangeIndex].FSamples = GetDataU32();
   };

   // Add Index 6: RefLevel, Gain and IF LookUpTable
   for(int rgIndex=0;rgIndex<8;rgIndex++)
   {
    CalData->CalRefGainTable[rgIndex].RefLevel      = (signed char  )GetDataU8();
    CalData->CalRefGainTable[rgIndex].Gain          = (unsigned char)GetDataU8();
   };

   // Get Index 7: Device Production Hardware Id Information
   CalData->DevHwId             = GetDataU32();

   // Get Index 8: Device USB TUSB3410 Serialnumber
   for(int index=0;index<16;index++)
    CalData->DevUsbSerNr[index] = GetDataU8();

   // Get Index 9: Device FXtal Frequency Information
   CalData->DevFxtal            = GetDataU32();

   // Get Index 10: Device FXtal Frequency Information
   CalData->DevFxtalppm         = GetDataU16();

   // Get Index 11: Device Calibration Start Temerature
   for(int tempIndex=0;tempIndex<6;tempIndex++)
    CalData->DevTempStart[tempIndex] = GetDataU8();

   // Get Index 12: Device Calibration Stop  Temerature
   for(int tempIndex=0;tempIndex<6;tempIndex++)
    CalData->DevTempStop[tempIndex] = GetDataU8();

   // Get Index 13: Device Calibration Coeff and DcSelect Index Data per Frequency Range and Gain Level
   for(int frqIndex=0;frqIndex<3;frqIndex++)
    for(int gainIndex=0;gainIndex<8;gainIndex++)
    {
     CalData->DevCalCoeffsFrqGain[frqIndex][gainIndex].DcSelect = GetDataU8();
     for(int valueIndex=0;valueIndex<8;valueIndex++)
     {
      CalData->DevCalCoeffsFrqGain[frqIndex][gainIndex].Values[valueIndex] = (double)GetDataDouble();
     };
    };

  };
  done=true;
 };

 return(done);
}


bool drvSA430::cmdFlashRead(unsigned short AddrStart, QByteArray *Data,unsigned short Size)
{
 bool done = false;
 unsigned char cmdData[4];
 unsigned short NrOfBlocksToRead;
 unsigned short tmpLength;
 unsigned short tmpAddr;
 unsigned short tmpDataSize;
 sa430Frame dataFrame;

 NrOfBlocksToRead = Size/255;
 tmpLength = Size;

 for(unsigned short blockIndex=0;blockIndex<=NrOfBlocksToRead;blockIndex++)
 {
  done = false;
  if(tmpLength>255)
  {
   tmpDataSize = 255;
  }
  else
  {
   tmpDataSize = tmpLength;
  };
  tmpAddr   = AddrStart+(blockIndex*255);
  cmdData[0] = tmpAddr >> 8;
  cmdData[1] = tmpAddr & 0xff;
  cmdData[2] = tmpLength >> 8;
  cmdData[3] = tmpLength & 0x00FF;
  if(sa430SendCmd(CMD_FLASH_READ, &cmdData[0], 4))
   if(cmdWaitForConfirmation(CMD_FLASH_READ,500))
   {
    if(cmdWaitForData(CMD_FLASH_READ,&dataFrame,500))
    {
     for(int index=0;index<dataFrame.Length;index++)
     {
      Data->append(dataFrame.Data[index]);
      tmpLength--;
     };
     done = true;
    }
    else
    {
     blockIndex = 0xffff;
     done = false;
     Data->clear();
    };
   };
  };

 return(done);
}

void drvSA430::SetGetDataPtr(QByteArray *DataBuffer)
{
 GetData = *DataBuffer;
}

quint8  drvSA430::GetDataU8(void)
{
 unsigned char u8 = 0;

 if(GetData.size()>=1)
 {
  u8 = GetData.at(0);
  GetData.remove(0,1);
 };

 return(u8);
}


quint16 drvSA430::GetDataU16(void)
{
 quint16 u16 = 0;

 if(GetData.size()>=2)
 {
  u16  = (quint16)((unsigned char)GetData.at(0)<< 8);
  u16 |= (quint16)((unsigned char)GetData.at(1) & 0xff);
  GetData.remove(0,2);
 };

 return(u16);
}

quint32  drvSA430::GetDataU32(void)
{
 quint32 u32 = 0;
 quint8  u8[4];

 if(GetData.size()>=4)
 {
  u8[0]=(unsigned char) GetData.at(0);
  u8[1]=(unsigned char) GetData.at(1);
  u8[2]=(unsigned char) GetData.at(2);
  u8[3]=(unsigned char) GetData.at(3);

  u32  = (quint32)(u8[0]<<24);
  u32 |= (quint32)(u8[1]<<16);
  u32 |= (quint32)(u8[2]<< 8);
  u32 |= (quint32)(u8[3]    );
  GetData.remove(0,4);
 };

 return(u32);
}


quint64 drvSA430::GetDataU64(void)
{
 quint64 u64 = 0;
 quint8  u8[8];

 if(GetData.size()>=8)
 {
  u8[0]=(unsigned char) GetData.at(0);
  u8[1]=(unsigned char) GetData.at(1);
  u8[2]=(unsigned char) GetData.at(2);
  u8[3]=(unsigned char) GetData.at(3);
  u8[4]=(unsigned char) GetData.at(4);
  u8[5]=(unsigned char) GetData.at(5);
  u8[6]=(unsigned char) GetData.at(6);
  u8[7]=(unsigned char) GetData.at(7);

  u64  = (quint64)((quint64)u8[0]<<56);
  u64 |= (quint64)((quint64)u8[1]<<48);
  u64 |= (quint64)((quint64)u8[2]<<40);
  u64 |= (quint64)((quint64)u8[3]<<32);
  u64 |= (quint64)((quint64)u8[4]<<24);
  u64 |= (quint64)((quint64)u8[5]<<16);
  u64 |= (quint64)((quint64)u8[6]<< 8);
  u64 |= (quint64)((quint64)u8[7]);
  GetData.remove(0,8);
 };

 return(u64);
}

double drvSA430::GetDataDouble(void)
{
 double        dValue = 0;

 quint64 u64Value;
 double *ptrDoubleValue = (double*)&u64Value;

 if(GetData.size()>=8)
 {
  u64Value = (quint64)GetDataU64();
  dValue = (double)*ptrDoubleValue;
 };
 return(dValue);
}

bool drvSA430::GetDataStr(char *Str , int Length)
{
 bool done = false;
 int index = 0;

 if(GetData.size()>=Length && Str!=NULL)
 {
  do
  {
   *Str = GetData.at(index);
   index++;
  }while(index<Length);
  GetData.remove(0,Length);
  done = true;
 };

 return(done);
}

double drvSA430::_calcFrqCorrect(double Frq)
{
 unsigned long lFrq = 0;
 double        dFrq = 0;

 double    Fxtal = 0;


 if(Status.flagCalDataLoaded)
 {
  Fxtal = (double)((double)Status.activeCalData.DevFxtal/(double)1000000.0);
 }
 else
 {
  Fxtal = (double)26;
 };

 dFrq   = (double)((double)((double)Frq*65536)/Fxtal);
 lFrq   = (unsigned long) dFrq;
 dFrq   = (double)(((double)Fxtal*(double)lFrq)/(double)65536);

 return(dFrq);
}

unsigned long drvSA430::_calcFrqStart(double fStart)
{
 unsigned long fstart = 0;

 if(Status.flagCalDataLoaded)
 {
  fstart  =(unsigned long)((double)((double)(fStart*(double)65536.0)/(double)((double)Status.activeCalData.DevFxtal/(double)1000000)));
 }
 else
 {
  fstart  = (unsigned long)((fStart*0xffff)/26);
 };

  fstart &= 0x00ffffff;

  return(fstart);
}

unsigned long drvSA430::_calcFrqStop(double fStop)
{
 unsigned long fstop;

 fstop = 0;

 if(Status.flagCalDataLoaded)
 {
  fstop  =(unsigned long)((double)((double)(fStop*(double)65536.0)/(double)((double)Status.activeCalData.DevFxtal/(double)1000000)));
 }
 else
 {
  fstop  = (unsigned long)((fStop*0xffff)/26);
 };


 fstop &= 0x00ffffff;

 return(fstop);
}

unsigned short drvSA430::_calcFrqStep(double fStep)
{
 unsigned short fstep;

 fstep = 0;

 if(Status.flagCalDataLoaded)
 {
  fstep  =(unsigned short)((double)((double)(fStep*(double)65536.0)/(double)((double)Status.activeCalData.DevFxtal/(double)1000000)));
 }
 else
 {
  fstep = (unsigned short)((fStep*0xffff)/26);
 };


 return(fstep);
}

unsigned char drvSA430::_calcFrqRBW(unsigned char fRBWIndex)
{
 return(FrqTableFRBW[fRBWIndex].RegValue);
}

unsigned char drvSA430::_calcFrqIf(unsigned char fRBWIndex)
{
 return(FrqTableFRBW[fRBWIndex].RegValueIf);
}

unsigned char  drvSA430::_calcFrqGain(unsigned char RefDcLevelIndex)
{
 return(FrqTableRefLevel[RefDcLevelIndex].RegValue);
}


void drvSA430::u16toPar(unsigned short value, unsigned char *par)
{
 *par    = (unsigned char) ((value>>8) & 0xff);
 *(par+1)= (unsigned char) (value & 0xff);
}
void drvSA430::u32toPar(unsigned long value, unsigned char *par)
{
 *par    = (unsigned char) ((value>>16) & 0xff);
 *(par+1)= (unsigned char) ((value>>8) & 0xff);
 *(par+2)= (unsigned char) (value & 0xff);
}

bool drvSA430::cmdSetFrq(sFrqValues *FrqSetting, sFrqValues *FrqCorrected)
{
 bool           done = false;
 unsigned char  fstartBuffer[3];
 unsigned char  fstopBuffer[3];
 unsigned char  fstepBuffer[2];
 sFrqParameterBuffer FrqData;
 unsigned long NrOfSamples = 0;


 // Correct New Frq Settings
 FrqData.FrqStart     = _calcFrqStart(FrqSetting->FrqStart);
 FrqData.FrqStop      = _calcFrqStop(FrqSetting->FrqStop);
 FrqData.FrqStepWidth = _calcFrqStep((double)(FrqSetting->FrqStepWidth/1000));
 FrqData.RBW          = _calcFrqRBW(FrqSetting->RBWIndex);
 FrqData.If           = _calcFrqIf(FrqSetting->RBWIndex);
 FrqData.Gain         = _calcFrqGain(FrqSetting->RefDcLevelIndex);
 // Return Correct Frq Settings
 FrqCorrected->FrqStart         = _calcFrqCorrect(FrqSetting->FrqStart);
 FrqCorrected->FrqStop          = _calcFrqCorrect(FrqSetting->FrqStop);
 FrqCorrected->FrqStepWidth     = _calcFrqCorrect((double)FrqSetting->FrqStepWidth/1000)*1000;
 FrqCorrected->RBW              = FrqTableFRBW[FrqSetting->RBWIndex].FrqKHz;
 FrqCorrected->If               = FrqTableFRBW[FrqSetting->RBWIndex].FrqKHz;
 FrqCorrected->RefDcLevel       = FrqTableRefLevel[FrqSetting->RefDcLevelIndex].Value;
 FrqCorrected->RefDcLevelIndex  = FrqSetting->RefDcLevelIndex;
 FrqCorrected->FrqRange         = FrqSetting->FrqRange;

 NrOfSamples = ((FrqData.FrqStop - FrqData.FrqStart)/FrqData.FrqStepWidth)+1;

 u32toPar(FrqData.FrqStart     , &fstartBuffer[0]);
 u32toPar(FrqData.FrqStop      , &fstopBuffer[0] );
 u16toPar(FrqData.FrqStepWidth , &fstepBuffer[0] );


 if(CmdSetX(CMD_SETFSTART   ,(unsigned char *)&fstartBuffer[0]  ,3))
  if(CmdSetX(CMD_SETFSTOP   ,(unsigned char *)&fstopBuffer[0]   ,3))
   if(CmdSetX(CMD_SETFSTEP  ,(unsigned char *)&fstepBuffer[0]   ,2))
    if(CmdSetX(CMD_SETRBW   ,(unsigned char *)&FrqData.RBW      ,1))
     if(CmdSetX(CMD_SETIF   ,(unsigned char *)&FrqData.If       ,1))
      if(CmdSetX(CMD_SETGAIN,(unsigned char *)&FrqData.Gain     ,1))
       {
        done = true;
       };

 return(done);
}


void drvSA430::specSave(QList<sa430Frame> *DecoderBuffer)
{
 int index;
 signed char tmpValue;
 sSpectrum spectrum;

 if(!DecoderBuffer)
  return;

 spectrum.SpecId = currentSpectrumId;
 spectrum.Data.clear();
 foreach(sa430Frame item, *DecoderBuffer)
 {
  for(index=0;index<item.Length;index++)
  {
   tmpValue = (signed char) item.Data[index];
   spectrum.Data.append(tmpValue);
  };
 };
 specOffset(&spectrum);
 SpectrumBuffer.append(spectrum);
 emit signalSpectrumReceived();
}


void drvSA430::specCalcOffset(int SpecId,sFrqValues *FrqValues)
{
// int NrOfSamples=0;
// QVector<double> FrqList;

// if(FrqValues)
// {
//  SpectrumOffset.SpecId = SpecId;
//  // Caluclate required FrqSamples samples
//  NrOfSamples = (FrqValues->FrqStop - FrqValues->FrqStart)/FrqValues->FrqStepWidth;
//  FrqList.clear();
//  // Generate FrqList
//  for(int index=0;index<NrOfSamples;index++)
//  {
//   FrqList.append((double)(FrqValues->FrqStart+(index*FrqValues->FrqStepWidth)));
//  };
//  // Caluclate Offset List
//  SpectrumOffset.Offset.clear();
//  foreach(double frq,FrqList)
//  {// ToDo: Add Polynom Caluclation here and take care to use the right one!!!!!!
//   //SpectrumOffset.Offset
//  };
// };
}

void drvSA430::specOffset(sSpectrum *Spectrum)
{
// if(Spectrum->SpecId == SpectrumOffset.SpecId)
// {
//  for(int index=0;index<Spectrum->Data.count();index++)
//  {
//   Spectrum->Data[index]+=SpectrumOffset.Offset[index];
//  };
// };
}

// Firmware Updater
bool drvSA430::FwVersionIsOk(void)
{
 bool ok = false;

 if(
       Status.activeDeviceInfo.CoreVersion >= 0x0209
    && Status.activeDeviceInfo.CoreVersion != 0xffff
    && Status.activeDeviceInfo.SpecVersion >= 0x0204
    && Status.activeDeviceInfo.SpecVersion != 0xffff
   )
 {
  ok = true;
 };

 return(ok);
}




