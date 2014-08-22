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
#ifndef APPTYPEDEF_H
#define APPTYPEDEF_H

#include <QColor>
#include <QVector>
#include "sa430.h"

enum eMarker
{
 M_0 = 0,
 M_1,
 M_2,
 M_3,
 M_OFF,
 M_PEAK,
 M_ALL,
 M_UNDEFINED = 0xff
};

enum eMarkerTrace
{
 M_TRACE_CLRWRITE = 0,
 M_TRACE_MAXHOLD,
 M_TRACE_AVERAGE,
 M_TRACE_PEAK,
 M_TRACE_UNDEFINED = 0xff
};

enum eMarkerMode
{
 M_MODE_0 = 0,
 M_MODE_1,
 M_MODE_2,
 M_MODE_3,
 M_MODE_UNDEFINED = 0xff
};

enum eGridMode
{
 GRID_OFF=0,
 GRID_LINES,
 GRID_DOTTED,
 GRID_UNDEFINED = 0xff
};

enum eTrace
{
 TRACE_0=0,
 TRACE_1,
 TRACE_2,
 TRACE_3,
 TRACE_OFF,
 TRACE_UNDEFINED = 0xff
};

enum eTraceMode
{
 T_MODE_OFF = 0,
 T_MODE_CLEARWRITE,
 T_MODE_MAXHOLD,
 T_MODE_AVERAGE,
 T_MODE_UNDEFINED = 0xff
};

enum eFrqRange
{
 FRQRANGE_300_348= 0,
 FRQRANGE_389_464,
 FRQRANGE_779_928,
 FRQRANGE_UNDEFINED = 0xff
};

enum eFrqInputMode
{
 FRQMODE_CENTERSPAN= 0,
 FRQMODE_STARTSTOP,
 FRQMODE_UNDEFINED = 0xff
};

typedef struct _sFrqRefLevel
{
 signed char Value;
 unsigned char RegValue;
}sFrqRefLevel;

typedef struct _sFrqRBW
{
 double FrqKHz;
 unsigned char RegValue;
 unsigned char RegValueIf;
}sFrqRBW;

#define MAX_TRACES  4
#define MAX_MARKERS 4

#define MAX_ITEMS_REFLEVEL    8
#define MAX_ITEMS_RBW        16

extern const sFrqRefLevel   FrqTableRefLevel[MAX_ITEMS_REFLEVEL];
extern const sFrqRBW        FrqTableFRBW[MAX_ITEMS_RBW];


typedef struct _sFrqRange
{
 double    FrqMin;
 double    FrqMax;
}sFrqRange;

typedef struct _sFrqRefLevGain
{
 double RefLevel;
 double Gain;
}sFrqRefLevGain;

typedef struct _sFrqValues
{
 QString        ProfileName;
 bool           flagModeStartStop;
 bool           flagModeEasyRf;
 bool           flagModeContinous;
 int            FrqRange;
 double         FrqStart;
 double         FrqStop;
 double         FrqCenter;
 double         FrqSpan;
 double         FrqStepWidth;
 int            RefDcLevelIndex;
 signed char    RefDcLevel;
 int            RBWIndex;
 double         RBW;
 signed char    Gain;
 unsigned char  If;
}sFrqValues;

typedef struct _sFrqParameterBuffer
{
 unsigned long  FrqStart;
 unsigned long  FrqStop;
 unsigned long  FrqStepWidth;
 unsigned char  RefDcLevel;
 unsigned char  Gain;
 unsigned char  RBW;
 unsigned char  If;
}sFrqParameterBuffer;

typedef struct _sFrqSetting
{
 QString        Name;
 sFrqValues     Values;
}sFrqSetting;

typedef struct _sSettingMarker
{
 bool        On;
 eMarker     Nr;
 eMarkerMode Mode;
 eTrace      Trace;
 int    Index;
 QColor Color;
}sSettingMarker;

typedef struct _sMarkerInfo
{
 bool        On[3];
 double      FrqValue[3];
 double      dBmValue[3];
}sMarkerInfo;

typedef struct _sSettingTrace
{
 bool       On;
 eTrace     Nr;
 eTraceMode Mode;
 QColor     Color;
}sSettingTrace;

typedef struct _sSettingGrid
{
 bool       On;
 eGridMode  Mode;
}sSettingGrid;

typedef struct _sSettingGui
{
 sSettingGrid   Grid;
 sSettingTrace  Trace[MAX_TRACES];
 sSettingMarker Marker[MAX_MARKERS];
 sFrqSetting    DefaultFrqSetting;
}sSettingGui;

typedef struct _sSpectrum
{
 int SpecId;
 QVector<double>  Data;
}sSpectrum;


typedef struct _sSpectrumOffset
{
 int SpecId;
 QVector<double>  Offset;
}sSpectrumOffset;


typedef struct _sDeviceInfo
{
 QString         IDN;
 unsigned short  CoreVersion;
 unsigned short  SpecVersion;
 unsigned short  ProdVersion;
 unsigned long   SerialNumber;
 unsigned long   FrqXtal;
}sDeviceInfo;

// Typedef Struct - Flash Calibration Data
typedef struct sRefLevGain
{
 signed char   RefLevel;
 unsigned char Gain;
}sRefLevGain;

typedef struct sCalCoeffs
{
 unsigned char DcSelect;
 double        Values[8];
}sCalCoeffs;

typedef struct sCalFrqRange
{
 unsigned long  FStart;
 unsigned long  FStop;
 unsigned long  FSamples;
}sCalFrqRange;

typedef struct sCalibrationData
{
 unsigned short     CalFormatVer;
 char               CalDate[16];
 unsigned short     CalSwVer;
 unsigned char      CalProdSide;
 sCalFrqRange       CalFrqRange[3];
 sRefLevGain        CalRefGainTable[8];
 unsigned long      DevHwId;
 char               DevUsbSerNr[16];
 unsigned long      DevFxtal;
 short              DevFxtalppm;
 unsigned char      DevTempStart[6];
 unsigned char      DevTempStop[6];
 sCalCoeffs         DevCalCoeffsFrqGain[3][8];
}sCalibrationData;

typedef struct _sProgHeader
{
 unsigned short MemStartAddr;
 unsigned short MemLength;
 unsigned short MemType;
 unsigned short TypeVersion;
 unsigned short Crc16;
}sProgHeader;

typedef struct _sCalMemory
{
 sProgHeader      Header;
 sCalibrationData CalData;
}sCalMemory;


#endif // APPTYPEDEF_H
