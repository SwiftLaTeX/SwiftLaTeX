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

#ifndef LATEXEDITOR_H
#define LATEXEDITOR_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QCompleter>
#include <QDateTime>
#include <QTimer>
#include <QKeySequence>
#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QQueue>
#include <QDebug>
#include <QScriptEngine>
//#include <QScriptEngineDebugger>

#include "latexhighlighter.h"
#include "textblockselection.h"
#include "hunspell/hunspell.hxx"

typedef  int UserBookmarkList[3];

class StructItem {
public:
int line;
QString item;
int type;
QTextCursor cursor;
StructItem(int l, const QString& it, int t,const QTextCursor& curs): line(l),item(it),type(t),cursor(curs) { };
bool operator==( const StructItem other ) const
    {
    return ((item==other.item) && (type==other.type));
    }
bool operator<( const StructItem other ) const
    {
    return (item<other.item);
    }
};

struct updateStruct {
bool isdirty;
QList<StructItem> list;
};

class LatexEditor : public QPlainTextEdit  {
   Q_OBJECT
public:
LatexEditor(QWidget *parent,QFont & efont, QList<QColor> edcolors, QList<QColor> hicolors,bool inlinespelling=false, QString ignoredWords="",Hunspell *spellChecker=0,bool tabspaces=false,int tabwidth=4,const QKeySequence viewfocus=QKeySequence("Ctrl+Space"), QString name="",QStringList ulist=QStringList());
~LatexEditor();
static void clearMarkerFormat(const QTextBlock &block, int markerId);
void gotoLine( int line );
bool search( const QString &expr, bool cs, bool wo, bool forward, bool startAtCursor, bool isRegExp);
int searchInSelection( const QString &expr, bool cs, bool wo, bool isRegExp, int start, int end);
void replace( const QString &r,bool isRegExp,const QString &o);
void commentSelection();
void uncommentSelection();
void indentSelection();
void unindentSelection();
void changeFont(QFont & new_font);
QString getEncoding();
void setEncoding(QString enc);
int getCursorPosition(int parag, int index);
void setCursorPosition(int para, int index);
void removeOptAlt();
int numoflines();
int searchLine( const QString &expr);
int linefromblock(const QTextBlock& p);
UserBookmarkList UserBookmark;
void selectword(int line, int col, QString word);
LatexHighlighter *highlighter;
void setCompleter(QCompleter *completer);
QCompleter *completer() const;
QStringList alwaysignoredwordList;
void setSpellChecker(Hunspell * checker);
void activateInlineSpell(bool enable);
Hunspell * pChecker;
void insertNewLine();
void fold(int start, int end);
void unfold(int start, int end);
QRectF blockGeometry(const QTextBlock & block) {return blockBoundingGeometry(block).translated(contentOffset());}
//const QRectF blockGeometry(const QTextBlock & block);
//const QRectF blockRect(const QTextBlock & block) {return blockBoundingRect(block).translated(contentOffset());};
QMap<int,int> foldedLines;
QMap<int,int> foldableLines;



QStringList structlist, structitem;
//QList<int> structtype, structpos, structlen;
QList<QTextCursor> structcursor;

void checkStructUpdate(QTextDocument *doc,int blockpos,QString text,int line);

int getLastNumLines();
void setLastNumLines(int n);

QDateTime getLastSavedTime();
void setLastSavedTime(QDateTime t);
void setTabSettings(bool tabspaces,int tabwidth);
void setKeyViewerFocus(QKeySequence s);
void updateName(QString f);


const QList<StructItem> getStructItems() const {return StructItemsList; }



QString beginningLine();

void setTextCursor(const QTextCursor &cursor);

TextBlockSelection blockSelection;
int insertWithMemoryIndent(QString t);

/*** SCRIPT ***/
QString selectedText() { return textCursor().selectedText().replace(QChar(QChar::ParagraphSeparator), "\n"); }
QString text() { return toPlainText(); }
int cursorPosition() const { return textCursor().position(); }
int selectionStart() const { return textCursor().selectionStart(); }
int selectionLength() const { return textCursor().selectionEnd() - textCursor().selectionStart(); }
Q_PROPERTY(int cursorPosition READ cursorPosition STORED false)
Q_PROPERTY(QString selection READ selectedText STORED false)
Q_PROPERTY(int selectionStart READ selectionStart STORED false)
Q_PROPERTY(int selectionLength READ selectionLength STORED false)
Q_PROPERTY(QString text READ text STORED false)

void ExecuteScript(QString location);
int getlastSaveRevision() { return lastSaveRevision; }
void resetRevisions();
void updateRevisions();

public slots:
void matchAll();
void setHightLightLine();
void clearHightLightLine();
virtual void paste();
virtual void cut();
void setCursorVisible() {ensureCursorVisible ();};
void setColors(QList<QColor> colors);
void setUserTagsList(QStringList utlist);
void undoText();
void redoText();

void selectRange(int start, int length = 0);
void insertText(QString text);

void setUncommittedLines(QList<int> lines);
private:
int lastSaveRevision;
bool overmode;
QStringList userTagsList;
bool inBlockSelectionMode;
QKeySequence vfocus;
QString fname;
bool tabSpaces;
int tabWidth;
QDateTime lastSavedTime;
QList<StructItem> StructItemsList, OldStructItemsList;
QString encoding;
int lastnumlines;
//QString textUnderCursor() const;
QString commandUnderCursor() const;
QStringList fullcommandUnderCursor();
QCompleter *c;
QString spell_dic, spell_encoding;
QStringList ignoredwordList, hardignoredwordList;
bool inlinecheckSpelling;
bool isWordSeparator(QChar c) const;
bool isSpace(QChar c) const;

bool matchLeftPar ( QTextBlock currentBlock, int index, int numRightPar );
bool matchRightPar( QTextBlock currentBlock, int index, int numLeftPar );
void createParSelection( int pos );

bool matchLeftLat ( QTextBlock currentBlock, int index, int numRightLat, int bpos );
bool matchRightLat( QTextBlock currentBlock, int index, int numLeftLat, int epos );
void createLatSelection(int start, int end );
int endBlock;
QTimer highlightRemover;
bool highlightLine;
QString copyBlockSelection() const;
QColor colorBackground, colorLine, colorHighlight, colorCursor;

QScriptEngine fScriptEngine;


private slots:
void correctWord();
void checkSpellingWord();
void checkSpellingSelection();
void checkSpellingDocument();
void insertCompletion(const QString &completion);
void jumpToPdf();
void jumpToEndBlock();
void requestNewNumLines(int n);

void matchPar();
void matchLat();

void ensureFinalNewLine();//Qt 4.7.1 bug
void removeBlockSelection(const QString &text = QString());
void slotSelectionChanged();

protected:
void paintEvent(QPaintEvent *event);
void contextMenuEvent(QContextMenuEvent *e);
void keyPressEvent ( QKeyEvent * e );
void focusInEvent(QFocusEvent *e);
void mouseMoveEvent(QMouseEvent *);
void mousePressEvent(QMouseEvent *);
QMimeData *createMimeDataFromSelection() const;
bool canInsertFromMimeData(const QMimeData *source) const;
void insertFromMimeData(const QMimeData *source);
void dropEvent(QDropEvent *event);

signals:
void spellme();
void tooltiptab();
void requestpdf(int );
void setBlockRange(int,int);
void updatelineWidget();
void requestUpdateStructure();
void requestGotoStructure(int);
void poshaschanged(int,int);
void numLinesChanged(int);
};

#endif
