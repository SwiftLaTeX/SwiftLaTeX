/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *   addons by Luis Silvestre, Julián Moreno Patiño                        *
 *   contains some code from CLedit (C) 2010 Heinz van Saanen -GPL         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "latexeditor.h"

#include <QPainter>
#include <QTextLayout>
#include <QMetaProperty>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QPalette>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QTextCodec>
#include <QFile>
#include <QFileInfo>
#include <QTextDocumentFragment>
#include <QFuture>
#include <QtCore>
#include <QtConcurrent>
#include <QScriptValue>
//#include <QPlatformInputContext>


#include "blockdata.h"

QMap<QAction*,QString> spellingLookup;

static void convertToPlainText(QString &txt)
{
    QChar *uc = txt.data();
    QChar *e = uc + txt.size();

    for (; uc != e; ++uc) {
        switch (uc->unicode()) {
        case 0xfdd0: // QTextBeginningOfFrame
        case 0xfdd1: // QTextEndOfFrame
        case QChar::ParagraphSeparator:
        case QChar::LineSeparator:
            *uc = QLatin1Char('\n');
            break;
        case QChar::Nbsp:
            *uc = QLatin1Char(' ');
            break;
        default:
            ;
        }
    }
}
extern struct updateStruct updateStructureList(QTextDocument *doc,int blockpos,QString text,int line,QList<StructItem> list)
{
bool haschanged=false;
QList<StructItem> templist=list;
int i=line;
int j=0;
StructItem oldItem=StructItem(i,"",-1,QTextCursor());
StructItem oldLabelItem=StructItem(i,"",-1,QTextCursor());
for ( j=templist.count()-1;j>=0; --j ) 
  {
    if ( templist[j].cursor.block().blockNumber()==line)
    {
      if (templist[j].type==1) oldLabelItem=templist[j];
      else oldItem=templist[j];
      templist.removeAt(j);
    }
  }
StructItem newItem=StructItem(i,"",-1,QTextCursor());
StructItem newLabelItem=StructItem(i,"",-1,QTextCursor());
bool found=false;
int tagStart, tagEnd,offset,numLeftPar;
int tagComment=text.indexOf("%");
QString s;
QString struct_level1="part";
QString struct_level2="chapter";
QString struct_level3="section";
QString struct_level4="subsection";
QString struct_level5="subsubsection";


//// label ////
tagStart=tagEnd=offset=0;
s=text;
tagStart=s.indexOf("\\label{", tagEnd);
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.mid(tagStart+7,s.length());
	s=s.trimmed();
	tagStart=s.indexOf("}", tagEnd);
	if (tagStart!=-1)
		{
		s=s.mid(0,tagStart);
		if (s.startsWith("{")) s=s.remove(0,1);
		if (s.endsWith("}")) s=s.remove(s.length()-1,1);
		QTextCursor	cursor(doc);
		cursor.setPosition(blockpos + offset+1);
		newLabelItem=StructItem(i,s,1,cursor);
		//found=true;
		}
	};

//section
tagStart=tagEnd=offset=0;
numLeftPar=0;
s=text; 
tagStart=s.indexOf(QRegExp("\\\\"+struct_level3+"\\*?[\\{\\[]"), tagEnd);
tagEnd=-1;
offset=tagStart;
	if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.trimmed();
	tagStart=s.indexOf("{");
	s=s.mid(tagStart+1,s.length());
	tagStart=0;
	for (int index=0;index<s.length();++index)
	  {
	  if (s.mid(index,1)=="{")   
	      {
	      ++numLeftPar;
	      continue;
	      }
	  if (s.mid(index,1)=="}" && numLeftPar == 0 ) 
	    {
	    tagEnd=index;
	    break;
	    }
	  if (s.mid(index,1)=="}" && numLeftPar != 0 ) --numLeftPar;
	  }
	if ((tagEnd!=-1) && (tagStart!=-1) )
	  {
	  s=s.mid(tagStart,tagEnd);
	  QTextCursor	cursor(doc);
	  cursor.setPosition(blockpos + offset+1);
	  newItem=StructItem(i,s,6,cursor);
	  found=true;
	  }
	};
if (!found)
{
//// subsection ////
tagStart=tagEnd=offset=0;
numLeftPar=0;
s=text;
tagStart=s.indexOf(QRegExp("\\\\"+struct_level4+"\\*?[\\{\\[]"), tagEnd);
tagEnd=-1;
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.trimmed();
	tagStart=s.indexOf("{");
	s=s.mid(tagStart+1,s.length());
	tagStart=0;
	for (int index=0;index<s.length();++index)
	  {
	  if (s.mid(index,1)=="{")   
	      {
	      ++numLeftPar;
	      continue;
	      }
	  if (s.mid(index,1)=="}" && numLeftPar == 0 ) 
	    {
	    tagEnd=index;
	    break;
	    }
	  if (s.mid(index,1)=="}" && numLeftPar != 0 ) --numLeftPar;
	  }
	if ((tagEnd!=-1) && (tagStart!=-1) )
	  {
	  s=s.mid(tagStart,tagEnd);
	  QTextCursor	cursor(doc);
	  cursor.setPosition(blockpos + offset+1);
	  newItem=StructItem(i,s,7,cursor);
	  found=true;
	  }
	};
}
if (!found)
{
//// subsubsection ////
tagStart=tagEnd=offset=0;
numLeftPar=0;
s=text;
tagStart=s.indexOf(QRegExp("\\\\"+struct_level5+"\\*?[\\{\\[]"), tagEnd);
tagEnd=-1;
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.trimmed();
	tagStart=s.indexOf("{");
	s=s.mid(tagStart+1,s.length());
	tagStart=0;
	for (int index=0;index<s.length();++index)
	  {
	  if (s.mid(index,1)=="{")   
	      {
	      ++numLeftPar;
	      continue;
	      }
	  if (s.mid(index,1)=="}" && numLeftPar == 0 ) 
	    {
	    tagEnd=index;
	    break;
	    }
	  if (s.mid(index,1)=="}" && numLeftPar != 0 ) --numLeftPar;
	  }
	if ((tagEnd!=-1) && (tagStart!=-1) )
	  {
	  s=s.mid(tagStart,tagEnd);
	  QTextCursor	cursor(doc);
	  cursor.setPosition(blockpos + offset+1);
	  newItem=StructItem(i,s,8,cursor);
	  found=true;
	  }
	};
}
if (!found)
{
//// block ////
tagStart=tagEnd=offset=0;
s=text;
tagStart=s.indexOf(QRegExp("\\\\begin\\{block\\}\\*?[\\{\\[]"), tagEnd);
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	tagStart=s.indexOf("begin{block}", tagEnd);
	s=s.mid(tagStart+12,s.length());
	if (s.startsWith("{")) s=s.remove(0,1);
	if (s.endsWith("}")) s=s.remove(s.length()-1,1);
	if (!s.isEmpty())
	  {
	  QTextCursor	cursor(doc);
	  cursor.setPosition(blockpos + offset+1);
	  newItem=StructItem(i,s,0,cursor);
	  found=true;
	  }
	};
}

if (!found)
{
//// include ////
tagStart=tagEnd=offset=0;
s=text;
tagStart=s.indexOf("\\include{", tagEnd);
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.mid(tagStart+8,s.length());
	s=s.trimmed();
	tagStart=s.indexOf("}", tagEnd);
	if (tagStart!=-1)
		{
		s=s.mid(0,tagStart+1);
		if (s.startsWith("{")) s=s.remove(0,1);
		if (s.endsWith("}")) s=s.remove(s.length()-1,1);
		QTextCursor	cursor(doc);
		cursor.setPosition(blockpos + offset+1);
		newItem=StructItem(i,s,2,cursor);
		found=true;
		}
	};
}
if (!found)
{
//// input ////
tagStart=tagEnd=offset=0;
s=text;
tagStart=s.indexOf("\\input{", tagEnd);
offset=tagStart;
	if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.mid(tagStart+6,s.length());
	s=s.trimmed();
	tagStart=s.indexOf("}", tagEnd);
	if (tagStart!=-1)
		{
		s=s.mid(0,tagStart+1);
		if (s.startsWith("{")) s=s.remove(0,1);
		if (s.endsWith("}")) s=s.remove(s.length()-1,1);
		QTextCursor	cursor(doc);
		cursor.setPosition(blockpos + offset+1);
		newItem=StructItem(i,s,3,cursor);
		found=true;
		}
	};
}
if (!found)
{
//// part ////
tagStart=tagEnd=offset=0;
numLeftPar=0;
s=text;
tagStart=s.indexOf(QRegExp("\\\\"+struct_level1+"\\*?[\\{\\[]"), tagEnd);
tagEnd=-1;
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.trimmed();
	tagStart=s.indexOf("{");
	s=s.mid(tagStart+1,s.length());
	tagStart=0;
	for (int index=0;index<s.length();++index)
	  {
	  if (s.mid(index,1)=="{")   
	      {
	      ++numLeftPar;
	      continue;
	      }
	  if (s.mid(index,1)=="}" && numLeftPar == 0 ) 
	    {
	    tagEnd=index;
	    break;
	    }
	  if (s.mid(index,1)=="}" && numLeftPar != 0 ) --numLeftPar;
	  }
	if ((tagEnd!=-1) && (tagStart!=-1) )
	  {
	  s=s.mid(tagStart,tagEnd);
	  QTextCursor	cursor(doc);
	  cursor.setPosition(blockpos + offset+1);
	  newItem=StructItem(i,s,4,cursor);
	  found=true;
	  }
	};
}
if (!found)
{
//// chapter ////
tagStart=tagEnd=offset=0;
numLeftPar=0;
s=text;
tagStart=s.indexOf(QRegExp("\\\\"+struct_level2+"\\*?[\\{\\[]"), tagEnd);
tagEnd=-1;
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.trimmed();
	tagStart=s.indexOf("{");
	s=s.mid(tagStart+1,s.length());
	tagStart=0;
	for (int index=0;index<s.length();++index)
	  {
	  if (s.mid(index,1)=="{")   
	      {
	      ++numLeftPar;
	      continue;
	      }
	  if (s.mid(index,1)=="}" && numLeftPar == 0 ) 
	    {
	    tagEnd=index;
	    break;
	    }
	  if (s.mid(index,1)=="}" && numLeftPar != 0 ) --numLeftPar;
	  }
	if ((tagEnd!=-1) && (tagStart!=-1) )
	  {
	  s=s.mid(tagStart,tagEnd);
	  QTextCursor	cursor(doc);
	  cursor.setPosition(blockpos + offset+1);
	  newItem=StructItem(i,s,5,cursor);
	  found=true;
	  }
	};
}
if (!found)
{
//// bib files ////
tagStart=tagEnd=offset=0;
s=text;
tagStart=s.indexOf("\\bibliography{", tagEnd);
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.mid(tagStart+13,s.length());
	s=s.trimmed();
	tagStart=s.indexOf("}", tagEnd);
	if (tagStart!=-1)
		{
		s=s.mid(0,tagStart+1);
		if (s.startsWith("{")) s=s.remove(0,1);
		if (s.endsWith("}")) s=s.remove(s.length()-1,1);
		QTextCursor	cursor(doc);
		cursor.setPosition(blockpos + offset+1);
		newItem=StructItem(i,s,9,cursor);
		found=true;
		}
	};
}
if (!found)
{
//// bib files ////
tagStart=tagEnd=offset=0;
s=text;
tagStart=s.indexOf("\\addbibresource{", tagEnd);
offset=tagStart;
if ((tagStart!=-1) && ((tagComment==-1) || ((tagComment!=-1) && (tagStart<=tagComment))))
	{
	s=s.mid(tagStart+15,s.length());
	s=s.trimmed();
	tagStart=s.indexOf("}", tagEnd);
	if (tagStart!=-1)
		{
		s=s.mid(0,tagStart+1);
		if (s.startsWith("{")) s=s.remove(0,1);
		if (s.endsWith("}")) s=s.remove(s.length()-1,1);
		QTextCursor	cursor(doc);
		cursor.setPosition(blockpos + offset+1);
		newItem=StructItem(i,s,9,cursor);
		found=true;
		}
	};
}

j=0;
if (newItem.type>=0)
{
while (j < templist.count()) 
  {
   if (templist[j].cursor.position() > newItem.cursor.position()) break;
  ++j;
  }
templist.insert(j,newItem);
}
j=0;
if (newLabelItem.type>=0)
{
while (j < templist.count()) 
  {
   if (templist[j].cursor.position() > newLabelItem.cursor.position()) break;
  ++j;
  }
templist.insert(j,newLabelItem);
}
if (!(newItem==oldItem)) haschanged=true;
if (!(newLabelItem==oldLabelItem)) haschanged=true;
struct updateStruct newstruct;
newstruct.isdirty=haschanged;
newstruct.list=templist;
return newstruct;
}


LatexEditor::LatexEditor(QWidget *parent,QFont & efont, QList<QColor> edcolors, QList<QColor> hicolors,bool inlinespelling,QString ignoredWords,Hunspell *spellChecker,bool tabspaces,int tabwidth,const QKeySequence viewfocus, QString name,QStringList ulist) : QPlainTextEdit(parent),c(0),lastSaveRevision(0)
{
  
QScriptValue appContext = fScriptEngine.newQObject(this); 
fScriptEngine.globalObject().setProperty("TM", appContext);

fname=name;
vfocus=viewfocus;
setAcceptDrops(true);
/***********************/
inBlockSelectionMode=false;
blockSelection.tabSize=tabwidth;
blockSelection.clear();
setCursorWidth(2);
connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
/***********************/
colorBackground=edcolors.at(0);
colorLine=edcolors.at(1);
colorHighlight=edcolors.at(2);
colorCursor=edcolors.at(3);

QPalette p = palette();
p.setColor(QPalette::Active, QPalette::Base, colorBackground);
p.setColor(QPalette::Inactive, QPalette::Base, colorBackground);
p.setColor(QPalette::Inactive, QPalette::Window, colorBackground);
p.setColor(QPalette::Active, QPalette::Window, colorBackground);
p.setColor(QPalette::Normal,QPalette::Text,colorCursor);
p.setColor(QPalette::Inactive, QPalette::Highlight,p.color(QPalette::Active, QPalette::Highlight));
p.setColor(QPalette::Inactive, QPalette::HighlightedText,p.color(QPalette::Active, QPalette::HighlightedText));
setPalette(p);
setBackgroundVisible(true);

setFrameStyle(QFrame::NoFrame);
//setAcceptRichText(false);
setLineWidth(0);
setFrameShape(QFrame::NoFrame);
for (int i = 0; i < 3; ++i) UserBookmark[i]=0;
encoding="";
setFont(efont);
tabWidth=tabwidth;
tabSpaces=tabspaces;
setTabStopWidth(fontMetrics().width(" ")*tabWidth);
setTabChangesFocus(false);
endBlock=-1;
foldableLines.clear();
lastnumlines=0;
/*********************************/
inlinecheckSpelling=inlinespelling;
pChecker = spellChecker;
if (pChecker) spell_encoding=QString(pChecker->get_dic_encoding());
if (!ignoredWords.isEmpty()) alwaysignoredwordList=ignoredWords.split(",");
else alwaysignoredwordList.clear();
ignoredwordList=alwaysignoredwordList;
QFile wordsfile(":/spell/spellignore.txt");
QString line;
if (wordsfile.open(QFile::ReadOnly))
    {
    while (!wordsfile.atEnd()) 
	    {
	    line = wordsfile.readLine();
	    if (!line.isEmpty()) hardignoredwordList.append(line.trimmed());
	    }
    }
/********************************/

highlighter = new LatexHighlighter(document(),inlinespelling,ignoredWords,spellChecker);
highlighter->SetEditor(this);
highlighter->setColors(hicolors);
QFileInfo fi(fname);
if (!fname.startsWith("untitled") && (fi.suffix()=="asy" || fi.suffix()=="mp")) highlighter->setModeGraphic(true);


//c=0;
//connect(this, SIGNAL(cursorPositionChanged()), viewport(), SLOT(update()));
connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchAll()));
connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(requestNewNumLines(int)));

//connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchLat()));
//grabShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Tab), Qt::WidgetShortcut);
//setCenterOnScroll(true);
setFocusPolicy(Qt::WheelFocus);
setFocus();
lastSavedTime=QDateTime::currentDateTime();
highlightLine=false;
highlightRemover.setSingleShot(true);
connect(&highlightRemover, SIGNAL(timeout()), this, SLOT(clearHightLightLine()));
emit poshaschanged(1,1);
setUserTagsList(ulist);
overmode=false;
setOverwriteMode (overmode);
}
LatexEditor::~LatexEditor(){
//delete pChecker;
}

void LatexEditor::setColors(QList<QColor> colors)
{
colorBackground=colors.at(0);
colorLine=colors.at(1);
colorHighlight=colors.at(2);
colorCursor=colors.at(3);
QPalette p = palette();
p.setColor(QPalette::Active, QPalette::Base, colorBackground);
p.setColor(QPalette::Inactive, QPalette::Base, colorBackground);
p.setColor(QPalette::Inactive, QPalette::Window, colorBackground);
p.setColor(QPalette::Active, QPalette::Window, colorBackground);
p.setColor(QPalette::Normal,QPalette::Text,colorCursor);
p.setColor(QPalette::Inactive, QPalette::Highlight,p.color(QPalette::Active, QPalette::Highlight));
p.setColor(QPalette::Inactive, QPalette::HighlightedText,p.color(QPalette::Active, QPalette::HighlightedText));
setPalette(p);
setBackgroundVisible(true);
highlighter->rehighlight();
update();
}

void LatexEditor::paintEvent(QPaintEvent *event)
{
QPainter painter(viewport());
QColor selBlendColor=colorHighlight;
QColor blCol = selBlendColor.dark( 140 );
blCol.setAlphaF( 0.2 );
QRect rect = cursorRect();
rect.setX(0);
rect.setWidth(viewport()->width());
if (!highlightLine)
  {
  const QBrush brush(colorLine);
  painter.fillRect(rect, brush);
  //painter.end();
  }
else
  {
  const QBrush brush(blCol);
  painter.fillRect(rect, brush);
  //painter.end();
  }      
QTextDocument *doc = document();  
QPointF offset(contentOffset());
QTextBlock textCursorBlock = textCursor().block();
bool hasMainSelection = textCursor().hasSelection();
QRect er = event->rect();
QRect viewportRect = viewport()->rect();
painter.setBrushOrigin(offset);


int blockSelectionIndex = -1;

QAbstractTextDocumentLayout::PaintContext context = getPaintContext();
if (inBlockSelectionMode
    && context.selections.count() && context.selections.last().cursor == textCursor()) 
{
    blockSelectionIndex = context.selections.size()-1;
    context.selections[blockSelectionIndex].format.clearBackground();
}

QTextLayout *cursor_layout = 0;
QPointF cursor_offset;
int cursor_cpos = 0;
QPen cursor_pen;
QTextBlock block = firstVisibleBlock();
while (block.isValid()) {
QRectF r = blockBoundingRect(block).translated(offset);

if (r.bottom() >= er.top() && r.top() <= er.bottom()) {

    QTextLayout *layout = block.layout();

    QTextOption option = layout->textOption();

	option.setFlags(option.flags() & ~QTextOption::SuppressColors);
	painter.setPen(context.palette.text().color());
   
    layout->setTextOption(option);
    layout->setFont(doc->defaultFont()); 
int blpos = block.position();
int bllen = block.length();

QVector<QTextLayout::FormatRange> selections;
QVector<QTextLayout::FormatRange> prioritySelections;

for (int i = 0; i < context.selections.size(); ++i) {
    const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
    const int selStart = range.cursor.selectionStart() - blpos;
    const int selEnd = range.cursor.selectionEnd() - blpos;
    if (selStart < bllen && selEnd >= 0
	&& selEnd >= selStart) {
	QTextLayout::FormatRange o;
	o.start = selStart;
	o.length = selEnd - selStart;
	o.format = range.format;
	if (i == blockSelectionIndex) {
	    QString text = block.text();
	    o.start = blockSelection.positionAtColumn(text, blockSelection.firstVisualColumn,0);
	    o.length = blockSelection.positionAtColumn(text, blockSelection.lastVisualColumn,0) - o.start;
	}
	if ((hasMainSelection && i == context.selections.size()-1)
	    || (o.format.foreground().style() == Qt::NoBrush
	    && o.format.underlineStyle() != QTextCharFormat::NoUnderline
	    && o.format.background() == Qt::NoBrush))
	    prioritySelections.append(o);
	else
	    selections.append(o);
    }
}
 selections += prioritySelections;
 
QRectF blockSelectionCursorRect;
if (inBlockSelectionMode
	&& block.position() >= blockSelection.firstBlock.block().position()
	&& block.position() <= blockSelection.lastBlock.block().position()) {
    QString text = block.text();
    qreal spacew = QFontMetricsF(font()).width(QLatin1Char(' '));

    int offset = 0;
    int relativePos  =  blockSelection.positionAtColumn(text, blockSelection.firstVisualColumn, &offset);
    QTextLine line = layout->lineForTextPosition(relativePos);
    qreal x = line.cursorToX(relativePos) + offset * spacew;

    int eoffset = 0;
    int erelativePos  =  blockSelection.positionAtColumn(text, blockSelection.lastVisualColumn, &eoffset);
    QTextLine eline = layout->lineForTextPosition(erelativePos);
    qreal ex = eline.cursorToX(erelativePos) + eoffset * spacew;

    QRectF rr = line.naturalTextRect();
    rr.moveTop(rr.top() + r.top());
    rr.setLeft(r.left() + x);
    if (line.lineNumber() == eline.lineNumber())  {
	rr.setRight(r.left() + ex);
    }
    painter.fillRect(rr, palette().highlight());
    if ((blockSelection.anchor == TextBlockSelection::TopLeft
	    && block == blockSelection.firstBlock.block())
	    || (blockSelection.anchor == TextBlockSelection::BottomLeft
		&& block == blockSelection.lastBlock.block())
	    ) {
	rr.setRight(rr.left()+2);
	blockSelectionCursorRect = rr;
    }
    for (int i = line.lineNumber() + 1; i < eline.lineNumber(); ++i) {
	rr = layout->lineAt(i).naturalTextRect();
	rr.moveTop(rr.top() + r.top());
	rr.setLeft(r.left() + x);
	painter.fillRect(rr, palette().highlight());
    }

    rr = eline.naturalTextRect();
    rr.moveTop(rr.top() + r.top());
    rr.setRight(r.left() + ex);
    if (line.lineNumber() != eline.lineNumber())
	painter.fillRect(rr, palette().highlight());
    if ((blockSelection.anchor == TextBlockSelection::TopRight
	  && block == blockSelection.firstBlock.block())
	    || (blockSelection.anchor == TextBlockSelection::BottomRight
		&& block == blockSelection.lastBlock.block())) {
	rr.setLeft(rr.right()-2);
	blockSelectionCursorRect = rr;
    }
}
            bool drawCursor = ((true) // we want the cursor in read-only mode
                               && context.cursorPosition >= blpos
                               && context.cursorPosition < blpos + bllen);

            bool drawCursorAsBlock = drawCursor && overwriteMode() ;

            if (drawCursorAsBlock) {
                int relativePos = context.cursorPosition - blpos;
                bool doSelection = true;
                QTextLine line = layout->lineForTextPosition(relativePos);
                qreal x = line.cursorToX(relativePos);
                qreal w = 0;
                if (relativePos < line.textLength() - line.textStart()) {
                    w = line.cursorToX(relativePos + 1) - x;
                    if (doc->characterAt(context.cursorPosition) == QLatin1Char('\t')) {
                        doSelection = false;
                        qreal space = QFontMetricsF(layout->font()).width(QLatin1Char(' '));
                        if (w > space) {
                            x += w-space;
                            w = space;
                        }
                    }
                } else
                    w = QFontMetrics(layout->font()).width(QLatin1Char(' ')); // in sync with QTextLine::draw()

                QRectF rr = line.rect();
                rr.moveTop(rr.top() + r.top());
                rr.moveLeft(r.left() + x);
                rr.setWidth(w);
                painter.fillRect(rr, palette().text());
                if (doSelection) {
                    QTextLayout::FormatRange o;
                    o.start = relativePos;
                    o.length = 1;
                    o.format.setForeground(palette().base());
                    selections.append(o);
                }
            }
layout->draw(&painter, offset, selections, er); 
            if ((drawCursor && !drawCursorAsBlock)
                || (context.cursorPosition < -1
                    && !layout->preeditAreaText().isEmpty())) {
                int cpos = context.cursorPosition;
                if (cpos < -1)
                    cpos = layout->preeditAreaPosition() - (cpos + 2);
                else
                    cpos -= blpos;
                cursor_layout = layout;
                cursor_offset = offset;
                cursor_cpos = cpos;
                cursor_pen = painter.pen();
            }
#if !defined(Q_OS_MAC) // no visible cursor on mac
            if (blockSelectionCursorRect.isValid())
                painter.fillRect(blockSelectionCursorRect, palette().text());
#endif
}
offset.ry() += r.height();


if (offset.y() > viewportRect.height())
    break;

block = block.next();
       if (!block.isVisible()) {
            // invisible blocks do have zero line count
            block = doc->findBlockByLineNumber(block.firstLineNumber());
        }

}
    painter.setPen(context.palette.text().color());

    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().background());
    }
    

   if (cursor_layout && !inBlockSelectionMode) {
	cursor_pen.setBrush(QBrush(colorCursor));
        painter.setPen(cursor_pen);
        cursor_layout->drawCursor(&painter, cursor_offset, cursor_cpos, cursorWidth());
    }
}

void LatexEditor::contextMenuEvent(QContextMenuEvent *e)
{
QMenu *menu=new QMenu(this);
//menu = createStandardContextMenu();
QAction *a;
/*******************************************/
if (inlinecheckSpelling && pChecker && !textCursor().hasSelection())
      {
      QFont spellmenufont (qApp->font());
      spellmenufont.setBold(true);
      QTextCursor c = cursorForPosition(e->pos());
      int cpos=c.position();
      QTextBlock block=c.block();
      int bpos=block.position();
      BlockData *data = static_cast<BlockData *>(c.block().userData() );
      QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
      QByteArray encodedString;
      QString text,word;
      bool gonext=true;
      QByteArray t;
      int li,cols,cole,colstart,colend,check,ns;
      char ** wlst;
      QStringList suggWords;
      colstart=cpos-bpos;
      if (data->misspelled[colstart]==true)
      {
 	while ((colstart>=0) && (colstart<data->misspelled.count()) && (data->misspelled[colstart]==true))
	{
	colstart--;
	}
	colstart++;
	colend=cpos-bpos;
	while ((colend>=0) && (colend<data->misspelled.count()) && (data->misspelled[colend]==true))
	{
	colend++;
	}
	c.setPosition(bpos+colstart,QTextCursor::MoveAnchor);
	c.setPosition(bpos+colend,QTextCursor::KeepAnchor);
	text=c.selectedText();
 
	li=c.blockNumber();
	cols=colstart;
        if (text.length()>1 && colend>colstart)
		{
		word=text;
		if (!ignoredwordList.contains(word) && !hardignoredwordList.contains(word))
			{
			encodedString = codec->fromUnicode(word);
			check = pChecker->spell(encodedString.data());
			if (!check)
				{
				selectword(li,cols,word);
				gonext=false;
				ns = pChecker->suggest(&wlst,encodedString.data());
				if (ns > 0)
					{
					suggWords.clear();
                    spellingLookup.clear();
					for (int i=0; i < ns; i++) 
						{
						suggWords.append(codec->toUnicode(wlst[i]));
					//free(wlst[i]);
						} 
				//free(wlst);
					pChecker->free_list(&wlst, ns);
					if (!suggWords.isEmpty())
						{
						if (suggWords.contains(word)) gonext=true;
						else
							{
							foreach (const QString &suggestion, suggWords)
							    {
							    a = menu->addAction(suggestion, this, SLOT(correctWord()),0);
							    a->setFont(spellmenufont);
                                a->setText(suggestion);
                                spellingLookup.insert(a,suggestion);
							    }
							}
						}
					}
				}
			}
		}
	menu->addSeparator();
	}
      }
/*******************************************/
a = menu->addAction(tr("Undo"), this, SLOT(undoText()));
a->setShortcut(Qt::CTRL+Qt::Key_Z);
a->setEnabled(document()->isUndoAvailable());
a = menu->addAction(tr("Redo") , this, SLOT(redoText()));
a->setShortcut(Qt::CTRL+Qt::Key_Y);
a->setEnabled(document()->isRedoAvailable());
menu->addSeparator();
a = menu->addAction(tr("Cut"), this, SLOT(cut()));
a->setShortcut(Qt::CTRL+Qt::Key_X);
a->setEnabled(textCursor().hasSelection());
a = menu->addAction(tr("Copy"), this, SLOT(copy()));
a->setShortcut(Qt::CTRL+Qt::Key_C);
a->setEnabled(textCursor().hasSelection());
a = menu->addAction(tr("Paste") , this, SLOT(paste()));
a->setShortcut(Qt::CTRL+Qt::Key_P);
const QMimeData *md = QApplication::clipboard()->mimeData();
a->setEnabled(md && canInsertFromMimeData(md));
menu->addSeparator();
a = menu->addAction(tr("Select All"), this, SLOT(selectAll()));
a->setShortcut(Qt::CTRL+Qt::Key_A);
a->setEnabled(!document()->isEmpty());
menu->addSeparator();
if (!document()->isEmpty() && !inlinecheckSpelling)
{
a = menu->addAction(tr("Check Spelling Word"), this, SLOT(checkSpellingWord()));
//a->setEnabled(!document()->isEmpty() && !inlinecheckSpelling);
}
if (textCursor().hasSelection())
{
a = menu->addAction(tr("Check Spelling Selection"), this, SLOT(checkSpellingSelection()));
//a->setEnabled(textCursor().hasSelection());
}
a = menu->addAction(tr("Check Spelling Document"), this, SLOT(checkSpellingDocument()));
a->setEnabled(!document()->isEmpty() /*&& !textCursor().hasSelection()*/);
if (endBlock>=0)
  {
  if (linefromblock(textCursor().block())-1!=endBlock)
    {
    menu->addSeparator();
    a=new QAction(tr("Jump to the end of this block"),menu);
    connect(a, SIGNAL(triggered()), this, SLOT(jumpToEndBlock()));
    menu->addAction(a);
    }
  }
menu->addSeparator();
a=new QAction(tr("Jump to pdf"),menu);
a->setShortcut(vfocus);
// #ifdef Q_WS_MACX
// a->setShortcut(Qt::CTRL+Qt::Key_Dollar);
// #else
// a->setShortcut(Qt::CTRL+Qt::Key_Space);
// #endif
a->setData(QVariant(cursorForPosition(e->pos()).blockNumber() + 1));
connect(a, SIGNAL(triggered()), this, SLOT(jumpToPdf()));
menu->addAction(a);

// QInputContext *qic = inputContext();
// if (qic) 
//     {
//     QList<QAction *> imActions = qic->actions();
//     for (int i = 0; i < imActions.size(); ++i)
//     menu->addAction(imActions.at(i));
//     }

menu->exec(e->globalPos());
delete menu;
}

void LatexEditor::correctWord()
{
QAction *action = qobject_cast<QAction *>(sender());
if (action)
	{
	//QString newword = action->text();
    QString newword = spellingLookup.find(action).value();
	replace(newword,false,"");
	}
}

bool LatexEditor::search( const QString &expr, bool cs, bool wo, bool forward, bool startAtCursor, bool isRegExp)
{
QTextDocument::FindFlags flags = 0;
if (cs) flags |= QTextDocument::FindCaseSensitively;
if (wo) flags |= QTextDocument::FindWholeWords;
QTextCursor c = textCursor();

//if (!c.hasSelection()) 
//	{
//	if (forward) c.movePosition(QTextCursor::Start);
//	else c.movePosition(QTextCursor::End);
//	setTextCursor(c);
//	}
if (! startAtCursor) 
	{
	  c.setPosition(0);
	//c.movePosition(QTextCursor::Start);//Qt 4.7.1 bug
	setTextCursor(c);
	}
if (forward == false) flags |= QTextDocument::FindBackward;
QTextCursor found = isRegExp?document()->find(QRegExp(expr), c, flags) : document()->find(expr, c, flags);

if (found.isNull()) return false;
else 
	{
	setTextCursor(found);
	return true;
	}
}

int LatexEditor::searchInSelection( const QString &expr, bool cs, bool wo, bool isRegExp, int start, int end)
{
QTextDocument::FindFlags flags = 0;
if (cs) flags |= QTextDocument::FindCaseSensitively;
if (wo) flags |= QTextDocument::FindWholeWords;

QTextCursor found = isRegExp?document()->find(QRegExp(expr), start, flags) : document()->find(expr, start, flags);

if (found.isNull()) return -1;
else 
	{
	if (found.selectionEnd()>end) return -1;
	else
	  {
	  setTextCursor(found);
	  return found.selectionEnd();
	  }
	}
}

int LatexEditor::searchLine( const QString &expr)
{
int result=-1;
QTextCursor c = textCursor();
c.setPosition(0);
//c.movePosition(QTextCursor::Start); //Qt 4.7.1 bug
QTextCursor found = document()->find(expr, c, QTextDocument::FindCaseSensitively|QTextDocument::FindWholeWords);
if (found.isNull()) return result;
QTextBlock p = document()->findBlock(found.position());
if (p.blockNumber()>=0) result=p.blockNumber();
return result;
}

void LatexEditor::replace( const QString &r,bool isRegExp, const QString &o)
{
int start;
QTextCursor c = textCursor();
if (c.hasSelection()) 
	{
	if (isRegExp)
	  {
	  QRegExp expr1(o);
	  QStringList texts;
	  QString rtext1=r;
	  int pos = 0;
	  while ((pos = expr1.indexIn(c.selectedText(), pos)) != -1) 
	    {
	    texts << expr1.cap(1);
	    pos += expr1.matchedLength();
	    }
	  for(int i = 0; i < 10 && i < texts.count(); i++)
	    {
	    rtext1.replace("$"+QString::number(i+1), texts[i]);
	    }
	  start=c.selectionStart();
	  c.removeSelectedText();
	  c.insertText(rtext1);
	  c.setPosition(start,QTextCursor::MoveAnchor);
	  c.setPosition(start+rtext1.length(),QTextCursor::KeepAnchor);
  //	c.movePosition(QTextCursor::NextWord,QTextCursor::KeepAnchor);
	  setTextCursor(c);
	  }
	else
	  {
	  start=c.selectionStart();
	  c.removeSelectedText();
	  c.insertText(r);
	  c.setPosition(start,QTextCursor::MoveAnchor);
	  c.setPosition(start+r.length(),QTextCursor::KeepAnchor);
  //	c.movePosition(QTextCursor::NextWord,QTextCursor::KeepAnchor);
	  setTextCursor(c);
	  }
	}
}

void LatexEditor::gotoLine( int line )
{
if (line<=numoflines())  
  {
  setCursorPosition( line, 0 );
  if (textCursor().block().isVisible()) setHightLightLine();
  }
}

void LatexEditor::commentSelection()
{
bool go=true;
QTextCursor cur=textCursor();
if (cur.hasSelection())
	{
	int start=cur.selectionStart();
	int end=cur.selectionEnd();
	cur.setPosition(start,QTextCursor::MoveAnchor);
	cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
	while ( cur.position() < end && go)
		{
		cur.insertText("%");
		end++;
		go=cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor);
		}
      }
else 
    {
    cur.movePosition(QTextCursor::StartOfBlock);
    cur.insertText("%");
    }
}

void LatexEditor::indentSelection()
{
bool go=true;
QTextCursor cur=textCursor();
if (cur.hasSelection())
	{
	int start=cur.selectionStart();
	int end=cur.selectionEnd();
	cur.setPosition(start,QTextCursor::MoveAnchor);
	cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
	while ( cur.position() < end && go)
		{
		cur.insertText("\t");
		end++;
		go=cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor);
		}
	}
}

void LatexEditor::uncommentSelection()
{
bool go=true;
int pos=0;
QTextCursor cur=textCursor();
if (!cur.hasSelection())
	{
    cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    }
int start=cur.selectionStart();
int end=cur.selectionEnd();
cur.setPosition(start,QTextCursor::MoveAnchor);
cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
while ( cur.position() < end && go)
    {
    cur.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
    while (cur.selectedText()=="\t" || cur.selectedText()==" ")
    {
    pos=cur.selectionEnd();
    cur.setPosition(pos,QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
    }	
    if (cur.selectedText()=="%") 
        {
        cur.removeSelectedText();
        end--;
        }
    go=cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor);
    }
}

void LatexEditor::unindentSelection()
{
bool go=true;
QTextCursor cur=textCursor();
if (cur.hasSelection())
	{
	int start=cur.selectionStart();
	int end=cur.selectionEnd();
	cur.setPosition(start,QTextCursor::MoveAnchor);
	cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
	while ( cur.position() < end && go)
		{
		cur.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
		if (cur.selectedText()=="\t") 
			{
			cur.removeSelectedText();
			end--;
			}
		go=cur.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor);
		}
	}
}

void LatexEditor::changeFont(QFont & new_font)
{
setFont(new_font);
}

QString LatexEditor::getEncoding()
{
 return encoding;
}

void LatexEditor::setEncoding(QString enc)
{
 encoding=enc;
}  

int LatexEditor::getCursorPosition(int para, int index)
{
return document()->findBlockByNumber(para).position()+index;
}

void LatexEditor::setCursorPosition(int para, int index)
{
int pos=getCursorPosition(para,index);
QTextCursor cur=textCursor();
cur.setPosition(pos,QTextCursor::MoveAnchor);
if (cur.block().isVisible()) setTextCursor(cur);
ensureCursorVisible();
setFocus();
}

void LatexEditor::removeOptAlt()
{
QTextCursor cur=textCursor();
QTextBlock p = document()->begin();
QString s;
while (p.isValid())
	{
	s = p.text();
	s=s.left(3);
	if (s=="OPT" || s=="ALT")
		{
		int pos=p.position();
		p = p.next();
		cur.setPosition(pos,QTextCursor::MoveAnchor);
		cur.select(QTextCursor::BlockUnderCursor);
		cur.removeSelectedText();	
		}
	else 
		{
		p = p.next();
		}
	}
setFocus();
}

int LatexEditor::numoflines()
{
return document()->blockCount();
}

int LatexEditor::linefromblock(const QTextBlock& p)
{
return p.blockNumber()+1;
}

void LatexEditor::selectword(int line, int col, QString word)
{
QTextCursor cur=textCursor();
int i = 0;
QTextBlock p = document()->begin();
while ( p.isValid() ) 
	{
	if (line==i) break;
	i++;
	p = p.next();
	}
int pos=p.position();
int offset=word.length();
cur.setPosition(pos+col,QTextCursor::MoveAnchor);
cur.setPosition(pos+col+offset,QTextCursor::KeepAnchor);
setTextCursor(cur);
ensureCursorVisible();
}

void LatexEditor::checkSpellingWord()
{
QTextCursor c=textCursor();
int cpos=c.position();
QTextBlock block=c.block();
int bpos=block.position();
BlockData *data = static_cast<BlockData *>(c.block().userData() );
QString text;
int li,cols,colstart,colend;
colstart=cpos-bpos;
if (data->misspelled[colstart]==true)
  {
    while ((colstart>=0) && (colstart<data->misspelled.count()) && (data->misspelled[colstart]==true))
    {
    colstart--;
    }
    colstart++;
    colend=cpos-bpos;
    while ((colend>=0) && (colend<data->misspelled.count()) && (data->misspelled[colend]==true))
    {
    colend++;
    }
    c.setPosition(bpos+colstart,QTextCursor::MoveAnchor);
    c.setPosition(bpos+colend,QTextCursor::KeepAnchor);
    text=c.selectedText();
      li=c.blockNumber();
  cols=colstart;
  if (text.length()>1 && colend>colstart) 
    {
    selectword(li,cols,text);
    emit spellme();
    }
  }

}

void LatexEditor::checkSpellingDocument()
{
QTextCursor cur=textCursor();

cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
setTextCursor(cur);

emit spellme();
}

void LatexEditor::checkSpellingSelection()
{
QTextCursor cur=textCursor();
if (!cur.hasSelection())
{
cur.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
setTextCursor(cur);
}
emit spellme();
}

/*QString LatexEditor::textUnderCursor() const
 {
QTextCursor tc = textCursor();
int oldpos=tc.position();
tc.select(QTextCursor::WordUnderCursor);
int newpos = tc.selectionStart();
tc.setPosition(newpos, QTextCursor::MoveAnchor);
tc.setPosition(oldpos, QTextCursor::KeepAnchor);
QString word=tc.selectedText();
QString prevword="";
QString nextword="";
tc.setPosition(newpos, QTextCursor::MoveAnchor);
tc.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor);
//tc.setPosition(oldpos, QTextCursor::KeepAnchor);
QString sep=tc.selectedText();
if (sep=="{")
    {
    tc.movePosition(QTextCursor::PreviousWord,QTextCursor::MoveAnchor);
    tc.movePosition(QTextCursor::PreviousWord,QTextCursor::MoveAnchor);
    tc.setPosition(oldpos, QTextCursor::KeepAnchor);
    prevword=tc.selectedText();
    prevword=prevword.trimmed();
    }
if ((!prevword.isEmpty()) && (prevword.startsWith("\\"))) {word=prevword;}
//if ((nextword.contains("{")) || (nextword.contains("(")) || (nextword.contains("["))) word=""; 
//if (!nextword.isEmpty()) word=""; 
QString sword=word.trimmed();
if (word.right(1)!=sword.right(1)) word="";
return word;
 }*/

QString LatexEditor::commandUnderCursor() const
{
QString result="";
QTextCursor tc = textCursor();
const QTextBlock block = document()->findBlock(tc.position());

int index=tc.position()-block.position()-1;
if (index<0) return "";
const QString text = block.text();
if (text.length() < 1) return "";
if (index >= text.length()) return "";

// 	QRegExp completionStartRegExp("(\\\\([\\s\\{\\}\\[\\]\\w,.=\"'~:]|(\\&)|(\\$)|(\\%)(\\#)(\\_)|(\\{)|(\\})|(\\backslash)|(\\^)|(\\[)|(\\]))*)$");
// 	completionStartRegExp.setMinimal(true);
// 	QString leftSubstring = text.left(index);
// 	int start = completionStartRegExp.lastIndexIn(leftSubstring);
// 	int end=index;
int start=index;
int end=index;
QChar	ch = text.at(index);
#define IS_WORD_BACK(ch) (ch.isLetter() || ch.isNumber() || ch.isMark() || ch=='{' || ch=='[' || ch==':' || ch=='_')
#define IS_WORD_FORWARD(ch) (ch.isLetter() || ch.isMark() )

bool isControlSeq = false; // becomes true if we include an @ sign or a leading backslash
bool includesApos = false; // becomes true if we include an apostrophe
if (IS_WORD_BACK(ch) || ch == '@' /* || ch == '\'' || ch == 0x2019 */) 
{
if (ch == '@') isControlSeq = true;
while (start > 0) 
  {
  --start;
  ch = text.at(start);
  if (IS_WORD_BACK(ch))
	  continue;
  if (!includesApos && ch == '@') 
    {
    isControlSeq = true;
    continue;
    }
  if (!isControlSeq && (ch == '\'' || ch == 0x2019) && start > 0 && IS_WORD_BACK(text.at(start - 1))) 
    {
    includesApos = true;
    continue;
    }
  ++start;
  break;
  }
if (start > 0 && text.at(start - 1) == '\\') 
  {
  isControlSeq = true;
  --start;
  }
while (++end < text.length()) 
  {
  ch = text.at(end);
  if (IS_WORD_FORWARD(ch))
	  continue;
  if (!includesApos && ch == '@') 
    {
    isControlSeq = true;
    continue;
    }
  if (!isControlSeq && (ch == '\'' || ch == 0x2019) && end < text.length() - 1 && IS_WORD_FORWARD(text.at(end + 1))) 
  {
  includesApos = true;
  continue;
  }
  break;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return result;
}

if (index > 0 && text.at(index - 1) == '\\') 
  {
  start = index - 1;
  end = index + 1;
  return "";
  }

if (ch.isNumber()) 
  {
  while (start > 0) {
	  --start;
	  ch = text.at(start);
	  if (ch.isNumber())
		  continue;
	  ++start;
	  break;
  }
  while (++end < text.length()) {
	  ch = text.at(end);
	  if (ch.isNumber())
		  continue;
	  break;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return result;
  }

if (ch == ' ' || ch == '\t') 
  {
  while (start > 0) {
	  --start;
	  ch = text.at(start);
	  if (!(ch == ' ' || ch == '\t')) {
		  ++start;
		  break;
	  }
  }
  while (++end < text.length()) {
	  ch = text.at(end);
	  if (!(ch == ' ' || ch == '\t'))
		  break;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return result;
  }

if (ch == '\\') 
  {
  if (++end < text.length()) {
	  ch = text.at(end);
	  if (IS_WORD_FORWARD(ch) || ch == '@')
		  while (++end < text.length()) {
			  ch = text.at(end);
			  if (IS_WORD_FORWARD(ch) || ch == '@')
				  continue;
			  break;
		  }
	  else
		  ++end;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return result;
  }
end = index + 1;
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return result;
}

QStringList LatexEditor::fullcommandUnderCursor()
{
QString result="";
QTextCursor tc = textCursor();
const QTextBlock block = document()->findBlock(tc.position());
int index=tc.position()-block.position()-1;
int start=index;
int end=index;
if (index<0) return (QStringList() << "" << QString::number(index) << QString::number(start) << QString::number(end));
const QString text = block.text();
if (text.length() < 1) return (QStringList() << "" << QString::number(index) << QString::number(start) << QString::number(end));
if (index >= text.length()) return (QStringList() << "" << QString::number(index) << QString::number(start) << QString::number(end));
QChar	ch = text.at(index);
#define IS_WORD_FORMING(ch) (ch.isLetter() || ch.isNumber() || ch.isMark() || ch=='{' || ch=='[' || ch=='}' || ch==']' || ch==':' || ch=='_')
//#define IS_WORD_FORMING(ch) (ch.isLetter() || ch.isMark() || ch=='{' || ch=='[' || ch=='}' || ch==']')
bool isControlSeq = false; // becomes true if we include an @ sign or a leading backslash
bool includesApos = false; // becomes true if we include an apostrophe
bool first=true;
if (IS_WORD_FORMING(ch) || ch == '@' /* || ch == '\'' || ch == 0x2019 */) 
{
if (ch == '@') isControlSeq = true;
while (start > 0) 
  {
  --start;
  ch = text.at(start);

  if (IS_WORD_FORMING(ch))
	  continue;
  if (!includesApos && ch == '@') 
    {
    isControlSeq = true;
    continue;
    }
  if (!isControlSeq && (ch == '\'' || ch == 0x2019) && start > 0 && IS_WORD_FORMING(text.at(start - 1))) 
    {
    includesApos = true;
    continue;
    }
  ++start;
  break;
  }
if (start > 0 && text.at(start - 1) == '\\') 
  {
  isControlSeq = true;
  --start;
  }
while (++end < text.length()) 
  {
  ch = text.at(end);

  if (IS_WORD_FORMING(ch))
  {
    if ((first) && (ch=='}' || ch==']')) {++end;break;}
    first=false;
    continue;
	  
  }
  if (!includesApos && ch == '@') 
    {
    isControlSeq = true;
    continue;
    }
  if (!isControlSeq && (ch == '\'' || ch == 0x2019) && end < text.length() - 1 && IS_WORD_FORMING(text.at(end + 1))) 
  {
  includesApos = true;
  continue;
  }
  break;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return (QStringList() << result << QString::number(index) << QString::number(start) << QString::number(end));
}

if (index > 0 && text.at(index - 1) == '\\') 
  {
  start = index - 1;
  end = index + 1;
  return (QStringList() << "" << QString::number(index) << QString::number(start) << QString::number(end));;
  }

if (ch.isNumber()) 
  {
  while (start > 0) {
	  --start;
	  ch = text.at(start);
	  if (ch.isNumber())
		  continue;
	  ++start;
	  break;
  }
  while (++end < text.length()) {
	  ch = text.at(end);
	  if (ch.isNumber())
		  continue;
	  break;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return (QStringList() << result << QString::number(index) << QString::number(start) << QString::number(end));;
  }

if (ch == ' ' || ch == '\t') 
  {
  while (start > 0) {
	  --start;
	  ch = text.at(start);
	  if (!(ch == ' ' || ch == '\t')) {
		  ++start;
		  break;
	  }
  }
  while (++end < text.length()) {
	  ch = text.at(end);
	  if (!(ch == ' ' || ch == '\t'))
		  break;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return (QStringList() << result << QString::number(index) << QString::number(start) << QString::number(end));
  }

if (ch == '\\') 
  {
  if (++end < text.length()) {
	  ch = text.at(end);
	  if (IS_WORD_FORMING(ch) || ch == '@')
		  while (++end < text.length()) {
			  ch = text.at(end);
			  if (IS_WORD_FORMING(ch) || ch == '@')
				  continue;
			  break;
		  }
	  else
		  ++end;
  }
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return (QStringList() << result << QString::number(index) << QString::number(start) << QString::number(end));
  }
end = index + 1;
tc.setPosition(block.position() + start,QTextCursor::MoveAnchor);
tc.setPosition(block.position() + end, QTextCursor::KeepAnchor);
result=tc.selectedText();
return (QStringList() << result << QString::number(index) << QString::number(start) << QString::number(end));
}


void LatexEditor::keyPressEvent ( QKeyEvent * e ) 
{
if (e == QKeySequence::Undo) {undoText(); return;}
if (e == QKeySequence::Redo) {redoText(); return;}
   if (inBlockSelectionMode) {
        if (e == QKeySequence::Cut) {
            
                cut();
                e->accept();
                return;
      
        } else if (e == QKeySequence::Delete || e->key() == Qt::Key_Backspace) {
            
                removeBlockSelection();
                e->accept();
                return;
           
        } else if (e == QKeySequence::Paste) {
            
                removeBlockSelection();
                // continue
          
        }
    }
if (c && c->popup()->isVisible()) 
	{
	switch (e->key()) 
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
		e->ignore();
		return; 
		default:
		break;
		}
	}
const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
if (!ctrlOrShift && (e->key() == Qt::Key_Insert))
  {
  overmode=!overmode;
  setOverwriteMode(overmode);
  }
else if ( e->key()==Qt::Key_Tab) 
    {
    QTextCursor cursor=textCursor();
    QTextBlock block=cursor.block();
    if (block.isValid()) 
	{
	QString txt=block.text();
	if (txt.contains(QString(0x2022)))
	    {
	    //e->ignore();
	    search(QString(0x2022) ,true,false,true,true,false);
	    return;		
	    }
	}
    QTextBlock blocknext=block.next();
    if (blocknext.isValid()) 
	{
	QString txtnext=blocknext.text();
	if (txtnext.contains(QString(0x2022)))
	    {
	    //e->ignore();
	    search(QString(0x2022) ,true,false,true,true,false);
	    return;		
	    }
	}
    if (tabSpaces) insertPlainText(QString().fill(' ', tabWidth));
    else QPlainTextEdit::keyPressEvent(e);
    }
else if ( e->key()==Qt::Key_Backtab) 
    {
    QTextCursor cursor=textCursor();
    QTextBlock block=cursor.block();
    if (block.isValid()) 
	{
	QString txt=block.text();
	if (txt.contains(QString(0x2022)))
	    {
	    //e->ignore();
	    search(QString(0x2022) ,true,false,false,true,false);
	    return;		
	    }
	}
    cursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor);
    if (cursor.selectedText()=="\t") 
	    {
	    cursor.removeSelectedText();
	    }
    }
// if (((e->modifiers() & ~Qt::ShiftModifier) == Qt::ControlModifier) && e->key()==Qt::Key_Tab) 
//   {
//   e->ignore();
//   search(QString(0x2022) ,true,true,true,true);
//   return;
//   }
// if (((e->modifiers() & ~Qt::ShiftModifier) == Qt::ControlModifier) && e->key()==Qt::Key_Backtab) 
//   {
//   e->ignore();
//   search(QString(0x2022) ,true,true,false,true);
//   return;
//   }
// if ((e->key()==Qt::Key_Backtab))
// 	{
// 	QTextCursor cursor=textCursor();
// 	cursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor);
// 	if (cursor.selectedText()=="\t") 
// 			{
// 			cursor.removeSelectedText();
// 			}
// 	}
else if ((e->key()==Qt::Key_Enter)||(e->key()==Qt::Key_Return))
	{
	QPlainTextEdit::keyPressEvent(e);
	QTextCursor cursor=textCursor();
	cursor.joinPreviousEditBlock();
	QTextBlock block=cursor.block();
	QTextBlock blockprev=block.previous();
	if (blockprev.isValid()) 
		{
		QString txt=blockprev.text();
		int j=0;
		while ( (j<txt.count()) && ((txt[j]==' ') || txt[j]=='\t') ) 
			{
			cursor.insertText(QString(txt[j]));
			j++;
			}

		}
	cursor.endEditBlock();
	}
else
	{
	int qtKeyCode = e->key();
	if(e->modifiers() & Qt::ShiftModifier) {
		qtKeyCode += Qt::SHIFT;
	}
	if(e->modifiers() & Qt::ControlModifier) {
		qtKeyCode += Qt::CTRL;
	}
	if(e->modifiers() & Qt::AltModifier) {
		qtKeyCode += Qt::ALT;
	}
	if(e->modifiers() & Qt::MetaModifier) {
		qtKeyCode += Qt::META;
	}
	QKeySequence s1 = QKeySequence(qtKeyCode);
	if (s1.matches(vfocus)==QKeySequence::ExactMatch) emit requestpdf(textCursor().blockNumber() + 1);
	else if (e->key()==Qt::Key_Right)
	  {
	  bool found=false;
	  QTextCursor cursor=textCursor();
	  QTextBlock block=cursor.block();
	  if (block.isValid()) 
	      {
	      QString txt=block.text();
	      QStringList tagList;
	      QString trigger;
	      QString code;
	      for (int i = 0; i < userTagsList.count(); ++i)
		  {
		  tagList.clear();
		  tagList= userTagsList.at(i).split("#");
		  code=tagList.at(1);
		  if (tagList.count()==3)
		    {
		    trigger=":"+tagList.at(2);
		  int endw=cursor.position();
		  int startw=cursor.position()-1-tagList.at(2).length();
		  if (startw<0) startw=0;
		  cursor.setPosition(startw,QTextCursor::MoveAnchor);
		  cursor.setPosition(endw, QTextCursor::KeepAnchor);
		  //qDebug() << trigger << startw << endw << cursor.selectedText();
		    if ((trigger!=":") && (trigger==cursor.selectedText()))
		      {
			QRegExp rx("(@+)");
			int index=0;
			while ((index = rx.indexIn(code,index)) != -1) 
			{
			if (rx.cap(1)=="@") code.replace(rx.pos(1),1,QString(0x2022));
			else if (rx.cap(1)=="@@") code.replace(rx.pos(1),2,"@");
			index += rx.matchedLength();
			}
		      //code.replace("@",QString(0x2022));
		      if (code.left(1)=="%")
			{
			code=code.remove(0,1);
			code="\\begin{"+code+"}\n"+QString(0x2022)+"\n\\end{"+code+"}\n";
			}
		      search(trigger, true, false,false,true,false);
		      int pos=cursor.position();
		      cursor.removeSelectedText();
		      insertWithMemoryIndent(code);
		      //replace(code,false,"");
		      //int pos=cursor.position();
		      if (code.contains(QString(0x2022))) 
			{
			cursor.setPosition(pos,QTextCursor::MoveAnchor);
			setTextCursor(cursor);
			//search(QString(0x2022) ,true,false,false,true,false);
			search(QString(0x2022) ,true,false,true,true,false);
			}
		      else 
			{
			cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,0);
			setTextCursor(cursor);
			}
		      found=true;
		      }
		    }
		  }
	      
	      }
	  if (!found) QPlainTextEdit::keyPressEvent(e);
	  }
	else if (s1.matches(QKeySequence(Qt::CTRL + Qt::Key_S))!=QKeySequence::ExactMatch && s1.matches(QKeySequence(Qt::CTRL + Qt::Key_Y))!=QKeySequence::ExactMatch) QPlainTextEdit::keyPressEvent(e);
	}
if (c && !c->popup()->isVisible()) 
	{
	switch (e->key()) 
		{
		case Qt::Key_Down:
		case Qt::Key_Right:
		case Qt::Key_Left:
		case Qt::Key_Up:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		return; 
		default:
		break;
		}
	}

if (!c || (ctrlOrShift && e->text().isEmpty())) {return;}

bool hasModifier = (e->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ));
QString completionPrefix = commandUnderCursor();

if ( completionPrefix.length() < 3)
	{
	c->popup()->hide();
	return;
	}
if (!e->text().isEmpty())
	{
	QChar firstchar=e->text().at(0);
	if ( isWordSeparator(firstchar) || isSpace(firstchar))
		{
		c->popup()->hide();
		return;
		}
	}
if ((c->completionModel()->rowCount()==1) && (c->completionModel()->data(c->completionModel()->index(0,0))==completionPrefix))
	  {
	  c->popup()->hide();
	  return;
	  }
if (completionPrefix != c->completionPrefix()) 
	{
	c->setCompletionPrefix(completionPrefix);
	if ((c->completionModel()->rowCount()==1) && (c->completionModel()->data(c->completionModel()->index(0,0))==completionPrefix))
		  {
		  c->popup()->hide();
		  return;
		  }
	if (completionPrefix.startsWith("\\beg")) c->popup()->setCurrentIndex(c->completionModel()->index(c->completionModel()->rowCount()-1,0));
	else c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
	}
int pos;
QString item="";
QVariant aa,bb;
int i=0;
while (i<c->completionModel()->rowCount())
{
pos=c->completionModel()->data(c->completionModel()->index(i,0)).toString().indexOf("*");
if (pos >-1) 
  {
  item=c->completionModel()->data(c->completionModel()->index(i,0)).toString().left(pos);
  i++;
  if (i<c->completionModel()->rowCount())
    {
    if (c->completionModel()->data(c->completionModel()->index(i,0)).toString().startsWith(item))
      {
      aa=c->completionModel()->data(c->completionModel()->index(i-1,0));
      bb=c->completionModel()->data(c->completionModel()->index(i,0));
      c->completionModel()->setData(c->completionModel()->index(i-1, 0),bb);
      c->completionModel()->setData(c->completionModel()->index(i, 0),aa);
      }
    }
  }
i++;
}
QRect cr = cursorRect();
cr.setWidth(c->popup()->sizeHintForColumn(0)+ c->popup()->verticalScrollBar()->sizeHint().width());
c->complete(cr); 

}

QCompleter *LatexEditor::completer() const
 {
     return c;
 }

void LatexEditor::setCompleter(QCompleter *completer)
{
if (c) QObject::disconnect(c, 0, this, 0);
c = completer;
if (!c) return;
c->setWidget(this);
c->setCompletionMode(QCompleter::PopupCompletion);
c->setCaseSensitivity(Qt::CaseSensitive);
QObject::connect(c, SIGNAL(activated(const QString&)),this, SLOT(insertCompletion(const QString&)));
}

 void LatexEditor::insertCompletion(const QString& completion)
{
if (c->widget() != this) return;
QTextCursor tc = textCursor();
//tc.select(QTextCursor::WordUnderCursor);
//qDebug() << tc.selectedText();
QStringList check=fullcommandUnderCursor();
int t=check.at(1).toInt();
int r=check.at(2).toInt();
int w=check.at(0).size();
//qDebug() << check << w;

QString insert_word = completion;

tc.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,t-r+1);
QString bef=tc.selectedText();
tc.removeSelectedText();
int pos=tc.position();
tc.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,w-t-1+r);
QString aft=tc.selectedText();
//qDebug() << bef << aft;
//tc.removeSelectedText();
if (bef.contains("{") || (!aft.contains("}") && !aft.contains("]"))) tc.removeSelectedText();
else tc.setPosition(pos,QTextCursor::MoveAnchor);

QString env;
QRegExp rbb("begin\\{\\s*([A-Za-z_]+(\\*)?)\\}");
//QRegExp rbb("begin\\{\\s*(.*)\\}");
if (completion.contains(rbb)) 
	{
	tc.insertText(insert_word);
	insertNewLine();
	insertNewLine();
	tc.select(QTextCursor::WordUnderCursor);
	env=rbb.cap(1);
	tc.insertText("\\end{"+env+"}");
	tc.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor,1);
	setTextCursor(tc);
	if (insert_word.contains(QString(0x2022)))
	    {
	    tc.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor,1);
	    setTextCursor(tc);
	    search(QString(0x2022) ,true,false,true,true,false);
	    emit tooltiptab();
	    }

	}
else
	{
	tc.insertText(insert_word);
	tc.setPosition(pos,QTextCursor::MoveAnchor);
	setTextCursor(tc);
	if (!insert_word.contains(QString(0x2022)))
	  {
	  tc.setPosition(pos+completion.length(),QTextCursor::MoveAnchor);
	  setTextCursor(tc);
	  }
	else if (!search(QString(0x2022) ,true,false,true,true,false))
	  {
	  tc.setPosition(pos+completion.length(),QTextCursor::MoveAnchor);
	  setTextCursor(tc);
	  }
	 else emit tooltiptab();
	} 

}

void LatexEditor::insertNewLine()
{
QKeyEvent e(QEvent::KeyPress,Qt::Key_Enter,Qt::NoModifier);
QPlainTextEdit::keyPressEvent(&e);
QTextCursor cursor=textCursor();
cursor.joinPreviousEditBlock();
QTextBlock block=cursor.block();
QTextBlock blockprev=block.previous();
if (blockprev.isValid()) 
	{
	QString txt=blockprev.text();
	int j=0;
	while ( (j<txt.count()) && ((txt[j]==' ') || txt[j]=='\t') ) 
		{
		cursor.insertText(QString(txt[j]));
		j++;
		}
	}
cursor.endEditBlock();  
}

 void LatexEditor::focusInEvent(QFocusEvent *e)
{
if (c) c->setWidget(this);
QPlainTextEdit::focusInEvent(e);
}

 void LatexEditor::setSpellChecker(Hunspell * checker)
{
pChecker = checker;
if (pChecker) spell_encoding=QString(pChecker->get_dic_encoding());
}

void LatexEditor::activateInlineSpell(bool enable)
{
inlinecheckSpelling=enable;
}

void LatexEditor::jumpToPdf()
{
QAction *act = qobject_cast<QAction*>(sender());
if (act != NULL) 
  {
  int i=act->data().toInt();
  emit requestpdf(i);
  }
}

bool LatexEditor::isWordSeparator(QChar c) const
{
    switch (c.toLatin1()) {
//    case '.':
//    case ',':
    case '?':
    case '!':
//    case ':':
//    case ';':
//    case '-':
    case '<':
    case '>':
//    case '[':
//    case ']':
//    case '(':
//    case ')':
//    case '{':
//    case '}':
    case '=':
    case '/':
    case '+':
    case '%':
    case '&':
    case '^':
    case '*':
    case '\'':
    case '"':
    case '~':
        return true;
    default:
        return false;
    }
}

bool LatexEditor::isSpace(QChar c) const
{
    return c == QLatin1Char(' ')
        || c == QChar::Nbsp
        || c == QChar::LineSeparator
        || c == QLatin1Char('\t')
        ;
}

void LatexEditor::matchAll() 
{
viewport()->update();
QList<QTextEdit::ExtraSelection> selections;
setExtraSelections(selections);
matchPar();
matchLat();

if (foldableLines.keys().contains(textCursor().block().blockNumber())) emit requestGotoStructure(textCursor().block().blockNumber());

//emit poshaschanged(textCursor().blockNumber() + 1,textCursor().position() - document()->findBlock(textCursor().selectionStart()).position()+1);
emit poshaschanged(textCursor().blockNumber() + 1,textCursor().position() - textCursor().block().position()+1);
}

void LatexEditor::matchPar() 
{

//QList<QTextEdit::ExtraSelection> selections;
//setExtraSelections(selections);
QTextBlock textBlock = textCursor().block();
BlockData *data = static_cast<BlockData *>( textBlock.userData() );
if ( data ) {
	QVector<ParenthesisInfo *> infos = data->parentheses();
	int pos = textCursor().block().position();

	for ( int i=0; i<infos.size(); ++i ) {
		ParenthesisInfo *info = infos.at(i);
		int curPos = textCursor().position() - textBlock.position();
		// Clicked on a left parenthesis?
		if ( info->position == curPos-1 && info->character == '{' ) {
			if ( matchLeftPar( textBlock, i+1, 0 ) )
				createParSelection( pos + info->position );
		}

		// Clicked on a right parenthesis?
		if ( info->position == curPos-1 && info->character == '}' ) {
			if ( matchRightPar( textBlock, i-1, 0 ) )
				createParSelection( pos + info->position );
		}
	}
}
}

bool LatexEditor::matchLeftPar(	QTextBlock currentBlock, int index, int numLeftPar ) 
{

BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
QVector<ParenthesisInfo *> infos = data->parentheses();
int docPos = currentBlock.position();

// Match in same line?
for ( ; index<infos.size(); ++index ) {
	ParenthesisInfo *info = infos.at(index);

	if ( info->character == '{' ) {
		++numLeftPar;
		continue;
	}

	if ( info->character == '}' && numLeftPar == 0 ) {
		createParSelection( docPos + info->position );
		return true;
	} else
		--numLeftPar;
}

// No match yet? Then try next block
currentBlock = currentBlock.next();
if ( currentBlock.isValid() )
	return matchLeftPar( currentBlock, 0, numLeftPar );

// No match at all
return false;
}

bool LatexEditor::matchRightPar(QTextBlock currentBlock, int index, int numRightPar) 
{

BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
QVector<ParenthesisInfo *> infos = data->parentheses();
int docPos = currentBlock.position();

// Match in same line?
for (int j=index; j>=0; --j ) {
	ParenthesisInfo *info = infos.at(j);

	if ( info->character == '}' ) {
		++numRightPar;
		continue;
	}

	if ( info->character == '{' && numRightPar == 0 ) {
		createParSelection( docPos + info->position );
		return true;
	} else
		--numRightPar;
}

// No match yet? Then try previous block
currentBlock = currentBlock.previous();
if ( currentBlock.isValid() ) {

	// Recalculate correct index first
	BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
	QVector<ParenthesisInfo *> infos = data->parentheses();

	return matchRightPar( currentBlock, infos.size()-1, numRightPar );
}

// No match at all
return false;
}

void LatexEditor::createParSelection( int pos ) 
{
QList<QTextEdit::ExtraSelection> selections = extraSelections();
QTextEdit::ExtraSelection selection;
QTextCharFormat format = selection.format;
format.setBackground( QColor("#FFFF99") );
format.setForeground( QColor("#FF0000") );
selection.format = format;

QTextCursor cursor = textCursor();
cursor.setPosition( pos );
cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
selection.cursor = cursor;
selections.append( selection );
setExtraSelections( selections );
}

/********************************************************/
void LatexEditor::matchLat() 
{
//QList<QTextEdit::ExtraSelection> selections;
//setExtraSelections(selections);

QTextBlock textBlock = textCursor().block();
if (foldableLines.keys().contains(textBlock.blockNumber())) createLatSelection(textBlock.blockNumber(),foldableLines[textBlock.blockNumber()]);
else
  {
  BlockData *data = static_cast<BlockData *>( textBlock.userData() );
  if ( data ) 
    {
    QVector<LatexBlockInfo *> infos = data->latexblocks();
    int pos = textCursor().block().position();
    if (infos.size()==0) 
      {
      emit setBlockRange(-1,-1);
      endBlock=-1;
      }
    for ( int i=0; i<infos.size(); ++i ) 
      {
      LatexBlockInfo *info = infos.at(i);
      int curPos = textCursor().position() - textBlock.position();
      if ( info->position <= curPos && info->character == 'b' ) matchLeftLat( textBlock, i+1, 0, textBlock.blockNumber());
      if ( info->position <= curPos && info->character == 'e' ) matchRightLat( textBlock, i-1, 0,textBlock.blockNumber());
      }
    }
  }

}

bool LatexEditor::matchLeftLat(	QTextBlock currentBlock, int index, int numLeftLat, int bpos ) 
{

BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
QVector<LatexBlockInfo *> infos = data->latexblocks();
int docPos = currentBlock.position();

// Match in same line?
for ( ; index<infos.size(); ++index ) {
	LatexBlockInfo *info = infos.at(index);

	if ( info->character == 'b' ) {
		++numLeftLat;
		continue;
	}

	if ( info->character == 'e' && numLeftLat == 0 ) {
		createLatSelection( bpos,currentBlock.blockNumber() );
		return true;
	} else
		--numLeftLat;
}

// No match yet? Then try next block
currentBlock = currentBlock.next();
if ( currentBlock.isValid() )
	return matchLeftLat( currentBlock, 0, numLeftLat, bpos );

// No match at all
return false;
}

bool LatexEditor::matchRightLat(QTextBlock currentBlock, int index, int numRightLat, int epos) 
{

BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
QVector<LatexBlockInfo *> infos = data->latexblocks();
int docPos = currentBlock.position();

// Match in same line?
for (int j=index; j>=0; --j ) {
	LatexBlockInfo *info = infos.at(j);

	if ( info->character == 'e' ) {
		++numRightLat;
		continue;
	}

	if ( info->character == 'b' && numRightLat == 0 ) {
		createLatSelection( epos, currentBlock.blockNumber() );
		return true;
	} else
		--numRightLat;
}

// No match yet? Then try previous block
currentBlock = currentBlock.previous();
if ( currentBlock.isValid() ) {

	// Recalculate correct index first
	BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
	QVector<LatexBlockInfo *> infos = data->latexblocks();

	return matchRightLat( currentBlock, infos.size()-1, numRightLat, epos );
}

// No match at all
return false;
}

void LatexEditor::createLatSelection( int start, int end ) 
{
int s=qMin(start,end);
int e=qMax(start,end);
emit setBlockRange(s,e);
endBlock=e;
}

void LatexEditor::jumpToEndBlock()
{
gotoLine(endBlock);
QTextCursor c = textCursor();
c.movePosition(QTextCursor::EndOfBlock);
setTextCursor(c);
}

void LatexEditor::fold(int start, int end)
{
foldedLines.insert(start, end);
ensureFinalNewLine();//Qt 4.7.1 bug
for (int i = start + 1; i <= end; i++) {document()->findBlockByNumber(i).setVisible(false);}
update();
resizeEvent(new QResizeEvent(QSize(0, 0), size()));
viewport()->update();
emit updatelineWidget();
ensureCursorVisible();
}
 
void LatexEditor::unfold(int start, int end)
{
if (!foldedLines.keys().contains(start)) return;
foldedLines.remove(start);
int i=start+1;
while (i<=end)
{
if (foldedLines.keys().contains(i)) 
  {
  document()->findBlockByNumber(i).setVisible(true);
  i=foldedLines[i]; 
  }
else document()->findBlockByNumber(i).setVisible(true);
i++;
}
update();
resizeEvent(new QResizeEvent(QSize(0, 0), size()));
viewport()->update();
emit updatelineWidget();
ensureCursorVisible();
}

void LatexEditor::checkStructUpdate(QTextDocument *doc,int blockpos,QString text,int line)
{
QList<StructItem> temp=StructItemsList;
QBrush brushcomment(highlighter->ColorComment);
QFuture< struct updateStruct > result=QtConcurrent::run(updateStructureList,doc,blockpos,text,line,temp);
struct updateStruct newstruct=result;
StructItemsList=newstruct.list;
if (newstruct.isdirty) {emit requestUpdateStructure();}
}



void LatexEditor::ensureFinalNewLine()//Qt 4.7.1 bug
{
disconnect(this, SIGNAL(blockCountChanged(int)), this, SLOT(requestNewNumLines(int)));
QTextCursor cursor = textCursor();
cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
bool emptyFile = !cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
if (!emptyFile && cursor.selectedText().at(0) != QChar::ParagraphSeparator)
    {
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    cursor.insertText(QLatin1String("\n"));
    }
setLastNumLines(document()->blockCount());
connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(requestNewNumLines(int)));
}

int LatexEditor::getLastNumLines()
{
return lastnumlines;
}

void LatexEditor::setLastNumLines(int n)
{
lastnumlines=n;  
}

void LatexEditor::setHightLightLine()
{
highlightRemover.stop();
highlightLine=true;
update();
highlightRemover.start(1000);
}

void LatexEditor::clearHightLightLine()
{
highlightLine=false;
emit cursorPositionChanged();
update();
}

QDateTime LatexEditor::getLastSavedTime()
{
return lastSavedTime;  
}

void LatexEditor::setLastSavedTime(QDateTime t)
{
lastSavedTime=t;  
}

QString LatexEditor::beginningLine()
{
QString result="";
QTextCursor cursor=textCursor();
QTextBlock block=cursor.block();
if (block.isValid()) 
	{
	QString txt=block.text();
	int j=0;
	while ( (j<txt.count()) && ((txt[j]==' ') || txt[j]=='\t') ) 
		{
		result+=QString(txt[j]);
		j++;
		}

	}
return result;
}

void LatexEditor::setTabSettings(bool tabspaces,int tabwidth)
{
tabWidth=tabwidth;
tabSpaces=tabspaces;
setTabStopWidth(fontMetrics().width(" ")*tabWidth);
}

void LatexEditor::requestNewNumLines(int n)
{
emit numLinesChanged(n);
}

void LatexEditor::setKeyViewerFocus(QKeySequence s)
{
vfocus=s;
}

void LatexEditor::updateName(QString f)
{
QFileInfo fiold(fname);
QFileInfo finew(f);
if ((fname.startsWith("untitled") || (fiold.suffix()!="asy" && fiold.suffix()!="mp")) && (finew.suffix()=="asy" || finew.suffix()=="mp") )
  {
  highlighter->setModeGraphic(true);
  highlighter->rehighlight();
  }
fname=f;
}

void LatexEditor::mouseMoveEvent(QMouseEvent *e)
{
QPlainTextEdit::mouseMoveEvent(e);

if (e->modifiers() & Qt::AltModifier) 
{
    if (!inBlockSelectionMode) {
	blockSelection.fromSelection(textCursor());
	inBlockSelectionMode = true;
    } else {

	QTextCursor cursor = textCursor();
	// get visual column
	int column = blockSelection.columnAt(cursor.block().text(), cursor.positionInBlock());
	if (cursor.positionInBlock() == cursor.block().length()-1) {
	    column += (e->pos().x() - cursorRect().center().x())/QFontMetricsF(font()).width(QLatin1Char(' '));
	}
	  blockSelection.moveAnchor(cursor.blockNumber(), column);
	setTextCursor(blockSelection.selection());
	viewport()->update();
    }
}
    if (viewport()->cursor().shape() == Qt::BlankCursor)
        viewport()->setCursor(Qt::IBeamCursor);
}

void LatexEditor::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
    if (inBlockSelectionMode) {
        inBlockSelectionMode = false;
        blockSelection.clear();
        QTextCursor cursor = textCursor();
        cursor.clearSelection();
        setTextCursor(cursor);
    }       
    }

    QPlainTextEdit::mousePressEvent(e);
}

void LatexEditor::cut()
{
    if (inBlockSelectionMode) {
        copy();
        removeBlockSelection();
        return;
    }
    QPlainTextEdit::cut();
}

void LatexEditor::paste()
{
    if (inBlockSelectionMode) {
        removeBlockSelection();
    }
    QPlainTextEdit::paste();
}

void LatexEditor::removeBlockSelection(const QString &text)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection() || !inBlockSelectionMode)
        return;

    int cursorPosition = cursor.selectionStart();
    cursor.clearSelection();
    cursor.beginEditBlock();

   
    QTextBlock block = blockSelection.firstBlock.block();
    QTextBlock lastBlock = blockSelection.lastBlock.block();
    for (;;) {
        QString text = block.text();
        int startOffset = 0;
        int startPos = blockSelection.positionAtColumn(text, blockSelection.firstVisualColumn, &startOffset);
        int endOffset = 0;
        int endPos = blockSelection.positionAtColumn(text, blockSelection.lastVisualColumn, &endOffset);

        cursor.setPosition(block.position() + startPos);
        cursor.setPosition(block.position() + endPos, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();

        if (startOffset < 0)
            cursor.insertText(QString(blockSelection.tabSize + startOffset, QLatin1Char(' ')));
        if (endOffset < 0)
            cursor.insertText(QString(-endOffset, QLatin1Char(' ')));

        if (block == lastBlock)
            break;
        block = block.next();
    }

    cursor.setPosition(cursorPosition);
    if (!text.isEmpty())
        cursor.insertText(text);
    cursor.endEditBlock();
    setTextCursor(cursor);
}

QMimeData *LatexEditor::createMimeDataFromSelection() const
{
    if (inBlockSelectionMode) {
        QMimeData *mimeData = new QMimeData;
        QString text = copyBlockSelection();
        mimeData->setData(QLatin1String("application/vnd.texmaker.vblocktext"), text.toUtf8());
        mimeData->setText(text); // for exchangeability
        return mimeData;
    } else if (textCursor().hasSelection()) {
        QTextCursor cursor = textCursor();
        QMimeData *mimeData = new QMimeData;

        // Copy the selected text as plain text
        QString text = cursor.selectedText();
        convertToPlainText(text);
        mimeData->setText(text);

        // Copy the selected text as HTML
        {
            // Create a new document from the selected text document fragment
            QTextDocument *tempDocument = new QTextDocument;
            QTextCursor tempCursor(tempDocument);
            tempCursor.insertFragment(cursor.selection());

            // Apply the additional formats set by the syntax highlighter
            QTextBlock start = document()->findBlock(cursor.selectionStart());
            QTextBlock end = document()->findBlock(cursor.selectionEnd());
            end = end.next();

            const int selectionStart = cursor.selectionStart();
            const int endOfDocument = tempDocument->characterCount() - 1;
            for (QTextBlock current = start; current.isValid() && current != end; current = current.next()) {
                const QTextLayout *layout = current.layout();
                foreach (const QTextLayout::FormatRange &range, layout->additionalFormats()) {
                    const int start = current.position() + range.start - selectionStart;
                    const int end = start + range.length;
                    if (end <= 0 || start >= endOfDocument)
                        continue;
                    tempCursor.setPosition(qMax(start, 0));
                    tempCursor.setPosition(qMin(end, endOfDocument), QTextCursor::KeepAnchor);
                    tempCursor.setCharFormat(range.format);
                }
            }

            // Reset the user states since they are not interesting
            for (QTextBlock block = tempDocument->begin(); block.isValid(); block = block.next())
                block.setUserState(-1);

            // Make sure the text appears pre-formatted
            tempCursor.setPosition(0);
            tempCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            QTextBlockFormat blockFormat = tempCursor.blockFormat();
            blockFormat.setNonBreakableLines(true);
            tempCursor.setBlockFormat(blockFormat);

            mimeData->setHtml(tempCursor.selection().toHtml());
            delete tempDocument;
        }

        /*
          Try to figure out whether we are copying an entire block, and store the complete block
          including indentation in the qtcreator.blocktext mimetype.
        */
        QTextCursor selstart = cursor;
        selstart.setPosition(cursor.selectionStart());
        QTextCursor selend = cursor;
        selend.setPosition(cursor.selectionEnd());

        bool startOk = blockSelection.cursorIsAtBeginningOfLine(selstart);
        bool multipleBlocks = (selend.block() != selstart.block());

        if (startOk && multipleBlocks) {
            selstart.movePosition(QTextCursor::StartOfBlock);
            if (blockSelection.cursorIsAtBeginningOfLine(selend))
                selend.movePosition(QTextCursor::StartOfBlock);
            cursor.setPosition(selstart.position());
            cursor.setPosition(selend.position(), QTextCursor::KeepAnchor);
            text = cursor.selectedText();
            mimeData->setData(QLatin1String("application/vnd.texmaker.blocktext"), text.toUtf8());
        }
        return mimeData;
    }
    return 0;
}

bool LatexEditor::canInsertFromMimeData(const QMimeData *source) const
{
    return QPlainTextEdit::canInsertFromMimeData(source);
}

void LatexEditor::insertFromMimeData(const QMimeData *source)
{
    if (isReadOnly())
        return;

    if (source->hasFormat(QLatin1String("application/vnd.texmaker.vblocktext"))) {
        QString text = QString::fromUtf8(source->data(QLatin1String("application/vnd.texmaker.vblocktext")));
        if (text.isEmpty())
            return;


        QStringList lines = text.split(QLatin1Char('\n'));
        QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        int initialCursorPosition = cursor.position();

        int column = blockSelection.columnAt(cursor.block().text(), cursor.positionInBlock());

        cursor.insertText(lines.first());
        for (int i = 1; i < lines.count(); ++i) {
            QTextBlock next = cursor.block().next();
            if (next.isValid()) {
                cursor.setPosition(next.position());
            } else {
                cursor.movePosition(QTextCursor::EndOfBlock);
                cursor.insertBlock();
            }
            int offset = 0;
            int position = blockSelection.positionAtColumn(cursor.block().text(), column, &offset);
            cursor.setPosition(cursor.block().position() + position);
            if (offset < 0) {
                cursor.deleteChar();
                cursor.insertText(QString(-offset, QLatin1Char(' ')));
            } else {
                cursor.insertText(QString(offset, QLatin1Char(' ')));
            }
            cursor.insertText(lines.at(i));
        }
        cursor.setPosition(initialCursorPosition);
        cursor.endEditBlock();
        setTextCursor(cursor);
        ensureCursorVisible();


        return;
    }

    QString text = source->text();
    if (text.isEmpty())
        return;

    QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        cursor.insertText(text);
        cursor.endEditBlock();
        setTextCursor(cursor);
}

QString LatexEditor::copyBlockSelection() const
{
    QString selection;
    QTextCursor cursor = textCursor();
    if (!inBlockSelectionMode)
        return selection;
    QTextBlock block = blockSelection.firstBlock.block();
    QTextBlock lastBlock = blockSelection.lastBlock.block();
    for (;;) {
        QString text = block.text();
        int startOffset = 0;
        int startPos = blockSelection.positionAtColumn(text, blockSelection.firstVisualColumn, &startOffset);
        int endOffset = 0;
        int endPos = blockSelection.positionAtColumn(text, blockSelection.lastVisualColumn, &endOffset);

        if (startPos == endPos) {
            selection += QString(endOffset - startOffset, QLatin1Char(' '));
        } else {
            if (startOffset < 0)
                selection += QString(-startOffset, QLatin1Char(' '));
            if (endOffset < 0)
                --endPos;
            selection += text.mid(startPos, endPos - startPos);
            if (endOffset < 0) {
                selection += QString(blockSelection.tabSize + endOffset, QLatin1Char(' '));
            } else if (endOffset > 0) {
                selection += QString(endOffset, QLatin1Char(' '));
            }
        }
        if (block == lastBlock)
            break;
        selection += QLatin1Char('\n');
        block = block.next();
    }
    return selection;
}

void LatexEditor::setTextCursor(const QTextCursor &cursor)
{
    // workaround for QTextControl bug
    bool selectionChange = cursor.hasSelection() || textCursor().hasSelection();
    QTextCursor c = cursor;
    QPlainTextEdit::setTextCursor(c);
    if (selectionChange)
        slotSelectionChanged();
}

void LatexEditor::slotSelectionChanged()
{
    if (inBlockSelectionMode && !textCursor().hasSelection()) {
        inBlockSelectionMode = false;
        blockSelection.clear();
        viewport()->update();
    }

}

void LatexEditor::setUserTagsList(QStringList utlist)
{
userTagsList=utlist;  
}

void LatexEditor::undoText() // workaround for QT bug
{
blockSignals(true); 
undo();
blockSignals(false);
matchAll();
emit updatelineWidget();
emit requestUpdateStructure();
}

void LatexEditor::redoText() // workaround for QT bug
{
blockSignals(true); 
redo();
blockSignals(false);
matchAll();
emit updatelineWidget();
emit requestUpdateStructure();
}

int LatexEditor::insertWithMemoryIndent(QString t) // workaround for QT bug
{
QString Entity=t;
int offset=0;
QString pream="";
QTextCursor cursor=textCursor();
QTextBlock block=cursor.block();
if (block.isValid()) 
	{
	QString txt=block.text();
	int j=0;
	while ( (j<txt.count()) && ((txt[j]==' ') || txt[j]=='\t') ) 
		{
		pream+=QString(txt[j]);
		j++;
		}
	}
offset=pream.length();
Entity.replace("\n","\n"+pream);
insertPlainText(Entity);
return offset;
}

void LatexEditor::dropEvent(QDropEvent *event)
{
event->acceptProposedAction();
#if defined(Q_OS_WIN32)
QRegExp rxpics("file:(/+)(.*\\.(?:eps|pdf|png|jpeg|jpg|tiff))");
#else
QRegExp rxpics("file:(//)(.*\\.(?:eps|pdf|png|jpeg|jpg|tiff))");
#endif
QList<QUrl> uris=event->mimeData()->urls();
if (event && (uris.size()>0))
{
QString uri;
QString tag;
QString finame,fn;
uri=uris.at(0).toString();
QTextCursor cursor = cursorForPosition(event->pos());
int initialCursorPosition = cursor.position();
QPlainTextEdit::dropEvent(event);
cursor.setPosition(initialCursorPosition,QTextCursor::MoveAnchor);
cursor.setPosition(initialCursorPosition+uri.length(),QTextCursor::KeepAnchor);
cursor.removeSelectedText();
if (rxpics.exactMatch(uri))
  {
  fn=rxpics.cap(2);
  if (!fname.startsWith("untitled"))
      {
      QFileInfo fi(fname);
      QDir rootdir=fi.dir();
      QFileInfo fie(rootdir.relativeFilePath(fn));
      cursor.insertText("\\includegraphics[scale=1]{"+fie.filePath()+"} ");
      }
  else 
      {
      QFileInfo fie(fn);
      cursor.insertText("\\includegraphics[scale=1]{"+fie.fileName()+"} ");
      }
  }
setTextCursor(cursor);
}
else
{
QPlainTextEdit::dropEvent(event);
}
}

/*const QRectF LatexEditor::blockGeometry(const QTextBlock & block) 
{
qDebug() << "ok1" << block.isValid() << block.isVisible();
//if (block.isVisible()) return blockBoundingGeometry(block).translated(contentOffset());
//else return QRectF();
return blockBoundingGeometry(block).translated(contentOffset());
//const QRectF rec=blockBoundingGeometry(block);
//qDebug() << "ok2";
//if (rec.isValid()) return rec.translated(contentOffset());
//else return QRectF();
}*/

/*** SCRIPT ***/
void LatexEditor::ExecuteScript(QString location)
{
if ( !location.isEmpty() ) 
	{
	QString text ="";
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QFile scriptfile(location);
	if (scriptfile.open(QFile::ReadOnly))
	    {
	    QTextStream ts_script(&scriptfile);
	    ts_script.setCodec(codec);
	    text= ts_script.readAll();
	    }
	QScriptValue result = fScriptEngine.evaluate(text);
	}
}

void LatexEditor::selectRange(int start, int length)
{
	QTextCursor c = textCursor();
	c.setPosition(start);
	c.setPosition(start + length, QTextCursor::KeepAnchor);
	setTextCursor(c);
}

void LatexEditor::insertText(QString text)
{
textCursor().insertText(text);
}

void LatexEditor::setUncommittedLines(QList<int> lines)
{
    foreach(int l, lines)
    {
        if(document()->findBlockByNumber(l - 1).revision() == getlastSaveRevision())
        {
            document()->findBlockByNumber(l - 1).setRevision(-lastSaveRevision - 1);
        }
    }
emit updatelineWidget();
}

void LatexEditor::resetRevisions()
{
    lastSaveRevision = document()->revision();

    for (QTextBlock block = document()->begin(); block.isValid(); block = block.next())
        block.setRevision(lastSaveRevision);
}

void LatexEditor::updateRevisions()
{
    int oldLastSaveRevision = lastSaveRevision;
    lastSaveRevision = document()->revision();

    if (oldLastSaveRevision != lastSaveRevision) {
        for (QTextBlock block = document()->begin(); block.isValid(); block = block.next()) {
            if (block.revision() < 0 || block.revision() != oldLastSaveRevision)
                block.setRevision(-lastSaveRevision - 1);
            else
                block.setRevision(lastSaveRevision);
        }
    }
}

