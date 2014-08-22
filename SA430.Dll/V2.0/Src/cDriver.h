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
#ifndef CDRIVER_H
#define CDRIVER_H

#include <string>
#include <windows.h>
#include <setupapi.h>

#include "cevent.h"

using namespace std;
/*Serial Line Status Bits*/
#define LS_CTS  0x01
#define LS_DSR  0x02
#define LS_DCD  0x04
#define LS_RI   0x08
#define LS_RTS  0x10
#define LS_DTR  0x20
#define LS_ST   0x40
#define LS_SR   0x80

/*Serial Port Error Constant's*/
#define E_NO_ERROR                   0
#define E_INVALID_FD                 1
#define E_NO_MEMORY                  2
#define E_CAUGHT_NON_BLOCKED_SIGNAL  3
#define E_PORT_TIMEOUT               4
#define E_INVALID_DEVICE             5
#define E_BREAK_CONDITION            6
#define E_FRAMING_ERROR              7
#define E_IO_ERROR                   8
#define E_BUFFER_OVERRUN             9
#define E_RECEIVE_OVERFLOW          10
#define E_RECEIVE_PARITY_ERROR      11
#define E_TRANSMIT_OVERFLOW         12
#define E_READ_FAILED               13
#define E_WRITE_FAILED              14

/*!
 \brief Valid Baudrate Values

 \enum BaudRateType
*/
enum BaudRateType
{
 B50,                //POSIX ONLY
 B75,                //POSIX ONLY
 B110,
 B134,               //POSIX ONLY
 B150,               //POSIX ONLY
 B200,               //POSIX ONLY
 B300,
 B600,
 B1200,
 B1800,              //POSIX ONLY
 B2400,
 B4800,
 B9600,
 B14400,             //WINDOWS ONLY
 B19200,
 B38400,
 B56000,             //WINDOWS ONLY
 B57600,
 B76800,             //POSIX ONLY
 B115200,
 B128000,            //WINDOWS ONLY
 B256000,             //WINDOWS ONLY
 B926100=926100
};

/*!
 \brief Std. data bits types

 \enum DataBitsType
*/
enum  DataBitsType
{
 DAT_5,
 DAT_6,
 DAT_7,
 DAT_8
};

/*!
 \brief Std. parity types

 \enum ParityType
*/
enum ParityType
{
 PAR_NONE,
 PAR_ODD,
 PAR_EVEN,
 PAR_MARK,               //WINDOWS ONLY
 PAR_SPACE
};

/*!
 \brief Std. stop bit types

 \enum StopBitsType
*/
enum StopBitsType
{
 STOP_1,
 STOP_1_5,               //WINDOWS ONLY
 STOP_2
};

/*!
 \brief Std. flow types

 \enum FlowType
*/
enum FlowType
{
 FLOW_OFF,
 FLOW_HARDWARE,
 FLOW_XONXOFF
};


/*!
 \brief

 \class sPortSetting cDriver.h "cDriver.h"
*/
typedef struct sPortSetting
{
 BaudRateType	BaudRate;   /*!< TODO */
 DataBitsType	DataBits;   /*!< TODO */
 ParityType	Parity;     /*!< TODO */
 StopBitsType	StopBits;   /*!< TODO */
 FlowType	FlowControl;/*!< TODO */
 long		TimeoutMs;  /*!< TODO */
}sPortSetting;

/*!
 \brief

 \class sError cDriver.h "cDriver.h"
*/
typedef struct sError
{
 int         Code;  /*!< TODO */
 std::string Msg;   /*!< TODO */
}sError;


/*!
 \brief

 \class cDriver cDriver.h "cDriver.h"
*/
class cDriver
{
 public:
   /*!
    \brief Constructor

    \fn cDriver
   */
   cDriver();
   /*!
    \brief Desctructor

    \fn ~cDriver
   */
   ~cDriver();
   /*!
    \brief Open Comport based on given parameter

    \fn Open
    \param strPort
    \param Baud
    \param Bits
    \param Parity
    \param Stopbits
    \param FlowCtrl
   */
   bool Open(std::string strPort, BaudRateType Baud=B115200, DataBitsType Bits=DAT_8, ParityType Parity=PAR_NONE, StopBitsType Stopbits=STOP_1, FlowType FlowCtrl=FLOW_OFF);
   /*!
    \brief Checks if comport driver has no error

    \fn IsOk
    \param void
   */
   bool IsOk(void);
   /*!
    \brief Returns last error code

    \fn GetLastErrorCode
    \param void
   */
   int GetLastErrorCode(void);
   /*!
    \brief Returns last error code string

    \fn GetLastErrorString
    \param void
   */
   std::string GetLastErrorString(void);
   /*!
    \brief Close active comport

    \fn Close
    \param void
   */
   bool Close(void);
   /*!
    \brief Flush current receive an transmit buffer

    \fn Flush
    \param void
   */
   bool Flush(void);
   /*!
    \brief Returns number of received bytes from the comport

    \fn GetRcvBufferSize
    \param Size
   */
   bool GetRcvBufferSize(unsigned long &Size);
   /*!
    \brief Set DTR signal to high

    \fn SetDtr
    \param state
   */
   int  SetDtr(bool state=true);
   /*!
    \brief Set RTS signal to high

    \fn SetRts
    \param state
   */
   int  SetRts(bool state=true);
   /*!
    \brief Get CTS signal state

    \fn GetCts
    \param void
   */
   bool GetCts(void);
   /*!
    \brief Write data to the comport

    \fn WriteData
    \param Data
    \param size
   */
   bool WriteData(unsigned char *Data, unsigned short size);
   /*!
    \brief Read data from the comport

    \fn ReadData
    \param Data
    \param size
    \param timeoutms
   */
   bool ReadData(unsigned char *Data, unsigned short size, unsigned long timeoutms);
   /*!
    \brief Reset comport driver

    \fn Reset
    \param void
   */
   bool Reset(void);

private:
   HANDLE hPort; /*!< TODO */
   sPortSetting    PortSetting; /*!< TODO */
   COMMCONFIG      PortConfig; /*!< TODO */
   COMMTIMEOUTS    PortTimeouts; /*!< TODO */

   sError          Error; /*!< TODO */
   std::string     rxFifo; /*!< TODO */
   cEvent          *eErrorSignal; /*!< TODO */

   /*!
    \brief Signal a comport error

    \fn SignalError
    \param Code
    \param Msg
   */
   void SignalError(int Code, std::string Msg);
   /*!
    \brief Resets the comport error signal

    \fn SignalErrorReset
    \param void
   */
   void SignalErrorReset(void);
   /*!
    \brief Checks if comport open

    \fn isOpen
    \param void
   */
   bool isOpen(void);
   /*!
    \brief Set the comport flow control

    \fn setFlowControl
    \param FlowType
   */
   void setFlowControl(FlowType);
   /*!
    \brief Set the comport parity bits

    \fn setParity
    \param ParityType
   */
   void setParity(ParityType);
   /*!
    \brief Set number of comport data bits

    \fn setDataBits
    \param DataBitsType
   */
   void setDataBits(DataBitsType);
   /*!
    \brief Set number of comport stop bits

    \fn setStopBits
    \param StopBitsType
   */
   void setStopBits(StopBitsType);
   /*!
    \brief Set comport baudrate

    \fn setBaudRate
    \param BaudRateType
   */
   void setBaudRate(BaudRateType);
   /*!
    \brief Set comport timeout

    \fn setTimeout
    \param ms
   */
   void setTimeout( long ms);
};

#endif // CDRIVER_H
