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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "ui_exportdialog.h"

#include <QProcess>
#include <QPointer>

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    ExportDialog(QWidget* parent = 0, QString f="", QString ep="");
	~ExportDialog();
Ui::ExportDialog ui;

private slots:
void browseHtlatex();
void runHtlatex();
void readFromStderr();
void readFromStdoutput();
void SlotEndProcess(int err);

private :
QString filename;
QString extra_path;
QPointer<QProcess> proc;
};

#endif
