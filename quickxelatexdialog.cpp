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

#include "quickxelatexdialog.h"


QuickXelatexDialog::QuickXelatexDialog(QWidget *parent, const char *name)
    :QDialog( parent)
{
setWindowTitle(name);
setModal(true);
ui.setupUi(this);
connect(ui.pushButtonClass , SIGNAL(clicked()), SLOT(addUserClass()) );
ui.comboBoxSize->addItem( "10pt" );
ui.comboBoxSize->addItem( "11pt" );
ui.comboBoxSize->addItem( "12pt" );
connect(ui.pushButtonPaper , SIGNAL(clicked()), SLOT(addUserPaper()) );
connect(ui.pushButtonOptions , SIGNAL(clicked()), SLOT(addUserOptions()) );
connect(ui.pushButtonBabel , SIGNAL(clicked()), SLOT(addUserBabel()) );
ui.listWidgetOptions->setSelectionMode(QAbstractItemView::ExtendedSelection  );
ui.listWidgetBabel->setSelectionMode(QAbstractItemView::ExtendedSelection  );
setWindowTitle(tr("Quick Start"));
}

QuickXelatexDialog::~QuickXelatexDialog(){
}

void QuickXelatexDialog::Init()
{
ui.comboBoxClass->clear();
ui.comboBoxClass->addItem( "article" );
ui.comboBoxClass->addItem( "report");
ui.comboBoxClass->addItem( "letter" );
ui.comboBoxClass->addItem( "book" );
ui.comboBoxClass->addItem( "beamer" );
if (!otherClassList.isEmpty ()) ui.comboBoxClass->addItems(otherClassList);

ui.comboBoxPaper->clear();
ui.comboBoxPaper->addItem( "a4paper" );
ui.comboBoxPaper->addItem( "a5paper" );
ui.comboBoxPaper->addItem( "b5paper" );
ui.comboBoxPaper->addItem( "letterpaper" );
ui.comboBoxPaper->addItem( "legalpaper" );
ui.comboBoxPaper->addItem( "executivepaper" );
if (!otherPaperList.isEmpty ()) ui.comboBoxPaper->addItems(otherPaperList);

ui.listWidgetOptions->clear();
ui.listWidgetOptions->addItem("landscape" );
ui.listWidgetOptions->addItem("draft" );
ui.listWidgetOptions->addItem("final" );
ui.listWidgetOptions->addItem("oneside" );
ui.listWidgetOptions->addItem("twoside" );
ui.listWidgetOptions->addItem("openright" );
ui.listWidgetOptions->addItem("openany" );
ui.listWidgetOptions->addItem("onecolumn" );
ui.listWidgetOptions->addItem("twocolumn" );
ui.listWidgetOptions->addItem("titlepage" );
ui.listWidgetOptions->addItem("notitlepage" );
ui.listWidgetOptions->addItem("openbib" );
ui.listWidgetOptions->addItem("leqno" );
ui.listWidgetOptions->addItem("fleqn" );
if (!otherOptionsList.isEmpty ()) ui.listWidgetOptions->addItems(otherOptionsList);

ui.listWidgetBabel->clear();
ui.listWidgetBabel->addItem("arabic" );
ui.listWidgetBabel->addItem("czech" );
ui.listWidgetBabel->addItem("english" );
ui.listWidgetBabel->addItem("farsi" );
ui.listWidgetBabel->addItem("finnish" );
ui.listWidgetBabel->addItem("french" );
ui.listWidgetBabel->addItem("german" );
ui.listWidgetBabel->addItem("greek" );
ui.listWidgetBabel->addItem("icelandic" );
ui.listWidgetBabel->addItem("italian" );
ui.listWidgetBabel->addItem("magyar" );
ui.listWidgetBabel->addItem("polish" );
ui.listWidgetBabel->addItem("portuguese" );
ui.listWidgetBabel->addItem("russian" );
ui.listWidgetBabel->addItem("slovak" );
ui.listWidgetBabel->addItem("spanish" );
if (!otherBabelList.isEmpty ()) ui.listWidgetBabel->addItems(otherBabelList);
}

void QuickXelatexDialog::addUserClass()
{
QString newoption="";
dlg = new AddOptionDialog(this,"New");
  if ( dlg->exec() )
  {
  newoption=dlg->ui.lineEdit->text();
  if (newoption!="") otherClassList.append(newoption);
  Init();
  }
delete (dlg);
}

void QuickXelatexDialog::addUserPaper()
{
QString newoption="";
dlg = new AddOptionDialog(this,"New");
  if ( dlg->exec() )
  {
  newoption=dlg->ui.lineEdit->text();
  if (newoption!="") otherPaperList.append(newoption);
  Init();
  }
delete (dlg);
}

void QuickXelatexDialog::addUserOptions()
{
QString newoption="";
dlg = new AddOptionDialog(this,"New");
  if ( dlg->exec() )
  {
  newoption=dlg->ui.lineEdit->text();
  if (newoption!="") otherOptionsList.append(newoption);
  Init();
  }
delete (dlg);
}

void QuickXelatexDialog::addUserBabel()
{
QString newoption="";
dlg = new AddOptionDialog(this,"New");
  if ( dlg->exec() )
  {
  newoption=dlg->ui.lineEdit->text();
  if (newoption!="") otherBabelList.append(newoption);
  Init();
  }
delete (dlg);
}
