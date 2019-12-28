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

#include "lightlinenumberwidget.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QScrollBar>
#include <QToolTip>
#include "blockdata.h"
#include "theme.h"

LightLineNumberWidget::LightLineNumberWidget(LightLatexEditor* editor, QWidget* parent)
	: QWidget( parent) ,
	  m_editor( editor )
{
setAutoFillBackground( true );
QPalette p( palette() );
p.setColor( backgroundRole(), Theme::darkbackgroundColor );
setPalette( p );
//setToolTip(tr("Click to add or remove a bookmark"));
start=-1;
end=-1;
connect( m_editor->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( update() ) );
connect( m_editor->verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( update() ) );
connect( m_editor, SIGNAL( textChanged() ), this, SLOT( update() ) );
connect(m_editor, SIGNAL(updatelineWidget()), this, SLOT(update()));
}

LightLineNumberWidget::~LightLineNumberWidget()
{
}

void LightLineNumberWidget::paintEvent( QPaintEvent* event )
{
int max=0;
int l=0;
QPainter painter( this );
painter.setFont(numfont);
const QFontMetrics fm(numfont);
const QBrush oldbrush=painter.brush();
QPen oldpen(Theme::grayColor);
oldpen.setStyle(Qt::SolidLine);
const QBrush bookmarkbrush(QColor("#6F6F6F"));
const QPen bookmarkpen(QColor("#FFFFFF"));

QTextDocument *doc = m_editor->document();
int i = 1;
QTextBlock p = doc->begin();
QTextBlock np;
QString numtext;


painter.setPen(oldpen);
painter.drawLine(width()-2,0,width()-2,height());

int rmin=-1;
int rmax=-1;
int realmin=-1;
int top;
int delta;
while ( p.isValid()) 
	{
	if (p.isVisible()) top = (int) m_editor->blockGeometry(p).top();
	np=p.next();
	if ( np.isValid() && np.isVisible()) delta= (int) m_editor->blockGeometry(np).top()-top;
	else delta=fm.lineSpacing();
	if (top<0)
	  {
	  if (i==start+1)
	    {
	    realmin=top;
	    if (top>=0) rmin=top;
	    else rmin=0;
	    }
	  if (i<=end+1) rmax=top+delta;//top+fm.lineSpacing();
	  p = p.next();
	  i++;
	  continue;
	  }
	  if (top>height()) break;
	  for (int j = 0; j < 3; ++j)
		  {
		  if (m_editor->UserBookmark[j]==i) 
			  {
			  
			  painter.fillRect(2, top,fm.width("0")+6,fm.lineSpacing(), bookmarkbrush);	  
			  painter.setPen(bookmarkpen);
			  painter.drawText(4,top,width()-4,fm.lineSpacing(),Qt::AlignLeft | Qt::AlignTop,QString::number(j+1));
			  }
		  }
	painter.setPen(oldpen);
	numtext=QString::number(i);
	if (p.isVisible()) painter.drawText(10+fm.width("0"), top,fm.width(numtext),fm.lineSpacing(),Qt::AlignRight | Qt::AlignTop,numtext);
	l= fm.width(numtext)+8+fm.width("0");
	if (l>max) max=l;
	if (i==start+1)
	  {
	  realmin=top;
	  if (top>=0) rmin=top;
	  else rmin=0;
	  }
	if (i<=end+1) rmax=top+delta;//top+fm.lineSpacing();

	p = p.next();
	i++;
	}
if (i>=10000) setFixedWidth(max);	
painter.end();
}

void LightLineNumberWidget::mousePressEvent(QMouseEvent *e)
{
e->accept();
const QFontMetrics fm(numfont);
QPoint p = m_editor->viewport()->mapFromGlobal(e->globalPos());
QTextCursor cur( m_editor->cursorForPosition(p) );
int i = m_editor->linefromblock(cur.block());

if ( i==-1 ) return;
for (int j = 0; j < 3; ++j)
      {
      if (m_editor->UserBookmark[j]==i) 
	      {
	      m_editor->UserBookmark[j]=0;
	      update();
	      return;
	      }
      }
for (int j = 0; j < 3; ++j)
      {
      if (m_editor->UserBookmark[j]==0) 
	      {
	      m_editor->UserBookmark[j]=i;
	      update();
	      return;
	      }
      }

}

void LightLineNumberWidget::setFont(QFont efont)
{
numfont=efont;
}

bool LightLineNumberWidget::event(QEvent *event)
{
const QFontMetrics fm(numfont);
if (event->type() == QEvent::ToolTip) 
  {
  QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
  int p = helpEvent->pos().x();
  if (abs(p)>=fm.width("0")+8) 
    {
     QToolTip::hideText();
     event->ignore();
    }
    else QToolTip::showText(helpEvent->globalPos(),tr("Click to add or remove a bookmark"));
  return true;
  }
return QWidget::event(event);
}

