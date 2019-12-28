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

#include "findwidget.h"

#include <QDebug>
#include <QTextCursor>
#include <QMessageBox>
#include "geticon.h"

FindWidget::FindWidget(QWidget* parent)
    : QWidget( parent)
{
ui.setupUi(this);
connect(ui.findButton, SIGNAL( clicked() ), this, SLOT( doFind() ) );
connect(ui.closeButton, SIGNAL( clicked() ), this, SLOT( doHide() ) );
//ui.findButton->setShortcut(Qt::Key_Return);
ui.findButton->setToolTip("Return");
ui.closeButton->setShortcut(Qt::Key_Escape);
ui.closeButton->setToolTip("Escape");
//ui.closeButton->setIcon(getIcon(":/images/fileclose.png"));
ui.moreButton->setCheckable(true);
ui.moreButton->setAutoDefault(false);
connect(ui.moreButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
ui.checkRegExp->setChecked( false );
ui.checkSelection->setChecked( false );
connect(ui.checkSelection, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
ui.extension->hide();
updateGeometry();
 }

FindWidget::~FindWidget()
{

}

void FindWidget::doFind()
{
if ( !editor )
  {
  startpos=-1; 
  endpos=-1;
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
int result=-1;  
if (ui.checkSelection->isChecked())
  {
  if ((startpos>-1) && (endpos>-1)) 
	{
	result=editor->searchInSelection( ui.comboFind->currentText(), ui.checkCase->isChecked(),ui.checkWords->isChecked(), ui.checkRegExp->isChecked(),startpos,endpos); 
	if (result>-1) startpos=result;
	else ui.checkSelection->setChecked( false );  
	}
  }
else
  {
  startpos=-1; 
  endpos=-1;

  if ( !editor->search( ui.comboFind->currentText(), ui.checkCase->isChecked(),	ui.checkWords->isChecked(), ui.radioForward->isChecked(), !ui.checkBegin->isChecked(), ui.checkRegExp->isChecked()) )
    {
    ui.checkBegin->setChecked( true );
    }
  else ui.checkBegin->setChecked( false );
  }
editor->viewport()->repaint();
}

void FindWidget::doHide()
{
emit requestHide();
startpos=-1; 
endpos=-1;
if ( editor ) 
	{
	editor->viewport()->repaint();
	editor->setFocus();
	}
}

void FindWidget::SetEditor(LatexEditor *ed)
{
editor=ed;
}

void FindWidget::expand(bool e)
{
 ui.extension->setVisible(e);
 if (!e) 
    {
    ui.checkRegExp->setChecked( false );
    ui.checkSelection->setChecked( false );
    }
 updateGeometry();
 emit requestExtension();
 if ( editor ) editor->viewport()->repaint();
}

void FindWidget::updateSelection(bool e)
{
ui.checkBegin->setEnabled(!e);
if (e) ui.radioForward->setChecked(true);
if ( !editor ) 
  {
  startpos=-1; 
  endpos=-1;
  }
else if (e)
  {
  QTextCursor c=editor->textCursor();
  if (c.hasSelection()) 
	{
	startpos=c.selectionStart();
	endpos=c.selectionEnd();
	}
  else
	{
	QMessageBox::warning( this,"Texmaker", tr("Text must be selected before checking this option."));
	startpos=-1; 
	endpos=-1;
	ui.checkSelection->setChecked( false );
	}
  }
else
  {
  startpos=-1; 
  endpos=-1;
  }
}

void FindWidget::keyPressEvent ( QKeyEvent * e ) 
{
if ((e->key()==Qt::Key_Enter)||(e->key()==Qt::Key_Return)) doFind();
}
