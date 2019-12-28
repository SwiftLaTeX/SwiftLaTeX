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

#ifndef USERTOOLDIALOG_H
#define USERTOOLDIALOG_H

#include <QString>
#include "userquickdialog.h"
#include "ui_usertooldialog.h"

typedef QString userCd[5];


class UserToolDialog : public QDialog  {
   Q_OBJECT
public: 
	UserToolDialog(QWidget *parent=0, QString name="", QStringList names=QStringList(""), QStringList commands=QStringList(""));
	~UserToolDialog();
	Ui::UserToolDialog ui;

      userCd Name,Tool;

private:
    int previous_index;
    UserQuickDialog *userquickdlg;
    QStringList usualNames, usualCommands;
public slots:
    void init();

private slots:
    void change(int index);
    void slotOk();
    void updateItem();
    void userQuickWizard();
};

#endif
