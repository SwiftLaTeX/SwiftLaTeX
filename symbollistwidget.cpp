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

#include "symbollistwidget.h"
#include "icondelegate.h" 
#include <QFont>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QApplication>
#include <QDebug>

#include "theme.h"

SymbolListWidget :: SymbolListWidget(QWidget *parent, int page) : QTableWidget(parent)
{
hide();
setAutoFillBackground( true );
QPalette pal( palette() );
pal.setColor( QPalette::Active, QPalette::HighlightedText, Theme::darkbackgroundColor );
pal.setColor( QPalette::Inactive, QPalette::HighlightedText, Theme::darkbackgroundColor );
pal.setColor( QPalette::Disabled, QPalette::HighlightedText, Theme::darkbackgroundColor );
pal.setColor( QPalette::Active, QPalette::Base, Theme::darkbackgroundColor );
pal.setColor( QPalette::Inactive, QPalette::Base, Theme::darkbackgroundColor );
pal.setColor( QPalette::Disabled, QPalette::Base, Theme::darkbackgroundColor );
pal.setColor(QPalette::Background, Qt::black);
setPalette( pal );
setItemDelegate(new IconDelegate(this));
QString icon_name;
setShowGrid(true);
verticalHeader()->hide();
horizontalHeader()->hide();

		  if (qApp->devicePixelRatio()>=2)
		  {
		  setIconSize ( QSize(64,64 ));
		  }
else setIconSize ( QSize(32,32 ));

//setIconSize ( QSize(32,32 ));
setSelectionMode (QAbstractItemView::SingleSelection);
setContextMenuPolicy(Qt::CustomContextMenu);
menu = new QMenu( this );
addAct=new QAction(tr("Add to favorites"), this);
remAct=new QAction(tr("Remove from favorites"), this);
connect( this, SIGNAL( customContextMenuRequested( const QPoint & )), this, SLOT( customContentsMenu( const QPoint & )));


switch (page)
{
	case 0:
	{
setUpdatesEnabled(false);
		setColumnCount(4);
		setRowCount(57);
		for ( uint j = 0; j < 57; ++j ) setRowHeight(j,36);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
		for ( uint i = 0; i <= 225; ++i )
		{
		icon_name=":/symbols/inverted_img"+QString::number(i+1)+".png";
        QTableWidgetItem* item= new QTableWidgetItem();
        item->setIcon(QIcon(icon_name));
		item->setText(code[i]+";"+QString::number(i));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setToolTip(code[i]);
		setItem(i/4,i%4,item);
		}
setUpdatesEnabled(true);
		menu->addAction(addAct);
	}
	break;
	case 1:
	{
		setColumnCount(4);
		setRowCount(17);
		for ( uint j = 0; j < 17; ++j ) setRowHeight(j,36);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
		for ( uint i = 247; i <= 313; ++i )
		{
		icon_name=":/symbols/inverted_img"+QString::number(i+1)+".png";
		QTableWidgetItem* item= new QTableWidgetItem();
		item->setIcon(QIcon(icon_name));
		item->setText(code[i]+";"+QString::number(i));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setToolTip(code[i]);
		setItem((i-247)/4,(i-247)%4,item);
		}
		menu->addAction(addAct);
	}
	break;
	case 2:
	{
		setColumnCount(4);
		setRowCount(15);
		for ( uint j = 0; j < 15; ++j ) setRowHeight(j,36);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
		for ( uint i = 314; i <= 371; ++i )
		{
		icon_name=":/symbols/inverted_img"+QString::number(i+1)+".png";
		QTableWidgetItem* item= new QTableWidgetItem();
		item->setIcon(QIcon(icon_name));
		item->setText(code[i]+";"+QString::number(i));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setToolTip(code[i]);
		setItem((i-314)/4,(i-314)%4,item);
		}
		menu->addAction(addAct);
	}
	break;
	case 3:
	{
		setColumnCount(4);
		setRowCount(6);
		for ( uint j = 0; j < 6; ++j ) setRowHeight(j,36);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
		for ( uint i = 226; i <= 246; ++i )
		{
		icon_name=":/symbols/inverted_img"+QString::number(i+1)+".png";
		QTableWidgetItem* item= new QTableWidgetItem();
		item->setIcon(QIcon(icon_name));
		item->setText(code[i]+";"+QString::number(i));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setToolTip(code[i]);
		setItem((i-226)/4,(i-226)%4,item);
		}
		menu->addAction(addAct);
	}
	break;
	case 4:
	{
		setColumnCount(4);
		setRowCount(10);
		for ( uint j = 0; j < 10; ++j ) setRowHeight(j,36);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
		for ( uint i = 0; i <= 39; ++i )
		{
		icon_name=":/symbols/inverted_img"+QString::number(i+1)+"greek.png";
		QTableWidgetItem* item= new QTableWidgetItem();
		item->setIcon(QIcon(icon_name));
		item->setText(code[i+372]+";"+QString::number(i+372));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setToolTip(code[i+372]);
		setItem(i/4,i%4,item);
		}
		menu->addAction(addAct);
	}
	break;
	case 5:
	{
		setColumnCount(4);
		setRowCount(3);
		for ( uint j = 0; j < 3; ++j ) setRowHeight(j,36);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
	}
	break;
	case 6:
	{
		setColumnCount(4);
		setColumnWidth(0,36);
		setColumnWidth(1,36);
		setColumnWidth(2,36);
		setColumnWidth(3,36);
		menu->addAction(remAct);
	}
	break;
show();
}

}
void SymbolListWidget::SetUserPage(usercodelist ulist)
{
QString icon_name;
for ( uint i = 0; i <=11; ++i )
	{
	if ((ulist[i]>-1) && (ulist[i]<412))
		{
		QTableWidgetItem* item= new QTableWidgetItem();
		if (ulist[i]>=372)
			{
			icon_name=":/symbols/inverted_img"+QString::number(ulist[i]-371)+"greek.png";
			}
		else
			{
			icon_name=":/symbols/inverted_img"+QString::number(ulist[i]+1)+".png";
			}
	item->setText(code[ulist[i]]+";"+QString::number(ulist[i]));
        item->setIcon(QIcon(icon_name));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setToolTip(code[ulist[i]]);
		setItem(i/4,i%4,item);
		}
	}
}

void SymbolListWidget::SetFavoritePage(QList<int> flist)
{
clearContents();
int rows=(flist.count()-1)/4+1;
setRowCount(rows);
for ( uint j = 0; j < rows; ++j ) setRowHeight(j,36);
QString icon_name;
for( int i = 0; i < flist.count(); i++ )
	{
	if ((flist.at(i)>-1) && (flist.at(i)<412))
		{
		QTableWidgetItem* item= new QTableWidgetItem();
		if (flist.at(i)>=372)
			{
			icon_name=":/symbols/inverted_img"+QString::number(flist.at(i)-371)+"greek.png";
			}
		else
			{
			icon_name=":/symbols/inverted_img"+QString::number(flist.at(i)+1)+".png";
			}
		item->setText(code[flist.at(i)]+";"+QString::number(flist.at(i)));
		item->setIcon(QIcon(icon_name));
		item->setToolTip(code[flist.at(i)]);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		setItem(i/4,i%4,item);
		}
	}
}

void SymbolListWidget::customContentsMenu( const QPoint &pos )
{
QTableWidgetItem* item= new QTableWidgetItem();
item=this->itemAt(pos);
if (item) 
	{
	addAct->setData(item->text());
	remAct->setData(item->text());
	}
QPoint globalPos = this->mapToGlobal(pos);
menu->exec( globalPos );
}


