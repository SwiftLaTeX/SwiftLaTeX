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

#include "exportdialog.h"


#include <QTextCodec>
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

#include <QProcessEnvironment>


ExportDialog::ExportDialog(QWidget* parent,QString f, QString ep): QDialog( parent)
{
setModal(true);
ui.setupUi(this);
filename=f;
extra_path=ep;
connect(ui.radioButton5, SIGNAL(toggled(bool)),ui.lineEditOptions, SLOT(setEnabled(bool)));
connect( ui.pushButtonPath, SIGNAL(clicked()), this, SLOT(browseHtlatex()));
connect(ui.pushButtonRun, SIGNAL(clicked()), this, SLOT(runHtlatex()));
}

ExportDialog::~ExportDialog(){
if (proc && proc->state()==QProcess::Running) 
	{
	proc->disconnect();
	proc->kill(); 
	delete proc ;
	}
}


void ExportDialog::browseHtlatex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\"";
	ui.lineEditPath->setText( location );
	}
}

void ExportDialog::runHtlatex()
{
QFileInfo fi(filename);
QString commandline="\""+ui.lineEditPath->text()+"\" \""+filename+"\" ";
if (ui.radioButton1->isChecked())
  {
  commandline+="\" \" \" \" \" \" -interaction=nonstopmode";
  }
else if (ui.radioButton2->isChecked())
  {
  commandline+="\"html,2,sections+\" \" \" \" \" -interaction=nonstopmode";
  }
else if (ui.radioButton3->isChecked())
  {
  commandline+="\"xhtml,ooffice\" \"ooffice/! -cmozhtf\" \"-coo\" -interaction=nonstopmode";
  }
else if (ui.radioButton4->isChecked())
  {
  commandline+="\"xhtml,mozilla\" \" -cmozhtf\" \"-cvalidate\" -interaction=nonstopmode";
  }
else if (ui.radioButton4->isChecked())
  {
  commandline+=ui.lineEditOptions->text();
  }
proc = new QProcess( this );
proc->setWorkingDirectory(fi.absolutePath());
proc->setProperty("command",commandline);
connect(proc, SIGNAL(readyReadStandardError()),this, SLOT(readFromStderr()));
//connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(readFromStdoutput()));
connect(proc, SIGNAL(finished(int)),this, SLOT(SlotEndProcess(int)));
ui.plainTextEditResult->clear();

#if defined(Q_OS_MAC)
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
if (extra_path.isEmpty()) env.insert("PATH", env.value("PATH") + ":/Library/TeX/texbin:/Library/TeX/Distributions/.DefaultTeX/Contents/Programs/texbin:/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin:");
else
 env.insert("PATH", env.value("PATH") + ":/Library/TeX/texbin:/Library/TeX/Distributions/.DefaultTeX/Contents/Programs/texbin:/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin:"+extra_path);
proc->setProcessEnvironment(env);
#endif
#if defined(Q_OS_WIN32)
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
// if (extra_path.isEmpty()) env.insert("PATH", env.value("PATH") + ";c:/texlive/2018/bin;c:/texlive/2017/bin;c:/texlive/2016/bin;c:/texlive/2015/bin;c:/texlive/2014/bin;c:/texlive/2013/bin;c:/texlive/2012/bin;c:/texlive/2011/bin;c:/texlive/2010/bin;c:/w32tex/bin;c:/Program Files/MiKTeX 3.0/miktex/bin;c:/Program Files (x86)/MiKTeX 3.0/miktex/bin;c:/Program Files/MiKTeX 2.9/miktex/bin;c:/Program Files (x86)/MiKTeX 2.9/miktex/bin;c:/Program Files/MiKTeX 2.8/miktex/bin;c:/Program Files (x86)/MiKTeX 2.8/miktex/bin");
// else
//  env.insert("PATH", env.value("PATH") + ";c:/texlive/2018/bin;c:/texlive/2017/bin;c:/texlive/2016/bin;c:/texlive/2015/bin;c:/texlive/2014/bin;c:/texlive/2013/bin;c:/texlive/2012/bin;c:/texlive/2011/bin;c:/texlive/2010/bin;c:/w32tex/bin;c:/Program Files/MiKTeX 3.0/miktex/bin;c:/Program Files (x86)/MiKTeX 3.0/miktex/bin;c:/Program Files/MiKTeX 2.9/miktex/bin;c:/Program Files (x86)/MiKTeX 2.9/miktex/bin;c:/Program Files/MiKTeX 2.8/miktex/bin;c:/Program Files (x86)/MiKTeX 2.8/miktex/bin;"+extra_path);
if (!extra_path.isEmpty()) 
  {
  env.insert("PATH", env.value("PATH") + ";"+extra_path);
  proc->setProcessEnvironment(env);
  }
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
if (!extra_path.isEmpty()) 
  {
  env.insert("PATH", env.value("PATH") + ":"+extra_path);
  proc->setProcessEnvironment(env);
  }
#endif
qputenv("PATH", env.value("PATH").toLatin1());
ui.plainTextEditResult->insertPlainText(commandline+"\n");
proc->start(commandline);
}

void ExportDialog::readFromStderr()
{
QByteArray result=proc->readAllStandardError();
QString t=QString(result);
t=t.simplified();
if (!t.isEmpty()) ui.plainTextEditResult->insertPlainText(t+"\n");
}

void ExportDialog::readFromStdoutput()
{
QByteArray result=proc->readAllStandardOutput ();
QString t=QString(result);
t=t.simplified();
if (!t.isEmpty()) ui.plainTextEditResult->insertPlainText(t+"\n");
}

void ExportDialog::SlotEndProcess(int err)
{
QString result=((err) ? "Process exited with error(s)" : "Process exited normally");
ui.plainTextEditResult->insertPlainText(result);

QStringList extension=QString(".4,.4ct,.4tc,.aux,.dvi,.idv,.lg,.log,.tmp,.xref").split(",");

if (!err)
  {
  QFileInfo fi(filename);
  QDirIterator iterator(QDir(fi.absolutePath()),QDirIterator::NoIteratorFlags);
  while(iterator.hasNext())
    {
    QString entry(iterator.next());
    if(!QFileInfo(entry).isDir())
      {
      if(extension.contains("."+QFileInfo(entry).completeSuffix().toLower()) && QFileInfo(entry).baseName()==fi.baseName())
	{
	if (QFile::exists(entry))
		{
		QFile file(entry);
		file.open( QIODevice::ReadOnly );
		file.remove();
		}
	}
      }
    }
  }
}

