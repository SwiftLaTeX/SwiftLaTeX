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
#include <QPixmap>
#include "addtagdialog.h"


AddTagDialog::AddTagDialog(QWidget *parent)
    : QDialog( parent)
{
setModal(true);
ui.setupUi(this);
QPixmap pixmap1, pixmap2;

if (qApp->devicePixelRatio()>=2)
  {
  pixmap1.load(":/images/trigger@2x.png");
  pixmap1.setDevicePixelRatio(qApp->devicePixelRatio());
  pixmap2.load(":/images/triggerbis@2x.png");
  pixmap2.setDevicePixelRatio(qApp->devicePixelRatio());
  }
else 
  {
  pixmap1.load(":/images/trigger.png");
  pixmap2.load(":/images/triggerbis.png");
  }

ui.label_4->setPixmap(pixmap1);
ui.label_5->setPixmap(pixmap2);
}
AddTagDialog::~AddTagDialog()
{
}

