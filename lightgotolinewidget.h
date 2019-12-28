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

#ifndef LIGHTGOTOLINEWIDGET_H
#define LIGHTGOTOLINEWIDGET_H

#include "ui_gotolinewidget.h"
#include "lightlatexeditor.h"

class LightGotoLineWidget : public QWidget
{ 
    Q_OBJECT

public:
    LightGotoLineWidget(QWidget* parent = 0);
    ~LightGotoLineWidget();
    Ui::GotoLineWidget ui;

public slots:
    virtual void gotoLine();
    void SetEditor(LightLatexEditor *ed);
    void doHide();
protected:
    LightLatexEditor *editor;
signals:
void requestHide();
};

#endif 
