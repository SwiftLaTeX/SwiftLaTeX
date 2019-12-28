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

#ifndef SOURCEVIEW_H
#define SOURCEVIEW_H

#include <QWidget>
#include <QFont>
#include <QColor>
#include <QStackedWidget>
#include <QToolBar>
#include <QLabel>
#include <QSplitter>
#include "lightlatexeditor.h"
#include "lightlinenumberwidget.h"
#include "lightfindwidget.h"
#include "lightgotolinewidget.h"
#include "minisplitter.h"
#include "dropshadowlabel.h"

class SourceView : public QWidget  {
   Q_OBJECT
public: 
SourceView(QWidget *parent, QFont & efont,bool line, QList<QColor> edcolors, QList<QColor> hicolors);
~SourceView();
  LightLatexEditor *editor;
  QToolBar *centralToolBar;
  void changeSettings(QFont & new_font,bool line);
private:
  MiniSplitter *splitter;
  LightLineNumberWidget* m_lineNumberWidget;
  void setLineNumberWidgetVisible( bool );
  QStackedWidget *Stack;
  DropShadowLabel* titleLabel;
  LightFindWidget *findwidget;
  LightGotoLineWidget *gotolinewidget;
  QString lastdocument;
private slots:
  void fileOpen();
  void showFind();
  void showGoto();
  void checkDiff();
signals:
void showDiff();
};

#endif
