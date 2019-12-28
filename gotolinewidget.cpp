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

#include "gotolinewidget.h"
#include <QAction>

GotoLineWidget::GotoLineWidget(QWidget* parent)
    : QWidget( parent)
{
ui.setupUi(this);
connect( ui.closeButton, SIGNAL( clicked() ), this, SLOT( doHide() ) );
connect( ui.gotoButton, SIGNAL( clicked() ), this, SLOT( gotoLine() ) );
ui.gotoButton->setToolTip("Return");
ui.closeButton->setShortcut(Qt::Key_Escape);
ui.closeButton->setToolTip("Escape");
}

GotoLineWidget::~GotoLineWidget()
{

}
void GotoLineWidget::gotoLine()
{
if ( editor )
  {
  editor->gotoLine( ui.spinLine->value() - 1 );
  editor->viewport()->repaint();
  editor->setFocus();
  }

}
void GotoLineWidget::SetEditor(LatexEditor *ed)
{
editor=ed;
}

void GotoLineWidget::doHide()
{
emit requestHide();
if ( editor ) 
	{
	editor->viewport()->repaint();
	editor->setFocus();
	}
}

void GotoLineWidget::keyPressEvent ( QKeyEvent * e ) 
{
if ((e->key()==Qt::Key_Enter)||(e->key()==Qt::Key_Return)) gotoLine();
}
