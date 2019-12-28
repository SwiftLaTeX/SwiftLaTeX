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

#ifndef LIGHTFINDWIDGET_H
#define LIGHTFINDWIDGET_H

#include "ui_findwidget.h"
#include "lightlatexeditor.h"

class LightFindWidget : public QWidget
{ 
    Q_OBJECT

public:
    LightFindWidget(QWidget* parent = 0);
    ~LightFindWidget();
    Ui::FindWidget ui;
public slots:
    virtual void doFind();
    void SetEditor(LightLatexEditor *ed);
    void doHide();
private slots:
    void expand(bool e);
    void updateSelection(bool e);
private :
  int startpos, endpos;
protected:
    LightLatexEditor *editor;
signals:
void requestHide();
void requestExtension();
};

#endif
