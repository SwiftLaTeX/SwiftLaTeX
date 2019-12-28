/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation  either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include "geticon.h"
QIcon getIcon(QString name)
{
QIcon dIcon=QIcon();
QString base=name.remove(".png");

if (qApp->devicePixelRatio()>=2)
{
QPixmap normalPixmap(base+"@2x.png");
normalPixmap.setDevicePixelRatio(qApp->devicePixelRatio());
QPixmap disabledPixmap(normalPixmap.size());
disabledPixmap.setDevicePixelRatio(qApp->devicePixelRatio());
disabledPixmap.fill(Qt::transparent);
QPainter p(&disabledPixmap);
p.setBackgroundMode(Qt::TransparentMode);
p.setBackground(QBrush(Qt::transparent));
p.eraseRect(normalPixmap.rect());
p.setOpacity(0.5);
p.drawPixmap(0, 0, normalPixmap);
p.end();
dIcon.addPixmap( normalPixmap, QIcon::Active);
dIcon.addPixmap( disabledPixmap, QIcon::Disabled  );
}
else
{
QPixmap normalPixmap(base+".png");
QPixmap disabledPixmap(normalPixmap.size());
disabledPixmap.fill(Qt::transparent);
QPainter p(&disabledPixmap);
p.setBackgroundMode(Qt::TransparentMode);
p.setBackground(QBrush(Qt::transparent));
p.eraseRect(normalPixmap.rect());
p.setOpacity(0.5);
p.drawPixmap(0, 0, normalPixmap);
p.end();
dIcon.addPixmap( normalPixmap, QIcon::Active);
dIcon.addPixmap( disabledPixmap, QIcon::Disabled  );
}

return dIcon;
}


