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

#include "usertagslistwidget.h"

#include <QListWidget>
#include <QFont>
#include <QApplication>
#include <QColor>
#include <QFontDatabase>
#include <QDebug>
#include "theme.h"

UserTagsListWidget::UserTagsListWidget(QWidget *parent):QListWidget(parent)
{
setDragDropMode(QAbstractItemView::InternalMove);
setContextMenuPolicy(Qt::CustomContextMenu);
viewport()->setStyleSheet(Theme::viewportDarkStyleSheet);
setStyleSheet(Theme::listwidgetStyleSheet);
menu = new QMenu( this );
addAct=new QAction(tr("Add tag"), this);
menu->addAction(addAct);
remAct=new QAction(tr("Remove"), this);
menu->addAction(remAct);
changeAct=new QAction(tr("Modify"), this);
menu->addAction(changeAct);
connect( this, SIGNAL( customContextMenuRequested( const QPoint & )), this, SLOT( customContentsMenu( const QPoint & )));
}

void UserTagsListWidget::updateList(QStringList ulist)
{
QFont commandFont=QFont("DejaVu Sans Condensed",qApp->font().pointSize());
clear();
QStringList tagList;
for (int i = 0; i < ulist.count(); ++i)
    {
    QListWidgetItem *item = new QListWidgetItem(this);
    tagList.clear();
    tagList= ulist.at(i).split("#");
    if (tagList.count()==3)
      {
      item->setText(tagList.at(0));
      item->setData(Qt::UserRole,tagList.at(1)+"#"+tagList.at(2));
      item->setFont(commandFont);
      item->setToolTip(tagList.at(1)+"\n[:"+tagList.at(2)+"]");
      }
    }
  
}

void UserTagsListWidget::customContentsMenu( const QPoint &pos )
{
QListWidgetItem* item= new QListWidgetItem();
item=this->itemAt(pos);
if (item) 
	{
	//addAct->setData(item->text());
	remAct->setData(item->text()+"#"+item->data(Qt::UserRole).toString());
	remAct->setEnabled(true);
	changeAct->setData(item->text()+"#"+item->data(Qt::UserRole).toString());
	changeAct->setEnabled(true);
	}
else
	{
	remAct->setEnabled(false);
	changeAct->setEnabled(false);
	}
QPoint globalPos = this->mapToGlobal(pos);
menu->exec( globalPos );
}

void UserTagsListWidget::dropEvent(QDropEvent *event)
{
    QListWidget::dropEvent(event);
    emit posChanged();
}

