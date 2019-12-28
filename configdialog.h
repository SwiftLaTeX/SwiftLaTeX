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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "ui_configdialog.h"

#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "keysequencedialog.h"
#include "userquickdialog.h"

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget* parent = 0);
	~ConfigDialog();
Ui::ConfigDialog ui;

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private slots:
void createIcons();
void browseAspell();
void browseSvn();
void browseLatex();
void browseDvips();
void browseBibtex();
void browseMakeindex();
void browseDviviewer();
void browsePsviewer();
void browsePdflatex();
void browseXelatex();
void browseLualatex();
void browseDvipdfm();
void browsePs2pdf();
void browsePdfviewer();
void browseMetapost();
void browseGhostscript();
void browseAsymptote();
void browseLatexmk();
void configureShortCut(QTableWidgetItem *item);
void configureKeyToggle();
void userQuickWizard();
void asyQuickWizard();
void restoreColors();
void darkColors();
private :
KeySequenceDialog *keydlg;
UserQuickDialog *userquickdlg;
};

#endif
