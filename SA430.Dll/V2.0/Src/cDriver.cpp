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
using namespace std;

#include "cdriver.h"
#include "cEvent.h"

/*!
 \brief Constructor

 \fn cDriver::cDriver
*/
cDriver::cDriver()
{
 eErrorSignal = new cEvent(true);
 SignalErrorReset();
 hPort = INVALID_HANDLE_VALUE;
}

/*!
 \brief Desctructor

 \fn cDriver::~cDriver
*/
cDriver::~cDriver()
{
 Close();
 delete  eErrorSignal;
 hPort = INVALID_HANDLE_VALUE;
}

/*!
 \brief Open Comport based on given parameter

 \fn cDriver::Open
 \param strPort
 \param Baud Baudrate
 \param Bits Data Bit Length
 \param Parity Parity Bit
 \param Stopbits Number of Stop Bits
 \param FlowCtrl  Flow Control
 \return bool true:comport is open false:failed to open
*/
bool cDriver::Open(std::string strPort, BaudRateType Baud, DataBitsType Bits, ParityType Parity, StopBitsType Stopbits, FlowType FlowCtrl)
{
 unsigned long confSize = 0;

 strPort = "//./" + strPort;

 Close();

 SignalErrorReset();

 hPort=CreateFile(strPort.c_str(), GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, NULL);
 if(hPort==INVALID_HANDLE_VALUE)
  {
   SignalError(GetLastError(),"Driver: Could not open com port");
   return(false);
  };
 Flush();
 //configure port PortSetting
 confSize = sizeof(COMMCONFIG);
 PortConfig.dwSize = confSize;
 GetCommConfig(hPort, &PortConfig, &confSize);
 GetCommState(hPort, &(PortConfig.dcb));

 //set up parameters
 PortConfig.dcb.fBinary=TRUE;
 PortConfig.dcb.fInX=FALSE;
 PortConfig.dcb.fOutX=FALSE;
 PortConfig.dcb.fAbortOnError=true;
 PortConfig.dcb.fNull=FALSE;
 setBaudRate(Baud);
 setDataBits(Bits);
 setStopBits(Stopbits);
 setParity(Parity);
 setFlowControl(FlowCtrl);
 setTimeout(1000);
 if(!SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG)))
  {
   SignalError(GetLastError(),"Driver: Could not setup com port");
   return(false);
  };

 return(true);
}

/*!
 \brief Close active comport

 \fn cDriver::Close
 \param void
 \return bool
*/
bool cDriver::Close(void)
{
 if(hPort != INVALID_HANDLE_VALUE)
  {
   CloseHandle(hPort);
   hPort = INVALID_HANDLE_VALUE;
  };
 return(true);
}

/*!
 \brief Flush current receive an transmit buffer

 \fn cDriver::Flush
 \param void
 \return bool
*/
bool cDriver::Flush(void)
{
 bool ok = false;

 if(isOpen())
  {
   if(!PurgeComm(hPort,(PURGE_RXABORT|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_TXCLEAR)))
     {
      SignalError(GetLastError(),"Driver: ClearCommError");
     }
    else
     {
      ok = true;
     };
  };
 return(true);
}


/*!
 \brief Returns number of received bytes from the comport

 \fn cDriver::GetRcvBufferSize
 \param Size
 \return bool
*/
bool cDriver::GetRcvBufferSize(unsigned long &Size)
{
 COMSTAT Status;
 DWORD   Errors=0;

 if(isOpen())
  {
    if(ClearCommError(hPort, &Errors, &Status))
    {
     Size = Status.cbInQue;
     if(Errors)
       {
        SignalError(GetLastError(),"Driver: ClearCommError Code");
        return(false);
       };
      return(true);
     }
    else
    {
     SignalError(GetLastError(),"Driver: ClearCommError");
    };
  };
 Size = 0;
return(false);
}

/*!
 \brief Set DTR signal to high

 \fn cDriver::SetDtr
 \param state
 \return int
*/
int cDriver::SetDtr(bool state)
{
 int ok = 0;

 if(isOpen())
  {
   if(state)
     ok = EscapeCommFunction(hPort, SETDTR);
    else
     ok = EscapeCommFunction(hPort, CLRDTR);
   };

 return(ok);
}

/*!
 \brief Set RTS signal to high

 \fn cDriver::SetRts
 \param state
 \return int
*/
int cDriver::SetRts(bool state)
{
 int ok = 0;

 if(isOpen())
  {
   if(state)
     ok = EscapeCommFunction(hPort, SETRTS);
    else
     ok = EscapeCommFunction(hPort, CLRRTS);
  };

 return(ok);
}

/*!
 \brief Get CTS signal state

 \fn cDriver::GetCts
 \param void
 \return bool
*/
bool cDriver::GetCts(void)
{

 return(false);
}

/*!
 \brief Write data to the comport

 \fn cDriver::WriteData
 \param Data
 \param size
 \return bool
*/
bool cDriver::WriteData(unsigned char *Data, unsigned short size)
{
 DWORD retVal = 0;

 if(!WriteFile(hPort, (void*)Data, (DWORD)size, &retVal, NULL))
  {
   SignalError(GetLastError(),"Driver: Write Data Error");
   return(false);
  };

 return(true);
}
/*!
 \brief Read data from the comport

 \fn cDriver::ReadData
 \param Data
 \param size
 \param timeoutms
 \return bool
*/
bool cDriver::ReadData(unsigned char *Data, unsigned short size, unsigned long timeoutms)
{
 DWORD retVal = 0;

 if(!ReadFile(hPort, (void*)Data, (DWORD)size, &retVal,NULL))
  {
   SignalError(GetLastError(),"Driver: Read Data Error");
   return(false);
  };

 return(true);
}

/*!
 \brief Reset comport driver

 \fn cDriver::Reset
 \param void
 \return bool
*/
bool cDriver::Reset(void)
{

 return(false);
}

/*!
 \brief Checks if comport driver has no error

 \fn cDriver::IsOk
 \param void
 \return bool true >No Error false: Error occured
*/
bool cDriver::IsOk(void)
{
 return(!eErrorSignal->Check());
}

/*!
 \brief Returns last error code

 \fn cDriver::GetLastErrorCode
 \param void
 \return int
*/
int cDriver::GetLastErrorCode(void)
{
 return(Error.Code);
}

/*!
 \brief Returns last error code string

 \fn cDriver::GetLastErrorString
 \param void
 \return std::string
*/
std::string cDriver::GetLastErrorString(void)
{
 if(Error.Code)
  {
   return(Error.Msg);
  };
 return("DevDriver: No Error");
}


// Private Function Definition

/*!
 \brief Signal a comport error

 \fn cDriver::SignalError
 \param Code
 \param Msg
*/
void cDriver::SignalError(int Code, std::string Msg)
{
 Error.Code = Code;
 Error.Msg  = Msg;
 eErrorSignal->Signal();
 Close();
}

/*!
 \brief Resets the comport error signal

 \fn cDriver::SignalErrorReset
 \param void
*/
void cDriver::SignalErrorReset(void)
{
 Error.Code = E_NO_ERROR;
 Error.Msg  = "No Error";
 eErrorSignal->Reset();
}


/*!
 \brief Checks if comport open

 \fn cDriver::isOpen
 \param void
 \return bool true:Open false:Not Open
*/
bool cDriver::isOpen(void)
{
 if(hPort!=INVALID_HANDLE_VALUE)
    return(true);
 return(false);
}

/*!
 \brief Set the comport flow control

 \fn cDriver::setFlowControl
 \param flow
*/
void cDriver::setFlowControl(FlowType flow)
{
 if(PortSetting.FlowControl== flow)
       return;

 PortSetting.FlowControl=flow;

 if(isOpen())
  {
   switch(flow)
   {
    /*No Flow Control*/
    case FLOW_OFF:
          PortConfig.dcb.fOutxCtsFlow   = FALSE;
          PortConfig.dcb.fRtsControl    = RTS_CONTROL_DISABLE;
          PortConfig.dcb.fInX           = FALSE;
          PortConfig.dcb.fOutX          = FALSE;
          SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
         break;
     /*Software (XON/XOFF) Flow Control*/
    case FLOW_XONXOFF:
          PortConfig.dcb.fOutxCtsFlow   = TRUE;
          PortConfig.dcb.fRtsControl    = RTS_CONTROL_DISABLE;
          PortConfig.dcb.fInX           = TRUE;
          PortConfig.dcb.fOutX          = TRUE;
          SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
         break;
    /*Hardwarwe Flow Control*/
    case FLOW_HARDWARE:
           PortConfig.dcb.fOutxCtsFlow  = TRUE;
           PortConfig.dcb.fRtsControl   = RTS_CONTROL_HANDSHAKE;
           PortConfig.dcb.fInX          = FALSE;
           PortConfig.dcb.fOutX         = FALSE;
           SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
          break;
     };
    };
  // UNLOCK_MUTEX();
};

/*!
 \brief Set the comport parity bits

 \fn cDriver::setParity
 \param parity
*/
void cDriver::setParity(ParityType parity)
{
 // LOCK_MUTEX();
 if(PortSetting.Parity == parity)
    return;

  PortSetting.Parity = parity;
  if(isOpen())
   {
    PortConfig.dcb.Parity=(unsigned char)parity;
    switch(parity)
    {
     /*space parity*/
     case PAR_SPACE:
           PortConfig.dcb.fParity=TRUE;
          break;
     /*mark parity - WINDOWS ONLY*/
     case PAR_MARK:
           PortConfig.dcb.fParity=TRUE;
          break;
      /*no parity*/
      case PAR_NONE:
            PortConfig.dcb.fParity=FALSE;
          break;
      /*even parity*/
      case PAR_EVEN:
            PortConfig.dcb.fParity=TRUE;
          break;
      /*odd parity*/
      case PAR_ODD:
            PortConfig.dcb.fParity=TRUE;
          break;
     };
     SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
    };
}



/*!
 \brief Set number of comport data bits

 \fn cDriver::setDataBits
 \param dataBits
*/
void cDriver::setDataBits(DataBitsType dataBits)
{
 if(PortSetting.DataBits!=dataBits)
    {
     if((PortSetting.StopBits==STOP_2 && dataBits==DAT_5) || (PortSetting.StopBits==STOP_1_5 && dataBits!=DAT_5))
       {
       }
      else
       {
        PortSetting.DataBits=dataBits;
       };
     };
  if(isOpen())
   {
    switch(dataBits)
    {
     /*5 data bits*/
     case DAT_5:
           PortConfig.dcb.ByteSize=5;
           SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
        break;
     /*6 data bits*/
     case DAT_6:
            PortConfig.dcb.ByteSize=6;
            SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
        break;
     /*7 data bits*/
     case DAT_7:
             PortConfig.dcb.ByteSize=7;
             SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
           break;

     /*8 data bits*/
     case DAT_8:
             PortConfig.dcb.ByteSize=8;
             SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
        break;
     };
    };
}
/*!
 \brief Set number of comport stop bits

 \fn cDriver::setStopBits
 \param stopBits
*/
void cDriver::setStopBits(StopBitsType stopBits)
{
 if(PortSetting.StopBits!=stopBits)
 {
  if((PortSetting.DataBits==DAT_5 && stopBits==STOP_2) || (stopBits==STOP_1_5 && PortSetting.DataBits!=DAT_5))
  {
  }
  else
  {
   PortSetting.StopBits=stopBits;
  };
 };
 if(isOpen())
 {
  switch (stopBits)
  {
   /*one stop bit*/
   case STOP_1:
        PortConfig.dcb.StopBits=ONESTOPBIT;
        SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
       break;
   /*1.5 stop bits*/
   case STOP_1_5:
        PortConfig.dcb.StopBits=ONE5STOPBITS;
        SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
       break;
   /*two stop bits*/
   case STOP_2:
       PortConfig.dcb.StopBits=TWOSTOPBITS;
       SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
      break;
  };
 };
}
/*!
 \brief Set comport baudrate

 \fn cDriver::setBaudRate
 \param baudRate
*/
void cDriver::setBaudRate(BaudRateType baudRate)
{
    if (PortSetting.BaudRate!=baudRate)
    {
        switch (baudRate)
        {
            case B50:
            case B75:
            case B134:
            case B150:
            case B200:
                PortSetting.BaudRate=B110;
                break;

            case B1800:
                PortSetting.BaudRate=B1200;
                break;

            case B76800:
                PortSetting.BaudRate=B57600;
                break;
        case B926100:
            PortSetting.BaudRate=B926100;
            break;
            default:
                break;
        };
    };
    if (isOpen())
    {
        switch (baudRate)
        {

            /*50 baud*/
            case B50:
                PortConfig.dcb.BaudRate=CBR_110;
                break;
          /*75 baud*/
            case B75:
                PortConfig.dcb.BaudRate=CBR_110;
                break;
            /*110 baud*/
            case B110:
                PortConfig.dcb.BaudRate=CBR_110;
                break;
            /*134.5 baud*/
            case B134:
                PortConfig.dcb.BaudRate=CBR_110;
                break;
            /*150 baud*/
            case B150:
                PortConfig.dcb.BaudRate=CBR_110;
                break;
            /*200 baud*/
            case B200:
                PortConfig.dcb.BaudRate=CBR_110;
                break;
            /*300 baud*/
            case B300:
                PortConfig.dcb.BaudRate=CBR_300;
                break;
            /*600 baud*/
            case B600:
                PortConfig.dcb.BaudRate=CBR_600;
                break;
            /*1200 baud*/
            case B1200:
                PortConfig.dcb.BaudRate=CBR_1200;
                break;
            /*1800 baud*/
            case B1800:
                PortConfig.dcb.BaudRate=CBR_1200;
                break;
            /*2400 baud*/
            case B2400:
                PortConfig.dcb.BaudRate=CBR_2400;
                break;
            /*4800 baud*/
            case B4800:
                PortConfig.dcb.BaudRate=CBR_4800;
                break;
            /*9600 baud*/
            case B9600:
                PortConfig.dcb.BaudRate=CBR_9600;
                break;
            /*14400 baud*/
            case B14400:
                PortConfig.dcb.BaudRate=CBR_14400;
                break;
            /*19200 baud*/
            case B19200:
                PortConfig.dcb.BaudRate=CBR_19200;
                break;
            /*38400 baud*/
            case B38400:
                PortConfig.dcb.BaudRate=CBR_38400;
                break;
            /*56000 baud*/
            case B56000:
                PortConfig.dcb.BaudRate=CBR_56000;
                break;
            /*57600 baud*/
            case B57600:
                PortConfig.dcb.BaudRate=CBR_57600;
                break;
            /*76800 baud*/
            case B76800:
                PortConfig.dcb.BaudRate=CBR_57600;
                break;
            /*115200 baud*/
            case B115200:
                PortConfig.dcb.BaudRate=CBR_115200;
                break;
            /*128000 baud*/
            case B128000:
                PortConfig.dcb.BaudRate=CBR_128000;
                break;
            /*256000 baud*/
            case B256000:
               PortConfig.dcb.BaudRate=CBR_256000;
                break;
        default:
                PortConfig.dcb.BaudRate = baudRate;
                break;
        };
        SetCommConfig(hPort, &PortConfig, sizeof(COMMCONFIG));
    };
}

/*!
 \brief Set comport timeout

 \fn cDriver::setTimeout
 \param ms
*/
void cDriver::setTimeout(long ms)
{
 PortSetting.TimeoutMs = ms;

 if(ms == -1)
   {
    PortTimeouts.ReadIntervalTimeout = MAXDWORD;
    PortTimeouts.ReadTotalTimeoutConstant = 0;
   }
  else
   {
    PortTimeouts.ReadIntervalTimeout = ms;
    PortTimeouts.ReadTotalTimeoutConstant = ms;
   };
  PortTimeouts.ReadTotalTimeoutMultiplier = 0;
  PortTimeouts.WriteTotalTimeoutMultiplier = ms;
  PortTimeouts.WriteTotalTimeoutConstant = 0;
  SetCommTimeouts(hPort, &PortTimeouts);

};


