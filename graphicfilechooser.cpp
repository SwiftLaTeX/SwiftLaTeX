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
 
#include "graphicfilechooser.h"
#include <QFileDialog>

GraphicFileChooser::GraphicFileChooser( QWidget *parent, QString name)
    : QDialog( parent)
{
setWindowTitle(name);
setModal(true);
ui.setupUi(this);
connect(ui.lineEdit, SIGNAL(textChanged( const QString & ) ), this, SIGNAL( fileNameChanged( const QString & ) ) );
connect(ui.pushButton, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
ui.moreButton->setCheckable(true);
ui.moreButton->setAutoDefault(false);
connect(ui.moreButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
ui.extension->hide();
updateGeometry();
setWindowTitle(name);
}

void GraphicFileChooser::setDir( const QString &di )
{
dir=di;
}

void GraphicFileChooser::setFilter( const QString &fil )
{
filter=fil;
}
QString GraphicFileChooser::fileName() const
{
return ui.lineEdit->text();
}

void GraphicFileChooser::chooseFile()
{
QString fn;
fn =QFileDialog::getOpenFileName(this,tr("Select a File"),dir,filter);
if ( !fn.isEmpty() )
	{
	ui.lineEdit->setText( fn );
	emit fileNameChanged( fn );
	}
}

void GraphicFileChooser::expand(bool e)
{
 ui.extension->setVisible(e);
 updateGeometry();
}

