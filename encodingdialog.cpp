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

#include "encodingdialog.h"
#include <QTextCodec>

EncodingDialog::EncodingDialog(QWidget *parent, const char *name)
    :QDialog( parent)
{
setWindowTitle("Texmaker");
setModal(true);
ui.setupUi(this);
ui.comboBoxEncoding->addItem("UTF-8");
foreach (int mib, QTextCodec::availableMibs()) 
	{
	QTextCodec *codec = QTextCodec::codecForMib(mib);
	if (codec->name()!="UTF-8") ui.comboBoxEncoding->addItem(codec->name());
	}
}

EncodingDialog::~EncodingDialog(){
}

