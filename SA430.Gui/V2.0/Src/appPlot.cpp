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
#include "appPlot.h"
#include <qwt_plot_renderer.h>
#include <QFileDialog>
#include <QImageWriter>
#include <QPrintDialog>
#include <QPrinter>

appPlot::appPlot(QwtPlot *qwtplot)
{
 // Parameter Check
 if(!qwtplot)
  return;
 // Generate Objects
 // Grid
 qwtCtrl = qwtplot;
 gridCtrl = new appGrid(qwtCtrl);
 // Markers
 markerCtrl[0] = new appMarker(qwtCtrl,0,Qt::white,10);
 markerCtrl[1] = new appMarker(qwtCtrl,1,Qt::white,20);
 markerCtrl[2] = new appMarker(qwtCtrl,2,Qt::white,30);

 markerActiveCtrl = NULL;
 markerActiveCfg  = NULL;
 // Curves
 traceCtrl[0] = new appCurve(qwtCtrl,"Trc0",Qt::red   );
 traceCtrl[1] = new appCurve(qwtCtrl,"Trc1",Qt::green );
 traceCtrl[2] = new appCurve(qwtCtrl,"Trc2",Qt::yellow);
 traceCtrl[3] = new appCurve(qwtCtrl,"Trc3",Qt::blue  );
 // Init Objects
 Init();
}

appPlot::~appPlot()
{
 delete gridCtrl;
}

// Public Init Function Decleration
void appPlot::SetTitel(QString *strTitel)
{
 QwtText strText;

 if(strTitel)
 {
  strText.setFont(QFont("Arial",9));
  strText.setText(*strTitel);
 }
 else
 {
  strText.setText("");
 };
 qwtCtrl->setTitle(strText);
}

void appPlot::GetTitel(QString *strTitel)
{
 QwtText strText;
 if(strTitel)
 {
  strText = qwtCtrl->title();
  *strTitel = strText.text();
 };
}

// Public Grid Function Decleration
bool appPlot::SetNewSpectrumParameter(int SpecId,sFrqValues *NewParameters,sCalibrationData *CalibrationData)
{
 bool done = false;

 if(NewParameters)
 {
  if(calcCalDataOffset(NewParameters,CalibrationData,DataX))
  {
   SetFrequencyRange(NewParameters->FrqStart,NewParameters->FrqStop);
   SetAmplitudeRange(NewParameters->RefDcLevel-50,NewParameters->RefDcLevel+20);
   flagDataFirstSpectrum = true;
   ActiveSpecId = SpecId;
   done = true;
  };
 };

 return(done);
}

bool appPlot::SetSpectrumData(sSpectrum *NewSpectrum)
{
 bool done = false;
 if(NewSpectrum)
 {
  if(NewSpectrum->SpecId == ActiveSpecId)
  {
   if(flagDataFirstSpectrum)
   { // First Frame of the new Spectrum
    DataCalcClrWrite(&NewSpectrum->Data);
    DataMaxHold = DataClrWrite;
    DataAvarage = DataClrWrite;
//    DataView[0] = DataClrWrite;
//    DataView[1] = DataClrWrite;
//    DataView[2] = DataClrWrite;
//    DataView[3] = DataClrWrite;
    flagDataFirstSpectrum = false;
   }
   else
   {
    // Data Calc Offset
    DataCalcClrWrite(&NewSpectrum->Data);
    // Data Calc MaxHold
    DataCalcMaxHold(&DataClrWrite);
    // Data Calc Avarage
    DataCalcAvarage(&DataClrWrite);
    // Data Calc Peak
    DataCalcPeak(&DataClrWrite);
   };
   // Update Trace Data based on TraceMode
   TraceDataUpdate(TRACE_0);
   TraceDataUpdate(TRACE_1);
   TraceDataUpdate(TRACE_2);
   TraceDataUpdate(TRACE_3);
   done = true;
  };
 };
 return(done);
}

// Public Grid Function Defintion
void appPlot::GridOff(void)
{
 gridCtrl->Off();
 qwtCtrl->replot();
}

void appPlot::GridLines(void)
{
 gridCtrl->Lines();
 qwtCtrl->replot();
}

void appPlot::GridDotted(void)
{
 gridCtrl->Dotted();
 qwtCtrl->replot();
}

// Public Marker Function Defintion
void appPlot::MarkerOn(eMarker marker)
{
 appMarker  *mCtrl = NULL;
 sMarkerCfg *mCfg  = NULL;

 if(marker>=M_0 && marker<=M_3 )
 {
  mCtrl = markerCtrl[marker];
  mCfg  = &markerCfg[marker];
 };


 switch(marker)
 {
  case M_0:
  case M_1:
  case M_2:
        if(mCfg->Trace == TRACE_UNDEFINED || mCfg->Trace == TRACE_OFF)
        {
         MarkerSetColor(marker,QColor(Qt::white));
        }
        else
        {
         if(mCfg->Color != traceCfg[mCfg->Trace].Color)
         {
          MarkerSetColor(marker,traceCfg[mCfg->Trace].Color);
         };
        };
        mCtrl->On();
        MarkerSetPos(marker,MarkerGetPos(marker));
        mCfg->On = true;
     break;
 case M_ALL:
//        markerCtrl[0]->On();
//        markerCtrl[1]->On();
//        markerCtrl[2]->On();
//        markerCfg[0].On = true;
//        markerCfg[1].On = true;
//        markerCfg[2].On = true;
      break;
   default:
      break;
 };
 qwtCtrl->replot();
}

void appPlot::MarkerOff(eMarker marker)
{
 appMarker  *mCtrl = NULL;
 sMarkerCfg *mCfg  = NULL;

 if(marker>=M_0 && marker<=M_3)
 {
  mCtrl = markerCtrl[marker];
  mCfg  = &markerCfg[marker];
  mCtrl->Off();
  mCfg->On = false;
 };

 if(marker == M_ALL)
 {
  markerCtrl[0]->Off();
  markerCtrl[1]->Off();
  markerCtrl[2]->Off();
  markerCfg[0].On = false;
  markerCfg[1].On = false;
  markerCfg[2].On = false;
 };

 qwtCtrl->replot();
}

void appPlot::MarkerSetActive(eMarker marker)
{
 if(marker>=M_0 && marker<=M_3)
 {
  if(markerActiveCtrl!= markerCtrl[marker])
  {
   if(markerCtrl[marker])
    {
     markerActiveCtrl = markerCtrl[marker];
     markerActiveCfg  = &markerCfg[marker];
    }
   else
    {
     markerActiveCtrl = NULL;
     markerActiveCfg  = NULL;
    };
  };
 };
}


int appPlot::MarkerSetActivePos(int index)
{
 double datax,datay;

 if(markerActiveCtrl && markerActiveCfg)
  if(markerActiveCfg->On)
   if(markerActiveCfg->Trace!= TRACE_UNDEFINED)
    if(markerActiveCfg->DataX!=NULL && markerActiveCfg->DataY!=NULL)
     if(markerActiveCfg->DataX->count() == markerActiveCfg->DataY->count())
      if(index < markerActiveCfg->DataX->count() && index >=0)
      {
       datax = markerActiveCfg->DataX->at(index);
       datay = markerActiveCfg->DataY->at(index);
       markerActiveCfg->Pos = index;
       markerActiveCtrl->Move(datax,datay);
       MarkerUpdatePos();
       return(index);
     };
 return(0);
}

int appPlot::MarkerGetActivePos(void)
{
 int pos = 0;

 if(markerActiveCtrl && markerActiveCfg)
  if(markerActiveCfg->On)
   if(markerActiveCfg->Trace!= TRACE_UNDEFINED)
    if(markerActiveCfg->DataX!=NULL && markerActiveCfg->DataY!=NULL)
    {
     pos = markerActiveCfg->Pos;
    };

 return(pos);
}


void appPlot::MarkerSetColor(eMarker marker, QColor Color)
{
 markerCtrl[marker]->ColorSet(Color);
}

QColor appPlot::MarkerGetColor(eMarker marker)
{
 return(markerCtrl[marker]->ColorGet());
}


bool appPlot::MarkerSetTrace(eMarker marker, eTrace traceNr)
{
 bool done = false;

 appMarker  *mCtrl   = NULL;
 sMarkerCfg *mrkCfg  = NULL;
 sTraceCfg  *trcCfg  = NULL;


 if(marker>=M_0 && marker<=M_2)
 {
  mCtrl  = markerCtrl[marker];
  mrkCfg = &markerCfg[marker];
  trcCfg = &traceCfg[traceNr];
  if(trcCfg->On)
  {
   mrkCfg->Trace    = traceNr;
   trcCfg->Marker[marker]= markerCtrl[marker];
   mrkCfg->DataX = traceCfg[traceNr].DataX;
   mrkCfg->DataY = traceCfg[traceNr].DataY;
   MarkerOn(marker);
   done = true;
  }
  else
  {
   if(markerActiveCfg == &markerCfg[marker])
   {
    markerActiveCfg = NULL;
    markerActiveCtrl= NULL;
   };
   trcCfg->Marker[marker]= NULL;
   mrkCfg->DataX = NULL;
   mrkCfg->DataY = NULL;
   mrkCfg->Pos   = 0;
   MarkerOff(marker);
   done = false;
  };
 };
 return(done);
}

void appPlot::MarkerSetPos(eMarker marker, int index)
{
 double datax,datay;

 if(markerCfg[marker].On)
 {
  if(markerCfg[marker].Trace!= TRACE_UNDEFINED && markerCfg[marker].Trace!= TRACE_OFF)
  {
   if(markerCfg[marker].DataX!=NULL && markerCfg[marker].DataY!=NULL)
   {
    markerCfg[marker].Pos = index;
    if(index>=markerCfg[marker].DataX->count())
    {
     markerCfg[marker].Pos = 0;
    };
    datax = markerCfg[marker].DataX->at(markerCfg[marker].Pos);
    datay = markerCfg[marker].DataY->at(markerCfg[marker].Pos);
    markerCtrl[marker]->Move(datax,datay);
   };
  }
  else
  {

  };
 };
}

void appPlot::MarkerUpdatePos(void)
{
 MarkerSetPos(M_0,markerCfg[M_0].Pos);
 MarkerSetPos(M_1,markerCfg[M_1].Pos);
 MarkerSetPos(M_2,markerCfg[M_2].Pos);
};

void appPlot::MarkerGetInfo(sMarkerInfo *mInfo)
{
 sMarkerCfg *mCfg=NULL;
 if(mInfo)
 {
  for(int index=0;index<3;index++)
  {
   mCfg = &markerCfg[index];
   if(mCfg->On)
   {
    mInfo->On[index]       = true;
    mInfo->FrqValue[index] = mCfg->DataX->at(mCfg->Pos);
    mInfo->dBmValue[index] = mCfg->DataY->at(mCfg->Pos);
   }
   else
   {
    mInfo->On[index]= false;
   };
  };
 };
}


int appPlot::MarkerGetPos(eMarker marker)
{
 return(markerCfg[marker].Pos);
}

bool appPlot::MarkerGetPosData(eMarker marker, double &xData, double &yData)
{
 if(markerCfg[marker].On && markerCfg[marker].DataX && markerCfg[marker].DataY)
 {
  if(markerCfg[marker].Pos<markerCfg[marker].DataX->count())
  {
   xData = markerCfg[marker].DataX->at(markerCfg[marker].Pos);
   yData = markerCfg[marker].DataY->at(markerCfg[marker].Pos);
   return(true);
  };
 };
 return(false);
}


void appPlot::MarkerUpdate(eMarker marker)
{
 appMarker  *mCtrl = NULL;
 sMarkerCfg *mCfg  = NULL;

 if(marker>=M_0 && marker<=M_3)
 {
  mCtrl = markerCtrl[marker];
  mCfg  = &markerCfg[marker];

  if(mCfg->flagUpdate)
  {
   // On/Off
   if(mCtrl->IsOn())
    mCtrl->Off();
   else
    mCtrl->On();


   // Clear UpdateFlag
   mCfg->flagUpdate = false;
  };
 };
}

// Public Trace Function Decleration
void appPlot::TraceOn(eTrace traceNr)
{
 traceCtrl[traceNr]->On();
}

bool appPlot::TraceIsOn(eTrace traceNr)
{
 return(traceCtrl[traceNr]->IsOn());
}

void appPlot::TraceOff(eTrace traceNr)
{
 traceCtrl[traceNr]->Off();
}

void appPlot::TraceHoldTrigger(eTrace traceNr)
{
 if(traceNr>=TRACE_0 && traceNr <=TRACE_3)
 {
  if(traceCfg[traceNr].On)
   if(traceCfg[traceNr].Mode != T_MODE_UNDEFINED)
    if(traceCfg[traceNr].DataX != NULL && traceCfg[traceNr].DataY != NULL)
    {
     traceCfg[traceNr].DataHoldY = *traceCfg[traceNr].DataY;
     traceCfg[traceNr].DataY     = &traceCfg[traceNr].DataHoldY;
     traceCfg[traceNr].flagHold = true;
     for(int index=0;index<3;index++)
     {
      if(markerCfg[index].Trace == traceNr)
      {
       MarkerSetTrace((eMarker)index,traceNr);
      };
     };
     TraceDataUpdate(traceNr);
    };
 };
}

void appPlot::TraceHoldReset(eTrace traceNr)
{
 if(traceNr>=TRACE_0 && traceNr <=TRACE_3)
 {
  if(traceCfg[traceNr].On)
   if(traceCfg[traceNr].DataX != NULL || traceCfg[traceNr].DataY != NULL)
   {
    switch(traceCfg[traceNr].Mode)
    {
     case T_MODE_CLEARWRITE:
        traceCfg[traceNr].DataY    = &DataClrWrite;
        traceCfg[traceNr].flagHold = false;
       break;
     case T_MODE_MAXHOLD:
        traceCfg[traceNr].DataY    = &DataMaxHold;
        traceCfg[traceNr].flagHold = false;
       break;
     case T_MODE_AVERAGE:
        traceCfg[traceNr].DataY    = &DataAvarage;
        traceCfg[traceNr].flagHold = false;
       break;
     case T_MODE_UNDEFINED:
     case T_MODE_OFF:
        traceCfg[traceNr].DataY    = NULL;
        traceCfg[traceNr].DataX    = NULL;
        traceCfg[traceNr].flagHold = false;
        break;
       default:
         break;
   };

   for(int index=0;index<3;index++)
   {
    if(markerCfg[index].Trace == traceNr)
    {
     MarkerSetTrace((eMarker)index,traceNr);
    };
   };
   TraceDataUpdate(traceNr);
  };
 };
}

void appPlot::TraceClear(eTrace traceNr)
{
 if(traceNr>=TRACE_0 && traceNr <=TRACE_3)
 {
  if(traceCfg[traceNr].On)
   if(traceCfg[traceNr].DataX != NULL || traceCfg[traceNr].DataY != NULL)
    {
     switch(traceCfg[traceNr].Mode)
     {
      case T_MODE_CLEARWRITE:
          break;
        case T_MODE_MAXHOLD:
           DataMaxHold = DataClrWrite;
          break;
        case T_MODE_AVERAGE:
           DataAvarage = DataClrWrite;
          break;
        case T_MODE_UNDEFINED:
        case T_MODE_OFF:
           break;
          default:
            break;
      };

      for(int index=0;index<3;index++)
      {
       if(markerCfg[index].Trace == traceNr)
       {
        MarkerSetTrace((eMarker)index,traceNr);
       };
      };
      TraceDataUpdate(traceNr);
     };
    };
}


void appPlot::TraceSetColor(eTrace traceNr, QColor newColor)
{
 traceCfg[traceNr].Color = newColor;
 traceCtrl[traceNr]->SetColor(traceCfg[traceNr].Color);

 for(int iMarker=0;iMarker<3;iMarker++)
 {
  if(traceCfg[traceNr].Marker[iMarker]!=NULL)
  {
   traceCfg[traceNr].Marker[iMarker]->ColorSet(newColor);
  };
 };
}

QColor appPlot::TraceGetColor(eTrace traceNr)
{
 return(traceCfg[traceNr].Color);
}

void appPlot::TraceSetMode(eTrace traceNr, eTraceMode traceMode)
{
 bool flagUpdate = false;
 bool flagOn     = false;

 switch(traceMode)
 {
  case T_MODE_UNDEFINED:
  case T_MODE_OFF:
     TraceOff(traceNr);
     traceCfg[traceNr].On        = false;
     traceCfg[traceNr].Mode      = traceMode;
     traceCfg[traceNr].flagHold  = false;
     traceCfg[traceNr].DataY     = NULL;
     traceCfg[traceNr].DataX     = NULL;
     flagOn     = false;
     flagUpdate = false;
    break;
  case T_MODE_CLEARWRITE:
     traceCfg[traceNr].On        = true;
     traceCfg[traceNr].Mode      = traceMode;
     traceCfg[traceNr].flagHold  = false;
     traceCfg[traceNr].DataY     = &DataClrWrite;
     traceCfg[traceNr].DataX     = &DataX;
     flagOn     = true;
     flagUpdate = true;
    break;
  case T_MODE_MAXHOLD:
     traceCfg[traceNr].On       = true;
     traceCfg[traceNr].Mode     = traceMode;
     traceCfg[traceNr].flagHold = false;
     traceCfg[traceNr].DataY    = &DataMaxHold;
     traceCfg[traceNr].DataX    = &DataX;
     flagOn     = true;
     flagUpdate = true;
    break;
  case T_MODE_AVERAGE:
     traceCfg[traceNr].On    = true;
     traceCfg[traceNr].Mode  = traceMode;
     traceCfg[traceNr].DataY = &DataAvarage;
     traceCfg[traceNr].DataX = &DataX;
     flagOn     = true;
     flagUpdate = true;
    break;
   default:
     break;
 };

 if(flagUpdate)
 {
  TraceDataUpdate(traceNr);
 };

 if(flagOn)
 {
  if(!TraceIsOn(traceNr) && traceMode != T_MODE_OFF && traceMode != T_MODE_UNDEFINED)
  {
   TraceOn(traceNr);
  };
 }
 else
 {
  if(TraceIsOn(traceNr))
   TraceOff(traceNr);
 };
 MarkerUpdatePos();
 qwtCtrl->replot();
}

eTraceMode appPlot::TraceGetMode(eTrace traceNr)
{
 return(traceCfg[traceNr].Mode);
}

void appPlot::TraceDataUpdate(eTrace traceNr)
{
 if(traceCfg[traceNr].On)
 {
  if(traceCfg[traceNr].Mode!=T_MODE_OFF && traceCfg[traceNr].Mode!=T_MODE_UNDEFINED)
  {
   if(traceCfg[traceNr].DataX !=NULL && traceCfg[traceNr].DataY!=NULL)
   {
    traceCtrl[traceNr]->SetData(traceCfg[traceNr].DataX,traceCfg[traceNr].DataY);
   };
  };
 };
}

bool appPlot::TraceGetData(eTrace traceNr, QVector<double> *DataFrq, QVector<double> *DatadBm)
{
 bool done = false;
 if(traceCfg[traceNr].On)
 {
  if(traceCfg[traceNr].Mode!=T_MODE_OFF && traceCfg[traceNr].Mode!=T_MODE_UNDEFINED)
  {
   if(traceCfg[traceNr].DataX != NULL || traceCfg[traceNr].DataY!=NULL)
   {
    *DataFrq = *traceCfg[traceNr].DataX;
    *DatadBm = *traceCfg[traceNr].DataY;
    done = true;
   };
  };
 };
 return(done);
}

// Public Tools Function Defintion
void appPlot::Export(void)
{
 QStringList filter;
 QString fileName = "rasterview.pdf";

 const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();

 filter += "PDF Documents (*.pdf)";
 filter += "Postscript Documents (*.ps)";
 #ifndef QWT_NO_SVG
  filter += "SVG Documents (*.svg)";
 #endif

 if(imageFormats.size() > 0 )
 {
  QString imageFilter("Images (");
  for(int i=0; i<imageFormats.size(); i++)
  {
   if( i>0 )
    imageFilter += " ";
   imageFilter += "*.";
   imageFilter += imageFormats[i];
  };
  imageFilter += ")";
  filter += imageFilter;
 };

 fileName = QFileDialog::getSaveFileName(NULL, "Export File Name", fileName,filter.join(";;"), NULL, QFileDialog::DontConfirmOverwrite);
 if(!fileName.isEmpty())
 {
  QwtPlotRenderer renderer;
  renderer.setDiscardFlags(QwtPlotRenderer::DiscardNone);
  renderer.setLayoutFlags(QwtPlotRenderer::FrameWithScales);

  renderer.renderDocument(qwtCtrl, fileName, QSizeF(160, 120), 85);
 };
}

void appPlot::Print(void)
{
 QPrinter printer(QPrinter::HighResolution);
 QString docName = qwtCtrl->title().text();

 if(!docName.isEmpty())
 {
  docName.replace (QRegExp (QString::fromLatin1 ("\n")), " -- ");
  printer.setDocName (docName);
 };

 printer.setCreator("Print Example");
 printer.setOrientation(QPrinter::Landscape);

 QPrintDialog dialog(&printer);

 if(dialog.exec())
 {
  QwtPlotRenderer renderer;

//  if( printer.colorMode() == QPrinter::GrayScale )
//  {
   renderer.setDiscardFlags(QwtPlotRenderer::DiscardNone);
   renderer.setLayoutFlags(QwtPlotRenderer::FrameWithScales);
//  };

  renderer.renderTo(qwtCtrl, printer);
 };
}


// Private Function Defintion
void appPlot::Init(void)
{
 QwtText strText;

 // Init QwtPlot Area
 strText.setFont(QFont("Arial",9));
 strText.setText("Spectrum");
 qwtCtrl->setTitle(strText);
 qwtCtrl->setAutoReplot(true);

 // Init xAxis
 strText.setText("MHz");
 qwtCtrl->setAxisTitle(QwtPlot::xBottom,strText);
 qwtCtrl->setAxisScale(QwtPlot::xBottom, 300, 348);
 // Init yAxis
 strText.setText("dBm");
 qwtCtrl->setAxisTitle(QwtPlot::yLeft,strText);
 qwtCtrl->setAxisScale(QwtPlot::yLeft, -128, +20);

 gridCtrl->Off();

 // Init Marker
 markerCtrl[0]->Off();
 markerCtrl[1]->Off();
 markerCtrl[2]->Off();

 markerCtrl[0]->TextOff();
 markerCtrl[1]->TextOff();
 markerCtrl[2]->TextOff();

 for(int index=0;index<3;index++)
 {
  markerCfg[index].flagUpdate   = false;
  markerCfg[index].On           = false;
  markerCfg[index].Pos          = 0;
  markerCfg[index].Trace        = TRACE_OFF;
  markerCfg[index].DataX        = NULL;
  markerCfg[index].DataY        = NULL;
 };

 // Init Curves

 initData();
 initTrace();
 traceCtrl[TRACE_0]->SetData(&DataX,&DataClrWrite);
 traceCtrl[TRACE_1]->SetData(&DataX,&DataClrWrite);
 traceCtrl[TRACE_2]->SetData(&DataX,&DataClrWrite);
 traceCtrl[TRACE_3]->SetData(&DataX,&DataClrWrite);

 traceCtrl[TRACE_0]->Off();
 traceCtrl[TRACE_1]->Off();
 traceCtrl[TRACE_2]->Off();
 traceCtrl[TRACE_3]->Off();

 qwtCtrl->replot();
}

// Private Data Function Defintion
void appPlot::initTrace(void)
{
 for(int index=0;index<4;index++)
 {
  traceCfg[index].flagUpdate = false;
  traceCfg[index].On         = false;
  traceCfg[index].Color      = Qt::white;
  traceCfg[index].Mode       = T_MODE_OFF;
  traceCfg[index].Marker[0]  = NULL;
  traceCfg[index].Marker[1]  = NULL;
  traceCfg[index].Marker[2]  = NULL;
  traceCfg[index].DataX      = NULL;
  traceCfg[index].DataY      = NULL;
  traceCfg[index].DataHoldY.clear();
 };
}


// Private Data Function Defintion
void appPlot::initData(void)
{
 //Frq Axis
 SetFrequencyRange(300,348);
 DataReset(&DataClrWrite,50,-110);
 DataReset(&DataAvarage,50,-111);
 DataReset(&DataMaxHold,50,-112);
 DataReset(&traceCfg[TRACE_0].DataHoldY,50,-(120));
 DataReset(&traceCfg[TRACE_1].DataHoldY,50,-(121));
 DataReset(&traceCfg[TRACE_2].DataHoldY,50,-(122));
 DataReset(&traceCfg[TRACE_3].DataHoldY,50,-(123));
 DataReset(&DataOffset,50,0);
 flagDataFirstSpectrum = false;
}

void appPlot::DataReset(QVector<double> *Data,int newSize, double fillValue)
{
 Data->clear();
 Data->resize(newSize);
 Data->fill(fillValue);
}

void appPlot::DataCalcClrWrite(QVector<double> *Data)
{
 if(Data)
 {
  DataClrWrite = *Data;
  if(Data->size()== DataOffset.size())
  {
   for(int index=0;index<Data->count();index++)
   {
    DataClrWrite[index]= ((Data->at(index))/2)-DataOffset.at(index);
   };
  }
  else
  {
   for(int index=0;index<Data->count();index++)
   {
    DataClrWrite[index]  = ((Data->at(index))/2) + DataOffset[index];
   };
  };
 };
}

void appPlot::DataCalcMaxHold(QVector<double> *Data)
{
 if(Data)
 {
  for(int index=0;index<Data->count();index++)
  {
   if(DataMaxHold.count()!=Data->count())
   {
    DataMaxHold = *Data;
   }
   else
   {
    if(Data->at(index)>DataMaxHold.at(index))
    DataMaxHold[index] = Data->at(index);
   };
  };
 };
}

void appPlot::DataCalcAvarage(QVector<double> *Data)
{
 if(Data)
 {
  if(Data->count() != DataAvarage.count())
  {
   DataAvarage = *Data;
  }
  else
  {
   for(int index=0;index<Data->count();index++)
   {
    DataAvarage[index]= (DataAvarage.at(index)*5+Data->at(index))/6;
   };
  };
 };
}

void appPlot::DataCalcPeak(QVector<double> *Data)
{
 Data = Data;
}



// Private Axis Function Defintion

void appPlot::SetFrequencyRange(double FrqStart, double FrqStop)
{
 // Re-Scale X Axis
 qwtCtrl->setAxisScale(QwtPlot::xBottom, FrqStart, FrqStop);
}

void appPlot::SetAmplitudeRange(double dBmMax, double dBmMin)
{
 qwtCtrl->setAxisScale(QwtPlot::yLeft, dBmMax, dBmMin);
}

// Calibration Function Definition
bool appPlot::calcCalDataOffset(sFrqValues *FrqValues , sCalibrationData *CalData, QVector<double> &Data)
{
 bool done = true;
 double Fstart;
 double Fstop;
 double Fstepwidth;
 double Frq = 0;

 unsigned long nrofsamples;
 double *ptrCalDataCoeffs = NULL;

 if(FrqValues && CalData)
 {
  // Generate Frequency Table
  Fstart      = FrqValues->FrqStart;
  Fstop       = FrqValues->FrqStop;
  Fstepwidth  = FrqValues->FrqStepWidth/1000;
  nrofsamples = (unsigned long) ((Fstop-Fstart)/ Fstepwidth)+1;
  Data.clear();
  Data.resize(nrofsamples);
  Frq = Fstart;
  for(int index=0;index<Data.count();index++)
  {
   Data[index] = Frq;
   Frq += Fstepwidth;
  };
  // Define CoeffPointer based on Frequency Settings
  ptrCalDataCoeffs = (double*)&(CalData->DevCalCoeffsFrqGain[FrqValues->FrqRange][FrqValues->RefDcLevelIndex].Values[0]);
  // Calculate OffSet based on Frequency Table
  DataOffset.clear();
  DataOffset.resize(nrofsamples);
  for(int index=0;index<DataOffset.count();index++)
  {
   calcCoeff(Data[index],(double*)ptrCalDataCoeffs, DataOffset[index]);
  };

  done = true;
 };

 return(done);
}

bool appPlot::calcCoeff(double FrqValue,double *Coeffs, double &dBm)
{
 bool done = false;

 double coefs[8];
 double FrqMHz;

 for(int x=0;x<8;x++)
 {
  coefs[x]=*Coeffs;
  Coeffs++;
 };

 dBm    = 0;
 FrqMHz = (double)((double)FrqValue *(double)1000000.0);

 if(Coeffs)
 {
  for(int n=0;n<8;n++)
  {
   dBm += (double) ((double)coefs[n]*(double)qPow(FrqMHz,n));
  };
  done = true;
 };

 return(done);
}
