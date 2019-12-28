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
#include <QLabel>

#ifndef DROPSHADOWLABEL_H
#define DROPSHADOWLABEL_H

class DropShadowLabel : public QLabel
{
	Q_OBJECT
 
public:
 
	DropShadowLabel(const QString &text,QWidget* parent = 0);
	void paintEvent(QPaintEvent *event);
	void setColor(const QColor & color);
	void setDropShadowColor(const QColor & color);
private:
 
	QColor dropShadowColor;
	QColor textColor;
	void drawText(QPainter *painter, QPoint offset);
	void drawTextEffect(QPainter* painter, QPoint offset);
};
#endif 
