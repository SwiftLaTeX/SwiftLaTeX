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

#ifndef LOGEDITOR_H
#define LOGEDITOR_H

#include <QPlainTextEdit>
#include <QString>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QWidget>

#include "loghighlighter.h"

class LogEditor : public QPlainTextEdit {
  Q_OBJECT
public:
  LogEditor(QWidget *parent);
  ~LogEditor();

public slots:
  void insertLine(QString l);
  void setCursorPosition(int parag, int index);

protected:
  virtual void mousePressEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *event);
signals:
  void clickonline(int l);

private:
  LogHighlighter *highlighter;
};

#endif
