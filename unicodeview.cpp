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

#include "unicodeview.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QTextDocument>
#include <QTextCursor>
#include <QFileDialog>
#include <QTextBlock>
#include <QLineEdit>
#include <QTextCodec>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

#include "geticon.h"

UnicodeView::UnicodeView(QWidget *parent,QFont & efont,bool line, QList<QColor> edcolors, QList<QColor> hicolors ) : QWidget(parent)
{
  
splitter=new MiniSplitter(this);
splitter->setOrientation(Qt::Vertical);

QFrame *framebis=new QFrame(splitter);
// framebis->setLineWidth(0);
// framebis->setFrameShape(QFrame::NoFrame);
// framebis->setFrameShadow(QFrame::Plain);
// framebis->setFrameStyle(QFrame::NoFrame);


QVBoxLayout* mainlay = new QVBoxLayout(framebis );
mainlay->setSpacing(0);
mainlay->setMargin(0);

centralToolBar=new QToolBar("ToolBar",framebis);
centralToolBar->setFloatable(false);
centralToolBar->setOrientation(Qt::Horizontal);
centralToolBar->setMovable(false);
centralToolBar->setIconSize(QSize(16,16 ));
centralToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
mainlay->addWidget(centralToolBar);
QAction *Act = new QAction(getIcon(":/images/filesave.png"),tr("Save"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileSave()));
centralToolBar->addAction(Act);
centralToolBar->addSeparator();
titleLabel=new QLabel("Unicode UTF-8",centralToolBar);
centralToolBar->addWidget(titleLabel);


QFrame *frame=new QFrame(framebis);
// frame->setLineWidth(0);
// frame->setFrameShape(QFrame::NoFrame);
// frame->setFrameShadow(QFrame::Plain);
// frame->setFrameStyle(QFrame::NoFrame);
mainlay->addWidget(frame);

lastdocument="";
editor=new LightLatexEditor(frame,efont,edcolors,hicolors,"");
connect(editor, SIGNAL(requestFind()), this, SLOT(showFind()));
connect(editor, SIGNAL(requestGotoLine()), this, SLOT(showGoto()));
m_lineNumberWidget = new LightLineNumberWidget( editor, frame);
m_lineNumberWidget->setFont(efont);
QFontMetrics fm( efont );
m_lineNumberWidget->setFixedWidth( fm.width( "00000" ) + 22 );
QHBoxLayout* lay = new QHBoxLayout( frame );
lay->setSpacing(0);
lay->setMargin(0);
lay->addWidget( m_lineNumberWidget );
lay->addWidget( editor );
setFocusProxy( editor );
setLineNumberWidgetVisible(line);

Stack=new QStackedWidget(this);
Stack->setLineWidth(0);
Stack->setFrameShape(QFrame::NoFrame);
Stack->setFrameShadow(QFrame::Plain);

findwidget=new LightFindWidget(Stack);
Stack->addWidget(findwidget);
findwidget->SetEditor(editor);
connect(findwidget, SIGNAL( requestHide() ), Stack, SLOT( hide() ) );
connect(findwidget, SIGNAL( requestExtension() ), this, SLOT( showFind() ) );

gotolinewidget=new LightGotoLineWidget(Stack);
Stack->addWidget(gotolinewidget);
gotolinewidget->SetEditor(editor);
connect(gotolinewidget, SIGNAL( requestHide() ), Stack, SLOT( hide() ) );

Stack->hide();

splitter->addWidget(framebis);
splitter->addWidget(Stack);

QVBoxLayout *mainlayout= new QVBoxLayout(this);
mainlayout->setSpacing(0);
mainlayout->setMargin(0);
mainlayout->addWidget(splitter);
}

UnicodeView::~UnicodeView()
{
}

void UnicodeView::setLineNumberWidgetVisible( bool b )
{
    if( b ){
	m_lineNumberWidget->show();
    } else {
	m_lineNumberWidget->hide();
    }
}

void UnicodeView::changeSettings(QFont & new_font,bool line)
{
  editor->changeFont(new_font);
  m_lineNumberWidget->setFont(new_font);
  QFontMetrics fm( new_font );
  m_lineNumberWidget->setFixedWidth( fm.width( "00000" ) + 32 );
  setLineNumberWidgetVisible(line);
}

void UnicodeView::fileSave()
{
QString currentDir=QDir::homePath();
if (!lastdocument.isEmpty())
	{
	QFileInfo fi(lastdocument);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getSaveFileName(this,tr("Save As"),currentDir,"TeX files (*.tex *.bib *.sty *.cls *.mp *.Rnw *.asy);;All files (*.*)");
if ( !fn.isEmpty() )
	{
	if (!fn.contains('.')) fn += ".tex";
	QFile file(fn);
	if ( !file.open( QIODevice::WriteOnly ) )
		{
		QMessageBox::warning( this,tr("Error"),tr("The file could not be saved. Please check if you have write permission."));
		return;
		}
	QTextStream ts( &file );
	QTextCodec* codec = QTextCodec::codecForName(QString("UTF-8").toLatin1());
	ts.setCodec(codec);
	ts << editor->toPlainText();
	file.close();
	lastdocument=fn;
	}

}

void UnicodeView::showFind()
{
QList<int> sizes;
sizes  << height()-findwidget->height() << findwidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(findwidget);
Stack->setMaximumHeight(findwidget->minimumSizeHint().height());
Stack->show();
QTextCursor c =editor->textCursor();
if (c.hasSelection()) findwidget->ui.comboFind->lineEdit()->setText(c.selectedText());
findwidget->ui.comboFind->setFocus();
findwidget->ui.comboFind->lineEdit()->selectAll();
}

/*void UnicodeView::showFindNext()
{
QList<int> sizes;
sizes << height()-findwidget->height() << findwidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(findwidget);
Stack->setMaximumHeight(findwidget->minimumSizeHint().height());
Stack->show();
if (findwidget->ui.comboFind->lineEdit()->text()!="") findwidget->doFind();
}*/

void UnicodeView::showGoto()
{
QList<int> sizes;
sizes  << height()-gotolinewidget->height() << gotolinewidget->height();
splitter->setSizes( sizes );
Stack->setCurrentWidget(gotolinewidget);
Stack->setMaximumHeight(gotolinewidget->minimumSizeHint().height());
Stack->show();
gotolinewidget->ui.spinLine->setFocus();
gotolinewidget->ui.spinLine->setMinimum( 1 );
gotolinewidget->ui.spinLine->setMaximum(editor->numoflines() );
gotolinewidget->ui.spinLine->selectAll();
}

