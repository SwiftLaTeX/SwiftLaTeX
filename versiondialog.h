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

#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

#include "ui_versiondialog.h"

class VersionDialog : public QDialog  {
   Q_OBJECT
public:
	VersionDialog(QWidget *parent=0);
	~VersionDialog();
	Ui::VersionDialog ui;
private:
    QNetworkAccessManager manager;
    QNetworkReply       * reply;
    QTimer timer;
private slots:
  void gotoDownloadPage();
  void launchChecker();
  void showResultChecker();
  void stopChecker();
};


#endif
