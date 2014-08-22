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
#ifndef APPPLOT_H
#define APPPLOT_H

#include <QColor>

#include "qwt_painter.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_text.h"

#include "appTypedef.h"
#include "appGrid.h"
#include "appMarker.h"
#include "appCurve.h"

typedef struct sMarkerCfg
{
 bool            flagUpdate;
 bool            On;
 QColor          Color;
 int             Pos;
 eTrace          Trace;
 QVector<double> *DataX;
 QVector<double> *DataY;
}sMarkerCfg;

typedef struct sTraceCfg
{
 bool               flagUpdate;
 bool               On;
 bool               flagHold;
 QColor             Color;
 eTraceMode         Mode;
 appMarker          *Marker[3];
 QVector<double>    *DataX;
 QVector<double>    *DataY;
 QVector<double>    DataHoldY;
}sTraceCfg;


class appPlot
{
 public:
    appPlot(QwtPlot *qwtplot);
    ~appPlot();
    // Public Init Function Decleration
    void GetTitel(QString *strTitel);
    void SetTitel(QString *strTitel);

    // Public Grid Function Decleration
    bool SetNewSpectrumParameter(int SpecId,sFrqValues *NewParameters,sCalibrationData *CalibrationData);
    bool SetSpectrumData(sSpectrum *NewSpectrum);

    // Public Grid Function Decleration
    void GridOff(void);
    void GridLines(void);
    void GridDotted(void);

    // Public Marker Function Decleration
    void MarkerOn(eMarker marker);
    void MarkerOff(eMarker marker);
    void MarkerSetActive(eMarker marker);
    int MarkerSetActivePos(int index);
    int  MarkerGetActivePos(void);
    void MarkerSetColor(eMarker marker, QColor Color);
    QColor MarkerGetColor(eMarker marker);
    bool  MarkerSetTrace(eMarker marker, eTrace traceNr);
    void MarkerSetPos(eMarker marker, int index);
    int  MarkerGetPos(eMarker marker);
    bool MarkerGetPosData(eMarker marker, double &xData, double &yData);
    void MarkerUpdate(eMarker marker);
    void MarkerUpdatePos(void);
    void MarkerGetInfo(sMarkerInfo *mInfo);

    // Public Trace Function Decleration
    void TraceOn(eTrace traceNr);
    bool TraceIsOn(eTrace traceNr);
    void TraceOff(eTrace traceNr);

    void TraceSetColor(eTrace traceNr, QColor newColor);
    QColor TraceGetColor(eTrace traceNr);
    void TraceSetMode(eTrace traceNr, eTraceMode traceMode);
    eTraceMode TraceGetMode(eTrace traceNr);
    bool TraceGetData(eTrace traceNr, QVector<double> *DataFrq, QVector<double> *DatadBm);

    void TraceHoldTrigger(eTrace traceNr);
    void TraceHoldReset(eTrace traceNr);

    void TraceClear(eTrace traceNr);

    // Public Tools Function Decleration
    void Export(void);
    void Print(void);

 private:
    QwtPlot     *qwtCtrl;
    appGrid     *gridCtrl;
    // Marker
    appMarker   *markerActiveCtrl;
    appMarker   *markerCtrl[3];
    sMarkerCfg  *markerActiveCfg;
    sMarkerCfg   markerCfg[3];
    // Trace
    appCurve    *traceCtrl[4];
    sTraceCfg    traceCfg[4];
    void initTrace(void);
    void TraceDataUpdate(eTrace traceNr);

    // Spectrum
    int          ActiveSpecId;


    // Data
    bool            flagDataFirstSpectrum;
    QVector<double> DataX;
    QVector<double> DataClrWrite;
    QVector<double> DataAvarage;
    QVector<double> DataMaxHold;
    QVector<double> DataOffset;
    void initData(void);
    void DataReset(QVector<double> *Data,int newSize, double fillValue);
    void DataCalcClrWrite(QVector<double> *Data);
    void DataCalcMaxHold(QVector<double> *Data);
    void DataCalcAvarage(QVector<double> *Data);
    void DataCalcPeak(QVector<double> *Data);
    // Others
    void Init(void);

    // Private Axis Function Decleration

    void SetFrequencyRange(double FrqStart, double FrqStop);
    void SetAmplitudeRange(double dBmMax, double dBmMin);

    // Calibration Function Decleration
    bool calcCalDataOffset(sFrqValues *FrqValues , sCalibrationData *CalData, QVector<double> &Data);
    bool calcCoeff(double FrqValue,double *Coeffs, double &dBm);
};

#endif // APPPLOT_H
