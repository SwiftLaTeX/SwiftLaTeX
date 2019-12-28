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

#include "unicodedialog.h"
#include <QTextCodec>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>


UnicodeDialog::UnicodeDialog(QWidget *parent)
    :QDialog( parent)
{
setModal(true);
ui.setupUi(this);
foreach (int mib, QTextCodec::availableMibs()) 
	{
	QTextCodec *codec = QTextCodec::codecForMib(mib);
	if (codec->name()!="UTF-8") ui.comboBoxEncoding->addItem(codec->name());
	}
ui.comboBoxEncoding->setCurrentIndex(ui.comboBoxEncoding->findText("ISO-8859-1", Qt::MatchExactly));
connect(ui.comboBoxEncoding, SIGNAL(currentIndexChanged( const QString & ) ), this, SLOT( resetEditor() )); 
connect(ui.lineEdit, SIGNAL(textChanged( const QString & ) ), this, SLOT( resetEditor() )); 
connect(ui.pushButton, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
connect(ui.pushButtonConvert, SIGNAL( clicked() ), this, SLOT( convertFile() ) );
mainlay = new QVBoxLayout(ui.frameEditor );
mainlay->setSpacing(0);
mainlay->setMargin(0);
}

UnicodeDialog::~UnicodeDialog(){
}

void UnicodeDialog::init(QFont & efont,bool line, QList<QColor> edcolors, QList<QColor> hicolors)
{
unicodeviewWidget=new UnicodeView(ui.frameEditor,efont,line,edcolors,hicolors);  
mainlay->addWidget(unicodeviewWidget);
}

void UnicodeDialog::chooseFile()
{
QString fn;
fn =QFileDialog::getOpenFileName(this,tr("Select a File"),QDir::homePath(),"TeX files (*.tex *.bib *.sty *.cls *.mp *.Rnw *.asy);;All files (*.*)");
if ( !fn.isEmpty() )
	{
	ui.lineEdit->setText( fn );
	}
}

void UnicodeDialog::convertFile()
{
QString f=ui.lineEdit->text();
if (f.isEmpty() || !QFile::exists( f )) return;
QFile file( f );
if ( !file.open( QIODevice::ReadOnly ) )
	{
	QMessageBox::warning( this,tr("Error"), tr("You do not have read permission to this file."));
	return;
	}
QTextCodec* codec = QTextCodec::codecForName(ui.comboBoxEncoding->currentText().toLatin1());
//if(!codec) codec = QTextCodec::codecForLocale();
unicodeviewWidget->editor->setPlainText(codec->toUnicode(file.readAll()));

}

void UnicodeDialog::resetEditor()
{
unicodeviewWidget->editor->clear();
}
