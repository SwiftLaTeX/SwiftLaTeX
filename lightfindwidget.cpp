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

#include "lightfindwidget.h"

#include <QDebug>
#include <QTextCursor>
#include <QMessageBox>

LightFindWidget::LightFindWidget(QWidget* parent)
    : QWidget( parent)
{
ui.setupUi(this);
connect(ui.findButton, SIGNAL( clicked() ), this, SLOT( doFind() ) );
connect(ui.closeButton, SIGNAL( clicked() ), this, SLOT( doHide() ) );
ui.findButton->setShortcut(Qt::Key_Return);
ui.findButton->setToolTip("Return");
ui.closeButton->setShortcut(Qt::Key_Escape);
ui.closeButton->setToolTip("Escape");
ui.moreButton->setCheckable(true);
ui.moreButton->setAutoDefault(false);
connect(ui.moreButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
ui.checkRegExp->setChecked( false );
ui.checkSelection->setChecked( false );
connect(ui.checkSelection, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
ui.extension->hide();
updateGeometry();
 }

LightFindWidget::~LightFindWidget()
{

}

void LightFindWidget::doFind()
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
  if ( !editor->search( ui.comboFind->currentText(), ui.checkCase->isChecked(),	ui.checkWords->isChecked(), ui.radioForward->isChecked(), !ui.checkBegin->isChecked(),ui.checkRegExp->isChecked()) )
    {
    ui.checkBegin->setChecked( true );
    }
  else ui.checkBegin->setChecked( false );
  }
editor->viewport()->repaint();
}

void LightFindWidget::doHide()
{
startpos=-1; 
endpos=-1;
emit requestHide();
if ( editor ) 
	{
	editor->viewport()->repaint();
	editor->setFocus();
	}
}

void LightFindWidget::SetEditor(LightLatexEditor *ed)
{
editor=ed;
}

void LightFindWidget::expand(bool e)
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

void LightFindWidget::updateSelection(bool e)
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
	startpos=-1; 
	endpos=-1;
	}
  }
else
  {
  startpos=-1; 
  endpos=-1;
  }
}
