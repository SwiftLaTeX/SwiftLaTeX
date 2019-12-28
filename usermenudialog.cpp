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
 
#include "usermenudialog.h"


UserMenuDialog::UserMenuDialog( QWidget* parent,  QString name)
    : QDialog( parent)
{
setWindowTitle(name);
setModal(true);
ui.setupUi(this);

previous_index=0;

connect(ui.listWidget, SIGNAL(currentRowChanged(int)),this,SLOT(change(int)));
connect( ui.buttonBox, SIGNAL(accepted()), SLOT(slotOk()) );
}

UserMenuDialog::~UserMenuDialog()
{
}

void UserMenuDialog::init()
{
ui.listWidget->insertItem(0, "Menu 1 : "+Name[0] );
ui.listWidget->insertItem(1, "Menu 2 : "+Name[1] );
ui.listWidget->insertItem(2, "Menu 3 : "+Name[2] );
ui.listWidget->insertItem(3, "Menu 4 : "+Name[3] );
ui.listWidget->insertItem(4, "Menu 5 : "+Name[4] );
ui.listWidget->insertItem(5, "Menu 6 : "+Name[5] );
ui.listWidget->insertItem(6, "Menu 7 : "+Name[6] );
ui.listWidget->insertItem(7, "Menu 8 : "+Name[7] );
ui.listWidget->insertItem(8, "Menu 9 : "+Name[8] );
ui.listWidget->insertItem(9, "Menu 10 : "+Name[9] );
ui.tagEdit->setPlainText(Tag[0]);
ui.itemEdit->setText(Name[0]);
ui.listWidget->setCurrentRow(0); 
connect(ui.itemEdit, SIGNAL(textChanged(const QString &)),this,SLOT(updateItem()));
}

void UserMenuDialog::change(int index)
{
Tag[previous_index]=ui.tagEdit->toPlainText();
Name[previous_index]=ui.itemEdit->text();
ui.tagEdit->setPlainText(Tag[index]);
ui.itemEdit->setText(Name[index]);
previous_index=index;
}

void UserMenuDialog::slotOk()
{
Tag[previous_index]=ui.tagEdit->toPlainText();
Name[previous_index]=ui.itemEdit->text();
accept();
}

void UserMenuDialog::updateItem()
{
int i=ui.listWidget->currentRow();
ui.listWidget->item(i)->setText("Menu "+QString::number(i+1)+" : "+ui.itemEdit->text());
}
