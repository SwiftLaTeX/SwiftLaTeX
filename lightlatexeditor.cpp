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

#include "lightlatexeditor.h"

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
#include <QTextCodec>
#include <QFileInfo>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextDocumentFragment>

#include "blockdata.h"
#include "encodingdialog.h"

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

LightLatexEditor::LightLatexEditor(QWidget *parent,QFont & efont, QList<QColor> edcolors, QList<QColor> hicolors,QString name) : QPlainTextEdit(parent)
{
fname=name;
/***********************/
inBlockSelectionMode=false;
blockSelection.tabSize=4;
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
setTabStopWidth(fontMetrics().width("    "));
setTabChangesFocus(false);
endBlock=-1;
lastnumlines=0;

highlighter = new LightLatexHighlighter(document());
highlighter->SetEditor(this);
highlighter->setColors(hicolors);
QFileInfo fi(fname);
if (!fname.startsWith("untitled") && (fi.suffix()=="asy" || fi.suffix()=="mp")) highlighter->setModeGraphic(true);

connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchAll()));

setReadOnly(true);
setEnabled(true);
setFocusPolicy(Qt::WheelFocus);
setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
setFocus();

}
LightLatexEditor::~LightLatexEditor(){
}

void LightLatexEditor::setColors(QList<QColor> colors)
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
update();
}

bool LightLatexEditor::search( const QString &expr, bool cs, bool wo, bool forward, bool startAtCursor, bool isRegExp)
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
QTextDocument::FindFlags options;
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

int LightLatexEditor::searchInSelection( const QString &expr, bool cs, bool wo, bool isRegExp, int start, int end)
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

int LightLatexEditor::searchLine( const QString &expr)
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

void LightLatexEditor::gotoLine( int line )
{
if (line<=numoflines())  
  {
  setCursorPosition( line, 0 );
  }
}


void LightLatexEditor::changeFont(QFont & new_font)
{
setFont(new_font);
}

QString LightLatexEditor::getEncoding()
{
 return encoding;
}

void LightLatexEditor::setEncoding(QString enc)
{
 encoding=enc;
}  

int LightLatexEditor::getCursorPosition(int para, int index)
{
return document()->findBlockByNumber(para).position()+index;
}

void LightLatexEditor::setCursorPosition(int para, int index)
{
int pos=getCursorPosition(para,index);
QTextCursor cur=textCursor();
cur.setPosition(pos,QTextCursor::MoveAnchor);
if (cur.block().isVisible()) setTextCursor(cur);
ensureCursorVisible();
setFocus();
}



int LightLatexEditor::numoflines()
{
return document()->blockCount();
}

int LightLatexEditor::linefromblock(const QTextBlock& p)
{
return p.blockNumber()+1;
}

void LightLatexEditor::selectword(int line, int col, QString word)
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

void LightLatexEditor::insertNewLine()
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


void LightLatexEditor::matchAll() 
{
viewport()->update();
QList<QTextEdit::ExtraSelection> selections;
setExtraSelections(selections);
matchPar();
emit updatelineWidget();
}

void LightLatexEditor::matchPar() 
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

bool LightLatexEditor::matchLeftPar(	QTextBlock currentBlock, int index, int numLeftPar ) 
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

bool LightLatexEditor::matchRightPar(QTextBlock currentBlock, int index, int numRightPar) 
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

void LightLatexEditor::createParSelection( int pos ) 
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

void LightLatexEditor::jumpToEndBlock()
{
gotoLine(endBlock);
QTextCursor c = textCursor();
c.movePosition(QTextCursor::EndOfBlock);
setTextCursor(c);
}

void LightLatexEditor::ensureFinalNewLine()//Qt 4.7.1 bug
{
QTextCursor cursor = textCursor();
cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
bool emptyFile = !cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
if (!emptyFile && cursor.selectedText().at(0) != QChar::ParagraphSeparator)
    {
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    cursor.insertText(QLatin1String("\n"));
    }
}

int LightLatexEditor::getLastNumLines()
{
return lastnumlines;
}

void LightLatexEditor::setLastNumLines(int n)
{
lastnumlines=n;  
}

QString LightLatexEditor::beginningLine()
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

void LightLatexEditor::load( const QString &f )
{
QFile file( f );
if ( !file.open( QIODevice::ReadOnly ) )
	{
	QMessageBox::warning( this,tr("Error"), tr("You do not have read permission to this file."));
	return;
	}
bool hasDecodingError=false;
QByteArray buf = file.readAll();
int bytesRead = buf.size();
file.close();

QTextCodec* detected_codec;
QTextCodec* codec = QTextCodec::codecForName(encoding.toLatin1());
if(!codec) codec = QTextCodec::codecForLocale();
QString text = codec->toUnicode(buf);
QByteArray verifyBuf = codec->fromUnicode(text);
QString new_encoding="";
// unicode detection
if (bytesRead >= 4 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe && uchar(buf[2]) == 0 && uchar(buf[3]) == 0) || (uchar(buf[0]) == 0 && uchar(buf[1]) == 0 && uchar(buf[2]) == 0xfe && uchar(buf[3]) == 0xff))) 
    {
      detected_codec = QTextCodec::codecForName("UTF-32");
      if (detected_codec) new_encoding=detected_codec->name();
    } 
else if (bytesRead >= 2 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe) || (uchar(buf[0]) == 0xfe && uchar(buf[1]) == 0xff))) 
    {
      detected_codec = QTextCodec::codecForName("UTF-16");
      if (detected_codec) new_encoding=detected_codec->name();

    } 
else if (bytesRead >= 3 && uchar(buf[0]) == 0xef && uchar(buf[1]) == 0xbb && uchar(buf[2])== 0xbf) 
    {
      detected_codec = QTextCodec::codecForName("UTF-8");
      if (detected_codec) new_encoding=detected_codec->name();
    }
else
{
const char *  	data= buf.constData();
int length=buf.size();
bool canbeutf8=true;
static const unsigned char highest1Bits = 0x80;
static const unsigned char highest2Bits = 0xC0;
static const unsigned char highest3Bits = 0xE0;
static const unsigned char highest4Bits = 0xF0;
static const unsigned char highest5Bits = 0xF8;
int multiByte=0;
for (int i=0; i<length; ++i)
  {
      unsigned char c = data[i];

      if (multiByte>0)
      {
	  if ((c & highest2Bits) == 0x80)
	  {
	      --(multiByte);
	      continue;
	  }
	  canbeutf8=false;
	  break;
      }

      // most significant bit zero, single char
      if ((c & highest1Bits) == 0x00)
	  continue;

      // 110xxxxx => init 1 following bytes
      if ((c & highest3Bits) == 0xC0)
      {
	  multiByte = 1;
	  continue;
      }

      // 1110xxxx => init 2 following bytes
      if ((c & highest4Bits) == 0xE0)
      {
	  multiByte = 2;
	  continue;
      }

      // 11110xxx => init 3 following bytes
      if ((c & highest5Bits) == 0xF0)
      {
	  multiByte = 3;
	  continue;
      }
	canbeutf8=false;
	  break;
  }
if (canbeutf8) 
      {
      detected_codec = QTextCodec::codecForName("UTF-8");
      if (detected_codec) new_encoding=detected_codec->name();
      }
 }  

if (new_encoding!="")
  {
    if (new_encoding!=codec->name()) hasDecodingError=true;
  }
else
  {  
  // no unicode
   int minSize = qMin(verifyBuf.size(), buf.size());
  hasDecodingError = (minSize < buf.size()- 4 || memcmp(verifyBuf.constData() + verifyBuf.size() - minSize,buf.constData() + buf.size() - minSize, minSize));
  QEncodingProber prober (QEncodingProber::Universal);
  if (hasDecodingError)
    {
    prober.feed (buf.constData());
    if (prober.confidence() > 0.6) //Kencodingprober works very bad with tex documents
      {
      detected_codec = QTextCodec::codecForName(prober.encoding());
      if (detected_codec) new_encoding=detected_codec->name();
      else if (encoding=="UTF-8") new_encoding="ISO-8859-1";
      else if (encoding=="ISO-8859-1") new_encoding="UTF-8";
      else new_encoding=QString(QTextCodec::codecForLocale()->name());
      }
    else if (encoding=="UTF-8") new_encoding="ISO-8859-1";
    else if (encoding=="ISO-8859-1") new_encoding="UTF-8";
    else new_encoding=QString(QTextCodec::codecForLocale()->name());
    }

  }

if (hasDecodingError)
  {
  EncodingDialog *encDlg = new EncodingDialog(this);
  encDlg->ui.comboBoxEncoding->setCurrentIndex(encDlg->ui.comboBoxEncoding->findText(new_encoding, Qt::MatchExactly));
  encDlg->ui.label->setText(encDlg->ui.label->text()+ " ("+encoding+").");
  if (encDlg->exec())
	  {
	  new_encoding=encDlg->ui.comboBoxEncoding->currentText();
	  codec = QTextCodec::codecForName(new_encoding.toLatin1());
	  text = codec->toUnicode(buf);
	  }
  else return;
  }

// QTextCodec* codec = QTextCodec::codecForName(encoding.toLatin1());
// if(!codec) codec = QTextCodec::codecForLocale();
// #if 0 // should work, but does not, Qt bug with "system" codec
// QTextDecoder *decoder = codec->makeDecoder();
// QString text = decoder->toUnicode(buf);
// hasDecodingError = (decoder->hasFailure());
// delete decoder;
// #else
// QString text = codec->toUnicode(buf);
// QByteArray verifyBuf = codec->fromUnicode(text); // slow
// // the minSize trick lets us ignore unicode headers
// int minSize = qMin(verifyBuf.size(), buf.size());
// hasDecodingError = (minSize < buf.size()- 4 || memcmp(verifyBuf.constData() + verifyBuf.size() - minSize,buf.constData() + buf.size() - minSize, minSize));
// #endif
// QString new_encoding;
// QEncodingProber prober (QEncodingProber::Universal);
// if (hasDecodingError)
//   {
//   prober.feed (buf.constData());
//   QTextCodec* detected_codec;
//   if (prober.confidence() > 0.5) //Kencodingprober works very bad with tex documents
//     {
//     detected_codec = QTextCodec::codecForName(prober.encoding());
//     if (detected_codec) new_encoding=detected_codec->name();
//     else if (encoding=="UTF-8") new_encoding="ISO-8859-1";
//     else if (encoding=="ISO-8859-1") new_encoding="UTF-8";
//     else new_encoding=QString(QTextCodec::codecForLocale()->name());
//     }
//   else if (encoding=="UTF-8") new_encoding="ISO-8859-1";
//   else if (encoding=="ISO-8859-1") new_encoding="UTF-8";
//   else new_encoding=QString(QTextCodec::codecForLocale()->name());
//   EncodingDialog *encDlg = new EncodingDialog(this);
//   encDlg->ui.comboBoxEncoding->setCurrentIndex(encDlg->ui.comboBoxEncoding->findText(new_encoding, Qt::MatchExactly));
//   encDlg->ui.label->setText(encDlg->ui.label->text()+ " ("+encoding+").");
//   if (encDlg->exec())
// 	  {
// 	  new_encoding=encDlg->ui.comboBoxEncoding->currentText();
// 	  codec = QTextCodec::codecForName(new_encoding.toLatin1());
// 	  text = codec->toUnicode(buf);
// 	  }
//   else return;
//   }
if (hasDecodingError) setEncoding(new_encoding);
else setEncoding(encoding);
updateName(f);
setPlainText(text);
}

void LightLatexEditor::paintEvent(QPaintEvent *event)
{
QRect rect = cursorRect();
QRect rectbis=rect;
rectbis.setX(0);
rectbis.setWidth(viewport()->width());
QPainter painter(viewport());
//const QBrush brush(QColor("#000000"));
const QBrush brushbis(colorLine);
painter.fillRect(rectbis, brushbis);
//painter.fillRect(rect, brush);
//painter.end();
//QPlainTextEdit::paintEvent(event);
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
#if !defined(Q_OS_MAC)
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

void LightLatexEditor::contextMenuEvent(QContextMenuEvent *e)
{
QMenu *menu=createStandardContextMenu();
menu->addSeparator();
QAction *Act;
Act = new QAction(tr("Click to jump to the bookmark")+"1", this);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoBookmark1()));
menu->addAction(Act);
Act = new QAction(tr("Click to jump to the bookmark")+"2", this);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoBookmark2()));
menu->addAction(Act);
Act = new QAction(tr("Click to jump to the bookmark")+"3", this);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoBookmark3()));
menu->addAction(Act);
menu->addSeparator();
Act = new QAction( tr("Find"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(editFind()));
menu->addAction(Act);
Act = new QAction(tr("Goto Line"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(editGotoLine()));
menu->addAction(Act);
menu->exec(e->globalPos());
delete menu;
}

void LightLatexEditor::gotoBookmark1()
{
int l=UserBookmark[0];
if (l>0) gotoLine(l-1);
}

void LightLatexEditor::gotoBookmark2()
{
int l=UserBookmark[1];
if (l>0) gotoLine(l-1);
}

void LightLatexEditor::gotoBookmark3()
{
int l=UserBookmark[2];
if (l>0) gotoLine(l-1);
}

void LightLatexEditor::editFind()
{
emit requestFind();
}

void LightLatexEditor::editGotoLine()
{
emit requestGotoLine();
}

void LightLatexEditor::updateName(QString f)
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

void LightLatexEditor::mouseMoveEvent(QMouseEvent *e)
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

void LightLatexEditor::mousePressEvent(QMouseEvent *e)
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

void LightLatexEditor::removeBlockSelection(const QString &text)
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

QMimeData *LightLatexEditor::createMimeDataFromSelection() const
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

bool LightLatexEditor::canInsertFromMimeData(const QMimeData *source) const
{
    return QPlainTextEdit::canInsertFromMimeData(source);
}

void LightLatexEditor::insertFromMimeData(const QMimeData *source)
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

QString LightLatexEditor::copyBlockSelection() const
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

void LightLatexEditor::setTextCursor(const QTextCursor &cursor)
{
    // workaround for QTextControl bug
    bool selectionChange = cursor.hasSelection() || textCursor().hasSelection();
    QTextCursor c = cursor;
    QPlainTextEdit::setTextCursor(c);
    if (selectionChange)
        slotSelectionChanged();
}

void LightLatexEditor::slotSelectionChanged()
{
    if (inBlockSelectionMode && !textCursor().hasSelection()) {
        inBlockSelectionMode = false;
        blockSelection.clear();
        viewport()->update();
    }

}
