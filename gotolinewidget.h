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

#ifndef GOTOLINEWIDGET_H
#define GOTOLINEWIDGET_H

#include "ui_gotolinewidget.h"
#include "latexeditor.h"

class GotoLineWidget : public QWidget
{ 
    Q_OBJECT

public:
    GotoLineWidget(QWidget* parent = 0);
    ~GotoLineWidget();
    Ui::GotoLineWidget ui;

public slots:
    virtual void gotoLine();
    void SetEditor(LatexEditor *ed);
    void doHide();
protected:
    LatexEditor *editor;
void keyPressEvent ( QKeyEvent * e );
signals:
void requestHide();
};

#endif 
