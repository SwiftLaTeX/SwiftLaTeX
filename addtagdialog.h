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

#ifndef ADDTAGDIALOG_H
#define ADDTAGDIALOG_H

#include "ui_addtagdialog.h"

class AddTagDialog : public QDialog
{
    Q_OBJECT
public: 
	AddTagDialog(QWidget *parent = 0);
	~AddTagDialog();
	Ui::AddTagDialog ui;
};

#endif
