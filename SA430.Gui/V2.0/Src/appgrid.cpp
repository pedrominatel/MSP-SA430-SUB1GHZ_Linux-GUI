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
#include "appgrid.h"
#include "qwt.h"
#include "qwt_plot_grid.h"
#include <QPen>
appGrid::appGrid(QwtPlot *qwtPlot)
{
 if(!qwtPlot)
  return;

 plothandle = qwtPlot;
 gridhandle = new QwtPlotGrid();

 gridhandle->setTitle("Grid Test");
 gridhandle->enableXMin(false);
 gridhandle->enableYMin(false);
 gridhandle->enableX(true);
 gridhandle->enableY(true);
 gridhandle->setMajPen(QPen(Qt::darkGray, 0, Qt::DotLine));
 gridhandle->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
 gridhandle->setVisible(false);
 gridhandle->attach(plothandle);
}

appGrid::~appGrid()
{
 delete gridhandle;
}

void appGrid::Off(void)
{
 gridhandle->setVisible(false);
}

void appGrid::Lines(void)
{
 gridhandle->setMajPen(QPen(Qt::darkGray, 0, Qt::SolidLine));
 gridhandle->setMinPen(QPen(Qt::gray, 0 , Qt::SolidLine));
 gridhandle->setVisible(true);
}

void appGrid::Dotted(void)
{
 gridhandle->setMajPen(QPen(Qt::darkGray, 0, Qt::DotLine));
 gridhandle->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
 gridhandle->setVisible(true);
}

