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

#ifndef REPLACEWIDGET_H
#define REPLACEWIDGET_H

#include "ui_replacewidget.h"
#include "latexeditor.h"

class ReplaceWidget : public QWidget
{ 
    Q_OBJECT

public:
    ReplaceWidget(QWidget* parent = 0);
    ~ReplaceWidget();
    Ui::ReplaceWidget ui;

public slots:
    virtual void doReplace();
    virtual void doReplaceAll();
    void SetEditor(LatexEditor *ed);
    void doHide();
private slots:
    void expand(bool e);
    void updateSelection(bool e);
    void updateReg(bool e);
private :
  int startpos, endpos, deltacol;
  void replaceSelection();
protected:
    LatexEditor *editor;
void keyPressEvent ( QKeyEvent * e );
signals:
void requestHide();
void requestExtension();
};

#endif 
