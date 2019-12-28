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

#ifndef USERTAGSLISTWIDGET_H
#define USERTAGSLISTWIDGET_H

#include <QListWidget>
#include <QMenu>
#include <QAction>


class UserTagsListWidget : public QListWidget  {
  Q_OBJECT
public:
	UserTagsListWidget(QWidget *parent);
	QAction *addAct, *remAct, *changeAct;
void updateList(QStringList ulist);
private slots:
void customContentsMenu( const QPoint &pos );
private:
QMenu *menu;
protected:
    void dropEvent(QDropEvent *event);
signals:
  void posChanged();
};

#endif
