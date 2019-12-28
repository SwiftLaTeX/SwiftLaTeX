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

#include "configdialog.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QFileDialog>
#include <QColorDialog>
#include <QDir>
#include <QTableWidgetItem>
#include <QDebug>

#include "geticon.h"

ConfigDialog::ConfigDialog(QWidget* parent): QDialog( parent)
{
setModal(true);
ui.setupUi(this);

ui.contentsWidget->setIconSize(QSize(64, 64));
ui.contentsWidget->setViewMode(QListView::IconMode);
ui.contentsWidget->setMovement(QListView::Static);
ui.labelBibtex->setText("Bib(la)tex");
//pageditor
QFontDatabase fdb;
ui.comboBoxFont->addItems( fdb.families() );

ui.comboBoxEncoding->addItem("UTF-8");
foreach (int mib, QTextCodec::availableMibs()) 
	{
	QTextCodec *codec = QTextCodec::codecForMib(mib);
	if (codec->name()!="UTF-8") ui.comboBoxEncoding->addItem(codec->name());
	}
connect( ui.pushButtonAspell, SIGNAL(clicked()), this, SLOT(browseAspell()));
connect( ui.pushButtonSvn, SIGNAL(clicked()), this, SLOT(browseSvn()));

//ui.labelGetDic->setText( tr("Get dictionary at: %1").arg("<br><a href=\"http://wiki.services.openoffice.org/wiki/Dictionaries\">http://wiki.services.openoffice.org/wiki/Dictionaries</a>") );
//ui.labelGetDic->setOpenExternalLinks(true);


QTableWidgetItem *colorItem;
ui.colortableWidget->setRowCount(12);

colorItem= new QTableWidgetItem("Background");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(0,0,colorItem);

colorItem= new QTableWidgetItem("Current line");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(1,0,colorItem);


colorItem= new QTableWidgetItem("Highlighted line");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(2,0,colorItem);


colorItem= new QTableWidgetItem("Text");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(3,0,colorItem);


colorItem= new QTableWidgetItem("Comment");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(4,0,colorItem);


colorItem= new QTableWidgetItem("Math");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(5,0,colorItem);


colorItem= new QTableWidgetItem("LaTeX command");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(6,0,colorItem);


colorItem= new QTableWidgetItem("Structure");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(7,0,colorItem);


colorItem= new QTableWidgetItem("Verbatim/Graphics environments");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(8,0,colorItem);


colorItem= new QTableWidgetItem("Todo");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(9,0,colorItem);


colorItem= new QTableWidgetItem("Graphic keywords");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(10,0,colorItem);


colorItem= new QTableWidgetItem("Graphic Numbers");
colorItem->setFlags(Qt::NoItemFlags);
ui.colortableWidget->setItem(11,0,colorItem);

ui.colortableWidget->horizontalHeader()->resizeSection( 0, 150 );

connect( ui.pushButtonDefault, SIGNAL(clicked()), this, SLOT(restoreColors()));
connect( ui.pushButtonDark, SIGNAL(clicked()), this, SLOT(darkColors()));


//pagequick
connect(ui.radioButton6, SIGNAL(toggled(bool)),ui.lineEditUserquick, SLOT(setEnabled(bool)));
connect(ui.radioButton6, SIGNAL(toggled(bool)),ui.pushButtonWizard, SLOT(setEnabled(bool)));

//pagetools
connect( ui.pushButtonLatex, SIGNAL(clicked()), this, SLOT(browseLatex()));
connect( ui.pushButtonDvips, SIGNAL(clicked()), this, SLOT(browseDvips()));
connect( ui.pushButtonBibtex, SIGNAL(clicked()), this, SLOT(browseBibtex()));
connect( ui.pushButtonMakeindex, SIGNAL(clicked()), this, SLOT(browseMakeindex()));
connect( ui.pushButtonDviviewer, SIGNAL(clicked()), this, SLOT(browseDviviewer()));
connect( ui.pushButtonPsviewer, SIGNAL(clicked()), this, SLOT(browsePsviewer()));
connect( ui.pushButtonPdflatex, SIGNAL(clicked()), this, SLOT(browsePdflatex()));
connect( ui.pushButtonXelatex, SIGNAL(clicked()), this, SLOT(browseXelatex()));
connect( ui.pushButtonLualatex, SIGNAL(clicked()), this, SLOT(browseLualatex()));
connect( ui.pushButtonDvipdfm, SIGNAL(clicked()), this, SLOT(browseDvipdfm()));
connect( ui.pushButtonPs2pdf, SIGNAL(clicked()), this, SLOT(browsePs2pdf()));
connect( ui.pushButtonPdfviewer, SIGNAL(clicked()), this, SLOT(browsePdfviewer()));
connect( ui.pushButtonMetapost, SIGNAL(clicked()), this, SLOT(browseMetapost()));
connect( ui.pushButtonGhostscript, SIGNAL(clicked()), this, SLOT(browseGhostscript()));
connect( ui.pushButtonAsymptote, SIGNAL(clicked()), this, SLOT(browseAsymptote()));
connect( ui.pushButtonLatexmk, SIGNAL(clicked()), this, SLOT(browseLatexmk()));

connect( ui.pushButtonWizard, SIGNAL(clicked()), this, SLOT(userQuickWizard()));
connect( ui.pushButtonAsyWizard, SIGNAL(clicked()), this, SLOT(asyQuickWizard()));

connect(ui.shorttableWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(configureShortCut(QTableWidgetItem*)));
connect(ui.pushButtonToggleFocus, SIGNAL(clicked()), this, SLOT(configureKeyToggle()));

createIcons();
ui.contentsWidget->setCurrentRow(0);

#if defined(Q_OS_WIN32)
ui.lineEditPrinter->setEnabled(false);
#endif
#if defined(Q_OS_MAC)
resize(995,754);
#endif
}

ConfigDialog::~ConfigDialog(){
}

void ConfigDialog::createIcons()
{
int w=0;
int h=0;
ui.contentsWidget->setIconSize ( QSize(64,64 ));

QListWidgetItem *commandButton = new QListWidgetItem(ui.contentsWidget);
commandButton->setIcon(getIcon(":/images/configtools.png"));
commandButton->setText(tr("Commands"));
commandButton->setTextAlignment(Qt::AlignHCenter);
commandButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


QListWidgetItem *quickButton = new QListWidgetItem(ui.contentsWidget);
quickButton->setIcon(getIcon(":/images/configquick.png"));
quickButton->setText(tr("Quick Build"));
quickButton->setTextAlignment(Qt::AlignHCenter);
quickButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

QListWidgetItem *editorButton = new QListWidgetItem(ui.contentsWidget);
editorButton->setIcon(getIcon(":/images/configeditor.png"));
editorButton->setText(tr("Editor"));
editorButton->setTextAlignment(Qt::AlignHCenter);
editorButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

QListWidgetItem *keysButton = new QListWidgetItem(ui.contentsWidget);
keysButton->setIcon(getIcon(":/images/configkeys.png"));
keysButton->setText(tr("Shortcuts"));
keysButton->setTextAlignment(Qt::AlignHCenter);
keysButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

h=ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Commands")).height();
w=ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Commands")).width();
if (ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Quick Build")).width()>w) w=ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Quick Build")).width();
if (ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Editor")).width()>w) w=ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Editor")).width();
if (ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Shortcuts")).width()>w) w=ui.contentsWidget->fontMetrics().size(Qt::TextSingleLine,tr("Shortcuts")).width();
if (w<64) w=64;
ui.contentsWidget->setGridSize(QSize(w+24,64+2*h));

w=w+ui.contentsWidget->contentsMargins().left()+ui.contentsWidget->contentsMargins().right();

ui.contentsWidget->setMinimumWidth(w+24);
ui.contentsWidget->setMaximumWidth(w+24);

connect(ui.contentsWidget,
	SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
	this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    ui.pagesWidget->setCurrentIndex(ui.contentsWidget->row(current));
}

//pageditor
void ConfigDialog::browseAspell()
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QDir spelldir(QCoreApplication::applicationDirPath());
#else
#ifdef DEBIAN_SPELLDIR
QDir spelldir(PREFIX"/share/myspell/dicts");
#else
QDir spelldir(PREFIX"/share/texmaker");
#endif
#endif

#endif
#if defined(Q_OS_MAC)
QDir spelldir(QCoreApplication::applicationDirPath() + "/../Resources");
#endif
#if defined(Q_OS_WIN32)
QDir spelldir(QCoreApplication::applicationDirPath());
#endif
QString location=QFileDialog::getOpenFileName(this,tr("Browse dictionary"),spelldir.absolutePath(),"Dictionary (*.dic)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
//	location="\""+location+"\"";
	ui.lineEditAspellCommand->setText( location );
	}
}

void ConfigDialog::browseSvn()
{
#if defined(Q_OS_WIN32)
QString svndir=qgetenv("PROGRAMFILES");
#else
QString svndir="/usr/bin";
#endif
QString folder = QFileDialog::getExistingDirectory(this, tr("svn command directory"), svndir, 0);
if(!folder.isEmpty())
{
ui.lineEditSvn->setText( folder );    
}

}

void ConfigDialog::restoreColors()
{
QTableWidgetItem *colorItem;

colorItem= new QTableWidgetItem("#FFFFFF");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(0,1,colorItem);

colorItem= new QTableWidgetItem("#ececec");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(1,1,colorItem);

colorItem= new QTableWidgetItem("#FF0000");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(2,1,colorItem);

colorItem= new QTableWidgetItem("#000000");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(3,1,colorItem);

colorItem= new QTableWidgetItem("#606060");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(4,1,colorItem);

colorItem= new QTableWidgetItem("#008000");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(5,1,colorItem);

colorItem= new QTableWidgetItem("#800000");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(6,1,colorItem);

colorItem= new QTableWidgetItem("#0000CC");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(7,1,colorItem);

colorItem= new QTableWidgetItem("#9A4D00");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(8,1,colorItem);

colorItem= new QTableWidgetItem("#FF0000");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(9,1,colorItem);

colorItem= new QTableWidgetItem("#006699");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(10,1,colorItem);


colorItem= new QTableWidgetItem("#660066");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(11,1,colorItem);
}

void ConfigDialog::darkColors()
{
QTableWidgetItem *colorItem;

colorItem= new QTableWidgetItem("#2E2E2E");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(0,1,colorItem);

colorItem= new QTableWidgetItem("#6F6F6F");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(1,1,colorItem);

colorItem= new QTableWidgetItem("#FF0000");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(2,1,colorItem);

colorItem= new QTableWidgetItem("#f8f8f8");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(3,1,colorItem);

colorItem= new QTableWidgetItem("#aeaeae");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(4,1,colorItem);

colorItem= new QTableWidgetItem("#61ce3c");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(5,1,colorItem);

colorItem= new QTableWidgetItem("#8da6ce");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(6,1,colorItem);

colorItem= new QTableWidgetItem("#fadd2d");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(7,1,colorItem);

colorItem= new QTableWidgetItem("#e6a252");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(8,1,colorItem);

colorItem= new QTableWidgetItem("#FF6400");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(9,1,colorItem);

colorItem= new QTableWidgetItem("#f47bda");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(10,1,colorItem);


colorItem= new QTableWidgetItem("#660066");
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
ui.colortableWidget->setItem(11,1,colorItem);
}

//pagetools
void ConfigDialog::browseLatex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" -interaction=nonstopmode %.tex";
	ui.lineEditLatex->setText( location );
	}
}

void ConfigDialog::browseDvips()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" -o %.ps %.dvi";
	ui.lineEditDvips->setText( location );
	}
}

void ConfigDialog::browseBibtex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	if (location.contains("bibtex") && !location.contains("MiKTeX")) location="\""+location+"\" %.aux";
	else location="\""+location+"\" %";
	ui.lineEditBibtex->setText( location );
	}
}

void ConfigDialog::browseMakeindex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.idx";
	ui.lineEditMakeindex->setText( location );
	}
}

void ConfigDialog::browseDviviewer()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.dvi";
	ui.lineEditDviviewer->setText( location );
	}
}

void ConfigDialog::browsePsviewer()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.ps";
	ui.lineEditPsviewer->setText( location );
	}
}

void ConfigDialog::browsePdflatex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" -interaction=nonstopmode %.tex";
	ui.lineEditPdflatex->setText( location );
	}
}

void ConfigDialog::browseXelatex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" -interaction=nonstopmode %.tex";
	ui.lineEditXelatex->setText( location );
	}
}

void ConfigDialog::browseLualatex()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" -interaction=nonstopmode %.tex";
	ui.lineEditLualatex->setText( location );
	}
}

void ConfigDialog::browseDvipdfm()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.dvi";
	ui.lineEditDvipdfm->setText( location );
	}
}

void ConfigDialog::browsePs2pdf()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.ps";
	ui.lineEditPs2pdf->setText( location );
	}
}

void ConfigDialog::browsePdfviewer()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.pdf";
	ui.lineEditPdfviewer->setText( location );
	}
}

void ConfigDialog::browseMetapost()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" --interaction nonstopmode ";
	ui.lineEditMetapost->setText( location );
	}
}

void ConfigDialog::browseGhostscript()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\"";
	ui.lineEditGhostscript->setText( location );
	}
}

void ConfigDialog::browseAsymptote()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" %.asy";
	ui.lineEditAsymptote->setText( location );
	}
}

void ConfigDialog::browseLatexmk()
{
QString location=QFileDialog::getOpenFileName(this,tr("Browse program"),QDir::rootPath(),"Program (*)",0,QFileDialog::DontResolveSymlinks);
if ( !location.isEmpty() ) 
	{
	location.replace(QString("\\"),QString("/"));
	location="\""+location+"\" -e \"$pdflatex=q/pdflatex -interaction=nonstopmode/\" -pdf %.tex";
	ui.lineEditLatexmk->setText( location );
	}
}

void ConfigDialog::configureShortCut(QTableWidgetItem *item)
{
QString shortcut,data,newshortcut;
if (item)
	{
	shortcut=item->text();
	data=item->data(Qt::UserRole).toString();
	if (data=="key") 
	    {
	    keydlg = new KeySequenceDialog(this);
	    keydlg->setKeySequence(QKeySequence(shortcut));
	    if ( keydlg->exec() )
		{
		newshortcut=keydlg->ui.lineEdit->text();
		if (!newshortcut.isEmpty()) 
		    {
		    item->setText(newshortcut);
		    item->setData(Qt::UserRole,QString("key"));
		    }
		else
		    {
		    item->setText("none");
		    item->setData(Qt::UserRole,QString("key"));
		    }
		}
	    delete (keydlg);
	    }
	}
}

void ConfigDialog::configureKeyToggle()
{
QString shortcut,data,newshortcut;
shortcut=ui.pushButtonToggleFocus->text();
keydlg = new KeySequenceDialog(this);
keydlg->setKeySequence(QKeySequence(shortcut));
if ( keydlg->exec() )
	{
	newshortcut=keydlg->ui.lineEdit->text();
	if (!newshortcut.isEmpty()) 
	    {
	    ui.pushButtonToggleFocus->setText(newshortcut);
	    }
	else
	    {
	    ui.pushButtonToggleFocus->setText("none");
	    }
	}
delete (keydlg);
}

void ConfigDialog::userQuickWizard()
{
QStringList usualNames, usualCommands;

usualNames.append(tr("LaTeX"));
usualCommands.append(ui.lineEditLatex->text());

usualNames.append(tr("PdfLaTeX"));
usualCommands.append(ui.lineEditPdflatex->text());

usualNames.append(tr("dvips"));
usualCommands.append(ui.lineEditDvips->text());

usualNames.append(tr("Dvi Viewer"));
usualCommands.append(ui.lineEditDviviewer->text());

usualNames.append(tr("PS Viewer"));
usualCommands.append(ui.lineEditPsviewer->text());

usualNames.append(tr("Dvipdfm"));
usualCommands.append(ui.lineEditDvipdfm->text());

usualNames.append(tr("ps2pdf"));
usualCommands.append(ui.lineEditPs2pdf->text());

usualNames.append(tr("Bibtex"));
usualCommands.append(ui.lineEditBibtex->text());

usualNames.append(tr("Makeindex"));
usualCommands.append(ui.lineEditMakeindex->text());

usualNames.append(tr("Pdf Viewer"));
usualCommands.append(ui.lineEditPdfviewer->text());

usualNames.append(tr("metapost"));
usualCommands.append(ui.lineEditMetapost->text());

usualNames.append(tr("ghostscript"));
usualCommands.append(ui.lineEditGhostscript->text());

usualNames.append(tr("Asymptote"));
usualCommands.append(ui.lineEditAsymptote->text());

usualNames.append(tr("Latexmk"));
usualCommands.append(ui.lineEditLatexmk->text());

usualNames.append(tr("R Sweave"));
usualCommands.append(ui.lineEditSweave->text());

usualNames.append(tr("XeLaTeX"));
usualCommands.append(ui.lineEditXelatex->text());

usualNames.append(tr("LuaLaTeX"));
usualCommands.append(ui.lineEditLualatex->text());

userquickdlg= new UserQuickDialog(this,usualNames,usualCommands);
if ( userquickdlg->exec() )
   {
   ui.lineEditUserquick->setText(userquickdlg->userQuickCommand);
   }
delete (userquickdlg);
}

void ConfigDialog::asyQuickWizard()
{
QStringList usualNames, usualCommands;

usualNames.append(tr("LaTeX"));
usualCommands.append(ui.lineEditLatex->text());

usualNames.append(tr("PdfLaTeX"));
usualCommands.append(ui.lineEditPdflatex->text());

usualNames.append(tr("dvips"));
usualCommands.append(ui.lineEditDvips->text());

usualNames.append(tr("Dvi Viewer"));
usualCommands.append(ui.lineEditDviviewer->text());

usualNames.append(tr("PS Viewer"));
usualCommands.append(ui.lineEditPsviewer->text());

usualNames.append(tr("Dvipdfm"));
usualCommands.append(ui.lineEditDvipdfm->text());

usualNames.append(tr("ps2pdf"));
usualCommands.append(ui.lineEditPs2pdf->text());

usualNames.append(tr("Bibtex"));
usualCommands.append(ui.lineEditBibtex->text());

usualNames.append(tr("Makeindex"));
usualCommands.append(ui.lineEditMakeindex->text());

usualNames.append(tr("Pdf Viewer"));
usualCommands.append(ui.lineEditPdfviewer->text());

usualNames.append(tr("metapost"));
usualCommands.append(ui.lineEditMetapost->text());

usualNames.append(tr("ghostscript"));
usualCommands.append(ui.lineEditGhostscript->text());

usualNames.append(tr("Asymptote"));
usualCommands.append(ui.lineEditAsymptote->text());

usualNames.append(tr("Latexmk"));
usualCommands.append(ui.lineEditLatexmk->text());

usualNames.append(tr("R Sweave"));
usualCommands.append(ui.lineEditSweave->text());

usualNames.append(tr("XeLaTeX"));
usualCommands.append(ui.lineEditXelatex->text());

usualNames.append(tr("LuaLaTeX"));
usualCommands.append(ui.lineEditLualatex->text());

userquickdlg= new UserQuickDialog(this,usualNames,usualCommands);
if ( userquickdlg->exec() )
   {
   ui.lineEditAsyQuick->setText(userquickdlg->userQuickCommand);
   }
delete (userquickdlg);
}
