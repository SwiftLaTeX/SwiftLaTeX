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

#ifndef QUICKXELATEXDIALOG_H
#define QUICKXELATEXDIALOG_H

#include "ui_quickxelatexdialog.h"
#include "addoptiondialog.h"



class QuickXelatexDialog : public QDialog  {
   Q_OBJECT
public:
	QuickXelatexDialog(QWidget *parent=0, const char *name=0);
	~QuickXelatexDialog();
	Ui::QuickXelatexDialog ui;

public:

  QStringList otherClassList, otherPaperList, otherOptionsList, otherBabelList;

public slots:
    void Init();
private slots:
    void addUserClass();
    void addUserPaper();
    void addUserOptions();
    void addUserBabel();
private :
AddOptionDialog *dlg;
};


#endif
