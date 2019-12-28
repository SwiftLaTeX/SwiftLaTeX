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

#include "versiondialog.h"

#include <QtCore/QUrl>
#include <QDesktopServices>
#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define VERSION_STR STRINGIFY(TEXMAKERVERSION)

VersionDialog::VersionDialog(QWidget *parent)
    :QDialog( parent)
{
ui.setupUi(this);
timer.setSingleShot(true);
connect(&timer, SIGNAL(timeout()), this, SLOT(stopChecker()));
ui.lineEditCurrent->setText(QLatin1String(VERSION_STR));
ui.lineEditAvailable->setText(QString::fromUtf8("?.?.?"));
connect(ui.pushButtonDownload, SIGNAL( clicked() ), this, SLOT( gotoDownloadPage() ) );
connect(ui.pushButtonCheck, SIGNAL( clicked() ), this, SLOT( launchChecker() ) );
}

VersionDialog::~VersionDialog(){
}

void VersionDialog::gotoDownloadPage()
{
QDesktopServices::openUrl(QUrl("http://www.xm1math.net/texmaker/download.html"));
}

void VersionDialog::launchChecker()
{
ui.pushButtonCheck->setEnabled(false);
timer.start(10000);
reply = manager.get (  QNetworkRequest(QUrl("http://www.xm1math.net/texmaker/version.txt"))  );
QObject::connect (reply, SIGNAL (finished()),this, SLOT(showResultChecker()));
}

void VersionDialog::showResultChecker()
{
timer.stop();
if (reply->error()) ui.lineEditAvailable->setText(tr("Error"));
else ui.lineEditAvailable->setText(QString(reply->readAll()));
ui.pushButtonCheck->setEnabled(true);
}

void VersionDialog::stopChecker()
{
ui.lineEditAvailable->setText(tr("Error"));
QObject::disconnect (reply, SIGNAL (finished()),this, SLOT(showResultChecker()));
reply->abort();
ui.pushButtonCheck->setEnabled(true);
}
