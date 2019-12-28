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

#include <QDebug>

#include "usercompletiondialog.h"

UserCompletionDialog::UserCompletionDialog(QWidget *parent,QStringList extraCompletion)
    :QDialog( parent)
{
ui.setupUi(this);


ui.listWidget->clear();
ui.listWidget->setAlternatingRowColors(true);
userlist=extraCompletion;
ui.listWidget->addItems(userlist);
connect(ui.listWidget, SIGNAL(itemActivated ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
connect(ui.listWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
connect(ui.addButton, SIGNAL( clicked() ), this, SLOT( actionAddCommand() ) );
connect(ui.deleteButton, SIGNAL( clicked() ), this, SLOT( actionDelete() ) );
connect(ui.replaceButton, SIGNAL( clicked() ), this, SLOT( actionReplace() ) );
}

UserCompletionDialog::~UserCompletionDialog(){
}

void UserCompletionDialog::slotItemClicked(QListWidgetItem* item)
{
int current=-1;
QList<QListWidgetItem *> items;
items = ui.listWidget->selectedItems();
if (items.count() > 0) 
	{
	ui.listWidget->setCurrentItem(items[0]);
	current=ui.listWidget->row(items[0]);
	}
qDebug() << current;
if (current>=0) 
	{
	ui.lineEdit->setText(ui.listWidget->currentItem()->text());
	}
}


void UserCompletionDialog::actionDelete()
{
int current=ui.listWidget->currentRow();
if (current<0) return;
delete ui.listWidget->currentItem();
userlist.removeAt(current);
if (current==ui.listWidget->count())
	{
	ui.listWidget->setCurrentItem(ui.listWidget->item(current-1));
	ui.listWidget->setItemSelected(ui.listWidget->item(current-1),true);
	}
else
	{
	ui.listWidget->setCurrentItem(ui.listWidget->item(current));
	ui.listWidget->setItemSelected(ui.listWidget->item(current),true);
	}
ui.lineEdit->clear();
}

void UserCompletionDialog::actionReplace()
{
int current=ui.listWidget->currentRow();
if (current<0) return;
QString newcommand=ui.lineEdit->text();
newcommand.replace("@",QString(0x2022));
userlist.replace(current,newcommand);
ui.listWidget->clear();
ui.listWidget->addItems(userlist);
if (ui.listWidget->count()>0)
	{
	ui.listWidget->setCurrentItem(ui.listWidget->item(current));
	ui.listWidget->setItemSelected(ui.listWidget->item(current),true);
	}
ui.lineEdit->clear();
}

void UserCompletionDialog::actionAddCommand()
{
QString newcommand=ui.lineEdit->text();
newcommand.replace("@",QString(0x2022));
ui.listWidget->addItem(newcommand);
userlist.append(newcommand);
if (ui.listWidget->count()>0)
	{
	ui.listWidget->setCurrentItem(ui.listWidget->item(ui.listWidget->count()-1));
	ui.listWidget->setItemSelected(ui.listWidget->currentItem(), true);
	}
}




