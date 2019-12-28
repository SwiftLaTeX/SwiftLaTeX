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


#ifndef LIGHTLATEXHIGHLIGHTER_H
#define LIGHTLATEXHIGHLIGHTER_H


#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QColor>
#include <QTextBlockUserData>
//#include "latexeditor.h"

class QTextDocument;
class LightLatexEditor;

class LightLatexHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    LightLatexHighlighter(QTextDocument *parent = 0);
    ~LightLatexHighlighter();
    QColor ColorStandard, ColorComment, ColorMath, ColorCommand, ColorKeyword, ColorVerbatim, ColorTodo, ColorKeywordGraphic, ColorNumberGraphic;
    QStringList KeyWords, KeyWordsGraphic, KeyWordsGraphicBis;
public slots:
void setColors(QList<QColor> colors);
void SetEditor(LightLatexEditor *ed);
void setModeGraphic(bool m);
private :
bool isWordSeparator(QChar c) const;
bool isSpace(QChar c) const;
bool isGraphic;
protected:
LightLatexEditor *editor;
void highlightBlock(const QString &text);
};


#endif
