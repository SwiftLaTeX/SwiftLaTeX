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

#include "logeditor.h"
#include <QRegExp>
#include <QPainter>
#include <QDebug>

LogEditor::LogEditor(QWidget *parent) : QPlainTextEdit(parent)
{
//setToolTip(tr("Click to jump to the line"));
highlighter = new LogHighlighter(document());

connect(this, SIGNAL(cursorPositionChanged()),this, SLOT(update()));
setReadOnly(true);
setEnabled(true);
setFocusPolicy(Qt::WheelFocus);
setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
}
LogEditor::~LogEditor(){
}

void LogEditor::insertLine(QString l)
{
appendPlainText(l);
}

void LogEditor::setCursorPosition(int para, int index)
{
QTextCursor cur=textCursor();
int i = 0;
QTextBlock p = document()->begin();
while ( p.isValid() ) {
	if (para==i) break;
	i++;
p = p.next();
}
int pos=p.position();
cur.movePosition(QTextCursor::End);
setTextCursor(cur);
cur.setPosition(pos+index,QTextCursor::MoveAnchor);
setTextCursor(cur);
ensureCursorVisible();
}

void LogEditor::mousePressEvent (QMouseEvent *e)
{
QPlainTextEdit::mousePressEvent(e);
QString content=textCursor().block().text();
 int Start, End;
 bool ok;
 QString s;
 QString line="";
 //// l. ///
 s = content;
 Start=End=0;
 Start=s.indexOf(QRegExp("l.[0-9]"), End);
 if (Start!=-1)
  {
  Start=Start+2;
  s=s.mid(Start,s.length());
  End=s.indexOf(QRegExp("[ a-zA-Z.\\-]"),0);
  if (End!=-1)
    line=s.mid(0,End);
  else
    line=s.mid(0,s.length());
  };
 //// line ///
 s = content;
 Start=End=0;
 Start=s.indexOf(QRegExp("line [0-9]"), End);
 if (Start!=-1)
  {
  Start=Start+5;
  s=s.mid(Start,s.length());
  End=s.indexOf(QRegExp("[ a-zA-Z.\\-]"),0);
  if (End!=-1)
    line=s.mid(0,End);
  else
    line=s.mid(0,s.length());
  };
 //// lines ///
 s = content;
 Start=End=0;
 Start=s.indexOf(QRegExp("lines [0-9]"), End);
 if (Start!=-1)
  {
  Start=Start+6;
  s=s.mid(Start,s.length());
  End=s.indexOf(QRegExp("[ a-zA-Z.\\-]"),0);
  if (End!=-1)
    line=s.mid(0,End);
  else
    line=s.mid(0,s.length());
  };
int l=line.toInt(&ok,10)-1;
if (ok)
 {
emit clickonline(l);
 }

}

void LogEditor::paintEvent(QPaintEvent *event)
{
QRect rect = cursorRect();
QRect rectbis=rect;
rectbis.setX(0);
rectbis.setWidth(viewport()->width());
QPainter painter(viewport());
const QBrush brush(QColor("#000000"));
const QBrush brushbis(QColor("#ececec"));
painter.fillRect(rectbis, brushbis);
painter.fillRect(rect, brush);
painter.end();
QPlainTextEdit::paintEvent(event);
}
