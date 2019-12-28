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


#include <QPainter>
#include <QFont>
#include <QApplication>
#include "dropshadowlabel.h"

DropShadowLabel::DropShadowLabel(const QString &text,QWidget* parent) :
	QLabel(text,parent)
{ 
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    setFont(QFont("DejaVu Sans Condensed",qApp->font().pointSize()));
}
 
void DropShadowLabel::drawText(QPainter *painter,
								 QPoint offset)
{
	Q_ASSERT(painter != 0);

	// Draw shadow.
	painter->setPen(QPen(textColor));
	painter->drawText(rect().translated(offset),
					  alignment(), text());
}
void DropShadowLabel::drawTextEffect(QPainter *painter,
								 QPoint offset)
{
	Q_ASSERT(painter != 0);
 
	// Draw shadow.
	painter->setPen(QPen(dropShadowColor));
	painter->drawText(rect().translated(offset),
					  alignment(), text());
}
 
void DropShadowLabel::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setFont(font());
#ifndef Q_OS_MAC
	drawTextEffect(&painter, QPoint(contentsMargins().left(), 1));
#endif
	drawText(&painter, QPoint(contentsMargins().left(), 0));
}

void DropShadowLabel::setColor(const QColor & color)
{
	textColor = color;
}

void DropShadowLabel::setDropShadowColor(const QColor & color)
{
	dropShadowColor = color;
}
