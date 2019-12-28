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

#include <QHeaderView>
#include <QScrollBar>
#include <QDebug>

#include "userquickdialog.h"

UserQuickDialog::UserQuickDialog(QWidget *parent,QStringList usualNames, QStringList usualCommands)
    :QDialog( parent)
{
ui.setupUi(this);

connect(ui.addButton, SIGNAL( clicked() ), this, SLOT( actionAddCommand() ) );
connect(ui.upButton, SIGNAL( clicked() ), this, SLOT( actionUp() ) );
connect(ui.downButton, SIGNAL( clicked() ), this, SLOT( actionDown() ) );
connect(ui.deleteButton, SIGNAL( clicked() ), this, SLOT( actionDelete() ) );

ui.listWidget->clear();
ui.listWidget->setAlternatingRowColors(true);
userlist.clear();

userQuickCommand="";
//ui.commandWidget->setSelectionMode (QAbstractItemView::ExtendedSelection);
ui.commandWidget->setViewMode(QListView::ListMode);
ui.commandWidget->addItems(usualNames);

for(int i=0; i<usualNames.count(); i++ ) cmdmap.insert(usualNames.at(i),usualCommands.at(i));
}

UserQuickDialog::~UserQuickDialog(){
}





void UserQuickDialog::addItem(QString cmd)
{
ui.listWidget->addItem(cmd);
int mediacounter=ui.listWidget->count()-1;
userlist.append(cmd);
if (ui.listWidget->count()>0)
	{
	ui.listWidget->setCurrentItem(ui.listWidget->item(ui.listWidget->count()-1));
	ui.listWidget->setItemSelected(ui.listWidget->currentItem(), true);
	}
}

void UserQuickDialog::actionUp()
{
int current=ui.listWidget->currentRow();
if (current<=0) return;
QListWidgetItem *item=ui.listWidget->item(current)->clone();
delete ui.listWidget->item(current);
ui.listWidget->insertItem(current-1,item);
userlist.swap(current,current-1);
ui.listWidget->setCurrentItem(ui.listWidget->item(current-1));
ui.listWidget->setItemSelected(ui.listWidget->item(current-1),true);
updateQuickCommand();
}

void UserQuickDialog::actionDown()
{
int current=ui.listWidget->currentRow();
if (current>=ui.listWidget->count()-1 || current<0) return;
QListWidgetItem *item=ui.listWidget->item(current)->clone();
delete ui.listWidget->item(current);
ui.listWidget->insertItem(current+1,item);
userlist.swap(current,current+1);
ui.listWidget->setCurrentItem(ui.listWidget->item(current+1));
ui.listWidget->setItemSelected(ui.listWidget->item(current+1),true);
updateQuickCommand();
}

void UserQuickDialog::actionDelete()
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
updateQuickCommand();
}

void UserQuickDialog::actionAddCommand()
{
QString fn;
QList<QListWidgetItem *> items;
items = ui.commandWidget->selectedItems();
for (int i = 0; i < items.count(); ++i) 
	{
	fn=items[i]->text();
	addItem(fn);
	}
updateQuickCommand();
}

void UserQuickDialog::updateQuickCommand()
{
userQuickCommand="";
if (userlist.count()>0)
    {
    for(int i=0; i<userlist.count(); i++ )
      {
      userQuickCommand+=cmdmap[userlist.at(i)]+"|";
      }
    userQuickCommand.chop(1);
    }
}


