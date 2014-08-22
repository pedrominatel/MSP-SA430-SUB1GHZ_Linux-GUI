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
#include "appMarker.h"

appMarker::appMarker(QwtPlot *qwtPlot,int mNr,const QColor Color,double txtOffset)
{
 if(!qwtPlot)
  return;

 flagOn     = false;
 flagTextOn = false;

 markerColor= Color;
 markerNr   = mNr;
 labelOffset= txtOffset;

 qwtCtl     = qwtPlot;

 markerSymbol   = new QwtSymbol(QwtSymbol::Diamond,QBrush(Qt::transparent),QPen(Qt::white,1),QSize(10,10));
 markerCtrl     = new QwtPlotMarker();
 symbolCtrl     = new QwtPlotMarker();
 markerText     = new QwtText(QString("M%0").arg(markerNr));
 Init();

 markerCtrl->attach(qwtCtl);
 symbolCtrl->attach(qwtCtl);
}

appMarker::~appMarker()
{
 delete markerCtrl;
 delete symbolCtrl;
}

// Public Function Definition
void appMarker::On(void)
{
 markerCtrl->show();
 symbolCtrl->show();

 flagOn = true;

 qwtCtl->replot();

}

bool appMarker::IsOn(void)
{
 return(flagOn);
}

void appMarker::Off(void)
{
 markerCtrl->hide();
 symbolCtrl->hide();

 flagOn     = false;
 flagTextOn = false;

 qwtCtl->replot();
}

void appMarker::TextOn(void)
{
 markerCtrl->setLabel(*markerText);
 flagTextOn = true;

 qwtCtl->replot();
}

void appMarker::TextOff(void)
{
 markerCtrl->setLabel(QwtText(QString("")));
 flagTextOn = false;

 qwtCtl->replot();
}

void appMarker::ColorSet(QColor Color)
{
 markerColor = Color;
 markerCtrl->setLinePen(QPen(Color));
 symbolCtrl->setLinePen(QPen(Color));

 qwtCtl->replot();
}

QColor appMarker::ColorGet(void)
{
 return(markerColor);
}


void appMarker::Move(double ValueMHz, double ValuedBm)
{
 markerText->setText(QString("M%0 [%1 dBm]").arg(markerNr).arg(ValuedBm,0,'f',1));
 markerCtrl->setValue(ValueMHz,-20);
 markerCtrl->setLabel(*markerText);
 markerCtrl->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
 symbolCtrl->setValue(ValueMHz,ValuedBm);
}

// Private Function Definition
void appMarker::Init(void)
{
 markerCtrl->setLinePen(QPen(markerColor));
 markerCtrl->setLineStyle(QwtPlotMarker::VLine);
 markerCtrl->setLabelOrientation(Qt::Horizontal);

 markerCtrl->setLabel(*markerText);
 markerText->setColor(QColor(Qt::white));
 markerCtrl->label().setText("");
 markerCtrl->setValue(310,-20);

 symbolCtrl->setLinePen(QPen(markerColor));
 symbolCtrl->setSymbol(markerSymbol);
 symbolCtrl->setValue(310,0);
}
