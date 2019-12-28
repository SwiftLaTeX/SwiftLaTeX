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

#include "usertooldialog.h"

UserToolDialog::UserToolDialog(QWidget *parent, QString name,QStringList names, QStringList commands ) : QDialog(parent)
{
setWindowTitle(name);
setModal(true);
ui.setupUi(this);
usualNames=names;
usualCommands=commands;
previous_index=0;


connect(ui.listWidget, SIGNAL(currentRowChanged(int)),this,SLOT(change(int)));
connect( ui.pushButtonWizard, SIGNAL(clicked()), this, SLOT(userQuickWizard()));

connect( ui.buttonBox, SIGNAL(accepted()), SLOT(slotOk()) );
}

UserToolDialog::~UserToolDialog()
{
}

void UserToolDialog::init()
{
ui.listWidget->insertItem(0, "Command 1 : "+Name[0] );
ui.listWidget->insertItem(1, "Command 2 : "+Name[1] );
ui.listWidget->insertItem(2, "Command 3 : "+Name[2] );
ui.listWidget->insertItem(3, "Command 4 : "+Name[3] );
ui.listWidget->insertItem(4, "Command 5 : "+Name[4] );
ui.toolEdit->setText(Tool[0]);
ui.itemEdit->setText(Name[0]);
ui.listWidget->setCurrentRow(0);
connect(ui.itemEdit, SIGNAL(textChanged(const QString &)),this,SLOT(updateItem()));
}

void UserToolDialog::change(int index)
{
Tool[previous_index]=ui.toolEdit->text();
Name[previous_index]=ui.itemEdit->text();
ui.toolEdit->setText(Tool[index]);
ui.itemEdit->setText(Name[index]);
previous_index=index;
}

void UserToolDialog::slotOk()
{
Tool[previous_index]=ui.toolEdit->text();
Name[previous_index]=ui.itemEdit->text();
accept();
}

void UserToolDialog::updateItem()
{
int i=ui.listWidget->currentRow();
ui.listWidget->item(i)->setText("Command "+QString::number(i+1)+" : "+ui.itemEdit->text());
}

void UserToolDialog::userQuickWizard()
{
userquickdlg= new UserQuickDialog(this,usualNames,usualCommands);
if ( userquickdlg->exec() )
   {
   ui.toolEdit->setText(userquickdlg->userQuickCommand);
   }
delete (userquickdlg);
}
