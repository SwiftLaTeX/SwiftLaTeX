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

#ifndef UNICODEDIALOG_H
#define UNICODEDIALOG_H

#include "ui_unicodedialog.h"
#include "unicodeview.h"
#include <QVBoxLayout>

class UnicodeDialog : public QDialog  {
   Q_OBJECT
public:
	UnicodeDialog(QWidget *parent=0);
	~UnicodeDialog();
	Ui::UnicodeDialog ui;
void init(QFont & efont,bool line, QList<QColor> edcolors, QList<QColor> hicolors);
private :
  UnicodeView* unicodeviewWidget;
  QVBoxLayout* mainlay;
private slots:
    void chooseFile();
    void convertFile();
    void resetEditor();
};


#endif
