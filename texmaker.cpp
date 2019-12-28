/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   addons by Luis Silvestre ; S. Razi Alavizadeh; Robin.Watts            *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation  either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//#include <stdlib.h>

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QFileInfo>
#include <QLabel>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QProcess>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QLocale>
#include <QTabWidget>
#include <QStyleFactory>
#include <QStyle>
#include <QFontDatabase>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextBlock>
#include <QDebug>
#include <QDesktopServices>
#include <QAbstractItemModel>
#include <QTextCharFormat>
#include <QTextTableFormat>
#include <QTextLength>
#include <QFrame>
#include <QFontMetrics>
#include <QDateTime>
#include <QTemporaryFile>
#include <QPrintDialog>
#include <QPrinter>
#include <QProcessEnvironment>
#include <QSysInfo> 




//#ifdef Q_WS_WIN
//#include <windows.h>
//#endif

#include "geticon.h"
#include "texmaker.h"
#include "texmakerapp.h"
#include "latexeditorview.h"
//#include "manhattanstyle.h"
#include "structdialog.h"
#include "filechooser.h"
#include "graphicfilechooser.h"
#include "tabdialog.h"
#include "arraydialog.h"
#include "tabbingdialog.h"
#include "letterdialog.h"
#include "quickdocumentdialog.h"
#include "quickxelatexdialog.h"
#include "quickbeamerdialog.h"
#include "usermenudialog.h"
#include "usertooldialog.h"
#include "refdialog.h"
#include "configdialog.h"
#include "aboutdialog.h"
#include "spellerdialog.h"
#include "encodingdialog.h"
#include "usercompletiondialog.h"
#include "texdocdialog.h"
#include "addtagdialog.h"
#include "exportdialog.h"
#include "versiondialog.h"
#include "unicodedialog.h"
#include "svnhelper.h"
#include "theme.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#include "x11fontdialog.h"
#endif


Texmaker::Texmaker(QWidget *parent)
    : QMainWindow(parent)
{
eraseSettings=false;
replaceSettings=false;

ReadSettings();


QString tempDir=QDir::tempPath();
#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)
QString path=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
if (QDir().mkpath(path)) tempDir=path;
#endif
QString prefixFile=QDir::homePath();
prefixFile="tks_temp_"+prefixFile.section('/',-1);
prefixFile=QString(QUrl::toPercentEncoding(prefixFile));
prefixFile.remove("%");
sessionTempFile=tempDir+"/"+prefixFile+".tks";

 if (spelldicExist()) 
       {
       QString dic=spell_dic.left(spell_dic.length()-4);
       spellChecker = new Hunspell(dic.toLatin1()+".aff",dic.toLatin1()+".dic");
       }
 else spellChecker=0;

//spellChecker=0;
untitled_id=1;


#if defined(Q_OS_MAC)
setWindowIcon(QIcon(":/images/logo128.png"));
//MacSupport::addFullscreen(this);
#else
setWindowIcon(getIcon(":/images/appicon.png"));
#endif
QApplication::setOrganizationName("Xm1");
QApplication::setApplicationName("Texmaker"); 

setIconSize(QSize(22,22 ));
 
completer = new QCompleter(this);
initCompleter();


QAction *Act;
splitter1=new MiniSplitter(this);
splitter1->setOrientation(Qt::Horizontal);
splitter2=new MiniSplitter(splitter1);
splitter2->setOrientation(Qt::Vertical);
// PANNEAU STRUCTURE

LeftPanelFrameBis=new QFrame(this);
LeftPanelFrameBis->setLineWidth(0);
LeftPanelFrameBis->setFrameShape(QFrame::NoFrame);
LeftPanelFrameBis->setFrameShadow(QFrame::Plain);



LeftPanelToolBarBis=new QToolBar("TitleBar",LeftPanelFrameBis);
LeftPanelToolBarBis->setFloatable(false);
LeftPanelToolBarBis->setOrientation(Qt::Horizontal);
LeftPanelToolBarBis->setMovable(false);
LeftPanelToolBarBis->setIconSize(QSize(16,16 ));
LeftPanelToolBarBis->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
LeftPanelToolBarBis->setStyleSheet(Theme::viewportLightStyleSheet);

QFrame *LeftPanelFrame=new QFrame(this);
LeftPanelFrame->setLineWidth(0);
LeftPanelFrame->setFrameShape(QFrame::NoFrame);
LeftPanelFrame->setFrameShadow(QFrame::Plain);


splitter3=new MiniSplitter(splitter1);
splitter3->setOrientation(Qt::Vertical);

LeftPanelLayout= new QHBoxLayout(LeftPanelFrame);
LeftPanelToolBar=new QToolBar("LogToolBar",LeftPanelFrame);
LeftPanelToolBar->setFloatable(false);
LeftPanelToolBar->setOrientation(Qt::Vertical);
LeftPanelToolBar->setMovable(false);
LeftPanelToolBar->setIconSize(QSize(16,16 ));
LeftPanelToolBar->setStyleSheet(Theme::viewportLightStyleSheet);


LeftPanelStackedWidget=new QStackedWidget(LeftPanelFrame);




StructureTreeWidget=new QTreeWidget(LeftPanelStackedWidget);
StructureTreeWidget->setFrameStyle(QFrame::NoFrame);
StructureTreeWidget->setColumnCount(1);
StructureTreeWidget->header()->hide();
StructureTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
StructureTreeWidget->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

StructureTreeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

StructureTreeWidget->header()->setStretchLastSection(false);

StructureTreeWidget->viewport()->setStyleSheet(Theme::viewportDarkStyleSheet);
StructureTreeWidget->setStyleSheet(Theme::treeviewStyleSheet);
//StructureTreeWidget->setIndentation(10);

connect( StructureTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *,int )), SLOT(ClickedOnStructure(QTreeWidgetItem *,int)));

connect(LeftPanelToolBar->addAction(getIcon(":/images/structure.png"),tr("Structure")), SIGNAL(triggered()), this, SLOT(ShowStructure()));
LeftPanelStackedWidget->addWidget(StructureTreeWidget);

OpenedFilesListWidget=new QListWidget(LeftPanelFrame);
//OpenedFilesListWidget=new QListWidget(LeftPanelStackedWidget);
OpenedFilesListWidget->setFrameStyle(QFrame::NoFrame);
connect(OpenedFilesListWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(OpenedFileActivated(QListWidgetItem*)));
OpenedFilesListWidget->viewport()->setStyleSheet(Theme::viewportDarkStyleSheet);
OpenedFilesListWidget->setStyleSheet(Theme::listwidgetStyleSheet);
//connect(LeftPanelToolBar->addAction(QIcon(":/images/opened.png"),tr("Opened Files")), SIGNAL(triggered()), this, SLOT(ShowOpenedFiles()));
//LeftPanelStackedWidget->addWidget(OpenedFilesListWidget);


LeftPanelToolBar->addSeparator();


RelationListWidget=new SymbolListWidget(LeftPanelStackedWidget,0);
RelationListWidget->setFrameStyle(QFrame::NoFrame);
connect(RelationListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
relationAct = new QAction(getIcon(":/images/math1.png"),tr("Relation symbols"), this);
connect(relationAct, SIGNAL(triggered()), this, SLOT(ShowRelation()));
LeftPanelToolBar->addAction(relationAct);
connect(RelationListWidget->addAct, SIGNAL(triggered()), this, SLOT(InsertFavoriteSymbols()));
LeftPanelStackedWidget->addWidget(RelationListWidget);


ArrowListWidget=new SymbolListWidget(LeftPanelStackedWidget,1);
ArrowListWidget->setFrameStyle(QFrame::NoFrame);
connect(ArrowListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
arrowAct = new QAction(getIcon(":/images/math2.png"),tr("Arrow symbols"), this);
connect(arrowAct, SIGNAL(triggered()), this, SLOT(ShowArrow()));
LeftPanelToolBar->addAction(arrowAct);
connect(ArrowListWidget->addAct, SIGNAL(triggered()), this, SLOT(InsertFavoriteSymbols()));
LeftPanelStackedWidget->addWidget(ArrowListWidget);

MiscellaneousListWidget=new SymbolListWidget(LeftPanelStackedWidget,2);
MiscellaneousListWidget->setFrameStyle(QFrame::NoFrame);
connect(MiscellaneousListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
miscAct = new QAction(getIcon(":/images/math3.png"),tr("Miscellaneous symbols"), this);
connect(miscAct, SIGNAL(triggered()), this, SLOT(ShowMisc()));
LeftPanelToolBar->addAction(miscAct);
connect(MiscellaneousListWidget->addAct, SIGNAL(triggered()), this, SLOT(InsertFavoriteSymbols()));
LeftPanelStackedWidget->addWidget(MiscellaneousListWidget);

DelimitersListWidget=new SymbolListWidget(LeftPanelStackedWidget,3);
DelimitersListWidget->setFrameStyle(QFrame::NoFrame);
connect(DelimitersListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
delimAct = new QAction(getIcon(":/images/math4.png"),tr("Delimiters"), this);
connect(delimAct, SIGNAL(triggered()), this, SLOT(ShowDelim()));
LeftPanelToolBar->addAction(delimAct);
connect(DelimitersListWidget->addAct, SIGNAL(triggered()), this, SLOT(InsertFavoriteSymbols()));
LeftPanelStackedWidget->addWidget(DelimitersListWidget);

GreekListWidget=new SymbolListWidget(LeftPanelStackedWidget,4);
GreekListWidget->setFrameStyle(QFrame::NoFrame);
connect(GreekListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
greekAct = new QAction(getIcon(":/images/math5.png"),tr("Greek letters"), this);
connect(greekAct, SIGNAL(triggered()), this, SLOT(ShowGreek()));
LeftPanelToolBar->addAction(greekAct);
connect(GreekListWidget->addAct, SIGNAL(triggered()), this, SLOT(InsertFavoriteSymbols()));
LeftPanelStackedWidget->addWidget(GreekListWidget);

MostUsedListWidget=new SymbolListWidget(LeftPanelStackedWidget,5);
MostUsedListWidget->setFrameStyle(QFrame::NoFrame);
connect(MostUsedListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
usedAct = new QAction(getIcon(":/images/math6.png"),tr("Most used symbols"), this);
connect(usedAct, SIGNAL(triggered()), this, SLOT(ShowMostUsed()));
LeftPanelToolBar->addAction(usedAct);
SetMostUsedSymbols();
LeftPanelStackedWidget->addWidget(MostUsedListWidget);

FavoriteListWidget=new SymbolListWidget(LeftPanelStackedWidget,6);
FavoriteListWidget->setFrameStyle(QFrame::NoFrame);
connect(FavoriteListWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(InsertSymbol(QTableWidgetItem*)));
favAct = new QAction(getIcon(":/images/math7.png"),tr("Favorites symbols"), this);
connect(favAct, SIGNAL(triggered()), this, SLOT(ShowFavorite()));
LeftPanelToolBar->addAction(favAct);
FavoriteListWidget->SetFavoritePage(favoriteSymbolList);
connect(FavoriteListWidget->remAct, SIGNAL(triggered()), this, SLOT(RemoveFavoriteSymbols()));
LeftPanelStackedWidget->addWidget(FavoriteListWidget);

LeftPanelToolBar->addSeparator();


leftrightWidget=new XmlTagsListWidget(LeftPanelStackedWidget,":/tags/leftright_tags.xml");
leftrightWidget->setFrameStyle(QFrame::NoFrame);
connect(leftrightWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(InsertXmlTag(QListWidgetItem*)));
leftrightAct = new QAction(getIcon(":/images/leftright.png"),"left/right", this);
connect(leftrightAct, SIGNAL(triggered()), this, SLOT(ShowLeftRight()));
LeftPanelToolBar->addAction(leftrightAct);
LeftPanelStackedWidget->addWidget(leftrightWidget);

LeftPanelToolBar->addSeparator();

usertagsListWidget=new UserTagsListWidget(LeftPanelStackedWidget);
usertagsListWidget->setFrameStyle(QFrame::NoFrame);
connect(usertagsListWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(InsertUserElement(QListWidgetItem*)));
userpanelAct = new QAction(getIcon(":/images/user.png"),tr("User"), this);
connect(userpanelAct, SIGNAL(triggered()), this, SLOT(ShowUserPanel()));
LeftPanelToolBar->addAction(userpanelAct);
usertagsListWidget->updateList(userTagsList);
connect(usertagsListWidget->remAct, SIGNAL(triggered()), this, SLOT(RemoveUserTag()));
connect(usertagsListWidget->addAct, SIGNAL(triggered()), this, SLOT(AddUserTag()));
connect(usertagsListWidget->changeAct, SIGNAL(triggered()), this, SLOT(ChangeUserTag()));
connect(usertagsListWidget, SIGNAL(posChanged()), this, SLOT(UpdateUserTag()));

LeftPanelStackedWidget->addWidget(usertagsListWidget);

LeftPanelToolBar->addSeparator();

PsListWidget=new XmlTagsListWidget(LeftPanelStackedWidget,":/tags/pstricks_tags.xml");
PsListWidget->setFrameStyle(QFrame::NoFrame);
connect(PsListWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(InsertXmlTag(QListWidgetItem*)));
pstricksAct = new QAction(getIcon(":/images/pstricks.png"),tr("Pstricks Commands"), this);
connect(pstricksAct, SIGNAL(triggered()), this, SLOT(ShowPstricks()));
if (showPstricks) LeftPanelToolBar->addAction(pstricksAct);
LeftPanelStackedWidget->addWidget(PsListWidget);

MpListWidget=new XmlTagsListWidget(LeftPanelStackedWidget,":/tags/metapost_tags.xml");
MpListWidget->setFrameStyle(QFrame::NoFrame);
connect(MpListWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(InsertXmlTag(QListWidgetItem*)));
mpAct = new QAction(getIcon(":/images/metapost.png"),tr("MetaPost Commands"), this);
connect(mpAct, SIGNAL(triggered()), this, SLOT(ShowMplist()));
if (showMp) LeftPanelToolBar->addAction(mpAct);
LeftPanelStackedWidget->addWidget(MpListWidget);

tikzWidget=new XmlTagsListWidget(LeftPanelStackedWidget,":/tags/tikz_tags.xml");
tikzWidget->setFrameStyle(QFrame::NoFrame);
connect(tikzWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(InsertXmlTag(QListWidgetItem*)));
tikzAct = new QAction(getIcon(":/images/tikz.png"),tr("Tikz Commands"), this);
connect(tikzAct, SIGNAL(triggered()), this, SLOT(ShowTikz()));
if (showTikz) LeftPanelToolBar->addAction(tikzAct);
LeftPanelStackedWidget->addWidget(tikzWidget);

asyWidget=new XmlTagsListWidget(LeftPanelStackedWidget,":/tags/asymptote_tags.xml");
asyWidget->setFrameStyle(QFrame::NoFrame);
connect(asyWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(InsertXmlTag(QListWidgetItem*)));
asyAct = new QAction(getIcon(":/images/asymptote.png"),tr("Asymptote Commands"), this);
connect(asyAct, SIGNAL(triggered()), this, SLOT(ShowAsy()));
if (showAsy) LeftPanelToolBar->addAction(asyAct);
LeftPanelStackedWidget->addWidget(asyWidget);





viewPstricksAct = new QAction(tr("Pstricks Commands"), this);
viewPstricksAct->setCheckable(true);
connect(viewPstricksAct, SIGNAL(triggered()), this, SLOT(TogglePstricks()));
viewMpAct = new QAction(tr("MetaPost Commands"), this);
viewMpAct->setCheckable(true);
connect(viewMpAct, SIGNAL(triggered()), this, SLOT(ToggleMetapost()));
viewTikzAct = new QAction(tr("Tikz Commands"), this);
viewTikzAct->setCheckable(true);
connect(viewTikzAct, SIGNAL(triggered()), this, SLOT(ToggleTikz()));
viewAsyAct = new QAction(tr("Asymptote Commands"), this);
viewAsyAct->setCheckable(true);
connect(viewAsyAct, SIGNAL(triggered()), this, SLOT(ToggleAsymptote()));
LeftPanelToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
connect(LeftPanelToolBar, SIGNAL( customContextMenuRequested( const QPoint & )), this, SLOT( customContentsMenuStructure( const QPoint & )));

LeftPanelLayout->setSpacing(0);
LeftPanelLayout->setMargin(0);
LeftPanelLayout->addWidget(LeftPanelToolBar);
LeftPanelLayout->addWidget(LeftPanelStackedWidget);

LeftPanelStackedWidget->setCurrentWidget(StructureTreeWidget);
//Act = new QAction(QIcon(":/images/empty.png"),"", this);
//LeftPanelToolBarBis->addAction(Act);
//Act->setEnabled(false);
titleLeftPanel=new DropShadowLabel(tr("Structure").toUpper(),LeftPanelToolBarBis);
titleLeftPanel->setColor(Theme::grayColor);
titleLeftPanel->setDropShadowColor(QColor("#000000"));
titleLeftPanel->setStyleSheet(Theme::labelStyleSheet);
LeftPanelToolBarBis->addWidget(titleLeftPanel);

LeftPanelLayoutBis= new QVBoxLayout(LeftPanelFrameBis);
LeftPanelLayoutBis->setSpacing(0);
LeftPanelLayoutBis->setMargin(0);
LeftPanelLayoutBis->addWidget(LeftPanelToolBarBis);
LeftPanelLayoutBis->addWidget(LeftPanelFrame);

splitter3->addWidget(LeftPanelFrameBis);
splitter3->addWidget(OpenedFilesListWidget);

splitter1->addWidget(splitter3);
splitter3->setMinimumWidth(210);

Outputframe=new QFrame(this);
Outputframe->setLineWidth(0);
Outputframe->setFrameShape(QFrame::NoFrame);
Outputframe->setFrameShadow(QFrame::Plain);

OutputLayoutH= new QHBoxLayout(Outputframe);
OutputLayoutH->setSpacing(0);
OutputLayoutH->setMargin(0);

logToolBar=new QToolBar("LogToolBar",Outputframe);
logToolBar->setFloatable(false);
logToolBar->setOrientation(Qt::Vertical);
logToolBar->setMovable(false);
logToolBar->setIconSize(QSize(16,16 ));
logToolBar->setStyleSheet(Theme::viewportLightStyleSheet);

QFrame *Outputframebis=new QFrame(this);
Outputframebis->setLineWidth(0);
Outputframebis->setFrameShape(QFrame::NoFrame);
Outputframebis->setFrameShadow(QFrame::Plain);

OutputLayoutV= new QVBoxLayout(Outputframebis);
OutputLayoutV->setSpacing(0);
OutputLayoutV->setMargin(0);

OutputTableWidget= new QTableWidget (1,5,Outputframebis);
//OutputTableWidget->setFrameShape(QFrame::Box);
OutputTableWidget->setFrameShadow(QFrame::Plain);
OutputTableWidget->setFrameStyle(QFrame::NoFrame);
QTableWidgetItem *HeaderItem = new QTableWidgetItem(" ");
HeaderItem->setTextAlignment(Qt::AlignLeft);
OutputTableWidget->setHorizontalHeaderItem(0,HeaderItem);
HeaderItem = new QTableWidgetItem("File");
HeaderItem->setTextAlignment(Qt::AlignLeft);
OutputTableWidget->setHorizontalHeaderItem(1,HeaderItem);
HeaderItem = new QTableWidgetItem("Type");
HeaderItem->setTextAlignment(Qt::AlignLeft);
OutputTableWidget->setHorizontalHeaderItem(2,HeaderItem);
HeaderItem = new QTableWidgetItem("Line");
HeaderItem->setTextAlignment(Qt::AlignLeft);
OutputTableWidget->setHorizontalHeaderItem(3,HeaderItem);
HeaderItem = new QTableWidgetItem("Message");
HeaderItem->setTextAlignment(Qt::AlignLeft);
OutputTableWidget->setHorizontalHeaderItem(4,HeaderItem);

//OutputTableWidget->setWordWrap(true);
OutputTableWidget->setSelectionMode (QAbstractItemView::SingleSelection);
QFontMetrics fm(qApp->font());
OutputTableWidget->setColumnWidth(0,fm.width("> "));
OutputTableWidget->setColumnWidth(1,10*fm.width("w"));
OutputTableWidget->setColumnWidth(2,fm.width("WarningWW"));
OutputTableWidget->setColumnWidth(3,fm.width("Line WWWWWWWW"));
OutputTableWidget->setColumnWidth(4,20*fm.width("w"));
connect(OutputTableWidget, SIGNAL(itemClicked ( QTableWidgetItem*)), this, SLOT(ClickedOnLogLine(QTableWidgetItem*)));
OutputTableWidget->horizontalHeader()->setStretchLastSection(true);
OutputTableWidget->setMinimumHeight(4*(fm.lineSpacing()+4));
//OutputTableWidget->setMaximumHeight(8*(fm.lineSpacing()+4));
OutputTableWidget->verticalHeader()->hide();
//OutputTableWidget->horizontalHeader()->hide();




OutputTextEdit = new LogEditor(Outputframebis);
OutputTextEdit->setFrameStyle(QFrame::NoFrame);
OutputTextEdit->setMinimumHeight(4*(fm.lineSpacing()+4));
connect(OutputTextEdit, SIGNAL(clickonline(int )),this,SLOT(ClickedOnOutput(int )));

separatorline = new QFrame(Outputframebis);
separatorline->setMinimumHeight(1);
separatorline->setMaximumHeight(1);
separatorline->setFrameShape(QFrame::Box);
separatorline->setFrameShadow(QFrame::Plain);
separatorline->setStyleSheet("color:black");


OutputLayoutV->addWidget(OutputTableWidget);
OutputLayoutV->addWidget(separatorline);
OutputLayoutV->addWidget(OutputTextEdit);
OutputLayoutH->addWidget(logToolBar);
OutputLayoutH->addWidget(Outputframebis);
OutputLayoutH->setSpacing(0);

OutputTableWidget->hide();
separatorline->hide();


logpresent=false;
listViewerCommands.clear();
checkViewerInstance=false;

errorFileList.clear();
errorTypeList.clear();
errorLineList.clear();
errorMessageList.clear();
errorLogList.clear();
onlyErrorList.clear();
errorIndex=-1;

translationList.clear();
translationList.append(QString("en"));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#ifdef USB_VERSION
QDir transdir(QCoreApplication::applicationDirPath());
#else
QDir transdir(PREFIX"/share/texmaker");
#endif

#endif
#if defined(Q_OS_MAC)
QDir transdir(QCoreApplication::applicationDirPath() + "/../Resources");
#endif
#if defined(Q_OS_WIN32)
QDir transdir(QCoreApplication::applicationDirPath());
#endif
foreach (QFileInfo qmFileInfo, transdir.entryInfoList(QStringList("texmaker_*.qm"),QDir::Files | QDir::Readable, QDir::Name | QDir::IgnoreCase)) 
    {
    QString transName = qmFileInfo.completeBaseName();
    transName.remove("texmaker_");
    translationList.append(transName);
    }
    
scriptList.clear();    
foreach (QFileInfo qmScriptInfo, transdir.entryInfoList(QStringList("*.tms"),QDir::Files | QDir::Readable, QDir::Name | QDir::IgnoreCase)) 
    {
    scriptList.append(qmScriptInfo.completeBaseName());
    }    
    
    
StackedViewers=new QStackedWidget(this);
StackedViewers->setLineWidth(0);
StackedViewers->setFrameShape(QFrame::NoFrame);
StackedViewers->setFrameShadow(QFrame::Plain);
StackedViewers->setMinimumWidth(200);


// EDITEUR
QFrame *centralFrame=new QFrame(this);
centralFrame->setLineWidth(0);
centralFrame->setFrameShape(QFrame::NoFrame);
centralFrame->setFrameShadow(QFrame::Plain);


centralToolBar=new QToolBar("LogToolBar",centralFrame);
centralToolBar->setFloatable(false);
centralToolBar->setOrientation(Qt::Vertical);
centralToolBar->setMovable(false);
centralToolBar->setIconSize(QSize(16,16 ));
centralToolBar->setStyleSheet(Theme::viewportLightStyleSheet);


sectionMenu=new QMenu(this);
Act = new QAction("part", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);
Act = new QAction("chapter", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);
Act = new QAction("section", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);
Act = new QAction("subsection", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);
Act = new QAction("subsubsection", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);
Act = new QAction("paragraph", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);
Act = new QAction("subparagraph", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SectionCommand()));
sectionMenu->addAction(Act);

refMenu=new QMenu(this);
Act = new QAction("label", this);
connect(Act, SIGNAL(triggered()), this, SLOT(OtherCommand()));
refMenu->addAction(Act);
Act = new QAction("ref", this);
connect(Act, SIGNAL(triggered()), this, SLOT(OtherCommand()));
refMenu->addAction(Act);
Act = new QAction("pageref", this);
connect(Act, SIGNAL(triggered()), this, SLOT(OtherCommand()));
refMenu->addAction(Act);
Act = new QAction("index", this);
connect(Act, SIGNAL(triggered()), this, SLOT(OtherCommand()));
refMenu->addAction(Act);
Act = new QAction("cite", this);
connect(Act, SIGNAL(triggered()), this, SLOT(OtherCommand()));
refMenu->addAction(Act);
Act = new QAction("footnote", this);
connect(Act, SIGNAL(triggered()), this, SLOT(OtherCommand()));
refMenu->addAction(Act);

sizeMenu=new QMenu(this);
Act = new QAction("tiny", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("scriptsize", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("footnotesize", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("small", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("normalsize", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("large", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("Large", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("LARGE", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("huge", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);
Act = new QAction("Huge", this);
connect(Act, SIGNAL(triggered()), this, SLOT(SizeCommand()));
sizeMenu->addAction(Act);

Act = new QAction(getIcon(":/images/sectioning.png"),"part/chapter/section...", this);
connect(Act, SIGNAL(triggered()), this, SLOT(ShowSectionMenu()));
centralToolBar->addAction(Act);
Act = new QAction(getIcon(":/images/ref.png"),"Label/ref/cite...", this);
connect(Act, SIGNAL(triggered()), this, SLOT(ShowRefMenu()));
centralToolBar->addAction(Act);
Act = new QAction(getIcon(":/images/size.png"),"tiny/small/large...", this);
connect(Act, SIGNAL(triggered()), this, SLOT(ShowSizeMenu()));
centralToolBar->addAction(Act);

centralToolBar->addSeparator();

Act = new QAction(getIcon(":/images/text_bold.png"),tr("Bold"), this);
Act->setData("\\textbf{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
centralToolBar->addAction(Act);

Act = new QAction(getIcon(":/images/text_italic.png"),tr("Italic"), this);
Act->setData("\\textit{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
centralToolBar->addAction(Act);

emphasisAct = new QAction(getIcon(":/images/text_emphasis.png"),"Emphasis", this);
emphasisAct->setData("\\emph{/}/6/0");
connect(emphasisAct, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
if (showEmphasis) centralToolBar->addAction(emphasisAct);

Act = new QAction(getIcon(":/images/text_left.png"),tr("Left"), this);
Act->setData("\\begin{flushleft}\n/\n\\end{flushleft}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
centralToolBar->addAction(Act);

Act = new QAction(getIcon(":/images/text_center.png"),tr("Center"), this);
Act->setData("\\begin{center}\n/\n\\end{center}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
centralToolBar->addAction(Act);

Act = new QAction(getIcon(":/images/text_right.png"),tr("Right"), this);
Act->setData("\\begin{flushright}\n/\n\\end{flushright}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
centralToolBar->addAction(Act);
centralToolBar->addSeparator();

newlineAct = new QAction(getIcon(":/images/newline.png"),tr("New line"), this);
newlineAct->setData("\\\\\n/0/1/The \\newline command breaks the line right where it is. It can only be used in paragraph mode.");
connect(newlineAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showNewline) centralToolBar->addAction(newlineAct);

centralToolBar->addSeparator();

mathmodeAct = new QAction(getIcon(":/images/mathmode.png"),"$...$", this);
mathmodeAct->setData("$  $/2/0/The math environment can be used in both paragraph and LR mode");
connect(mathmodeAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showMathmode) centralToolBar->addAction(mathmodeAct);

indiceAct = new QAction(getIcon(":/images/indice.png"),"_{} - subscript", this);
indiceAct->setData("_{}/2/0/ ");
connect(indiceAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showIndice) centralToolBar->addAction(indiceAct);

puissanceAct = new QAction(getIcon(":/images/puissance.png"),"^{} - superscript", this);
puissanceAct->setData("^{}/2/0/ ");
connect(puissanceAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showPuissance) centralToolBar->addAction(puissanceAct);

smallfracAct = new QAction(getIcon(":/images/smallfrac.png"),"\\frac{}{}", this);
smallfracAct->setData("\\frac{}{}/6/0/ ");
connect(smallfracAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showSmallfrac) centralToolBar->addAction(smallfracAct);

dfracAct = new QAction(getIcon(":/images/dfrac.png"),"\\dfrac{}{}", this);
dfracAct->setData("\\dfrac{}{}/7/0/ ");
connect(dfracAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showDfrac) centralToolBar->addAction(dfracAct);

racineAct = new QAction(getIcon(":/images/racine.png"),"\\sqrt{}", this);
racineAct->setData("\\sqrt{}/6/0/ ");
connect(racineAct, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
if (showRacine) centralToolBar->addAction(racineAct);

showemphasisAct = new QAction("Emphasis", this);
showemphasisAct->setCheckable(true);
connect(showemphasisAct, SIGNAL(triggered()), this, SLOT(ToggleEmphasis()));
shownewlineAct = new QAction(tr("New line"), this);
shownewlineAct->setCheckable(true);
connect(shownewlineAct, SIGNAL(triggered()), this, SLOT(ToggleNewline()));
showmathmodeAct = new QAction("$...$", this);
showmathmodeAct->setCheckable(true);
connect(showmathmodeAct, SIGNAL(triggered()), this, SLOT(ToggleMathmode()));
showindiceAct = new QAction("_{} - subscript", this);
showindiceAct->setCheckable(true);
connect(showindiceAct, SIGNAL(triggered()), this, SLOT(ToggleIndice()));
showpuissanceAct = new QAction("^{} - superscript", this);
showpuissanceAct->setCheckable(true);
connect(showpuissanceAct, SIGNAL(triggered()), this, SLOT(TogglePuissance()));
showsmallfracAct = new QAction("\\frac{}{}", this);
showsmallfracAct->setCheckable(true);
connect(showsmallfracAct, SIGNAL(triggered()), this, SLOT(ToggleSmallfrac()));
showdfracAct = new QAction("\\dfrac{}{}", this);
showdfracAct->setCheckable(true);
connect(showdfracAct, SIGNAL(triggered()), this, SLOT(ToggleDfrac()));
showracineAct = new QAction("\\sqrt{}", this);
showracineAct->setCheckable(true);
connect(showracineAct, SIGNAL(triggered()), this, SLOT(ToggleRacine()));
centralToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
connect(centralToolBar, SIGNAL( customContextMenuRequested( const QPoint & )), this, SLOT( customContentsMenuMain( const QPoint & )));

QFrame *centralFrameBis=new QFrame(this);
centralFrameBis->setLineWidth(0);
centralFrameBis->setFrameShape(QFrame::NoFrame);
centralFrameBis->setFrameShadow(QFrame::Plain);

centralToolBarBis=new QToolBar("FileBar",centralFrameBis);
centralToolBarBis->setFloatable(false);
centralToolBarBis->setOrientation(Qt::Horizontal);
centralToolBarBis->setMovable(false);
centralToolBarBis->setIconSize(QSize(16,16 ));
centralToolBarBis->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
centralToolBarBis->setStyleSheet(Theme::viewportLightStyleSheet);

//LeftPanelToolBarBis->setMinimumHeight(centralToolBarBis->height());
//LeftPanelToolBarBis->setMaximumHeight(centralToolBarBis->height());
//centralToolBarBis->setStyle(QStyleFactory::create("Plastique"));

ToggleDocAct=new QAction(getIcon(":/images/toggle.png"),tr("Toggle between the master document and the current document")+" (CTRL+SHIFT+F2)", this);
ToggleDocAct->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_F2);
connect(ToggleDocAct, SIGNAL(triggered()), this, SLOT(ToggleMasterCurrent()));
centralToolBarBis->addAction(ToggleDocAct);

Act = new QAction(getIcon(":/images/errorprev.png"),tr("Previous Document"), this);
//Act->setShortcut(Qt::ALT+Qt::Key_PageUp);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoPrevDocument()));
centralToolBarBis->addAction(Act);
Act = new QAction(getIcon(":/images/errornext.png"),tr("Next Document"), this);
//Act->setShortcut(Qt::ALT+Qt::Key_PageDown);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoNextDocument()));
centralToolBarBis->addAction(Act);


comboFiles=new QComboBox(centralToolBarBis);
comboFiles->setMaximumWidth(300);
comboFiles->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
comboFiles->setMinimumContentsLength(20);
comboFiles->setMaxVisibleItems(40);
comboFiles->setContextMenuPolicy(Qt::CustomContextMenu);
comboFiles->setStyleSheet(Theme::comboboxLightStyleSheet);
connect(comboFiles, SIGNAL(activated(int)), this, SLOT(listSelectionActivated(int)));
centralToolBarBis->addWidget(comboFiles);
QWidget* spacer = new QWidget();
spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
centralToolBarBis->addWidget(spacer);
Act = new QAction(getIcon(":/images/fileclose.png"), tr("Close"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileClose()));
centralToolBarBis->addAction(Act);
centralToolBarBis->addSeparator();

posLabel=new DropShadowLabel("L: C: ",centralToolBarBis);
posLabel->setFixedWidth(fm.width("L:99999 C:99999"));
posLabel->setColor(Theme::grayColor);
posLabel->setDropShadowColor(QColor("#000000"));
posLabel->setStyleSheet(Theme::labelStyleSheet);

//posLabel=new QLabel("L: C: ",centralToolBarBis);
//posLabel->setFixedWidth(fm.width("L:99999 C:99999"));
centralToolBarBis->addWidget(posLabel);
centralToolBarBis->addSeparator();
Act = new QAction(getIcon(":/images/bookmark1.png"),tr("Click to jump to the bookmark"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoBookmark1()));
centralToolBarBis->addAction(Act);
Act = new QAction(getIcon(":/images/bookmark2.png"),tr("Click to jump to the bookmark"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoBookmark2()));
centralToolBarBis->addAction(Act);
Act = new QAction(getIcon(":/images/bookmark3.png"),tr("Click to jump to the bookmark"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(gotoBookmark3()));
centralToolBarBis->addAction(Act);

EditorView=new QStackedWidget(centralFrame);
EditorView->setLineWidth(0);
EditorView->setFrameShape(QFrame::NoFrame);
EditorView->setFrameShadow(QFrame::Plain);

connect(EditorView, SIGNAL( currentChanged( int ) ), this, SLOT(UpdateCaption()) );
connect(EditorView, SIGNAL( currentChanged( int ) ), this, SLOT(UpdateStructure()) );

CentralLayout= new QHBoxLayout(centralFrame);
CentralLayout->setSpacing(0);
CentralLayout->setMargin(0);
CentralLayout->addWidget(centralToolBar);
CentralLayout->addWidget(EditorView);

CentralLayoutBis= new QVBoxLayout(centralFrameBis);
CentralLayoutBis->setSpacing(0);
CentralLayoutBis->setMargin(0);
CentralLayoutBis->addWidget(centralToolBarBis);
CentralLayoutBis->addWidget(centralFrame);


splitter2->addWidget(centralFrameBis);
splitter2->addWidget(Outputframe);
connect(splitter2,SIGNAL(splitterMoved(int,int)), this, SLOT(splitter2Changed()));
//splitter1->addWidget(splitter3);
splitter1->addWidget(splitter2);
splitter1->addWidget(StackedViewers);
setCentralWidget(splitter1);

splitter2->show();
splitter3->show();
splitter1->show();



QList<int> sizes;
sizes << height()-200 << 200;
splitter2->setSizes( sizes );
sizes.clear();
sizes << 180 << (int) (width()-180)*0.5 << (int) (width()-180)*0.5;
splitter1->setSizes( sizes );
sizes.clear();
sizes << height()-50 << 50;
splitter3->setSizes( sizes );




createStatusBar();
setupMenus();
setupToolBars();



connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

QPalette pal = QApplication::palette();
QColor col=pal.color(QPalette::Window);

if (new_gui) 
{
restoreState(windowstate, 0);
if (winmaximized) this->setWindowState(Qt::WindowMaximized);
splitter1->restoreState(splitter1state);
splitter2->restoreState(splitter2state);
splitter3->restoreState(splitter3state);
}



ShowOutputView(false);
ShowStructView(false);
ShowFilesView(false);

sourceviewerWidget=new SourceView(StackedViewers,EditorFont,showline,edcolors(),hicolors());
sourceviewerWidget->editor->setEncoding(input_encoding);
if (wordwrap) {sourceviewerWidget->editor->setWordWrapMode(QTextOption::WordWrap);}
else {sourceviewerWidget->editor->setWordWrapMode(QTextOption::NoWrap);}
connect (sourceviewerWidget, SIGNAL(showDiff()), this, SLOT(compareDocuments()));
StackedViewers->addWidget(sourceviewerWidget);

ShowPdfView(false);
ShowSourceView(false);

if (embedinternalpdf && builtinpdfview && showpdfview ) 
  {
  StackedViewers->show();
  sourceviewerWidget->hide();
  }
else if (showsourceview)
  {
  StackedViewers->setCurrentWidget(sourceviewerWidget);
  StackedViewers->show();
  }
else StackedViewers->hide();

UpdateRecentFile();

UpdateCaption();
singlemode=true;
ToggleDocAct->setEnabled(false);
MasterName=getName();

show();

splitter2Changed();



LeftPanelToolBarBis->setMinimumHeight(centralToolBarBis->height());
sourceviewerWidget->centralToolBar->setMinimumHeight(centralToolBarBis->height());
sourceviewerWidget->centralToolBar->setMaximumHeight(centralToolBarBis->height());
stat1->setText(QString(" %1 ").arg(tr("Normal Mode")));
//stat2->setText(QString(" %1 ").arg(tr("Ready")));
stat3->setText(QString(" %1 ").arg(input_encoding));

setAcceptDrops(true);
autosaveTimer = new QTimer(this);
if (autosave)
    {
    connect(autosaveTimer, SIGNAL(timeout()), this, SLOT(fileBackupAll()));
    autosaveTimer->start(600000);
    }

}

Texmaker::~Texmaker(){
if (spellChecker) delete spellChecker;
}

void Texmaker::setupMenus()
{
QAction *Act;
//bool gtkEnv=gtkSession();
bool gtkEnv=false;
//file
fileMenu = menuBar()->addMenu(tr("&File"));
if (gtkEnv) Act = new QAction(QIcon::fromTheme("document-new", QIcon(":/images/filenew.png")), tr("New"), this);
else Act = new QAction(getIcon(":/images/filenew.png"), tr("New"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_N);
connect(Act, SIGNAL(triggered()), this, SLOT(fileNew()));
fileMenu->addAction(Act);

Act = new QAction(tr("New by copying an existing file"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileNewFromFile()));
fileMenu->addAction(Act);

if (gtkEnv) Act = new QAction(QIcon::fromTheme("document-open", QIcon(":/images/fileopen.png")), tr("Open"), this);
else Act = new QAction(getIcon(":/images/fileopen.png"), tr("Open"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_O);
connect(Act, SIGNAL(triggered()), this, SLOT(fileOpen()));
fileMenu->addAction(Act);

recentMenu=fileMenu->addMenu(tr("Open Recent"));
for (int i = 0; i < 10; ++i) 
	{
	recentFileActs[i] = new QAction(this);
	recentFileActs[i]->setVisible(false);
	connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(fileOpenRecent()));
	recentMenu->addAction(recentFileActs[i]);
	}
recentMenu->addSeparator();
Act = new QAction(tr("Clean"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(CleanRecent()));
recentMenu->addAction(Act);

sessionMenu=fileMenu->addMenu(tr("Session"));
Act = new QAction(tr("Restore previous session"), this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_F8);
connect(Act, SIGNAL(triggered()), this, SLOT(LoadLastSession()));
sessionMenu->addAction(Act);

Act = new QAction(tr("Save session"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(SaveSession()));
sessionMenu->addAction(Act);
Act = new QAction(tr("Load session"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(LoadSession()));
sessionMenu->addAction(Act);

if (gtkEnv) SaveAct = new QAction(QIcon::fromTheme("document-save", QIcon(":/images/filesave.png")), tr("Save"), this);
else SaveAct = new QAction(getIcon(":/images/filesave.png"), tr("Save"), this);
SaveAct->setShortcut(Qt::CTRL+Qt::Key_S);
connect(SaveAct, SIGNAL(triggered()), this, SLOT(fileSave()));
fileMenu->addAction(SaveAct);
fileMenu->addSeparator();


Act = new QAction(tr("Save As"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
fileMenu->addAction(Act);

Act = new QAction(tr("Save All"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileSaveAll()));
fileMenu->addAction(Act);

Act = new QAction(tr("Save A Copy"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileSaveACopy()));
fileMenu->addAction(Act);

Act = new QAction(getIcon(":/images/fileclose.png"), tr("Close"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_W);
connect(Act, SIGNAL(triggered()), this, SLOT(fileClose()));
fileMenu->addSeparator();
fileMenu->addAction(Act);

Act = new QAction(tr("Close All"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileCloseAll()));
fileMenu->addAction(Act);

Act = new QAction(tr("Reload document from file"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileReload()));
fileMenu->addAction(Act);

Act = new QAction(tr("Reload all documents from file"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(allReload()));
fileMenu->addAction(Act);

Act = new QAction(tr("Print"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_P);
connect(Act, SIGNAL(triggered()), this, SLOT(filePrint()));
fileMenu->addSeparator();
fileMenu->addAction(Act);

if (gtkEnv) Act = new QAction(QIcon::fromTheme("application-exit", QIcon(":/images/exit.png")), tr("Exit"), this);
else Act = new QAction(getIcon(":/images/exit.png"), tr("Exit"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_Q);
connect(Act, SIGNAL(triggered()), this, SLOT(fileExit()));
fileMenu->addSeparator();
fileMenu->addAction(Act);

editMenu = menuBar()->addMenu(tr("&Edit"));
if (gtkEnv) UndoAct = new QAction(QIcon::fromTheme("edit-undo", QIcon(":/images/undo.png")), tr("Undo"), this);
else UndoAct = new QAction(getIcon(":/images/undo.png"), tr("Undo"), this);
UndoAct->setShortcut(Qt::CTRL+Qt::Key_Z);
connect(UndoAct, SIGNAL(triggered()), this, SLOT(editUndo()));
editMenu->addAction(UndoAct);

if (gtkEnv) RedoAct = new QAction(QIcon::fromTheme("edit-redo", QIcon(":/images/redo.png")), tr("Redo"), this);
else RedoAct = new QAction(getIcon(":/images/redo.png"), tr("Redo"), this);
RedoAct->setShortcut(Qt::CTRL+Qt::Key_Y);
connect(RedoAct, SIGNAL(triggered()), this, SLOT(editRedo()));
editMenu->addAction(RedoAct);
editMenu->addSeparator();

if (gtkEnv) CopyAct = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/images/editcopy.png")), tr("Copy"), this);
else CopyAct = new QAction(getIcon(":/images/editcopy.png"), tr("Copy"), this);
CopyAct->setShortcut(Qt::CTRL+Qt::Key_C);
connect(CopyAct, SIGNAL(triggered()), this, SLOT(editCopy()));
editMenu->addAction(CopyAct);

if (gtkEnv) CutAct = new QAction(QIcon::fromTheme("edit-cut", QIcon(":/images/editcut.png")), tr("Cut"), this);
else CutAct = new QAction(getIcon(":/images/editcut.png"), tr("Cut"), this);
CutAct->setShortcut(Qt::CTRL+Qt::Key_X);
connect(CutAct, SIGNAL(triggered()), this, SLOT(editCut()));
editMenu->addAction(CutAct);

if (gtkEnv) PasteAct = new QAction(QIcon::fromTheme("edit-paste", QIcon(":/images/editpaste.png")), tr("Paste"), this);
else PasteAct = new QAction(getIcon(":/images/editpaste.png"), tr("Paste"), this);
PasteAct->setShortcut(Qt::CTRL+Qt::Key_V);
connect(PasteAct, SIGNAL(triggered()), this, SLOT(editPaste()));
editMenu->addAction(PasteAct);

Act = new QAction( tr("Select All"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_A);
connect(Act, SIGNAL(triggered()), this, SLOT(editSelectAll()));
editMenu->addAction(Act);
editMenu->addSeparator();

Act = new QAction( tr("Comment"), this);
Act->setData("Comment");
Act->setShortcut(Qt::CTRL+Qt::Key_T);
connect(Act, SIGNAL(triggered()), this, SLOT(editComment()));
editMenu->addAction(Act);

Act = new QAction( tr("Uncomment"), this);
Act->setData("Uncomment");
Act->setShortcut(Qt::CTRL+Qt::Key_U);
connect(Act, SIGNAL(triggered()), this, SLOT(editUncomment()));
editMenu->addAction(Act);

Act = new QAction( tr("Indent"), this);
Act->setData("Indent");
Act->setShortcut(Qt::CTRL+Qt::Key_Greater);
connect(Act, SIGNAL(triggered()), this, SLOT(editIndent()));
editMenu->addAction(Act);

Act = new QAction( tr("Unindent"), this);
Act->setData("Unindent");
Act->setShortcut(Qt::CTRL+Qt::Key_Less);
connect(Act, SIGNAL(triggered()), this, SLOT(editUnindent()));
editMenu->addAction(Act);
editMenu->addSeparator();

Act = new QAction( tr("Fold"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_BracketLeft);
connect(Act, SIGNAL(triggered()), this, SLOT(foldEnclosing()));
editMenu->addAction(Act);
Act = new QAction( tr("Unfold"), this);
Act->setShortcut(Qt::CTRL+Qt::Key_BracketRight);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldEnclosing()));
editMenu->addAction(Act);

foldMenu=editMenu->addMenu(tr("&Fold All"));
Act = new QAction("Parts", this);
Act->setData(0);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAll()));
foldMenu->addAction(Act);
Act = new QAction("Chapters", this);
Act->setData(1);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAll()));
foldMenu->addAction(Act);
Act = new QAction("Sections", this);
Act->setData(2);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAll()));
foldMenu->addAction(Act);
Act = new QAction("Subsections", this);
Act->setData(3);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAll()));
foldMenu->addAction(Act);
Act = new QAction("Subsubsections", this);
Act->setData(4);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAll()));
foldMenu->addAction(Act);
foldMenu->addSeparator();
Act = new QAction("Parts+", this);
Act->setData(0);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAllUnder()));
foldMenu->addAction(Act);
Act = new QAction("Chapters+", this);
Act->setData(1);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAllUnder()));
foldMenu->addAction(Act);
Act = new QAction("Sections+", this);
Act->setData(2);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAllUnder()));
foldMenu->addAction(Act);
Act = new QAction("Subsections+", this);
Act->setData(3);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAllUnder()));
foldMenu->addAction(Act);
Act = new QAction("Subsubsections+", this);
Act->setData(4);
connect(Act, SIGNAL(triggered()), this, SLOT(foldAllUnder()));
foldMenu->addAction(Act);

unfoldMenu=editMenu->addMenu(tr("&Unfold All"));
Act = new QAction("Parts", this);
Act->setData(0);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAll()));
unfoldMenu->addAction(Act);
Act = new QAction("Chapters", this);
Act->setData(1);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAll()));
unfoldMenu->addAction(Act);
Act = new QAction("Sections", this);
Act->setData(2);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAll()));
unfoldMenu->addAction(Act);
Act = new QAction("Subsection", this);
Act->setData(3);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAll()));
unfoldMenu->addAction(Act);
Act = new QAction("Subsubsection", this);
Act->setData(4);
unfoldMenu->addAction(Act);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAllUnder()));
unfoldMenu->addAction(Act);
unfoldMenu->addSeparator();
Act = new QAction("Parts+", this);
Act->setData(0);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAllUnder()));
unfoldMenu->addAction(Act);
Act = new QAction("Chapters+", this);
Act->setData(1);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAllUnder()));
unfoldMenu->addAction(Act);
Act = new QAction("Sections+", this);
Act->setData(2);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAllUnder()));
unfoldMenu->addAction(Act);
Act = new QAction("Subsections+", this);
Act->setData(3);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAllUnder()));
unfoldMenu->addAction(Act);
Act = new QAction("Subsubsection+", this);
Act->setData(4);
connect(Act, SIGNAL(triggered()), this, SLOT(unfoldAllUnder()));
unfoldMenu->addAction(Act);

editMenu->addSeparator();


Act = new QAction( tr("Find"), this);
Act->setData("Find");
Act->setShortcut(Qt::CTRL+Qt::Key_F);
connect(Act, SIGNAL(triggered()), this, SLOT(editFind()));
editMenu->addAction(Act);

Act = new QAction( tr("FindNext"), this);
Act->setData("FindNext");
Act->setShortcut(Qt::CTRL+Qt::Key_M);
connect(Act, SIGNAL(triggered()), this, SLOT(editFindNext()));
editMenu->addAction(Act);

Act = new QAction( tr("Find In Directory"), this);
Act->setData("Find In Directory");
connect(Act, SIGNAL(triggered()), this, SLOT(editFindInDirectory()));
editMenu->addAction(Act);

Act = new QAction( tr("Replace"), this);
Act->setData("Replace");
Act->setShortcut(Qt::CTRL+Qt::Key_R);
connect(Act, SIGNAL(triggered()), this, SLOT(editReplace()));
editMenu->addAction(Act);

Act = new QAction(getIcon(":/images/goto.png"), tr("Goto Line"), this);
Act->setData("Goto Line");
Act->setShortcut(Qt::CTRL+Qt::Key_G);
connect(Act, SIGNAL(triggered()), this, SLOT(editGotoLine()));
editMenu->addAction(Act);
editMenu->addSeparator();

Act = new QAction(tr("Check Spelling"), this);
Act->setData("Check Spelling");
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_F7);
connect(Act, SIGNAL(triggered()), this, SLOT(editSpell()));
editMenu->addAction(Act);
editMenu->addSeparator();

Act = new QAction(tr("Refresh Structure"), this);
Act->setData("Refresh Structure");
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_F1);
connect(Act, SIGNAL(triggered()), this, SLOT(refreshAll()));
editMenu->addAction(Act);

Act = new QAction(tr("Refresh Bibliography"), this);
Act->setData("Refresh Bibliography");
connect(Act, SIGNAL(triggered()), this, SLOT(UpdateBibliography()));
editMenu->addAction(Act);

toolMenu = menuBar()->addMenu(tr("&Tools"));
Act = new QAction(getIcon(":/images/quick.png"),tr("Quick Build"), this);
Act->setData(Act->text());
Act->setShortcut(Qt::Key_F1);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickBuild()));
toolMenu->addAction(Act);
toolMenu->addSeparator();
Act = new QAction("LaTeX", this);
Act->setData("LaTeX");
Act->setShortcut(Qt::Key_F2);
connect(Act, SIGNAL(triggered()), this, SLOT(Latex()));
toolMenu->addAction(Act);
Act = new QAction(tr("View Dvi"), this);
Act->setData("View Dvi");
Act->setShortcut(Qt::Key_F3);
connect(Act, SIGNAL(triggered()), this, SLOT(ViewDvi()));
toolMenu->addAction(Act);
Act = new QAction("Dvi->PS", this);
Act->setData("Dvi->PS");
Act->setShortcut(Qt::Key_F4);
connect(Act, SIGNAL(triggered()), this, SLOT(DviToPS()));
toolMenu->addAction(Act);
Act = new QAction(tr("View PS"), this);
Act->setData("View PS");
Act->setShortcut(Qt::Key_F5);
connect(Act, SIGNAL(triggered()), this, SLOT(ViewPS()));
toolMenu->addAction(Act);
Act = new QAction("PDFLaTeX", this);
Act->setData("PDFLaTeX");
Act->setShortcut(Qt::Key_F6);
connect(Act, SIGNAL(triggered()), this, SLOT(PDFLatex()));
toolMenu->addAction(Act);
Act = new QAction(tr("View PDF"), this);
Act->setData("View PDF");
Act->setShortcut(Qt::Key_F7);
connect(Act, SIGNAL(triggered()), this, SLOT(ViewPDF()));
toolMenu->addAction(Act);
Act = new QAction("PS->PDF", this);
Act->setData("PS->PDF");
Act->setShortcut(Qt::Key_F8);
connect(Act, SIGNAL(triggered()), this, SLOT(PStoPDF()));
toolMenu->addAction(Act);
Act = new QAction("DVI->PDF", this);
Act->setData("DVI->PDF");
Act->setShortcut(Qt::Key_F9);
connect(Act, SIGNAL(triggered()), this, SLOT(DVItoPDF()));
toolMenu->addAction(Act);
Act = new QAction(tr("View Log"), this);
Act->setData("View Log");
Act->setShortcut(Qt::Key_F10);
connect(Act, SIGNAL(triggered()), this, SLOT(ViewLog()));
toolMenu->addAction(Act);
Act = new QAction("BibTeX", this);
Act->setData("BibTeX");
Act->setShortcut(Qt::Key_F11);
connect(Act, SIGNAL(triggered()), this, SLOT(MakeBib()));
toolMenu->addAction(Act);
Act = new QAction("MakeIndex", this);
Act->setData("MakeIndex");
Act->setShortcut(Qt::Key_F12);
connect(Act, SIGNAL(triggered()), this, SLOT(MakeIndex()));
toolMenu->addAction(Act);
toolMenu->addSeparator();
Act = new QAction("MPost", this);
Act->setData("MPost");
connect(Act, SIGNAL(triggered()), this, SLOT(MetaPost()));
toolMenu->addAction(Act);
Act = new QAction("Asymptote", this);
Act->setData("Asymptote");
connect(Act, SIGNAL(triggered()), this, SLOT(Asymptote()));
toolMenu->addAction(Act);
Act = new QAction("Latexmk", this);
Act->setData("Latexmk");
connect(Act, SIGNAL(triggered()), this, SLOT(LatexMk()));
toolMenu->addAction(Act);
Act = new QAction("R Sweave", this);
Act->setData("R Sweave");
connect(Act, SIGNAL(triggered()), this, SLOT(Sweave()));
toolMenu->addAction(Act);
Act = new QAction("XeLaTeX", this);
Act->setData("XeLaTeX");
toolMenu->addAction(Act);
connect(Act, SIGNAL(triggered()), this, SLOT(Xelatex()));
Act = new QAction("LuaLaTeX", this);
Act->setData("LuaLaTeX");
connect(Act, SIGNAL(triggered()), this, SLOT(Lualatex()));
toolMenu->addAction(Act);
toolMenu->addSeparator();
Act = new QAction(tr("Clean"), this);
Act->setData("Clean");
connect(Act, SIGNAL(triggered()), this, SLOT(CleanAll()));
toolMenu->addAction(Act);
toolMenu->addSeparator();
Act = new QAction(tr("Open Terminal"), this);
Act->setData("Open Terminal");
connect(Act, SIGNAL(triggered()), this, SLOT(OpenTerminal()));
toolMenu->addAction(Act);
Act = new QAction(tr("Export via TeX4ht"), this);
Act->setData("Export via TeX4ht");
connect(Act, SIGNAL(triggered()), this, SLOT(Export()));
toolMenu->addAction(Act);
toolMenu->addSeparator();
Act = new QAction(tr("Convert to unicode"), this);
Act->setData("Convert to unicode");
connect(Act, SIGNAL(triggered()), this, SLOT(ConvertToUnicode()));
toolMenu->addAction(Act);

Act = new QAction(getIcon(":/images/errorprev.png"),tr("Previous LaTeX Error"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(PreviousError()));
Act = new QAction(getIcon(":/images/errornext.png"),tr("Next LaTeX Error"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(NextError()));

latex1Menu = menuBar()->addMenu(tr("&LaTeX"));
Act = new QAction("\\documentclass", this);
Act->setData("\\documentclass[10pt]{}/21/0/\\documentclass[options]{class}");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\usepackage{}", this);
Act->setData("\\usepackage{} /12/0/\\usepackage[options]{pkg}");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("AMS packages", this);
Act->setData("\\usepackage{amsmath}\n\\usepackage{amsfonts}\n\\usepackage{amssymb}\n/0/3/The main American Mathematical Society packages");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\begin{document}", this);
Act->setData("\\begin{document}\n\n\\end{document}/0/1/Text is allowed only between \\begin{document} and \\end{document}.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\author{}", this);
Act->setData("\\author{}/8/0/\\author{names}\nThe \\author command declares the author(s), where names is a list of authors separated by \\and commands.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\title{}", this);
Act->setData("\\title{}/7/0/\\title{text}\nThe \\title command declares text to be the title.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\maketitle", this);
Act->setData("\\maketitle/10/0/This command generates a title on a separate title page\n- except in the article class, where the title normally goes at the top of the first page.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\tableofcontents", this);
Act->setData("\\tableofcontents/16/0/Put this command where you want the table of contents to go");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);

latex11Menu=latex1Menu->addMenu(tr("&Sectioning"));
Act = new QAction("\\part", this);
Act->setData("\\part");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);
Act = new QAction("\\chapter", this);
Act->setData("\\chapter");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);
Act = new QAction("\\section", this);
Act->setData("\\section");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);
Act = new QAction("\\subsection", this);
Act->setData("\\subsection");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);
Act = new QAction("\\subsubsection", this);
Act->setData("\\subsubsection");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);
Act = new QAction("\\paragraph", this);
Act->setData("\\paragraph");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);
Act = new QAction("\\subparagraph", this);
Act->setData("\\subparagraph");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertStruct()));
latex11Menu->addAction(Act);

latex12Menu=latex1Menu->addMenu(tr("&Environment"));
Act = new QAction(getIcon(":/images/text_center.png"),"\\begin{center} [selection]", this);
Act->setData("\\begin{center}\n/\n\\end{center}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/text_left.png"),"\\begin{flushleft} [selection]", this);
Act->setData("\\begin{flushleft}\n/\n\\end{flushleft}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/text_right.png"),"\\begin{flushright}  [selection]", this);
Act->setData("\\begin{flushright}\n/\n\\end{flushright}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{quote}  [selection]", this);
Act->setData("\\begin{quote}\n/\n\\end{quote}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{quotation}  [selection]", this);
Act->setData("\\begin{quotation}\n/\n\\end{quotation}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{verse}  [selection]", this);
Act->setData("\\begin{verse}\n/\n\\end{verse}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{verbatim}  [selection]", this);
Act->setData("\\begin{verbatim}\n/\n\\end{verbatim}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{table}  [selection]", this);
Act->setData("\\begin{table}\n/\n\\caption{}\n\\end{table}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{figure}  [selection]", this);
Act->setData("\\begin{figure}\n/\n\\caption{}\n\\end{figure}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{titlepage}  [selection]", this);
Act->setData("\\begin{titlepage}\n/\n\\end{titlepage}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);
Act = new QAction("\\begin{minipage}  [selection]", this);
Act->setData("\\begin{minipage}{}\n/\n\\end{minipage}/0/1");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex12Menu->addAction(Act);

latex13Menu=latex1Menu->addMenu(tr("&List Environment"));
Act = new QAction(getIcon(":/images/itemize.png"),"\\begin{itemize}", this);
Act->setData("\\begin{itemize}\n\\item \n\\end{itemize}/6/1/The itemize environment produces a 'bulleted' list.\nEach item of an itemized list begins with an \\item command.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex13Menu->addAction(Act);
Act = new QAction(getIcon(":/images/enumerate.png"),"\\begin{enumerate}", this);
Act->setData("\\begin{enumerate}\n\\item \n\\end{enumerate}/6/1/The enumerate environment produces a numbered list.\nEach item of an enumerated list begins with an \\item command.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex13Menu->addAction(Act);
Act = new QAction("\\begin{description}", this);
Act->setData("\\begin{description}\n\\item[]\n\\end{description}/6/1/The description environment is used to make labelled lists.\nEach item of the list begins with an \\item[label] command.\n");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex13Menu->addAction(Act);
Act = new QAction("\\begin{list}", this);
Act->setData("\\begin{list}{}{}\n\\item \n\\end{list}/13/0/\\begin{list}{label}{spacing}\nThe {label} argument is a piece of text that is inserted in a box to form the label.\nThe {spacing} argument contains commands to change the spacing parameters for the list.\nEach item of the list begins with an \\item command.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex13Menu->addAction(Act);

Act = new QAction(getIcon(":/images/item.png"),"\\item", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_I);
Act->setData("\\item/5/0/\\item[label] Hello");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex13Menu->addAction(Act);

latex14Menu=latex1Menu->addMenu(tr("Font St&yles"));
Act = new QAction(getIcon(":/images/text_italic.png"),"\\textit - Italics  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::Key_I);
Act->setData("\\textit{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);
Act = new QAction("\\textsl - Slanted  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_S);
Act->setData("\\textsl{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);
Act = new QAction(getIcon(":/images/text_bold.png"),"\\textbf - Boldface  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::Key_B);
Act->setData("\\textbf{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);
Act = new QAction("\\texttt - Typewriter  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_T);
Act->setData("\\texttt{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);
Act = new QAction("\\textsc - Small caps  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_C);
Act->setData("\\textsc{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);
Act = new QAction("\\textsf - Sans Serif  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_A);
Act->setData("\\textsf{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);
Act = new QAction(getIcon(":/images/text_emphasis.png"),"\\emph - Emphasis  [selection]", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_E);
Act->setData("\\emph{/}/6/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex14Menu->addAction(Act);

latex15Menu=latex1Menu->addMenu(tr("&Tabular Environment"));
Act = new QAction("\\begin{tabbing}", this);
Act->setData("\\begin{tabbing}\n\n\\end{tabbing}/0/1/\\begin{tabbing}\ntext \\= more text \\= still more text \\= last text \\\\\nsecond row \\>  \\> more \\\\\n\\end{tabbing}");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex15Menu->addAction(Act);
Act = new QAction("\\begin{tabular}", this);
Act->setData("\\begin{tabular}{}\n\n\\end{tabular}/16/0/\\begin{tabular}[pos]{cols}\ncolumn 1 entry & column 2 entry ... & column n entry \\\\\n...\n\\end{tabular}");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex15Menu->addAction(Act);
Act = new QAction("\\multicolumn", this);
Act->setData("\\multicolumn{}{}{} /13/0/\\multicolumn{cols}{pos}{text}\ncol, specifies the number of columns to span.\npos specifies the formatting of the entry: c for centred, l for flushleft, r for flushright.\ntext specifies what text is to make up the entry.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex15Menu->addAction(Act);
Act = new QAction("\\hline", this);
Act->setData("\\hline /7/0/The \\hline command draws a horizontal line the width of the table.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex15Menu->addAction(Act);
Act = new QAction("\\vline", this);
Act->setData("\\vline /7/0/The \\vline command draws a vertical line extending the full height and depth of its row.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex15Menu->addAction(Act);
Act = new QAction("\\cline", this);
Act->setData("\\cline{-} /7/0/The \\cline{i-j} command draws horizontal lines across the columns specified, beginning in column i and ending in column j");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex15Menu->addAction(Act);

latex16Menu=latex1Menu->addMenu(tr("S&pacing"));
Act = new QAction("\\newpage", this);
Act->setData("\\newpage /9/0/The \\newpage command ends the current page");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex16Menu->addAction(Act);
Act = new QAction("\\linebreak", this);
Act->setData("\\linebreak /11/0/The \\linebreak command tells LaTeX to break the current line at the point of the command.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex16Menu->addAction(Act);
Act = new QAction("\\pagebreak", this);
Act->setData("\\pagebreak /11/0/The \\pagebreak command tells LaTeX to break the current page at the point of the command.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex16Menu->addAction(Act);
Act = new QAction("\\bigskip", this);
Act->setData("\\bigskip /9/0/The \\bigskip command adds a 'big' vertical space.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex16Menu->addAction(Act);
Act = new QAction("\\medskip", this);
Act->setData("\\medskip /9/0/The \\medskip command adds a 'medium' vertical space.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex16Menu->addAction(Act);
Act = new QAction(getIcon(":/images/newline.png"),"New line", this);
Act->setData("\\\\\n/0/1/The \\newline command breaks the line right where it is.");
Act->setShortcut(Qt::CTRL+Qt::Key_Return);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex16Menu->addAction(Act);

latex17Menu=latex1Menu->addMenu(tr("International &Accents"));
Act = new QAction(getIcon(":/images/accent1.png"),"\\'{}", this);
Act->setData("\\'{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent2.png"),"\\`{}", this);
Act->setData("\\`{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent3.png"),"\\^{}", this);
Act->setData("\\^{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent4.png"),"\\\"{}", this);
Act->setData("\\\"{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent5.png"),"\\~{}", this);
Act->setData("\\~{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent6.png"),"\\={}", this);
Act->setData("\\={}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent7.png"),"\\.{}", this);
Act->setData("\\.{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent8.png"),"\\v{}", this);
Act->setData("\\v{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent9.png"),"\\u{}", this);
Act->setData("\\u{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);
Act = new QAction(getIcon(":/images/accent10.png"),"\\H{}", this);
Act->setData("\\H{}/3/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex17Menu->addAction(Act);

latex18Menu=latex1Menu->addMenu(tr("International &Quotes"));
Act = new QAction("French Quotes  [selection]", this);
Act->setData("\\og / \\fg{}/4/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex18Menu->addAction(Act);
Act = new QAction("German Quotes  [selection]", this);
Act->setData("\\glqq /\\grqq/6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex18Menu->addAction(Act);
Act = new QAction("Polish Quotes  [selection]", this);
Act->setData("\\quotedblbase /\\textquotedblright/14/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
latex18Menu->addAction(Act);

Act = new QAction("\\includegraphics{file}", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertImage()));
latex1Menu->addAction(Act);
Act = new QAction("\\include{file}", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertInclude()));
latex1Menu->addAction(Act);
Act = new QAction("\\input{file}", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertInput()));
latex1Menu->addAction(Act);
latex1Menu->addSeparator();

Act = new QAction("\\label{}", this);
Act->setData("\\label{} /7/0/\\label{key}");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\cite{}", this);
Act->setData("\\cite{} /6/0/\\cite{ref} :\nThis command generates an in-text citation to the reference associated with the ref entry in the bib file");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\footnote{}", this);
Act->setData("\\footnote{} /10/0/\\footnote[number]{text}\nThe \\footnote command places the numbered footnote text at the bottom of the current page.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\bibliographystyle{}", this);
Act->setData("\\bibliographystyle{} /19/0/The argument to \\bibliographystyle refers to a file style.bst, which defines how your citations will look");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
latex1Menu->addAction(Act);
Act = new QAction("\\bibliography{}", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib()));
latex1Menu->addAction(Act);

Act = new QAction("\\addbibresource{}", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex()));
latex1Menu->addAction(Act);

math1Menu = menuBar()->addMenu(tr("&Math"));
Act = new QAction(tr("Inline math mode $...$"), this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_M);
Act->setData("$"+QString(0x2022)+"$/2/0/The math environment can be used in both paragraph and LR mode");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction(tr("Display math mode \\[...\\]"), this);
Act->setShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_M);
Act->setData("\\["+QString(0x2022)+"\\]/3/0/The displaymath environment can be used only in paragraph mode");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction(tr("Numbered equations \\begin{equation}"), this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_N);
Act->setData("\\begin{equation}\n\n\\end{equation}/0/1/The equation environment centres your equation on the page and places the equation number in the right margin.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\begin{eqnarray}", this);
Act->setData("\\begin{eqnarray}\n\n\\end{eqnarray}/0/1/\\begin{eqnarray}\nmath formula 1 \\\\\n\\end{eqnarray}\nThe eqnarray environment is used to display a sequence of equations or inequalities.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\begin{align} (AMS)", this);
Act->setData("\\begin{align}\n\n\\end{align}/0/1/\\begin{align}\nmath formula 1 \\\\\n\\end{align}\nThe AMS align environment is used to display a sequence of equations or inequalities.");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("_{} - subscript", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_D);
Act->setData("_{}/2/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("^{} - superscript", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_U);
Act->setData("^{}/2/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\frac{}{}", this);
Act->setShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_F);
Act->setData("\\frac{}{}/6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\dfrac{}{}", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_F);
Act->setData("\\dfrac{}{}/7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\sqrt{}", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_Q);
Act->setData("\\sqrt{}/6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\left", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_L);
Act->setData("\\left /6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\right", this);
Act->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_R);
Act->setData("\\right /7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);
Act = new QAction("\\begin{array}", this);
Act->setData("\\begin{array}{}\n\n\\end{array}/14/0/\\begin{array}{col1col2...coln}\ncolumn 1 entry & column 2 entry ... & column n entry \\\\\n...\n\\end{array}");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math1Menu->addAction(Act);

math14Menu=math1Menu->addMenu(tr("Math &Functions"));
Act = new QAction("\\arccos", this);
Act->setData("\\arccos /8/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\arcsin", this);
Act->setData("\\arcsin /8/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\arctan", this);
Act->setData("\\arctan /8/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\cos", this);
Act->setData("\\cos /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\cosh", this);
Act->setData("\\cosh /6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\cot", this);
Act->setData("\\cot /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\coth", this);
Act->setData("\\coth /6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\csc", this);
Act->setData("\\csc /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\deg", this);
Act->setData("\\deg /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\det", this);
Act->setData("\\det /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\dim", this);
Act->setData("\\dim /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\exp", this);
Act->setData("\\exp /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\gcd", this);
Act->setData("\\gcd /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\hom", this);
Act->setData("\\hom /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\inf", this);
Act->setData("\\inf /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\ker", this);
Act->setData("\\ker /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\lg", this);
Act->setData("\\lg /4/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\lim", this);
Act->setData("\\lim /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\liminf", this);
Act->setData("\\liminf /8/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\limsup", this);
Act->setData("\\limsup /8/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\ln", this);
Act->setData("\\ln /4/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\log", this);
Act->setData("\\log /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\max", this);
Act->setData("\\max /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\min", this);
Act->setData("\\min /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\sec", this);
Act->setData("\\sec /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\sin", this);
Act->setData("\\sin /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\sinh", this);
Act->setData("\\sinh /6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\sup", this);
Act->setData("\\sup /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\tan", this);
Act->setData("\\tan /5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);
Act = new QAction("\\tanh", this);
Act->setData("\\tanh /6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math14Menu->addAction(Act);

math11Menu=math1Menu->addMenu(tr("Math Font St&yles"));
Act = new QAction("\\mathrm{}  [selection]", this);
Act->setData("\\mathrm{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathit{}  [selection]", this);
Act->setData("\\mathit{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathbf{}  [selection]", this);
Act->setData("\\mathbf{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathsf{}  [selection]", this);
Act->setData("\\mathsf{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathtt{}  [selection]", this);
Act->setData("\\mathtt{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathcal{}  [selection]", this);
Act->setData("\\mathcal{/}/9/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathbb{}  [selection]", this);
Act->setData("\\mathbb{/}/8/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);
Act = new QAction("\\mathfrak{}  [selection]", this);
Act->setData("\\mathfrak{/}/10/0");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertWithSelectionFromAction()));
math11Menu->addAction(Act);

math12Menu=math1Menu->addMenu(tr("Math &Accents"));
Act = new QAction(getIcon(":/images/acute.png"),"\\acute{}", this);
Act->setData("\\acute{}/7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/grave.png"),"\\grave{}", this);
Act->setData("\\grave{}/7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/tilde.png"),"\\tilde{}", this);
Act->setData("\\tilde{}/7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/bar.png"),"\\bar{}", this);
Act->setData("\\bar{}/5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/vec.png"),"\\vec{}", this);
Act->setData("\\vec{}/5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/hat.png"),"\\hat{}", this);
Act->setData("\\hat{}/5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/check.png"),"\\check{}", this);
Act->setData("\\check{}/7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/breve.png"),"\\breve{}", this);
Act->setData("\\breve{}/7/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/dot.png"),"\\dot{}", this);
Act->setData("\\dot{}/5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);
Act = new QAction(getIcon(":/images/ddot.png"),"\\ddot{}", this);
Act->setData("\\ddot{}/6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math12Menu->addAction(Act);

math13Menu=math1Menu->addMenu(tr("Math S&paces"));
Act = new QAction("small", this);
Act->setData("\\,/2/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math13Menu->addAction(Act);
Act = new QAction("medium", this);
Act->setData("\\:/2/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math13Menu->addAction(Act);
Act = new QAction("large", this);
Act->setData("\\;/2/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math13Menu->addAction(Act);
Act = new QAction("\\quad", this);
Act->setData("\\quad/5/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math13Menu->addAction(Act);
Act = new QAction("\\qquad", this);
Act->setData("\\qquad/6/0/ ");
connect(Act, SIGNAL(triggered()), this, SLOT(InsertFromAction()));
math13Menu->addAction(Act);

wizardMenu = menuBar()->addMenu(tr("&Wizard"));
Act = new QAction(tr("Quick Start"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickDocument()));
wizardMenu->addAction(Act);
Act = new QAction(tr("Quick Beamer Presentation"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickBeamer()));
wizardMenu->addAction(Act);
Act = new QAction(tr("Quick Xelatex Document"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickXelatex()));
wizardMenu->addAction(Act);
Act = new QAction(tr("Quick Letter"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickLetter()));
wizardMenu->addAction(Act);
wizardMenu->addSeparator();
Act = new QAction(tr("Quick Tabular"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickTabular()));
wizardMenu->addAction(Act);
Act = new QAction(tr("Quick Tabbing"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickTabbing()));
wizardMenu->addAction(Act);
Act = new QAction(tr("Quick Array"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(QuickArray()));
wizardMenu->addAction(Act);

bibMenu = menuBar()->addMenu(tr("&Bibliography"));

bibtexMenu=bibMenu->addMenu("Bibtex");
Act = new QAction("Article in Journal", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib1()));
bibtexMenu->addAction(Act);
Act = new QAction("Article in Conference Proceedings", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib2()));
bibtexMenu->addAction(Act);
Act = new QAction("Article in a collection", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib3()));
bibtexMenu->addAction(Act);
Act = new QAction("Chapter or Pages in a Book", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib4()));
bibtexMenu->addAction(Act);
Act = new QAction("Conference Proceedings", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib5()));
bibtexMenu->addAction(Act);
Act = new QAction("Book", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib6()));
bibtexMenu->addAction(Act);
Act = new QAction("Booklet", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib7()));
bibtexMenu->addAction(Act);
Act = new QAction("PhD. Thesis", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib8()));
bibtexMenu->addAction(Act);
Act = new QAction("Master's Thesis", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib9()));
bibtexMenu->addAction(Act);
Act = new QAction("Technical Report", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib10()));
bibtexMenu->addAction(Act);
Act = new QAction("Technical Manual", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib11()));
bibtexMenu->addAction(Act);
Act = new QAction("Unpublished", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib12()));
bibtexMenu->addAction(Act);
Act = new QAction("Miscellaneous", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBib13()));
bibtexMenu->addAction(Act);


biblatexMenu=bibMenu->addMenu("Biblatex");
Act = new QAction("Article in Journal", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex1()));
biblatexMenu->addAction(Act);
Act = new QAction("Single-volume book", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex2()));
biblatexMenu->addAction(Act);
Act = new QAction("Multi-volume book", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex3()));
biblatexMenu->addAction(Act);
Act = new QAction("Part of a book", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex4()));
biblatexMenu->addAction(Act);
Act = new QAction("Booklet", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex5()));
biblatexMenu->addAction(Act);
Act = new QAction("Single-volume collection", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex6()));
biblatexMenu->addAction(Act);
Act = new QAction("Multi-volume collection", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex7()));
biblatexMenu->addAction(Act);
Act = new QAction("Part of a collection", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex8()));
biblatexMenu->addAction(Act);
Act = new QAction("Technical documentation", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex9()));
biblatexMenu->addAction(Act);
Act = new QAction("Miscellaneous", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex10()));
biblatexMenu->addAction(Act);
Act = new QAction("Online resource", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex11()));
biblatexMenu->addAction(Act);
Act = new QAction("Issue of a periodical", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex12()));
biblatexMenu->addAction(Act);
Act = new QAction("Single-volume conference proceedings", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex13()));
biblatexMenu->addAction(Act);
Act = new QAction("Multi-volume conference proceedings", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex14()));
biblatexMenu->addAction(Act);
Act = new QAction("Article in conference proceedings", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex15()));
biblatexMenu->addAction(Act);
Act = new QAction("Technical report", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex16()));
biblatexMenu->addAction(Act);
Act = new QAction("Thesis", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex17()));
biblatexMenu->addAction(Act);
Act = new QAction("Patent", this);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertBibLatex18()));
biblatexMenu->addAction(Act);

bibMenu->addSeparator();
Act = new QAction(tr("Clean"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(CleanBib()));
bibMenu->addAction(Act);


user1Menu = menuBar()->addMenu(tr("&User"));
user11Menu=user1Menu->addMenu(tr("User &Tags"));
Act = new QAction("1: "+UserMenuName[0], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F1);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag1()));
user11Menu->addAction(Act);
Act = new QAction("2: "+UserMenuName[1], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F2);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag2()));
user11Menu->addAction(Act);
Act = new QAction("3: "+UserMenuName[2], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F3);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag3()));
user11Menu->addAction(Act);
Act = new QAction("4: "+UserMenuName[3], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F4);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag4()));
user11Menu->addAction(Act);
Act = new QAction("5: "+UserMenuName[4], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F5);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag5()));
user11Menu->addAction(Act);
Act = new QAction("6: "+UserMenuName[5], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F6);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag6()));
user11Menu->addAction(Act);
Act = new QAction("7: "+UserMenuName[6], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F7);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag7()));
user11Menu->addAction(Act);
Act = new QAction("8: "+UserMenuName[7], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F8);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag8()));
user11Menu->addAction(Act);
Act = new QAction("9: "+UserMenuName[8], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F9);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag9()));
user11Menu->addAction(Act);
Act = new QAction("10: "+UserMenuName[9], this);
Act->setShortcut(Qt::SHIFT+Qt::Key_F10);
connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag10()));
user11Menu->addAction(Act);
user11Menu->addSeparator();
Act = new QAction(tr("Edit User &Tags"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(EditUserMenu()));
user11Menu->addAction(Act);
user12Menu=user1Menu->addMenu(tr("User &Commands"));
Act = new QAction("1: "+UserToolName[0], this);
Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F1);
connect(Act, SIGNAL(triggered()), this, SLOT(UserTool1()));
user12Menu->addAction(Act);
Act = new QAction("2: "+UserToolName[1], this);
Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F2);
connect(Act, SIGNAL(triggered()), this, SLOT(UserTool2()));
user12Menu->addAction(Act);
Act = new QAction("3: "+UserToolName[2], this);
Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F3);
connect(Act, SIGNAL(triggered()), this, SLOT(UserTool3()));
user12Menu->addAction(Act);
Act = new QAction("4: "+UserToolName[3], this);
Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F4);
connect(Act, SIGNAL(triggered()), this, SLOT(UserTool4()));
user12Menu->addAction(Act);
Act = new QAction("5: "+UserToolName[4], this);
Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F5);
connect(Act, SIGNAL(triggered()), this, SLOT(UserTool5()));
user12Menu->addAction(Act);
user12Menu->addSeparator();
Act = new QAction(tr("Edit User &Commands"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(EditUserTool()));
user12Menu->addAction(Act);

Act = new QAction(tr("Customize Completion"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(EditUserCompletion()));
user1Menu->addAction(Act);

user1Menu->addSeparator();
scriptMenu=user1Menu->addMenu(tr("Run script"));
for (int i=0; i < scriptList.count(); i++)
	{
	Act = new QAction(scriptList.at(i), this);
	Act->setData(scriptList.at(i)+".tms");
	connect(Act, SIGNAL(triggered()), this, SLOT(editRunFurnishedScript()));
	scriptMenu->addAction(Act);
	}



Act = new QAction(tr("Other script"), this);
Act->setData("Other script");
connect(Act, SIGNAL(triggered()), this, SLOT(editRunScript()));
scriptMenu->addAction(Act);

viewMenu = menuBar()->addMenu(tr("&View"));
NextDocAct = new QAction(tr("Next Document"), this);
NextDocAct->setData("Next");
NextDocAct->setShortcut(Qt::ALT+Qt::Key_PageDown);
connect(NextDocAct, SIGNAL(triggered()), this, SLOT(gotoNextDocument()));
viewMenu->addAction(NextDocAct);
PrevDocAct = new QAction(tr("Previous Document"), this);
PrevDocAct->setData("Prev");
PrevDocAct->setShortcut(Qt::ALT+Qt::Key_PageUp);
connect(PrevDocAct, SIGNAL(triggered()), this, SLOT(gotoPrevDocument()));
viewMenu->addAction(PrevDocAct);
viewMenu->addSeparator();

ViewStructurePanelAct = new QAction(tr("Structure"), this);
ViewStructurePanelAct->setCheckable(true);
connect(ViewStructurePanelAct, SIGNAL(triggered()), this, SLOT(ToggleStructurePanel()));
ViewStructurePanelAct->setChecked(showstructview);
viewMenu->addAction(ViewStructurePanelAct);
ViewLogPanelAct = new QAction(tr("Messages / Log File"), this);
ViewLogPanelAct->setCheckable(true);
ViewLogPanelAct->setChecked(showoutputview);
connect(ViewLogPanelAct, SIGNAL(triggered()), this, SLOT(ToggleLogPanel()));
viewMenu->addAction(ViewLogPanelAct);

ViewPdfPanelAct = new QAction(tr("Pdf Viewer"), this);
ViewPdfPanelAct->setCheckable(true);
ViewPdfPanelAct->setChecked(showpdfview);
connect(ViewPdfPanelAct, SIGNAL(triggered()), this, SLOT(TogglePdfPanel()));
if (embedinternalpdf) 
  {
  viewMenu->addAction(ViewPdfPanelAct);
  if (builtinpdfview) ViewPdfPanelAct->setEnabled(true);
  else ViewPdfPanelAct->setEnabled(false);
  }

ViewSourcePanelAct = new QAction("Source Viewer", this);
ViewSourcePanelAct->setCheckable(true);
ViewSourcePanelAct->setChecked(showsourceview);
connect(ViewSourcePanelAct, SIGNAL(triggered()), this, SLOT(ToggleSourcePanel()));
viewMenu->addAction(ViewSourcePanelAct);

ViewOpenedFilesPanelAct= new QAction(tr("List of opened files"), this);
ViewOpenedFilesPanelAct->setCheckable(true);
ViewOpenedFilesPanelAct->setChecked(showfilesview);
connect(ViewOpenedFilesPanelAct, SIGNAL(triggered()), this, SLOT(ToggleFilesPanel()));
viewMenu->addAction(ViewOpenedFilesPanelAct);
viewMenu->addSeparator();

FullScreenAct = new QAction(tr("Full Screen"), this);
FullScreenAct->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_F11);
FullScreenAct->setCheckable(true);
FullScreenAct->setChecked(false);
connect(FullScreenAct, SIGNAL(triggered()), this, SLOT(ToggleFullScreen()));
viewMenu->addAction(FullScreenAct);


optionsMenu = menuBar()->addMenu(tr("&Options"));
Act = new QAction(getIcon(":/images/configure.png"), tr("Configure Texmaker"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(GeneralOptions()));
optionsMenu->addAction(Act);
optionsMenu->addSeparator();
ToggleAct = new QAction(tr("Define Current Document as 'Master Document'"), this);
connect(ToggleAct, SIGNAL(triggered()), this, SLOT(ToggleMode()));
optionsMenu->addAction(ToggleAct);
optionsMenu->addSeparator();
// appearanceMenu=optionsMenu->addMenu(tr("Interface Appearance"));
// appearanceGroup = new QActionGroup(this);
// Act = new QAction("Modern", this);
// Act->setCheckable(true);
// connect(Act, SIGNAL(triggered()), this, SLOT(updateAppearance()));
// appearanceGroup->addAction(Act);
// if (modern_style) Act->setChecked(true);
// appearanceMenu->addAction(Act);
// Act = new QAction("Classic", this);
// Act->setCheckable(true);
// connect(Act, SIGNAL(triggered()), this, SLOT(updateAppearance()));
// appearanceGroup->addAction(Act);
// if (!modern_style) Act->setChecked(true);
// appearanceMenu->addAction(Act);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
Act = new QAction(tr("Change Interface Font"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(SetInterfaceFont()));
optionsMenu->addAction(Act);
#endif
optionsMenu->addSeparator();
translationMenu=optionsMenu->addMenu(tr("Interface Language"));
translationGroup = new QActionGroup(this);
QString currentTranslation=TexmakerApp::instance()->language;
for (int i=0; i < translationList.count(); i++)
	{
	Act = new QAction(translationList.at(i), this);
	Act->setCheckable(true);
	connect(Act, SIGNAL(triggered()), this, SLOT(updateTranslation()));
	translationGroup->addAction(Act);
	if ((currentTranslation==translationList.at(i)) || (currentTranslation.left(2)==translationList.at(i)) )
	    {
	    Act->setChecked(true);
	    }
	translationMenu->addAction(Act);
	}
optionsMenu->addSeparator();
#if defined(Q_OS_MAC)
settingsMenu=optionsMenu->addMenu(tr("Manage Settings File"));
#else
settingsMenu=optionsMenu->addMenu(tr("Settings File"));
#endif
Act = new QAction( tr("Reset Settings"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(DeleteSettings()));
settingsMenu->addAction(Act);
Act = new QAction( tr("Save a copy of the settings file"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(CopySettings()));
settingsMenu->addAction(Act);
Act = new QAction( tr("Replace the settings file by a new one"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(ReplaceSettings()));
settingsMenu->addAction(Act);
	
helpMenu = menuBar()->addMenu(tr("&Help"));
if (gtkEnv) Act = new QAction(QIcon::fromTheme("help-contents", QIcon(":/images/help.png")), tr("LaTeX Reference"), this);
else Act = new QAction(getIcon(":/images/help.png"), tr("LaTeX Reference"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(LatexHelp()));
helpMenu->addAction(Act);
if (gtkEnv) Act = new QAction(QIcon::fromTheme("help-contents", QIcon(":/images/help.png")), tr("User Manual"), this);
else Act = new QAction(getIcon(":/images/help.png"), tr("User Manual"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(UserManualHelp()));
helpMenu->addAction(Act);
helpMenu->addSeparator();
if (gtkEnv) Act = new QAction(QIcon::fromTheme("help-contents", QIcon(":/images/help.png")), "TexDoc [selection]", this);
else Act = new QAction(getIcon(":/images/help.png"), "TexDoc [selection]", this);
connect(Act, SIGNAL(triggered()), this, SLOT(TexDocHelp()));
helpMenu->addAction(Act);


// QString locale = QString(QLocale::system().name()).left(2);
// if (locale=="fr")
// {
// if (gtkEnv) Act = new QAction(QIcon::fromTheme("help-contents", QIcon(":/images/help.png")), QString::fromUtf8("Documentation LaTeX/Texmaker en ligne"), this); 
// else Act = new QAction(getIcon(":/images/help.png"), QString::fromUtf8("Documentation LaTeX/Texmaker en ligne"), this);
// connect(Act, SIGNAL(triggered()), this, SLOT(Docufrlatex()));
// helpMenu->addAction(Act);  
// }
// else
// {
if (gtkEnv) Act = new QAction(QIcon::fromTheme("help-contents", QIcon(":/images/help.png")), QString::fromUtf8("LaTeX wikibook"), this); 
else Act = new QAction(getIcon(":/images/help.png"), QString::fromUtf8("LaTeX wikibook"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(Doculatex()));
helpMenu->addAction(Act);  
//}  

helpMenu->addSeparator();
Act = new QAction( tr("Check for Update"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(CheckVersion()));
helpMenu->addAction(Act);

helpMenu->addSeparator();
Act = new QAction(getIcon(":/images/appicon.png"), tr("About Texmaker"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(HelpAbout()));
helpMenu->addAction(Act);


QList<QAction *> listaction;
KeysMap::Iterator its;
bool hasNextPrev=false;
bool hasQuote=false;
bool hasTools=false;
bool hasEdit=false;

if (shortcuts.isEmpty())
	{
	actionstext.clear();
	listaction << toolMenu->actions();
	listaction << editMenu->actions();
	listaction << NextDocAct << PrevDocAct;
	listaction << latex1Menu->actions();
	listaction << latex11Menu->actions();
	listaction << latex12Menu->actions();
	listaction << latex13Menu->actions();
	listaction << latex14Menu->actions();
	listaction << latex15Menu->actions();
	listaction << latex16Menu->actions();
	listaction << latex17Menu->actions();
	listaction << latex18Menu->actions();
	listaction << math1Menu->actions();
	listaction << math11Menu->actions();
	listaction << math12Menu->actions();
	listaction << math13Menu->actions();
	listaction << math14Menu->actions();
	QListIterator<QAction*> iterator(listaction);
	while ( iterator.hasNext() )
		{
		QAction *action = iterator.next();
		if (action && (!action->menu()) && (!action->data().toString().isEmpty())) 
			{
			if (action->shortcut().isEmpty()) shortcuts.insert(action->data().toString(),"none");
			else shortcuts.insert(action->data().toString(),action->shortcut().toString(QKeySequence::PortableText));
			actionstext.insert(action->data().toString(),action->text());
			}
		}
	}
else 
  {
  for( its = shortcuts.begin(); its != shortcuts.end(); ++its )
      {
      if (its.key()=="Next") hasNextPrev=true;
      if (its.key().contains("\\glqq / \\grqq")) hasQuote=true;
      if (its.key()=="LaTeX") hasTools=true;
      if (its.key()=="Comment") hasEdit=true;
      }
  if (!hasNextPrev) 
      {
      shortcuts.insert(NextDocAct->data().toString(),NextDocAct->shortcut().toString(QKeySequence::PortableText));
      actionstext.insert(NextDocAct->data().toString(),NextDocAct->text());
      shortcuts.insert(PrevDocAct->data().toString(),PrevDocAct->shortcut().toString(QKeySequence::PortableText));
      actionstext.insert(PrevDocAct->data().toString(),PrevDocAct->text());
      }
  if (!hasQuote) 
      {
      shortcuts.insert(latex18Menu->actions().at(0)->data().toString(),latex18Menu->actions().at(0)->shortcut().toString(QKeySequence::PortableText));
      actionstext.insert(latex18Menu->actions().at(0)->data().toString(),latex18Menu->actions().at(0)->text());
      shortcuts.insert(latex18Menu->actions().at(1)->data().toString(),latex18Menu->actions().at(1)->shortcut().toString(QKeySequence::PortableText));
      actionstext.insert(latex18Menu->actions().at(1)->data().toString(),latex18Menu->actions().at(1)->text());
      }
  if (!hasTools) 
      {
	listaction << toolMenu->actions();
	QListIterator<QAction*> iterator(listaction);
	while ( iterator.hasNext() )
		{
		QAction *action = iterator.next();
		if (action && (!action->menu()) && (!action->data().toString().isEmpty())) 
			{
			if (action->shortcut().isEmpty()) shortcuts.insert(action->data().toString(),"none");
			else shortcuts.insert(action->data().toString(),action->shortcut().toString(QKeySequence::PortableText));
			actionstext.insert(action->data().toString(),action->text());
			}
		}
      }
  listaction.clear();
  if (!hasEdit) 
      {
	listaction << editMenu->actions();
	QListIterator<QAction*> iterator(listaction);
	while ( iterator.hasNext() )
		{
		QAction *action = iterator.next();
		if (action && (!action->menu()) && (!action->data().toString().isEmpty())) 
			{
			if (action->shortcut().isEmpty()) shortcuts.insert(action->data().toString(),"none");
			else shortcuts.insert(action->data().toString(),action->shortcut().toString(QKeySequence::PortableText));
			actionstext.insert(action->data().toString(),action->text());
			}
		}
      }
  ModifyShortcuts();
  }
}

void Texmaker::setupToolBars()
{
QAction *Act;
QStringList list;
bool gtkEnv=false;
//bool gtkEnv=gtkSession();
//file
fileToolBar = addToolBar("File ToolBar");
fileToolBar->setObjectName("File");
fileToolBar->setStyleSheet(Theme::viewportDarkStyleSheet);

if (gtkEnv) Act = new QAction(QIcon::fromTheme("document-new", QIcon(":/images/filenew.png")), tr("New"), this);
else Act = new QAction(getIcon(":/images/filenew.png"), tr("New"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileNew()));
fileToolBar->addAction(Act);

if (gtkEnv) Act = new QAction(QIcon::fromTheme("document-open", QIcon(":/images/fileopen.png")), tr("Open"), this);
else Act = new QAction(getIcon(":/images/fileopen.png"), tr("Open"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(fileOpen()));
fileToolBar->addAction(Act);

//Act = new QAction(QIcon(":/images/filesave.png"), tr("Save"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(fileSave()));
fileToolBar->addAction(SaveAct);

//Act = new QAction(QIcon(":/images/fileclose.png"), tr("Close"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(fileClose()));
//fileToolBar->addAction(Act);

//edit
editToolBar = addToolBar("Edit ToolBar");
editToolBar->setObjectName("Edit");
editToolBar->setStyleSheet(Theme::viewportDarkStyleSheet);

//Act = new QAction(QIcon(":/images/undo.png"), tr("Undo"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(editUndo()));
editToolBar->addAction(UndoAct);

//Act = new QAction(QIcon(":/images/redo.png"), tr("Redo"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(editRedo()));
editToolBar->addAction(RedoAct);

//Act = new QAction(QIcon(":/images/editcopy.png"), tr("Copy"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(editCopy()));
editToolBar->addAction(CopyAct);

//Act = new QAction(QIcon(":/images/editcut.png"), tr("Cut"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(editCut()));
editToolBar->addAction(CutAct);

//Act = new QAction(QIcon(":/images/editpaste.png"), tr("Paste"), this);
//connect(Act, SIGNAL(triggered()), this, SLOT(editPaste()));
editToolBar->addAction(PasteAct);

//format
//formatToolBar = addToolBar("Format ToolBar");
//formatToolBar->setObjectName("Format");
//insertToolBarBreak(formatToolBar);


//tools
runToolBar = addToolBar("Tools Toolbar");
runToolBar->setObjectName("Tools");
runToolBar->setStyleSheet(Theme::viewportDarkStyleSheet);

list.clear();
list.append(tr("Quick Build"));
list.append("LaTeX");
list.append("Dvi->PS");
list.append("PDFLaTeX");
list.append("BibTeX");
list.append("MakeIndex");
list.append("MPost");
list.append("PS->PDF");
list.append("DVI->PDF");
list.append("Asymptote");
list.append("LatexMk");
list.append("R Sweave");
list.append("XeLaTeX");
list.append("LuaLaTeX");

for ( int i = 0; i <= 4; i++ ) list.append(QString::number(i+1)+": "+UserToolName[i]);

comboCompil = new QComboBox(runToolBar);
comboCompil->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
comboCompil->setStyleSheet(Theme::comboboxDarkStyleSheet);
comboCompil->addItems(list);
comboCompil->setCurrentIndex(runIndex);
connect(runToolBar->addAction(getIcon(":/images/run.png"),tr("Run")), SIGNAL(triggered()), this, SLOT(doCompile()));
runToolBar->addWidget(comboCompil);

list.clear();
list.append(tr("View Dvi"));
list.append(tr("View PS"));
list.append(tr("View PDF"));

comboView = new QComboBox(runToolBar);
comboView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
comboView->setStyleSheet(Theme::comboboxDarkStyleSheet);
comboView->addItems(list);
comboView->setCurrentIndex(viewIndex);
connect(runToolBar->addAction(getIcon(":/images/run.png"),tr("View")), SIGNAL(triggered()), this, SLOT(doView()));
runToolBar->addWidget(comboView);



Act = new QAction(getIcon(":/images/viewlog.png"),tr("View Log"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(ViewLog()));
logToolBar->addAction(Act);

Act = new QAction(getIcon(":/images/errornext.png"),tr("Next LaTeX Error"), this);
connect(Act, SIGNAL(triggered()), this, SLOT(NextError()));
Act->setShortcut(Qt::ALT+Qt::Key_Down);
Act->setToolTip("Alt+Down");
logToolBar->addAction(Act);

Act = new QAction(getIcon(":/images/errorprev.png"),tr("Previous LaTeX Error"), this);
Act->setShortcut(Qt::ALT+Qt::Key_Up);
Act->setToolTip("Alt+Up");
connect(Act, SIGNAL(triggered()), this, SLOT(PreviousError()));
logToolBar->addAction(Act);

StopAct = new QAction(getIcon(":/images/process-stop.png"),tr("Stop Process"), this);
connect(StopAct, SIGNAL(triggered()), this, SLOT(stopProcess()));
logToolBar->addAction(StopAct);
StopAct->setEnabled(false);

viewMenu->addSeparator();
viewMenu->addAction(fileToolBar->toggleViewAction());
viewMenu->addAction(editToolBar->toggleViewAction());
//viewMenu->addAction(formatToolBar->toggleViewAction());
viewMenu->addAction(runToolBar->toggleViewAction());
}


void Texmaker::createStatusBar()
{
toggleStructureButton=new PlayerButton(statusBar());
toggleStructureButton->setImages("structure_button");
connect(toggleStructureButton, SIGNAL( clicked() ), this, SLOT(ToggleStructurePanel() ) );
statusBar()->addPermanentWidget(toggleStructureButton,0);
toggleLogButton=new PlayerButton(statusBar());
toggleLogButton->setImages("log_button");
connect(toggleLogButton, SIGNAL( clicked() ), this, SLOT(ToggleLogPanel() ) );
statusBar()->addPermanentWidget(toggleLogButton,0);
togglePdfButton=new PlayerButton(statusBar());
togglePdfButton->setImages("pdf_button");
connect(togglePdfButton, SIGNAL( clicked() ), this, SLOT(TogglePdfPanel() ) );
statusBar()->addPermanentWidget(togglePdfButton,0);
toggleSourceButton=new PlayerButton(statusBar());
toggleSourceButton->setImages("source_button");
connect(toggleSourceButton, SIGNAL( clicked() ), this, SLOT(ToggleSourcePanel() ) );
statusBar()->addPermanentWidget(toggleSourceButton,0);

if (embedinternalpdf && builtinpdfview) togglePdfButton->show();
else togglePdfButton->hide();

stat2=new DropShadowLabel("Ready",statusBar());
stat2->setColor(Theme::grayColor);
stat2->setDropShadowColor(QColor("#000000"));
stat2->setStyleSheet(Theme::labelStyleSheet);
statusBar()->addPermanentWidget(stat2,0);

statusBar()->addPermanentWidget(new QLabel(),1);

stat1=new DropShadowLabel("",statusBar());
stat1->setColor(Theme::grayColor);
stat1->setDropShadowColor(QColor("#000000"));
stat1->setStyleSheet(Theme::labelStyleSheet);

stat3=new DropShadowLabel("",statusBar());
stat3->setColor(Theme::grayColor);
stat3->setDropShadowColor(QColor("#000000"));
stat3->setStyleSheet(Theme::labelStyleSheet);


statusBar()->addPermanentWidget(stat3,0);
statusBar()->addPermanentWidget(stat1,0);
statusBar()->setAutoFillBackground( true );
QPalette pal( palette() );
pal.setColor( QPalette::Active, QPalette::WindowText,Theme::grayColor );
pal.setColor( QPalette::Inactive, QPalette::WindowText, Theme::grayColor );
pal.setColor( QPalette::Disabled, QPalette::WindowText, Theme::grayColor );
pal.setColor( QPalette::Active, QPalette::HighlightedText, Theme::grayColor );
pal.setColor( QPalette::Inactive, QPalette::HighlightedText, Theme::grayColor );
pal.setColor( QPalette::Disabled, QPalette::HighlightedText,Theme::grayColor );
pal.setColor( QPalette::Active, QPalette::Base, Theme::grayColor );
pal.setColor( QPalette::Inactive, QPalette::Base, Theme::grayColor );
pal.setColor( QPalette::Disabled, QPalette::Base, Theme::grayColor);
pal.setColor(QPalette::Background, Theme::lightbackgroundColor);
statusBar()->setPalette( pal );
//statusBar()->setStyleSheet("QWidget {background-color:#6F6F6F;}");

toggleStructureButton->setEnabled(showstructview);
toggleLogButton->setEnabled(showoutputview);
togglePdfButton->setEnabled(showpdfview);
toggleSourceButton->setEnabled(showsourceview);
}

void Texmaker::UpdateCaption()
{
QString title;
if   ( !currentEditorView() )	{title="Texmaker";}
else
	{
	title="Document : "+getName();
	//input_encoding=currentEditorView()->editor->getEncoding();
	}
setWindowTitle(title);
//UpdateStructure();
if (singlemode)
	{
	OutputTextEdit->clear();
	OutputTableWidget->hide();
	if (splitter2->sizes().at(1)>0) OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
	separatorline->hide();
	logpresent=false;
	}
QString finame=getName();
int check=comboFiles->findData(finame,Qt::UserRole,Qt::MatchExactly | Qt::MatchCaseSensitive);
comboFiles->setCurrentIndex(check);
if ((check>-1) && (check<OpenedFilesListWidget->count())) OpenedFilesListWidget->setCurrentRow(check);
if (!finame.startsWith("untitled") && finame!="") 
  {
  lastDocument=finame;
  }
if   (currentEditorView())
  {
   SaveAct->setEnabled(currentEditorView()->editor->document()->isModified());
   UndoAct->setEnabled(currentEditorView()->editor->document()->isUndoAvailable());
   RedoAct->setEnabled(currentEditorView()->editor->document()->isRedoAvailable());
   CopyAct->setEnabled(currentEditorView()->editor->textCursor().hasSelection());
   CutAct->setEnabled(currentEditorView()->editor->textCursor().hasSelection());
   stat3->setText(QString(" %1 ").arg(currentEditorView()->editor->getEncoding()));
  }
else
  {
   SaveAct->setEnabled(false);
   UndoAct->setEnabled(false);
   RedoAct->setEnabled(false);
   CopyAct->setEnabled(false);
   CutAct->setEnabled(false);
   stat3->setText(QString(" %1 ").arg(input_encoding));
  }
if (currentEditorView()) 
  {
  currentEditorView()->editor->setFocus();
  showCursorPos(currentEditorView()->editor->textCursor().blockNumber() + 1,currentEditorView()->editor->textCursor().position() - currentEditorView()->editor->textCursor().block().position()+1);
//  showCursorPos(currentEditorView()->editor->textCursor().blockNumber() + 1,currentEditorView()->editor->textCursor().position() - currentEditorView()->editor->document()->findBlock(currentEditorView()->editor->textCursor().selectionStart()).position());
  }
else posLabel->setText("L: C: ");
}

void Texmaker::NewDocumentStatus(bool m)
{
if ( !currentEditorView() )	return;
QString finame=getName();
int check=comboFiles->findData(finame,Qt::UserRole,Qt::MatchExactly | Qt::MatchCaseSensitive);
if ((check>-1) && (check<OpenedFilesListWidget->count())) OpenedFilesListWidget->setCurrentRow(check);
if (m)
	{
	//EditorView->setTabIcon(EditorView->indexOf(currentEditorView()),getIcon(":/images/modified.png"));
	//EditorView->setTabText(EditorView->indexOf(currentEditorView()),QFileInfo( getName() ).fileName());
	comboFiles->setItemIcon(comboFiles->findData(finame,Qt::UserRole,Qt::MatchExactly | Qt::MatchCaseSensitive),getIcon(":/images/modified.png"));
	if ((check>-1) && (check<OpenedFilesListWidget->count())) OpenedFilesListWidget->item(check)->setIcon(getIcon(":/images/modified.png"));
	SaveAct->setEnabled(true);
	}
else
	{
	//EditorView->setTabIcon(EditorView->indexOf(currentEditorView()),getIcon(":/images/empty.png"));
	//EditorView->setTabText(EditorView->indexOf(currentEditorView()),QFileInfo( getName() ).fileName());
	currentEditorView()->editor->updateRevisions();
	comboFiles->setItemIcon(comboFiles->findData(finame,Qt::UserRole,Qt::MatchExactly | Qt::MatchCaseSensitive),QIcon(":/images/empty.png"));
	if ((check>-1) && (check<OpenedFilesListWidget->count())) OpenedFilesListWidget->item(check)->setIcon(QIcon(":/images/empty.png"));
	SaveAct->setEnabled(false);
	}
}

LatexEditorView *Texmaker::currentEditorView() const
{
if ( EditorView->currentWidget() && EditorView->currentWidget()->inherits( "LatexEditorView" ) ) return (LatexEditorView*)EditorView->currentWidget();
return 0;
}

QString Texmaker::getName()
{
QString title;
if ( !currentEditorView() )	{title="";}
else {title=filenames[currentEditorView()];}
return title;
}

bool Texmaker::FileAlreadyOpen(QString f)
{
bool rep=false;
FilesMap::Iterator it;
QString fw32,funix,forig, canorig, can;
canorig=QFileInfo(f).canonicalFilePath();
for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	forig=filenames[it.key()];
	fw32=filenames[it.key()];
	funix=filenames[it.key()];
	fw32.replace(QString("\\"),QString("/"));
	funix.replace(QString("/"),QString("\\"));
	can=QFileInfo(filenames[it.key()]).canonicalFilePath();
	if ( (forig==f) || (fw32==f) || (funix==f) || (canorig==can)) 
		{
		EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
		rep=true;
		}
	}
return rep;
}

void Texmaker::ComboFilesInsert(const QString & file)
{
int index;
QString fname = QFileInfo( file ).fileName();

for (index=0; index<comboFiles->count(); index++)
    if (comboFiles->itemText(index).localeAwareCompare(fname) > 0) break;

comboFiles->insertItem(index, fname, file);
comboFiles->setItemIcon(index,QIcon(":/images/empty.png"));
OpenedFilesListWidget->insertItem(index,fname);
OpenedFilesListWidget->item(index)->setIcon(QIcon(":/images/empty.png"));
}
///////////////////FILE//////////////////////////////////////
void Texmaker::load( const QString &f )
{
if (FileAlreadyOpen(f) || !QFile::exists( f )) return;
QFileInfo fi(f);
if  (fi.suffix()=="tks") 
  {
  LoadSessionFile(f);
  return;
  }

QFile file( f );
if ( !file.open( QIODevice::ReadOnly ) )
	{
	QMessageBox::warning( this,tr("Error"), tr("You do not have read permission to this file."));
	return;
	}
bool hasDecodingError=false;
QByteArray peekBytes(file.peek(1024));
QByteArray buf = file.readAll();

int bytesRead = buf.size();
file.close();

QTextCodec* detected_codec;
QTextCodec* codec = QTextCodec::codecForName(input_encoding.toLatin1());
if(!codec) codec = QTextCodec::codecForLocale();

QRegExp reEnc("% *!TEX +encoding *= *([^\\r\\n\\x2029]+)[\\r\\n\\x2029]", Qt::CaseInsensitive);
int pos = reEnc.indexIn(codec->toUnicode(peekBytes));
if (pos > -1) 
  {
  QString reqName = reEnc.cap(1).trimmed();
  codec = QTextCodec::codecForName(reqName.toLatin1());
  if(!codec) codec = QTextCodec::codecForLocale();
  input_encoding=codec->name();
  }


QString text = codec->toUnicode(buf);
QByteArray verifyBuf = codec->fromUnicode(text);
QString new_encoding="";

// unicode detection
if (bytesRead >= 4 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe && uchar(buf[2]) == 0 && uchar(buf[3]) == 0) || (uchar(buf[0]) == 0 && uchar(buf[1]) == 0 && uchar(buf[2]) == 0xfe && uchar(buf[3]) == 0xff))) 
    {
      detected_codec = QTextCodec::codecForName("UTF-32");
      if (detected_codec) new_encoding=detected_codec->name();
    } 
else if (bytesRead >= 2 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe) || (uchar(buf[0]) == 0xfe && uchar(buf[1]) == 0xff))) 
    {
      detected_codec = QTextCodec::codecForName("UTF-16");
      if (detected_codec) new_encoding=detected_codec->name();

    } 
else if (bytesRead >= 3 && uchar(buf[0]) == 0xef && uchar(buf[1]) == 0xbb && uchar(buf[2])== 0xbf) 
    {
      detected_codec = QTextCodec::codecForName("UTF-8");
      if (detected_codec) new_encoding=detected_codec->name();
    }
else
{
const char *  	data= buf.constData();
int length=buf.size();
bool canbeutf8=true;
static const unsigned char highest1Bits = 0x80;
static const unsigned char highest2Bits = 0xC0;
static const unsigned char highest3Bits = 0xE0;
static const unsigned char highest4Bits = 0xF0;
static const unsigned char highest5Bits = 0xF8;
int multiByte=0;
for (int i=0; i<length; ++i)
  {
      unsigned char c = data[i];

      if (multiByte>0)
      {
	  if ((c & highest2Bits) == 0x80)
	  {
	      --(multiByte);
	      continue;
	  }
	  canbeutf8=false;
	  break;
      }

      // most significant bit zero, single char
      if ((c & highest1Bits) == 0x00)
	  continue;

      // 110xxxxx => init 1 following bytes
      if ((c & highest3Bits) == 0xC0)
      {
	  multiByte = 1;
	  continue;
      }

      // 1110xxxx => init 2 following bytes
      if ((c & highest4Bits) == 0xE0)
      {
	  multiByte = 2;
	  continue;
      }

      // 11110xxx => init 3 following bytes
      if ((c & highest5Bits) == 0xF0)
      {
	  multiByte = 3;
	  continue;
      }
	canbeutf8=false;
	  break;
  }
if (canbeutf8) 
      {
      detected_codec = QTextCodec::codecForName("UTF-8");
      if (detected_codec) new_encoding=detected_codec->name();
      }
 }  

if (new_encoding!="")
  {
    if (new_encoding!=codec->name()) hasDecodingError=true;
  }
else
  {  
  // no unicode
   int minSize = qMin(verifyBuf.size(), buf.size());
  hasDecodingError = (minSize < buf.size()- 4 || memcmp(verifyBuf.constData() + verifyBuf.size() - minSize,buf.constData() + buf.size() - minSize, minSize));
  QEncodingProber prober (QEncodingProber::Universal);
  if (hasDecodingError)
    {
    prober.feed (buf.constData());
    if (prober.confidence() > 0.6) //Kencodingprober works very bad with tex documents
      {
      detected_codec = QTextCodec::codecForName(prober.encoding());
      if (detected_codec) new_encoding=detected_codec->name();
      else if (input_encoding=="UTF-8") new_encoding="ISO-8859-1";
      else if (input_encoding=="ISO-8859-1") new_encoding="UTF-8";
      else new_encoding=QString(QTextCodec::codecForLocale()->name());
      }
    else if (input_encoding=="UTF-8") new_encoding="ISO-8859-1";
    else if (input_encoding=="ISO-8859-1") new_encoding="UTF-8";
    else new_encoding=QString(QTextCodec::codecForLocale()->name());
    }

  }


  
if (hasDecodingError)
  {
  EncodingDialog *encDlg = new EncodingDialog(this);
  encDlg->ui.comboBoxEncoding->setCurrentIndex(encDlg->ui.comboBoxEncoding->findText(new_encoding, Qt::MatchExactly));
  encDlg->ui.label->setText(encDlg->ui.label->text()+ " ("+input_encoding+").");
  if (encDlg->exec())
	  {
	  new_encoding=encDlg->ui.comboBoxEncoding->currentText();
	  codec = QTextCodec::codecForName(new_encoding.toLatin1());
	  text = codec->toUnicode(buf);
	  }
  else return;
  }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QString dicDir=QCoreApplication::applicationDirPath() + "/";
#else
#ifdef DEBIAN_SPELLDIR
QString dicDir=PREFIX"/share/myspell/dicts/";
#else
QString dicDir=PREFIX"/share/texmaker/";
#endif
#endif

#endif
#if defined(Q_OS_MAC)
QString dicDir=QCoreApplication::applicationDirPath() + "/../Resources/";
#endif
#if defined(Q_OS_WIN32)
QString dicDir=QCoreApplication::applicationDirPath() + "/";
#endif
QRegExp reSpell("% *!TEX +spellcheck *= *([^\\r\\n\\x2029]+)[\\r\\n\\x2029]", Qt::CaseInsensitive);
pos = reSpell.indexIn(codec->toUnicode(peekBytes));
if (pos > -1) 
  {
  QString lang = dicDir+reSpell.cap(1).trimmed();
  if (spellChecker) delete spellChecker;
  QString affdic=lang+".aff";
  QString spelldic=lang+".dic";
  QFileInfo fidic(spelldic);
  QFileInfo fiaff(affdic);
  if (fidic.exists() && fidic.isReadable() && fiaff.exists() && fiaff.isReadable())
      {
      spellChecker = new Hunspell(lang.toLatin1()+".aff",lang.toLatin1()+".dic");
      }
  else spellChecker=0;
  }  
  
LatexEditorView *edit = new LatexEditorView(0,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),f,userTagsList);
EditorView->addWidget( edit);
ComboFilesInsert(f);
disconnect(EditorView, SIGNAL( currentChanged( int ) ), this, SLOT(UpdateStructure()) );
EditorView->setCurrentIndex(EditorView->indexOf(edit));
connect(EditorView, SIGNAL( currentChanged( int ) ), this, SLOT(UpdateStructure()) );
edit->editor->setReadOnly(false);
if (hasDecodingError) edit->editor->setEncoding(new_encoding);
else edit->editor->setEncoding(input_encoding);
initCompleter();
if (completion) edit->editor->setCompleter(completer);
else edit->editor->setCompleter(0);
edit->editor->setPlainText(text);
filenames.remove( edit);
filenames.insert( edit, f );

edit->editor->resetRevisions();
if(svnEnable) 
{
connect(new SvnHelper(f,svnPath), SIGNAL(uncommittedLines(QList<int>)),edit->editor , SLOT(setUncommittedLines(QList<int>)));
edit->editor->viewport()->update();
}

edit->editor->document()->setModified(false);
connect(edit->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
connect(edit->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
connect(edit->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
connect(edit->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
connect(edit->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
currentEditorView()->editor->setLastNumLines(currentEditorView()->editor->numoflines());
connect(edit->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
connect(edit->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
connect(edit->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
connect(edit->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));

if (wordwrap) {edit->editor->setWordWrapMode(QTextOption::WordWrap);}
else {edit->editor->setWordWrapMode(QTextOption::NoWrap);}



UpdateCaption();
NewDocumentStatus(false);
AddRecentFile(f);
ShowStructure();
UpdateStructure();
UpdateBibliography();

QString rootName,rootFilePath;
QTextCursor curs(edit->editor->document());
curs.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,1024);
QString peekStr = curs.selectedText();
QRegExp re("% *!TEX +root *= *([^\\x2029]+)\\x2029", Qt::CaseInsensitive);
pos = re.indexIn(peekStr);
if (pos > -1) 
  {
  rootName = re.cap(1).trimmed();
  QFileInfo rootFileInfo(fi.canonicalPath() + "/" + rootName);
  if (rootFileInfo.exists()) rootFilePath = rootFileInfo.canonicalFilePath();
  else rootFilePath = rootFileInfo.filePath();
  setMasterDocument(rootFilePath);
  }




#if !defined(Q_OS_MAC)
show();
if (windowState()==Qt::WindowMinimized) setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
qApp->setActiveWindow(this);
activateWindow();
setFocus();
//raise();
//#ifdef Q_WS_WIN
//        if (IsIconic (this->winId())) ShowWindow(this->winId(), SW_RESTORE);
//#endif
#endif
if (winmaximized) setWindowState(windowState() & Qt::WindowMaximized | Qt::WindowActive);
edit->editor->setFocus();
}

void Texmaker::setLine( const QString &line )
{
bool ok;
int l=line.toInt(&ok,10);
if (currentEditorView() && ok)
	{
	QTextCursor cur=currentEditorView()->editor->textCursor();
	cur.movePosition(QTextCursor::End);
	currentEditorView()->editor->setTextCursor(cur);
	currentEditorView()->editor->gotoLine(l-1);
	currentEditorView()->editor->setFocus();
	}
}

void Texmaker::insertFromCommandLine(const QString &entity)
{
if (currentEditorView())
	{
	currentEditorView()->editor->insertPlainText(entity);
	#if !defined(Q_OS_MAC)
	show();
	if (windowState()==Qt::WindowMinimized) setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
	qApp->setActiveWindow(this);
	activateWindow();
	setFocus();
	#endif
	if (winmaximized) setWindowState(windowState() & Qt::WindowMaximized | Qt::WindowActive);
	currentEditorView()->editor->setFocus();
	}
}

void Texmaker::fileNew()
{
LatexEditorView *edit = new LatexEditorView(0,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),"untitled"+QString::number(untitled_id),userTagsList);
edit->editor->setReadOnly(false);
edit->editor->setEncoding(input_encoding);
initCompleter();
if (completion) edit->editor->setCompleter(completer);
else edit->editor->setCompleter(0);
EditorView->addWidget( edit);
EditorView->setCurrentIndex(EditorView->indexOf(edit));
if (wordwrap) {edit->editor->setWordWrapMode(QTextOption::WordWrap);}
else {edit->editor->setWordWrapMode(QTextOption::NoWrap);}
filenames.remove( edit);
filenames.insert( edit, "untitled"+QString::number(untitled_id) );
ComboFilesInsert("untitled"+QString::number(untitled_id));
untitled_id++;
edit->editor->document()->setModified(false);
connect(edit->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
connect(edit->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
connect(edit->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
connect(edit->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
connect(edit->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
currentEditorView()->editor->setLastNumLines(currentEditorView()->editor->numoflines());
connect(edit->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
connect(edit->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
connect(edit->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
connect(edit->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));
UpdateCaption();
NewDocumentStatus(false);
edit->editor->setFocus();
}

void Texmaker::fileNewFromFile()
{
QString currentDir=QDir::homePath();
if (!lastTemplate.isEmpty())
	{
	QFileInfo fi(lastTemplate);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getOpenFileName(this,tr("Open File"),currentDir,"TeX files (*.tex *.bib *.sty *.cls *.mp *.Rnw *.asy);;All files (*.*)");
if (fn.isEmpty()) return;
QFile file( fn );
if ( !file.open( QIODevice::ReadOnly ) )
	{
	QMessageBox::warning( this,tr("Error"), tr("You do not have read permission to this file."));
	return;
	}
lastTemplate=fn;
LatexEditorView *edit = new LatexEditorView(0,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),fn,userTagsList);
edit->editor->setReadOnly(false);
edit->editor->setEncoding(input_encoding);
initCompleter();
if (completion) edit->editor->setCompleter(completer);
else edit->editor->setCompleter(0);
EditorView->addWidget( edit);
EditorView->setCurrentIndex(EditorView->indexOf(edit));
if (wordwrap) {edit->editor->setWordWrapMode(QTextOption::WordWrap);}
else {edit->editor->setWordWrapMode(QTextOption::NoWrap);}
filenames.remove( edit);
filenames.insert( edit, "untitled"+QString::number(untitled_id));
ComboFilesInsert("untitled"+QString::number(untitled_id));
untitled_id++;
QTextStream ts( &file );
QTextCodec* codec = QTextCodec::codecForName(input_encoding.toLatin1());
if(!codec) codec = QTextCodec::codecForLocale();
ts.setCodec(codec);
edit->editor->setPlainText( ts.readAll() );
file.close();
edit->editor->document()->setModified(true);
connect(edit->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
connect(edit->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
connect(edit->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
currentEditorView()->editor->setLastNumLines(currentEditorView()->editor->numoflines());
connect(edit->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
connect(edit->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
connect(edit->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
connect(edit->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
connect(edit->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
connect(edit->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
connect(edit->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));

UpdateCaption();
NewDocumentStatus(true);
UpdateStructure();
UpdateBibliography();
edit->editor->setFocus();
}

void Texmaker::fileOpen()
{
QString currentDir=QDir::homePath();
if (!lastDocument.isEmpty())
	{
	QFileInfo fi(lastDocument);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QStringList filesNames = QFileDialog::getOpenFileNames(this,tr("Open File"),currentDir,"TeX files (*.tex *.bib *.sty *.cls *.mp *.Rnw *.asy);;All files (*.*)");
foreach (const QString& fn, filesNames)
  {
  if ( !fn.isEmpty() ) load( fn );
  }
if ((filesNames.count()==1) && embedinternalpdf && builtinpdfview && showpdfview)
  {
  if ( !currentEditorView() || !singlemode ) return;
  QString finame=getName();
  QFileInfo fi(finame);
  QString basename=fi.completeBaseName();
  QString pdfname=outputName(finame,".pdf");
  QFileInfo pdfi(pdfname);
  if (pdfi.exists() && pdfi.isReadable()) 
    {
      if (pdfviewerWidget)
	{
	pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
	StackedViewers->setCurrentWidget(pdfviewerWidget);
	
	//pdfviewerWidget->raise();
	pdfviewerWidget->show();
	}
      else
	{
    //    pdfviewerWidget=new PdfViewer(outputName(finame,".pdf"),viewpdf_command, this);
	pdfviewerWidget=new PdfViewerWidget(outputName(finame,".pdf"),viewpdf_command,ghostscript_command,lp_options,QKeySequence(keyToggleFocus),pdfCheckerLang,lastScale,StackedViewers);
	pdfviewerWidget->centralToolBarBis->setMinimumHeight(centralToolBarBis->height());
	pdfviewerWidget->centralToolBarBis->setMaximumHeight(centralToolBarBis->height());
	connect(pdfviewerWidget, SIGNAL(openDocAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool)));
	connect(pdfviewerWidget, SIGNAL(sendFocusToEditor()), this, SLOT(getFocusToEditor()));
	connect(pdfviewerWidget, SIGNAL(sendPaperSize(const QString&)), this, SLOT(setPrintPaperSize(const QString&)));
	StackedViewers->addWidget(pdfviewerWidget);
	StackedViewers->setCurrentWidget(pdfviewerWidget);
	//pdfviewerWidget->raise();
	pdfviewerWidget->show();
	pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
	}
    }
  }
if (currentEditorView()) currentEditorView()->editor->setFocus();
}

bool Texmaker::isCurrentModifiedOutside()
{
if ( !currentEditorView() ) return false;
QString fn=*filenames.find( currentEditorView() );
QFileInfo fi(fn);
fi.refresh();
QDateTime disktime=fi.lastModified();
int delta=disktime.secsTo(currentEditorView()->editor->getLastSavedTime());
if (watchfiles && (delta<-3)) return true;
else return false;
}

void Texmaker::checkModifiedOutsideAll()
{
if (QApplication::activeWindow() != this) return;

QList<QTreeWidgetItem *> fItems;
bool islabels_expanded=false;
bool isblocks_expanded=true;
fItems=StructureTreeWidget->findItems ("LABELS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      islabels_expanded=fItems.at(0)->isExpanded();
      }
  }
fItems.clear();
fItems=StructureTreeWidget->findItems ("BLOCKS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      isblocks_expanded=fItems.at(0)->isExpanded();
      }
  }  
LatexEditorView *temp = new LatexEditorView(EditorView,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),getName(),userTagsList);
temp=currentEditorView();
FilesMap::Iterator it;
QString fn;
int choice;
disconnect(EditorView, SIGNAL( currentChanged( int ) ), this, SLOT(UpdateStructure()) ); //*******
for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
	if ( !getName().startsWith("untitled")) 
	    {
	    fn=*filenames.find( currentEditorView() );
	    if (isCurrentModifiedOutside())
	      {
	      QFileInfo fi(fn);
	      choice= QMessageBox::warning(this, "Texmaker",
					    tr("The document has been changed outside Texmaker."
					    "Do you want to reload it (and discard your changes) or save it (and overwrite the file)?"),
					    tr("Reload the file"), tr("Save"), tr("Cancel"),
					    0,
					    2 );
	      if (choice==0)
		{
		  if (fi.exists() && fi.isReadable())
		    {
		    QFile file( fn );
		    if (file.open( QIODevice::ReadOnly ) )
			{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			QTextStream ts( &file );
			QTextCodec* codec = QTextCodec::codecForName(currentEditorView()->editor->getEncoding().toLatin1());
			ts.setCodec(codec ? codec : QTextCodec::codecForLocale());
			disconnect(currentEditorView()->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
			disconnect(currentEditorView()->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
			disconnect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
			disconnect(currentEditorView()->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
			disconnect(currentEditorView()->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
			disconnect(currentEditorView()->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
			disconnect(currentEditorView()->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));
			currentEditorView()->editor->setPlainText( ts.readAll() );
			file.close();
			currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
			connect(currentEditorView()->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
			connect(currentEditorView()->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
			connect(currentEditorView()->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
			connect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
			connect(currentEditorView()->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
			connect(currentEditorView()->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
			connect(currentEditorView()->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
			connect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
			connect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
			currentEditorView()->editor->setLastNumLines(currentEditorView()->editor->numoflines());
			connect(currentEditorView()->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
			connect(currentEditorView()->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
			connect(currentEditorView()->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));
			UpdateStructure();
			UpdateBibliography();
			QApplication::restoreOverrideCursor();
			}
		    }
		}
	      else if (choice==1)
		{
		QFile file(fn);
		if (file.open( QIODevice::WriteOnly ) )
		    {
		    QTextStream ts( &file );
		    QTextCodec* codec = QTextCodec::codecForName(currentEditorView()->editor->getEncoding().toLatin1());
		    ts.setCodec(codec ? codec : QTextCodec::codecForLocale());
		    ts << currentEditorView()->editor->toPlainText();
		    file.close();
		    currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
		    currentEditorView()->editor->document()->setModified(false);
		    }
		}
		else currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
	      }
	    }
	}

EditorView->setCurrentIndex(EditorView->indexOf(temp));
connect(EditorView, SIGNAL( currentChanged( int ) ), this, SLOT(UpdateStructure()) ); //********
QString title;
if   ( !currentEditorView() )	{title="Texmaker";}
else
	{
	title="Document : "+getName();
	//input_encoding=currentEditorView()->editor->getEncoding();
	}
setWindowTitle(title);

UpdateStructure();
fItems.clear();
fItems=StructureTreeWidget->findItems ("LABELS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      StructureTreeWidget->setItemExpanded(fItems.at(0),islabels_expanded);
      }
  }
fItems.clear();
fItems=StructureTreeWidget->findItems ("BLOCKS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      StructureTreeWidget->setItemExpanded (fItems.at(0),isblocks_expanded);
      }
  }  

QString finame=getName();
int check=comboFiles->findData(finame,Qt::UserRole,Qt::MatchExactly | Qt::MatchCaseSensitive);
comboFiles->setCurrentIndex(check);
if ((check>-1) && (check<OpenedFilesListWidget->count())) OpenedFilesListWidget->setCurrentRow(check);


if (!finame.startsWith("untitled") && finame!="") 
  {
  lastDocument=finame;

  }
if   (currentEditorView())
  {
   SaveAct->setEnabled(currentEditorView()->editor->document()->isModified());
   UndoAct->setEnabled(currentEditorView()->editor->document()->isUndoAvailable());
   RedoAct->setEnabled(currentEditorView()->editor->document()->isRedoAvailable());
   CopyAct->setEnabled(currentEditorView()->editor->textCursor().hasSelection());
   CutAct->setEnabled(currentEditorView()->editor->textCursor().hasSelection());
   stat3->setText(QString(" %1 ").arg(currentEditorView()->editor->getEncoding()));
  }
else
  {
   SaveAct->setEnabled(false);
   UndoAct->setEnabled(false);
   RedoAct->setEnabled(false);
   CopyAct->setEnabled(false);
   CutAct->setEnabled(false);    
  }
if (currentEditorView()) currentEditorView()->editor->setFocus();
}

void Texmaker::fileSave()
{
if ( !currentEditorView() )	return;
QString fn;
if ( getName().startsWith("untitled") ) {fileSaveAs();}
else
	{
	fn=*filenames.find( currentEditorView() );
	if (isCurrentModifiedOutside())
	{
	QFileInfo fi(fn);
	switch(  QMessageBox::warning(this, "Texmaker",
					tr("The document has been changed outside Texmaker."
					"Do you want to reload it (and discard your changes) or save it (and overwrite the file)?"),
					tr("Reload the file"), tr("Save"), tr("Cancel"),
					0,
					2 ) )
		{
		case 0:
		  if (fi.exists() && fi.isReadable())
		    {
		    filenames.remove(currentEditorView());
		    comboFiles->removeItem(comboFiles->currentIndex());
		    delete OpenedFilesListWidget->currentItem();
		    delete currentEditorView();
		    load(fn);
		    return;
		    }
		  break;
		case 1:
		  break;
		case 2:
		  default:
		  currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
		  return;
		  break;
		}
	}
	QFile file(fn);
	if ( !file.open( QIODevice::WriteOnly ) )
		{
		QMessageBox::warning( this,tr("Error"),tr("The file could not be saved. Please check if you have write permission."));
		return;
		}
	QTextStream ts( &file );
	QTextCodec* codec = QTextCodec::codecForName(currentEditorView()->editor->getEncoding().toLatin1());
	ts.setCodec(codec ? codec : QTextCodec::codecForLocale());
	ts << currentEditorView()->editor->toPlainText();
	file.close();
	currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
	currentEditorView()->editor->document()->setModified(false);
	fn=getName();
	AddRecentFile(fn);
	}
UpdateCaption();
}

bool Texmaker::currentfileSaved()
{
if ( !currentEditorView() ) return true;
QString fn;
if ( getName().startsWith("untitled") ) {return false;}
else
	{
	fn=*filenames.find( currentEditorView() );
	if (isCurrentModifiedOutside())
	{
	QFileInfo fi(fn);
	switch(  QMessageBox::warning(this, "Texmaker",
					tr("The document has been changed outside Texmaker."
					"Do you want to reload it (and discard your changes) or save it (and overwrite the file)?"),
					tr("Reload the file"), tr("Save"), tr("Cancel"),
					0,
					2 ) )
		{
		case 0:
		  if (fi.exists() && fi.isReadable())
		    {
		    filenames.remove(currentEditorView());
		    comboFiles->removeItem(comboFiles->currentIndex());
		    delete OpenedFilesListWidget->currentItem();
		    delete currentEditorView();
		    load(fn);
		    return true;
		    }
		  break;
		case 1:
		  break;
		case 2:
		  default:
		  currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
		  return false;
		  break;
		}
	}
	QFile file(fn);
	if ( !file.open( QIODevice::WriteOnly ) )
		{
		if (currentEditorView()->editor->document()->isModified())
		  {
		  QMessageBox::warning( this,tr("Error"),tr("The file could not be saved. Please check if you have write permission."));
		  return false;
		  }
		else return true;
		}
	QTextStream ts( &file );
	QTextCodec* codec = QTextCodec::codecForName(currentEditorView()->editor->getEncoding().toLatin1());
	ts.setCodec(codec ? codec : QTextCodec::codecForLocale());
	ts << currentEditorView()->editor->toPlainText();
	file.close();
	currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
	currentEditorView()->editor->document()->setModified(false);
	fn=getName();
	AddRecentFile(fn);
	UpdateCaption();
	return true;
	}
}

void Texmaker::fileSaveAs()
{
if ( !currentEditorView() ) 	return;
QString currentDir=QDir::homePath();
if (!lastDocument.isEmpty())
	{
	QFileInfo fi(lastDocument);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getSaveFileName(this,tr("Save As"),currentDir,"TeX files (*.tex *.bib *.sty *.cls *.mp *.Rnw *.asy);;All files (*.*)");
if ( !fn.isEmpty() )
	{
	if (!fn.contains('.')) fn += ".tex";
	QFileInfo fic(fn);
	filenames.remove(currentEditorView());
	comboFiles->removeItem(comboFiles->currentIndex());
	delete OpenedFilesListWidget->currentItem();
	filenames.insert(currentEditorView(), fn );
	fileSave();
	currentEditorView()->editor->updateName(fn);
	//EditorView->setTabText(EditorView->indexOf(currentEditorView()),fic.fileName());
	ComboFilesInsert(fn);
	}
UpdateCaption();
}

void Texmaker::fileSaveAll()
{
LatexEditorView *temp = new LatexEditorView(EditorView,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),getName(),userTagsList);
temp=currentEditorView();
FilesMap::Iterator it;
for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
	fileSave();
	}
EditorView->setCurrentIndex(EditorView->indexOf(temp));
UpdateCaption();
}

void Texmaker::fileBackupAll()
{
if (!currentEditorView() ) return;
QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
LatexEditorView *temp = new LatexEditorView(EditorView,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),getName(),userTagsList);
temp=currentEditorView();
QString fn;
FilesMap::Iterator it;
for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
	if (currentEditorView() && !getName().startsWith("untitled"))
	    {
	    fn=*filenames.find( currentEditorView() )+".bak";
	    QFile file(fn);
	    if ( !file.open( QIODevice::WriteOnly ) ) continue;
	    else
		{
		QTextStream ts( &file );
		QTextCodec* codec = QTextCodec::codecForName(currentEditorView()->editor->getEncoding().toLatin1());
		ts.setCodec(codec ? codec : QTextCodec::codecForLocale());
		ts << currentEditorView()->editor->toPlainText();
		file.close();
		}
	    }
	}
EditorView->setCurrentIndex(EditorView->indexOf(temp));
UpdateCaption();
QApplication::restoreOverrideCursor();
}

void Texmaker::fileSaveACopy()
{
if (!currentEditorView() ) return;
QString currentDir=QDir::homePath();
if (!lastDocument.isEmpty())
	{
	QFileInfo fi(lastDocument);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getSaveFileName(this,tr("Save As"),currentDir,"TeX files (*.tex *.bib *.sty *.cls *.mp *.Rnw *.asy);;All files (*.*)");
if ( !fn.isEmpty() )
	{
	if (!fn.contains('.')) fn += ".tex";
	QFile file(fn);
	if ( file.open( QIODevice::WriteOnly ) )
	    {
	    QTextStream ts( &file );
	    QTextCodec* codec = QTextCodec::codecForName(currentEditorView()->editor->getEncoding().toLatin1());
	    ts.setCodec(codec ? codec : QTextCodec::codecForLocale());
	    ts << currentEditorView()->editor->toPlainText();
	    file.close();
	    }	  
	}
}

void Texmaker::fileClose()
{
if ( !currentEditorView() )	return;
if (currentEditorView()->editor->document()->isModified())
	{
int query;
QString locale = TexmakerApp::instance()->language.left(2);
if (locale=="en")
{
query=QMessageBox::warning(this, "Texmaker",
					"The document contains unsaved work. "
					"Do you want to save it before closing?",
					"Save and Close", "Close without saving", "Cancel",
					0,
					2 );
}
else
{
query=QMessageBox::warning(this, "Texmaker",
					tr("The document contains unsaved work. "
					"Do you want to save it before closing?"),
					tr("Save and Close"), tr("Don't Save and Close"), tr("Cancel"),
					0,
					2 );
}
	switch( query )
		{
		case 0:
		  fileSave();
		  filenames.remove(currentEditorView());
		  comboFiles->removeItem(comboFiles->currentIndex());
		  delete OpenedFilesListWidget->currentItem();
		  delete currentEditorView();
		  break;
		case 1:
		  filenames.remove(currentEditorView());
		  comboFiles->removeItem(comboFiles->currentIndex());
		  delete OpenedFilesListWidget->currentItem();
		  delete currentEditorView();
		  break;
		case 2:
		  default:
		  return;
		  break;
		}
	}
else
{
filenames.remove(currentEditorView());
comboFiles->removeItem(comboFiles->currentIndex());
delete OpenedFilesListWidget->currentItem();
delete currentEditorView();
}
UpdateCaption();
}

void Texmaker::fileCloseRequested( int index)
{
EditorView->setCurrentIndex(index);
fileClose();
}

void Texmaker::fileCloseAll()
{
bool go=true;
int query;
QString locale = TexmakerApp::instance()->language.left(2);
while (currentEditorView() && go)
	{
	if (currentEditorView()->editor->document()->isModified())
		{
if (locale=="en")
{
query=QMessageBox::warning(this, "Texmaker",
					"The document contains unsaved work. "
					"Do you want to save it before closing?",
					"Save and Close", "Close without saving", "Cancel",
					0,
					2 );
}
else
{
query=QMessageBox::warning(this, "Texmaker",
					tr("The document contains unsaved work. "
					"Do you want to save it before closing?"),
					tr("Save and Close"), tr("Don't Save and Close"), tr("Cancel"),
					0,
					2 );
}
		switch(query)
			{
			case 0:
			fileSave();
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
			delete currentEditorView();
			break;
			case 1:
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
			delete currentEditorView();
			break;
			case 2:
			default:
			go=false;
			return;
			break;
			}
		}
	else
		{
		filenames.remove(currentEditorView());
		comboFiles->removeItem(comboFiles->currentIndex());
		delete OpenedFilesListWidget->currentItem();
		delete currentEditorView();
		}
	}
UpdateCaption();
}

void Texmaker::fileExit()
{
if (clean_exit) AutoCleanAll();
SaveSettings();
#ifdef INTERNAL_BROWSER
if (browserWindow) browserWindow->close();
if (diffWindow) diffWindow->close();
#endif

if (pdfviewerWidget) {StackedViewers->removeWidget(pdfviewerWidget);delete(pdfviewerWidget);} 
if (pdfviewerWindow) pdfviewerWindow->close(); 
bool accept=true;
int query;
QString locale = TexmakerApp::instance()->language.left(2);
while (currentEditorView() && accept)
	{
	if (currentEditorView()->editor->document()->isModified())
		{
if (locale=="en")
{
query=QMessageBox::warning(this, "Texmaker",
					"The document contains unsaved work. "
					"Do you want to save it before closing?",
					"Save and Close", "Close without saving", "Cancel",
					0,
					2 );
}
else
{
query=QMessageBox::warning(this, "Texmaker",
					tr("The document contains unsaved work. "
					"Do you want to save it before closing?"),
					tr("Save and Close"), tr("Don't Save and Close"), tr("Cancel"),
					0,
					2 );
}
		switch(query)
			{
			case 0:
			fileSave();
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
			delete currentEditorView();
			break;
			case 1:
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
			delete currentEditorView();
			break;
			case 2:
			default:
			accept=false;
			break;
			}
		
		}
	else
		{
		filenames.remove(currentEditorView());
		comboFiles->removeItem(comboFiles->currentIndex());
	      delete OpenedFilesListWidget->currentItem();
		delete currentEditorView();
		}
	}

if (accept) 
  {
  if (eraseSettings && QFile::exists(settingsFileName)) 
      {
      QFile file(settingsFileName);
      file.open( QIODevice::ReadOnly );
      file.remove();
      }
  if (replaceSettings)
      {
      QString from_file = QFileDialog::getOpenFileName(this,tr("Select a File"),QDir::homePath(),"Setting files (*.ini);;All files (*.*)");
      if (!from_file.isEmpty() && QFile::exists(from_file))
	  {
	  if (!settingsFileName.isEmpty() && QFile::exists(settingsFileName)) 
		{
		QFile file(settingsFileName);
		file.open( QIODevice::ReadOnly );
		file.remove();
		QFile fichier_or(from_file);
		fichier_or.copy(settingsFileName);
		}
	  }
     else
	{
	replaceSettings=false;
	return;
	}
      }
  qApp->quit();
  }
else
  {
  eraseSettings=false;
  replaceSettings=false;
  }
}

void Texmaker::closeEvent(QCloseEvent *e)
{
if (clean_exit) AutoCleanAll();
SaveSettings();
#ifdef INTERNAL_BROWSER
if (browserWindow) browserWindow->close();
if (diffWindow) diffWindow->close();
#endif

if (pdfviewerWidget) {StackedViewers->removeWidget(pdfviewerWidget);delete(pdfviewerWidget);} 
if (pdfviewerWindow) pdfviewerWindow->close(); 
bool accept=true;
int query;
QString locale = TexmakerApp::instance()->language.left(2);

while (currentEditorView() && accept)
	{
	if (currentEditorView()->editor->document()->isModified())
		{
if (locale=="en")
{
query=QMessageBox::warning(this, "Texmaker",
					"The document contains unsaved work. "
					"Do you want to save it before closing?",
					"Save and Close", "Close without saving", "Cancel",
					0,
					2 );
}
else
{
query=QMessageBox::warning(this, "Texmaker",
					tr("The document contains unsaved work. "
					"Do you want to save it before closing?"),
					tr("Save and Close"), tr("Don't Save and Close"), tr("Cancel"),
					0,
					2 );
}
		switch(query)
			{
			case 0:
				fileSave();
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
				delete currentEditorView();
				break;
			case 1:
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
				delete currentEditorView();
				break;
			case 2:
			default: 
				accept=false;
				break;
			}
		}
	else
		{
		filenames.remove(currentEditorView());
		comboFiles->removeItem(comboFiles->currentIndex());
		delete OpenedFilesListWidget->currentItem();
		delete currentEditorView();
		}
	}
if (accept) e->accept();
else e->ignore();
}


void Texmaker::fileOpenRecent()
{
QAction *action = qobject_cast<QAction *>(sender());
if (action) 
  {
  load(action->data().toString());
  if (embedinternalpdf && builtinpdfview && showpdfview)
      {
      if ( !currentEditorView() || !singlemode ) return;
      QString finame=getName();
      QFileInfo fi(finame);
      QString basename=fi.completeBaseName();
      QString pdfname=outputName(finame,".pdf");
      QFileInfo pdfi(pdfname);
      if (pdfi.exists() && pdfi.isReadable()) 
	{
	  if (pdfviewerWidget)
	    {
	    pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
	    StackedViewers->setCurrentWidget(pdfviewerWidget);
	    //pdfviewerWidget->raise();
	    pdfviewerWidget->show();
	    }
	  else
	    {
	//    pdfviewerWidget=new PdfViewer(outputName(finame,".pdf"),viewpdf_command, this);
	    pdfviewerWidget=new PdfViewerWidget(outputName(finame,".pdf"),viewpdf_command,ghostscript_command,lp_options,QKeySequence(keyToggleFocus),pdfCheckerLang,lastScale,StackedViewers);
	    pdfviewerWidget->centralToolBarBis->setMinimumHeight(centralToolBarBis->height());
	    pdfviewerWidget->centralToolBarBis->setMaximumHeight(centralToolBarBis->height());
	    connect(pdfviewerWidget, SIGNAL(openDocAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool)));
	    connect(pdfviewerWidget, SIGNAL(sendFocusToEditor()), this, SLOT(getFocusToEditor()));
	    connect(pdfviewerWidget, SIGNAL(sendPaperSize(const QString&)), this, SLOT(setPrintPaperSize(const QString&)));
	    StackedViewers->addWidget(pdfviewerWidget);
	    StackedViewers->setCurrentWidget(pdfviewerWidget);
	    //pdfviewerWidget->raise();
	    pdfviewerWidget->show();
	    pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
	    }
	}
      }
  }
if (currentEditorView()) currentEditorView()->editor->setFocus();
}

void Texmaker::AddRecentFile(const QString &f)
{
QString nativeFileName = QDir::toNativeSeparators(f);
if (recentFilesList.contains(nativeFileName))
    {
    recentFilesList.move(recentFilesList.indexOf(nativeFileName), 0);
    UpdateRecentFile();
    return; 
    }
if (recentFilesList.count() < 10) recentFilesList.prepend(nativeFileName);
else
	{
	recentFilesList.removeLast();
	recentFilesList.prepend(nativeFileName);
	}
UpdateRecentFile();
}

void Texmaker::UpdateRecentFile()
{
for (int i=0; i < recentFilesList.count(); i++)
	{
        recentFileActs[i]->setIconText(recentFilesList.at(i));
        
        recentFileActs[i]->setData(recentFilesList.at(i));
        recentFileActs[i]->setVisible(true);
	}
for (int j = recentFilesList.count(); j < 10; ++j) recentFileActs[j]->setVisible(false);
}

void Texmaker::CleanRecent()
{
recentFilesList.clear();
UpdateRecentFile();
}

void Texmaker::filePrint()
{
if ( !currentEditorView() ) return;
QTextDocument *document = currentEditorView()->editor->document();
QPrinter printer;
QPrintDialog *dlg = new QPrintDialog(&printer, this);
if (dlg->exec() != QDialog::Accepted) return;
document->print(&printer);
}

void Texmaker::fileOpenAndGoto(const QString &f, int line,bool focus)
{
load(f);
if (currentEditorView())
	{
	QTextCursor cur=currentEditorView()->editor->textCursor();
	cur.movePosition(QTextCursor::End);
	currentEditorView()->editor->setTextCursor(cur);
	currentEditorView()->editor->gotoLine(line-1);
	//currentEditorView()->editor->ensureCursorVisible();
	//currentEditorView()->editor->setHightLightLine();
	}
setLine(QString::number(line));
if (focus) getFocusToEditor();
}

void Texmaker::getFocusToEditor()
{
#if !defined(Q_OS_MAC)
show();
if (windowState()==Qt::WindowMinimized) setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
#endif
show();
if (windowState()==Qt::WindowMinimized) setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
qApp->setActiveWindow(this);
activateWindow();
setFocus();
if (currentEditorView()) currentEditorView()->editor->setFocus();
}

void Texmaker::fileReload()
{
if ( !currentEditorView() ) return;
if ( getName().startsWith("untitled") ) return;
QString f=filenames[currentEditorView()];
if (currentEditorView()->editor->document()->isModified())
	{
	switch(  QMessageBox::warning(this, "Texmaker",
					tr("The document contains unsaved work."
					"You will lose changes by reloading the document."),
					tr("Reload the file"), tr("Cancel"),
					0,
					1 ) )
		{
		case 0:
			filenames.remove(currentEditorView());
			comboFiles->removeItem(comboFiles->currentIndex());
			delete OpenedFilesListWidget->currentItem();
			delete currentEditorView();
			load(f);
			break;
		case 1:
		default:
			return;
			break;
		}
	}
else
  {
  filenames.remove(currentEditorView());
comboFiles->removeItem(comboFiles->currentIndex());
delete OpenedFilesListWidget->currentItem();
  delete currentEditorView();
  load(f);
  }
}

void Texmaker::allReload()
{
LatexEditorView *temp = new LatexEditorView(EditorView,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),getName(),userTagsList);
temp=currentEditorView();
FilesMap::Iterator it;
FilesMap tempfilenames=filenames;
for( it = tempfilenames.begin(); it != tempfilenames.end(); ++it )
	{
	EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
	fileReload();
	}
EditorView->setCurrentIndex(EditorView->indexOf(temp));
UpdateCaption();  
}

void Texmaker::listSelectionActivated(int index)
{
disconnect(OpenedFilesListWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(OpenedFileActivated(QListWidgetItem*)));
if ((index>-1) && (index<OpenedFilesListWidget->count())) OpenedFilesListWidget->setCurrentRow(index);
connect(OpenedFilesListWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(OpenedFileActivated(QListWidgetItem*)));
QString f=comboFiles->itemData(index, Qt::UserRole).toString();
FilesMap::Iterator it;
QString fw32,funix,forig;
for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	forig=filenames[it.key()];
	fw32=filenames[it.key()];
	funix=filenames[it.key()];
	fw32.replace(QString("\\"),QString("/"));
	funix.replace(QString("/"),QString("\\"));
	if ( (forig==f) || (fw32==f) || (funix==f)) 
		{
		EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
		}
	}
}

void Texmaker::ToggleMasterCurrent()
{
QString dest;
FilesMap::Iterator it;
QString fw32,funix,forig;
if (singlemode) 
  {
  lastChild="";
  return;
  }
if (getName()==MasterName)
  {
   dest=lastChild; 
  }
else
  {
  lastChild=getName();
  dest=MasterName;  
  }
if (!dest.isEmpty()) 
  {
  for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	forig=filenames[it.key()];
	fw32=filenames[it.key()];
	funix=filenames[it.key()];
	fw32.replace(QString("\\"),QString("/"));
	funix.replace(QString("/"),QString("\\"));
	if ( (forig==dest) || (fw32==dest) || (funix==dest)) 
		{
		EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
		}
	}
  }
}

//////////////////////////// EDIT ///////////////////////
void Texmaker::editUndo()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->undoText();
}

void Texmaker::editRedo()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->redoText();
}

void Texmaker::editCut()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->cut();
}

void Texmaker::editCopy()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->copy();
}

void Texmaker::editPaste()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->paste();
}

void Texmaker::editSelectAll()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->selectAll();
}

void Texmaker::editFindInDirectory()
{
if (!scanDialog) 
{
 scanDialog = new ScanDialog(this);
 connect(scanDialog, SIGNAL(openFileAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool))); 
}
scanDialog->show();
scanDialog->raise();
}

void Texmaker::editFind()
{
if ( !currentEditorView() )	return;
currentEditorView()->showFind();
}

void Texmaker::editFindNext()
{
if ( !currentEditorView() )	return;
currentEditorView()->showFindNext();
}


void Texmaker::editReplace()
{
if ( !currentEditorView() )	return;
currentEditorView()->showReplace();
}

void Texmaker::editGotoLine()
{
if ( !currentEditorView() )	return;
currentEditorView()->showGoto();
}

void Texmaker::editComment()
{
if ( !currentEditorView() )	return;
currentEditorView()->editor->commentSelection();
}

void Texmaker::editUncomment()
{
if ( !currentEditorView() )	return;
currentEditorView()->editor->uncommentSelection();
}

void Texmaker::editIndent()
{
if ( !currentEditorView() )	return;
currentEditorView()->editor->indentSelection();
}

void Texmaker::editUnindent()
{
if ( !currentEditorView() )	return;
currentEditorView()->editor->unindentSelection();
}

void Texmaker::editSpell()
{
if ( !currentEditorView() )	return;
if (spelldicExist()) 
	{
	SpellerDialog *spellDlg=new SpellerDialog(this,currentEditorView()->editor,spell_ignored_words);
	if (spellDlg->exec())
		{
		spell_ignored_words=spellDlg->alwaysignoredwordList.join(",");
		currentEditorView()->editor->highlighter->SetAlwaysIgnoredWords(spell_ignored_words);
		currentEditorView()->editor->highlighter->rehighlight();
		}
	}
else
	{
	QMessageBox::warning( this,tr("Error"),tr("Error : Can't open the dictionary"));
	return;
	}
}

void Texmaker::editTipTab()
{
if ( !currentEditorView() )	return;
OutputTextEdit->clear();
OutputTextEdit->insertLine("Use the Tab key to reach the next "+QString(0x2022)+" field");
}

void Texmaker::editRunScript()
{
if ( !currentEditorView() )	return;
QString currentDir=QDir::homePath();
if (!lastScript.isEmpty())
	{
	QFileInfo fi(lastScript);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getOpenFileName(this,tr("Browse script"),currentDir,"Script (*.tms)");
if (fn.isEmpty()) return;
lastScript=fn;
currentEditorView()->editor->ExecuteScript(fn);
}

void Texmaker::editRunFurnishedScript()
{
QString actData;
QAction *action = qobject_cast<QAction *>(sender());
if ( !currentEditorView() )	return;
if (action)
	{
	actData=action->data().toString();
	#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

	#ifdef USB_VERSION
	QString scriptfile=QCoreApplication::applicationDirPath() + "/"+actData;
	#else
	QString scriptfile=PREFIX"/share/texmaker/"+actData;
	#endif

	#endif
	#if defined(Q_OS_MAC)
	QString scriptfile=QCoreApplication::applicationDirPath() + "/../Resources/"+actData;
	#endif
	#if defined(Q_OS_WIN32)
	QString scriptfile=QCoreApplication::applicationDirPath() + "/"+actData;
	#endif

	QFileInfo fic(scriptfile);
	if (fic.exists() && fic.isReadable() )
		{
		currentEditorView()->editor->ExecuteScript(scriptfile);
		}
	else { QMessageBox::warning( this,tr("Error"),tr("File not found"));}
	

	}

}

/************ by Robin.Watts ***********************/
static void lineTo(LatexEditor *editor, int line)
{
	QTextCursor cur=editor->textCursor();
	cur.movePosition(QTextCursor::End);
	editor->setTextCursor(cur);
	editor->gotoLine(line);
	editor->setFocus();
}

void Texmaker::foldAll()
{
	if ( !currentEditorView() )	return;

	QAction *action = qobject_cast<QAction *>(sender());

	if (!action) return;

	doFoldAll(action->data().toInt());
}

void Texmaker::foldAllUnder()
{
	if ( !currentEditorView() )	return;

	QAction *action = qobject_cast<QAction *>(sender());

	if (!action) return;

	int i = action->data().toInt();
	int n = 5;

	for (n--; n >= i; n--)
		doFoldAll(n);
}

void Texmaker::doFoldAll(int i)
{
	LatexEditor *editor = currentEditorView()->editor;
	QTextDocument *doc = editor->document();
	int line = editor->textCursor().blockNumber();
	int newLine = line;

	QMapIterator<int, int> it(editor->foldableLines);
	while (it.hasNext()) {
		it.next();

		int startLine = it.key();
		int endLine = it.value();
		QTextBlock block = doc->findBlockByNumber(startLine);
		QString text = block.text();
		if (text.contains(foldClasses[i], Qt::CaseInsensitive))
		{
			editor->fold(startLine, endLine);
			if (newLine >= startLine && newLine <= endLine)
				newLine = startLine;
		}
	}

	lineTo(editor, newLine);
}

void Texmaker::unfoldAll()
{
	if ( !currentEditorView() )	return;

	QAction *action = qobject_cast<QAction *>(sender());

	if (!action) return;

	int i=action->data().toInt();
	doUnfoldAll(i);
}

void Texmaker::unfoldAllUnder()
{
	if ( !currentEditorView() )	return;

	QAction *action = qobject_cast<QAction *>(sender());

	if (!action) return;

	int i = action->data().toInt();
	int n = 5;

	for (n--; n >= i; n--)
		doUnfoldAll(n);
}

void Texmaker::doUnfoldAll(int i)
{
	LatexEditor *editor = currentEditorView()->editor;
	QTextDocument *doc = editor->document();

	QMapIterator<int, int> it(editor->foldedLines);
	while (it.hasNext()) {
		it.next();

		int startLine = it.key();
		QTextBlock block = doc->findBlockByNumber(startLine);
		QString text = block.text();
		if (text.contains(foldClasses[i], Qt::CaseInsensitive))
			editor->unfold(startLine, it.value());
	}
}

void Texmaker::foldEnclosing()
{
	if ( !currentEditorView() )	return;
	LatexEditor *editor = currentEditorView()->editor;
	int line = editor->textCursor().blockNumber();

	/* Find the largest folded area at this point */
	int foldedStartLine = INT_MAX;
	int foldedEndLine = INT_MIN;
	QMapIterator<int, int> it(editor->foldedLines);
	while (it.hasNext()) {
		it.next();
		int startLine = it.key();
		int endLine = it.value();
		if (startLine <= line && endLine >= line && foldedStartLine >= startLine && foldedEndLine <= endLine)
		{
			foldedStartLine = startLine;
			foldedEndLine = endLine;
		}
	}

	/* Now find the smallest foldable area, larger than the folded area. */
	int bestStartLine = INT_MIN;
	int bestEndLine = INT_MAX;
	QMapIterator<int, int> it2(editor->foldableLines);
	while (it2.hasNext()) {
		it2.next();
		int startLine = it2.key();
		int endLine = it2.value();
		if (startLine <= line && endLine > line &&
		    bestStartLine <= startLine && bestEndLine >= endLine &&
		    foldedStartLine > startLine && foldedEndLine <= endLine)
		{
			bestStartLine = startLine;
			bestEndLine = endLine;
		}
	}
	if (bestStartLine >= 0)
	{
		editor->fold(bestStartLine, bestEndLine);
		lineTo(editor, bestStartLine);
	}
	else
	{
		/* FIXME: Beep! */
	}
}

void Texmaker::unfoldEnclosing()
{
	if ( !currentEditorView() )	return;
	LatexEditor *editor = currentEditorView()->editor;
	int line = editor->textCursor().blockNumber();
	int bestStartLine = INT_MAX;
	int bestEndLine = INT_MIN;
	QMapIterator<int, int> it(editor->foldedLines);
	while (it.hasNext()) {
		it.next();
		int startLine = it.key();
		int endLine = it.value();
		if (startLine <= line && endLine > line && bestStartLine >= startLine && bestEndLine <= endLine)
		{
			bestStartLine = startLine;
			bestEndLine = endLine;
		}
	}
	if (bestEndLine >= 0)
	{
		editor->unfold(bestStartLine, bestEndLine);
		lineTo(editor, bestStartLine + (bestStartLine != bestEndLine ? 1 : 0));
	}
	else
	{
		/* FIXME: Beep! */
	}
}
/////////////// CONFIG ////////////////////
void Texmaker::ReadSettings()
{

#ifdef USB_VERSION
QSettings *config=new QSettings(QCoreApplication::applicationDirPath()+"/texmaker.ini",QSettings::IniFormat); //for USB-stick version :
#else
QSettings *config=new QSettings(QSettings::IniFormat,QSettings::UserScope,"xm1","texmaker");
#endif
settingsFileName=config->fileName();
if (!config->contains("IniMode")) 
	{
	delete config;
	config=new QSettings("xm1","texmaker");
	}
config->beginGroup( "texmaker" );
singlemode=true;


QFontDatabase fdb;
QStringList xf = fdb.families();
QString deft;

bool new_user=(!config->contains("GUI/New Version"));

modern_style=config->value( "GUI/Style",true).toBool();
new_gui=config->value( "GUI/New Version",false).toBool();


QRect screen = QApplication::desktop()->screenGeometry();
int w= config->value( "Geometries/MainwindowWidth",screen.width()-150).toInt();
int h= config->value( "Geometries/MainwindowHeight",screen.height()-100).toInt() ;
int x= config->value( "Geometries/MainwindowX",30).toInt();
int y= config->value( "Geometries/MainwindowY",30).toInt() ;
resize(w,h);
move(x,y);
windowstate=config->value("MainWindowState").toByteArray();
winmaximized=config->value("MainWindowMaximized",false).toBool();

splitter1state=config->value("Splitter1State").toByteArray();
splitter2state=config->value("Splitter2State").toByteArray();
splitter3state=config->value("Splitter3State").toByteArray();
psize=config->value( "Print/PaperSize","a4").toString();
lastScale=config->value( "PdfView/Scale",10).toFloat()/100.0;
#if defined(Q_OS_WIN32)
if (xf.contains("Courier New",Qt::CaseInsensitive)) deft="Courier New";
else deft=qApp->font().family();
QString fam=config->value("Editor/Font Family",deft).toString();
int si=config->value( "Editor/Font Size",10).toInt();
#else
if (xf.contains("DejaVu Sans Mono",Qt::CaseInsensitive)) deft="DejaVu Sans Mono";
else if (xf.contains("Lucida Sans Typewriter",Qt::CaseInsensitive)) deft="Lucida Sans Typewriter";
else deft=qApp->font().family();
QString fam=config->value("Editor/Font Family",deft).toString();
int si=config->value( "Editor/Font Size",qApp->font().pointSize()).toInt();
#endif
QFont F(fam,si);
EditorFont=F;

tabspaces=config->value( "Editor/TabSpaces",false).toBool();
tabwidth=config->value( "Editor/TabWidth",4).toInt();
wordwrap=config->value( "Editor/WordWrap",true).toBool();
watchfiles=config->value( "Editor/Watcher",false).toBool();
autosave=config->value( "Editor/AutoSave",false).toBool();
parenmatch=config->value( "Editor/Parentheses Matching",true).toBool();
showline=config->value( "Editor/Line Numbers",true).toBool();
completion=config->value( "Editor/Completion",true).toBool();
userCompletionList=config->value( "Editor/UserCompletion",true).toStringList();
svnEnable=config->value( "Editor/SvnEnable",false).toBool();
svnPath=config->value("Editor/SvnPath","").toString();
shortcuts.clear();
QStringList data,shortcut;
data=config->value("Shortcuts/data").toStringList();
shortcut=config->value("Shortcuts/shortcut").toStringList();
QStringListIterator dataiterator(data);
QStringListIterator shortcutiterator(shortcut);
while ( dataiterator.hasNext() && shortcutiterator.hasNext())
	{
	QString d=dataiterator.next();
	if (!d.isEmpty()) shortcuts.insert(d,shortcutiterator.next());
	}

showoutputview=config->value("Show/OutputView",false).toBool();
showstructview=config->value( "Show/Structureview",true).toBool();
showpdfview=config->value( "Show/Pdfview",true).toBool();
showsourceview=config->value( "Show/Sourceview",false).toBool();
showfilesview=config->value( "Show/Filesview",false).toBool();

showPstricks=config->value( "Show/Pstricks",true).toBool();
showMp=config->value( "Show/Metapost",true).toBool();
showTikz=config->value( "Show/Tikz",true).toBool();
showAsy=config->value( "Show/Asymptote",true).toBool();
showEmphasis=config->value( "Show/Emphasis",true).toBool();
showNewline=config->value( "Show/Newline",true).toBool();
showMathmode=config->value( "Show/Mathmode",true).toBool();
showIndice=config->value( "Show/Indice",true).toBool();
showPuissance=config->value( "Show/Puissance",true).toBool();
showSmallfrac=config->value( "Show/Smallfrac",true).toBool();
showDfrac=config->value( "Show/Dfrac",true).toBool();
showRacine=config->value( "Show/Racine",true).toBool();

extra_path=config->value("Tools/ExtraPath","").toString();
useoutputdir=config->value( "Tools/OutputDir",false).toBool();
clean_exit=config->value( "Tools/CleanWhenExit",false).toBool();
quickmode=config->value( "Tools/Quick Mode",3).toInt();
QString baseName = qApp->style()->objectName();
builtinpdfview=config->value("Tools/IntegratedPdfViewer",true).toBool();
embedinternalpdf=config->value("Tools/PdfInternalViewEmbed", screen.width() > 1400).toBool();
singleviewerinstance=config->value("Tools/SingleViewerInstance",false).toBool();
htlatex_options=config->value("Tools/HtOptions","\"\" \"\" \"\" -interaction=nonstopmode").toString();

#if defined(Q_OS_MAC)

keyToggleFocus=config->value("Shortcuts/togglefocus","Ctrl+$").toString();
latex_command=config->value("Tools/Latex","\"latex\" -interaction=nonstopmode %.tex").toString();
dvips_command=config->value("Tools/Dvips","\"dvips\" -o %.ps %.dvi").toString();
ps2pdf_command=config->value("Tools/Ps2pdf","\"ps2pdf\" %.ps").toString();
makeindex_command=config->value("Tools/Makeindex","\"makeindex\" %.idx").toString();
bibtex_command=config->value("Tools/Bibtex","\"bibtex\" %.aux").toString();
pdflatex_command=config->value("Tools/Pdflatex","\"pdflatex\" -synctex=1 -interaction=nonstopmode %.tex").toString();
xelatex_command=config->value("Tools/Xelatex","\"xelatex\" -synctex=1 -interaction=nonstopmode %.tex").toString();
lualatex_command=config->value("Tools/Lualatex","\"lualatex\" -interaction=nonstopmode %.tex").toString();
if (lualatex_command.isEmpty()) lualatex_command=QString("\"lualatex\" -interaction=nonstopmode %.tex");
if (xelatex_command.isEmpty()) xelatex_command=QString("\"xelatex\" -synctex=1 -interaction=nonstopmode %.tex");
dvipdf_command=config->value("Tools/Dvipdf","\"dvipdfm\" %.dvi").toString();
metapost_command=config->value("Tools/Metapost","\"mpost\" --interaction nonstopmode ").toString();
viewdvi_command=config->value("Tools/Dvi","open %.dvi").toString();
viewps_command=config->value("Tools/Ps","open %.ps").toString();
viewpdf_command=config->value("Tools/Pdf","open %.pdf").toString();
ghostscript_command=config->value("Tools/Ghostscript","gs").toString();
asymptote_command=config->value("Tools/Asymptote","asy %.asy").toString();
latexmk_command=config->value("Tools/Latexmk","\"latexmk\" -e \"$pdflatex=q/pdflatex -synctex=1 -interaction=nonstopmode/\" -pdf %.tex").toString();
sweave_command=config->value("Tools/Sweave","R CMD Sweave %.Rnw").toString();
texdoc_command=config->value("Tools/Texdoc","texdoc").toString();
htlatex_command=config->value("Tools/Htlatex","htlatex").toString();
quick_asy_command=config->value("Tools/QuickAsy","asy -f pdf -noView %.asy|open %.pdf").toString();
lp_options=config->value("Tools/LP","-o fitplot").toString();

qApp->setStyle(QLatin1String("Fusion"));
#endif
#if defined(Q_OS_WIN32)
keyToggleFocus=config->value("Shortcuts/togglefocus","Ctrl+Space").toString();
latex_command=config->value("Tools/Latex","latex -interaction=nonstopmode %.tex").toString();
dvips_command=config->value("Tools/Dvips","dvips -Pdownload35 -o %.ps %.dvi").toString();
ps2pdf_command=config->value("Tools/Ps2pdf","ps2pdf %.ps").toString();
makeindex_command=config->value("Tools/Makeindex","makeindex %.idx").toString();
bibtex_command=config->value("Tools/Bibtex","bibtex %").toString();
//bibtex %.aux
pdflatex_command=config->value("Tools/Pdflatex","pdflatex -synctex=1 -interaction=nonstopmode %.tex").toString();
xelatex_command=config->value("Tools/Xelatex","xelatex -synctex=1 -interaction=nonstopmode %.tex").toString();
lualatex_command=config->value("Tools/Lualatex","lualatex -interaction=nonstopmode %.tex").toString();
dvipdf_command=config->value("Tools/Dvipdf","dvipdfm %.dvi").toString();
metapost_command=config->value("Tools/Metapost","mpost --interaction nonstopmode ").toString();
viewdvi_command=config->value("Tools/Dvi","\"C:/Program Files/MiKTeX 2.9/miktex/bin/yap.exe\" %.dvi").toString();
viewps_command=config->value("Tools/Ps","\"C:/Program Files/Ghostgum/gsview/gsview32.exe\" %.ps").toString();
viewpdf_command=config->value("Tools/Pdf","\"C:/Program Files/Adobe/Reader 11.0/Reader/AcroRd32.exe\" %.pdf").toString();
ghostscript_command=config->value("Tools/Ghostscript","\"C:/Program Files/gs/gs9.07/bin/gswin32c.exe\"").toString();
asymptote_command=config->value("Tools/Asymptote","\"C:/Program Files/Asymptote/asy.exe\" %.asy").toString();
latexmk_command=config->value("Tools/Latexmk","latexmk -e \"$pdflatex=q/pdflatex -synctex=1 -interaction=nonstopmode/\" -pdf %.tex").toString();
sweave_command=config->value("Tools/Sweave","C:/Program Files/R/R-2.13.2/bin/R.exe CMD Sweave %.Rnw").toString();
texdoc_command=config->value("Tools/Texdoc","texdoc").toString();
htlatex_command=config->value("Tools/Htlatex","htlatex").toString();
QString yap="C:/Program Files/MiKTeX 2.9/miktex/bin/yap.exe";
QString gsview="C:/Program Files/Ghostgum/gsview/gsview32.exe";
QString gswin="C:/Program Files/gs/gs9.21/bin/gswin32c.exe";
QString acro="C:/Program Files/Adobe/Reader 11.0/Reader/AcroRd32.exe";

if (new_user)
  {
  if (!QFileInfo(gswin).exists())
    {
    if (QFileInfo("C:/Program Files (x86)/gs/gs9.21/bin/gswin32c.exe").exists()) gswin="C:/Program Files (x86)/gs/gs9.21/bin/gswin32c.exe";
    else if (QFileInfo("C:/Program Files/gs/gs9.20/bin/gswin32c.exe").exists()) gswin="C:/Program Files/gs/gs9.20/bin/gswin32c.exe";
    else if (QFileInfo("C:/Program Files (x86)/gs/gs9.20/bin/gswin32c.exe").exists()) gswin="C:/Program Files (x86)/gs/gs9.20/bin/gswin32c.exe";
    else if (QFileInfo("C:/Program Files/gs/gs9.19/bin/gswin32c.exe").exists()) gswin="C:/Program Files/gs/gs9.19/bin/gswin32c.exe";
    else if (QFileInfo("C:/Program Files (x86)/gs/gs9.19/bin/gswin32c.exe").exists()) gswin="C:/Program Files (x86)/gs/gs9.19/bin/gswin32c.exe";
    else if (QFileInfo("C:/Program Files/gs/gs9.18/bin/gswin32c.exe").exists()) gswin="C:/Program Files/gs/gs9.18/bin/gswin32c.exe";
    else if (QFileInfo("C:/Program Files (x86)/gs/gs9.18/bin/gswin32c.exe").exists()) gswin="C:/Program Files (x86)/gs/gs9.18/bin/gswin32c.exe";
    }
  ghostscript_command="\""+gswin+"\"";
  if (!QFileInfo(yap).exists())
    {
    if (QFileInfo("C:/Program Files (x86)/MiKTeX 2.9/miktex/bin/yap.exe").exists()) yap="C:/Program Files (x86)/MiKTeX 2.9/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files/MiKTeX 3.0/miktex/bin/yap.exe").exists()) yap="C:/Program Files/MiKTeX 3.0/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files (x86)/MiKTeX 3.0/miktex/bin/yap.exe").exists()) yap="C:/Program Files (x86)/MiKTeX 3.0/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files/MiKTeX 2.8/miktex/bin/yap.exe").exists()) yap="C:/Program Files/MiKTeX 2.8/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files (x86)/MiKTeX 2.8/miktex/bin/yap.exe").exists()) yap="C:/Program Files (x86)/MiKTeX 2.8/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files/MiKTeX 2.7/miktex/bin/yap.exe").exists()) yap="C:/Program Files/MiKTeX 2.7/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files (x86)/MiKTeX 2.7/miktex/bin/yap.exe").exists()) yap="C:/Program Files (x86)/MiKTeX 2.7/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files/MiKTeX 2.5/miktex/bin/yap.exe").exists()) yap="C:/Program Files/MiKTeX 2.5/miktex/bin/yap.exe";
    else if (QFileInfo("C:/Program Files (x86)/MiKTeX 2.5/miktex/bin/yap.exe").exists()) yap="C:/Program Files (x86)/MiKTeX 2.5/miktex/bin/yap.exe";
    else if (QFileInfo("C:/texlive/2009/bin/win32/dviout.exe").exists()) yap="C:/texlive/2009/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2010/bin/win32/dviout.exe").exists()) yap="C:/texlive/2010/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2011/bin/win32/dviout.exe").exists()) yap="C:/texlive/2011/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2012/bin/win32/dviout.exe").exists()) yap="C:/texlive/2012/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2013/bin/win32/dviout.exe").exists()) yap="C:/texlive/2013/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2014/bin/win32/dviout.exe").exists()) yap="C:/texlive/2014/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2015/bin/win32/dviout.exe").exists()) yap="C:/texlive/2015/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2016/bin/win32/dviout.exe").exists()) yap="C:/texlive/2016/bin/win32/dviout.exe";
    else if (QFileInfo("C:/texlive/2017/bin/win32/dviout.exe").exists()) yap="C:/texlive/2017/bin/win32/dviout.exe";
    }
  viewdvi_command="\""+yap+"\" %.dvi";  
  if (!QFileInfo(gsview).exists())
    {
    if (QFileInfo("C:/Program Files (x86)/Ghostgum/gsview/gsview32.exe").exists()) gsview="C:/Program Files (x86)/Ghostgum/gsview/gsview32.exe";
    else if (QFileInfo("C:/texlive/2009/bin/win32/psv.exe").exists()) gsview="C:/texlive/2009/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2010/bin/win32/psv.exe").exists()) gsview="C:/texlive/2010/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2011/bin/win32/psv.exe").exists()) gsview="C:/texlive/2011/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2012/bin/win32/psv.exe").exists()) gsview="C:/texlive/2012/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2013/bin/win32/psv.exe").exists()) gsview="C:/texlive/2012/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2014/bin/win32/psv.exe").exists()) gsview="C:/texlive/2014/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2015/bin/win32/psv.exe").exists()) gsview="C:/texlive/2015/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2016/bin/win32/psv.exe").exists()) gsview="C:/texlive/2016/bin/win32/psv.exe";
    else if (QFileInfo("C:/texlive/2017/bin/win32/psv.exe").exists()) gsview="C:/texlive/2017/bin/win32/psv.exe";
    }
  viewps_command="\""+gsview+"\" %.ps";
  if (!QFileInfo(acro).exists())
    {
    if (QFileInfo("C:/Program Files (x86)/Adobe/Reader 11.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files (x86)/Adobe/Reader 11.0/Reader/AcroRd32.exe";
    else if (QFileInfo("C:/Program Files/Adobe/Reader 10.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files/Adobe/Reader 10.0/Reader/AcroRd32.exe";
    else if (QFileInfo("C:/Program Files (x86)/Adobe/Reader 10.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files (x86)/Adobe/Reader 10.0/Reader/AcroRd32.exe";
    else if (QFileInfo("C:/Program Files/Adobe/Reader 9.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files/Adobe/Reader 9.0/Reader/AcroRd32.exe";
    else if (QFileInfo("C:/Program Files (x86)/Adobe/Reader 9.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files (x86)/Adobe/Reader 9.0/Reader/AcroRd32.exe";
    else if (QFileInfo("C:/Program Files/Adobe/Reader 8.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files/Adobe/Reader 8.0/Reader/AcroRd32.exe";
    else if (QFileInfo("C:/Program Files (x86)/Adobe/Reader 8.0/Reader/AcroRd32.exe").exists()) acro="C:/Program Files (x86)/Adobe/Reader 8.0/Reader/AcroRd32.exe";
    }
  viewpdf_command="\""+acro+"\" %.pdf"; 
  }
//\"C:/Program Files/MiKTeX 2.7/miktex/bin/yap.exe\" -1 -s @%.tex %.dvi
quick_asy_command=config->value("Tools/QuickAsy","\"C:/Program Files/Asymptote/asy.exe\" -f pdf -noView %.asy|"+viewpdf_command).toString();
lp_options=config->value("Tools/LP","").toString();

qApp->setStyle(QLatin1String("Fusion"));

#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
keyToggleFocus=config->value("Shortcuts/togglefocus","Ctrl+Space").toString();
int desktop_env=1; // 1 : no kde ; 2: kde ; 3 : kde4 ; 
QStringList styles = QStyleFactory::keys();
QString kdesession= ::getenv("KDE_FULL_SESSION");
QString kdeversion= ::getenv("KDE_SESSION_VERSION");
if (!kdesession.isEmpty()) desktop_env=2;
if (!kdeversion.isEmpty()) desktop_env=3;

//desktop_env=1;
latex_command=config->value("Tools/Latex","latex -interaction=nonstopmode %.tex").toString();
dvips_command=config->value("Tools/Dvips","dvips -o %.ps %.dvi").toString();
ps2pdf_command=config->value("Tools/Ps2pdf","ps2pdf %.ps").toString();
makeindex_command=config->value("Tools/Makeindex","makeindex %.idx").toString();
bibtex_command=config->value("Tools/Bibtex","bibtex %.aux").toString();
pdflatex_command=config->value("Tools/Pdflatex","pdflatex -synctex=1 -interaction=nonstopmode %.tex").toString();
xelatex_command=config->value("Tools/Xelatex","xelatex -synctex=1 -interaction=nonstopmode %.tex").toString();
lualatex_command=config->value("Tools/Lualatex","lualatex -interaction=nonstopmode %.tex").toString();
dvipdf_command=config->value("Tools/Dvipdf","dvipdfm %.dvi").toString();
metapost_command=config->value("Tools/Metapost","mpost --interaction nonstopmode ").toString();
// xdvi %.dvi  -sourceposition @:%.tex
// kdvi "file:%.dvi#src:@ %.tex"
switch (desktop_env)
	{
	case 1:
		{
		viewdvi_command=config->value("Tools/Dvi","evince %.dvi").toString();
		viewps_command=config->value("Tools/Ps","evince %.ps").toString();
		viewpdf_command=config->value("Tools/Pdf","evince %.pdf").toString();
		}break;
	case 2:
		{
		viewdvi_command=config->value("Tools/Dvi","kdvi %.dvi").toString();
		viewps_command=config->value("Tools/Ps","kghostview %.ps").toString();
		viewpdf_command=config->value("Tools/Pdf","kpdf %.pdf").toString();
		}break;
	case 3:
		{
		viewdvi_command=config->value("Tools/Dvi","okular %.dvi").toString();
		viewps_command=config->value("Tools/Ps","okular %.ps").toString();
		viewpdf_command=config->value("Tools/Pdf","okular %.pdf").toString();
		}break;
	}



ghostscript_command=config->value("Tools/Ghostscript","gs").toString();
asymptote_command=config->value("Tools/Asymptote","asy %.asy").toString();
latexmk_command=config->value("Tools/Latexmk","latexmk -e \"$pdflatex=q/pdflatex -synctex=1 -interaction=nonstopmode/\" -pdf %.tex").toString();
sweave_command=config->value("Tools/Sweave","R CMD Sweave %.Rnw").toString();
texdoc_command=config->value("Tools/Texdoc","texdoc").toString();
htlatex_command=config->value("Tools/Htlatex","htlatex").toString();

x11style=config->value( "X11/Style","Fusion").toString();
if (xf.contains("DejaVu Sans",Qt::CaseInsensitive)) deft="DejaVu Sans";
else if (xf.contains("DejaVu Sans LGC",Qt::CaseInsensitive)) deft="DejaVu Sans LGC";
else if (xf.contains("Bitstream Vera Sans",Qt::CaseInsensitive)) deft="Bitstream Vera Sans";
else if (xf.contains("Luxi Sans",Qt::CaseInsensitive)) deft="Luxi Sans";
else deft=qApp->font().family();
x11fontfamily=config->value("X11/Font Family",deft).toString();
x11fontsize=config->value( "X11/Font Size","10").toInt();

#ifdef AUTHORIZE_LINUX_QSTYLES
if(desktop_env == 1)
{
if (styles.contains("GTK+")) qApp->setStyle(QLatin1String("gtkstyle"));
else if (styles.contains("Breeze")) qApp->setStyle(QLatin1String("breeze"));
else qApp->setStyle(QLatin1String("fusion"));    
}
else if (styles.contains("Breeze")) qApp->setStyle(QLatin1String("breeze"));
else qApp->setStyle(QLatin1String("fusion"));
#else
qApp->setStyle(QLatin1String("fusion"));
#endif
// QApplication::setPalette(QApplication::style()->standardPalette());
QFont x11Font (x11fontfamily,x11fontsize);
QApplication::setFont(x11Font);

quick_asy_command=config->value("Tools/QuickAsy","asy -f pdf -noView %.asy|"+viewpdf_command).toString();
lp_options=config->value("Tools/LP","-o fitplot").toString();
#endif
userquick_command=config->value("Tools/Userquick","latex -interaction=nonstopmode %.tex|bibtex %.aux|latex -interaction=nonstopmode %.tex|latex -interaction=nonstopmode %.tex|xdvi %.dvi").toString();
userClassList=config->value("Tools/User Class").toStringList();
userPaperList=config->value("Tools/User Paper").toStringList();
userEncodingList=config->value("Tools/User Encoding").toStringList();
userOptionsList=config->value("Tools/User Options").toStringList();
userBabelList=config->value("Tools/User Babel").toStringList();
runIndex=config->value( "Tools/Run","0").toInt();
viewIndex=config->value( "Tools/View","2").toInt();

lastDocument=config->value("Files/Last Document","").toString();
lastTemplate=config->value("Files/Last Template","").toString();
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QString scriptDir=QCoreApplication::applicationDirPath() + "/";
#else
QString scriptDir=PREFIX"/share/texmaker/";
#endif
#endif

#if defined(Q_OS_MAC)
QString scriptDir=QCoreApplication::applicationDirPath() + "/../Resources/";
#endif
#if defined(Q_OS_WIN32)
QString scriptDir=QCoreApplication::applicationDirPath() + "/";
#endif
lastScript=config->value("Files/Last Script",scriptDir).toString();
recentFilesList=config->value("Files/Recent Files New").toStringList();

input_encoding=config->value("Files/Input Encoding","UTF-8").toString();
UserMenuName[0]=config->value("User/Menu1","").toString();
UserMenuTag[0]=config->value("User/Tag1","").toString();
UserMenuName[1]=config->value("User/Menu2","").toString();
UserMenuTag[1]=config->value("User/Tag2","").toString();
UserMenuName[2]=config->value("User/Menu3","").toString();
UserMenuTag[2]=config->value("User/Tag3","").toString();
UserMenuName[3]=config->value("User/Menu4","").toString();
UserMenuTag[3]=config->value("User/Tag4","").toString();
UserMenuName[4]=config->value("User/Menu5","").toString();
UserMenuTag[4]=config->value("User/Tag5","").toString();
UserMenuName[5]=config->value("User/Menu6","").toString();
UserMenuTag[5]=config->value("User/Tag6","").toString();
UserMenuName[6]=config->value("User/Menu7","").toString();
UserMenuTag[6]=config->value("User/Tag7","").toString();
UserMenuName[7]=config->value("User/Menu8","").toString();
UserMenuTag[7]=config->value("User/Tag8","").toString();
UserMenuName[8]=config->value("User/Menu9","").toString();
UserMenuTag[8]=config->value("User/Tag9","").toString();
UserMenuName[9]=config->value("User/Menu10","").toString();
UserMenuTag[9]=config->value("User/Tag10","").toString();
UserToolName[0]=config->value("User/ToolName1","").toString();
UserToolCommand[0]=config->value("User/Tool1","").toString();
UserToolName[1]=config->value("User/ToolName2","").toString();
UserToolCommand[1]=config->value("User/Tool2","").toString();
UserToolName[2]=config->value("User/ToolName3","").toString();
UserToolCommand[2]=config->value("User/Tool3","").toString();
UserToolName[3]=config->value("User/ToolName4","").toString();
UserToolCommand[3]=config->value("User/Tool4","").toString();
UserToolName[4]=config->value("User/ToolName5","").toString();
UserToolCommand[4]=config->value("User/Tool5","").toString();

userTagsList=config->value("User/TagList").toStringList();

struct_level1=config->value("Structure/Structure Level 1","part").toString();
struct_level2=config->value("Structure/Structure Level 2","chapter").toString();
struct_level3=config->value("Structure/Structure Level 3","section").toString();
struct_level4=config->value("Structure/Structure Level 4","subsection").toString();
struct_level5=config->value("Structure/Structure Level 5","subsubsection").toString();




document_class=config->value("Quick/Class","article").toString();
typeface_size=config->value("Quick/Typeface","10pt").toString();
paper_size=config->value("Quick/Papersize","a4paper").toString();
document_encoding=config->value("Quick/Encoding","utf8").toString();
ams_packages=config->value( "Quick/AMS",true).toBool();
makeidx_package=config->value( "Quick/MakeIndex",false).toBool();
babel_package=config->value( "Quick/Babel",false).toBool();

QString locale = QString(QLocale::system().name()).left(2);
if (locale=="en") babel_default=config->value("Quick/BabelDefault","english").toString();
else if (locale=="de") babel_default=config->value("Quick/BabelDefault","german").toString();
else if (locale=="fr") babel_default=config->value("Quick/BabelDefault","french").toString();
else if (locale=="ru") babel_default=config->value("Quick/BabelDefault","russian").toString();
else if (locale=="it") babel_default=config->value("Quick/BabelDefault","italian").toString();
else if (locale=="es") babel_default=config->value("Quick/BabelDefault","spanish").toString();
else if (locale=="pl") babel_default=config->value("Quick/BabelDefault","polish").toString();
else if (locale=="fa") babel_default=config->value("Quick/BabelDefault","farsi").toString();
else if (locale=="cz") babel_default=config->value("Quick/BabelDefault","czech").toString();
else if (locale=="pt") babel_default=config->value("Quick/BabelDefault","portuguese").toString();
else if (locale=="sl") babel_default=config->value("Quick/BabelDefault","slovak").toString();
else if (locale=="gr") babel_default=config->value("Quick/BabelDefault","greek").toString();
else if (locale=="fi") babel_default=config->value("Quick/BabelDefault","finish").toString();
else if (locale=="hu") babel_default=config->value("Quick/BabelDefault","magyar").toString();
else babel_default=config->value("Quick/BabelDefault","").toString();

geometry_package=config->value( "Quick/Geometry",false).toBool();
graphicx_package=config->value( "Quick/Graphicx",false).toBool();
lmodern_package=config->value( "Quick/Lmodern",false).toBool();
kpfonts_package=config->value( "Quick/Kpfonts",false).toBool();
fourier_package=config->value( "Quick/Fourier",false).toBool();

author=config->value("Quick/Author","").toString();
geometry_options=config->value("Quick/GeometryOptions","left=2cm,right=2cm,top=2cm,bottom=2cm").toString();

beamer_theme=config->value("Beamer/Theme","Warsaw").toString();
beamer_size=config->value("Beamer/Size","11pt").toString();
beamer_encoding=config->value("Beamer/Encoding","utf8").toString();
beamer_author=config->value("Beamer/Author","").toString();
if (locale=="en") beamer_babel=config->value("Beamer/BabelDefault","english").toString();
else if (locale=="de") beamer_babel=config->value("Beamer/BabelDefault","german").toString();
else if (locale=="fr") beamer_babel=config->value("Beamer/BabelDefault","french").toString();
else if (locale=="ru") beamer_babel=config->value("Beamer/BabelDefault","russian").toString();
else if (locale=="it") beamer_babel=config->value("Beamer/BabelDefault","italian").toString();
else if (locale=="es") beamer_babel=config->value("Beamer/BabelDefault","spanish").toString();
else if (locale=="pl") beamer_babel=config->value("Beamer/BabelDefault","polish").toString();
else if (locale=="fa") beamer_babel=config->value("Beamer/BabelDefault","farsi").toString();
else if (locale=="cz") beamer_babel=config->value("Beamer/BabelDefault","czech").toString();
else if (locale=="pt") beamer_babel=config->value("Beamer/BabelDefault","portuguese").toString();
else if (locale=="sl") beamer_babel=config->value("Beamer/BabelDefault","slovak").toString();
else if (locale=="gr") beamer_babel=config->value("Beamer/BabelDefault","greek").toString();
else if (locale=="fi") beamer_babel=config->value("Beamer/BabelDefault","finish").toString();
else if (locale=="hu") beamer_babel=config->value("Beamer/BabelDefault","magyar").toString();
else beamer_babel=config->value("Beamer/BabelDefault","").toString();


#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QString dicDir=QCoreApplication::applicationDirPath() + "/";
#else
#ifdef DEBIAN_SPELLDIR
QString dicDir=PREFIX"/share/myspell/dicts/";
#else
QString dicDir=PREFIX"/share/texmaker/";
#endif
#endif

#endif
#if defined(Q_OS_MAC)
QString dicDir=QCoreApplication::applicationDirPath() + "/../Resources/";
#endif
#if defined(Q_OS_WIN32)
QString dicDir=QCoreApplication::applicationDirPath() + "/";
#endif
QString defaultDic=dicDir+QString(QLocale::system().name())+".dic";
QFileInfo fi(defaultDic);
if (!fi.exists() || !fi.isReadable()) defaultDic=dicDir+"en_GB.dic";
spell_dic=config->value("Spell/Dic",defaultDic).toString();
QFileInfo fispell(spell_dic);
pdfCheckerLang=fispell.fileName().left(2);
if (!pdfCheckerLang.contains(QRegExp("(de|en|es|fr|id|it|nl|pl|pt|ru)"))) pdfCheckerLang="en";

spell_ignored_words=config->value("Spell/Words","").toString();
inlinespellcheck=config->value( "Spell/Inline",true).toBool();
for (int i=0; i <412 ; i++)
	{
	symbolScore[i]=config->value( "Symbols/symbol"+QString::number(i),0).toInt();
	}
favoriteSymbolList.clear();
QList<QVariant> favoriteSymbolSettings = config->value( "Symbols/Favorites" ).toList( );
if( !favoriteSymbolSettings.isEmpty())
	{
	for( int i = 0; i < favoriteSymbolSettings.count( ); i++ ) favoriteSymbolList.append(favoriteSymbolSettings.at(i).toInt());
	}
	

	
colorBackground=config->value("Color/Background",QColor("#FFFFFF")).value<QColor>();
colorLine=config->value("Color/Line",QColor("#ececec")).value<QColor>();
colorHighlight=config->value("Color/Highlight",QColor("#FF0000")).value<QColor>();
colorStandard=config->value("Color/Standard",QColor("#000000")).value<QColor>();
colorComment=config->value("Color/Comment",QColor("#606060")).value<QColor>();
colorMath=config->value("Color/Math",QColor("#008000")).value<QColor>();
colorCommand=config->value("Color/Command",QColor("#800000")).value<QColor>();
colorKeyword=config->value("Color/Keyword",QColor("#0000CC")).value<QColor>();
colorVerbatim=config->value("Color/Verbatim",QColor("#9A4D00")).value<QColor>();
colorTodo=config->value("Color/Todo",QColor("#FF0000")).value<QColor>();
colorKeywordGraphic=config->value("Color/KeywordGraphic",QColor("#006699")).value<QColor>();
colorNumberGraphic=config->value("Color/NumberGraphic",QColor("#660066")).value<QColor>();

config->endGroup();
}

void Texmaker::SaveSettings()
{
SaveLastSession();
#ifdef USB_VERSION
QSettings config(QCoreApplication::applicationDirPath()+"/texmaker.ini",QSettings::IniFormat); //for USB-stick version 
#else
QSettings config(QSettings::IniFormat,QSettings::UserScope,"xm1","texmaker");
#endif

config.setValue( "IniMode",true);
config.beginGroup( "texmaker" );
QList<int> sizes;
QList<int>::Iterator it;

config.setValue( "GUI/Style",modern_style);
config.setValue( "GUI/New Version",true);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
config.setValue( "X11/Style",x11style);
config.setValue("X11/Font Family",x11fontfamily);
config.setValue( "X11/Font Size",x11fontsize);
#endif

config.setValue("MainWindowState",saveState(0));
config.setValue("MainWindowMaximized",windowState().testFlag(Qt::WindowMaximized)); 
config.setValue("Splitter1State",splitter1->saveState());
config.setValue("Splitter2State",splitter2->saveState());
config.setValue("Splitter3State",splitter3->saveState());
config.setValue("Geometries/MainwindowWidth", width() );
config.setValue("Geometries/MainwindowHeight", height() );
config.setValue("Geometries/MainwindowX", x() );
config.setValue("Geometries/MainwindowY", y() );

config.setValue("Print/PaperSize",psize);
int pscale=10;
if (builtinpdfview)
  {
  if (pdfviewerWindow) pscale= (int) (pdfviewerWindow->getScale()*100);
  if (embedinternalpdf && pdfviewerWidget) pscale= (int) (pdfviewerWidget->getScale()*100);
  }

config.setValue("PdfView/Scale",pscale);

config.setValue("Editor/Font Family",EditorFont.family());
config.setValue( "Editor/Font Size",EditorFont.pointSize());

config.setValue( "Editor/TabSpaces",tabspaces);
config.setValue( "Editor/TabWidth",tabwidth);
config.setValue( "Editor/WordWrap",wordwrap);
config.setValue( "Editor/Watcher",watchfiles);
config.setValue( "Editor/AutoSave",autosave);
config.setValue( "Editor/Parentheses Matching",parenmatch);
config.setValue( "Editor/Line Numbers",showline);
config.setValue( "Editor/Completion",completion);
config.setValue( "Editor/UserCompletion",userCompletionList);
config.setValue( "Editor/SvnEnable",svnEnable);
config.setValue( "Editor/SvnPath",svnPath);
QStringList data,shortcut;
// data.clear();
// shortcut.clear();
KeysMap::Iterator its;

for( its = shortcuts.begin(); its != shortcuts.end(); ++its )
	{
	data.append(its.key());
	shortcut.append(its.value());
	}
config.setValue("Shortcuts/data",data);
config.setValue("Shortcuts/shortcut",shortcut);
config.setValue("Shortcuts/togglefocus",keyToggleFocus);

config.setValue("Show/OutputView",showoutputview);
config.setValue( "Show/Structureview",showstructview);
config.setValue( "Show/Pdfview",showpdfview);
config.setValue( "Show/Sourceview",showsourceview);
config.setValue( "Show/Filesview",showfilesview);

config.setValue( "Show/Pstricks",showPstricks);
config.setValue( "Show/Metapost",showMp);
config.setValue( "Show/Tikz",showTikz);
config.setValue( "Show/Asymptote",showAsy);

config.setValue( "Show/Emphasis",showEmphasis);
config.setValue( "Show/Newline",showNewline);
config.setValue( "Show/Mathmode",showMathmode);
config.setValue( "Show/Indice",showIndice);
config.setValue( "Show/Puissance",showPuissance);
config.setValue( "Show/Smallfrac",showSmallfrac);
config.setValue( "Show/Dfrac",showDfrac);
config.setValue( "Show/Racine",showRacine);

config.setValue("Tools/ExtraPath",extra_path);
config.setValue("Tools/OutputDir",useoutputdir);
config.setValue("Tools/CleanWhenExit",clean_exit);

config.setValue("Tools/Quick Mode",quickmode);
config.setValue("Tools/Latex",latex_command);
config.setValue("Tools/Dvi",viewdvi_command);
config.setValue("Tools/Dvips",dvips_command);
config.setValue("Tools/Ps",viewps_command);
config.setValue("Tools/Ps2pdf",ps2pdf_command);
config.setValue("Tools/Makeindex",makeindex_command);
config.setValue("Tools/Bibtex",bibtex_command);
config.setValue("Tools/Pdflatex",pdflatex_command);
config.setValue("Tools/Xelatex",xelatex_command);
config.setValue("Tools/Lualatex",lualatex_command);
config.setValue("Tools/Pdf",viewpdf_command);
config.setValue("Tools/Dvipdf",dvipdf_command);
config.setValue("Tools/Metapost",metapost_command);
config.setValue("Tools/Ghostscript",ghostscript_command);
config.setValue("Tools/Asymptote",asymptote_command);
config.setValue("Tools/Latexmk",latexmk_command);
config.setValue("Tools/Sweave",sweave_command);
config.setValue("Tools/Texdoc",texdoc_command);
config.setValue("Tools/HtOptions",htlatex_options); 
config.setValue("Tools/Htlatex",htlatex_command); 
config.setValue("Tools/Userquick",userquick_command);
config.setValue("Tools/QuickAsy",quick_asy_command);
config.setValue("Tools/LP",lp_options);

if (userClassList.count()>0) config.setValue("Tools/User Class",userClassList); 
if (userPaperList.count()>0) config.setValue("Tools/User Paper",userPaperList); 
if (userEncodingList.count()>0) config.setValue("Tools/User Encoding",userEncodingList); 
if (userOptionsList.count()>0) config.setValue("Tools/User Options",userOptionsList);
if (userBabelList.count()>0) config.setValue("Tools/User Babel",userBabelList); 
config.setValue( "Tools/Run",comboCompil->currentIndex());
config.setValue( "Tools/View",comboView->currentIndex());
config.setValue("Tools/IntegratedPdfViewer",builtinpdfview);
config.setValue("Tools/PdfInternalViewEmbed",embedinternalpdf);
config.setValue("Tools/SingleViewerInstance",singleviewerinstance);


config.setValue("Files/Last Document",lastDocument);
config.setValue("Files/Last Template",lastTemplate);
config.setValue("Files/Last Script",lastScript);
/*if (recentFilesList.count()>0)*/ config.setValue("Files/Recent Files New",recentFilesList); 

config.setValue("Files/Input Encoding", input_encoding);

config.setValue("User/Menu1",UserMenuName[0]);
config.setValue("User/Tag1",UserMenuTag[0]);
config.setValue("User/Menu2",UserMenuName[1]);
config.setValue("User/Tag2",UserMenuTag[1]);
config.setValue("User/Menu3",UserMenuName[2]);
config.setValue("User/Tag3",UserMenuTag[2]);
config.setValue("User/Menu4",UserMenuName[3]);
config.setValue("User/Tag4",UserMenuTag[3]);
config.setValue("User/Menu5",UserMenuName[4]);
config.setValue("User/Tag5",UserMenuTag[4]);
config.setValue("User/Menu6",UserMenuName[5]);
config.setValue("User/Tag6",UserMenuTag[5]);
config.setValue("User/Menu7",UserMenuName[6]);
config.setValue("User/Tag7",UserMenuTag[6]);
config.setValue("User/Menu8",UserMenuName[7]);
config.setValue("User/Tag8",UserMenuTag[7]);
config.setValue("User/Menu9",UserMenuName[8]);
config.setValue("User/Tag9",UserMenuTag[8]);
config.setValue("User/Menu10",UserMenuName[9]);
config.setValue("User/Tag10",UserMenuTag[9]);
config.setValue("User/ToolName1",UserToolName[0]);
config.setValue("User/Tool1",UserToolCommand[0]);
config.setValue("User/ToolName2",UserToolName[1]);
config.setValue("User/Tool2",UserToolCommand[1]);
config.setValue("User/ToolName3",UserToolName[2]);
config.setValue("User/Tool3",UserToolCommand[2]);
config.setValue("User/ToolName4",UserToolName[3]);
config.setValue("User/Tool4",UserToolCommand[3]);
config.setValue("User/ToolName5",UserToolName[4]);
config.setValue("User/Tool5",UserToolCommand[4]);

config.setValue("User/TagList",userTagsList);

config.setValue("Structure/Structure Level 1",struct_level1);
config.setValue("Structure/Structure Level 2",struct_level2);
config.setValue("Structure/Structure Level 3",struct_level3);
config.setValue("Structure/Structure Level 4",struct_level4);
config.setValue("Structure/Structure Level 5",struct_level5);

config.setValue( "Quick/Class",document_class);
config.setValue( "Quick/Typeface",typeface_size);
config.setValue( "Quick/Papersize",paper_size);
config.setValue( "Quick/Encoding",document_encoding);
config.setValue( "Quick/AMS",ams_packages);
config.setValue( "Quick/MakeIndex",makeidx_package);
config.setValue( "Quick/Babel",babel_package);
config.setValue( "Quick/BabelDefault",babel_default);
config.setValue( "Quick/Geometry",geometry_package);
config.setValue( "Quick/Graphicx",graphicx_package);
config.setValue( "Quick/Lmodern",lmodern_package);
config.setValue( "Quick/Kpfonts",kpfonts_package);
config.setValue( "Quick/Fourier",fourier_package);



config.setValue( "Quick/Author",author);
config.setValue( "Quick/GeometryOptions",geometry_options);

config.setValue( "Beamer/Theme",beamer_theme);
config.setValue( "Beamer/Size",beamer_size);
config.setValue( "Beamer/Encoding",beamer_encoding);
config.setValue( "Beamer/Author",beamer_author);
config.setValue( "Beamer/BabelDefault",beamer_babel);

config.setValue( "Spell/Dic",spell_dic);
config.setValue( "Spell/Words",spell_ignored_words);
config.setValue( "Spell/Inline",inlinespellcheck);

for (int i=0; i <412 ; i++)
	{
	config.setValue( "Symbols/symbol"+QString::number(i),symbolScore[i]);
	}
QList<QVariant> favoriteSymbolSettings;
if( !favoriteSymbolList.isEmpty())
	{
	for( int i = 0; i < favoriteSymbolList.count( ); i++ ) favoriteSymbolSettings.append(favoriteSymbolList.at(i));
	}
config.setValue("Symbols/Favorites",favoriteSymbolSettings);
config.setValue("Color/Background",colorBackground);
config.setValue("Color/Line",colorLine);
config.setValue("Color/Highlight",colorHighlight);
config.setValue("Color/Standard",colorStandard);
config.setValue("Color/Comment",colorComment);
config.setValue("Color/Math",colorMath);
config.setValue("Color/Command",colorCommand);
config.setValue("Color/Keyword",colorKeyword);
config.setValue("Color/Verbatim",colorVerbatim);
config.setValue("Color/Todo",colorTodo);
config.setValue("Color/KeywordGraphic",colorKeywordGraphic);
config.setValue("Color/NumberGraphic",colorNumberGraphic);


config.endGroup();
}

void Texmaker::DeleteSettings()
{
switch(  QMessageBox::warning(this, "Texmaker",
				tr("Delete settings file?\n(Texmaker will be closed and you will have to restart it)"),
				tr("Ok"), tr("Cancel"),
				0,
				1 ) )
	{
	case 0:
		eraseSettings=true;
		fileExit();
		break;
	case 1:
	default:
		return;
		break;
	}
}

void Texmaker::CopySettings()
{
QFileInfo fi_or(settingsFileName);
if (fi_or.exists())
  {
  QFile fichier_or(settingsFileName);
  QString to_file = QFileDialog::getSaveFileName(this,tr("Save As"),QDir::homePath(),"Setting files (*.ini);;All files (*.*)");
  if ( !to_file.isEmpty() )
    {
    QFileInfo fi_dest(to_file);
    if (fi_dest.exists())
	    {
	    QFile fichier_dest(to_file);
	    fichier_dest.remove();
	    fichier_or.copy(to_file);
	    }
    else
	    {
	    fichier_or.copy(to_file);
	    }
    }
  }
}

void Texmaker::ReplaceSettings()
{
switch(  QMessageBox::warning(this, "Texmaker",
				tr("Replace settings file by a new one?\n(Texmaker will be closed and you will have to restart it)"),
				tr("Ok"), tr("Cancel"),
				0,
				1 ) )
	{
	case 0:
		replaceSettings=true;
		fileExit();
		break;
	case 1:
	default:
		return;
		break;
	}
}

void Texmaker::setPrintPaperSize(const QString &p)
{
psize=p;
}

////////////////// STRUCTURE ///////////////////
void Texmaker::ShowOpenedFiles()
{
LeftPanelStackedWidget->setCurrentWidget(OpenedFilesListWidget);
titleLeftPanel->setText(tr("Opened Files").toUpper());
}

void Texmaker::OpenedFileActivated(QListWidgetItem *item)
{
int index=OpenedFilesListWidget->currentRow();
disconnect(comboFiles, SIGNAL(activated(int)), this, SLOT(listSelectionActivated(int)));
if ((index>-1) && (index<comboFiles->count())) comboFiles->setCurrentIndex(index);
connect(comboFiles, SIGNAL(activated(int)), this, SLOT(listSelectionActivated(int)));
QString f=comboFiles->itemData(index, Qt::UserRole).toString();
FilesMap::Iterator it;
QString fw32,funix,forig;
for( it = filenames.begin(); it != filenames.end(); ++it )
	{
	forig=filenames[it.key()];
	fw32=filenames[it.key()];
	funix=filenames[it.key()];
	fw32.replace(QString("\\"),QString("/"));
	funix.replace(QString("/"),QString("\\"));
	if ( (forig==f) || (fw32==f) || (funix==f)) 
		{
		EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
		}
	}
}
void Texmaker::ShowStructure()
{
LeftPanelStackedWidget->setCurrentWidget(StructureTreeWidget);
titleLeftPanel->setText(tr("Structure").toUpper());
}
void Texmaker::ShowRelation() //RelationListWidget
{
LeftPanelStackedWidget->setCurrentWidget(RelationListWidget);
titleLeftPanel->setText(tr("Relation symbols").toUpper());
}
void Texmaker::ShowArrow() //ArrowListWidget
{
LeftPanelStackedWidget->setCurrentWidget(ArrowListWidget);
titleLeftPanel->setText(tr("Arrow symbols").toUpper());
}
void Texmaker::ShowMisc() //MiscellaneousListWidget
{
LeftPanelStackedWidget->setCurrentWidget(MiscellaneousListWidget);
titleLeftPanel->setText(tr("Miscellaneous symbols").toUpper());
}
void Texmaker::ShowDelim() //DelimitersListWidget
{
LeftPanelStackedWidget->setCurrentWidget(DelimitersListWidget);
titleLeftPanel->setText(tr("Delimiters").toUpper());
}
void Texmaker::ShowGreek() //GreekListWidget
{
LeftPanelStackedWidget->setCurrentWidget(GreekListWidget);
titleLeftPanel->setText(tr("Greek letters").toUpper());
}
void Texmaker::ShowMostUsed() //MostUsedListWidget
{
LeftPanelStackedWidget->setCurrentWidget(MostUsedListWidget);
titleLeftPanel->setText(tr("Most used symbols").toUpper());
}
void Texmaker::ShowFavorite() //FavoriteListWidget
{
LeftPanelStackedWidget->setCurrentWidget(FavoriteListWidget);
titleLeftPanel->setText(tr("Favorites symbols").toUpper());
}
void Texmaker::ShowPstricks() //PsListWidget
{
LeftPanelStackedWidget->setCurrentWidget(PsListWidget);
titleLeftPanel->setText(tr("Pstricks Commands").toUpper());
}
void Texmaker::ShowLeftRight() //leftrightWidget
{
LeftPanelStackedWidget->setCurrentWidget(leftrightWidget);
titleLeftPanel->setText(QString("left/right").toUpper());
}
void Texmaker::ShowMplist() //MpListWidget
{
LeftPanelStackedWidget->setCurrentWidget(MpListWidget);
titleLeftPanel->setText(tr("MetaPost Commands").toUpper());
}
void Texmaker::ShowTikz() //TikzWidget
{
LeftPanelStackedWidget->setCurrentWidget(tikzWidget);
titleLeftPanel->setText(tr("Tikz Commands").toUpper());
}
void Texmaker::ShowAsy() //AsyWidget
{
LeftPanelStackedWidget->setCurrentWidget(asyWidget);
titleLeftPanel->setText(tr("Asymptote Commands").toUpper());
}

void Texmaker::ShowUserPanel()
{
LeftPanelStackedWidget->setCurrentWidget(usertagsListWidget);
titleLeftPanel->setText(tr("User").toUpper());
}

void Texmaker::UpdateStructure()
{
QFont deft=QFont("DejaVu Sans Condensed",qApp->font().pointSize());

QTreeWidgetItem *Child,*parent_level[5], *theitem;
QString current;
if (StructureTreeWidget->currentItem()) current=StructureTreeWidget->currentItem()->text(0);

if ( !currentEditorView() ) 
  {
  StructureTreeWidget->clear();
  return;
  }

QString shortName = getName();
if ((shortName.right(4)!=".tex") && (shortName.right(4)!=".Rnw") && (!shortName.startsWith("untitled")))  return;
/*************************************/
QList<QTreeWidgetItem *> fItems;
bool islabels_expanded=false;
bool isblocks_expanded=true;
fItems=StructureTreeWidget->findItems ("LABELS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      islabels_expanded=fItems.at(0)->isExpanded();
      }
  }
fItems.clear();
fItems=StructureTreeWidget->findItems ("BLOCKS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      isblocks_expanded=fItems.at(0)->isExpanded();
      }
  }  
/*************************************/
StructureTreeWidget->clear();
int pos;
while ( (pos = (int)shortName.indexOf('/')) != -1 )
shortName.remove(0,pos+1);
QTreeWidgetItem *top = new QTreeWidgetItem(StructureTreeWidget);
top->setIcon(0,getIcon(":/images/doc.png"));
top->setText(0,shortName);
top->setFont(0,deft);
Child=parent_level[0]=parent_level[1]=parent_level[2]=parent_level[3]=parent_level[4]=top;
labelitem.clear();
if (singlemode) 
  {
  listbibfiles.clear();
  listchildfiles.clear();
  }
QTreeWidgetItem *toplabel = new QTreeWidgetItem(top);
toplabel->setText(0,"LABELS");
toplabel->setFont(0,deft);
QTreeWidgetItem *blocklabel = new QTreeWidgetItem(top);
blocklabel->setText(0,"BLOCKS");
blocklabel->setFont(0,deft);
QString s;
QTextBlock p = currentEditorView()->editor->document()->begin();
const QList<StructItem>& structure = currentEditorView()->editor->getStructItems();
//int i;
for (int j = 0; j < structure.count(); j++)
{
//i=structure.at(j).cursor.block().blockNumber();
      switch (structure.at(j).type)
	{
	case 0:
		{
		s=structure.at(j).item;
		Child = new QTreeWidgetItem(blocklabel);
		Child->setText(0,s);
		Child->setFont(0,deft);
		Child->setText(1,QString::number(j));
		StructureTreeWidget->expandItem(Child);
		}break;
	case 1:
		{
		s=structure.at(j).item;
		labelitem.append(s);
		Child = new QTreeWidgetItem(toplabel);
		Child->setText(0,s);
		Child->setFont(0,deft);
		Child->setText(1,QString::number(j));
		StructureTreeWidget->expandItem(Child);
		}break;
	case 2:
		{
		s=structure.at(j).item;
		Child = new QTreeWidgetItem(top);
		Child->setText(0,s);
		Child->setFont(0,deft);
		Child->setText(1,QString::number(j));
		Child->setIcon(0,getIcon(":/images/include.png"));
		if (listchildfiles.indexOf(s)<0) addIncludeFiles(s);
		}break;
	case 3:
		{
		s=structure.at(j).item;
		Child = new QTreeWidgetItem(top);
		Child->setText(0,s);
		Child->setFont(0,deft);
		Child->setText(1,QString::number(j));
		Child->setIcon(0,getIcon(":/images/include.png"));
		if (listchildfiles.indexOf(s)<0) addIncludeFiles(s);
		}break;
	case 4:
		{
		s=structure.at(j).item;
		parent_level[0] = new QTreeWidgetItem(top);
		parent_level[0]->setText(0,s);
		parent_level[0]->setFont(0,deft);
		parent_level[0]->setText(1,QString::number(j));
		parent_level[0]->setIcon(0,getIcon(":/images/part.png"));
		StructureTreeWidget->expandItem(parent_level[0]);
		parent_level[1]=parent_level[2]=parent_level[3]=parent_level[4]=parent_level[0];
		}break;
	case 5:
		{
		s=structure.at(j).item;
		parent_level[1] = new QTreeWidgetItem(parent_level[0]);
		parent_level[1]->setText(0,s);
		parent_level[1]->setFont(0,deft);
		parent_level[1]->setText(1,QString::number(j));
		parent_level[1]->setIcon(0,getIcon(":/images/chapter.png"));
		StructureTreeWidget->expandItem(parent_level[1]);
		parent_level[2]=parent_level[3]=parent_level[4]=parent_level[1];
		}break;
	case 6:
		{
		s=structure.at(j).item;
		parent_level[2] = new QTreeWidgetItem(parent_level[1]);
		parent_level[2]->setText(0,s);
		parent_level[2]->setFont(0,deft);
		parent_level[2]->setText(1,QString::number(j));
		parent_level[2]->setIcon(0,getIcon(":/images/section.png"));
		StructureTreeWidget->expandItem(parent_level[2]);
		parent_level[3]=parent_level[4]=parent_level[2];
		}break;
	case 7:
		{
		s=structure.at(j).item;
		parent_level[3] = new QTreeWidgetItem(parent_level[2]);
		parent_level[3]->setText(0,s);
		parent_level[3]->setFont(0,deft);
		parent_level[3]->setText(1,QString::number(j));
		//parent_level[3]->setIcon(0,getIcon(":/images/subsection.png"));
		parent_level[4]=parent_level[3];
		}break;
	case 8:
		{
		s=structure.at(j).item;
		parent_level[4] = new QTreeWidgetItem(parent_level[3]);
		parent_level[4]->setText(0,s);
		parent_level[4]->setFont(0,deft);
		parent_level[4]->setText(1,QString::number(j));
		//parent_level[4]->setIcon(0,getIcon(":/images/subsubsection.png"));
		}break;
	case 9:
		{
		s=structure.at(j).item;
		Child = new QTreeWidgetItem(top);
		Child->setText(0,s);
		Child->setFont(0,deft);
		Child->setText(1,QString::number(j));
		Child->setIcon(0,getIcon(":/images/include.png"));
		if (listbibfiles.indexOf(s)<0) addBibFiles(s);
		}break;
	}
}	

if (!current.isEmpty()) 
	{
	QList<QTreeWidgetItem *> fItems=StructureTreeWidget->findItems (current,Qt::MatchRecursive,0);
	if ((fItems.size()>0 ) && (fItems.at(0))) 
		{
		StructureTreeWidget->setCurrentItem(fItems.at(0));
		theitem=fItems.at(0)->parent();
		while ((theitem) && (theitem!=top)) 
			{	
			StructureTreeWidget->expandItem(theitem);
			theitem=theitem->parent();
			}
		}
	}
StructureTreeWidget->setItemExpanded (top,true);
//StructureTreeWidget->setItemExpanded (toplabel,false);
//StructureTreeWidget->setItemExpanded (blocklabel,true);
/*************************************************/
fItems.clear();
fItems=StructureTreeWidget->findItems ("LABELS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      StructureTreeWidget->setItemExpanded(fItems.at(0),islabels_expanded);
      }
  }
fItems.clear();
fItems=StructureTreeWidget->findItems ("BLOCKS",Qt::MatchRecursive,0);
if (fItems.size()>0 ) 
  {
  if (fItems.at(0)) 
      {
      StructureTreeWidget->setItemExpanded (fItems.at(0),isblocks_expanded);
      }
  }  

/************************************************/

currentEditorView()->editor->foldableLines.clear();
int endpreamble = currentEditorView()->editor->searchLine("\\begin{document}");
if (endpreamble>1) currentEditorView()->editor->foldableLines.insert(0,endpreamble-1);
ParseTree(top);
UpdateChildsLabels(listchildfiles);
updateCompleter();
if (currentEditorView())
  {
  if (completion) currentEditorView()->editor->setCompleter(completer);
  else currentEditorView()->editor->setCompleter(0);
  currentEditorView()->editor->matchAll();
  }
}

void Texmaker::UpdateChildsLabels(QStringList listfiles)
{
if ( !currentEditorView() ) return;
QString shortName = getName();
if ((shortName.right(4)!=".tex") && (!shortName.startsWith("untitled")))  return;
//currentEditorView()->editor->highlighter->rehighlight();
int pos;
while ( (pos = (int)shortName.indexOf('/')) != -1 )
shortName.remove(0,pos+1);
/*********/
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
QString name=fi.absoluteFilePath();
QString flname=fi.fileName();
QString basename=name.left(name.length()-flname.length());
if (listfiles.count()>0)
  {
   QString fname;
   QRegExp macroName("\\label\\{(.*)\\}", Qt::CaseInsensitive);
   macroName.setMinimal(true);
   QString line;
  for ( int i = 0; i <listfiles.count(); i++ )
      {
      fname=listfiles.at(i);
      QFileInfo fi(fname);
      if (fi.exists() && fi.isReadable()) 
	{
	QFile f(fname);
	if ( f.open(QIODevice::ReadOnly) )
		{
		QTextStream t( &f );
		while ( !t.atEnd() )
			{
			line=t.readLine();
			line=line.simplified();
			if (!line.isEmpty()) 
			    {
			    if (macroName.indexIn(line)>-1) {labelitem.append(macroName.cap(1).trimmed());}
			    }
			}
		}
		f.close();
	}
      }
  }
}

void Texmaker::UpdateBibliography()
{
if ( !currentEditorView() ) return;
bibitem.clear();
if (listbibfiles.count()>0)
  {
   QString fname;
     QStringList types;
types << QLatin1String("article") << QLatin1String("book")
  << QLatin1String("booklet") << QLatin1String("inbook")
  << QLatin1String("incollection") << QLatin1String("inproceedings")
  << QLatin1String("manual") << QLatin1String("mastersthesis")
  << QLatin1String("misc") << QLatin1String("phdthesis")
  << QLatin1String("proceedings") << QLatin1String("techreport")
  << QLatin1String("unpublished") << QLatin1String("periodical")
  << QLatin1String("conference") << QLatin1String("mvbook")
  << QLatin1String("collection") << QLatin1String("mvcollection")
  << QLatin1String("online") << QLatin1String("mvproceedings")
  << QLatin1String("inproceedings") << QLatin1String("report")
  << QLatin1String("thesis") << QLatin1String("electronic") << QLatin1String("patent");
   QRegExp macroName("@("+types.join("|")+")\\s*\\{\\s*(.*),", Qt::CaseInsensitive);
   macroName.setMinimal(true);
   QString line;
  for ( int i = 0; i <listbibfiles.count(); i++ )
      {
      fname=listbibfiles.at(i);
      QFileInfo fi(fname);
      if (fi.exists() && fi.isReadable()) 
	{
	QFile f(fname);
	if ( f.open(QIODevice::ReadOnly) )
		{
		QTextStream t( &f );
		while ( !t.atEnd() )
			{
			line=t.readLine();
			line=line.simplified();
			if (!line.isEmpty()) 
			    {
			    if (macroName.indexIn(line)>-1) bibitem.append(macroName.cap(2).trimmed());
			    }
			}
		}
		f.close();
	}
      }
  }
/************/
updateCompleter();
if (currentEditorView())
  {
  if (completion) currentEditorView()->editor->setCompleter(completer);
  else currentEditorView()->editor->setCompleter(0);
  currentEditorView()->editor->matchAll();
  }
}

void Texmaker::ParseTree(QTreeWidgetItem *item)
{

  if (item->childCount()>0)
	{
	for (int i = 0; i < item->childCount(); i++) 
		{
		ItemToRange(item->child(i));
		}
	}
 
  if (item->childCount()>0)
	{
	for (int i = 0; i < item->childCount(); i++) 
		{
		ParseTree(item->child(i));
		}
	}
 
}

int Texmaker::LevelItem(const QTreeWidgetItem *item)
{
int level=0;
const QList<StructItem>& structure = currentEditorView()->editor->getStructItems();
if ((item) && (structure.count()>0))
  {
  int index = item->text(1).toInt();
  if (index<structure.count())
    {
    if ((structure.at(index).type<=3) || (structure.at(index).type==9) ) level=0;
    else level=structure.at(index).type-3;
    }
  }
return level;

/*if ((item) && (!structlist.isEmpty()))
  {
  QStringList::ConstIterator it1 = structitem.begin();
  QList<int>::ConstIterator it3 = structlevel.begin();
  for ( ; it1 !=structitem.end(); ++it1 )
	  {
	  if (*it1==item->text(0)) break;
	  ++it3;
	  }
  level=*it3;
  }
return level;*/
}

int Texmaker::LineItem(const QTreeWidgetItem *item)
{
int line=-1;
const QList<StructItem>& structure = currentEditorView()->editor->getStructItems();
if ((item) && (structure.count()>0))
  {
  int index = item->text(1).toInt();
  if (index<structure.count())
    {
    line=structure.at(index).cursor.block().blockNumber();//line=structure.at(index).line;
    }
  }
return line;

/*if ((item) && (!structlist.isEmpty()))
  {
  QStringList::ConstIterator it1 = structitem.begin();
  QStringList::ConstIterator it2 = structlist.begin();
  for ( ; it1 !=structitem.end(); ++it1 )
	  {
	  if (*it1==item->text(0)) break;
	  ++it2;
	  }
  QString s=*it2;
  bool ok;
  int l=s.toInt(&ok,10);
  if (ok) line=l;
  }
return line;*/
}

void Texmaker::ItemToRange(QTreeWidgetItem *item)
{
if (!item) return;
QTreeWidgetItem *parentitem, *theitem;
theitem=item;
int start=-1;
int end=-1;
int level=LevelItem(item);
int index;
if (level>=1)
  {
  start=LineItem(theitem);
  do 
      {
      parentitem=theitem->parent();
      if (parentitem) 
	{
	index=parentitem->indexOfChild(theitem);
	theitem=parentitem;
	} 
      else index=-1;
      } 
  while ((index>=0)&&(index>=parentitem->childCount()-1)&&(LevelItem(parentitem)>=1));
  if (index>=0 && index<parentitem->childCount()-1) end=LineItem(parentitem->child(index+1));
  else  
    {
    end=currentEditorView()->editor->searchLine("\\end{document}");
    if (end<0) end=currentEditorView()->editor->numoflines();
    }
  if (start<end-1) {currentEditorView()->editor->foldableLines.insert(start,end-1);}
  }
}

void Texmaker::ClickedOnStructure(QTreeWidgetItem *item,int col)
{
if ( !currentEditorView() ) return;
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QString name,flname,basename;
bool hasname=true;
if ((singlemode && !currentEditorView()) || finame.startsWith("untitled") || finame=="")
{
hasname=false;
}
else
{
QFileInfo fi(finame);
name=fi.absoluteFilePath();
flname=fi.fileName();
basename=name.left(name.length()-flname.length());
}
if (item)
  {
  int index = item->text(1).toInt();
  const QList<StructItem>& structure = currentEditorView()->editor->getStructItems();
  if (index<structure.count())
      {
      int type=structure.at(index).type;
      if (hasname && (type==2) || (type==3))
	  {
	    QString fname=structure.at(index).item;
	    loadIncludeFiles(fname,".tex");
	  }
	else  if (hasname && (type==9))
	  {
	    QString fname=structure.at(index).item;
	    loadIncludeFiles(fname,".bib");
	  }
	else
	  {
	    currentEditorView()->editor->gotoLine(structure.at(index).cursor.block().blockNumber());
	    if (!hasname) return;
	    QFileInfo fic(finame);
	    if (!fic.exists()) return;
	    QString basename=fic.completeBaseName();
	    if (embedinternalpdf && builtinpdfview)
	      {
	      if (pdfviewerWidget)
		{
		if (pdfviewerWidget->pdf_file!=fic.absolutePath()+"/"+basename+".pdf") pdfviewerWidget->openFile(fic.absolutePath()+"/"+basename+".pdf",viewpdf_command,ghostscript_command);
		StackedViewers->setCurrentWidget(pdfviewerWidget);
		pdfviewerWidget->show();
		if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1"))) pdfviewerWidget->jumpToPdfFromSource(getName(),structure.at(index).cursor.block().blockNumber()+1,1);
		}
	      }
	  }
      }
  }

}

//////////TAGS////////////////
void Texmaker::InsertTag(QString Entity, int dx, int dy)
{
if ( !currentEditorView() )	return;
OutputTextEdit->clear();
QTextCursor cur=currentEditorView()->editor->textCursor();
int pos=cur.position();
if (!Entity.startsWith("\\og")) Entity.replace("{}","{"+QString(0x2022)+"}");
Entity.replace("[]","["+QString(0x2022)+"]");
Entity.replace("\n\n","\n"+QString(0x2022)+"\n");
currentEditorView()->editor->insertWithMemoryIndent(Entity);
cur.setPosition(pos,QTextCursor::MoveAnchor);
if (Entity.contains(QString(0x2022))) 
    {
    currentEditorView()->editor->setTextCursor(cur);
    currentEditorView()->editor->search(QString(0x2022) ,true,false,true,true,false);
    OutputTextEdit->insertLine("Use the Tab key to reach the next "+QString(0x2022)+" field");
    }
else
    {
    if (dy>0) cur.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,dy);
    if (dx>0) cur.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,dx);
    currentEditorView()->editor->setTextCursor(cur);
    }
currentEditorView()->editor->setFocus();
OutputTableWidget->hide();
OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
separatorline->hide();
logpresent=false;
}

void Texmaker::InsertSymbol(QTableWidgetItem *item)
{
QString code_symbol;
QRegExp rxnumber(";([0-9]+)");
int number=-1;
int dx;
if (item)
	{
	if ( rxnumber.indexIn(item->text()) != -1) number=rxnumber.cap(1).toInt();
	if ((number>-1) && (number<412)) symbolScore[number]=symbolScore[number]+1;
	code_symbol=item->text().remove(rxnumber);
	dx=code_symbol.indexOf("{}",0);
	if (dx>-1) InsertTag(code_symbol,dx+1,0);
	else InsertTag(code_symbol,code_symbol.length(),0);
	SetMostUsedSymbols();
	}
}

void Texmaker::InsertXmlTag(QListWidgetItem *item)
{
if ( !currentEditorView() ) return;
QString txt, code, role;
QStringList tagList;
int dx,dy;
if (item  && !item->font().bold())
	{
	txt=item->text();
	txt.remove(QRegExp("\\[(.*)\\]"));
	txt.replace(QRegExp("([^\\\\])\\\\n"), "\\1\n"); 
	txt.replace(QRegExp("([^\\\\])\\\\n"), "\\1\n"); 
	role=item->data(Qt::UserRole).toString();
	tagList= role.split("#");
	code=tagList.at(0);
	code.remove(QRegExp("\\[(.*)\\]"));
	code.replace(QRegExp("([^\\\\])\\\\n"), "\\1\n"); 
	code.replace(QRegExp("([^\\\\])\\\\n"), "\\1\n"); 
	dx=tagList.at(1).toInt();
	dy=tagList.at(2).toInt();
	if ((dx==0) && (dy==0)) 
	{
	if (!currentEditorView()->editor->textCursor().hasSelection()) dx=code.length();
	else dx=code.length()-currentEditorView()->editor->textCursor().selectedText().length();
	}
	InsertTag(code,dx,dy);
	}
}

void Texmaker::InsertUserElement(QListWidgetItem *item)
{
if ( !currentEditorView() ) return;
QString txt, code, role;
QStringList tagList;
int dx;
if (item)
    {
    txt=item->text();
    role=item->data(Qt::UserRole).toString();
    tagList= role.split("#");
    code=tagList.at(0);
if (code.left(1)=="%")
	{
	QString t=code;
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(code);
	}
    }
}

void Texmaker::InsertFromAction()
{
bool ok;
QString actData;
QStringList tagList;
QAction *action = qobject_cast<QAction *>(sender());
if ( !currentEditorView() )	return;
if (action)
	{
	actData=action->data().toString();
	tagList= actData.split("/");
	InsertTag(tagList.at(0),tagList.at(1).toInt(&ok, 10),tagList.at(2).toInt(&ok, 10));
	OutputTextEdit->insertLine(tagList.at(3));
	}
}

void Texmaker::InsertWithSelectionFromAction()
{
bool ok;
QString actData;
QStringList tagList;
QAction *action = qobject_cast<QAction *>(sender());
if ( !currentEditorView() )	return;
if (action)
	{
	actData=action->data().toString();
	tagList= actData.split("/");
	if (!currentEditorView()->editor->textCursor().hasSelection())
		{
		OutputTextEdit->insertLine("You can select a text before using this environment.");
		InsertTag(tagList.at(0)+tagList.at(1),tagList.at(2).toInt(&ok, 10),tagList.at(3).toInt(&ok, 10));
		}
	else
		{
		currentEditorView()->editor->cut();
		InsertTag(tagList.at(0)+tagList.at(1),tagList.at(2).toInt(&ok, 10),tagList.at(3).toInt(&ok, 10));
		currentEditorView()->editor->paste();
//		InsertTag(tagList.at(1),0,0);
		}
	}
}

void Texmaker::InsertWithSelectionFromString(const QString& text)
{
bool ok;
QStringList tagList;
if ( !currentEditorView() )	return;
tagList= text.split("/");
if (!currentEditorView()->editor->textCursor().hasSelection())
	{
	InsertTag(tagList.at(0)+QString(0x2022)+tagList.at(1),tagList.at(2).toInt(&ok, 10),tagList.at(3).toInt(&ok, 10));
	}
else
	{
	currentEditorView()->editor->cut();
	InsertTag(tagList.at(0)+QString(0x2022)+tagList.at(1),tagList.at(2).toInt(&ok, 10),tagList.at(3).toInt(&ok, 10));
	currentEditorView()->editor->paste();
//	InsertTag(tagList.at(1),0,0);
	}
}

void Texmaker::InsertFromString(const QString& text)
{
bool ok;
QStringList tagList;
if ( !currentEditorView() ) return;
tagList= text.split("/");
InsertTag(tagList.at(0),tagList.at(1).toInt(&ok, 10),tagList.at(2).toInt(&ok, 10));
}

void Texmaker::InsertBib()
{
if ( !currentEditorView() )	return;
//currentEditorView()->editor->viewport()->setFocus();
QString tag;
QFileInfo fi(getName());
tag=QString("\\bibliography{");
tag +=fi.completeBaseName();
tag +=QString("}\n");
InsertTag(tag,0,1);
OutputTextEdit->clear();
OutputTableWidget->hide();
OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
separatorline->hide();
OutputTextEdit->insertLine("The argument to \\bibliography refers to the bib file (without extension)");
OutputTextEdit->insertLine("which should contain your database in BibTeX format.");
OutputTextEdit->insertLine("Texmaker inserts automatically the base name of the TeX file");
}

void Texmaker::InsertBibLatex()
{
if ( !currentEditorView() )	return;
//currentEditorView()->editor->viewport()->setFocus();
QString tag;
QFileInfo fi(getName());
tag=QString("\\addbibresource{");
tag +=fi.completeBaseName();
tag +=QString("}\n");
InsertTag(tag,0,1);
OutputTextEdit->clear();
OutputTableWidget->hide();
OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
separatorline->hide();
OutputTextEdit->insertLine("The argument to \\bibliography refers to the bib file (without extension)");
OutputTextEdit->insertLine("which should contain your database in BibTeX format.");
OutputTextEdit->insertLine("Texmaker inserts automatically the base name of the TeX file");
}

void Texmaker::InsertStruct()
{
QString actData, tag;
if ( !currentEditorView() )	return;
//currentEditorView()->editor->viewport()->setFocus();
QAction *action = qobject_cast<QAction *>(sender());
if (action)
	{
	actData=action->data().toString();
	StructDialog *stDlg = new StructDialog(this,actData);
	if ( stDlg->exec() )
		{
		if (stDlg->ui.checkBox->isChecked())
		{tag=actData+"{";}
		else
		{tag=actData+"*{";}
		tag +=stDlg->ui.TitlelineEdit->text();
		tag +=QString("}\n");
		InsertTag(tag,0,1);
		//UpdateStructure();
		}
	}
}

void Texmaker::InsertStructFromString(const QString& text)
{
QString tag;
if ( !currentEditorView() )	return;
//currentEditorView()->editor->viewport()->setFocus();
StructDialog *stDlg = new StructDialog(this,text);
if ( stDlg->exec() )
	{
	if (stDlg->ui.checkBox->isChecked())
	{tag=text+"{";}
	else
	{tag=text+"*{";}
	tag +=stDlg->ui.TitlelineEdit->text();
	tag +=QString("}\n");
	InsertTag(tag,0,1);
	//UpdateStructure();
	}
}

void Texmaker::InsertImage()
{
if ( !currentEditorView() )	return;
QString tag;
QString currentDir=QDir::homePath();
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
if (!finame.startsWith("untitled")) currentDir=fi.absolutePath();
QDir rootdir=fi.dir();
GraphicFileChooser *sfDlg = new GraphicFileChooser(this,tr("Select an image File"));
sfDlg->setFilter("Graphic files (*.eps *.pdf *.png *.jpeg *.jpg *.tiff);;All files (*.*)");
sfDlg->setDir(currentDir);
if (sfDlg->exec() )
	{
	QString fn=sfDlg->fileName();
	QFileInfo fi(rootdir.relativeFilePath(fn));
	if (!sfDlg->ui.moreButton->isChecked()) InsertTag("\\includegraphics[scale=1]{"+fi.filePath()+"} ",26,0);
	else
	  {
	  tag = "\\begin{figure}["+sfDlg->ui.lineEditPlacement->text()+"]\n";
	  if(sfDlg->ui.comboBoxCaption->currentIndex()==0) tag+="\\caption{"+sfDlg->ui.lineEditCaption->text()+"}\n";
	  if (sfDlg->ui.checkBoxCentering->isChecked()) tag+="\\centering\n";
	  tag+="\\includegraphics[scale=1]{"+fi.filePath()+"}\n";
	  if(sfDlg->ui.comboBoxCaption->currentIndex()==1) tag+="\\caption{"+sfDlg->ui.lineEditCaption->text()+"}\n";
	  tag+="\\end{figure}\n";
	  InsertTag(tag,0,4);
	  }
	}
}

void Texmaker::InsertInclude()
{
if ( !currentEditorView() )	return;
QString currentDir=QDir::homePath();
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
if (!finame.startsWith("untitled")) currentDir=fi.absolutePath();
QDir rootdir=fi.dir();
FileChooser *sfDlg = new FileChooser(this,tr("Select a File"));
sfDlg->setFilter("TeX files (*.tex);;All files (*.*)");
sfDlg->setDir(currentDir);
if (sfDlg->exec() )
	{
	QString fn=sfDlg->fileName();
	QFileInfo fi(rootdir.relativeFilePath(fn));
	QString suff=fi.suffix();
	QString name=fi.filePath();
	name=name.left(name.length()-suff.length()-1);
	InsertTag("\\include{"+name+"}",9,0);
	}
//UpdateStructure();
}

void Texmaker::InsertInput()
{
if ( !currentEditorView() )	return;
QString currentDir=QDir::homePath();
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
if (!finame.startsWith("untitled")) currentDir=fi.absolutePath();
QDir rootdir=fi.dir();
FileChooser *sfDlg = new FileChooser(this,tr("Select a File"));
sfDlg->setFilter("TeX files (*.tex);;All files (*.*)");
sfDlg->setDir(currentDir);
if (sfDlg->exec() )
	{
	QString fn=sfDlg->fileName();
	QFileInfo fi(rootdir.relativeFilePath(fn));
	QString suff=fi.suffix();
	QString name=fi.filePath();
	name=name.left(name.length()-suff.length()-1);
	InsertTag("\\input{"+name+"}",7,0);
	}
//UpdateStructure();
}

void Texmaker::QuickTabular()
{
if ( !currentEditorView() )	return;
QStringList borderlist, alignlist;
borderlist<< QString("|") << QString("||") << QString("") << QString("@{}");
alignlist << QString("c") << QString("l") << QString("r") << QString("p{}") << QString(">{\\centering\\arraybackslash}p{}") << QString(">{\\raggedleft\\arraybackslash}p{}");
QString al="";
QString vs="";
QString el="";
QString tag;
TabDialog *quickDlg = new TabDialog(this,"Tabular");
QTableWidgetItem *item=new QTableWidgetItem();
if ( quickDlg->exec() )
	{
	int y = quickDlg->ui.spinBoxRows->value();
	int x = quickDlg->ui.spinBoxColumns->value();
	tag = QString("\\begin{tabular}{");
	for ( int j=0;j<x;j++) 
	  {
	  tag+=borderlist.at(quickDlg->colDataList.at(j).leftborder);
	  tag+=alignlist.at(quickDlg->colDataList.at(j).alignment);
	  }
	tag+=borderlist.at(quickDlg->ui.comboBoxEndBorder->currentIndex());
	tag +=QString("}\n");
	for ( int i=0;i<y;i++) 
	  {
	  if (quickDlg->liDataList.at(i).topborder) tag+=QString("\\hline \n");
	  if (quickDlg->ui.checkBoxMargin->isChecked()) tag+="\\rule[-1ex]{0pt}{2.5ex} ";
	  if (quickDlg->liDataList.at(i).merge && (quickDlg->liDataList.at(i).mergeto>quickDlg->liDataList.at(i).mergefrom))
	    {
	    el="";
	    for ( int j=0;j<x;j++) 
		    {
		    item =quickDlg->ui.tableWidget->item(i,j);
		    
		    if (j==quickDlg->liDataList.at(i).mergefrom-1) 
		      {
		      if (item) el+=item->text();
		      tag+=QString("\\multicolumn{");
		      tag+=QString::number(quickDlg->liDataList.at(i).mergeto-quickDlg->liDataList.at(i).mergefrom+1);
		      tag+=QString("}{");
		      if ((j==0) && (quickDlg->colDataList.at(j).leftborder<2)) tag+=borderlist.at(quickDlg->colDataList.at(j).leftborder);
		      if (quickDlg->colDataList.at(j).alignment<3) tag+=alignlist.at(quickDlg->colDataList.at(j).alignment);
		      else tag+=QString("c");
		      if (quickDlg->liDataList.at(i).mergeto==x) tag+=borderlist.at(quickDlg->ui.comboBoxEndBorder->currentIndex());
		      else tag+=borderlist.at(quickDlg->colDataList.at(quickDlg->liDataList.at(i).mergeto).leftborder);
		      tag+=QString("}{");
		      }
		    else if (j==quickDlg->liDataList.at(i).mergeto-1)
		      {
		      if (item) el+=item->text();
		      if (el.isEmpty()) el=QString(0x2022);
		      tag+=el+QString("}");
		      if (j<x-1) tag+=" & ";
		      else tag+=QString(" \\\\ \n");
		      }
		    else if ((j>quickDlg->liDataList.at(i).mergefrom-1) && (j<quickDlg->liDataList.at(i).mergeto-1))
		      {
		      if (item) el+=item->text();
		      }
		    else
		      {
		      if (item) 
			{
			if (item->text().isEmpty()) tag +=QString(0x2022);
			else tag +=item->text();
			}
		      else tag +=QString(0x2022);
		      if (j<x-1) tag+=" & ";
		      else tag+=QString(" \\\\ \n");
		      }
		      
		    }
	    }
	  else
	    {
	    for ( int j=0;j<x-1;j++) 
		    {
		    item =quickDlg->ui.tableWidget->item(i,j);
		    if (item) 
		      {
		      if (item->text().isEmpty()) tag +=QString(0x2022)+QString(" & ");
		      else tag +=item->text()+ QString(" & ");
		      }
		    else tag +=QString(0x2022)+QString(" & ");
		    }
	    item =quickDlg->ui.tableWidget->item(i,x-1);
	    if (item) 
	      {
	      if (item->text().isEmpty()) tag +=QString(0x2022)+QString(" \\\\ \n");
	      else tag +=item->text()+ QString(" \\\\ \n");
	      }
	    else tag +=QString(0x2022)+QString(" \\\\ \n");   
	    }
	  }
	if (quickDlg->ui.checkBoxBorderBottom->isChecked()) tag +=QString("\\hline \n\\end{tabular} ");
	else tag +=QString("\\end{tabular} ");
	if (tag.contains("arraybackslash")) tag="% \\usepackage{array} is required\n"+tag;
	InsertTag(tag,0,0);
	}

}

void Texmaker::QuickArray()
{
if ( !currentEditorView() )	return;
QString al;
ArrayDialog *arrayDlg = new ArrayDialog(this,"Array");
QTableWidgetItem *item=new QTableWidgetItem();
if ( arrayDlg->exec() ) 
	{
	int y = arrayDlg->ui.spinBoxRows->value();
	int x = arrayDlg->ui.spinBoxColumns->value();
	QString env=arrayDlg->ui.comboEnvironment->currentText();
	QString tag = QString("\\begin{")+env+"}";
	if (env=="array")
		{
		tag+="{";
		if  ((arrayDlg->ui.comboAlignment->currentIndex())==0) al=QString("c");
		if  ((arrayDlg->ui.comboAlignment->currentIndex())==1) al=QString("l");
		if  ((arrayDlg->ui.comboAlignment->currentIndex())==2) al=QString("r");
		for ( int j=0;j<x;j++) {tag +=al;} tag+="}";
		}
	tag +=QString("\n");
	for ( int i=0;i<y-1;i++) 
		{
		for ( int j=0;j<x-1;j++) 
			{
			item =arrayDlg->ui.tableWidget->item(i,j);
			if (item) tag +=item->text()+ QString(" & ");
			else tag +=QString(0x2022)+QString(" & ");
			}
		item =arrayDlg->ui.tableWidget->item(i,x-1);
		if (item) tag +=item->text()+ QString(" \\\\ \n");
		else tag +=QString(0x2022)+QString(" \\\\ \n");
		}
	for ( int j=0;j<x-1;j++) 
		{
		item =arrayDlg->ui.tableWidget->item(y-1,j);
		if (item) tag +=item->text()+ QString(" & ");
		else tag +=QString(0x2022)+QString(" & ");
		}
	item =arrayDlg->ui.tableWidget->item(y-1,x-1);
	if (item) tag +=item->text()+ QString("\n\\end{")+env+"} ";
	else tag +=QString(0x2022)+QString("\n\\end{")+env+"} ";
	InsertTag(tag,0,0);
	}
}

void Texmaker::QuickTabbing()
{
if ( !currentEditorView() ) return;
TabbingDialog *tabDlg = new TabbingDialog(this,"Tabbing");
if ( tabDlg->exec() )
	{
	int	x = tabDlg->ui.spinBoxColumns->value();
	int	y = tabDlg->ui.spinBoxRows->value();
	QString s=tabDlg->ui.lineEdit->text();
	QString tag = QString("\\begin{tabbing}\n");
	for ( int j=1;j<x;j++) {tag +="\\hspace{"+s+"}\\=";}
	tag+="\\kill\n";
	for ( int i=0;i<y-1;i++)
		{
		for ( int j=1;j<x;j++) {tag +=" "+QString(0x2022)+" \\> ";}
		tag+=QString(0x2022)+" \\\\ \n";
		}
	for ( int j=1;j<x;j++) {tag +=" "+QString(0x2022)+" \\> ";}
	tag +=QString(0x2022)+QString("\n\\end{tabbing} ");
	InsertTag(tag,0,2);
	}
}

void Texmaker::QuickLetter()
{
if ( !currentEditorView() )	return;
QString tag=QString("\\documentclass[");
LetterDialog *ltDlg = new LetterDialog(this,"Letter");
int f=ltDlg->ui.comboBoxEncoding->findText(document_encoding,Qt::MatchExactly | Qt::MatchCaseSensitive);
ltDlg->ui.comboBoxEncoding->setCurrentIndex(f);
if ( ltDlg->exec() )
	{
	tag+=ltDlg->ui.comboBoxPt->currentText()+QString(",");
	tag+=ltDlg->ui.comboBoxPaper->currentText()+QString("]{letter}");
	tag+=QString("\n");
	if (ltDlg->ui.comboBoxEncoding->currentText()!="NONE") tag+=QString("\\usepackage[")+ltDlg->ui.comboBoxEncoding->currentText()+QString("]{inputenc}");
	if (ltDlg->ui.comboBoxEncoding->currentText().startsWith("utf8x")) tag+=QString(" \\usepackage{ucs}");
	tag+=QString("\n");
	if (ltDlg->ui.checkBox->isChecked()) tag+=QString("\\usepackage{amsmath}\n\\usepackage{amsfonts}\n\\usepackage{amssymb}\n");
	tag+="\\address{your name and address} \n";
	tag+="\\signature{your signature} \n";
	tag+="\\begin{document} \n";
	tag+="\\begin{letter}{name and address of the recipient} \n";
	tag+="\\opening{saying hello} \n \n";
	tag+="write your letter here \n \n";
	tag+="\\closing{saying goodbye} \n";
	tag+="%\\cc{Cclist} \n";
	tag+="%\\ps{adding a postscript} \n";
	tag+="%\\encl{list of enclosed material} \n";
	tag+="\\end{letter} \n";
	tag+="\\end{document}";
	if (ltDlg->ui.checkBox->isChecked()) {InsertTag(tag,9,5);}
	else {InsertTag(tag,9,2);}
	}
}

void Texmaker::QuickDocument()
{
QString opt="";
QString optbabel="";
int li=3;
int f;
QString fontenc="";
if ( !currentEditorView() ) fileNew();
QString tag=QString("\\documentclass[");
QuickDocumentDialog *startDlg = new QuickDocumentDialog(this,"Quick Start");
startDlg->otherClassList=userClassList;
startDlg->otherPaperList=userPaperList;
startDlg->otherEncodingList=userEncodingList;
startDlg->otherOptionsList=userOptionsList;
startDlg->otherBabelList=userBabelList;
startDlg->Init();
f=startDlg->ui.comboBoxClass->findText(document_class,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxClass->setCurrentIndex(f);
f=startDlg->ui.comboBoxSize->findText(typeface_size,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxSize->setCurrentIndex(f);
f=startDlg->ui.comboBoxPaper->findText(paper_size,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxPaper->setCurrentIndex(f);
f=startDlg->ui.comboBoxEncoding->findText(document_encoding,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxEncoding->setCurrentIndex(f);
QList<QListWidgetItem *> babItems=startDlg->ui.listWidgetBabel->findItems(babel_default,Qt::MatchExactly | Qt::MatchCaseSensitive);
if (babItems.size()>0) startDlg->ui.listWidgetBabel->setCurrentItem(babItems.at(0));
startDlg->ui.checkBoxAMS->setChecked(ams_packages);
startDlg->ui.checkBoxIDX->setChecked(makeidx_package);
startDlg->ui.checkBoxBabel->setChecked(babel_package);
startDlg->ui.listWidgetBabel->setEnabled(babel_package);
startDlg->ui.pushButtonBabel->setEnabled(babel_package);
startDlg->ui.checkBoxGeometry->setChecked(geometry_package);
startDlg->ui.lineEditGeometry->setEnabled(geometry_package);
startDlg->ui.checkBoxGraphicx->setChecked(graphicx_package);
startDlg->ui.checkBoxLmodern->setChecked(lmodern_package);
startDlg->ui.checkBoxKpfonts->setChecked(kpfonts_package);
startDlg->ui.checkBoxFourier->setChecked(fourier_package);
startDlg->ui.lineEditAuthor->setText(author);
startDlg->ui.lineEditGeometry->setText(geometry_options);
if ( startDlg->exec() )
	{
	tag+=startDlg->ui.comboBoxSize->currentText()+QString(",");
	tag+=startDlg->ui.comboBoxPaper->currentText();
	QList<QListWidgetItem *> selectedItems=startDlg->ui.listWidgetOptions->selectedItems();
	for (int i = 0; i < selectedItems.size(); ++i) 
		{
		if ( selectedItems.at(i)) opt+=QString(",")+selectedItems.at(i)->text();
		}
	tag+=opt+QString("]{");
	tag+=startDlg->ui.comboBoxClass->currentText()+QString("}");
	tag+=QString("\n");
	if (startDlg->ui.comboBoxEncoding->currentText()!="NONE") tag+=QString("\\usepackage[")+startDlg->ui.comboBoxEncoding->currentText()+QString("]{inputenc}");
	tag+=QString("\n");
	if (startDlg->ui.comboBoxEncoding->currentText().startsWith("utf8x"))
		{
		tag+=QString("\\usepackage{ucs}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxBabel->isChecked())
		{
		QList<QListWidgetItem *> babelItems=startDlg->ui.listWidgetBabel->selectedItems();
		for (int i = 0; i < babelItems.size(); ++i) 
			{
			if ( babelItems.at(i)) 
			  {
			  if ((babelItems.at(i)->text()=="arabic") && fontenc.isEmpty()) fontenc="LAE,LFE";
			  else if ((babelItems.at(i)->text()=="russian") && fontenc.isEmpty()) fontenc="OT1";
			  else if ((babelItems.at(i)->text()=="slovak") && fontenc.isEmpty()) fontenc="IL2";
			  else if ((babelItems.at(i)->text()=="francais") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="french") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="frenchb") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="german") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="portuguese") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="icelandic") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="czech") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="magyar") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="finnish") && fontenc.isEmpty()) fontenc="T1";
			  if (i==0) 
			      {
			      optbabel+=babelItems.at(i)->text();
			      babel_default=babelItems.at(i)->text();
			      }
			  else optbabel+=QString(",")+babelItems.at(i)->text();
			  }
			}
		tag+=QString("\\usepackage["+optbabel+"]{babel}\n");
		li=li+1;
		if (!fontenc.isEmpty())
		    {
		    tag+=QString("\\usepackage["+fontenc+"]{fontenc}\n");
		    li=li+1;
		    }
		}
	if (startDlg->ui.checkBoxAMS->isChecked())
		{
		tag+=QString("\\usepackage{amsmath}\n\\usepackage{amsfonts}\n\\usepackage{amssymb}\n");
		li=li+3;
		}
	if (startDlg->ui.checkBoxIDX->isChecked())
		{
		tag+=QString("\\usepackage{makeidx}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxGraphicx->isChecked())
		{
		tag+=QString("\\usepackage{graphicx}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxLmodern->isChecked())
		{
		tag+=QString("\\usepackage{lmodern}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxKpfonts->isChecked())
		{
		tag+=QString("\\usepackage{kpfonts}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxFourier->isChecked())
		{
		tag+=QString("\\usepackage{fourier}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxGeometry->isChecked())
		{
		tag+=QString("\\usepackage["+startDlg->ui.lineEditGeometry->text()+"]{geometry}\n");
		li=li+1;
		}
	if (startDlg->ui.lineEditAuthor->text()!="")
		{
		tag+="\\author{"+startDlg->ui.lineEditAuthor->text()+"}\n";
		li=li+1;
		}
	if (startDlg->ui.lineEditTitle->text()!="")
		{
		tag+="\\title{"+startDlg->ui.lineEditTitle->text()+"}\n";
		li=li+1;
		}
	tag+=QString("\\begin{document}\n\n\\end{document}");
	InsertTag(tag,0,li);
	document_class=startDlg->ui.comboBoxClass->currentText();
	typeface_size=startDlg->ui.comboBoxSize->currentText();
	paper_size=startDlg->ui.comboBoxPaper->currentText();
	document_encoding=startDlg->ui.comboBoxEncoding->currentText();
	ams_packages=startDlg->ui.checkBoxAMS->isChecked();
	makeidx_package=startDlg->ui.checkBoxIDX->isChecked();
	babel_package=startDlg->ui.checkBoxBabel->isChecked();
	geometry_package=startDlg->ui.checkBoxGeometry->isChecked();
	graphicx_package=startDlg->ui.checkBoxGraphicx->isChecked();
	lmodern_package=startDlg->ui.checkBoxLmodern->isChecked();
	kpfonts_package=startDlg->ui.checkBoxKpfonts->isChecked();
	fourier_package=startDlg->ui.checkBoxFourier->isChecked();
	author=startDlg->ui.lineEditAuthor->text();
	geometry_options=startDlg->ui.lineEditGeometry->text();
	userClassList=startDlg->otherClassList;
	userPaperList=startDlg->otherPaperList;
	userEncodingList=startDlg->otherEncodingList;
	userOptionsList=startDlg->otherOptionsList;
	userBabelList=startDlg->otherBabelList;
	}
}

void Texmaker::QuickXelatex()
{
QString opt="";
QString optbabel="";
int li=3;
int f;
if ( !currentEditorView() ) fileNew();
QString tag=QString("\\documentclass[");
QuickXelatexDialog *startDlg = new QuickXelatexDialog(this,"Quick Start");
startDlg->otherClassList=userClassList;
startDlg->otherPaperList=userPaperList;
startDlg->otherOptionsList=userOptionsList;
startDlg->otherBabelList=userBabelList;
startDlg->Init();
f=startDlg->ui.comboBoxClass->findText(document_class,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxClass->setCurrentIndex(f);
f=startDlg->ui.comboBoxSize->findText(typeface_size,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxSize->setCurrentIndex(f);
f=startDlg->ui.comboBoxPaper->findText(paper_size,Qt::MatchExactly | Qt::MatchCaseSensitive);
startDlg->ui.comboBoxPaper->setCurrentIndex(f);
QList<QListWidgetItem *> babItems=startDlg->ui.listWidgetBabel->findItems(babel_default,Qt::MatchExactly | Qt::MatchCaseSensitive);
if (babItems.size()>0) startDlg->ui.listWidgetBabel->setCurrentItem(babItems.at(0));
startDlg->ui.checkBoxAMS->setChecked(ams_packages);
startDlg->ui.checkBoxBabel->setChecked(babel_package);
startDlg->ui.listWidgetBabel->setEnabled(babel_package);
startDlg->ui.pushButtonBabel->setEnabled(babel_package);
startDlg->ui.checkBoxGeometry->setChecked(geometry_package);
startDlg->ui.lineEditGeometry->setEnabled(geometry_package);
startDlg->ui.checkBoxGraphicx->setChecked(graphicx_package);
startDlg->ui.lineEditAuthor->setText(author);
startDlg->ui.lineEditGeometry->setText(geometry_options);
if ( startDlg->exec() )
	{
	tag+=startDlg->ui.comboBoxSize->currentText()+QString(",");
	tag+=startDlg->ui.comboBoxPaper->currentText();
	QList<QListWidgetItem *> selectedItems=startDlg->ui.listWidgetOptions->selectedItems();
	for (int i = 0; i < selectedItems.size(); ++i) 
		{
		if ( selectedItems.at(i)) opt+=QString(",")+selectedItems.at(i)->text();
		}
	tag+=opt+QString("]{");
	tag+=startDlg->ui.comboBoxClass->currentText()+QString("}");
	tag+=QString("\n");
	
	tag+=QString("\\usepackage{fontspec}\n");
	tag+=QString("\\defaultfontfeatures{Mapping=tex-text}\n");
	tag+=QString("\\usepackage{xunicode}\n");
	tag+=QString("\\usepackage{xltxtra}\n");
	tag+=QString("%\\setmainfont{???}\n");
	if (startDlg->ui.checkBoxBabel->isChecked())
		{
		QList<QListWidgetItem *> babelItems=startDlg->ui.listWidgetBabel->selectedItems();
		for (int i = 0; i < babelItems.size(); ++i) 
			{
			if ( babelItems.at(i)) 
			  {
			  if (i==0) 
			      {
			      optbabel+=babelItems.at(i)->text();
			      babel_default=babelItems.at(i)->text();
			      }
			  }
			}
		tag+=QString("\\usepackage{polyglossia}\n\\setdefaultlanguage{"+optbabel+"}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxAMS->isChecked())
		{
		tag+=QString("\\usepackage{amsmath}\n\\usepackage{amsfonts}\n\\usepackage{amssymb}\n");
		li=li+3;
		}
	if (startDlg->ui.checkBoxGraphicx->isChecked())
		{
		tag+=QString("\\usepackage{graphicx}\n");
		li=li+1;
		}
	if (startDlg->ui.checkBoxGeometry->isChecked())
		{
		tag+=QString("\\usepackage["+startDlg->ui.lineEditGeometry->text()+"]{geometry}\n");
		li=li+1;
		}
	if (startDlg->ui.lineEditAuthor->text()!="")
		{
		tag+="\\author{"+startDlg->ui.lineEditAuthor->text()+"}\n";
		li=li+1;
		}
	if (startDlg->ui.lineEditTitle->text()!="")
		{
		tag+="\\title{"+startDlg->ui.lineEditTitle->text()+"}\n";
		li=li+1;
		}
	tag+=QString("\\begin{document}\n\n\\end{document}");
	InsertTag(tag,0,li);
	document_class=startDlg->ui.comboBoxClass->currentText();
	typeface_size=startDlg->ui.comboBoxSize->currentText();
	paper_size=startDlg->ui.comboBoxPaper->currentText();
	document_encoding="utf8";
	ams_packages=startDlg->ui.checkBoxAMS->isChecked();
	babel_package=startDlg->ui.checkBoxBabel->isChecked();
	geometry_package=startDlg->ui.checkBoxGeometry->isChecked();
	graphicx_package=startDlg->ui.checkBoxGraphicx->isChecked();
	author=startDlg->ui.lineEditAuthor->text();
	geometry_options=startDlg->ui.lineEditGeometry->text();
	userClassList=startDlg->otherClassList;
	userPaperList=startDlg->otherPaperList;
	userOptionsList=startDlg->otherOptionsList;
	userBabelList=startDlg->otherBabelList;
	}
}


void Texmaker::QuickBeamer()
{
QString opt="";
QString optbabel="";
int f;
QString fontenc="";
if ( !currentEditorView() ) fileNew();
QString tag=QString("\\documentclass[");
QuickBeamerDialog *beamDlg = new QuickBeamerDialog(this,"Quick Start");
beamDlg->Init();
f=beamDlg->ui.comboBoxTheme->findText(beamer_theme,Qt::MatchExactly | Qt::MatchCaseSensitive);
beamDlg->ui.comboBoxTheme->setCurrentIndex(f);
f=beamDlg->ui.comboBoxSize->findText(beamer_size,Qt::MatchExactly | Qt::MatchCaseSensitive);
beamDlg->ui.comboBoxSize->setCurrentIndex(f);
f=beamDlg->ui.comboBoxEncoding->findText(beamer_encoding,Qt::MatchExactly | Qt::MatchCaseSensitive);
beamDlg->ui.comboBoxEncoding->setCurrentIndex(f);
QList<QListWidgetItem *> babItems=beamDlg->ui.listWidgetBabel->findItems(beamer_babel,Qt::MatchExactly | Qt::MatchCaseSensitive);
if (babItems.size()>0) beamDlg->ui.listWidgetBabel->setCurrentItem(babItems.at(0));
beamDlg->ui.checkBoxAMS->setChecked(ams_packages);
beamDlg->ui.checkBoxGraphicx->setChecked(graphicx_package);
beamDlg->ui.checkBoxBabel->setChecked(babel_package);
beamDlg->ui.listWidgetBabel->setEnabled(babel_package);
beamDlg->ui.lineEditAuthor->setText(beamer_author);
if ( beamDlg->exec() )
	{
	tag+=beamDlg->ui.comboBoxSize->currentText();
	tag+=QString("]{beamer}\n");
	tag+="\\usetheme{"+beamDlg->ui.comboBoxTheme->currentText()+"}\n";
	if (beamDlg->ui.comboBoxEncoding->currentText()!="NONE") tag+=QString("\\usepackage[")+beamDlg->ui.comboBoxEncoding->currentText()+QString("]{inputenc}");
	tag+=QString("\n");
	if (beamDlg->ui.comboBoxEncoding->currentText().startsWith("utf8x"))
		{
		tag+=QString("\\usepackage{ucs}\n");
		}
	if (beamDlg->ui.checkBoxBabel->isChecked())
		{
		QList<QListWidgetItem *> babelItems=beamDlg->ui.listWidgetBabel->selectedItems();
		for (int i = 0; i < babelItems.size(); ++i) 
			{
			if ( babelItems.at(i)) 
			  {
			  if ((babelItems.at(i)->text()=="arabic") && fontenc.isEmpty()) fontenc="LAE,LFE";
			  else if ((babelItems.at(i)->text()=="russian") && fontenc.isEmpty()) fontenc="OT1";
			  else if ((babelItems.at(i)->text()=="slovak") && fontenc.isEmpty()) fontenc="IL2";
			  else if ((babelItems.at(i)->text()=="francais") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="french") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="frenchb") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="german") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="portuguese") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="icelandic") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="czech") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="magyar") && fontenc.isEmpty()) fontenc="T1";
			  else if ((babelItems.at(i)->text()=="finnish") && fontenc.isEmpty()) fontenc="T1";
			  if (i==0) 
			      {
			      optbabel+=babelItems.at(i)->text();
			      beamer_babel=babelItems.at(i)->text();
			      }
			  else optbabel+=QString(",")+babelItems.at(i)->text();
			  }
			}
		tag+=QString("\\usepackage["+optbabel+"]{babel}\n");
		if (!fontenc.isEmpty())
		    {
		    tag+=QString("\\usepackage["+fontenc+"]{fontenc}\n");
		    }
		}
	if (beamDlg->ui.checkBoxAMS->isChecked())
		{
		tag+=QString("\\usepackage{amsmath}\n\\usepackage{amsfonts}\n\\usepackage{amssymb}\n");
		}
	if (beamDlg->ui.checkBoxGraphicx->isChecked())
		{
		tag+=QString("\\usepackage{graphicx}\n");
		}
	if (beamDlg->ui.lineEditAuthor->text()!="")
		{
		tag+="\\author{"+beamDlg->ui.lineEditAuthor->text()+"}\n";
		}
	else
		{
		tag+="%\\author{}\n";
		}
	if (beamDlg->ui.lineEditTitle->text()!="")
		{
		tag+="\\title{"+beamDlg->ui.lineEditTitle->text()+"}\n";
		}
	else
		{
		tag+="%\\title{}\n";
		}
	tag+=QString("%\\setbeamercovered{transparent} \n");
	tag+=QString("%\\setbeamertemplate{navigation symbols}{} \n");
	tag+=QString("%\\logo{} \n");
	tag+=QString("%\\institute{} \n");
	tag+=QString("%\\date{} \n");
	tag+=QString("%\\subject{} \n");
	tag+=QString("\\begin{document}\n\n");
	if ((beamDlg->ui.lineEditAuthor->text()!="") || (beamDlg->ui.lineEditTitle->text()!="")) tag+=QString("\\begin{frame}\n\\titlepage\n\\end{frame}\n\n");
	else tag+=QString("%\\begin{frame}\n%\\titlepage\n%\\end{frame}\n\n");
	tag+=QString("%\\begin{frame}\n%\\tableofcontents\n%\\end{frame}\n\n");
	tag+=QString("\\begin{frame}{"+QString(0x2022)+"}\n\n\\end{frame}\n\n");
	tag+=QString("\\end{document}");
	if (currentEditorView())
	  {
	  OutputTextEdit->clear();
	  QTextCursor cur=currentEditorView()->editor->textCursor();
	  int pos=cur.position();;
	  currentEditorView()->editor->insertWithMemoryIndent(tag);
	  cur.setPosition(pos,QTextCursor::MoveAnchor);
	  currentEditorView()->editor->setTextCursor(cur);
	  currentEditorView()->editor->search(QString(0x2022) ,true,false,true,true,false);
	  OutputTextEdit->insertLine("Use the Tab key to reach the next "+QString(0x2022)+" field");
	  currentEditorView()->editor->setFocus();
	  OutputTableWidget->hide();
	  OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
	  separatorline->hide();
	  logpresent=false;
	  }
	beamer_theme=beamDlg->ui.comboBoxTheme->currentText();
	beamer_size=beamDlg->ui.comboBoxSize->currentText();
	beamer_encoding=beamDlg->ui.comboBoxEncoding->currentText();
	ams_packages=beamDlg->ui.checkBoxAMS->isChecked();
	babel_package=beamDlg->ui.checkBoxBabel->isChecked();
	graphicx_package=beamDlg->ui.checkBoxGraphicx->isChecked();
	beamer_author=beamDlg->ui.lineEditAuthor->text();
	}
}

void Texmaker::InsertBib1()
{
QString tag = QString("@Article{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="journal = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTvolume = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTpages = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,9,0);
OutputTextEdit->insertLine("Bib fields - Article in Journal");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBib2()
{
QString tag = QString("@InProceedings{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="booktitle = {},\n";
tag+="OPTcrossref = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTpages = {},\n";
tag+="OPTyear = {},\n";
tag+="OPTeditor = {},\n";
tag+="OPTvolume = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTseries = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTorganization = {},\n";
tag+="OPTpublisher = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,15,0);
OutputTextEdit->insertLine("Bib fields - Article in Conference Proceedings");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib3()
{
QString tag = QString("@InCollection{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="booktitle = {},\n";
tag+="OPTcrossref = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTpages = {},\n";
tag+="OPTpublisher = {},\n";
tag+="OPTyear = {},\n";
tag+="OPTeditor = {},\n";
tag+="OPTvolume = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTseries = {},\n";
tag+="OPTtype = {},\n";
tag+="OPTchapter = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTedition = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,14,0);
OutputTextEdit->insertLine("Bib fields - Article in a Collection");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib4()
{
QString tag = QString("@InBook{"+QString(0x2022)+",\n");
tag+="ALTauthor = {},\n";
tag+="ALTeditor = {},\n";
tag+="title = {},\n";
tag+="chapter = {},\n";
tag+="publisher = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTvolume = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTseries = {},\n";
tag+="OPTtype = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTedition = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTpages = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Chapter or Pages in a Book");
OutputTextEdit->insertLine( "ALT.... : you have the choice between these two fields");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib5()
{
QString tag = QString("@Proceedings{"+QString(0x2022)+",\n");
tag+="title = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTeditor = {},\n";
tag+="OPTvolume = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTseries = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTorganization = {},\n";
tag+="OPTpublisher = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,13,0);
OutputTextEdit->insertLine("Bib fields - Conference Proceedings");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib6()
{
QString tag = QString("@Book{"+QString(0x2022)+",\n");
tag+="ALTauthor = {},\n";
tag+="ALTeditor = {},\n";
tag+="title = {},\n";
tag+="publisher = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTvolume = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTseries = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTedition = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,6,0);
OutputTextEdit->insertLine("Bib fields - Book");
OutputTextEdit->insertLine( "ALT.... : you have the choice between these two fields");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib7()
{
QString tag = QString("@Booklet{"+QString(0x2022)+",\n");
tag+="title = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTauthor = {},\n";
tag+="OPThowpublished = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTyear = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,9,0);
OutputTextEdit->insertLine("Bib fields - Booklet");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib8()
{
QString tag = QString("@PhdThesis{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="school = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTtype = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,11,0);
OutputTextEdit->insertLine("Bib fields - PhD. Thesis");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib9()
{
QString tag = QString("@MastersThesis{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="school = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTtype = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,15,0);
OutputTextEdit->insertLine("Bib fields - Master's Thesis");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib10()
{
QString tag = QString("@TechReport{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="institution = {},\n";
tag+="year = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTtype = {},\n";
tag+="OPTnumber = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,12,0);
OutputTextEdit->insertLine("Bib fields - Technical Report");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib11()
{
QString tag = QString("@Manual{"+QString(0x2022)+",\n");
tag+="title = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTauthor = {},\n";
tag+="OPTorganization = {},\n";
tag+="OPTaddress = {},\n";
tag+="OPTedition = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTyear = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Technical Manual");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib12()
{
QString tag = QString("@Unpublished{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="note = {},\n";
tag+="OPTkey = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTyear = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,13,0);
OutputTextEdit->insertLine("Bib fields - Unpublished");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}
void Texmaker::InsertBib13()
{
QString tag = QString("@Misc{"+QString(0x2022)+",\n");
tag+="OPTkey = {},\n";
tag+="OPTauthor = {},\n";
tag+="OPTtitle = {},\n";
tag+="OPThowpublished = {},\n";
tag+="OPTmonth = {},\n";
tag+="OPTyear = {},\n";
tag+="OPTnote = {},\n";
tag+="OPTannote = {}\n";
tag+="}\n";
InsertTag(tag,6,0);
OutputTextEdit->insertLine("Bib fields - Miscellaneous");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex1()
{
QString tag = QString("@article{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="journaltitle = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "translator" << "annotator" << "commentator" << "subtitle" << "titleaddon" << "editor" << "editora" << "editorb" << "editorc" << "journalsubtitle" << "issuetitle" << "issuesubtitle" << "language" << "origlanguage" << "series" << "volume" << "number" << "eid" << "issue" << "month" << "pages" << "version" << "note" << "issn" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";

InsertTag(tag,9,0);
OutputTextEdit->insertLine("Bib fields - Article in Journal");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex2()
{
QString tag = QString("@book{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "editor" << "editora" << "editorb" << "editorc" << "translator" << "annotator" << "commentator" << "introduction" << "foreword" << "afterword" << "subtitle" << "titleaddon" << "maintitle" << "mainsubtitle" << "maintitleaddon" << "language" << "origlanguage" << "volume" << "part" << "edition" << "volumes" << "series" << "number" << "note" << "publisher" << "location" << "isbn" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,6,0);
OutputTextEdit->insertLine("Bib fields - Single-volume book");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex3()
{
QString tag = QString("@mvbook{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "editor" << "editora" << "editorb" << "editorc" << "translator" << "annotator" << "commentator" << "introduction" << "foreword" << "afterword" << "subtitle" << "titleaddon" << "language" << "origlanguage" << "edition" << "volumes" << "series" << "number" << "note" << "publisher" << "location" << "isbn" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Multi-volume book");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex4()
{
QString tag = QString("@inbook{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="booktitle = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "bookauthor" << "editor" << "editora" << "editorb" << "editorc" << "translator" << "annotator" << "commentator" << "introduction" << "foreword" << "afterword" << "subtitle" << "titleaddon" << "maintitle" << "mainsubtitle" << "maintitleaddon" << "booksubtitle" << "booktitleaddon" << "language" << "origlanguage" << "volume" << "part" << "edition" << "volumes" << "series" << "number" << "note" << "publisher" << "location" << "isbn" << "chapter" << "pages" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - A part of a book");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex5()
{
QString tag = QString("@booklet{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "language" << "howpublished" << "type" << "note" << "location" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,9,0);
OutputTextEdit->insertLine("Bib fields - A book-like work without a formal publisher");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex6()
{
QString tag = QString("@collection{"+QString(0x2022)+",\n");
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "editora" << "editorb" << "editorc" << "translator" << "annotator" << "commentator" << "introduction" << "foreword" << "afterword" << "subtitle" << "titleaddon" << "maintitle" << "mainsubtitle" << "maintitleaddon" << "language" << "origlanguage" << "volume" << "part" << "edition" << "volumes" << "series" << "number" << "note" << "publisher" << "location" << "isbn" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,11,0);
OutputTextEdit->insertLine("Bib fields - Single-volume collection");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex7()
{
QString tag = QString("@mvcollection{"+QString(0x2022)+",\n");
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "editora" << "editorb" << "editorc" << "translator" << "annotator" << "commentator" << "introduction" << "foreword" << "afterword" << "subtitle" << "titleaddon" << "language" << "origlanguage" << "edition" << "volumes" << "series" << "number" << "note" << "publisher" << "location" << "isbn" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,13,0);
OutputTextEdit->insertLine("Bib fields - Multi-volume collection");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex8()
{
QString tag = QString("@incollection{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="booktitle = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "editora" << "editorb" << "editorc" << "translator" << "annotator" << "commentator" << "introduction" << "foreword" << "afterword" << "subtitle" << "titleaddon" << "maintitle" << "mainsubtitle" << "maintitleaddon" << "booksubtitle" << "booktitleaddon" << "language" << "origlanguage" << "volume" << "part" << "edition" << "volumes" << "series" << "number" << "note" << "publisher" << "location" << "isbn" << "chapter" << "pages" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,13,0);
OutputTextEdit->insertLine("Bib fields - A part of a collection");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex9()
{
QString tag = QString("@manual{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "language" << "edition" << "type" << "series" << "number" << "version" << "note" << "organization" << "publisher" << "location" << "isbn" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Technical documentation");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex10()
{
QString tag = QString("@misc{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields  << "subtitle" << "titleaddon" << "language" << "howpublished" << "type" << "version" << "note" << "organization" << "location" << "date" << "month" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,6,0);
OutputTextEdit->insertLine("Bib fields - Miscellaneous");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex11()
{
QString tag = QString("@online{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
tag+="url = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "language" << "version" << "note" << "organization" << "date" << "month" << "addendum" << "pubstate" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Online resource");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex12()
{
QString tag = QString("@periodical{"+QString(0x2022)+",\n");
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "editora" << "editorb" << "editorc" << "subtitle" << "issuetitle" << "issuesubtitle" << "language" << "series" << "volume" << "number" << "issue" << "date" << "month" <<  "note" << "issn" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,11,0);
OutputTextEdit->insertLine("Bib fields - Issue of a periodical");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex13()
{
QString tag = QString("@proceedings{"+QString(0x2022)+",\n");
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "maintitle" << "mainsubtitle" << "maintitleaddon" << "eventtitle" << "eventdate" << "venue" << "language" << "volume" << "part" << "volumes" << "series" << "number" << "note" << "organization" << "publisher" << "location" << "month" << "isbn" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,12,0);
OutputTextEdit->insertLine("Bib fields - Single-volume conference proceedings");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex14()
{
QString tag = QString("@mvproceedings{"+QString(0x2022)+",\n");
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "eventtitle" << "eventdate" << "venue" << "language" << "volumes" << "series" << "number" << "note" << "organization" << "publisher" << "location" << "month" << "isbn" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,14,0);
OutputTextEdit->insertLine("Bib fields - Multi-volume conference proceedings");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex15()
{
QString tag = QString("@inproceedings{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="editor = {},\n";
tag+="title = {},\n";
tag+="booktitle = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "maintitle" << "mainsubtitle" << "maintitleaddon" << "booksubtitle" << "booktitleaddon" << "eventtitle" << "eventdate" << "venue" << "language" << "volume" << "part" << "volumes" << "series" << "number" << "note" << "organization" << "publisher" << "location" << "month" << "isbn" << "chapter" << "pages" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,14,0);
OutputTextEdit->insertLine("Bib fields - Article in conference proceedings");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex16()
{
QString tag = QString("@report{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="type = {},\n";
tag+="institution = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "language" << "number" << "version" << "note" << "location" << "month" << "isrn" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,7,0);
OutputTextEdit->insertLine("Bib fields - Technical report");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex17()
{
QString tag = QString("@thesis{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="type = {},\n";
tag+="institution = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "subtitle" << "titleaddon" << "language" << "note" << "location" << "month" << "isbn" << "chapter" << "pages" << "pagetotal" << "addendum" << "pubstate" << "doi" << "eprint" << "eprintclass" << "eprinttype" << "url" << "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Thesis");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::InsertBibLatex18()
{
QString tag = QString("@patent{"+QString(0x2022)+",\n");
tag+="author = {},\n";
tag+="title = {},\n";
tag+="number = {},\n";
tag+="year = {},\n";
QStringList optfields;
optfields << "holder" << "subtitle" <<  "titleaddon" << "type" << "version" << "location" << "note"<< "date" << " month" << "year" << "addendum" << "pubstate" <<"doi" << "eprint" << "eprintclass" << "eprinttype" << "url"<< "urldate";
for ( int i = 0; i <optfields.count(); i++ )
  {
  tag+="OPT"+optfields.at(i)+" = {},\n";
  }
tag+="}\n";
InsertTag(tag,8,0);
OutputTextEdit->insertLine("Bib fields - Paten");
OutputTextEdit->insertLine( "OPT.... : optional fields (use the 'Clean' command to remove them)");
}

void Texmaker::CleanBib()
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->removeOptAlt();
}

void Texmaker::InsertUserTag(QString Entity)
{
if ( !currentEditorView() )	return;
QString pre=currentEditorView()->editor->beginningLine();
QTextCursor cur=currentEditorView()->editor->textCursor();
bool selection=cur.hasSelection();
if (selection) currentEditorView()->editor->cut();
int pos=cur.position();

QRegExp rx("(@+)");
int index=0;
while ((index = rx.indexIn(Entity,index)) != -1) 
{
if (rx.cap(1)=="@") Entity.replace(rx.pos(1),1,QString(0x2022));
else if (rx.cap(1)=="@@") Entity.replace(rx.pos(1),2,"@");
index += rx.matchedLength();
}

//Entity.replace("@",QString(0x2022));
//if (Entity.contains("\n") && !pre.isEmpty()) Entity.replace("\n","\n"+pre);
currentEditorView()->editor->insertWithMemoryIndent(Entity);
cur.setPosition(pos,QTextCursor::MoveAnchor);
int dx=Entity.length();
if (Entity.contains(QString(0x2022))) 
    {
    currentEditorView()->editor->setTextCursor(cur);
    currentEditorView()->editor->search(QString(0x2022) ,true,false,true,true,false);
    if (selection) currentEditorView()->editor->paste();
    }
else
    {
    if (dx>0) cur.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,dx);
    currentEditorView()->editor->setTextCursor(cur);
    }
currentEditorView()->editor->setFocus();
OutputTextEdit->clear();
OutputTableWidget->hide();
OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
separatorline->hide();
logpresent=false;
}

void Texmaker::InsertUserTag1()
{
if (UserMenuTag[0].left(1)=="%")
	{
	QString t=UserMenuTag[0];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[0]);
	}
}

void Texmaker::InsertUserTag2()
{
if (UserMenuTag[1].left(1)=="%")
	{
	QString t=UserMenuTag[1];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[1]);
	}
}

void Texmaker::InsertUserTag3()
{
if (UserMenuTag[2].left(1)=="%")
	{
	QString t=UserMenuTag[2];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[2]);
	}
}

void Texmaker::InsertUserTag4()
{
if (UserMenuTag[3].left(1)=="%")
	{
	QString t=UserMenuTag[3];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[3]);
	}
}

void Texmaker::InsertUserTag5()
{
if (UserMenuTag[4].left(1)=="%")
	{
	QString t=UserMenuTag[4];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[4]);
	}
}

void Texmaker::InsertUserTag6()
{
if (UserMenuTag[5].left(1)=="%")
	{
	QString t=UserMenuTag[5];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[5]);
	}
}

void Texmaker::InsertUserTag7()
{
if (UserMenuTag[6].left(1)=="%")
	{
	QString t=UserMenuTag[6];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[6]);
	}
}

void Texmaker::InsertUserTag8()
{
if (UserMenuTag[7].left(1)=="%")
	{
	QString t=UserMenuTag[7];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[7]);
	}
}

void Texmaker::InsertUserTag9()
{
if (UserMenuTag[8].left(1)=="%")
	{
	QString t=UserMenuTag[8];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[8]);
	}
}

void Texmaker::InsertUserTag10()
{
if (UserMenuTag[9].left(1)=="%")
	{
	QString t=UserMenuTag[9];
	t=t.remove(0,1);
	QString s="\\begin{"+t+"}\n"+QString(0x2022)+"\n\\end{"+t+"}\n";
	InsertUserTag(s);
	}
else
	{
	InsertUserTag(UserMenuTag[9]);
	}
}

void Texmaker::EditUserMenu()
{
QAction *Act;
UserMenuDialog *umDlg = new UserMenuDialog(this,tr("Edit User &Tags"));
for ( int i = 0; i <= 9; i++ )
    {
    umDlg->Name[i]=UserMenuName[i];
    umDlg->Tag[i]=UserMenuTag[i];
    }
umDlg->init();
if ( umDlg->exec() )
	{
	for ( int i = 0; i <= 9; i++ )
		{
		UserMenuName[i]=umDlg->Name[i];
		UserMenuTag[i]=umDlg->Tag[i];
		}
	user11Menu->clear();
	Act = new QAction("1: "+UserMenuName[0], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F1);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag1()));
	user11Menu->addAction(Act);
	Act = new QAction("2: "+UserMenuName[1], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F2);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag2()));
	user11Menu->addAction(Act);
	Act = new QAction("3: "+UserMenuName[2], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F3);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag3()));
	user11Menu->addAction(Act);
	Act = new QAction("4: "+UserMenuName[3], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F4);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag4()));
	user11Menu->addAction(Act);
	Act = new QAction("5: "+UserMenuName[4], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F5);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag5()));
	user11Menu->addAction(Act);
	Act = new QAction("6: "+UserMenuName[5], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F6);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag6()));
	user11Menu->addAction(Act);
	Act = new QAction("7: "+UserMenuName[6], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F7);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag7()));
	user11Menu->addAction(Act);
	Act = new QAction("8: "+UserMenuName[7], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F8);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag8()));
	user11Menu->addAction(Act);
	Act = new QAction("9: "+UserMenuName[8], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F9);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag9()));
	user11Menu->addAction(Act);
	Act = new QAction("10: "+UserMenuName[9], this);
	Act->setShortcut(Qt::SHIFT+Qt::Key_F10);
	connect(Act, SIGNAL(triggered()), this, SLOT(InsertUserTag10()));
	user11Menu->addAction(Act);
	user11Menu->addSeparator();
	Act = new QAction(tr("Edit User &Tags"), this);
	connect(Act, SIGNAL(triggered()), this, SLOT(EditUserMenu()));
	user11Menu->addAction(Act);
	}
}

void Texmaker::SectionCommand()
{
if ( !currentEditorView() ) return;
QAction *action = qobject_cast<QAction *>(sender());
QString text=action->text();
text.remove("&");
InsertStructFromString("\\"+text);
}

void Texmaker::OtherCommand()
{
if ( !currentEditorView() ) return;
QAction *action = qobject_cast<QAction *>(sender());
QString text=action->text();
text.remove("&");
if (text=="label") 
	{
	InsertFromString("\\label{} /7/0");
	return;
	}
if (text=="ref") 
	{
	InsertRef();
	return;
	}
if (text=="pageref") 
	{
	InsertPageRef();
	return;
	}
if (text=="index") 
	{
	InsertFromString("\\index{}/7/0");
	return;
	}
if (text=="cite") 
	{
	InsertCite();
	//InsertFromString("\\cite{}/6/0");
	return;
	}
if (text=="footnote") 
	{
	InsertFromString("\\footnote{}/10/0");
	return;
	}
}

void Texmaker::InsertCite()
{
//UpdateStructure();
QString tag="";
RefDialog *refDlg = new RefDialog(this,"Bibliography Items");
refDlg->ui.comboBox->addItems(bibitem);
refDlg->ui.label->setText("Items");
if (!bibitem.isEmpty() && refDlg->exec() )
	{
	tag="\\cite{"+refDlg->ui.comboBox->currentText()+"}";
	InsertTag(tag,tag.length(),0);
	}
else InsertTag("\\cite{}",6,0);
OutputTextEdit->insertLine( "\\cite{bibiliography item}");
}

void Texmaker::InsertRef()
{
//UpdateStructure();
QString tag="";
RefDialog *refDlg = new RefDialog(this,"Labels");
refDlg->ui.comboBox->addItems(labelitem);
if (!labelitem.isEmpty() && refDlg->exec() )
	{
	tag="\\ref{"+refDlg->ui.comboBox->currentText()+"}";
	InsertTag(tag,tag.length(),0);
	}
else InsertTag("\\ref{}",5,0);
OutputTextEdit->insertLine( "\\ref{key}");
}

void Texmaker::InsertPageRef()
{
//UpdateStructure();
QString tag="";
RefDialog *refDlg = new RefDialog(this,"Labels");
refDlg->ui.comboBox->addItems(labelitem);
if (!labelitem.isEmpty() && refDlg->exec() )
	{
	tag="\\pageref{"+refDlg->ui.comboBox->currentText()+"}";
	InsertTag(tag,tag.length(),0);
	}
else InsertTag("\\pageref{}",9,0);
OutputTextEdit->insertLine( "\\pageref{key}");
}

void Texmaker::SizeCommand()
{

if ( !currentEditorView() ) return;
QAction *action = qobject_cast<QAction *>(sender());
QString text=action->text();
text.remove("&");
if (text=="tiny")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\tiny /}/7/0");
	    }
	else InsertWithSelectionFromString("\\begin{tiny}\n/\n\\end{tiny}/12/0");
	return;
	}
if (text=="scriptsize")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\scriptsize /}/13/0");
	    }
	else InsertWithSelectionFromString("\\begin{scriptsize}\n/\n\\end{scriptsize}/18/0");
	return;
	}
if (text=="footnotesize")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\footnotesize /}/15/0");
	    }
	else InsertWithSelectionFromString("\\begin{footnotesize}\n/\n\\end{footnotesize}/20/0");
	return;
	}
if (text=="small")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\small /}/8/0");
	    }
	else InsertWithSelectionFromString("\\begin{small}\n/\n\\end{small}/13/0");
	return;
	}
if (text=="normalsize")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\normalsize /}/13/0");
	    }
	else InsertWithSelectionFromString("\\begin{normalsize}\n/\n\\end{normalsize}/18/0");
	return;
	}
if (text=="large")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\large /}/8/0");
	    }
	else InsertWithSelectionFromString("\\begin{large}\n/\n\\end{large}/13/0");
	return;
	}
if (text=="Large")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\Large /}/8/0");
	    }
	else InsertWithSelectionFromString("\\begin{Large}\n/\n\\end{Large}/13/0");
	return;
	}
if (text=="LARGE")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\LARGE /}/8/0");
	    }
	else InsertWithSelectionFromString("\\begin{LARGE}\n/\n\\end{LARGE}/13/0");
	return;
	}
if (text=="huge")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\huge /}/7/0");
	    }
	else InsertWithSelectionFromString("\\begin{huge}\n/\n\\end{huge}/12/0");
	return;
	}
if (text=="Huge")
	{
	if (currentEditorView()->editor->textCursor().hasSelection() && !currentEditorView()->editor->textCursor().selectedText().contains(QString(0x2029)))
	    {
	    InsertWithSelectionFromString("{\\Huge /}/7/0");
	    }
	else InsertWithSelectionFromString("\\begin{Huge}\n/\n\\end{Huge}/12/0");
	return;
	}
}
void Texmaker::ShowSectionMenu()
{
QAction *action = qobject_cast<QAction *>(sender());
sectionMenu->exec(centralToolBar->widgetForAction(action)->mapToGlobal(QPoint(centralToolBar->width(),0)));  
}
void Texmaker::ShowRefMenu()
{
QAction *action = qobject_cast<QAction *>(sender());
refMenu->exec(centralToolBar->widgetForAction(action)->mapToGlobal(QPoint(centralToolBar->width(),0)));    
}
void Texmaker::ShowSizeMenu()
{
QAction *action = qobject_cast<QAction *>(sender());
sizeMenu->exec(centralToolBar->widgetForAction(action)->mapToGlobal(QPoint(centralToolBar->width(),0)));      
}

///////////////TOOLS////////////////////
void Texmaker::RunCommand(QString comd,bool waitendprocess)
{
QString finame;
QString commandline=comd;
QByteArray result;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) || finame=="")
	{
	QMessageBox::warning( this,tr("Error"),tr("Can't detect the file name"));
	return;
	}
if (finame.startsWith("untitled"))
	{
	QMessageBox::warning( this,tr("Error"),tr("A document must be saved with an extension (and without spaces or accents in the name) before being used by a command."));
	return;
	}
if (!currentfileSaved()) 
  {
  ERRPROCESS=true;
  return;
  }
// QFileInfo texfi(finame);
// QString suf="."+texfi.suffix();
// QString realname;
if ((comd.startsWith("latex") || comd.startsWith("pdflatex")) &&  useoutputdir)
  {
  commandline.replace("latex","latex --output-directory=build");
  createBuildSubdirectory(finame);
//  realname=finame;
  }
// else
//   {
//   realname=outputName(finame,suf);
//   }
// 
// qDebug() << realname;
QFileInfo fi(finame);
QString basename=fi.completeBaseName();



//commandline.replace("%","\""+basename+"\"");
//commandline.replace("!",fi.absolutePath());
QFileInfo ficur(getName());
//if (!commandline.contains("okular")) commandline.replace("#","\""+ficur.completeBaseName()+"\"");
int currentline=1;
int currentcol=1;
if (currentEditorView() )
  {
  currentline=currentEditorView()->editor->linefromblock(currentEditorView()->editor->textCursor().block());
  currentcol=currentEditorView()->editor->textCursor().position() - currentEditorView()->editor->textCursor().block().position()+1;
  }
//commandline.replace("@",QString::number(currentline));


QRegExp rx1("(#+)");
QString capt="";
if (rx1.indexIn(commandline) != -1) capt=rx1.cap(1);
if (capt=="#") commandline.replace("#","\""+ficur.completeBaseName()+"\"");
else if (capt=="##") commandline.replace("##","#");

QRegExp rx2("(%+)");
capt="";
if (rx2.indexIn(commandline) != -1) capt=rx2.cap(1);
if (capt=="%") commandline.replace("%","\""+basename+"\"");
else if (capt=="%%") commandline.replace("%%","%");

QRegExp rx3("(!+)");
capt="";
if (rx3.indexIn(commandline) != -1) capt=rx3.cap(1);
if (capt=="!") commandline.replace("!",fi.absolutePath());
else if (capt=="!!") commandline.replace("!!","!");

QRegExp rx4("(@+)");
capt="";
if (rx4.indexIn(commandline) != -1) capt=rx4.cap(1);
if (capt=="@") commandline.replace("@",QString::number(currentline));
else if (capt=="@@") commandline.replace("@@","@");

if (builtinpdfview && (comd==viewpdf_command))
  {
    if (embedinternalpdf)
      {
      if (pdfviewerWidget)
	{
	pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
	StackedViewers->setCurrentWidget(pdfviewerWidget);
	//pdfviewerWidget->raise();
      	showpdfview=true;
	ShowPdfView(false);
	pdfviewerWidget->show();
	if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1")) ) pdfviewerWidget->jumpToPdfFromSource(getName(),currentline,currentcol);
	}
      else
	{
    //    pdfviewerWidget=new PdfViewer(outputName(finame,".pdf"),viewpdf_command, this);
	pdfviewerWidget=new PdfViewerWidget(outputName(finame,".pdf"),viewpdf_command,ghostscript_command,lp_options,QKeySequence(keyToggleFocus),pdfCheckerLang,lastScale,StackedViewers);
	pdfviewerWidget->centralToolBarBis->setMinimumHeight(centralToolBarBis->height());
	pdfviewerWidget->centralToolBarBis->setMaximumHeight(centralToolBarBis->height());
	connect(pdfviewerWidget, SIGNAL(openDocAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool)));
	connect(pdfviewerWidget, SIGNAL(sendFocusToEditor()), this, SLOT(getFocusToEditor()));
	connect(pdfviewerWidget, SIGNAL(sendPaperSize(const QString&)), this, SLOT(setPrintPaperSize(const QString&)));
	StackedViewers->addWidget(pdfviewerWidget);
	StackedViewers->setCurrentWidget(pdfviewerWidget);
	//pdfviewerWidget->raise();
	pdfviewerWidget->show();
	pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
	if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1")) ) pdfviewerWidget->jumpToPdfFromSource(getName(),currentline,currentcol);
	}
      return;
      }
   else
    {
    if (pdfviewerWindow)
      {
      pdfviewerWindow->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
      pdfviewerWindow->raise();
      pdfviewerWindow->show();
      if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1"))) pdfviewerWindow->jumpToPdfFromSource(getName(),currentline,currentcol);
      }
    else
      {
  //    pdfviewerWindow=new PdfViewer(outputName(finame,".pdf"),viewpdf_command, this);
      pdfviewerWindow=new PdfViewer(outputName(finame,".pdf"),viewpdf_command,ghostscript_command,lp_options,QKeySequence(keyToggleFocus),pdfCheckerLang,lastScale,0);
      connect(pdfviewerWindow, SIGNAL(openDocAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool)));
      connect(pdfviewerWindow, SIGNAL(sendFocusToEditor()), this, SLOT(getFocusToEditor()));
      connect(pdfviewerWindow, SIGNAL(sendPaperSize(const QString&)), this, SLOT(setPrintPaperSize(const QString&)));
      pdfviewerWindow->raise();
      pdfviewerWindow->show();
      if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1"))) pdfviewerWindow->jumpToPdfFromSource(getName(),currentline,currentcol);
      }
    return;     
    }
  }
else if (comd==asymptote_command)
  {
  QDir currentdir(fi.absolutePath());
  foreach (QFileInfo asyFileInfo, currentdir.entryInfoList(QStringList(basename+"*.asy"),QDir::Files | QDir::Readable, QDir::Name | QDir::IgnoreCase)) 
    {
    QString asyName = asyFileInfo.absolutePath()+"/"+asyFileInfo.completeBaseName();
    AsyFile(asyName);
    }
  return;
  }
  

proc = new QProcess( this );
proc->setWorkingDirectory(fi.absolutePath());
proc->setProperty("command",commandline);

//****
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
//****
connect(proc, SIGNAL(readyReadStandardError()),this, SLOT(readFromStderr()));
if ((comd==asymptote_command) || (comd==bibtex_command) ) connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(readFromStdoutput()));
if (checkViewerInstance && ((comd==viewdvi_command) || (comd==viewps_command) || (comd==viewpdf_command)))
  {
  connect(proc, SIGNAL(finished(int)),this, SLOT(SlotEndViewerProcess(int)));
  if (singleviewerinstance)
    {
    if (listViewerCommands.contains(commandline)) return;
    else listViewerCommands.append(commandline);
    }
  else
    {
    listViewerCommands.clear();
    }    
  }
else 
{
  if (((comd!=viewdvi_command) && (comd!=viewps_command) && (comd!=viewpdf_command))) disableToolsActions();
  connect(proc, SIGNAL(finished(int)),this, SLOT(SlotEndProcess(int)));
}
OutputTextEdit->clear();
OutputTableWidget->hide();
OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
separatorline->hide();

//OutputTextEdit->insertLine(commandline+"\n");
proc->start(commandline);
if (!proc->waitForStarted(1000)) 
	{
	ERRPROCESS=true;
	OutputTextEdit->insertLine("Error : could not start the command : "+commandline+"\n");
	checkViewerInstance=false;
	return;
	}
else OutputTextEdit->insertLine("Process started\n");
stat2->setText(commandline.section(' ',0,0));
FINPROCESS=false;
STOPPROCESS=false;
if (waitendprocess)
	{
	StopAct->setEnabled(true);
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	while (!FINPROCESS) 
		{
		qApp->instance()->processEvents(QEventLoop::AllEvents);
		if (STOPPROCESS && proc && proc->state()==QProcess::Running) 
		  {
		  proc->kill();
		  FINPROCESS=true;
		  ERRPROCESS=true;
		  stat2->setText("Ready");
		  }
		}
	QApplication::restoreOverrideCursor();
	enableToolsActions();
	StopAct->setEnabled(false);
	}
}

void Texmaker::readFromStderr()
{
QByteArray result=proc->readAllStandardError();
QString t=QString(result);
t=t.simplified();
if (!t.isEmpty()) OutputTextEdit->insertLine(t+"\n");
}

void Texmaker::stopProcess()
{
STOPPROCESS=true;  
}
void Texmaker::readFromStdoutput()
{
QByteArray result=proc->readAllStandardOutput ();
QString t=QString(result);
t=t.simplified();
if (!t.isEmpty()) OutputTextEdit->insertLine(t+"\n");
}

void Texmaker::SlotEndProcess(int err)
{
FINPROCESS=true;
enableToolsActions();
QString result=((err) ? "Process exited with error(s)" : "Process exited normally");
if (err) {ERRPROCESS=true;checkViewerInstance=false;}
OutputTextEdit->insertLine(result);
stat2->setText("Ready");
//stat2->setText(QString(" %1 ").arg(tr("Ready")));
}

void Texmaker::SlotEndViewerProcess(int err)
{
QString commandline = static_cast<QProcess*>(sender())->property("command").toString();
if ((singleviewerinstance) && (listViewerCommands.contains(commandline))) listViewerCommands.removeAll(commandline);
FINPROCESS=true;
enableToolsActions();
QString result=((err) ? "Process exited with error(s)" : "Process exited normally");
if (err) {ERRPROCESS=true;checkViewerInstance=false;}
OutputTextEdit->insertLine(result);
//stat2->setText(QString(" %1 ").arg(tr("Ready")));
}

void Texmaker::QuickBuild()
{
//stat2->setText(QString(" %1 ").arg(tr("Quick Build")));
ERRPROCESS=false;
checkViewerInstance=true;
QString finame=getName();
QFileInfo fi(finame);
if (!finame.startsWith("untitled") && finame!="" && fi.suffix()=="asy")
   {
    QStringList asyCommandList=quick_asy_command.split("|");
    for (int i = 0; i < asyCommandList.size(); ++i) 
	{
	if ((!ERRPROCESS)&&(!asyCommandList.at(i).isEmpty())) 
	  {
	  RunCommand(asyCommandList.at(i),true);
	  if ((asyCommandList.at(i)==latex_command) || (asyCommandList.at(i)==pdflatex_command) || (asyCommandList.at(i)==xelatex_command) || (asyCommandList.at(i)==lualatex_command))
	      {
	      LoadLog();
	      if (showoutputview) ViewLog();
	      if (!NoLatexErrors()) NextError();
	      }
	  }
        else {checkViewerInstance=false;return;}
	}
    }
else
  {
  switch (quickmode)
  {
    case 1:
      {
      //stat2->setText(QString(" %1 ").arg("Latex"));
      RunCommand(latex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Dvips"));
	  if (!ERRPROCESS) RunCommand(dvips_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) ViewPS();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
    case 2:
      {
      //stat2->setText(QString(" %1 ").arg("Latex"));
      RunCommand(latex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  if (!ERRPROCESS) ViewDvi();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
  case 3:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(pdflatex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  if (!ERRPROCESS) ViewPDF();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
  case 4:
      {
      //stat2->setText(QString(" %1 ").arg("Latex"));
      RunCommand(latex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Dvi to Pdf"));
	  if (!ERRPROCESS) RunCommand(dvipdf_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) ViewPDF();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
  case 5:
      {
      //stat2->setText(QString(" %1 ").arg("Latex"));
      RunCommand(latex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Dvips"));
	  if (!ERRPROCESS) RunCommand(dvips_command,true);
	  else {checkViewerInstance=false;return;}
	  //stat2->setText(QString(" %1 ").arg("Ps to Pdf"));
	  if (!ERRPROCESS) RunCommand(ps2pdf_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) ViewPDF();
	  }
      else {NextError();}
      }break;
  case 6:
      {
      QStringList commandList=userquick_command.split("|");
      for (int i = 0; i < commandList.size(); ++i) 
	  {
	  if ((!ERRPROCESS)&&(!commandList.at(i).isEmpty())) 
	    {
	    RunCommand(commandList.at(i),true);
	    if ((commandList.at(i)==latex_command) || (commandList.at(i)==pdflatex_command) || (commandList.at(i)==xelatex_command) || (commandList.at(i)==lualatex_command))
		{
		LoadLog();
		if (showoutputview) ViewLog();
		if (!NoLatexErrors()) NextError();
		}
	    }
	  else {checkViewerInstance=false;return;}
	  }
      }break;
    case 7:
      {
      //stat2->setText(QString(" %1 ").arg("Latex"));
      RunCommand(latex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Asymptote"));
	  if (!ERRPROCESS) RunCommand(asymptote_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) 
	      {
	      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
	      RunCommand(latex_command,true);
	      if (ERRPROCESS && !LogExists()) 
		  {
		  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
		  checkViewerInstance=false;
		  return;
		  }
	      LoadLog();
	      if (showoutputview) ViewLog();
	      if (NoLatexErrors()) 
		  {
		  RunCommand(latex_command,true);
		  if (ERRPROCESS && !LogExists()) 
		      {
		      QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
		      checkViewerInstance=false;
		      return;
		      }
		  LoadLog();
		  if (showoutputview) ViewLog();
		  if (NoLatexErrors()) 
		      {
		      //stat2->setText(QString(" %1 ").arg("Dvips"));
		      if (!ERRPROCESS) RunCommand(dvips_command,true);
		      else {checkViewerInstance=false;return;}
		      if (!ERRPROCESS) ViewPS();
		      else {checkViewerInstance=false;return;}
		      }
		      else {NextError();}
		  }
	      else {NextError();}
	      }
	  else return;
// 	  if (!ERRPROCESS) 
// 	      {
// 	      //stat2->setText(QString(" %1 ").arg("Latex"));
// 	      RunCommand(latex_command,true);
// 	      if (ERRPROCESS && !LogExists()) 
// 		  {
// 		  QMessageBox::warning( this,tr("Error"),tr("Could not start the command."));
// 		  checkViewerInstance=false;
// 		  return;
// 		  }
// 	      LoadLog();
// 	      if (showoutputview) ViewLog();
// 	      if (NoLatexErrors()) 
// 		  {
// 		  //stat2->setText(QString(" %1 ").arg("Dvips"));
// 		  if (!ERRPROCESS) RunCommand(dvips_command,true);
// 		  else {checkViewerInstance=false;return;}
// 		  if (!ERRPROCESS) ViewPS();
// 		  else {checkViewerInstance=false;return;}
// 		  }
// 	      else {NextError();}
// 	      }
// 	  else return;
	  }
      else {NextError();}
      }break;
    case 8:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(pdflatex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Asymptote"));
	  if (!ERRPROCESS) RunCommand(asymptote_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) 
	      {
	      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
	      RunCommand(pdflatex_command,true);
	      if (ERRPROCESS && !LogExists()) 
		  {
		  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
		  checkViewerInstance=false;
		  return;
		  }
	      LoadLog();
	      if (showoutputview) ViewLog();
	      if (NoLatexErrors()) 
		  {
		  RunCommand(pdflatex_command,true);
		  if (ERRPROCESS && !LogExists()) 
		      {
		      QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
		      checkViewerInstance=false;
		      return;
		      }
		  LoadLog();
		  if (showoutputview) ViewLog();
		  if (NoLatexErrors()) 
		      {
		      if (!ERRPROCESS) ViewPDF();
		      else {checkViewerInstance=false;return;}
		      }
		      else {NextError();}
		  }
	      else {NextError();}
	      }
	  else return;
// 	  if (!ERRPROCESS) 
// 	      {
// 	      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
// 	      RunCommand(pdflatex_command,true);
// 	      if (ERRPROCESS && !LogExists()) 
// 		  {
// 		  QMessageBox::warning( this,tr("Error"),tr("Could not start the command."));
// 		  checkViewerInstance=false;
// 		  return;
// 		  }
// 	      LoadLog();
// 	      if (showoutputview) ViewLog();
// 	      if (NoLatexErrors()) 
// 		  {
// 		  if (!ERRPROCESS) ViewPDF();
// 		  else {checkViewerInstance=false;return;}
// 		  }
// 	      else {NextError();}
// 	      }
// 	  else return;
	  }
      else {NextError();}
      }break;
  case 9:
      {
      //stat2->setText(QString(" %1 ").arg("LatexMk"));
      RunCommand(latexmk_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latexmk_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  if (!ERRPROCESS) ViewPDF();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
  case 10:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(xelatex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+xelatex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  if (!ERRPROCESS) ViewPDF();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
  case 11:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(lualatex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+lualatex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  if (!ERRPROCESS) ViewPDF();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
    case 12:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(pdflatex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Asymptote"));
	  if (!ERRPROCESS) RunCommand(bibtex_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) 
	      {
	      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
	      RunCommand(pdflatex_command,true);
	      if (ERRPROCESS && !LogExists()) 
		  {
		  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
		  checkViewerInstance=false;
		  return;
		  }
	      LoadLog();
	      if (showoutputview) ViewLog();
	      if (NoLatexErrors()) 
		  {
		  RunCommand(pdflatex_command,true);
		  if (ERRPROCESS && !LogExists()) 
		      {
		      QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+pdflatex_command);
		      checkViewerInstance=false;
		      return;
		      }
		  LoadLog();
		  if (showoutputview) ViewLog();
		  if (NoLatexErrors()) 
		      {
		      if (!ERRPROCESS) ViewPDF();
		      else {checkViewerInstance=false;return;}
		      }
		      else {NextError();}
		  }
	      else {NextError();}
	      }
	  else return;
	  }
      else {NextError();}
      }break;
    case 13:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(latex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  //stat2->setText(QString(" %1 ").arg("Asymptote"));
	  if (!ERRPROCESS) RunCommand(bibtex_command,true);
	  else {checkViewerInstance=false;return;}
	  if (!ERRPROCESS) 
	      {
	      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
	      RunCommand(latex_command,true);
	      if (ERRPROCESS && !LogExists()) 
		  {
		  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
		  checkViewerInstance=false;
		  return;
		  }
	      LoadLog();
	      if (showoutputview) ViewLog();
	      if (NoLatexErrors()) 
		  {
		  RunCommand(latex_command,true);
		  if (ERRPROCESS && !LogExists()) 
		      {
		      QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+latex_command);
		      checkViewerInstance=false;
		      return;
		      }
		  LoadLog();
		  if (showoutputview) ViewLog();
		  if (NoLatexErrors()) 
		      {
		      //stat2->setText(QString(" %1 ").arg("Dvips"));
		      if (!ERRPROCESS) RunCommand(dvips_command,true);
		      else {checkViewerInstance=false;return;}
		      //stat2->setText(QString(" %1 ").arg("Ps to Pdf"));
		      if (!ERRPROCESS) RunCommand(ps2pdf_command,true);
		      else {checkViewerInstance=false;return;}
		      if (!ERRPROCESS) ViewPDF();
		      }
		      else {NextError();}
		  }
	      else {NextError();}
	      }
	  else return;
	  }
      else {NextError();}
      }break;
  case 14:
      {
      //stat2->setText(QString(" %1 ").arg("Pdf Latex"));
      RunCommand(sweave_command,true);
      if (!ERRPROCESS) RunCommand(pdflatex_command,true);
      if (ERRPROCESS && !LogExists()) 
	  {
	  QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n"+sweave_command);
	  checkViewerInstance=false;
	  return;
	  }
      LoadLog();
      if (showoutputview) ViewLog();
      if (NoLatexErrors()) 
	  {
	  if (!ERRPROCESS) ViewPDF();
	  else {checkViewerInstance=false;return;}
	  }
      else {NextError();}
      }break;
  }
  if (NoLatexErrors() && showoutputview) ViewLog();
  }
checkViewerInstance=false;
}

void Texmaker::Latex()
{
//stat2->setText(QString(" %1 ").arg("Latex"));
RunCommand(latex_command,true);
LoadLog();
if (showoutputview) ViewLog();
if (!NoLatexErrors()) NextError();
}

void Texmaker::ViewDvi()
{
//stat2->setText(QString(" %1 ").arg(tr("View Dvi file")));
RunCommand(viewdvi_command,false);
}

void Texmaker::DviToPS()
{
//stat2->setText(QString(" %1 ").arg("Dvips"));
RunCommand(dvips_command,false);
}

void Texmaker::ViewPS()
{
//stat2->setText(QString(" %1 ").arg(tr("View PS file")));
RunCommand(viewps_command,false);
}

void Texmaker::PDFLatex()
{
//stat2->setText(QString(" %1 ").arg("Pdf Latex"));
RunCommand(pdflatex_command,true);
LoadLog();
if (showoutputview) ViewLog();
if (!NoLatexErrors()) NextError();
}

void Texmaker::ViewPDF()
{
//stat2->setText(QString(" %1 ").arg(tr("View Pdf file")));
RunCommand(viewpdf_command,false);
}

void Texmaker::MakeBib()
{
//stat2->setText(QString(" %1 ").arg("Bibtex"));
RunCommand(bibtex_command,false);
}

void Texmaker::MakeIndex()
{
//stat2->setText(QString(" %1 ").arg("Make index"));
RunCommand(makeindex_command,false);
}

void Texmaker::PStoPDF()
{
//stat2->setText(QString(" %1 ").arg("Ps -> Pdf"));
RunCommand(ps2pdf_command,false);
}

void Texmaker::DVItoPDF()
{
//stat2->setText(QString(" %1 ").arg("Dvi -> Pdf"));
RunCommand(dvipdf_command,false);
}

void Texmaker::MetaPost()
{
//stat2->setText(QString(" %1 ").arg("Mpost"));
QString finame=getName();
QFileInfo fi(finame);
RunCommand(metapost_command+fi.completeBaseName()+"."+fi.suffix(),false);
}

void Texmaker::CleanAll()
{
QString finame,f;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) || finame.startsWith("untitled") || finame=="")
	{
	QMessageBox::warning( this,tr("Error"),tr("Can't detect the file name"));
	return;
	}
fileSave();
QStringList extension=QString(".log,.aux,.dvi,.lof,.lot,.bit,.idx,.glo,.bbl,.ilg,.toc,.ind,.out,.synctex.gz,.blg,.thm,.pre,.nlg,.nlo,.nls,.bcf,.snm,.nav,.vrb,.listing,.spx,.run.xml").split(",");
if (useoutputdir)
{
QFileInfo fi(outputName(finame,".pdf"));
  if(QMessageBox::warning(this, "Texmaker", tr("Make a copy of the %1.pdf/ps document in the \"build\" subdirectory and delete all the others %1.* files?").arg(fi.baseName()),tr("Ok"), tr("Cancel") )==0)
  {
  QDirIterator iterator(QDir(fi.absolutePath()),QDirIterator::NoIteratorFlags);
  while(iterator.hasNext())
    {
    QString entry(iterator.next());
    if (QFileInfo(entry).suffix().toLower()=="pdf" && QFileInfo(entry).baseName()==fi.baseName())
      {
      copyFile(entry,QFileInfo(finame).absolutePath()+"/"+QFileInfo(entry).completeBaseName()+".pdf");
      }
    else if (QFileInfo(entry).suffix().toLower()=="ps" && QFileInfo(entry).baseName()==fi.baseName())
      {
      copyFile(entry,QFileInfo(finame).absolutePath()+"/"+QFileInfo(entry).completeBaseName()+".ps");
      }
    else if(!QFileInfo(entry).isDir() && QFileInfo(entry).baseName()==fi.baseName())
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
else
{
QFileInfo fi(finame);
if (QMessageBox::warning(this, "Texmaker", tr("Delete the output files generated by LaTeX ?"),tr("Delete Files"), tr("Cancel") )==0)
  {
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
}


void Texmaker::AutoCleanAll()
{
QString finame,f;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) || finame.startsWith("untitled") || finame=="")
	{
	//QMessageBox::warning( this,tr("Error"),tr("Can't detect the file name"));
	return;
	}
QStringList extension=QString(".log,.aux,.dvi,.lof,.lot,.bit,.idx,.glo,.bbl,.ilg,.toc,.ind,.out,.synctex.gz,.blg,.thm,.pre,.nlg,.nlo,.nls,.bcf,.snm,.nav,.vrb,.listing,.spx,.run.xml").split(",");
if (useoutputdir)
{
QFileInfo fi(outputName(finame,".pdf"));
 
QDirIterator iterator(QDir(fi.absolutePath()),QDirIterator::NoIteratorFlags);
while(iterator.hasNext())
  {
  QString entry(iterator.next());
  if (QFileInfo(entry).suffix().toLower()=="pdf" && QFileInfo(entry).baseName()==fi.baseName())
    {
    copyFile(entry,QFileInfo(finame).absolutePath()+"/"+QFileInfo(entry).completeBaseName()+".pdf");
    }
  else if (QFileInfo(entry).suffix().toLower()=="ps" && QFileInfo(entry).baseName()==fi.baseName())
    {
    copyFile(entry,QFileInfo(finame).absolutePath()+"/"+QFileInfo(entry).completeBaseName()+".ps");
    }
  else if(!QFileInfo(entry).isDir() && QFileInfo(entry).baseName()==fi.baseName())
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
else
{
QFileInfo fi(finame);
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


void Texmaker::Asymptote()
{
//stat2->setText(QString(" %1 ").arg("Asymptote"));
RunCommand(asymptote_command,false);
}

void Texmaker::AsyFile(QString asyfile)
{
QString commandline=asymptote_command;
QFileInfo fi(asyfile);
commandline.replace("%","\""+asyfile+"\"");
proc = new QProcess( this );
proc->setWorkingDirectory(fi.absolutePath());
proc->setProperty("command",commandline);

//****
#if defined(Q_OS_MAC)
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
if (extra_path.isEmpty()) env.insert("PATH", env.value("PATH") + ":/Library/TeX/texbin:/Library/TeX/Distributions/.DefaultTeX/Contents/Programs/texbin:/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin:");
else
 env.insert("PATH", env.value("PATH") + ":/Library/TeX/texbin:/Library/TeX/Distributions/.DefaultTeX/Contents/Programs/texbin:/usr/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/texbin:/sw/bin:"+extra_path);
proc->setProcessEnvironment(env);
#endif
#if defined(Q_OS_WIN32)
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
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
//****
connect(proc, SIGNAL(readyReadStandardError()),this, SLOT(readFromStderr()));
//connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(readFromStdoutput()));
connect(proc, SIGNAL(finished(int)),this, SLOT(SlotEndProcess(int)));
OutputTextEdit->clear();
OutputTableWidget->hide();
OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
separatorline->hide();
//OutputTextEdit->insertLine(commandline+"\n");
proc->start(commandline);
if (!proc->waitForStarted(1000)) 
	{
	ERRPROCESS=true;
	OutputTextEdit->insertLine("Error : could not start the command : "+commandline+"\n");
	checkViewerInstance=false;
	return;
	}
else OutputTextEdit->insertLine("Process started\n");
FINPROCESS=false;
QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
while (!FINPROCESS) 
	{
	qApp->instance()->processEvents(QEventLoop::ExcludeUserInputEvents);
	}
QApplication::restoreOverrideCursor();
}

void Texmaker::LatexMk()
{
//stat2->setText(QString(" %1 ").arg("LatexMk"));
RunCommand(latexmk_command,true);
LoadLog();
if (showoutputview) ViewLog();
if (!NoLatexErrors()) NextError();
}

void Texmaker::Sweave()
{
//stat2->setText(QString(" %1 ").arg("LatexMk"));
RunCommand(sweave_command,false);
}

void Texmaker::Xelatex()
{
//stat2->setText(QString(" %1 ").arg("Pdf Latex"));
RunCommand(xelatex_command,true);
LoadLog();
if (showoutputview) ViewLog();
if (!NoLatexErrors()) NextError();
}

void Texmaker::Lualatex()
{
//stat2->setText(QString(" %1 ").arg("Pdf Latex"));
RunCommand(lualatex_command,true);
LoadLog();
if (showoutputview) ViewLog();
if (!NoLatexErrors()) NextError();
}

void Texmaker::UserTool1()
{
QStringList commandList=UserToolCommand[0].split("|");
ERRPROCESS=false;
for (int i = 0; i < commandList.size(); ++i) 
	{
	if ((!ERRPROCESS)&&(!commandList.at(i).isEmpty())) 
	  {
	  RunCommand(commandList.at(i),true);
	  if ((commandList.at(i)==latex_command) || (commandList.at(i)==pdflatex_command) || (commandList.at(i)==xelatex_command) || (commandList.at(i)==lualatex_command))
		{
		LoadLog();
		if (showoutputview) ViewLog();
		if (!NoLatexErrors()) NextError();
		}
	  }
        else return;
	}
}

void Texmaker::UserTool2()
{
QStringList commandList=UserToolCommand[1].split("|");
ERRPROCESS=false;
for (int i = 0; i < commandList.size(); ++i) 
	{
	if ((!ERRPROCESS)&&(!commandList.at(i).isEmpty())) 
	  {
	  RunCommand(commandList.at(i),true);
	  if ((commandList.at(i)==latex_command) || (commandList.at(i)==pdflatex_command) || (commandList.at(i)==xelatex_command) || (commandList.at(i)==lualatex_command))
		{
		LoadLog();
		if (showoutputview) ViewLog();
		if (!NoLatexErrors()) NextError();
		}
	  }
        else return;
	}
}

void Texmaker::UserTool3()
{
QStringList commandList=UserToolCommand[2].split("|");
ERRPROCESS=false;
for (int i = 0; i < commandList.size(); ++i) 
	{
	if ((!ERRPROCESS)&&(!commandList.at(i).isEmpty())) 
	  {
	  RunCommand(commandList.at(i),true);
	  if ((commandList.at(i)==latex_command) || (commandList.at(i)==pdflatex_command) || (commandList.at(i)==xelatex_command) || (commandList.at(i)==lualatex_command))
		{
		LoadLog();
		if (showoutputview) ViewLog();
		if (!NoLatexErrors()) NextError();
		}
	  }
        else return;
	}
}

void Texmaker::UserTool4()
{
QStringList commandList=UserToolCommand[3].split("|");
ERRPROCESS=false;
for (int i = 0; i < commandList.size(); ++i) 
	{
	if ((!ERRPROCESS)&&(!commandList.at(i).isEmpty())) 
	  {
	  RunCommand(commandList.at(i),true);
	  if ((commandList.at(i)==latex_command) || (commandList.at(i)==pdflatex_command) || (commandList.at(i)==xelatex_command) || (commandList.at(i)==lualatex_command))
		{
		LoadLog();
		if (showoutputview) ViewLog();
		if (!NoLatexErrors()) NextError();
		}
	  }
        else return;
	}
}

void Texmaker::UserTool5()
{
QStringList commandList=UserToolCommand[4].split("|");
ERRPROCESS=false;
for (int i = 0; i < commandList.size(); ++i) 
	{
	if ((!ERRPROCESS)&&(!commandList.at(i).isEmpty())) 
	  {
	  RunCommand(commandList.at(i),true);
	  if ((commandList.at(i)==latex_command) || (commandList.at(i)==pdflatex_command) || (commandList.at(i)==xelatex_command) || (commandList.at(i)==lualatex_command))
		{
		LoadLog();
		if (showoutputview) ViewLog();
		if (!NoLatexErrors()) NextError();
		}
	  }
        else return;
	}
}

void Texmaker::OpenTerminal()
{
QProcess process;

QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) || finame.startsWith("untitled") || finame=="")
	{
	finame=QDir::homePath();
	}
QFileInfo fi(finame); 


const QString pwd = QDir::toNativeSeparators(fi.isDir() ?
                                                 fi.absoluteFilePath() :
                                                 fi.absolutePath());
process.setWorkingDirectory(pwd);
#if defined(Q_OS_WIN32)
const QString terminalEmulator = QString::fromLocal8Bit(qgetenv("COMSPEC"));
#include <Windows.h>
STARTUPINFO si;
ZeroMemory(&si, sizeof(si));
si.cb = sizeof(si);

PROCESS_INFORMATION pinfo;
ZeroMemory(&pinfo, sizeof(pinfo));

bool success = CreateProcessW(0, (WCHAR *)terminalEmulator.utf16(),
                              0, 0, FALSE, CREATE_NEW_CONSOLE,
                              0, pwd.isEmpty() ? 0 : (WCHAR *)pwd.utf16(),
                              &si, &pinfo);

if (success) {
    CloseHandle(pinfo.hThread);
    CloseHandle(pinfo.hProcess);
}
#elif defined(Q_OS_MAC)
const QString terminalEmulator = QCoreApplication::applicationDirPath() + "/../Resources/openTerminal.command";
process.startDetached(terminalEmulator,QStringList()<< pwd);
#else
const QString shell = QString::fromLocal8Bit(qgetenv("SHELL"));
if (!process.startDetached("konsole --workdir \""+pwd+"\""))
  {
  if (!process.startDetached("gnome-terminal --working-directory=\""+pwd+"\"")) process.startDetached("xterm -lc -u8 -e \"cd \'"+pwd+"\' && "+shell+"\"");
  }
#endif

}

void Texmaker::Export()
{
if ( !currentEditorView() ) return;
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) || finame.startsWith("untitled") || finame=="")
	{
	QMessageBox::warning( this,tr("Error"),tr("Can't detect the file name"));
	return;
	}
ExportDialog *exportDlg = new ExportDialog(this,finame);
exportDlg->ui.lineEditPath->setText(htlatex_command);
exportDlg->ui.lineEditOptions->setText(htlatex_options);

if (exportDlg->exec())
	{
	htlatex_command=exportDlg->ui.lineEditPath->text();
	htlatex_options=exportDlg->ui.lineEditOptions->text();
	}
}

void Texmaker::ConvertToUnicode()
{
 UnicodeDialog *uniDlg = new UnicodeDialog(this);
 uniDlg->init(EditorFont,showline,edcolors(),hicolors());
 uniDlg->exec();
}

void Texmaker::EditUserTool()
{
QStringList usualNames, usualCommands;
usualNames << tr("LaTeX") << tr("PdfLaTeX") << tr("dvips") << tr("Dvi Viewer") << tr("PS Viewer") << tr("Dvipdfm") << tr("ps2pdf") << tr("Bibtex") << tr("Makeindex") << tr("Pdf Viewer") << tr("metapost") << tr("ghostscript") << tr("Asymptote") << tr("Latexmk") << tr("R Sweave") << tr("XeLaTex") << tr("LuaLaTex");
usualCommands << latex_command << pdflatex_command << dvips_command << viewdvi_command << viewps_command << dvipdf_command << ps2pdf_command << bibtex_command << makeindex_command<< viewpdf_command << metapost_command << ghostscript_command << asymptote_command << latexmk_command << sweave_command << xelatex_command << lualatex_command;
QAction *Act;
UserToolDialog *utDlg = new UserToolDialog(this,tr("Edit User &Commands"),usualNames, usualCommands);
for ( int i = 0; i <= 4; i++ )
	{
	utDlg->Name[i]=UserToolName[i];
	utDlg->Tool[i]=UserToolCommand[i];
	}
utDlg->init();
if ( utDlg->exec() )
	{
	for ( int i = 0; i <= 4; i++ )
		{
		UserToolName[i]=utDlg->Name[i];
		UserToolCommand[i]=utDlg->Tool[i];
		}
	user12Menu->clear();
	Act = new QAction("1: "+UserToolName[0], this);
	Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F1);
	connect(Act, SIGNAL(triggered()), this, SLOT(UserTool1()));
	user12Menu->addAction(Act);
	Act = new QAction("2: "+UserToolName[1], this);
	Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F2);
	connect(Act, SIGNAL(triggered()), this, SLOT(UserTool2()));
	user12Menu->addAction(Act);
	Act = new QAction("3: "+UserToolName[2], this);
	Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F3);
	connect(Act, SIGNAL(triggered()), this, SLOT(UserTool3()));
	user12Menu->addAction(Act);
	Act = new QAction("4: "+UserToolName[3], this);
	Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F4);
	connect(Act, SIGNAL(triggered()), this, SLOT(UserTool4()));
	user12Menu->addAction(Act);
	Act = new QAction("5: "+UserToolName[4], this);
	Act->setShortcut(Qt::SHIFT+Qt::ALT+Qt::Key_F5);
	connect(Act, SIGNAL(triggered()), this, SLOT(UserTool5()));
	user12Menu->addAction(Act);
	user12Menu->addSeparator();
	Act = new QAction(tr("Edit User &Commands"), this);
	connect(Act, SIGNAL(triggered()), this, SLOT(EditUserTool()));
	user12Menu->addAction(Act);
	QStringList list;
	list.append(tr("Quick Build"));
	list.append("LaTeX");
	list.append("Dvi->PS");
	list.append("PDFLaTeX");
	list.append("BibTeX");
	list.append("MakeIndex");
	list.append("MPost");
	list.append("PS->PDF");
	list.append("DVI->PDF");
	list.append("Asymptote");
	list.append("LatexMk");
	list.append("R Sweave");
	list.append("XeLaTeX");
	list.append("LuaLaTeX");
	int runIndex=comboCompil->currentIndex();
	for ( int i = 0; i <= 4; i++ ) comboCompil->setItemText(14+i,QString::number(i+1)+": "+UserToolName[i]);
	comboCompil->setCurrentIndex(runIndex);
	}
}

void Texmaker::doCompile()
{
switch (comboCompil->currentIndex())
	{
	case 0:
		{
QuickBuild();
		}break;
	case 1:
		{
Latex();
		}break;
	case 2:
		{
DviToPS();
		}break;
	case 3:
		{
PDFLatex();
		}break;
	case 4:
		{
MakeBib();
		}break;
	case 5:
		{
MakeIndex();
		}break;
	case 6:
		{
MetaPost();
		}break;
	case 7:
		{
PStoPDF();
		}break;
	case 8:
		{
DVItoPDF();
		}break;
	case 9:
		{
Asymptote();
		}break;
	case 10:
		{
LatexMk();
		}break;
	case 11:
		{
Sweave();
		}break;
	case 12:
		{
Xelatex();
		}break;
	case 13:
		{
Lualatex();
		}break;
	case 14:
		{
UserTool1();
		}break;
	case 15:
		{
UserTool2();
		}break;
	case 16:
		{
UserTool3();
		}break;
	case 17:
		{
UserTool4();
		}break;
	case 18:
		{
UserTool5();
		}break;
	}
}

void Texmaker::doView()
{
switch (comboView->currentIndex())
	{
	case 0:
		{
ViewDvi();
		}break;
	case 1:
		{
ViewPS();
		}break;
	case 2:
		{
ViewPDF();
		}break;
	}
}

void Texmaker::jumpToPdfline(int line)
{
if (!builtinpdfview) return;
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) || finame.startsWith("untitled") || finame=="")
	{
	QMessageBox::warning( this,tr("Error"),tr("Can't detect the file name"));
	return;
	}
fileSave();
QFileInfo fi(finame);
if (!fi.exists()) return;
QString basename=fi.completeBaseName();
if (embedinternalpdf)
    {
    if (pdfviewerWidget)
      {
      if (pdfviewerWidget->pdf_file!=outputName(finame,".pdf")) pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
      StackedViewers->setCurrentWidget(pdfviewerWidget);
      //pdfviewerWidget->raise();
      pdfviewerWidget->show();
      if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1"))) pdfviewerWidget->jumpToPdfFromSource(getName(),line,1);
      pdfviewerWidget->getFocus();
      }
    else
      {
  //    pdfviewerWidget=new PdfViewer(outputName(finame,".pdf"),viewpdf_command, this);
      pdfviewerWidget=new PdfViewerWidget(outputName(finame,".pdf"),viewpdf_command,ghostscript_command,lp_options,QKeySequence(keyToggleFocus),pdfCheckerLang,lastScale,StackedViewers);
      pdfviewerWidget->centralToolBarBis->setMinimumHeight(centralToolBarBis->height());
      pdfviewerWidget->centralToolBarBis->setMaximumHeight(centralToolBarBis->height());
      connect(pdfviewerWidget, SIGNAL(openDocAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool)));
      connect(pdfviewerWidget, SIGNAL(sendFocusToEditor()), this, SLOT(getFocusToEditor()));
      connect(pdfviewerWidget, SIGNAL(sendPaperSize(const QString&)), this, SLOT(setPrintPaperSize(const QString&)));
      StackedViewers->addWidget(pdfviewerWidget);
      StackedViewers->setCurrentWidget(pdfviewerWidget);
      //pdfviewerWidget->raise();
      pdfviewerWidget->show();
      pdfviewerWidget->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
      if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1"))) pdfviewerWidget->jumpToPdfFromSource(getName(),line,1);
      pdfviewerWidget->getFocus();
      }
    }
else
    {
    if (pdfviewerWindow)
      {
      if (pdfviewerWindow->pdf_file!=outputName(finame,".pdf")) pdfviewerWindow->openFile(outputName(finame,".pdf"),viewpdf_command,ghostscript_command);
      pdfviewerWindow->raise();
      pdfviewerWindow->show();
      qApp->setActiveWindow(pdfviewerWindow);
      pdfviewerWindow->setFocus();
      if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1")) ) pdfviewerWindow->jumpToPdfFromSource(getName(),line,1);
      }
    else
      {
  //    pdfviewerWindow=new PdfViewer(outputName(finame,".pdf"),viewpdf_command, this);
      pdfviewerWindow=new PdfViewer(outputName(finame,".pdf"),viewpdf_command,ghostscript_command,lp_options,QKeySequence(keyToggleFocus),pdfCheckerLang,lastScale,0);
      connect(pdfviewerWindow, SIGNAL(openDocAtLine(const QString&, int, bool)), this, SLOT(fileOpenAndGoto(const QString&, int, bool)));
      connect(pdfviewerWindow, SIGNAL(sendFocusToEditor()), this, SLOT(getFocusToEditor()));
      connect(pdfviewerWindow, SIGNAL(sendPaperSize(const QString&)), this, SLOT(setPrintPaperSize(const QString&)));
      pdfviewerWindow->raise();
      pdfviewerWindow->show();
      if ( (pdflatex_command.contains("synctex=1")) || (latex_command.contains("synctex=1")) || (xelatex_command.contains("synctex=1"))) pdfviewerWindow->jumpToPdfFromSource(getName(),line,1);
      }  
    }
}


//////////////// MESSAGES - LOG FILE///////////////////////
bool Texmaker::LogExists()
{
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) ||finame.startsWith("untitled") || finame=="")
	{
	return false;
	}
QString logname=outputName(finame,".log");
// QFileInfo fi(finame);
// QString name=fi.absoluteFilePath();
// QString ext=fi.suffix();
// QString basename=name.left(name.length()-ext.length()-1);
// QString logname=basename+".log";
QFileInfo fic(logname);
if (fic.exists() && fic.isReadable()) return true;
else return false;
}

void Texmaker::LoadLog()
{
OutputTextEdit->clear();
logpresent=false;
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
if ((singlemode && !currentEditorView()) ||finame.startsWith("untitled") || finame=="")
	{
	QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n view log ");
	ERRPROCESS=true;
	return;
	}
QString logname=outputName(finame,".log");
// QFileInfo fi(finame);
// QString name=fi.absoluteFilePath();
// QString ext=fi.suffix();
// QString basename=name.left(name.length()-ext.length()-1);
// QString logname=basename+".log";
QString line;
QFileInfo fic(logname);
QTextCodec* codec = QTextCodec::codecForName(input_encoding.toLatin1());
if(!codec) codec = QTextCodec::codecForLocale();
int li=0;
if (fic.exists() && fic.isReadable() )
	{
	OutputTextEdit->insertLine("LOG FILE :");
	QFile f(logname);
	if ( f.open(QIODevice::ReadOnly))
		{
		QTextStream t( &f );
		t.setCodec(codec);
//		OutputTextEdit->setPlainText( t.readAll() );
		while ( (!t.atEnd()) && (li<100000) )
			{
			li++;
			line=t.readLine();
			line=line.simplified();
			if (!line.isEmpty()) OutputTextEdit->insertLine(line);
			}
		}
		f.close();
	LatexError();
	logpresent=true;
	}
else {QMessageBox::warning( this,tr("Error"),tr("Log File not found !"));}
}
void Texmaker::ViewLog()
{
LoadLog();
if (!showoutputview) ShowOutputView(true);
}

void Texmaker::ClickedOnOutput(int l)
{
if ( !currentEditorView() ) return;
currentEditorView()->editor->gotoLine(l);
}

void Texmaker::ClickedOnLogLine(QTableWidgetItem *item)
{
if ( !currentEditorView() ) return;
if ( !item ) return;
QString content=item->text();
int row=OutputTableWidget->row(item);
int col=OutputTableWidget->column(item);
if (col!=3) content=OutputTableWidget->item(row,3)->text();
QString file=OutputTableWidget->item(row,1)->text();

int Start, End;
bool ok;
QString s;
QString line="";
//// l. ///
s = content;
Start=End=0;
Start=s.indexOf(QRegExp("l.[0-9]"), End);
if (Start!=-1)
	{
	Start=Start+2;
	s=s.mid(Start,s.length());
	
	if (End!=-1)
	line=s.mid(0,End);
	else
	line=s.mid(0,s.length());
	};
//// line ///
s = content;
Start=End=0;
Start=s.indexOf(QRegExp("line [0-9]"), End);
if (Start!=-1)
	{
	Start=Start+5;
	s=s.mid(Start,s.length());
	End=s.indexOf(QRegExp("[ a-zA-Z.\\-]"),0);
	if (End!=-1)
	line=s.mid(0,End);
	else
	line=s.mid(0,s.length());
	};
//// lines ///
s = content;
Start=End=0;
Start=s.indexOf(QRegExp("lines [0-9]"), End);
if (Start!=-1)
	{
	Start=Start+6;
	s=s.mid(Start,s.length());
	End=s.indexOf(QRegExp("[ a-zA-Z.\\-]"),0);
	if (End!=-1)
	line=s.mid(0,End);
	else
	line=s.mid(0,s.length());
	};
int l=line.toInt(&ok,10)-1;
if (ok)
	{
	if (file.isEmpty()) currentEditorView()->editor->gotoLine(l);
	else 
	  {
	  QFileInfo fi(file);
	  if (fi.exists() && fi.isReadable() ) fileOpenAndGoto(fi.absoluteFilePath(),l+1,true);
	  else 
	    {
	    QFileInfo fic(getName());
	    file=fic.absolutePath()+"/"+file;
	    QFileInfo ffi(file);
	    if (ffi.exists() && ffi.isReadable() ) fileOpenAndGoto(file,l+1,true);
	    }
	  }
	}
QString ll=item->data(Qt::UserRole).toString();
int logline=ll.toInt(&ok,10)-1;
OutputTextEdit->setCursorPosition(logline , 0);
}

////////////////////////// ERRORS /////////////////////////////
void Texmaker::LatexError()
{
errorFileList.clear();
errorTypeList.clear();
errorLineList.clear();
errorMessageList.clear();
errorLogList.clear();
onlyErrorList.clear();
errorIndex=-1;

QString mot,suivant,lettre,expression,warning,latexerror,badbox,name;
QStringList pile,filestack;
filestack.clear();
pile.clear();

int j;

int ligne=0;
int par=1;
int errorpar=1;

QRegExp rxWarning1("Warning: (.*) on.*line *(\\d+)");
QRegExp rxWarning2("Warning: (.*)");
QRegExp rxLatexError("(! )*(LaTeX Error:)* *(.*)\\.l\\.(\\d+) *(.*)");
QRegExp rxLineError("l\\.(\\d+)");
QRegExp rxBadBox("at (line|lines) ([0-9]+)");
QRegExp rxFile("(.*(\\.tex|\\.sty|\\.cls))");


QTextBlock tb = OutputTextEdit->document()->begin();
while (tb.isValid())
 	{
	errorpar=par;
	expression=tb.text();
	j=0;
	pile.clear();
	while (j<expression.length())
		{
		lettre=expression.mid(j,1);
		if (lettre=="(" || lettre==")") 
			{
			pile.prepend(lettre);
			j+=1;
			}
		else
			{
			mot="";
			while (j<expression.length() && (expression.mid(j,1)!="(" && expression.mid(j,1)!=")")) 
				{
				mot+=expression.mid(j,1);
				j+=1;
				}
			pile.prepend(mot);
			}
		}
	while (pile.count()>0)
		{
		mot=pile.last();
		pile.removeLast();
		if (mot=="(" && pile.count()>0)
			{
			suivant=pile.last();
			pile.removeLast();
			if (rxFile.indexIn(suivant) != -1) name=rxFile.cap(1);
			else name="";
			if (name.startsWith("./")) name.remove("./");
			filestack.append(name);
			}
		else if (mot==")" && filestack.count()>0) filestack.removeLast();
		}
	if (expression.contains("Warning"))
		{
		warning=expression.trimmed();
		while (tb.isValid() && !expression.contains(QRegExp("\\.$")))
			{
			par++;
			tb=tb.next();
			if (tb.isValid())
				{
				expression=tb.text();
				warning+=expression.trimmed();
				}
			}

		if ( rxWarning1.indexIn(warning) != -1 )
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Warning");
			errorLineList.append(rxWarning1.cap(2));
			errorMessageList.append(rxWarning1.cap(1).replace("*",""));
			errorLogList.append(QString::number(errorpar));
			}
		else if ( rxWarning2.indexIn(warning) != -1 )
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Warning");
			errorLineList.append("1");
			errorMessageList.append(rxWarning2.cap(1).replace("*",""));
			errorLogList.append(QString::number(errorpar));
			}
		else 
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Warning");
			errorLineList.append("1");
			errorMessageList.append(warning.replace("*",""));
			errorLogList.append(QString::number(errorpar));
 			}
		errorpar=par;
		}
//	else if (expression.contains(QRegExp("^!")))
	else if (expression.contains(QRegExp("^! (.*)")))
		{
		latexerror=expression.trimmed();
		while (tb.isValid() && !expression.contains(rxLineError))
			{
			par++;
			tb=tb.next();
			if (tb.isValid())
				{
				expression=tb.text();
				latexerror+=expression.trimmed();
				}
			}
// 		if ( rxLatexError.indexIn(latexerror) != -1 )
// 			{
// 			errorFileList.append(filestack.last());
// 			errorTypeList.append("Error");
// 			errorLineList.append(rxLatexError.cap(1));
// 			errorMessageList.append(rxLatexError.cap(3)+" :"+rxLatexError.cap(5));
// 			}
		if ( rxLineError.indexIn(latexerror) != -1 )
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Error");
			errorLineList.append(rxLineError.cap(1));
			errorMessageList.append(latexerror.remove(rxLineError).replace("*",""));
			errorLogList.append(QString::number(errorpar));
			}
		else 
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Error");
			errorLineList.append("1");
			errorMessageList.append(latexerror.replace("*",""));
			errorLogList.append(QString::number(errorpar));
			}
		errorpar=par;
		}
	else if (expression.contains(QRegExp("^(Over|Under)(full \\\\[hv]box .*)")))
		{
		badbox=expression.trimmed();
/*		while (tb.isValid() && !expression.contains(QRegExp("(.*) at line")))
			{
			par++;
			tb=tb.next();
			if (tb.isValid())
				{
				expression=tb.text();
				badbox+=expression.trimmed();
				}
			}*/
		if ( rxBadBox.indexIn(badbox) != -1 )
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Badbox");
			errorLineList.append(rxBadBox.cap(2));
			errorMessageList.append(badbox.replace("*",""));
			errorLogList.append(QString::number(errorpar));
			}
		else
			{
			if (!filestack.isEmpty()) errorFileList.append(filestack.last());
			else errorFileList.append("");
			errorTypeList.append("Badbox");
			errorLineList.append("1");
			errorMessageList.append(badbox.replace("*",""));
			errorLogList.append(QString::number(errorpar));
			}
		errorpar=par;
		}
	if (tb.isValid()) 
		{
		par++;
		tb = tb.next();
		}
	}
DisplayLatexError();
}

void Texmaker::DisplayLatexError()
{
OutputTableWidget->clearContents();
QFontMetrics fm(qApp->font());
int rowheight=fm.lineSpacing()+4;
int maxwidth=0;
int maxwidthline=0;
int row=0;
if (errorFileList.count()>0)
	{
	OutputTableWidget->setRowCount(errorFileList.count());
	for ( int i = 0; i <errorFileList.count(); i++ )
		{
		if (errorTypeList.at(i)=="Error")
			{
			QTableWidgetItem *ItemFile = new QTableWidgetItem(errorFileList.at(i));
			ItemFile->setData(Qt::UserRole,errorLogList.at(i));
			ItemFile->setForeground(QBrush(QColor(Qt::red)));
			ItemFile->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			OutputTableWidget->setItem(row,1, ItemFile);
			QTableWidgetItem *ItemType = new QTableWidgetItem(errorTypeList.at(i));
			ItemType->setData(Qt::UserRole,errorLogList.at(i));
			ItemType->setForeground(QBrush(QColor(Qt::red)));
			ItemType->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			OutputTableWidget->setItem(row,2, ItemType);
			QTableWidgetItem *ItemLine = new QTableWidgetItem("line "+errorLineList.at(i));
			if (fm.width("line  "+errorLineList.at(i))>maxwidthline) maxwidthline=fm.width("line  "+errorLineList.at(i));
			ItemLine->setData(Qt::UserRole,errorLogList.at(i));
			ItemLine->setForeground(QBrush(QColor(Qt::red)));
			ItemLine->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ItemLine->setToolTip(tr("Click to jump to the line"));
			OutputTableWidget->setItem(row,3, ItemLine);
			QTableWidgetItem *ItemMessage = new QTableWidgetItem(errorMessageList.at(i));
			if (fm.width(errorMessageList.at(i))>maxwidth) maxwidth=fm.width(errorMessageList.at(i));
			ItemMessage->setData(Qt::UserRole,errorLogList.at(i));
			ItemMessage->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ItemMessage->setForeground(QBrush(QColor(Qt::red)));
			OutputTableWidget->setItem(row,4, ItemMessage);

			onlyErrorList.append(i);
			row++;
			}
		}
	for ( int i = 0; i <errorFileList.count(); i++ )
		{
		if (errorTypeList.at(i)!="Error")
			{
			QTableWidgetItem *ItemFile = new QTableWidgetItem(errorFileList.at(i));
			ItemFile->setData(Qt::UserRole,errorLogList.at(i));
			ItemFile->setForeground(QBrush(QColor(Qt::blue)));
			ItemFile->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			OutputTableWidget->setItem(row,1, ItemFile);
			QTableWidgetItem *ItemType = new QTableWidgetItem(errorTypeList.at(i));
			ItemType->setData(Qt::UserRole,errorLogList.at(i));
			ItemType->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ItemType->setForeground(QBrush(QColor(Qt::blue)));
			OutputTableWidget->setItem(row,2, ItemType);
			QTableWidgetItem *ItemLine = new QTableWidgetItem("line "+errorLineList.at(i));
			if (fm.width("line  "+errorLineList.at(i))>maxwidthline) maxwidthline=fm.width("line  "+errorLineList.at(i));
			ItemLine->setData(Qt::UserRole,errorLogList.at(i));
			ItemLine->setForeground(QBrush(QColor(Qt::blue)));
			ItemLine->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ItemLine->setToolTip(tr("Click to jump to the line"));
			OutputTableWidget->setItem(row,3, ItemLine);
			QTableWidgetItem *ItemMessage = new QTableWidgetItem(errorMessageList.at(i));
			if (fm.width(errorMessageList.at(i))>maxwidth) maxwidth=fm.width(errorMessageList.at(i));
			ItemMessage->setData(Qt::UserRole,errorLogList.at(i));
			ItemMessage->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ItemMessage->setForeground(QBrush(QColor(Qt::blue)));
			OutputTableWidget->setItem(row,4, ItemMessage);
			row++;
			}
		}
	//OutputTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	OutputTableWidget->horizontalHeader()->resizeSection(3,maxwidthline+10);
	//OutputTableWidget->horizontalHeader()->resizeSection(4,maxwidth);
	//OutputTableWidget->setColumnWidth(4,maxwidth);
	//OutputTableWidget->setColumnWidth(3,maxwidthline);
	for ( int i = 0; i<OutputTableWidget->rowCount(); ++i )
		{
		OutputTableWidget->setRowHeight(i,rowheight);
		}
	OutputTableWidget->resizeColumnsToContents();
//	OutputTableWidget->resizeRowsToContents();
	}
else
	{
	OutputTableWidget->setRowCount(1);
	OutputTableWidget->setRowHeight(0,rowheight);
	OutputTableWidget->clearContents();
	}
OutputTextEdit->setMaximumHeight(5*(fm.lineSpacing()+4));
OutputTableWidget->show();
separatorline->show();
OutputTextEdit->setCursorPosition(0 , 0);
}

bool Texmaker::NoLatexErrors()
{
return onlyErrorList.isEmpty();
}

void Texmaker::NextError()
{
int line=0;
QTableWidgetItem *Item;
if (!logpresent) {LoadLog();}
if (!showoutputview) ShowOutputView(true);
//ViewLog();
if (logpresent && !onlyErrorList.isEmpty())
  	{
	if (errorIndex<onlyErrorList.size()-1) errorIndex=errorIndex+1;
	if (errorIndex<0) errorIndex=0;
	if (errorIndex>=onlyErrorList.count()) errorIndex=onlyErrorList.count()-1;
	if (singlemode)// && onlyErrorList.at(errorIndex)!=0) 
		{
		line=errorLineList.at(onlyErrorList.at(errorIndex)).toInt();
		for ( int i = 0; i <errorFileList.count(); i++ )
			{
			Item = new QTableWidgetItem(" ");
			OutputTableWidget->setItem(i,0, Item);
			}
		Item = new QTableWidgetItem(">");
		OutputTableWidget->setItem(errorIndex,0, Item);
		OutputTableWidget->scrollToItem(Item,QAbstractItemView::PositionAtCenter);
		if (line>1) ClickedOnOutput(line-1);
		int logline=errorLogList.at(onlyErrorList.at(errorIndex)).toInt()-1;
		OutputTextEdit->setCursorPosition(logline , 0);
		}
  	}
if (logpresent && onlyErrorList.isEmpty())
	{
	QMessageBox::information( this,"Texmaker",tr("No LaTeX errors detected !"));
	OutputTextEdit->setCursorPosition(0 , 0);
	}
}

void Texmaker::PreviousError()
{
int line=0;
QTableWidgetItem *Item;
if (!logpresent) {LoadLog();}
if (!showoutputview) ShowOutputView(true);
//ViewLog();
if (logpresent && !onlyErrorList.isEmpty())
  	{
	if (errorIndex>0) errorIndex=errorIndex-1;
	if (errorIndex<0) errorIndex=0;
	if (errorIndex>=onlyErrorList.count()) errorIndex=onlyErrorList.count()-1;
	if (singlemode)// && onlyErrorList.at(errorIndex)!=0) 
		{
		line=errorLineList.at(onlyErrorList.at(errorIndex)).toInt();
		for ( int i = 0; i <errorFileList.count(); i++ )
			{
			Item = new QTableWidgetItem(" ");
			OutputTableWidget->setItem(i,0, Item);
			}
		Item = new QTableWidgetItem(">");
		OutputTableWidget->setItem(errorIndex,0, Item);
		OutputTableWidget->scrollToItem(Item,QAbstractItemView::PositionAtCenter);
		if (line>1) ClickedOnOutput(line-1);
		int logline=errorLogList.at(onlyErrorList.at(errorIndex)).toInt()-1;
		OutputTextEdit->setCursorPosition(logline , 0);
		}
  	}
if (logpresent && onlyErrorList.isEmpty())
	{
	QMessageBox::information( this,"Texmaker",tr("No LaTeX errors detected !"));
	OutputTextEdit->setCursorPosition(0 , 0);
	}
}

//////////////// HELP /////////////////
void Texmaker::LatexHelp()
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QString docfile=QCoreApplication::applicationDirPath() + "/latexhelp.html";
#else
QString docfile=PREFIX"/share/texmaker/latexhelp.html";
#endif

#endif
#if defined(Q_OS_MAC)
QString docfile=QCoreApplication::applicationDirPath() + "/../Resources/latexhelp.html";
#endif
#if defined(Q_OS_WIN32)
QString docfile=QCoreApplication::applicationDirPath() + "/latexhelp.html";
#endif
QFileInfo fic(docfile);
if (fic.exists() && fic.isReadable() )
	{
#ifdef INTERNAL_BROWSER
        if (browserWindow)
          {
          browserWindow->close();
          }
	browserWindow=new Browser("file:///"+docfile,true, 0);
	browserWindow->raise();
	browserWindow->show();
#else
QDesktopServices::openUrl("file:///"+docfile);
#endif
	}
else { QMessageBox::warning( this,tr("Error"),tr("File not found"));}
}

void Texmaker::UserManualHelp()
{
QString locale = QString(QLocale::system().name()).left(2);
if ( locale.length() < 2 || (locale!="fr" /*&& locale!="hu" && locale!="ru"*/) ) locale = "en";
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QString docfile=QCoreApplication::applicationDirPath() + "/usermanual_"+locale+".html";
#else
QString docfile=PREFIX"/share/texmaker/usermanual_"+locale+".html";
#endif

#endif
#if defined(Q_OS_MAC)
QString docfile=QCoreApplication::applicationDirPath() + "/../Resources/usermanual_"+locale+".html";
#endif
#if defined(Q_OS_WIN32)
QString docfile=QCoreApplication::applicationDirPath() + "/usermanual_"+locale+".html";
#endif
QFileInfo fic(docfile);
if (fic.exists() && fic.isReadable() )
	{
#ifdef INTERNAL_BROWSER
        if (browserWindow)
          {
          browserWindow->close();
          }
	browserWindow=new Browser("file:///"+docfile,true, 0);
	browserWindow->raise();
	browserWindow->show();
#else
QDesktopServices::openUrl("file:///"+docfile);
#endif
	}
else { QMessageBox::warning( this,tr("Error"),tr("File not found"));}
}

void Texmaker::TexDocHelp()
{
QString text="";
QString item="";
TexdocDialog *texdocDlg = new TexdocDialog(this);
texdocDlg->ui.lineEditCommand->setText(texdoc_command);
if (currentEditorView())
  {
  if (currentEditorView()->editor->textCursor().hasSelection()) item=currentEditorView()->editor->textCursor().selectedText();
  }
texdocDlg->ui.lineEdit->setText(item);
if (texdocDlg->exec())
	{
	text =texdocDlg->ui.lineEdit->text();
	texdoc_command=texdocDlg->ui.lineEditCommand->text();
	}
if (!text.isEmpty())
  {
  QProcess* texdocprocess=new QProcess();
  if (!texdocprocess->startDetached(QString("texdoc ")+text)) QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\n texdoc");
  }
}

void Texmaker::HelpAbout()
{
 AboutDialog *abDlg = new AboutDialog(this);
 abDlg->exec();
}

void Texmaker::CheckVersion()
{
 VersionDialog *verDlg = new VersionDialog(this);
 verDlg->exec();
}



// void Texmaker::Docufrlatex()
// {
// QDesktopServices::openUrl(QUrl("http://www.xm1math.net/doculatex/index.html"));
// }

void Texmaker::Doculatex()
{
QDesktopServices::openUrl(QUrl("http://en.wikibooks.org/wiki/LaTeX"));
}

////////////// OPTIONS //////////////////////////////////////
void Texmaker::GeneralOptions()
{
ConfigDialog *confDlg = new ConfigDialog(this);

confDlg->ui.lineEditSvn->setText(svnPath);
confDlg->ui.checkBoxSvn->setChecked(svnEnable);

confDlg->ui.lineEditLualatex->setText(lualatex_command);
confDlg->ui.lineEditXelatex->setText(xelatex_command);
confDlg->ui.lineEditPath->setText(extra_path);
confDlg->ui.lineEditLatex->setText(latex_command);
confDlg->ui.lineEditPdflatex->setText(pdflatex_command);
confDlg->ui.lineEditDvips->setText(dvips_command);
confDlg->ui.lineEditDviviewer->setText(viewdvi_command);
confDlg->ui.lineEditPsviewer->setText(viewps_command);
confDlg->ui.lineEditDvipdfm->setText(dvipdf_command);
confDlg->ui.lineEditPs2pdf->setText(ps2pdf_command);
confDlg->ui.lineEditBibtex->setText(bibtex_command);
confDlg->ui.lineEditMakeindex->setText(makeindex_command);
confDlg->ui.lineEditPdfviewer->setText(viewpdf_command);
if (builtinpdfview) confDlg->ui.radioButtonInternalPdfViewer->setChecked(true);
else confDlg->ui.radioButtonExternalPdfViewer->setChecked(true);
confDlg->ui.checkBoxInternalPdfViewEmbed->setChecked(embedinternalpdf);
confDlg->ui.lineEditMetapost->setText(metapost_command);
confDlg->ui.lineEditGhostscript->setText(ghostscript_command);
confDlg->ui.lineEditAsymptote->setText(asymptote_command);
confDlg->ui.lineEditLatexmk->setText(latexmk_command);
confDlg->ui.lineEditSweave->setText(sweave_command);
if (singleviewerinstance) confDlg->ui.checkBoxSingleInstanceViewer->setChecked(true);
confDlg->ui.checkBoxTempBuild->setChecked(useoutputdir);
confDlg->ui.checkBoxClean->setChecked(clean_exit);

confDlg->ui.comboBoxFont->lineEdit()->setText(EditorFont.family() );
confDlg->ui.comboBoxEncoding->setCurrentIndex(confDlg->ui.comboBoxEncoding->findText(input_encoding, Qt::MatchExactly));

confDlg->ui.spinBoxSize->setValue(EditorFont.pointSize() );
if (wordwrap) {confDlg->ui.checkBoxWordwrap->setChecked(true);}
else {confDlg->ui.checkBoxWordwrap->setChecked(false);}
if (tabspaces) {confDlg->ui.checkBoxTab->setChecked(true);}
else {confDlg->ui.checkBoxTab->setChecked(false);}
confDlg->ui.spinBoxTab->setValue(tabwidth);
if (watchfiles) {confDlg->ui.checkBoxWatcher->setChecked(true);}
else {confDlg->ui.checkBoxWatcher->setChecked(false);}
if (autosave) {confDlg->ui.checkBoxAutoSave->setChecked(true);}
else {confDlg->ui.checkBoxAutoSave->setChecked(false);}
if (showline) {confDlg->ui.checkBoxLinenumber->setChecked(true);}
else {confDlg->ui.checkBoxLinenumber->setChecked(false);}
if (completion) {confDlg->ui.checkBoxCompletion->setChecked(true);}
else {confDlg->ui.checkBoxCompletion->setChecked(false);}
if (inlinespellcheck) {confDlg->ui.checkBoxInlineSpell->setChecked(true);}
else {confDlg->ui.checkBoxInlineSpell->setChecked(false);}

confDlg->ui.lineEditAspellCommand->setText(spell_dic);

if (quickmode==1) {confDlg->ui.radioButton1->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==2) {confDlg->ui.radioButton2->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==3) {confDlg->ui.radioButton3->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==4)  {confDlg->ui.radioButton4->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==5)  {confDlg->ui.radioButton5->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==6)  {confDlg->ui.radioButton6->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(true);confDlg->ui.pushButtonWizard->setEnabled(true);}
if (quickmode==7)  {confDlg->ui.radioButton7->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==8)  {confDlg->ui.radioButton8->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==9)  {confDlg->ui.radioButton9->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==10)  {confDlg->ui.radioButton10->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==11)  {confDlg->ui.radioButton11->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==12)  {confDlg->ui.radioButton12->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==13)  {confDlg->ui.radioButton13->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
if (quickmode==14)  {confDlg->ui.radioButton14->setChecked(true); confDlg->ui.lineEditUserquick->setEnabled(false);confDlg->ui.pushButtonWizard->setEnabled(false);}
confDlg->ui.lineEditUserquick->setText(userquick_command);
confDlg->ui.lineEditAsyQuick->setText(quick_asy_command);
confDlg->ui.lineEditPrinter->setText(lp_options);

int row=0;
KeysMap::Iterator its, iter;
QString d,f;
for( its = shortcuts.begin(); its != shortcuts.end(); ++its )
	{
	d=its.key().section("/",0,0);
	for( iter = actionstext.begin(); iter != actionstext.end(); ++iter )
		{
		f=iter.key().section("/",0,0);
		if (d==f)
			{
			QTableWidgetItem *newItem = new QTableWidgetItem(iter.value());
			//QTableWidgetItem *newItem = new QTableWidgetItem(*actionstext.find(its.key()));
			newItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
			newItem->setData(Qt::UserRole, its.key());
			confDlg->ui.shorttableWidget->setRowCount(row+1);
			confDlg->ui.shorttableWidget->setItem(row, 0, newItem);
			QTableWidgetItem *newItembis = new QTableWidgetItem(its.value());
			//QTableWidgetItem *newItem = new QTableWidgetItem(*actionstext.find(its.key()));
			newItembis->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
			newItembis->setData(Qt::UserRole,QString("key"));
			confDlg->ui.shorttableWidget->setItem(row, 1,newItembis);
			row++;
			}
		}
	}
confDlg->ui.shorttableWidget->horizontalHeader()->resizeSection( 0, 250 );
confDlg->ui.shorttableWidget->verticalHeader()->hide();
confDlg->ui.pushButtonToggleFocus->setText(keyToggleFocus);

QTableWidgetItem *colorItem;

colorItem= new QTableWidgetItem(colorBackground.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(0,1,colorItem);

colorItem= new QTableWidgetItem(colorLine.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(1,1,colorItem);

colorItem= new QTableWidgetItem(colorHighlight.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(2,1,colorItem);

colorItem= new QTableWidgetItem(colorStandard.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(3,1,colorItem);

colorItem= new QTableWidgetItem(colorComment.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(4,1,colorItem);

colorItem= new QTableWidgetItem(colorMath.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(5,1,colorItem);

colorItem= new QTableWidgetItem(colorCommand.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(6,1,colorItem);

colorItem= new QTableWidgetItem(colorKeyword.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(7,1,colorItem);

colorItem= new QTableWidgetItem(colorVerbatim.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(8,1,colorItem);

colorItem= new QTableWidgetItem(colorTodo.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(9,1,colorItem);

colorItem= new QTableWidgetItem(colorKeywordGraphic.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(10,1,colorItem);


colorItem= new QTableWidgetItem(colorNumberGraphic.name());
colorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
confDlg->ui.colortableWidget->setItem(11,1,colorItem);

disconnect(autosaveTimer, SIGNAL(timeout()), this, SLOT(fileBackupAll()));
autosaveTimer->stop();
if (confDlg->exec())
	{
	listViewerCommands.clear();
	for(int row=0; row<confDlg->ui.shorttableWidget->rowCount(); row++ )
	{
		QString itemtext = confDlg->ui.shorttableWidget->item(row, 0)->text();
		QString itemshortcut = confDlg->ui.shorttableWidget->item(row, 1)->text();
		QString itemdata=confDlg->ui.shorttableWidget->item(row, 0)->data(Qt::UserRole).toString();
		shortcuts.remove(itemdata);
		shortcuts.insert(itemdata,itemshortcut);
	}
	ModifyShortcuts();
	keyToggleFocus=confDlg->ui.pushButtonToggleFocus->text();

	if (confDlg->ui.radioButton1->isChecked()) quickmode=1;
	if (confDlg->ui.radioButton2->isChecked()) quickmode=2;
	if (confDlg->ui.radioButton3->isChecked()) quickmode=3;
	if (confDlg->ui.radioButton4->isChecked()) quickmode=4;
	if (confDlg->ui.radioButton5->isChecked()) quickmode=5;
	if (confDlg->ui.radioButton6->isChecked()) quickmode=6;
	if (confDlg->ui.radioButton7->isChecked()) quickmode=7;
	if (confDlg->ui.radioButton8->isChecked()) quickmode=8;
	if (confDlg->ui.radioButton9->isChecked()) quickmode=9;
	if (confDlg->ui.radioButton10->isChecked()) quickmode=10;
	if (confDlg->ui.radioButton11->isChecked()) quickmode=11;
	if (confDlg->ui.radioButton12->isChecked()) quickmode=12;
	if (confDlg->ui.radioButton13->isChecked()) quickmode=13;
	if (confDlg->ui.radioButton14->isChecked()) quickmode=14;
	userquick_command=confDlg->ui.lineEditUserquick->text();
	quick_asy_command=confDlg->ui.lineEditAsyQuick->text();
	lp_options=confDlg->ui.lineEditPrinter->text();
	
	svnEnable=confDlg->ui.checkBoxSvn->isChecked();
	svnPath=confDlg->ui.lineEditSvn->text();
	
	lualatex_command=confDlg->ui.lineEditLualatex->text();
	xelatex_command=confDlg->ui.lineEditXelatex->text();
	extra_path=confDlg->ui.lineEditPath->text();
	latex_command=confDlg->ui.lineEditLatex->text();
	pdflatex_command=confDlg->ui.lineEditPdflatex->text();
	dvips_command=confDlg->ui.lineEditDvips->text();
	viewdvi_command=confDlg->ui.lineEditDviviewer->text();
	viewps_command=confDlg->ui.lineEditPsviewer->text();
	dvipdf_command=confDlg->ui.lineEditDvipdfm->text();
	ps2pdf_command=confDlg->ui.lineEditPs2pdf->text();
	bibtex_command=confDlg->ui.lineEditBibtex->text();
	makeindex_command=confDlg->ui.lineEditMakeindex->text();
	viewpdf_command=confDlg->ui.lineEditPdfviewer->text();
	metapost_command=confDlg->ui.lineEditMetapost->text();
	if (metapost_command.right(1)!=" ") metapost_command+=" ";
	ghostscript_command=confDlg->ui.lineEditGhostscript->text();
	asymptote_command=confDlg->ui.lineEditAsymptote->text();
	latexmk_command=confDlg->ui.lineEditLatexmk->text();
	sweave_command=confDlg->ui.lineEditSweave->text();
	builtinpdfview=confDlg->ui.radioButtonInternalPdfViewer->isChecked();
	embedinternalpdf=confDlg->ui.checkBoxInternalPdfViewEmbed->isChecked();
	
	if (embedinternalpdf && builtinpdfview) 
	  {
	  StackedViewers->show();
	  ViewPdfPanelAct->setEnabled(true);
	  togglePdfButton->show();
	  ShowPdfView(false);
	  ShowSourceView(false);
	  if (showpdfview)
	    {
	    StackedViewers->show();
	    sourceviewerWidget->hide();
	    }
	  else if (showsourceview)
	    {
	    StackedViewers->setCurrentWidget(sourceviewerWidget);
	    StackedViewers->show();
	    }
	  else StackedViewers->hide();
	  }
	else 
	  {
	  if (pdfviewerWidget) {StackedViewers->removeWidget(pdfviewerWidget);delete(pdfviewerWidget);}
	  //StackedViewers->hide();
	  ViewPdfPanelAct->setEnabled(false);
	  togglePdfButton->hide();
	  if (showsourceview)
	    {
	    StackedViewers->setCurrentWidget(sourceviewerWidget);
	    StackedViewers->show();
	    }
	  else StackedViewers->hide();
	  }
	singleviewerinstance=confDlg->ui.checkBoxSingleInstanceViewer->isChecked();
	useoutputdir=confDlg->ui.checkBoxTempBuild->isChecked();
	clean_exit=confDlg->ui.checkBoxClean->isChecked();
	
	if ((pdfviewerWidget) && keyToggleFocus!="none") pdfviewerWidget->setKeyEditorFocus(QKeySequence(keyToggleFocus));
	if (pdfviewerWidget)  pdfviewerWidget->setGSCommand(ghostscript_command);
	if ((pdfviewerWindow) && keyToggleFocus!="none") pdfviewerWindow->setKeyEditorFocus(QKeySequence(keyToggleFocus));
	if (pdfviewerWindow)  pdfviewerWindow->setGSCommand(ghostscript_command);
	
	QString fam=confDlg->ui.comboBoxFont->lineEdit()->text();
	int si=confDlg->ui.spinBoxSize->value();
	QFont F(fam,si);
	EditorFont=F;
	
	input_encoding=confDlg->ui.comboBoxEncoding->currentText();
	stat3->setText(QString(" %1 ").arg(input_encoding));
	
	wordwrap=confDlg->ui.checkBoxWordwrap->isChecked();
	tabspaces=confDlg->ui.checkBoxTab->isChecked();
	tabwidth=confDlg->ui.spinBoxTab->value();
	watchfiles=confDlg->ui.checkBoxWatcher->isChecked();
	autosave=confDlg->ui.checkBoxAutoSave->isChecked();
	
	completion=confDlg->ui.checkBoxCompletion->isChecked();
	showline=confDlg->ui.checkBoxLinenumber->isChecked();
	inlinespellcheck=confDlg->ui.checkBoxInlineSpell->isChecked();
	spell_dic=confDlg->ui.lineEditAspellCommand->text();
	if (spellChecker) delete spellChecker;
	if (spelldicExist()) 
	      {
	      QString dic=spell_dic.left(spell_dic.length()-4);
	      spellChecker = new Hunspell(dic.toLatin1()+".aff",dic.toLatin1()+".dic");
	      }
	else spellChecker=0;
	QFileInfo fispell(spell_dic);
	pdfCheckerLang=fispell.fileName().left(2);
	if (!pdfCheckerLang.contains(QRegExp("(de|en|es|fr|id|it|nl|pl|pt|ru)"))) pdfCheckerLang="en";

	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(0,1)->text())) colorBackground=QColor(confDlg->ui.colortableWidget->item(0,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(1,1)->text())) colorLine=QColor(confDlg->ui.colortableWidget->item(1,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(2,1)->text())) colorHighlight=QColor(confDlg->ui.colortableWidget->item(2,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(3,1)->text())) colorStandard=QColor(confDlg->ui.colortableWidget->item(3,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(4,1)->text())) colorComment=QColor(confDlg->ui.colortableWidget->item(4,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(5,1)->text())) colorMath=QColor(confDlg->ui.colortableWidget->item(5,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(6,1)->text())) colorCommand=QColor(confDlg->ui.colortableWidget->item(6,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(7,1)->text())) colorKeyword=QColor(confDlg->ui.colortableWidget->item(7,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(8,1)->text())) colorVerbatim=QColor(confDlg->ui.colortableWidget->item(8,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(9,1)->text())) colorTodo=QColor(confDlg->ui.colortableWidget->item(9,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(10,1)->text())) colorKeywordGraphic=QColor(confDlg->ui.colortableWidget->item(10,1)->text());
	
	if (QColor::isValidColor(confDlg->ui.colortableWidget->item(11,1)->text())) colorNumberGraphic=QColor(confDlg->ui.colortableWidget->item(11,1)->text());

	QTextCodec* codec = QTextCodec::codecForName(input_encoding.toLatin1());
	if(!codec) codec = QTextCodec::codecForLocale();
	
	QString tmpSource =sourceviewerWidget->editor->toPlainText();
	sourceviewerWidget->editor->setEncoding(input_encoding);
	if (wordwrap) {sourceviewerWidget->editor->setWordWrapMode(QTextOption::WordWrap);}
	else {sourceviewerWidget->editor->setWordWrapMode(QTextOption::NoWrap);}
	sourceviewerWidget->changeSettings(EditorFont,showline);
	sourceviewerWidget->editor->setColors(edcolors());
	sourceviewerWidget->editor->highlighter->setColors(hicolors());
	QTextStream tsSource( &tmpSource,QIODevice::ReadOnly );
	tsSource.setCodec(codec);
	sourceviewerWidget->editor->setPlainText( tsSource.readAll() );
	

	if (currentEditorView())
		{
		LatexEditorView *temp = new LatexEditorView( EditorView,EditorFont,svnEnable,showline,edcolors(),hicolors(),inlinespellcheck,spell_ignored_words,spellChecker,tabspaces,tabwidth,QKeySequence(keyToggleFocus),getName(),userTagsList);
		temp=currentEditorView();
		FilesMap::Iterator it;
		initCompleter();
		for( it = filenames.begin(); it != filenames.end(); ++it )
			{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			EditorView->setCurrentIndex(EditorView->indexOf(it.key()));
			bool  MODIFIED =currentEditorView()->editor->document()->isModified();
			QString tmp =currentEditorView()->editor->toPlainText();
			disconnect(currentEditorView()->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
			disconnect(currentEditorView()->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
			disconnect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
			disconnect(currentEditorView()->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
			disconnect(currentEditorView()->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
			disconnect(currentEditorView()->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
			disconnect(currentEditorView()->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
			disconnect(currentEditorView()->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));
			//currentEditorView()->editor->clear();
			currentEditorView()->editor->setSpellChecker(spellChecker);
			currentEditorView()->editor->highlighter->setSpellChecker(spellChecker);
			currentEditorView()->editor->activateInlineSpell(inlinespellcheck);
			currentEditorView()->editor->highlighter->activateInlineSpell(inlinespellcheck);
			if (wordwrap) {currentEditorView()->editor->setWordWrapMode(QTextOption::WordWrap);}
			else {currentEditorView()->editor->setWordWrapMode(QTextOption::NoWrap);}
			if (completion) currentEditorView()->editor->setCompleter(completer);
			else currentEditorView()->editor->setCompleter(0);
			currentEditorView()->editor->setTabSettings(tabspaces,tabwidth);
			currentEditorView()->editor->setKeyViewerFocus(QKeySequence(keyToggleFocus));
			currentEditorView()->changeSettings(EditorFont,svnEnable,showline);
			currentEditorView()->editor->highlighter->setColors(hicolors());
			currentEditorView()->editor->setColors(edcolors());

			QTextStream ts( &tmp,QIODevice::ReadOnly );
			ts.setCodec(codec);
			currentEditorView()->editor->setPlainText( ts.readAll() );
			currentEditorView()->editor->setLastSavedTime(QDateTime::currentDateTime());
			if( MODIFIED ) currentEditorView()->editor->document()->setModified(true );
			else currentEditorView()->editor->document()->setModified( false );
			connect(currentEditorView()->editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(NewDocumentStatus(bool)));
			connect(currentEditorView()->editor, SIGNAL(spellme()), this, SLOT(editSpell()));
			connect(currentEditorView()->editor, SIGNAL(tooltiptab()), this, SLOT(editTipTab()));
			connect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
			connect(currentEditorView()->editor, SIGNAL(requestpdf(int)),this, SLOT(jumpToPdfline(int)));
			connect(currentEditorView()->editor->document(), SIGNAL(undoAvailable(bool)),UndoAct, SLOT(setEnabled(bool)));
			connect(currentEditorView()->editor->document(), SIGNAL(redoAvailable(bool)),RedoAct, SLOT(setEnabled(bool)));
			connect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CutAct, SLOT(setEnabled(bool)));
			connect(currentEditorView()->editor, SIGNAL(copyAvailable(bool)), CopyAct, SLOT(setEnabled(bool)));
			currentEditorView()->editor->setLastNumLines(currentEditorView()->editor->numoflines());
			connect(currentEditorView()->editor, SIGNAL(numLinesChanged(int)), this, SLOT(refreshAllFromCursor(int)));
			connect(currentEditorView()->editor, SIGNAL(requestGotoStructure(int)),this, SLOT(jumpToStructure(int)));
			connect(currentEditorView()->editor, SIGNAL(poshaschanged(int,int)),this, SLOT(showCursorPos(int,int)));
			//UpdateStructure();
			//UpdateBibliography();
			QApplication::restoreOverrideCursor();
			}
		EditorView->setCurrentIndex(EditorView->indexOf(temp));
		UpdateCaption();
		UpdateStructure();
		UpdateBibliography();
		OutputTextEdit->clear();
		OutputTableWidget->hide();
		OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
		separatorline->hide();
		//OutputTextEdit->insertLine("Editor settings apply only to new loaded document.");
		currentEditorView()->editor->setFocus();
		}
	}
if (autosave)
    {
    connect(autosaveTimer, SIGNAL(timeout()), this, SLOT(fileBackupAll()));
    autosaveTimer->start(600000);
    }
}

void Texmaker::ToggleMode()
{
//QAction *action = qobject_cast<QAction *>(sender());
if (!singlemode)
	{
	ToggleAct->setText(tr("Define Current Document as 'Master Document'"));
	OutputTextEdit->clear();
	OutputTableWidget->hide();
	OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
	separatorline->hide();
	logpresent=false;
	singlemode=true;
	listbibfiles.clear();
	listchildfiles.clear();
	stat1->setText(QString(" %1 ").arg(tr("Normal Mode")));
	ToggleDocAct->setEnabled(false);
	return;
	}
if (singlemode && currentEditorView())  
	{
	MasterName=getName();
	if (MasterName.startsWith("untitled") || MasterName=="")
		{
		QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\nno valid name for the master document");
		return;
		}
	QString shortName = MasterName;
	int pos;
	while ( (pos = (int)shortName.indexOf('/')) != -1 ) shortName.remove(0,pos+1);
	ToggleAct->setText(tr("Normal Mode (current master document :")+shortName+")");
	singlemode=false;
	stat1->setText(QString(" %1 ").arg(tr("Master Document :")+shortName));
	ToggleDocAct->setEnabled(true);
	UpdateStructure();
	UpdateBibliography();
	return;
	}
}

void Texmaker::onOtherInstanceMessage(const QString &msg)  // Added slot for messages to the single instance
{
QStringList argv = msg.split("#!#");
int argc = argv.size();
for ( int i = 1; i < argc; ++i )
	{
	QString arg = argv[ i ];
	if ( arg[0] != '-' )    load( arg );
	if ( arg == "-master" ) ToggleMode();
	if (( arg == "-line" ) && (i<argc-1))  setLine( argv[ ++i ] );
	if ( arg == "-insert" ) insertFromCommandLine( argv[ ++i ] );
	}
//A bad (but applicable) trick for activating Texmaker MainWindow //add by S. R. Alavizadeh
//setWindowState(Qt::WindowMinimized);
//setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
//show();
}
////////////////// VIEW ////////////////

void Texmaker::gotoNextDocument()
{
if ( EditorView->count() < 2 ) return;
int cPage = EditorView->currentIndex() + 1;
if ( cPage >= EditorView->count() ) EditorView->setCurrentIndex( 0 );
else EditorView->setCurrentIndex( cPage );
}

void Texmaker::gotoPrevDocument()
{
if ( EditorView->count() < 2 ) return;
int cPage = EditorView->currentIndex() - 1;
if ( cPage < 0 ) EditorView->setCurrentIndex( EditorView->count() - 1 );
else EditorView->setCurrentIndex( cPage );
}

void Texmaker::SetInterfaceFont()
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
X11FontDialog *xfdlg = new X11FontDialog(this);
int ft=xfdlg->ui.comboBoxFont->findText (x11fontfamily , Qt::MatchExactly);
xfdlg->ui.comboBoxFont->setCurrentIndex(ft);
xfdlg->ui.spinBoxSize->setValue(x11fontsize);
if (xfdlg->exec())
	{
	x11fontfamily=xfdlg->ui.comboBoxFont->currentText();
	x11fontsize=xfdlg->ui.spinBoxSize->value();
	QFont x11Font (x11fontfamily,x11fontsize);
	QApplication::setFont(x11Font);
	}
#endif
}

void Texmaker::gotoBookmark1()
{
if ( !currentEditorView() ) return;
int l=currentEditorView()->editor->UserBookmark[0];
if (l>0) currentEditorView()->editor->gotoLine(l-1);
}

void Texmaker::gotoBookmark2()
{
if ( !currentEditorView() ) return;
int l=currentEditorView()->editor->UserBookmark[1];
if (l>0) currentEditorView()->editor->gotoLine(l-1);
}

void Texmaker::gotoBookmark3()
{
if ( !currentEditorView() ) return;
int l=currentEditorView()->editor->UserBookmark[2];
if (l>0) currentEditorView()->editor->gotoLine(l-1);
}

//*********************************
void Texmaker::dragEnterEvent(QDragEnterEvent *event)
{
if (event->mimeData()->hasFormat("text/uri-list")) event->acceptProposedAction();
}

void Texmaker::dropEvent(QDropEvent *event)
{
#if defined(Q_OS_WIN32)
QRegExp rx("file:(/+)(.*\\.(?:tex|bib|sty|cls|mp|asy|Rnw|tks))");
#else
QRegExp rx("file:(//)(.*\\.(?:tex|bib|sty|cls|mp|asy|Rnw|tks))");
#endif
QList<QUrl> uris=event->mimeData()->urls();
QString uri;
for (int i = 0; i < uris.size(); ++i)
	{
	uri=uris.at(i).toString();
	if (rx.exactMatch(uri)) {load(rx.cap(2));}
	}
event->acceptProposedAction();
}

//***********************************
void Texmaker::SetMostUsedSymbols()
{
for ( int i = 0; i <=11; ++i ) symbolMostused[i]=-1;
QList<int> list_num, list_score;
list_num.clear();
list_score.clear();
for (int i=0; i <412 ; i++)
	{
	list_num.append(i);
	list_score.append(symbolScore[i]);
	}
int max;
for (int i = 0; i < 412; i++)
	{
	max=i;
	for (int j = i+1; j < 412; j++) 
		{
		if (list_score.at(j)>list_score.at(max)) max=j;
		}
	if (max!=i) 
		{
		list_num.swap(i,max);
		list_score.swap(i,max);
		}
	}
for ( int i = 0; i <=11; ++i ) 
	{
	if (list_score.at(i)>0) symbolMostused[i]=list_num.at(i);
	}
MostUsedListWidget->SetUserPage(symbolMostused);
}

void Texmaker::InsertFavoriteSymbols()
{
QString actData;
QAction *action = qobject_cast<QAction *>(sender());
if (action)
	{
	actData=action->data().toString();
	QRegExp rxnumber(";([0-9]+)");
	int number=-1;
	if (!actData.isEmpty())
		{
		if ( rxnumber.indexIn(actData) != -1) number=rxnumber.cap(1).toInt();
		if (!favoriteSymbolList.contains(number)) favoriteSymbolList.append(number);
		FavoriteListWidget->SetFavoritePage(favoriteSymbolList);
		}
	}
}

void Texmaker::RemoveFavoriteSymbols()
{
QString actData;
QAction *action = qobject_cast<QAction *>(sender());
if (action)
	{
	actData=action->data().toString();
	QRegExp rxnumber(";([0-9]+)");
	int number=-1;
	if (!actData.isEmpty())
		{
		if ( rxnumber.indexIn(actData) != -1) number=rxnumber.cap(1).toInt();
		if (favoriteSymbolList.contains(number)) favoriteSymbolList.removeOne(number);
		FavoriteListWidget->SetFavoritePage(favoriteSymbolList);
		}
	}
}

void Texmaker::RemoveUserTag()
{
QString actData;
QAction *action = qobject_cast<QAction *>(sender());
if (action)
	{
	actData=action->data().toString();
	userTagsList.removeOne(actData);
	usertagsListWidget->updateList(userTagsList);
	if (currentEditorView())
		{
		FilesMap::Iterator it;
		for( it = filenames.begin(); it != filenames.end(); ++it )
			{
			it.key()->editor->setUserTagsList(userTagsList);
			}
		}
	}

}

void Texmaker::AddUserTag()
{
AddTagDialog *atDlg = new AddTagDialog(this);
QString item,code;
QString trigger="";
if ( atDlg->exec() )
    {
    item=atDlg->ui.itemEdit->text();
    code=atDlg->ui.tagEdit->toPlainText();
    trigger=atDlg->ui.triggerEdit->text();
    if (!item.isEmpty() && !code.isEmpty())
	{
	item.remove("#");
	code.remove("#");
	trigger.remove("#");
	trigger.remove(":");
	userTagsList.append(item+"#"+code+"#"+trigger);
	usertagsListWidget->updateList(userTagsList);
	if (currentEditorView())
	    {
	    FilesMap::Iterator it;
	    for( it = filenames.begin(); it != filenames.end(); ++it )
		    {
		    it.key()->editor->setUserTagsList(userTagsList);
		    }
	    }
    
	}
    }
}

void Texmaker::UpdateUserTag()
{
userTagsList.clear();
QString data, item, code, trigger;
QStringList tagList;
for ( int i = 0; i <usertagsListWidget->count(); ++i ) 
    {
    tagList.clear();
    data=usertagsListWidget->item(i)->text()+"#"+usertagsListWidget->item(i)->data(Qt::UserRole).toString();
    tagList=data.split("#");
    item="";
    code="";
    trigger="";
    if (tagList.count()>=2)
	    {
	    item=tagList.at(0);
	    code=tagList.at(1);
	    if (tagList.count()==3) trigger=tagList.at(2);
	    if (!item.isEmpty() && !code.isEmpty())
		{
		item.remove("#");
		code.remove("#");
		trigger.remove("#");
		trigger.remove(":");
		userTagsList.append(item+"#"+code+"#"+trigger);
		}
	    }
    }
usertagsListWidget->updateList(userTagsList);
if (currentEditorView())
    {
    FilesMap::Iterator it;
    for( it = filenames.begin(); it != filenames.end(); ++it )
	    {
	    it.key()->editor->setUserTagsList(userTagsList);
	    }
    }
}


void Texmaker::ChangeUserTag()
{
QString actData;
QAction *action = qobject_cast<QAction *>(sender());
int index=-1;
QString item="";
QString code="";
QString trigger="";
QString newitem,newcode;
QString newtrigger="";
QStringList tagList;
if (action)
	{
	actData=action->data().toString();
	index=userTagsList.indexOf(actData,0);
	if (index>-1)
	    {  
	    tagList= userTagsList.at(index).split("#");
	    if (tagList.count()>=2)
		{
		item=tagList.at(0);
		code=tagList.at(1);
		if (tagList.count()==3) trigger=tagList.at(2);
		AddTagDialog *atDlg = new AddTagDialog(this);
		atDlg->ui.itemEdit->setText(item);
		atDlg->ui.tagEdit->setPlainText(code);
		atDlg->ui.triggerEdit->setText(trigger);
		if ( atDlg->exec() )
		    {
		    item=atDlg->ui.itemEdit->text();
		    code=atDlg->ui.tagEdit->toPlainText();
		    trigger=atDlg->ui.triggerEdit->text();
		    if (!item.isEmpty() && !code.isEmpty())
			{
			item.remove("#");
			code.remove("#");
			trigger.remove("#");
			trigger.remove(":");
			userTagsList.replace(index,item+"#"+code+"#"+trigger);
			usertagsListWidget->updateList(userTagsList);
			if (currentEditorView())
			    {
			    FilesMap::Iterator it;
			    for( it = filenames.begin(); it != filenames.end(); ++it )
				    {
				    it.key()->editor->setUserTagsList(userTagsList);
				    }
			    }
		    
			}
		    }
		}
	    }
	}

}

void Texmaker::ModifyShortcuts()
{
KeysMap::Iterator its;
QString d,f,s;
QList<QAction *> listaction;
listaction << toolMenu->actions();
listaction << editMenu->actions();
listaction << NextDocAct << PrevDocAct;
listaction << latex1Menu->actions();
listaction << latex11Menu->actions();
listaction << latex12Menu->actions();
listaction << latex13Menu->actions();
listaction << latex14Menu->actions();
listaction << latex15Menu->actions();
listaction << latex16Menu->actions();
listaction << latex17Menu->actions();
listaction << latex18Menu->actions();
listaction << math1Menu->actions();
listaction << math11Menu->actions();
listaction << math12Menu->actions();
listaction << math13Menu->actions();
listaction << math14Menu->actions();
QListIterator<QAction*> iterator(listaction);
actionstext.clear();
while ( iterator.hasNext() )
	{
	QAction *action=iterator.next();
        if (action && (!action->menu())  && (!action->data().toString().isEmpty())) 
		{
		actionstext.insert(action->data().toString(),action->text());
		d=action->data().toString().section("/",0,0);
		//action->setShortcut(QKeySequence(""));
		for( its = shortcuts.begin(); its != shortcuts.end(); ++its )
			{
			f=its.key().section("/",0,0);
			s=its.value();
			if (f==d) action->setShortcut(QKeySequence(""));
			if (f==d && s!="none" && !s.isEmpty()) 
			{ action->setShortcut(QKeySequence(s));}
			}
// 		its=shortcuts.find(action->data().toString());
// 		if (its!=shortcuts.end()) 
// 			{
// 			s=*its;
// 			if (s!="none" && !s.isEmpty()) action->setShortcut(s);
// 			}
		//QString s=*shortcuts.find(action->data().toString());
		//if (s!="none" && !s.isEmpty()) action->setShortcut(s);
		}
	}
}

void Texmaker::initCompleter()
{
QTextCodec *codec = QTextCodec::codecForName("UTF-8");
QAbstractItemModel *model;
QFile tagsfile(":/completion/completion.txt");

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifdef USB_VERSION
QFile userTagsfile(QCoreApplication::applicationDirPath()+"/completion.txt");
#else
QFile userTagsfile(PREFIX"/share/texmaker/completion.txt");
#endif

#endif
#if defined(Q_OS_MAC)
QFile userTagsfile(QCoreApplication::applicationDirPath() + "/../Resources/completion.txt");
#endif
#if defined(Q_OS_WIN32)
QFile userTagsfile(QCoreApplication::applicationDirPath()+"/completion.txt");
#endif
if (!tagsfile.open(QFile::ReadOnly)) model=new QStringListModel(completer);

QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
QStringList words;
QString line;
QTextStream tscompleter(&tagsfile);
tscompleter.setCodec(codec);
while (!tscompleter.atEnd()) 
	{
	line = tscompleter.readLine();
	if (!line.isEmpty()) words.append(line.remove("\n"));
	}
	
if (userTagsfile.open(QFile::ReadOnly))
    {
    QTextStream usertscompleter(&userTagsfile);
    usertscompleter.setCodec(codec);
    while (!usertscompleter.atEnd()) 
	    {
	    line = usertscompleter.readLine();
	    if (!line.isEmpty()) words.append(line.remove("\n"));
	    }
    }
for (int i=0; i<userCompletionList.count();++i) 
	{
	words.append(userCompletionList.at(i));
	}
words.removeDuplicates();
words.sort();

QApplication::restoreOverrideCursor();
model=new QStringListModel(words, completer);
completer->setModel(model);
completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
completer->setCaseSensitivity(Qt::CaseSensitive);
completer->setWrapAround(false);
}

void Texmaker::updateCompleter()
{
QStringList words;
QString item;
QStringList types,extraBibList,extraLabelList;
types << QLatin1String("cite") << QLatin1String("nocite") << QLatin1String("footcite")
  << QLatin1String("citep") << QLatin1String("ref")
  << QLatin1String("pageref") << QLatin1String("eqref")
  << QLatin1String("autoref");
QRegExp macroName("\\\\("+types.join("|")+")\\{(.*)\\}", Qt::CaseInsensitive);
macroName.setMinimal(true);
for (int i=0; i<completer->model()->rowCount();++i) 
  {
  item=completer->model()->data(completer->model()->index(i,0)).toString();
  if (!item.contains(macroName)) {words.append(item);}
  }

words.append("\\cite{"+QString(0x2022)+"}");
words.append("\\nocite{"+QString(0x2022)+"}");
words.append("\\footcite{"+QString(0x2022)+"}");
words.append("\\citep{"+QString(0x2022)+"}");
words.append("\\ref{"+QString(0x2022)+"}");
words.append("\\pageref{"+QString(0x2022)+"}");
words.append("\\eqref{"+QString(0x2022)+"}");
words.append("\\autoref{"+QString(0x2022)+"}");
  
for (int i=0; i<userCompletionList.count();++i) 
	{
	if (userCompletionList.at(i).contains("#bib#")) extraBibList.append(userCompletionList.at(i));
	if (userCompletionList.at(i).contains("#label#")) extraLabelList.append(userCompletionList.at(i));
	}

QAbstractItemModel *model;
QString tag;
for (int i=0; i<bibitem.count();++i) 
	{
	words.append("\\nocite{"+bibitem.at(i)+"}");
	words.append("\\cite{"+bibitem.at(i)+"}");
	words.append("\\footcite{"+bibitem.at(i)+"}");
	words.append("\\citep{"+bibitem.at(i)+"}");
	for (int j=0; j<extraBibList.count();++j) 
	  {
	  tag=extraBibList.at(j);
	  tag.replace("#bib#",bibitem.at(i));
	  words.append(tag);
	  }
	}
for (int i=0; i<labelitem.count();++i) 
	{
	words.append("\\ref{"+labelitem.at(i)+"}");
	words.append("\\pageref{"+labelitem.at(i)+"}");
	words.append("\\eqref{"+labelitem.at(i)+"}");
	words.append("\\autoref{"+labelitem.at(i)+"}");
	for (int j=0; j<extraLabelList.count();++j) 
	  {
	  tag=extraLabelList.at(j);
	  tag.replace("#label#",labelitem.at(i));
	  words.append(tag);
	  }
	}
for (int j=0; j<extraBibList.count();++j) 
  {
  tag=extraBibList.at(j);
  tag.replace("#bib#",QString(0x2022));
  words.removeOne(extraBibList.at(j));
  words.append(tag);
  }
for (int j=0; j<extraLabelList.count();++j) 
  {
  tag=extraLabelList.at(j);
  tag.replace("#label#",QString(0x2022));
  words.removeOne(extraLabelList.at(j));
  words.append(tag);
  }
words.removeDuplicates();
words.sort();

model=new QStringListModel(words, completer);
completer->setModel(model);
completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
completer->setCaseSensitivity(Qt::CaseSensitive);
completer->setWrapAround(false);
}

bool Texmaker::spelldicExist()
{
QString affdic=spell_dic.left(spell_dic.length()-4)+".aff";
QFileInfo fidic(spell_dic);
QFileInfo fiaff(affdic);
return (fidic.exists() && fidic.isReadable() && fiaff.exists() && fiaff.isReadable());
}

void Texmaker::updateTranslation()
{
QAction *action = qobject_cast<QAction *>(sender());
QString lang=action->text();
TexmakerApp::instance()->language=lang;
QMessageBox::information( this,"Texmaker",tr("The language setting will take effect after restarting the application."));
}

// void Texmaker::updateAppearance()
// {
// QAction *action = qobject_cast<QAction *>(sender());
// QString style=action->text();
// modern_style=(action->text()=="Modern");
// QMessageBox::information( this,"Texmaker",tr("The appearance setting will take effect after restarting the application."));
// }

void Texmaker::disableToolsActions()
{
QList<QAction *> listaction;
listaction << toolMenu->actions();
listaction << user12Menu->actions();
//listaction << optionsMenu->actions();
listaction << runToolBar->actions();
QListIterator<QAction*> iterator(listaction);
while ( iterator.hasNext() )
	{
	QAction *action=iterator.next();
	action->setEnabled(false);
	}
QTimer::singleShot(5000,this, SLOT(enableToolsActions()));
}

void Texmaker::enableToolsActions()
{
QList<QAction *> listaction;
listaction << toolMenu->actions();
listaction << user12Menu->actions();
//listaction << optionsMenu->actions();
listaction << runToolBar->actions();
QListIterator<QAction*> iterator(listaction);
while ( iterator.hasNext() )
	{
	QAction *action=iterator.next();
	action->setEnabled(true);
	}
}

void Texmaker::clipboardDataChanged()
{
if ( !currentEditorView() ) return;
PasteAct->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void Texmaker::refreshAll()
{
if ( !currentEditorView() ) return;
disconnect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
currentEditorView()->editor->highlighter->rehighlight();
UpdateStructure();
connect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
}

void Texmaker::refreshAllFromCursor(int newnumlines)
{
disconnect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
int oldnumlines=currentEditorView()->editor->getLastNumLines();
int delta=newnumlines-oldnumlines;
QTextBlock p = currentEditorView()->editor->textCursor().block();
int currentline=p.blockNumber();
int i=currentline-1;
QList<int> start,end;
/* QMapIterator<int, int> it(currentEditorView()->editor->foldedLines);
 while (it.hasNext()) {
     it.next();
     qDebug() << "avant folded" << it.key() << ":" << it.value();
 }*/
while (p.isValid())
  {
  if (currentEditorView()->editor->foldedLines.keys().contains(i))
    {
    start.append(i+delta);
    end.append(currentEditorView()->editor->foldedLines[i]+delta);
    currentEditorView()->editor->foldedLines.remove(i);
    }
  i++;
  p = p.next();
  }
for (int i =0; i <start.count(); i++) currentEditorView()->editor->foldedLines.insert(start[i],end[i]);
/* QMapIterator<int, int> itbis(currentEditorView()->editor->foldedLines);
 while (itbis.hasNext()) {
     itbis.next();
     qDebug() << "apres folded" << itbis.key() << ":" << itbis.value();
 }
qDebug() << "**********";*/
/*QList<int> listofmodifiedlines;
const QList<StructItem>& structure = currentEditorView()->editor->getStructItems();
for (int j = 0; j < structure.count(); j++)
  {
  l=structure.at(j).line;
  if (l>=currentline) 
    {
    listofmodifiedlines.append(l+delta);
    }
   }
int i=currentline;
while (p.isValid())
  {
  if (listofmodifiedlines.contains(i))
    {
    currentEditorView()->editor->highlighter->rehighlightBlock(p); 
    }
  i++;
  p = p.next();
  }
refreshRange();
//UpdateStructure();*/
connect(currentEditorView()->editor, SIGNAL(requestUpdateStructure()), this, SLOT(UpdateStructure()));
refreshRange();
currentEditorView()->editor->setLastNumLines(newnumlines);
}

void Texmaker::refreshRange()
{
currentEditorView()->editor->foldableLines.clear();
int endpreamble = currentEditorView()->editor->searchLine("\\begin{document}");
if (endpreamble>1) currentEditorView()->editor->foldableLines.insert(0,endpreamble-1);
int nb=StructureTreeWidget->topLevelItemCount();
if (nb>0)
    {
    for (int i = 0; i < nb; i++) 
	{
	ParseTree(StructureTreeWidget->topLevelItem(i));
	}
    }
currentEditorView()->editor->matchAll();
}

void Texmaker::jumpToStructure(int line)
{
const QList<StructItem>& structure = currentEditorView()->editor->getStructItems();
int index=-1;
for (int j = 0; j < structure.count(); j++)
    {
    if (structure.at(j).cursor.block().blockNumber()==line) {index=j; break;}
    } 
if (index<0) return;
QList<QTreeWidgetItem *> fItems=StructureTreeWidget->findItems (structure.at(index).item,Qt::MatchRecursive,0);

if (fItems.size()>0 ) 
  {
  for (int i = 0;  i< fItems.size(); i++)
    {
    if ((fItems.at(i)) && (fItems.at(i)->text(1)==QString::number(index))) 
      {
      StructureTreeWidget->scrollToItem(fItems.at(i),QAbstractItemView::EnsureVisible);
      StructureTreeWidget->setCurrentItem(fItems.at(i));
      }
    }
  }
}

void Texmaker::mainWindowActivated()
{
if (watchfiles) QTimer::singleShot(0, this, SLOT(checkModifiedOutsideAll()));
}


void Texmaker::ToggleStructurePanel()
{
ShowStructView(true);
}

void Texmaker::ToggleLogPanel()
{
ShowOutputView(true);
}

void Texmaker::TogglePdfPanel()
{
ShowPdfView(true);
}

void Texmaker::ToggleSourcePanel()
{
ShowSourceView(true);
}

void Texmaker::ToggleFilesPanel()
{
ShowFilesView(true);
}

void Texmaker::ShowStructView(bool change)
{
int pos=0;
QTextCursor cur;
if (currentEditorView() )
  {
  cur=currentEditorView()->editor->textCursor();
  pos=cur.position();
  }
if (change) showstructview=!showstructview;
if (showstructview)
   {
   splitter3->show();  
   //LeftPanelFrameBis->show();
   }
else
   {
   splitter3->hide(); 
   //LeftPanelFrameBis->hide();
   if (currentEditorView()) 
      {
      cur.setPosition(pos,QTextCursor::MoveAnchor);
      currentEditorView()->editor->setTextCursor(cur);
      currentEditorView()->editor->setFocus();
      }
   }
ViewStructurePanelAct->setChecked(showstructview);
toggleStructureButton->setEnabled(showstructview);
}

void Texmaker::ShowOutputView(bool change)
{
if (change) showoutputview=!showoutputview;
if (showoutputview)
    {
    Outputframe->show();
    splitter2Changed();
    if (currentEditorView()) QTimer::singleShot(10,currentEditorView()->editor, SLOT(setCursorVisible()));
    }
else
   {
   Outputframe->hide();
   }
ViewLogPanelAct->setChecked(showoutputview);
toggleLogButton->setEnabled(showoutputview);
}

void Texmaker::ShowPdfView(bool change)
{
disconnect(ViewPdfPanelAct, SIGNAL(triggered()), this, SLOT(TogglePdfPanel()));
disconnect(togglePdfButton, SIGNAL( clicked() ), this, SLOT(TogglePdfPanel() ) );
disconnect(ViewSourcePanelAct, SIGNAL(triggered()), this, SLOT(ToggleSourcePanel()));
disconnect(toggleSourceButton, SIGNAL( clicked() ), this, SLOT(ToggleSourcePanel() ) );
if (change) showpdfview=!showpdfview;
if (showpdfview)
    {
    sourceviewerWidget->hide();
    if (pdfviewerWidget) StackedViewers->setCurrentWidget(pdfviewerWidget);
    StackedViewers->show();
    showsourceview=false;
    ViewSourcePanelAct->setChecked(showsourceview);
    toggleSourceButton->setEnabled(showsourceview);
    }
else
   {
   StackedViewers->hide();
   }
ViewPdfPanelAct->setChecked(showpdfview);
togglePdfButton->setEnabled(showpdfview);
connect(ViewPdfPanelAct, SIGNAL(triggered()), this, SLOT(TogglePdfPanel()));
connect(togglePdfButton, SIGNAL( clicked() ), this, SLOT(TogglePdfPanel() ) );
connect(ViewSourcePanelAct, SIGNAL(triggered()), this, SLOT(ToggleSourcePanel()));
connect(toggleSourceButton, SIGNAL( clicked() ), this, SLOT(ToggleSourcePanel() ) );
}

void Texmaker::ShowSourceView(bool change)
{
disconnect(ViewPdfPanelAct, SIGNAL(triggered()), this, SLOT(TogglePdfPanel()));
disconnect(togglePdfButton, SIGNAL( clicked() ), this, SLOT(TogglePdfPanel() ) );
disconnect(ViewSourcePanelAct, SIGNAL(triggered()), this, SLOT(ToggleSourcePanel()));
disconnect(toggleSourceButton, SIGNAL( clicked() ), this, SLOT(ToggleSourcePanel() ) );
if (change) showsourceview=!showsourceview;
if (showsourceview)
    {
    StackedViewers->setCurrentWidget(sourceviewerWidget);
    sourceviewerWidget->show();
    StackedViewers->show();
    showpdfview=false;
    ViewPdfPanelAct->setChecked(showpdfview);
    togglePdfButton->setEnabled(showpdfview);
    }
else
   {
   StackedViewers->hide();
   }
ViewSourcePanelAct->setChecked(showsourceview);
toggleSourceButton->setEnabled(showsourceview);
connect(ViewPdfPanelAct, SIGNAL(triggered()), this, SLOT(TogglePdfPanel()));
connect(togglePdfButton, SIGNAL( clicked() ), this, SLOT(TogglePdfPanel() ) );
connect(ViewSourcePanelAct, SIGNAL(triggered()), this, SLOT(ToggleSourcePanel()));
connect(toggleSourceButton, SIGNAL( clicked() ), this, SLOT(ToggleSourcePanel() ) );
}

void Texmaker::ShowFilesView(bool change)
{
if (change) showfilesview=!showfilesview;
if (showfilesview)
    {
    OpenedFilesListWidget->show();
    }
else
   {
   OpenedFilesListWidget->hide();
   }
ViewOpenedFilesPanelAct->setChecked(showfilesview);
}

void Texmaker::ToggleFullScreen() 
{
if(FullScreenAct->isChecked()) 
  {
  windowstate=saveState(0);
  setWindowState(Qt::WindowFullScreen);
  restoreState(fullscreenstate,1);
  }
else 
  {
  fullscreenstate=saveState(1);
  setWindowState(Qt::WindowNoState);
  restoreState(windowstate,0);
  }
}

void Texmaker::EditUserCompletion() 
{
UserCompletionDialog *ucDlg = new UserCompletionDialog(this,userCompletionList);
QTextCodec *codec = QTextCodec::codecForName("UTF-8");
QFile tagsfile(":/completion/completion.txt");
if (tagsfile.open(QFile::ReadOnly))
    {
    QTextStream tscompleter(&tagsfile);
    tscompleter.setCodec(codec);
    ucDlg->ui.plainTextEdit->setPlainText( tscompleter.readAll() );
    }
if ( ucDlg->exec() )
	{
	userCompletionList=ucDlg->userlist;
	initCompleter();
	updateCompleter();
	if (currentEditorView())
	  {
	  if (completion) currentEditorView()->editor->setCompleter(completer);
	  else currentEditorView()->editor->setCompleter(0);
	  currentEditorView()->editor->matchAll();
	  }
	}
}

void Texmaker::addBibFiles(QString param)
{
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
QString name=fi.absoluteFilePath();
QString flname=fi.fileName();
QString basename=name.left(name.length()-flname.length());
QString fname;
  
QStringList fl=param.split(",");
for (int i = 0;  i< fl.count(); i++)
  {
  fname=fl.at(i);
  if (fname.right(4)==".bib") fname=basename+fname;
  else fname=basename+fname+".bib";
  QFileInfo fi(fname);
  if (fi.exists() && fi.isReadable()) listbibfiles.append(fname);
  else
    {
    fname=fl.at(i);
    if (fname.right(4)!=".bib") fname=fname+".bib";
    QFileInfo fi(fname);
    if (fi.isAbsolute() && fi.exists() && fi.isReadable()) listbibfiles.append(fname);
    }
  }
}

void Texmaker::addIncludeFiles(QString param)
{
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
QString name=fi.absoluteFilePath();
QString flname=fi.fileName();
QString basename=name.left(name.length()-flname.length());
QString fname;
  
QStringList fl=param.split(",");
for (int i = 0;  i< fl.count(); i++)
  {
  fname=fl.at(i);
  if (fname.right(4)==".tex") fname=basename+fname;
  else fname=basename+fname+".tex";
  QFileInfo fi(fname);
  if (fi.exists() && fi.isReadable()) listchildfiles.append(fname);
  else
    {
    fname=fl.at(i);
    if (fname.right(4)!=".tex") fname=fname+".tex";
    QFileInfo fi(fname);
    if (fi.isAbsolute() && fi.exists() && fi.isReadable()) listchildfiles.append(fname);
    }
  }
}

void Texmaker::loadIncludeFiles(QString param, QString extension)
{
QString finame;
if (singlemode) {finame=getName();}
else {finame=MasterName;}
QFileInfo fi(finame);
QString name=fi.absoluteFilePath();
QString flname=fi.fileName();
QString basename=name.left(name.length()-flname.length());
QString fname;
  
QStringList fl=param.split(",");
for (int i = 0;  i< fl.count(); i++)
  {
  fname=fl.at(i);
  if (fname.right(4)==extension) fname=basename+fname;
  else fname=basename+fname+extension;
  QFileInfo fi(fname);
  if (fi.exists() && fi.isReadable()) load(fname);
  else
    {
    fname=fl.at(i);
    if (fname.right(4)!=extension) fname=fname+extension;
    QFileInfo fi(fname);
    if (fi.isAbsolute() && fi.exists() && fi.isReadable()) load(fname);
    }
  }
}

void Texmaker::showCursorPos(int li, int col)
{
QString linenumber;
if (col>0) linenumber.sprintf("L: %d C: %d", li,col);
else linenumber.sprintf("L: %d C: %d", li,1);
posLabel->setText(linenumber);
}

void Texmaker::keyPressEvent(QKeyEvent *event)
{
 if ((event->key() == Qt::Key_Escape) && showoutputview) 
  {  
       ShowOutputView(true);
   } 
else QMainWindow::keyPressEvent(event);
}

void Texmaker::customContentsMenuStructure( const QPoint &pos )
{
QMenu *menu = QMainWindow::createPopupMenu();
menu->addSeparator();
viewPstricksAct->setChecked(showPstricks);
menu->addAction(viewPstricksAct);
viewMpAct->setChecked(showMp);
menu->addAction(viewMpAct);
viewTikzAct->setChecked(showTikz);
menu->addAction(viewTikzAct);
viewAsyAct->setChecked(showAsy);
menu->addAction(viewAsyAct);

QPoint globalPos = LeftPanelToolBar->mapToGlobal(pos);
menu->exec( globalPos );
}

void Texmaker::TogglePstricks()
{
if (showPstricks)
  {
  if (LeftPanelToolBar->actions().contains(pstricksAct)) LeftPanelToolBar->removeAction(pstricksAct);
  }
else if (!LeftPanelToolBar->actions().contains(pstricksAct)) LeftPanelToolBar->addAction(pstricksAct);
showPstricks=!showPstricks;
}
void Texmaker::ToggleMetapost()
{
if (showMp)
  {
  if (LeftPanelToolBar->actions().contains(mpAct)) LeftPanelToolBar->removeAction(mpAct);
  }
else if (!LeftPanelToolBar->actions().contains(mpAct)) LeftPanelToolBar->addAction(mpAct);
showMp=!showMp;
}
void Texmaker::ToggleTikz()
{
if (showTikz)
  {
  if (LeftPanelToolBar->actions().contains(tikzAct)) LeftPanelToolBar->removeAction(tikzAct);
  }
else if (!LeftPanelToolBar->actions().contains(tikzAct)) LeftPanelToolBar->addAction(tikzAct);
showTikz=!showTikz;
}
void Texmaker::ToggleAsymptote()
{
if (showAsy)
  {
  if (LeftPanelToolBar->actions().contains(asyAct)) LeftPanelToolBar->removeAction(asyAct);
  }
else if (!LeftPanelToolBar->actions().contains(asyAct)) LeftPanelToolBar->addAction(asyAct);
showAsy=!showAsy;
}

void Texmaker::customContentsMenuMain( const QPoint &pos )
{
QMenu *menu = QMainWindow::createPopupMenu();
menu->addSeparator();
showemphasisAct->setChecked(showEmphasis);
menu->addAction(showemphasisAct);

shownewlineAct->setChecked(showNewline);
menu->addAction(shownewlineAct);

showmathmodeAct->setChecked(showMathmode);
menu->addAction(showmathmodeAct);

showindiceAct->setChecked(showIndice);
menu->addAction(showindiceAct);

showpuissanceAct->setChecked(showPuissance);
menu->addAction(showpuissanceAct);

showsmallfracAct->setChecked(showSmallfrac);
menu->addAction(showsmallfracAct);

showdfracAct->setChecked(showDfrac);
menu->addAction(showdfracAct);

showracineAct->setChecked(showRacine);
menu->addAction(showracineAct);

QPoint globalPos = centralToolBar->mapToGlobal(pos);
menu->exec( globalPos );
}

void Texmaker::ToggleEmphasis()
{
if (showEmphasis)
  {
  if (centralToolBar->actions().contains(emphasisAct)) centralToolBar->removeAction(emphasisAct);
  }
else if (!centralToolBar->actions().contains(emphasisAct)) centralToolBar->addAction(emphasisAct);
showEmphasis=!showEmphasis;
}

void Texmaker::ToggleNewline()
{
if (showNewline)
  {
  if (centralToolBar->actions().contains(newlineAct)) centralToolBar->removeAction(newlineAct);
  }
else if (!centralToolBar->actions().contains(newlineAct)) centralToolBar->addAction(newlineAct);
showNewline=!showNewline;
}

void Texmaker::ToggleMathmode()
{
if (showMathmode)
  {
  if (centralToolBar->actions().contains(mathmodeAct)) centralToolBar->removeAction(mathmodeAct);
  }
else if (!centralToolBar->actions().contains(mathmodeAct)) centralToolBar->addAction(mathmodeAct);
showMathmode=!showMathmode;
}

void Texmaker::ToggleIndice()
{
if (showIndice)
  {
  if (centralToolBar->actions().contains(indiceAct)) centralToolBar->removeAction(indiceAct);
  }
else if (!centralToolBar->actions().contains(indiceAct)) centralToolBar->addAction(indiceAct);
showIndice=!showIndice;
}

void Texmaker::TogglePuissance()
{
if (showPuissance)
  {
  if (centralToolBar->actions().contains(puissanceAct)) centralToolBar->removeAction(puissanceAct);
  }
else if (!centralToolBar->actions().contains(puissanceAct)) centralToolBar->addAction(puissanceAct);
showPuissance=!showPuissance;
}

void Texmaker::ToggleSmallfrac()
{
if (showSmallfrac)
  {
  if (centralToolBar->actions().contains(smallfracAct)) centralToolBar->removeAction(smallfracAct);
  }
else if (!centralToolBar->actions().contains(smallfracAct)) centralToolBar->addAction(smallfracAct);
showSmallfrac=!showSmallfrac;
}

void Texmaker::ToggleDfrac()
{
if (showDfrac)
  {
  if (centralToolBar->actions().contains(dfracAct)) centralToolBar->removeAction(dfracAct);
  }
else if (!centralToolBar->actions().contains(dfracAct)) centralToolBar->addAction(dfracAct);
showDfrac=!showDfrac;
}

void Texmaker::ToggleRacine()
{
if (showRacine)
  {
  if (centralToolBar->actions().contains(racineAct)) centralToolBar->removeAction(racineAct);
  }
else if (!centralToolBar->actions().contains(racineAct)) centralToolBar->addAction(racineAct);
showRacine=!showRacine;
}

void Texmaker::splitter2Changed()
{
QFontMetrics fm(qApp->font());
if (OutputTableWidget->isVisible()) OutputTextEdit->setMaximumHeight(5*(fm.lineSpacing()+4));
else 
{
  if (splitter2->sizes().at(1)>0) OutputTextEdit->setMaximumHeight(splitter2->sizes().at(1));
  else OutputTextEdit->setMaximumHeight(splitter2->sizes().at(0));
}
}

void Texmaker::SaveSession()
{
if ( !currentEditorView() ) return;
QString currentDir=QDir::homePath();
if (!lastDocument.isEmpty())
	{
	QFileInfo fi(lastDocument);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getSaveFileName(this,tr("Save"),currentDir,"Texmaker session (*.tks);;All files (*.*)");
if ( !fn.isEmpty() )
	{
	if (!fn.contains('.')) fn += ".tks";
	QFile fic(fn);
	if (!fic.open(QIODevice::WriteOnly)) 
	    {
	    QMessageBox::warning( this,tr("Error"),tr("The file could not be saved. Please check if you have write permission."));  
	     return;
	    }
	QDomDocument doc;
	QDomProcessingInstruction instr =  doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(instr);
	QDomElement root=doc.createElement("TexmakerSession");
	root.setAttribute("quickmode",QString::number(quickmode));
	doc.appendChild(root);
	QDomElement element;
	FilesMap::Iterator itf;
	QString docname;
	for( itf = filenames.begin(); itf != filenames.end(); ++itf )
		{
		docname=filenames[itf.key()];
		if (!docname.startsWith("untitled"))
		    {
		    element=doc.createElement("document");
		    element.setAttribute("file",docname);
		    element.setAttribute("line",QString::number(itf.key()->editor->textCursor().blockNumber()));
		    element.setAttribute("bookmark1",QString::number(itf.key()->editor->UserBookmark[0]));
		    element.setAttribute("bookmark2",QString::number(itf.key()->editor->UserBookmark[1]));
		    element.setAttribute("bookmark3",QString::number(itf.key()->editor->UserBookmark[2]));
		    if ((!singlemode) && (MasterName==docname)) 		    element.setAttribute("master","true");
		    else element.setAttribute("master","false");
		    if (itf.key()==currentEditorView()) element.setAttribute("hasfocus","true");
		    else element.setAttribute("hasfocus","false");
		    root.appendChild(element);
		    }
		}

	QTextStream out (&fic);
	doc.save(out,4);
	fic.close();
	}  
}

void Texmaker::LoadSession()
{
QString currentDir=QDir::homePath();
if (!lastDocument.isEmpty())
	{
	QFileInfo fi(lastDocument);
	if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
	}
QString fn = QFileDialog::getOpenFileName(this,tr("Open File"),currentDir,"Texmaker session (*.tks);;All files (*.*)");
if (fn.isEmpty()) return;
LoadSessionFile(fn);
}

void Texmaker::LoadSessionFile(const QString &fn)
{
QFile fic( fn );
if ( !fic.open( QIODevice::ReadOnly ) )
	{
	QMessageBox::warning( this,tr("Error"), tr("You do not have read permission to this file."));
	return;
	}
QDomDocument doc;
if (!doc.setContent(&fic)) 
  	{
	fic.close();
	return;
	}
QDomElement root = doc.documentElement();
if (root.tagName() != "TexmakerSession")
	{
	fic.close();
	return;
	}
if (root.hasAttribute("quickmode")) quickmode=root.attribute("quickmode").toInt();
QString file, ofile;
int b1, b2, b3, l, ob1, ob2, ob3, ol;
bool ma, oma;
bool fo=false;
QDomElement element = root.firstChildElement();
if (!singlemode) ToggleMode();
while (!element.isNull()) 
  {
  b1=0;
  b2=0;
  b3=0;
  l=0;
  ma=false;
  file="";
  if (element.hasAttribute("file"))
      {
      file=element.attribute("file");
      if (element.hasAttribute("line")) l=element.attribute("line").toInt();
      if (element.hasAttribute("bookmark1")) b1=element.attribute("bookmark1").toInt();
      if (element.hasAttribute("bookmark2")) b2=element.attribute("bookmark2").toInt();
      if (element.hasAttribute("bookmark3")) b3=element.attribute("bookmark3").toInt();
      if (element.hasAttribute("master")) ma=(element.attribute("master")=="true");
      if (element.hasAttribute("hasfocus")) fo=(element.attribute("hasfocus")=="true");
      if (fo)
	{
	ofile=file;
	ol=l;
	ob1=b1;
	ob2=b2;
	ob3=b3;
	oma=ma;
	}
      else
	{
	fileOpenAndGoto(file,l+1,false);
	if (currentEditorView() && getName()==file)
	  {
	  currentEditorView()->editor->UserBookmark[0]=b1;
	  currentEditorView()->editor->UserBookmark[1]=b2;
	  currentEditorView()->editor->UserBookmark[2]=b3;
	  currentEditorView()->update();
	  if (singlemode && ma) ToggleMode();
	  }
	}
	
      }
element=element.nextSiblingElement();
  }
fic.close();
if (!ofile.isEmpty())
    {
    fileOpenAndGoto(ofile,l+1,true);
    if (currentEditorView() && getName()==ofile)
      {
      currentEditorView()->editor->UserBookmark[0]=ob1;
      currentEditorView()->editor->UserBookmark[1]=ob2;
      currentEditorView()->editor->UserBookmark[2]=ob3;
      currentEditorView()->update();
      if (singlemode && oma) ToggleMode();
      }
    }
}

void Texmaker::SaveLastSession()
{
if ( !currentEditorView() ) return;
QFile fic(sessionTempFile);
if (!fic.open(QIODevice::WriteOnly)) 
    {
      return;
    }
QDomDocument doc;
QDomProcessingInstruction instr =  doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
doc.appendChild(instr);
QDomElement root=doc.createElement("TexmakerSession");
root.setAttribute("quickmode",QString::number(quickmode));
doc.appendChild(root);
QDomElement element;
FilesMap::Iterator itf;
QString docname;
for( itf = filenames.begin(); itf != filenames.end(); ++itf )
	{
	docname=filenames[itf.key()];
	if (!docname.startsWith("untitled"))
	    {
	    element=doc.createElement("document");
	    element.setAttribute("file",docname);
	    element.setAttribute("line",QString::number(itf.key()->editor->textCursor().blockNumber()));
	    element.setAttribute("bookmark1",QString::number(itf.key()->editor->UserBookmark[0]));
	    element.setAttribute("bookmark2",QString::number(itf.key()->editor->UserBookmark[1]));
	    element.setAttribute("bookmark3",QString::number(itf.key()->editor->UserBookmark[2]));
	    if ((!singlemode) && (MasterName==docname)) 		    element.setAttribute("master","true");
	    else element.setAttribute("master","false");
	    if (itf.key()==currentEditorView()) element.setAttribute("hasfocus","true");
	    else element.setAttribute("hasfocus","false");
	    root.appendChild(element);
	    }
	}

QTextStream out (&fic);
doc.save(out,4);
fic.close();

}

void Texmaker::LoadLastSession()
{
QFileInfo fi(sessionTempFile);
if (!fi.exists()) return;
QFile fic( sessionTempFile );
if ( !fic.open( QIODevice::ReadOnly ) )
	{
	QMessageBox::warning( this,tr("Error"), tr("You do not have read permission to this file."));
	return;
	}
QDomDocument doc;
if (!doc.setContent(&fic)) 
  	{
	fic.close();
	return;
	}
QDomElement root = doc.documentElement();
if (root.tagName() != "TexmakerSession")
	{
	fic.close();
	return;
	}
if (root.hasAttribute("quickmode")) quickmode=root.attribute("quickmode").toInt();
QString file, ofile;
int b1, b2, b3, l, ob1, ob2, ob3, ol;
bool ma, oma;
bool fo=false;
QDomElement element = root.firstChildElement();
if (!singlemode) ToggleMode();
while (!element.isNull()) 
  {
  b1=0;
  b2=0;
  b3=0;
  l=0;
  ma=false;
  file="";
  if (element.hasAttribute("file"))
      {
      file=element.attribute("file");
      if (element.hasAttribute("line")) l=element.attribute("line").toInt();
      if (element.hasAttribute("bookmark1")) b1=element.attribute("bookmark1").toInt();
      if (element.hasAttribute("bookmark2")) b2=element.attribute("bookmark2").toInt();
      if (element.hasAttribute("bookmark3")) b3=element.attribute("bookmark3").toInt();
      if (element.hasAttribute("master")) ma=(element.attribute("master")=="true");
      if (element.hasAttribute("hasfocus")) fo=(element.attribute("focus")=="true");
      if (fo)
	{
	ofile=file;
	ol=l;
	ob1=b1;
	ob2=b2;
	ob3=b3;
	oma=ma;
	}
      else
	{
	fileOpenAndGoto(file,l+1,false);
      	if (currentEditorView() && getName()==file)
	  {
	  currentEditorView()->editor->UserBookmark[0]=b1;
	  currentEditorView()->editor->UserBookmark[1]=b2;
	  currentEditorView()->editor->UserBookmark[2]=b3;
	  currentEditorView()->update();
	  if (singlemode && ma) ToggleMode();
	  }
	}
	
      }
element=element.nextSiblingElement();
  }
fic.close();
if (!ofile.isEmpty())
    {
    fileOpenAndGoto(ofile,l+1,true);
	if (currentEditorView() && getName()==ofile)
	  {
	  currentEditorView()->editor->UserBookmark[0]=ob1;
	  currentEditorView()->editor->UserBookmark[1]=ob2;
	  currentEditorView()->editor->UserBookmark[2]=ob3;
	  currentEditorView()->update();
	  if (singlemode && oma) ToggleMode();
	  }
    }
}

bool Texmaker::copyFile(QString origin,QString destination)
{
if (destination.isEmpty() || origin.isEmpty()) return false;
QFileInfo fi_or(origin);
if (!fi_or.exists()) return false;
QFile file_or(origin);
QFileInfo fi_dest(destination);
if (fi_dest.exists())
	{
	QFile file_dest(destination);
	file_dest.remove();
	file_or.copy(destination);
	}
else
	{
	file_or.copy(destination);
	}
return true;
}

// void Texmaker::removeDir(QDir thedir)
// {
// QString name=thedir.dirName();
// QDirIterator iterator(thedir,QDirIterator::NoIteratorFlags);
// while(iterator.hasNext())
//   {
//   QString entry(iterator.next());
//   if(!QFileInfo(entry).isDir())
//     {
//     if (QFile::exists(entry))
// 	    {
// 	    QFile file(entry);
// 	    file.open( QIODevice::ReadOnly );
// 	    file.remove();
// 	    }    
//     }
//   }
// thedir.cdUp();
// thedir.rmdir(name);
// }


void Texmaker::createBuildSubdirectory(QString fn)
{
if (fn.isEmpty() || fn.startsWith("untitled")) return;
QFileInfo fi(fn);
if (!fi.exists()) return;
QDir basedir(fi.absolutePath());
QDir outputdir(fi.absolutePath()+"/build");
if (outputdir.exists()) return;
basedir.mkdir("build");
}

QString Texmaker::outputName(QString finame,QString extension)
{
return outputBaseName(finame)+extension;
}

QString Texmaker::outputBaseName(QString finame)
{
QString name="";
QFileInfo fi(finame);
QString path=fi.absolutePath();
QString fn=fi.fileName();
if (useoutputdir) name=path+"/build/"+fn;
else name=fi.absoluteFilePath();
QString ext=fi.suffix();
QString basename=name.left(name.length()-ext.length()-1);
return basename;
}

bool Texmaker::gtkSession()
{
bool result=false;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
result=true;// 1 : no kde ; 2: kde ; 3 : kde4 ; 
QString kdesession= ::getenv("KDE_FULL_SESSION");
QString kdeversion= ::getenv("KDE_SESSION_VERSION");
if (!kdesession.isEmpty()) result=false;
if (!kdeversion.isEmpty()) result=false;
#endif
return result;
}

void Texmaker::setMasterDocument(const QString &fn)
{
if (singlemode && currentEditorView())  
	{
	MasterName=fn;
	if (MasterName.startsWith("untitled") || MasterName=="")
		{
		QMessageBox::warning( this,tr("Error"),tr("Could not start the command.")+"\nno valid name for the master document");
		return;
		}
	QFileInfo fi(MasterName);
	if (fi.exists() && fi.isReadable())
	    {
	    QString shortName = MasterName;
	    int pos;
	    while ( (pos = (int)shortName.indexOf('/')) != -1 ) shortName.remove(0,pos+1);
	    ToggleAct->setText(tr("Normal Mode (current master document :")+shortName+")");
	    singlemode=false;
	    stat1->setText(QString(" %1 ").arg(tr("Master Document :")+shortName));
	    ToggleDocAct->setEnabled(true);
	    UpdateStructure();
	    UpdateBibliography();
	    }
	}
}

void Texmaker::compareDocuments()
{
if ( !currentEditorView() ) return;
QString code=QString::fromUtf8("<!-- Generated by Texmaker -->\n");
QTextCodec *codec = QTextCodec::codecForName("UTF-8");
QFile templatefile;
templatefile.setFileName(":/diff/checkdiff.txt");
templatefile.open(QIODevice::ReadOnly);
QTextStream t(&templatefile);
t.setCodec(codec);
while (!t.atEnd()) 
	{
	code+= t.readLine()+"\n";
	}
templatefile.close();
code.replace("#BASE#",currentEditorView()->editor->toPlainText());
code.replace("#NEW#",sourceviewerWidget->editor->toPlainText());

QString tempDir=QDir::tempPath();
QString prefixFile=QDir::homePath();
prefixFile="texmaker_temp_diff_"+prefixFile.section('/',-1);
prefixFile=QString(QUrl::toPercentEncoding(prefixFile));
prefixFile.remove("%");
QString tempFile=tempDir+"/"+prefixFile+".html";

QFile fi_html(tempFile);
if (!fi_html.open(QIODevice::WriteOnly)) return;
QTextStream out (&fi_html);
out.setCodec(codec);
out << code;
fi_html.close();
#ifdef INTERNAL_BROWSER
if (diffWindow)
  {
  diffWindow->close();
  }
diffWindow=new Browser("file:///"+tempFile,false, 0);
diffWindow->raise();
diffWindow->show();
#else
QDesktopServices::openUrl("file:///"+tempFile);
#endif
}
