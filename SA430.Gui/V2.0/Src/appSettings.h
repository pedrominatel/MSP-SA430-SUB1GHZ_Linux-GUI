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
#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QString>

#include "appTypedef.h"

class appSettings : public QObject
{
    Q_OBJECT
public:
    explicit appSettings(QObject *parent = 0);
    ~appSettings();

    // Gui Settings Function Delceration
    bool LoadGuiSettings(void);
    bool SaveGuiSettings(void);
    bool GuiSettingFileExists(void);

    // Frequency Settings Function Delceration
    bool LoadFrqSettings(QString FileName,sFrqSetting *FrqListItem);
    bool SaveFrqSettings(sFrqSetting *FrqListItem);
    bool FrqSettingFileExists(void);

signals:

public slots:

private:
   QSettings::Format XmlFormat;

   // Gui Settings
   bool                flagGuiUpdate;
   QSettings          *FileGui;
   sSettingGui         Gui;

   // Frq Settings
   bool loadGuiSettings(void);
   bool saveGuiSettings(void);

   bool loadFrqSettings(void);
   bool saveFrqSettings(void);

   void xmlWriteItem(QXmlStreamWriter * writer, QString Item ,QString Value);
   bool xmlReadInt(QXmlStreamReader *reader, QString Item ,int &iValue);
   bool xmlReadBool(QXmlStreamReader *reader, QString Item ,bool &bValue);
   bool xmlReadDouble(QXmlStreamReader *reader, QString Item ,double &dValue);
   bool xmlReadItem(QXmlStreamReader *reader, QString Item ,QString &Value);
};

#endif // APPSETTINGS_H
