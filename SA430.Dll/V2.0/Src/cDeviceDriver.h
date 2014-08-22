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
#ifndef CDEVICEDRIVER_H
#define CDEVICEDRIVER_H
#include <Windows.h>
#include <string>
#include <list>

using namespace std;

#include "sa430TypeDef.h"
#include "sa430.h"
#include "cdriver.h"
#include "cevent.h"
#include "cmutex.h"
#include "cthread.h"


/*!
 \brief Frame Decoder States

 \enum DecoderState
*/
enum DecoderState
{
 DS_WAITSMARKER = 0,
 DS_WAITLENGTH,
 DS_WAITCMD,
 DS_WAITDATA,
 DS_CRC_HIGH,
 DS_CRC_LOW,
};

/*!
 \brief Decoder Frame Format

 \class _sFrameDecoderState cDeviceDriver.h "cDeviceDriver.h"
*/
typedef struct _sFrameDecoderState
{
 DecoderState  State; /*!< TODO */
 unsigned short DataIndex; /*!< TODO */
 unsigned short CrcIndex; /*!< TODO */
 unsigned short Crc; /*!< TODO */
 sFrame        Frame; /*!< TODO */
}sFrameDecoderState;


/*!
 \brief SA430 Driver Class

 \class cDeviceDriver cDeviceDriver.h "cDeviceDriver.h"
*/
class cDeviceDriver
{
 public:
    /*!
     \brief Constructor

     \fn cDeviceDriver
    */
    cDeviceDriver();
    /*!
     \brief Desctructor

     \fn ~cDeviceDriver
    */
    ~cDeviceDriver();
    /*!
     \brief Check if SA430 driver class is init

     \fn IsInit
     \param void
    */
    bool IsInit(void);
        /*!
         \brief Open SA430 device

         \fn Open
         \param strPort
        */
        bool Open(std::string strPort);
    /*!
     \brief Check if SA430 is currently open

     \fn IsOpen
     \param void
    */
    bool IsOpen(void);
    /*!
     \brief Check if SA430 lowlevel driver is ok

     \fn IsDrvOk
     \param void
    */
    bool IsDrvOk(void);
    /*!
     \brief Close actual opened SA430 device
     \fn Close
     \param void
    */
    bool Close(void);
    /*!
     \brief Reset actual opened SA430 driver

     \fn Reset
     \param void
    */
    bool Reset(void);
    /*!
     \brief Send one frame to the SA430 device

     \fn SendFrame
     \param Cmd
     \param Data
     \param size
    */
    bool SendFrame(unsigned char Cmd, unsigned char *Data, unsigned short size);

    /*!
     \brief Checks if the receiver frame buffer is empty

     \fn IsFrameFifoEmpty
     \param void
    */
    bool IsFrameFifoEmpty(void);
    /*!
     \brief Return the actual number of received answer frames

     \fn GetFrameFifoSize
     \param Size
    */
    bool GetFrameFifoSize(unsigned short &Size);
    /*!
     \brief returns the first received frame from the Fifo and delete it from the Fifo

     \fn GetFrame
     \param Frame
    */
    bool GetFrame(sFrame *Frame);
    /*!
    \brief Check if in the  meantime framed from the SA430 device are received

    \fn HasFrameReceived
    \param void
    */
    bool HasFrameReceived(void);
    /*!
    \brief Checks the Crc value of the received SA430 frame

    \fn HasFrameCrcError
    \param void
    */
    bool HasFrameCrcError(void);
    /*!
    \brief Checks if the frame was received in-time

    \fn HasFrameTimeoutError
    \param void
    */
    bool HasFrameTimeoutError(void);

 private:
    // Device Driver Variables
    cEvent *eDrvOpen;       /*!< Driver Event */
    cEvent *eDecoderInit;   /*!< Decoder Event */
    cEvent *eDecoderStart;  /*!< Decoder Start Event */
    cEvent *eDecoderStop;   /*!< Decoder Stop Event */
    cEvent *eDecoderExit;   /*!< Decoder Exit Event */

    bool flagCreated;       /*!< Creat Flag */
    volatile bool flagOpen; /*!< Open Flag */

    /*!
     \brief Generate SA430 frame based on Cmd, Date and payload length

     \fn MakeFrame
     \param Frame
     \param Cmd
     \param Data
     \param Length
    */
    void MakeFrame(sFrame *Frame, unsigned char Cmd, unsigned char *Data, unsigned short Length);
    /*!
    \brief Converts byte stream to a SA430 frame

    \fn Raw2Frame
    \param Raw
    \param Frame
    */
    void Raw2Frame(std::string &Raw, sFrame &Frame);
    /*!
    \brief Converts a SA430 frame to a raw byte stream

    \fn Frame2Raw
    \param Frame
    \param Raw
    */
    void Frame2Raw(sFrame &Frame, std::string &Raw);

    // Crc16 Routines
    /*!
    \brief Calclulate a 16 Bit Crc out of a byte array

    \fn calcCrc16
    \param Raw
    */
    unsigned short calcCrc16(std::string &Raw);
    /*!
     \brief Add byte to the 16 bit Crc summ

     \fn crc16AddByte
     \param crc
     \param u8
    */
    void crc16AddByte(unsigned short &crc, unsigned char u8);

    //Driver Variables and Functions
    cMutex  DrvAccess;  /*!< Driver Mutex Lock */
    cEvent  *eDrvError; /*!< Driver Error Event */

    cDriver *Drv;       /*!< Driver */
    sError  Error;      /*!< Error  */
    /*!
    \brief

    \fn drvOpen
    \param strPort
    */
    bool drvOpen(std::string strPort);
    /*!
     \brief

     \fn drvClose
     \param void
    */
    bool drvClose(void);
    /*!
     \brief

     \fn drvSendFrame
     \param Frame
    */
    bool drvSendFrame(sFrame *Frame);
    /*!
    \brief

    \fn drvGetData
    \param rxFifo
    */
    bool drvGetData(std::string *rxFifo);
    /*!
    \brief

    \fn drvSignalError
    \param Code
    \param Msg
    */
    void drvSignalError(unsigned int Code, std::string Msg);
    /*!
    \brief

    \fn drvSignalErrorReset
    \param void
    */
    void drvSignalErrorReset(void);
    // Thread Variables and Functions
    TThread<cDeviceDriver> *ThreadHandle; /*!< TODO */
    volatile bool flagExitThread; /*!< TODO */
    /*!
    \brief

    \fn run
    */
    void run();

    // FrameDecoder Variables and Functions
    cEvent  *eventFrameReceived; /*!< TODO */
        cEvent  *eventFrameErrorCrc; /*!< TODO */
        cEvent  *eventFrameErrorTimeOut; /*!< TODO */

        sFrameDecoderState DecoderState; /*!< TODO */
    /*!
    \brief

    \fn FrameDecoderInit
    */
    void FrameDecoderInit();

    // Frame Variables and Functions
    cMutex  FrameFifoAccess; /*!< TODO */
    list<sFrame> *FrameFifo; /*!< TODO */
    /*!
    \brief

    \fn FrameReceived
    \param void
    */
    int  FrameReceived(void);
    /*!
     \brief Push frame to the Fifo

     \fn FramePush
     \param Frame
    */
    bool FramePush(sFrame *Frame);
    /*!
     \brief Pop Frame from the Fifo

     \fn FramePop
     \param Frame
    */
    bool FramePop(sFrame *Frame);
    /*!
    \brief Decode input stream data

    \fn FrameDecoder
    \param rxFifo
    */
    bool FrameDecoder(std::string *rxFifo);
    /*!
     \brief Reset frame content to initial values

     \fn ResetFrame
     \param Frame
    */
    void ResetFrame(sFrame *Frame);
};

#endif // CDEVICEDRIVER_H
