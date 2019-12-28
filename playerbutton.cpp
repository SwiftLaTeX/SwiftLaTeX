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


#include <QApplication>
#include "playerbutton.h"


PlayerButton::PlayerButton( QWidget *parent )
    : QLabel( parent )
{
state = true;
}


void PlayerButton::setImages( QString name )
{

if (qApp->devicePixelRatio()>=2)
  {
  this->pixEnabled.load(":/images/"+name+"_enabled@2x.png");
  this->pixEnabled.setDevicePixelRatio(qApp->devicePixelRatio());
  this->pixHover.load(":/images/"+name+"_enabled@2x.png");
  this->pixHover.setDevicePixelRatio(qApp->devicePixelRatio());
  this->pixDisabled.load(":/images/"+name+"_disabled@2x.png");
  this->pixDisabled.setDevicePixelRatio(qApp->devicePixelRatio());
  }
else 
  {
  this->pixEnabled.load(":/images/"+name+"_enabled.png");
  this->pixHover.load(":/images/"+name+"_enabled.png");
  this->pixDisabled.load(":/images/"+name+"_disabled.png");
  }

this->setPixmap(this->pixEnabled);
}


void PlayerButton::setEnabled( bool enabled )
{
state = enabled;
this->setPixmap( enabled ? this->pixEnabled : this->pixDisabled );
}


void PlayerButton::mouseReleaseEvent( QMouseEvent * e )
{
emit clicked();
}


void PlayerButton::enterEvent( QEvent * e )
{
//if (state) this->setPixmap( pixHover );
}


void PlayerButton::leaveEvent( QEvent *e )
{
//setEnabled( state );
}

