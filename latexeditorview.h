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

#ifndef LATEXEDITORVIEW_H
#define LATEXEDITORVIEW_H

#include <qwidget.h>
#include <qfont.h>
#include <qcolor.h>
#include <QStackedWidget>
#include <QSplitter>
#include "latexeditor.h"
#include "linenumberwidget.h"
#include "findwidget.h"
#include "gotolinewidget.h"
#include "replacewidget.h"
#include "minisplitter.h"
#include "hunspell/hunspell.hxx"

class LatexEditorView : public QWidget  {
   Q_OBJECT
public: 
	LatexEditorView(QWidget *parent, QFont & efont,bool svn,bool line,QList<QColor> edcolors, QList<QColor> hicolors,bool inlinespelling=false, QString ignoredWords="",Hunspell *spellChecker=0,bool tabspaces=false,int tabwidth=4,const QKeySequence viewfocus=QKeySequence("Ctrl+Space"), QString name="", QStringList ulist=QStringList());
	~LatexEditorView();
  LatexEditor *editor;
  void changeSettings(QFont & new_font,bool svn,bool line);
  void showFind();
  void showFindNext();
  void showGoto();
  void showReplace();
private slots:
void updateFind();
void updateReplace();
void updateInSelection();
private:
  MiniSplitter *splitter;
  LineNumberWidget* m_lineNumberWidget;
  void setLineNumberWidgetVisible( bool );
  QStackedWidget *Stack;
  FindWidget *findwidget;
  ReplaceWidget *replacewidget;
  GotoLineWidget *gotolinewidget;
};

#endif
