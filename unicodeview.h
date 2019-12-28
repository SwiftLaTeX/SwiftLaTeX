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

#ifndef UNICODEVIEW_H
#define UNICODEVIEW_H

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

class UnicodeView : public QWidget  {
   Q_OBJECT
public: 
UnicodeView(QWidget *parent, QFont & efont,bool line, QList<QColor> edcolors, QList<QColor> hicolors);
~UnicodeView();
  LightLatexEditor *editor;
  QToolBar *centralToolBar;
  void changeSettings(QFont & new_font,bool line);
private:
  MiniSplitter *splitter;
  LightLineNumberWidget* m_lineNumberWidget;
  void setLineNumberWidgetVisible( bool );
  QStackedWidget *Stack;
  QLabel* titleLabel;
  LightFindWidget *findwidget;
  LightGotoLineWidget *gotolinewidget;
  QString lastdocument;
private slots:
  void fileSave();
  void showFind();
  void showGoto();
};

#endif
