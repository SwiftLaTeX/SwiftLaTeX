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
#include <QtGui>
#include <QFileDialog>
#include "scandialog.h"


ScanDialog::ScanDialog(QWidget *parent, const char *name)
    : QDialog( parent)
{
setWindowTitle(name);
setModal(false);
ui.setupUi(this);
scanthread=new Scanner(this);
connect(ui.pushButtonBrowse, SIGNAL(clicked()), this, SLOT(browse()));
connect(ui.pushButtonFind, SIGNAL(clicked()), this, SLOT(find()));
connect(ui.pushButtonAbort, SIGNAL(clicked()), this, SLOT(abort()));
connect(scanthread, SIGNAL(finished()), this, SLOT(showResults()));
connect(scanthread, SIGNAL(fileCountChanged(int)), this, SLOT(setFileCount(int)));
ui.pushButtonAbort->setEnabled(false);
connect(ui.treeWidget, SIGNAL(itemDoubleClicked (QTreeWidgetItem*,int)), this, SLOT(ClickedOnResult(QTreeWidgetItem*,int)));
}
ScanDialog::~ScanDialog()
{
if (scanthread) 
  {
  scanthread->stop();
  scanthread->wait();
  }
}

void ScanDialog::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,tr("Find"), QDir::currentPath());
    if (!directory.isEmpty()) ui.lineEditDir->setText(directory);
}

void ScanDialog::find()
{
ui.treeWidget->clear();
ui.labelStat->setText(QString("%1 Files").arg(0));
if ((ui.lineEditSearch->text().isEmpty()) || (ui.lineEditDir->text().isEmpty()) ) return;
ui.labelStat->setText("");
ui.pushButtonBrowse->setEnabled(false);  
ui.pushButtonFind->setEnabled(false);
ui.checkBoxSub->setEnabled(false);
ui.lineEditSearch->setEnabled(false);

scanthread->setDir(QDir(ui.lineEditDir->text()));
scanthread->setText(ui.lineEditSearch->text());
scanthread->setRecursive(ui.checkBoxSub->isChecked());
ui.pushButtonAbort->setEnabled(true);
scanthread->start();
}

void ScanDialog::abort()
{
scanthread->stop();
scanthread->wait();
ui.pushButtonBrowse->setEnabled(true);  
ui.pushButtonFind->setEnabled(true);
ui.checkBoxSub->setEnabled(true);
ui.lineEditSearch->setEnabled(true);
}

void ScanDialog::showResults()
{
QStringList fileList, numlineList, lines;
fileList=scanthread->files();
numlineList=scanthread->lines();
for (int i = 0; i < fileList.count(); ++i) 
  {
    
    QTreeWidgetItem *fileNameItem = new QTreeWidgetItem(ui.treeWidget);
    fileNameItem->setData(0,Qt::UserRole,fileList.at(i));
    fileNameItem->setText(0,QFileInfo(fileList.at(i)).fileName());
    lines=numlineList.at(i).split(",");
    for (int j = 0; j < lines.count(); ++j) 
      {
      QTreeWidgetItem *lineItem = new QTreeWidgetItem(fileNameItem);
      lineItem->setText(0,"line "+lines.at(j));
      lineItem->setData(0,Qt::UserRole,fileList.at(i));
      }
  }
ui.pushButtonBrowse->setEnabled(true);  
ui.pushButtonFind->setEnabled(true);
ui.checkBoxSub->setEnabled(true);
ui.lineEditSearch->setEnabled(true);
ui.pushButtonAbort->setEnabled(false);

}

void ScanDialog::setFileCount(int cnt)
{
  ui.labelStat->setText(QString("%1 Files").arg(cnt));
}

void ScanDialog::ClickedOnResult(QTreeWidgetItem* item,int c)
{
if (item) 
  {
  QString dest=item->data(0,Qt::UserRole).toString();
  QString s=item->text(0);
  int l=0;
  if (s.contains("line ")) l=s.remove("line ").toInt();
  emit openFileAtLine(dest,l,true);
  }
}
