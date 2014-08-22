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
#ifndef DRVSA430_H
#define DRVSA430_H

#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QString>

#include "appTypedef.h"
#include "sa430Cmd.h"
#include "sa430.h"
#include "cEvent.h"

enum  eDrvState
{
 STATE_INIT = 0,
 STATE_OPEN,
 STATE_SETUP,
 STATE_RUN,
 STATE_EXIT,
};

#define CALDATA_FORMATVER     ((unsigned short)(0x0110))
#define LIB_VERSION	      ((unsigned short)(0x0140))

#define CALDATA_FLASH_START   ((unsigned short)(0xD400))
#define CALDATA_FLASH_END     ((unsigned short)(0xEBFF))

#define CMD_FLASH_READ        ((unsigned char)  ( 10))
#define CMD_FLASH_WRITE       ((unsigned char)  ( 11))
#define CMD_FLASH_ERASE       ((unsigned char)  ( 12))
#define CMD_FLASH_GETCRC      ((unsigned char)  ( 13))
#define FLASH_SEGMENT_SIZE    ((unsigned short) (512))
#define PROGTYPE_CALC         ((unsigned short) ( 62))

typedef struct sStatusSA430
{
 bool   flagInit;
 bool   flagDevConnected;
 bool   flagSpecTrigger;
 bool   flagSpecIsBusy;
 bool   flagSpecNewParameter;
 bool   flagSpecContinousModeOn;
 bool   flagCalDataLoaded;
 sCalibrationData  activeCalData;
 sa430UsbDevice    activeUsbInterface;
 sDeviceInfo       activeDeviceInfo;
 sFrqSetting       activeFrqSetting;
 sFrqValues        activeFrqValues;
 sFrqValues        activeFrqValuesCorrected;
}sStatusSA430;

class drvSA430: public QThread
{
 Q_OBJECT
 public:
    drvSA430();
    ~drvSA430();

    unsigned short GetDllVer(void);
    bool deviceSearch(unsigned short &NumDevs);
    bool deviceGetInfo(unsigned short DevNr,sa430UsbDevice *DevInfo);
    bool deviceConnect(sa430UsbDevice *usbInfo);
    bool deviceGetDevInfo(sDeviceInfo *devInfo);
    bool deviceGetUsbInfo(sa430UsbDevice *usbInfo);
    bool deviceGetCalData(sCalibrationData *calData);
    bool deviceDisconnect(void);
    bool spectrumSetParameter(sFrqValues *FrqValues);
    bool spectrumGetParameter(sFrqValues *FrqValues, sFrqValues *FrqCorrected);
    bool spectrumTriggerOn(void);
    bool spectrumTriggerOff(void);
    bool spectrumGetData(sSpectrum *Spectrum);

    signals :
    void signalConnected(void);
    void signalDisconnected(void);
    void signalDeviceUpdateRequired(QString ComPort);
    void signalNewParameterSet(bool Done, int SpecId);
    void signalSpectrumReceived(void);
    void signalErrorMsg(QString Msg);
// public slots:
 protected:
    void run(void);

 private:
    sStatusSA430  Status;

    cEvent *signalDeviceOpen;

    cEvent *signalSpecIsBusy;
    cEvent *signalSpecNewParameter;
    cEvent *signalSpecTrigger;


    sa430Frame          DecoderFrame;
    int                 currentSpectrumId;
    QList<sa430Frame>   DecoderSpectrumBuffer;
    QList<sSpectrum>    SpectrumBuffer;
    sSpectrumOffset     SpectrumOffset;
    QMutex DrvAccess;


    volatile eDrvState State;
    volatile bool      flagThreadExit;

    bool stateInit(void);
    bool stateOpen(void);
    bool stateSetup(void);
    bool stateExit(void);
    bool stateRun(void);


    bool IsFrameAvailable(void);
    bool GetFrame(sa430Frame *frame);
    bool cmdWaitForConfirmation(unsigned char Cmd,unsigned long ms);
    bool cmdWaitForData(unsigned char Cmd,sa430Frame *dataFrame, unsigned long ms);
    bool cmdSetU8(unsigned char Cmd,unsigned char U8);
    bool cmdSetU16(unsigned char Cmd,unsigned short U16);
    bool cmdGetU16(unsigned char Cmd, unsigned short *U16);
    bool cmdSetU32(unsigned char Cmd,unsigned long U32);
    bool cmdGetU32(unsigned char Cmd, unsigned long *U32);
    bool CmdSetX(unsigned char Cmd, unsigned char *ptrData, int Size);

    bool cmdBlink(void);
    bool cmdSync(void);
    bool cmdGetTLV(QByteArray &TlvData);
    bool cmdGetTemp(QByteArray &TempData);
    bool cmdGetXTal(unsigned long *XTal);
    bool cmdGetHwID(unsigned long *HwID);
    bool cmdGetCoreVersion(unsigned short *VerCore);
    bool cmdGetSpecVersion(unsigned short *VerSpec);
    bool cmdGetProdVersion(unsigned short *VerProd);
    bool cmdGetIDN(QString *strIDN);
    bool cmdModeSpectrumInit(void);
    bool cmdGetSpectrum(void);
    bool cmdFlashRead(unsigned short AddrStart, QByteArray *Data,unsigned short Size);
    bool cmdLoadCalData(sCalibrationData *CalData);



    bool cmdSetFrq(sFrqValues *Values , sFrqValues *FrqCorrected);
    double         _calcFrqCorrect(double Frq);
    unsigned long  _calcFrqStart(double fStart);
    unsigned long  _calcFrqStop(double fStop);
    unsigned short _calcFrqStep(double fStep);
    unsigned char  _calcFrqRBW(unsigned char fRBWIndex);
    unsigned char  _calcFrqIf(unsigned char fRBWIndex);
    unsigned char  _calcFrqGain(unsigned char RefDcLevelIndex);

    void u16toPar(unsigned short value, unsigned char *par);
    void u32toPar(unsigned long value, unsigned char *par);

    void specSave(QList<sa430Frame> *DecoderBuffer);
    void specCalcOffset(int SpecId,sFrqValues *FrqValues);
    void specOffset(sSpectrum *Spectrum);

    // Calibration Service Function Defintion
    QByteArray GetData;
    void    SetGetDataPtr(QByteArray *DataBuffer);
    quint8  GetDataU8(void);
    quint16 GetDataU16(void);
    quint32 GetDataU32(void);
    quint64 GetDataU64(void);
    double  GetDataDouble(void);
    bool    GetDataStr(char *Str , int Length);

    // Firmware Updater
    bool FwVersionIsOk(void);

};

#endif // DRVSA430_H
