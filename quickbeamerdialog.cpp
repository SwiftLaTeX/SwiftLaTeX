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

#include "quickbeamerdialog.h"


QuickBeamerDialog::QuickBeamerDialog(QWidget *parent, const char *name)
    :QDialog( parent)
{
setWindowTitle(name);
setModal(true);
ui.setupUi(this);
ui.listWidgetBabel->setSelectionMode(QAbstractItemView::ExtendedSelection  );
setWindowTitle(tr("Quick Start"));
labelImage = new QLabel(ui.scrollArea);
labelImage->setScaledContents(true);
labelImage->setMinimumSize(461,696);
labelImage->setMaximumSize(461,696);
ui.scrollArea->setMinimumWidth(500);
ui.scrollArea->setMaximumWidth(500);

if (qApp->devicePixelRatio()>=2)
{
labelImage->setMinimumSize(240,348);
labelImage->setMaximumSize(240,348);
ui.scrollArea->setMinimumWidth(280);
ui.scrollArea->setMaximumWidth(280);
}

ui.scrollArea->setBackgroundRole(QPalette::Dark);
ui.scrollArea->setWidget(labelImage);
}

QuickBeamerDialog::~QuickBeamerDialog(){
}

void QuickBeamerDialog::Init()
{
ui.comboBoxSize->clear();
ui.comboBoxSize->addItem( "8pt" );
ui.comboBoxSize->addItem( "9pt" );
ui.comboBoxSize->addItem( "10pt" );  
ui.comboBoxSize->addItem( "11pt" );
ui.comboBoxSize->addItem( "12pt" );
ui.comboBoxSize->addItem( "14pt" );
ui.comboBoxSize->addItem( "17pt" );
ui.comboBoxSize->addItem( "20pt" );
  
ui.comboBoxTheme->clear();
ui.comboBoxTheme->addItem( "AnnArbor" );
ui.comboBoxTheme->addItem( "Antibes" );
ui.comboBoxTheme->addItem( "Bergen" );
ui.comboBoxTheme->addItem( "Berkeley" );
ui.comboBoxTheme->addItem( "Berlin" );
ui.comboBoxTheme->addItem( "Boadilla" );
ui.comboBoxTheme->addItem( "CambridgeUS" );
ui.comboBoxTheme->addItem( "Copenhagen" );
ui.comboBoxTheme->addItem( "Darmstadt" );
ui.comboBoxTheme->addItem( "Dresden" );
ui.comboBoxTheme->addItem( "Frankfurt" );
ui.comboBoxTheme->addItem( "Goettingen" );
ui.comboBoxTheme->addItem( "JuanLesPins" );
ui.comboBoxTheme->addItem( "Luebeck" );
ui.comboBoxTheme->addItem( "Madrid" );
ui.comboBoxTheme->addItem( "Malmoe" );
ui.comboBoxTheme->addItem( "Marburg" );
ui.comboBoxTheme->addItem( "Montpellier" );
ui.comboBoxTheme->addItem( "PaloAlto" );
ui.comboBoxTheme->addItem( "Pittsburgh" );
ui.comboBoxTheme->addItem( "Rochester" );
ui.comboBoxTheme->addItem( "Singapore" );
ui.comboBoxTheme->addItem( "Szeged" );
ui.comboBoxTheme->addItem( "Warsaw" );

connect(ui.comboBoxTheme, SIGNAL(currentIndexChanged( const QString & ) ), this, SLOT( updatePreview(const QString &) ));

ui.comboBoxEncoding->clear();
ui.comboBoxEncoding->addItem( "latin1" );
ui.comboBoxEncoding->addItem( "latin2" );
ui.comboBoxEncoding->addItem( "latin3" );
ui.comboBoxEncoding->addItem( "latin5" );
ui.comboBoxEncoding->addItem( "utf8" );
ui.comboBoxEncoding->addItem( "utf8x" );
ui.comboBoxEncoding->addItem( "ascii" );
ui.comboBoxEncoding->addItem( "decmulti" );
ui.comboBoxEncoding->addItem( "cp850" );
ui.comboBoxEncoding->addItem( "cp852" );
ui.comboBoxEncoding->addItem( "cp437" );
ui.comboBoxEncoding->addItem( "cp437de" );
ui.comboBoxEncoding->addItem( "cp865" );
ui.comboBoxEncoding->addItem( "applemac" );
ui.comboBoxEncoding->addItem( "next" );
ui.comboBoxEncoding->addItem( "ansinew" );
ui.comboBoxEncoding->addItem( "cp1252" );
ui.comboBoxEncoding->addItem( "cp1250" );
ui.comboBoxEncoding->addItem( "NONE" );


ui.listWidgetBabel->clear();
ui.listWidgetBabel->addItem("arabic" );
ui.listWidgetBabel->addItem("czech" );
ui.listWidgetBabel->addItem("english" );
ui.listWidgetBabel->addItem("farsi" );
ui.listWidgetBabel->addItem("finnish" );
ui.listWidgetBabel->addItem("francais" );
ui.listWidgetBabel->addItem("french" );
ui.listWidgetBabel->addItem("frenchb" );
ui.listWidgetBabel->addItem("german" );
ui.listWidgetBabel->addItem("greek" );
ui.listWidgetBabel->addItem("icelandic" );
ui.listWidgetBabel->addItem("italian" );
ui.listWidgetBabel->addItem("magyar" );
ui.listWidgetBabel->addItem("polish" );
ui.listWidgetBabel->addItem("portuguese" );
ui.listWidgetBabel->addItem("russian" );
ui.listWidgetBabel->addItem("slovak" );
ui.listWidgetBabel->addItem("spanish" );
}

void QuickBeamerDialog::updatePreview(const QString &theme)
{
QPixmap pixmap;
pixmap.load(":/images/beamer/"+theme+".png");

if (qApp->devicePixelRatio()>=2) pixmap.setDevicePixelRatio(qApp->devicePixelRatio());

labelImage->setPixmap(pixmap);
}

