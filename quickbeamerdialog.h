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

#ifndef QUICKBEAMERDIALOG_H
#define QUICKBEAMERDIALOG_H

#include "ui_quickbeamerdialog.h"
#include <QLabel>




class QuickBeamerDialog : public QDialog  {
   Q_OBJECT
public:
	QuickBeamerDialog(QWidget *parent=0, const char *name=0);
	~QuickBeamerDialog();
	Ui::QuickBeamerDialog ui;
QLabel *labelImage;

public slots:
    void Init();
private slots:
void updatePreview(const QString &theme);
};


#endif
