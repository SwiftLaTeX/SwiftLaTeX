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
 
#include "browser.h"

#include <QtGui>
#include <QDebug>
#include <QPrinter>
#include <QMenuBar>
#include <QToolBar>
#include <QPushButton>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

#include "geticon.h"
#include "theme.h"

Browser::Browser( const QString home, bool showToolBar, QWidget* parent)
    : QMainWindow( parent)
{
setWindowTitle("Texmaker");
#if defined(Q_OS_MAC)
setWindowIcon(QIcon(":/images/logo128.png"));
#else
setWindowIcon(getIcon(":/images/appicon.png"));
#endif
progress = 0;
view = new QWebEngineView(this);
if ( !home.isEmpty()) view->load(QUrl(home));
index=home;
ontop=false;

connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
//connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));


QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
fileMenu->addAction(tr("Print"), this, SLOT(Print()));
fileMenu->addSeparator();
fileMenu->addAction(tr("Exit"), this, SLOT(close()));
if (showToolBar)
    {
    QToolBar *toolBar = addToolBar("Navigation");
    toolBar->setIconSize(QSize(22,22 ));
    toolBar->setStyleSheet(Theme::viewportLightStyleSheet);
    QAction *Act;
    Act = new QAction(getIcon(":/images/home.png"), tr("Index"), this);
    connect(Act, SIGNAL(triggered()), this, SLOT(Index()));
    toolBar->addAction(Act);
    Act=view->pageAction(QWebEnginePage::Back);
    Act->setIcon(getIcon(":/images/errorprev.png"));
    toolBar->addAction(Act);
    Act=view->pageAction(QWebEnginePage::Forward);
    Act->setIcon(getIcon(":/images/errornext.png"));
    toolBar->addAction(Act);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);
    searchLineEdit = new QLineEdit(toolBar);
    searchLineEdit->setStyleSheet(Theme::lineeditStyleSheet);
    connect(searchLineEdit, SIGNAL(returnPressed()), this, SLOT(Find()));
    toolBar->addWidget(searchLineEdit);

    searchAct=new QAction(getIcon(":/images/pdffind.png"), tr("Find"), this);
    toolBar->addAction(searchAct);
    connect(searchAct, SIGNAL(triggered()), this, SLOT(Find()));
    }
setCentralWidget(view);
setUnifiedTitleAndToolBarOnMac(true);
//if ( !home.isEmpty()) view->setUrl(QUrl(home));
//resize(780,580 );
}

Browser::~Browser()
{

}

void Browser::adjustTitle()
{
if (progress <= 0 || progress >= 100)
    setWindowTitle(view->title());
else
    setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void Browser::setProgress(int p)
{
progress = p;
adjustTitle();
}

void Browser::finishLoading(bool)
{
progress = 100;
adjustTitle();
if (ontop) view->page()->runJavaScript("window.location.href='#top';");
ontop=false;
}

void Browser::Index()
{
if ((view->url().toString(QUrl::RemoveFragment)!=index) && ( !index.isEmpty()))
  {
  ontop=true;
  view->load(QUrl(index));
  }
else view->page()->runJavaScript("window.location.href='#top';");
}

void Browser::Print()
{
pdffichier=QDir::homePath();
pdffichier="texmaker_page_temp_"+pdffichier.section('/',-1);
pdffichier=QString(QUrl::toPercentEncoding(pdffichier));
pdffichier.remove("%");
pdffichier=pdffichier+".pdf";
QString tempDir=QDir::tempPath();
pdffichier=tempDir+"/"+pdffichier;
view->page()->printToPdf(pdffichier);
QFileInfo fic(pdffichier);
if (fic.exists() && fic.isReadable() ) QDesktopServices::openUrl(QUrl("file://"+pdffichier));
// QPrinter printer;
// QPrintDialog *dialog = new QPrintDialog(&printer, this);
// dialog->setWindowTitle(tr("Print"));
// if (dialog->exec() != QDialog::Accepted) return;
// view->page()->mainFrame()->print(&printer);
}

void Browser::Find()
{
if (searchLineEdit->text().isEmpty()) return;
view->findText(searchLineEdit->text());
}
