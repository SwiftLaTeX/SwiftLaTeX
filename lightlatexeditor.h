/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *   addons by Luis Silvestre                                              *
 *   contains some code from CLedit (C) 2010 Heinz van Saanen -GPL         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIGHTLATEXEDITOR_H
#define LIGHTLATEXEDITOR_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>

#include "lightlatexhighlighter.h"
#include "textblockselection.h"
#include "encodingprober/qencodingprober.h"


typedef  int UserBookmarkList[3];

class LightLatexEditor : public QPlainTextEdit  {
   Q_OBJECT
public:
LightLatexEditor(QWidget *parent,QFont & efont,QList<QColor> edcolors, QList<QColor> hicolors, QString name="");
~LightLatexEditor();
static void clearMarkerFormat(const QTextBlock &block, int markerId);
void gotoLine( int line );
bool search( const QString &expr, bool cs, bool wo, bool forward, bool startAtCursor, bool isRegExp);
int searchInSelection( const QString &expr, bool cs, bool wo, bool isRegExp, int start, int end);
void changeFont(QFont & new_font);
QString getEncoding();
void setEncoding(QString enc);
int getCursorPosition(int parag, int index);
void setCursorPosition(int para, int index);
int numoflines();
int searchLine( const QString &expr);
int linefromblock(const QTextBlock& p);
UserBookmarkList UserBookmark;
void selectword(int line, int col, QString word);
LightLatexHighlighter *highlighter;
void insertNewLine();
QRectF blockGeometry(const QTextBlock & block) {return blockBoundingGeometry(block).translated(contentOffset());}
int getLastNumLines();
void setLastNumLines(int n);

QString beginningLine();
void updateName(QString f);
void setTextCursor(const QTextCursor &cursor);
TextBlockSelection blockSelection;
public slots:
void matchAll();
void load( const QString &f );
void setColors(QList<QColor> colors);

private:
bool inBlockSelectionMode;
QString encoding;
int lastnumlines;
QString fname;
bool matchLeftPar ( QTextBlock currentBlock, int index, int numRightPar );
bool matchRightPar( QTextBlock currentBlock, int index, int numLeftPar );
void createParSelection( int pos );

int endBlock;
QString copyBlockSelection() const;
QColor colorBackground, colorLine, colorHighlight, colorCursor;
private slots:
void jumpToEndBlock();
void matchPar();
void ensureFinalNewLine();//Qt 4.7.1 bug
void gotoBookmark1();
void gotoBookmark2();
void gotoBookmark3();
void editFind();
void editGotoLine();

void removeBlockSelection(const QString &text = QString());
void slotSelectionChanged();
protected:
void paintEvent(QPaintEvent *event);
void contextMenuEvent(QContextMenuEvent *e);
void mouseMoveEvent(QMouseEvent *);
void mousePressEvent(QMouseEvent *);
QMimeData *createMimeDataFromSelection() const;
bool canInsertFromMimeData(const QMimeData *source) const;
void insertFromMimeData(const QMimeData *source);

signals:
void updatelineWidget();
void requestFind();
void requestGotoLine();
};

#endif
