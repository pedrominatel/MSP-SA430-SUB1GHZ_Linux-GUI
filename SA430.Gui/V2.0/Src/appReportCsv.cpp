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
#include <QDate>
#include <QDateTime>
#include "appReportCsv.h"

appReportCsv::appReportCsv()
{
}

appReportCsv::~appReportCsv()
{
}

bool appReportCsv::WriteCurveCsv(QString FileName,sFrqValues *FrqValues, QVector<double> *DataFrq, QVector<double> *DatadBm)
{
 bool done = false;
 QString frqValue;
 QString dBmValue;

 if(DataFrq && DatadBm && !FileName.isEmpty())
 {
  // Open File
  QFile       csvFile(FileName);
  QTextStream csvStream(&csvFile);

  csvStream.setRealNumberPrecision(15);

  if(csvFile.open(QIODevice::WriteOnly))
  {
   //Generate and write Header
   csvStream << "Date           " << "," << QDateTime::currentDateTime().toString()  << "\r\n";
   csvStream << "FStart    [MHz]" << "," << (double)FrqValues->FrqStart              << "\r\n";
   csvStream << "FStop     [MHz]" << "," << (double)FrqValues->FrqStop               << "\r\n";
   csvStream << "FStepWidth[kHz]" << "," << (double)FrqValues->FrqStepWidth          << "\r\n";
   csvStream << "   MHz         " << ","  << "    dBm        "                       << "\r\n";
   csvStream << "---------------" << ","  << "---------------"                       << "\r\n";
   if(DataFrq->count() == DatadBm->count())
   {
    for(int index=0;index<DataFrq->count();index++)
    {
     csvStream << (double)DataFrq->at(index) << ","  << (double)DatadBm->at(index) << "\r\n";
    };

    done = true;
   };
  };
 };

 return(done);
}
