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

#include "latexeditorview.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QTextDocument>
#include <QTextCursor>
#include <QLineEdit>
#include <QTextBlock>

LatexEditorView::LatexEditorView(QWidget *parent,QFont & efont,bool svn,bool line,QList<QColor> edcolors, QList<QColor> hicolors,bool inlinespelling,QString ignoredWords,Hunspell *spellChecker,bool tabspaces, int tabwidth,const QKeySequence viewfocus,QString name,QStringList ulist) : QWidget(parent)
{
  
splitter=new MiniSplitter(this);
splitter->setOrientation(Qt::Vertical);

QFrame *framebis=new QFrame(splitter);
framebis->setLineWidth(0);
framebis->setFrameShape(QFrame::NoFrame);
framebis->setFrameShadow(QFrame::Plain);
framebis->setFrameStyle(QFrame::NoFrame);


QVBoxLayout* mainlay = new QVBoxLayout(framebis );
mainlay->setSpacing(0);
mainlay->setMargin(0);


QFrame *frame=new QFrame(framebis);
frame->setLineWidth(1);
frame->setFrameShape(QFrame::NoFrame);
frame->setFrameShadow(QFrame::Plain);
frame->setFrameStyle(QFrame::NoFrame);
mainlay->addWidget(frame);

editor=new LatexEditor(frame,efont,edcolors,hicolors,inlinespelling,ignoredWords,spellChecker,tabspaces,tabwidth,viewfocus,name,ulist);
m_lineNumberWidget = new LineNumberWidget( editor, frame,svn);
m_lineNumberWidget->setFont(efont);
QFontMetrics fm( efont );
m_lineNumberWidget->setFixedWidth( fm.width( "00000" ) + 32 );
QHBoxLayout* lay = new QHBoxLayout( frame );
lay->setSpacing(0);
lay->setMargin(0);
lay->addWidget( m_lineNumberWidget );
lay->addWidget( editor );
setFocusProxy( editor );
setLineNumberWidgetVisible(line);

Stack=new QStackedWidget(this);
Stack->setLineWidth(0);
Stack->setFrameShape(QFrame::NoFrame);
Stack->setFrameShadow(QFrame::Plain);

findwidget=new FindWidget(Stack);
Stack->addWidget(findwidget);
findwidget->SetEditor(editor);
connect(findwidget, SIGNAL( requestHide() ), Stack, SLOT( hide() ) );
connect(findwidget, SIGNAL( requestExtension() ), this, SLOT( updateFind() ) );


replacewidget=new ReplaceWidget(Stack);
Stack->addWidget(replacewidget);
replacewidget->SetEditor(editor);
connect(replacewidget, SIGNAL( requestHide() ), Stack, SLOT( hide() ) );
connect(replacewidget, SIGNAL( requestExtension() ), this, SLOT( updateReplace() ) );

connect(editor,SIGNAL(textChanged()), this, SLOT( updateInSelection() ) );

gotolinewidget=new GotoLineWidget(Stack);
Stack->addWidget(gotolinewidget);
gotolinewidget->SetEditor(editor);
connect(gotolinewidget, SIGNAL( requestHide() ), Stack, SLOT( hide() ) );

Stack->hide();

splitter->addWidget(framebis);
splitter->addWidget(Stack);
QVBoxLayout *mainlayout= new QVBoxLayout(this);
mainlayout->setSpacing(0);
mainlayout->setMargin(0);
mainlayout->addWidget(splitter);
}

LatexEditorView::~LatexEditorView()
{
}

void LatexEditorView::setLineNumberWidgetVisible( bool b )
{
    if( b ){
	m_lineNumberWidget->show();
    } else {
	m_lineNumberWidget->hide();
    }
}

void LatexEditorView::changeSettings(QFont & new_font,bool svn,bool line)
{
  editor->changeFont(new_font);
  m_lineNumberWidget->setFont(new_font);
  QFontMetrics fm( new_font );
  m_lineNumberWidget->setFixedWidth( fm.width( "00000" ) + 32 );
  m_lineNumberWidget->setSvn(svn);
  setLineNumberWidgetVisible(line);
}

void LatexEditorView::showFind()
{
QList<int> sizes;
sizes  << height()-findwidget->height() << findwidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(findwidget);
Stack->setMaximumHeight(findwidget->minimumSizeHint().height());
Stack->show();
QTextCursor c =editor->textCursor();
if (c.hasSelection()) findwidget->ui.comboFind->lineEdit()->setText(c.selectedText());
findwidget->ui.comboFind->setFocus();
findwidget->ui.comboFind->lineEdit()->selectAll();
}

void LatexEditorView::updateFind()
{
QList<int> sizes;
sizes  << height()-findwidget->height() << findwidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(findwidget);
Stack->setMaximumHeight(findwidget->minimumSizeHint().height());
Stack->show();
QTextCursor c =editor->textCursor();
if (c.hasSelection()) findwidget->ui.comboFind->lineEdit()->setText(c.selectedText());
findwidget->ui.comboFind->setFocus();
findwidget->ui.comboFind->lineEdit()->selectAll();
}

void LatexEditorView::showFindNext()
{
QList<int> sizes;
sizes << height()-findwidget->height() << findwidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(findwidget);
Stack->setMaximumHeight(findwidget->minimumSizeHint().height());
Stack->show();
if (findwidget->ui.comboFind->lineEdit()->text()!="") findwidget->doFind();
}

void LatexEditorView::showReplace()
{
QList<int> sizes;
sizes  << height()-replacewidget->height() << replacewidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(replacewidget);
Stack->setMaximumHeight(replacewidget->minimumSizeHint().height());
Stack->show();
QTextCursor c =editor->textCursor();
if (c.hasSelection()) replacewidget->ui.comboFind->lineEdit()->setText(c.selectedText());
replacewidget->ui.comboFind->setFocus();
replacewidget->ui.comboFind->lineEdit()->selectAll();
}

void LatexEditorView::updateReplace()
{
QList<int> sizes;
sizes  << height()-replacewidget->height() << replacewidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(replacewidget);
Stack->setMaximumHeight(replacewidget->minimumSizeHint().height());
Stack->show();
QTextCursor c =editor->textCursor();
if (c.hasSelection()) replacewidget->ui.comboFind->lineEdit()->setText(c.selectedText());
replacewidget->ui.comboFind->setFocus();
replacewidget->ui.comboFind->lineEdit()->selectAll();
}

void LatexEditorView::showGoto()
{
QList<int> sizes;
sizes  << height()-gotolinewidget->height() << gotolinewidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(gotolinewidget);
Stack->setMaximumHeight(gotolinewidget->minimumSizeHint().height());
Stack->show();
gotolinewidget->ui.spinLine->setFocus();
gotolinewidget->ui.spinLine->setMinimum( 1 );
gotolinewidget->ui.spinLine->setMaximum(editor->numoflines() );
gotolinewidget->ui.spinLine->selectAll();
}

void LatexEditorView::updateInSelection()
{
findwidget->ui.checkSelection->setChecked( false );
}


