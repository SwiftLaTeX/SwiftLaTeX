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

#include "replacewidget.h"
#include <QMessageBox>

ReplaceWidget::ReplaceWidget(QWidget* parent)
    : QWidget( parent)
{
ui.setupUi(this);
connect( ui.findButton, SIGNAL( clicked() ), this, SLOT( doReplace() ) );
connect( ui.replaceallButton, SIGNAL( clicked() ), this, SLOT( doReplaceAll() ) );
connect( ui.closeButton, SIGNAL( clicked() ), this, SLOT( doHide() ) );
//ui.findButton->setShortcut(Qt::Key_Return);
ui.findButton->setToolTip("Return");
ui.closeButton->setShortcut(Qt::Key_Escape);
ui.closeButton->setToolTip("Escape");
ui.moreButton->setCheckable(true);
ui.moreButton->setAutoDefault(false);
connect(ui.moreButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
ui.checkRegExp->setChecked( false );
ui.checkSelection->setChecked( false );
connect(ui.checkSelection, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
connect(ui.checkRegExp, SIGNAL(toggled(bool)), this, SLOT(updateReg(bool)));
ui.extension->hide();
updateGeometry();
}


ReplaceWidget::~ReplaceWidget()
{
}

void ReplaceWidget::doReplace()
{
//doHide();

if ( !editor )
  {
  startpos=-1; 
  endpos=-1;
  deltacol=0;
  return;
  }
if (ui.checkRegExp->isChecked())
  {
  QRegExp regex(ui.comboFind->currentText());
  if (!regex.isValid()) 
    {
    QMessageBox::warning( this,tr("Error"), tr("Invalid regular expression."));
    return;
    }
  }
bool go=true;
int result=-1; 

if (ui.checkSelection->isChecked())
  {
  while (go && (startpos>-1) && (endpos>-1))
    {
     result=editor->searchInSelection( ui.comboFind->currentText(), ui.checkCase->isChecked(),ui.checkWords->isChecked(), ui.checkRegExp->isChecked(),startpos,endpos); 
     if (result>-1)
      {
      startpos=result;
      switch(  QMessageBox::warning(this, "Texmaker",tr("Replace this occurence ? "),tr("Yes"), tr("No"), tr("Cancel"), 0,2 ) )
	{
	case 0:
	replaceSelection();  
	ui.checkBegin->setChecked( false );
	  break;
	case 1:
	ui.checkBegin->setChecked( false );
	  break;
	case 2:
	go=false;
	ui.checkSelection->setChecked( false );
	  break;
	}
      }
     else ui.checkSelection->setChecked( false );  
    }
  }
else
  {
  while (go && editor->search( ui.comboFind->currentText(), ui.checkCase->isChecked(),
	  ui.checkWords->isChecked(), ui.radioForward->isChecked(), !ui.checkBegin->isChecked(),ui.checkRegExp->isChecked()) )
	{
	switch(  QMessageBox::warning(this, "Texmaker",tr("Replace this occurence ? "),tr("Yes"), tr("No"), tr("Cancel"), 0,2 ) )
	  {
	  case 0:
	  editor->replace(ui.comboReplace->currentText(),ui.checkRegExp->isChecked(),ui.comboFind->currentText() );
	  ui.checkBegin->setChecked( false );
	    break;
	  case 1:
	  ui.checkBegin->setChecked( false );
	    break;
	  case 2:
	  go=false;
	    break;
	  }
	}
  if (go) ui.checkBegin->setChecked( true );
  }
}

void ReplaceWidget::doReplaceAll()
{
QString selectedword="";
if ( !editor )
  {
  startpos=-1; 
  endpos=-1;
  deltacol=0;
  return;
  }
if (ui.checkRegExp->isChecked())
  {
  QRegExp regex(ui.comboFind->currentText());
  if (!regex.isValid()) 
    {
    QMessageBox::warning( this,tr("Error"), tr("Invalid regular expression."));
    return;
    }
  }
bool go=true;
int result=-1;
if (ui.checkSelection->isChecked())
  {
  while (go && (startpos>-1) && (endpos>-1))
    {    
    result=editor->searchInSelection( ui.comboFind->currentText(), ui.checkCase->isChecked(),ui.checkWords->isChecked(), ui.checkRegExp->isChecked(),startpos,endpos); 
     if (result>-1)
	{
	
	startpos=result;
	replaceSelection();
	go=true;
	}
      else 
	{
	go=false;
	ui.checkSelection->setChecked( false );
	}
    }
  }
else
  {
  while ( editor->search( ui.comboFind->currentText(), ui.checkCase->isChecked(),
  ui.checkWords->isChecked(), ui.radioForward->isChecked(), !ui.checkBegin->isChecked(),ui.checkRegExp->isChecked()) )
      {
      editor->replace(ui.comboReplace->currentText(),ui.checkRegExp->isChecked(),ui.comboFind->currentText() );
      ui.checkBegin->setChecked( false );
      }
  ui.checkBegin->setChecked( true );
  }
}

void ReplaceWidget::SetEditor(LatexEditor *ed)
{
editor=ed;
}

void ReplaceWidget::doHide()
{
emit requestHide();
startpos=-1; 
endpos=-1;
deltacol=0;
if ( editor ) 
	{
	editor->viewport()->repaint();
	editor->setFocus();
	}
}

void ReplaceWidget::expand(bool e)
{
 ui.extension->setVisible(e);
  if (!e) 
    {
    ui.checkRegExp->setChecked( false );
    ui.checkSelection->setChecked( false );
    }
 updateGeometry();
 emit requestExtension();
 editor->viewport()->repaint();
}

void ReplaceWidget::updateSelection(bool e)
{
ui.checkBegin->setEnabled(!e);

if (e) 
  {
  ui.radioForward->setChecked(true);
  disconnect(ui.checkRegExp, SIGNAL(toggled(bool)), this, SLOT(updateReg(bool)));
  ui.checkRegExp->setChecked( false );
  ui.checkRegExp->setEnabled(false);
  connect(ui.checkRegExp, SIGNAL(toggled(bool)), this, SLOT(updateReg(bool)));
  }
else ui.checkRegExp->setEnabled(true);
if ( !editor ) 
  {
  startpos=-1; 
  endpos=-1;
  deltacol=0;
  }
else if (e)
  {
  QTextCursor c=editor->textCursor();
  if (c.hasSelection()) 
	{
	startpos=c.selectionStart();
	endpos=c.selectionEnd();
	deltacol=0;
	}
  else
	{
	QMessageBox::warning( this,"Texmaker", tr("Text must be selected before checking this option."));
	startpos=-1; 
	endpos=-1;
	deltacol=0;
	ui.checkSelection->setChecked( false );
	}
  }
else
  {
  startpos=-1; 
  endpos=-1;
  deltacol=0;
  }
}

void ReplaceWidget::updateReg(bool e)
{
if (e) 
  {
  disconnect(ui.checkSelection, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
  ui.checkSelection->setChecked( false );
  ui.checkSelection->setEnabled(false);
  connect(ui.checkSelection, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
  }
else ui.checkSelection->setEnabled(true);
}

void ReplaceWidget::replaceSelection()
{
QString selectedword="";
QTextCursor cursor=editor->textCursor();
if (cursor.hasSelection()) selectedword=cursor.selectedText();
QString newword=ui.comboReplace->currentText();
if (!newword.isEmpty()) 
	{
	deltacol=newword.length()-selectedword.length();
	editor->replace(ui.comboReplace->currentText(),ui.checkRegExp->isChecked(),ui.comboFind->currentText() );
	startpos=startpos+deltacol;
	endpos=endpos+deltacol;
	}

}

void ReplaceWidget::keyPressEvent ( QKeyEvent * e ) 
{
if ((e->key()==Qt::Key_Enter)||(e->key()==Qt::Key_Return)) doReplace();
}
