/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>

#include "aboutdialog.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define VERSION_STR STRINGIFY(TEXMAKERVERSION)

AboutDialog::AboutDialog(QWidget *parent)
    :QDialog( parent)
{
ui.setupUi(this);
QPixmap pixmap;
if (qApp->devicePixelRatio()>=2)
{
pixmap.load(":/images/splash@2x.png");
pixmap.setDevicePixelRatio(qApp->devicePixelRatio());
}
else pixmap.load(":/images/splash.png");

ui.label->setPixmap(pixmap);
ui.textBrowser1->setOpenExternalLinks(true);
ui.textBrowser2->setOpenExternalLinks(true);
ui.textBrowser3->setOpenExternalLinks(true);
ui.textBrowser4->setOpenExternalLinks(true);
QString head=QString::fromUtf8("<b>Texmaker ")+QLatin1String(VERSION_STR)+QString::fromUtf8("</b><br>(compiled with Qt ")+QLatin1String(QT_VERSION_STR);

ui.textBrowser1->setHtml(head+QString::fromUtf8(")<br><b>Copyright (c) 2003-2018 by Pascal Brachet</b><br><br>Project home site : <a href=\"http://www.xm1math.net/texmaker/\">http://www.xm1math.net/texmaker/</a>"));
ui.textBrowser2->setHtml(QString::fromUtf8("<b>Copyright (c) 2003-2018 by Pascal Brachet</b><br><br>Texmaker contains code from these programs:<br>Pdfium (BSD)<br>QtPDF (The Qt Company Ltd - GPL)<br>Hunspell (GPL)<br>qpdfview ((C) Adam Reichold GPL)<br>QtCreator (GPL)<br>SyncTeX ( (C) Jerome Laurens - License: GPL)<br>Texworks (GPL)<br>Tiled (Copyright  Vsevolod Klementjev - License: GPL)<br>jsdifflib ((C) Chas Emerick - license : BSD)"));
ui.textBrowser3->setHtml(QString::fromUtf8("<b>Thanks to these contributors :</b><br>Joël Amblard<br>Stéphane Yvon<br>Robin.Watts<br>Julián Moreno Patiño<br>Annette Klepp<br>Nguyen Duy Dien<br>Wouter Franssen<br>Patrick Wieschollek<br>Oriol González Llobet<br>Andriy Bandura<br> Merlin Raschtuttis<br>Sebastián Vanrell<br>Kleanthis Manolopoulos<br>Володимир Боденчук<br>Sandris LACIS<br>Koutheir ATTOUCHI<br>Andreas Pettersson<br>George Boumis<br>Pedro F. Silva<br>Nikolić Miroslav<br>Michele Mastropietro<br>Peter Axt<br>Torbjörn Klatt<br>Tomas Olarte Hernandez<br>Jan Jełowicki<br>Tamas Orosz<br>Adrián Yanes Martínez<br>Koen Wybo<br>Pavel Fric<br>Christian Cenker<br>Seyyed Razi Alavizadeh<br>Frederic Devernay<br>Luis Silvestre<br>Davide Alberelli<br>Ilya Barygin<br>Javier Jardón Cabezas<br>Felipe Bugno<br>Carles Gumí<br>Martin Dreher<br>Andreas Volk<br>Felix Löhr<br>Bernd Warken<br>Wolfgang Dautermann<br>Carlos Eduardo Valencia Urbina<br>Kleanthis Manolopoulos"));
QTextCodec *codec = QTextCodec::codecForName("UTF-8");
QString content;
QFile apropos(":/utilities/license_html.txt");
apropos.open(QIODevice::ReadOnly);
QTextStream in(&apropos);
in.setCodec(codec);
while (!in.atEnd()) 
	{
	content+= in.readLine()+"\n";
	}
apropos.close();
ui.textBrowser4->setHtml(content);
}

AboutDialog::~AboutDialog(){
}
