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

#include "linenumberwidget.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QToolTip>
#include <QScrollBar>
#include <QDebug>
#include "blockdata.h"
#include "theme.h"

LineNumberWidget::LineNumberWidget(LatexEditor* editor, QWidget* parent,bool svn)
	: QWidget( parent) ,
	  m_editor( editor ),
	  m_svn (svn)
{
setAutoFillBackground( true );
QPalette p( palette() );
p.setColor( backgroundRole(), Theme::darkbackgroundColor);
setPalette( p );
//setToolTip(tr("Click to add or remove a bookmark"));
start=-1;
end=-1;
connect( m_editor->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( update() ) );
connect( m_editor->verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( update() ) );
connect( m_editor, SIGNAL( textChanged() ), this, SLOT( update() ) );
//connect(m_editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(update()));
connect(m_editor, SIGNAL(updatelineWidget()), this, SLOT(update()));
connect( m_editor, SIGNAL( setBlockRange(int,int) ), this, SLOT( setRange(int,int) ) );
}

LineNumberWidget::~LineNumberWidget()
{
}

void LineNumberWidget::paintEvent( QPaintEvent* event )
{
int max=0;
int l=0;
QPainter painter( this );
painter.setFont(numfont);
const QFontMetrics fm(numfont);
const QBrush oldbrush=painter.brush();
QPen oldpen(Theme::grayColor);
QPen oldpenSvn(QColor("#FF00FF"));
oldpen.setStyle(Qt::SolidLine);
const QBrush bookmarkbrush(QColor("#6F6F6F"));
const QBrush markerbrush(QColor("#FF6F00"));
const QPen bookmarkpen(QColor("#FFFFFF"));
QPen rangepen(QColor("#FF6F00"));

QTextDocument *doc = m_editor->document();
int i = 1;
QTextBlock p = doc->begin();
QTextBlock np;
QString numtext;


painter.setPen(oldpen);
painter.drawLine(width()-2,0,width()-2,height());

rangepen.setWidth(1);
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
        if (m_svn && (m_editor->document()->findBlockByNumber(i-1).revision() != m_editor->getlastSaveRevision()))
        {
	painter.setPen(oldpenSvn);
	}
	else painter.setPen(oldpen);
	numtext=QString::number(i);
	if (p.isVisible()) painter.drawText(10+fm.width("0"), top,fm.width(numtext),fm.lineSpacing(),Qt::AlignRight | Qt::AlignTop,numtext);
	l= fm.width(numtext)+18+fm.width("0");
	if (l>max) max=l;
	if (i==start+1)
	  {
	  realmin=top;
	  if (top>=0) rmin=top;
	  else rmin=0;
	  }
	if (i<=end+1) rmax=top+delta;//top+fm.lineSpacing();
	if ((m_editor->foldedLines.keys().contains(i-1)) && (i!=start+1))
	  {
	  painter.setPen(bookmarkpen);
	  painter.fillRect(width()-20, top+(int) (fm.lineSpacing()/2)-5,10,10, markerbrush);
	  painter.drawLine(width()-17,top+(int) (fm.lineSpacing()/2),width()-13,top+(int) (fm.lineSpacing()/2));
	  painter.drawLine(width()-15,top+(int) (fm.lineSpacing()/2)-2,width()-15,top+(int) (fm.lineSpacing()/2)+2);
	  rangepen.setStyle(Qt::DotLine);
	  painter.setPen(rangepen);
	  painter.drawLine(fm.width("0")+8,top+fm.lineSpacing(),width()-12,top+fm.lineSpacing() );
	  rangepen.setStyle(Qt::SolidLine);
	  painter.setPen(rangepen);
	  }
	p = p.next();
	i++;
	}
if (i>=10000) setFixedWidth(max);	
//const QBrush rangebrush(QColor("#FF8000"));

painter.setPen(rangepen);
if ((rmin>=0) && (rmax>=0)) 
    {
    if (realmin>=0)
    {
    	  painter.setPen(bookmarkpen);
	  painter.fillRect(width()-20,rmin+(int) (fm.lineSpacing()/2)-5,10,10, markerbrush);
	  painter.drawLine(width()-17,rmin+(int) (fm.lineSpacing()/2),width()-14,rmin+(int) (fm.lineSpacing()/2));
	  painter.setPen(rangepen);
    if (m_editor->foldedLines.keys().contains(start)) 
      {
	painter.setPen(bookmarkpen);
	painter.drawLine(width()-17,rmin+(int) (fm.lineSpacing()/2),width()-13,rmin+(int) (fm.lineSpacing()/2));
	  painter.drawLine(width()-15,rmin+(int) (fm.lineSpacing()/2)-2,width()-15,rmin+(int) (fm.lineSpacing()/2)+2);
    	  rangepen.setStyle(Qt::DotLine);
	  painter.setPen(rangepen);
      painter.drawLine(fm.width("0")+8,rmin+fm.lineSpacing(),width()-12,rmin+fm.lineSpacing() );
    	rangepen.setStyle(Qt::SolidLine);
	  painter.setPen(rangepen);
      }
    else  
      {
	//painter.drawLine(width()-7,rmin+2,width()-7,qMin(rmax,height()));
	painter.fillRect(width()-7,rmin+2,2,qMin(rmax,height())-rmin-2, markerbrush);
	//painter.drawLine(width()-6,rmin+2,width()-6,qMin(rmax,height()));
	
      }

    }
    else 
    {
      //painter.drawLine(width()-7,rmin,width()-7,qMin(rmax,height())); 
      painter.fillRect(width()-7,rmin,2,qMin(rmax,height())-rmin, markerbrush);
      //painter.drawLine(width()-6,rmin,width()-6,qMin(rmax,height()));
      
    }
    }
painter.end();
}

void LineNumberWidget::mousePressEvent(QMouseEvent *e)
{
e->accept();
const QFontMetrics fm(numfont);
QPoint p = m_editor->viewport()->mapFromGlobal(e->globalPos());
QTextCursor cur( m_editor->cursorForPosition(p) );
int i = m_editor->linefromblock(cur.block());
if (abs(p.x())<width()-fm.width("0")-6) 
  {
  if (i==start+1) 
  {
/* QMapIterator<int, int> it(m_editor->foldedLines);
 while (it.hasNext()) {
     it.next();
     qDebug() << "folded" << it.key() << ": " << it.value();
 }*/   
    if (m_editor->foldedLines.keys().contains(start)) 
      {
      m_editor->unfold(start,end);
      }
    else  {m_editor->fold(start,end);}
  }
  else if (m_editor->foldedLines.keys().contains(i-1)) {m_editor->unfold(i-1,m_editor->foldedLines[i-1]);}
  }
else
  {
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
}

void LineNumberWidget::setFont(QFont efont)
{
numfont=efont;
}

void LineNumberWidget::setSvn(bool svn)
{
m_svn=svn;
}

void LineNumberWidget::setRange(int s, int e)
{
start=s;
end=e;
update();
}

bool LineNumberWidget::event(QEvent *event)
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

