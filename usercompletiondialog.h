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

#ifndef USERCOMPLETIONDIALOG_H
#define USERCOMPLETIONDIALOG_H

#include "ui_usercompletiondialog.h"

#include <QListWidgetItem>
#include <QList>
#include <QStringList>



class UserCompletionDialog : public QDialog  {
   Q_OBJECT
public:
	UserCompletionDialog(QWidget *parent=0, QStringList extraCompletion=QStringList(""));
	~UserCompletionDialog();
	Ui::UserCompletionDialog ui;
	QStringList userlist;
private slots:
void slotItemClicked(QListWidgetItem* item);
void actionDelete();
void actionReplace();
void actionAddCommand();
};


#endif
