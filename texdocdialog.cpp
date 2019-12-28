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
 
#include "texdocdialog.h"
#include <QFileDialog>

TexdocDialog::TexdocDialog( QWidget *parent)
    : QDialog( parent)
{
setModal(true);
ui.setupUi(this);
connect(ui.pushButton, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
ui.moreButton->setCheckable(true);
ui.moreButton->setAutoDefault(false);
connect(ui.moreButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
ui.extension->hide();
updateGeometry();
}

void TexdocDialog::chooseFile()
{
QString fn;
fn =QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !fn.isEmpty() )
	{
	ui.lineEditCommand->setText( fn );
	}
}

void TexdocDialog::expand(bool e)
{
 ui.extension->setVisible(e);
 updateGeometry();
}

