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
#include "appCurve.h"

appCurve::appCurve(QwtPlot *qwtPlot,QString Label,QColor Color)
{
 flagVisible = false;
 flagUpdate  = false;

 qwtCtrl = qwtPlot;
 curveLabelFont = new QFont("Arial",9);
 curveLabel     = new QwtText(Label);
 curveCtrl      = new QwtPlotCurve(*curveLabel);
 curveColor     = new QColor(Color);
 curvePen       = new QPen(*curveColor);

 curveDataX      = NULL;
 curveDataY      = NULL;

 curveCtrl->setPen(*curvePen);
 curveCtrl->setStyle(QwtPlotCurve::Lines);
 curveCtrl->attach(qwtCtrl);

 curveCtrl->setVisible(flagVisible);

}

appCurve::~appCurve()
{
 delete curveLabelFont;
 delete curveLabel;
 delete curveCtrl;
 delete curveColor;
 delete curvePen;
}

// Public Function Defintion
void appCurve::On(void)
{
 flagVisible = true;
 curveCtrl->setVisible(flagVisible);
}

bool appCurve::IsOn(void)
{
 return(flagVisible);
}

void appCurve::Off(void)
{
 flagVisible = false;
 curveCtrl->setVisible(flagVisible);
}

void   appCurve::SetColor(QColor Color)
{
 *curveColor = Color;
 curvePen->setColor(*curveColor);
 curveCtrl->setPen(*curvePen);
}

QColor appCurve::GetColor(void)
{
 return(*curveColor);
}

void appCurve::SetData(QVector<double> *DataX,QVector<double> *DataY)
{

 if(DataX && DataY)
 {
  curveDataX = DataX;
  curveDataY = DataY;
  curveCtrl->setSamples(*DataX,*DataY);
 }
 else
 {
  curveDataX = NULL;
  curveDataY = NULL;
  Off();
 };
}

// Public Function Defintion
