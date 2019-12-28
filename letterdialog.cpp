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

#include "letterdialog.h"

LetterDialog::LetterDialog(QWidget *parent, const char *name)
    :QDialog( parent)
{
setWindowTitle(name);
setModal(true);
ui.setupUi(this);

ui.comboBoxPt->insertItem(0, "10pt" );
ui.comboBoxPt->insertItem(1, "11pt" );
ui.comboBoxPt->insertItem(2, "12pt" );

ui.comboBoxPaper->insertItem(0, "a4paper" );
ui.comboBoxPaper->insertItem(1, "a5paper" );
ui.comboBoxPaper->insertItem(2, "b5paper" );
ui.comboBoxPaper->insertItem(3, "letterpaper" );
ui.comboBoxPaper->insertItem(4, "legalpaper" );
ui.comboBoxPaper->insertItem(5, "executivepaper" );

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


ui.checkBox->setChecked( true );
setWindowTitle(tr("Quick Letter"));
}

LetterDialog::~LetterDialog(){
}
