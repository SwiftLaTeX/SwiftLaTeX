/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *   inspired by the ktikz (GPL) program from Glad Deschrijver             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xmltagslistwidget.h"

#include <QListWidget>
#include <QFont>
#include <QColor>
#include <QDomDocument>
#include <QFile>
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

#include "theme.h"

XmlTagsListWidget::XmlTagsListWidget(QWidget *parent, QString file):QListWidget(parent)
{
QFile tagsFile(file);
if (tagsFile.open(QFile::ReadOnly))
    {
    QDomDocument domDocument;
    if (domDocument.setContent(&tagsFile))
	{
	QDomElement root = domDocument.documentElement();
	if (root.tagName() == "texmakertags")
	    {
	    xmlSections=getTags(root);
	    for (int i = 0; i < xmlSections.children.size(); ++i)
		{
		addListWidgetItems(xmlSections.children.at(i));
		}
	    }
	}
    }
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
//viewport()->setStyleSheet(Theme::viewportDarkStyleSheet);
setStyleSheet(Theme::listwidgetStyleSheet);
}

xmlTagList XmlTagsListWidget::getTags(const QDomElement &element)
{
xmlTag item;
xmlTagList tagList;
QList<xmlTag> tags;
tagList.title = element.attribute("title");
QDomElement child = element.firstChildElement("item");
QString txt, code, type;
while (!child.isNull())
    {
    code = child.attribute("tag");
    code.replace("\\\\", "\\");
    code.replace("&lt;", "<");
    code.replace("&gt;", ">");
    item.tag=code;
    txt = child.attribute("txt");
    if (txt!="")
      {
      txt.replace("\\\\", "\\");
      txt.replace("&lt;", "<");
      txt.replace("&gt;", ">");
      }
    else txt=code;
    item.txt=txt;
    item.dx=child.attribute("dx");
    item.dy=child.attribute("dy");
    item.type=child.attribute("type").toInt();
    tags << item;
    child = child.nextSiblingElement("item");
    }
tagList.tags=tags;
QDomElement section = element.firstChildElement("section");
while (!section.isNull())
    {
    tagList.children << getTags(section);
    section = section.nextSiblingElement("section");
    }
return tagList;
}

void XmlTagsListWidget::addListWidgetItems(const xmlTagList &tagList)
{
QFont deft=QFont("DejaVu Sans Condensed",qApp->font().pointSize());
QFont titleFont = deft;//qApp->font();
titleFont.setBold(true);
QFont optionFont=deft;//qApp->font();
optionFont.setItalic(true);
QFont commandFont=deft;//qApp->font();
QListWidgetItem *item = new QListWidgetItem(this);
QString itemText = tagList.title;
item->setText(itemText.toUpper());
item->setFont(titleFont);
item->setBackgroundColor(Theme::lightbackgroundColor);
for (int i = 0; i < tagList.tags.size(); ++i)
    {
    QListWidgetItem *item = new QListWidgetItem(this);
    QString itemText = tagList.tags.at(i).txt;
    item->setText(itemText);
    QString role=tagList.tags.at(i).tag+"#"+tagList.tags.at(i).dx+"#"+tagList.tags.at(i).dy;
    item->setData(Qt::UserRole,role);
    if (tagList.tags.at(i).type==0) {item->setFont(commandFont); }
    else item->setFont(optionFont);
    }
}
