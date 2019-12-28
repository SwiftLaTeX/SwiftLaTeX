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

#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QThread>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

#include "ui_scandialog.h"

class Scanner : public QThread
{
  Q_OBJECT
  
public:
  explicit Scanner(QObject *parent = 0)
    : QThread(parent), stopped(false)
  {
  }

  void run()
  {
    fileList.clear();
    numlineList.clear();
    QStringList extensions;
    extensions << "tex" << "bib" << "sty" << "cls" << "mp" << "Rnw" << "asy";
    QStringList listoflines;
    QDirIterator iterator(scanDir,isRecursive? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    while(!stopped && iterator.hasNext()) 
    {
      QString entry(iterator.next());
      if(extensions.contains(QFileInfo(entry).suffix().toLower())) 
      {
	
	QFile file(entry);
	listoflines.clear();
	
        if (file.open(QIODevice::ReadOnly)) 
	{
            QString line;
	   
            QTextStream in(&file);
	    int l=0;
            while (!in.atEnd()) 
	    {
                if (stopped) break;
                line = in.readLine();
		l++;
                if (line.contains(text)) listoflines << QString::number(l);
            }
        }
	
	if (listoflines.count()>0) 
	{
	fileList << entry;
	numlineList << listoflines.join(",");
	emit fileCountChanged(fileList.count());
	}
      }
    }
  quit(); 
  }
  
  void setRecursive(bool rec)
  {
  isRecursive=rec;  
  }
  
  void setDir(const QDir &dir)
  {
  scanDir=dir;  
  }

  void setText(const QString &t)
  {
  text=t;  
  }

  QStringList files() const
  {
    return fileList;
  }
  
  QStringList lines() const
  {
    return numlineList;
  }

public slots:
  void stop()
  {
    stopped = true;
  }

signals:
  void fileCountChanged(int);

private:
  QStringList fileList, numlineList;
  bool stopped;
  bool isRecursive;
  QDir scanDir;
  QString text;
};

class ScanDialog : public QDialog
{
    Q_OBJECT
public: 
	ScanDialog(QWidget *parent = 0, const char *name = 0);
	~ScanDialog();
	Ui::ScanDialog ui;
private:
Scanner* scanthread;
private slots:
    void browse();
    void find();
    void abort();
    void showResults();
    void setFileCount(int cnt);
    void ClickedOnResult(QTreeWidgetItem* item,int c);
signals:
  void openFileAtLine(QString,int,bool);
};

#endif
