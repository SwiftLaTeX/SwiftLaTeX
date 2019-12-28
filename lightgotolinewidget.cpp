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

#include "lightgotolinewidget.h"


LightGotoLineWidget::LightGotoLineWidget(QWidget* parent)
    : QWidget( parent)
{
ui.setupUi(this);
connect( ui.closeButton, SIGNAL( clicked() ), this, SLOT( doHide() ) );
connect( ui.gotoButton, SIGNAL( clicked() ), this, SLOT( gotoLine() ) );
ui.gotoButton->setShortcut(Qt::Key_Return);
ui.gotoButton->setToolTip("Return");
ui.closeButton->setShortcut(Qt::Key_Escape);
ui.closeButton->setToolTip("Escape");
}

LightGotoLineWidget::~LightGotoLineWidget()
{

}
void LightGotoLineWidget::gotoLine()
{
if ( editor )
  {
  editor->gotoLine( ui.spinLine->value() - 1 );
  editor->viewport()->repaint();
  editor->setFocus();
  }

}
void LightGotoLineWidget::SetEditor(LightLatexEditor *ed)
{
editor=ed;
}

void LightGotoLineWidget::doHide()
{
emit requestHide();
if ( editor ) 
	{
	editor->viewport()->repaint();
	editor->setFocus();
	}
}
