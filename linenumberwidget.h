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

#ifndef __linenumberwidget_h
#define __linenumberwidget_h

#include <QtGui>
#include <QWidget>
#include <QFont>
#include <QPaintEvent>
#include "latexeditor.h"


class LineNumberWidget: public QWidget
{
    Q_OBJECT
public:
    LineNumberWidget(LatexEditor*, QWidget* parent,bool svn);
    virtual ~LineNumberWidget();

public slots:
    void doRepaint() { repaint(); }
    void setFont(QFont efont);
    void setSvn(bool svn);
    void setRange(int s, int e);

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent( QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *e);

private:
    LatexEditor* m_editor;
    QFont numfont;
    int start, end;
    bool m_svn;
};

#endif // __linenumberwidget_h
