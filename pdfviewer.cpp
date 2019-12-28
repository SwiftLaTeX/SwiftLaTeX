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

#include "pdfviewer.h"

#include <QtGui>
#include <QAction>
#include <QFontMetrics>
#include <QDebug>
#include <QPrinter>
#include <QPrintDialog>
#include <QSettings>
#include <QTextStream>
#include <QDesktopServices>
#include <QTextCodec>
#include <QProgressDialog>
#include <QHeaderView>
#include <QScrollBar>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QDesktopWidget>

#include "geticon.h"
#include "theme.h"


#define SYNCTEX_GZ_EXT ".synctex.gz"
#define SYNCTEX_EXT ".synctex"

PdfViewer::PdfViewer( const QString fileName, const QString externalCommand, const QString ghostscriptCommand, const QString lpopt,const QKeySequence edfocus,const QString SpellLang, const qreal startScale, QWidget* parent)
    : QMainWindow( parent)
{
setWindowTitle("Texmaker : pdf preview");
#if defined(Q_OS_MAC)
setWindowIcon(QIcon(":/images/logo128.png"));
#else
setWindowIcon(getIcon(":/images/appicon.png"));
#endif

#ifdef USB_VERSION
QSettings *config=new QSettings(QCoreApplication::applicationDirPath()+"/texmakerpdfview.ini",QSettings::IniFormat); //for USB-stick version :
#else
QSettings *config=new QSettings(QSettings::IniFormat,QSettings::UserScope,"xm1","texmakerpdfview");
#endif
config->beginGroup( "pdfviewer" );
QRect screen = QApplication::desktop()->screenGeometry();
int w= config->value( "Geometries/MainwindowWidth",screen.width()/2).toInt();
int h= config->value( "Geometries/MainwindowHeight",screen.height()-150).toInt() ;
int x= config->value( "Geometries/MainwindowX",screen.width()/2).toInt();
int y= config->value( "Geometries/MainwindowY",30).toInt() ;
resize(w,h);
move(x,y);
windowstate=config->value("MainWindowState").toByteArray();
config->endGroup();

spell_lang=SpellLang;
KeySequenceEditorFocus=edfocus;
setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
//canDisplayPixmap=false;
pdf_file=fileName;
viewpdf_command=externalCommand;
gswin32c_command=ghostscriptCommand;
lp_options=lpopt;
lastFile=fileName;
lastPage=1;
lastHpos=0;
islastContinuous=true;
lastScale=startScale;
fileLoaded=false;
currentPage=1;


lastSearchPos=-1;
scanner=NULL;
path= QPainterPath();

StructureView = new QDockWidget(this);
StructureView->setObjectName("StructureView");
StructureView->setAllowedAreas(Qt::AllDockWidgetAreas);
StructureView->setFeatures(QDockWidget::DockWidgetClosable);
StructureView->setWindowTitle(tr("Structure"));
addDockWidget(Qt::LeftDockWidgetArea, StructureView);

QFrame *LeftPanelFrame=new QFrame(StructureView);
LeftPanelFrame->setLineWidth(0);
LeftPanelFrame->setFrameShape(QFrame::Box);
LeftPanelFrame->setFrameShadow(QFrame::Plain);
LeftPanelFrame->setMinimumWidth(100);

LeftPanelLayout= new QHBoxLayout(LeftPanelFrame);
LeftPanelToolBar=new QToolBar("StructureToolBar",StructureView);
LeftPanelToolBar->setFloatable(false);
LeftPanelToolBar->setOrientation(Qt::Vertical);
LeftPanelToolBar->setMovable(false);
LeftPanelToolBar->setIconSize(QSize(16,16 ));
LeftPanelStackedWidget=new QStackedWidget(StructureView);

listpagesWidget=new QListWidget(LeftPanelStackedWidget);
listpagesWidget->setFrameShape(QFrame::NoFrame);
listpagesWidget->setFrameShadow(QFrame::Plain);
listpagesWidget->viewport()->setStyleSheet(Theme::viewportDarkStyleSheet);
listpagesWidget->setStyleSheet(Theme::listwidgetStyleSheet);
listpagesWidget->setLineWidth(0);

connect(LeftPanelToolBar->addAction(getIcon(":/images/pages.png"),tr("Pages")), SIGNAL(triggered()), this, SLOT(ShowListPages()));
LeftPanelStackedWidget->addWidget(listpagesWidget);

LeftPanelToolBar->addSeparator();

StructureTreeView=new QTreeView(LeftPanelStackedWidget);
StructureTreeView->header()->hide();
StructureTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
StructureTreeView->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

StructureTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

StructureTreeView->header()->setStretchLastSection(false);
StructureTreeView->setIndentation(15);
StructureTreeView->setModel(0);
StructureTreeView->viewport()->setStyleSheet(Theme::viewportDarkStyleSheet);
StructureTreeView->setStyleSheet(Theme::treeviewStyleSheet);


connect(LeftPanelToolBar->addAction(getIcon(":/images/structure.png"),tr("Structure")), SIGNAL(triggered()), this, SLOT(ShowStructure()));
LeftPanelStackedWidget->addWidget(StructureTreeView);

showingListPages=true;
ShowListPages();

LeftPanelLayout->setSpacing(0);
LeftPanelLayout->setMargin(0);
LeftPanelLayout->addWidget(LeftPanelToolBar);
LeftPanelLayout->addWidget(LeftPanelStackedWidget);
StructureView->setWidget(LeftPanelFrame);
LeftPanelStackedWidget->setCurrentWidget(listpagesWidget);


QFrame *centralFrame=new QFrame(this);
centralFrame->setLineWidth(0);
centralFrame->setFrameShape(QFrame::StyledPanel);
centralFrame->setFrameShadow(QFrame::Plain);
centralFrame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

pdfview=new DocumentView(centralFrame);


CentralLayout= new QHBoxLayout(centralFrame);
CentralLayout->setSpacing(0);
CentralLayout->setMargin(0);
CentralLayout->addWidget(pdfview);

setCentralWidget(centralFrame);


QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
//printAct = new QAction(getIcon(":/images/print.png"), tr("Print"), this);
//connect(printAct, SIGNAL(triggered()), this, SLOT(printPdf()));
//fileMenu->addAction(printAct);
fileMenu->addAction(tr("Exit"), this, SLOT(close()),Qt::CTRL+Qt::Key_Q);

QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
findAct = new QAction( tr("Find"), this);
findAct->setShortcut(Qt::CTRL+Qt::Key_F);
editMenu->addAction(findAct);



QToolBar *toolBar = addToolBar("Pdf controls");
toolBar->setObjectName("Pdf controls");
toolBar->setStyleSheet(Theme::toolbarlightStyleSheet);

upAct = new QAction(getIcon(":/images/up.png"), tr("Previous"), this);
upAct->setShortcut(QKeySequence::MoveToPreviousPage);
toolBar->addAction(upAct);

downAct = new QAction(getIcon(":/images/down.png"), tr("Next"), this);
downAct->setShortcut(QKeySequence::MoveToNextPage);
toolBar->addAction(downAct);

toolBar->addSeparator();

QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
fitWithAct = new QAction(getIcon(":/images/zoom-fit-width.png"), tr("Fit Width"), this);
toolBar->addAction(fitWithAct);

fitPageAct = new QAction(getIcon(":/images/zoom-fit-best.png"), tr("Fit Page"), this);
toolBar->addAction(fitPageAct);

zoominAct = new QAction(getIcon(":/images/zoom-in.png"), tr("Zoom In"), this);
zoominAct->setShortcut(QKeySequence::ZoomIn);
toolBar->addAction(zoominAct);
viewMenu->addAction(zoominAct);

zoomoutAct = new QAction(getIcon(":/images/zoom-out.png"), tr("Zoom Out"), this);
zoomoutAct->setShortcut(QKeySequence::ZoomOut);
toolBar->addAction(zoomoutAct);
viewMenu->addAction(zoomoutAct);

continuousModeAction = new QAction(tr("Continuous"), this);
continuousModeAction->setCheckable(true);
continuousModeAction->setChecked(true);
viewMenu->addAction(continuousModeAction);
  
twoPagesModeAction = new QAction(tr("Two pages"), this);
twoPagesModeAction->setCheckable(true);
twoPagesModeAction->setChecked(false);
viewMenu->addAction(twoPagesModeAction);
  
rotateLeftAction = new QAction(tr("Rotate left"), this);
rotateLeftAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left));
viewMenu->addAction(rotateLeftAction);
  
rotateRightAction = new QAction(tr("Rotate right"), this);
rotateRightAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right));
viewMenu->addAction(rotateRightAction);

presentationAction = new QAction(tr("Presentation..."), this);
viewMenu->addAction(presentationAction);
  
viewMenu->addSeparator();
viewMenu->addAction(StructureView->toggleViewAction());


historyBackAct = new QAction(getIcon(":/images/errorprev.png"), tr("Previous Position"), this);
historyBackAct->setShortcut(QKeySequence::Back);
//connect(historyBackAct, SIGNAL(triggered()), this, SLOT(historyBack()));
LeftPanelToolBar->addAction(historyBackAct);

historyForwardAct = new QAction(getIcon(":/images/errornext.png"), tr("Next Position"), this);
historyForwardAct->setShortcut(QKeySequence::Forward);
//connect(historyForwardAct, SIGNAL(triggered()), this, SLOT(historyForward()));
LeftPanelToolBar->addAction(historyForwardAct);

connect( this, SIGNAL( backwardAvailable( bool ) ), historyBackAct, SLOT( setEnabled( bool ) ) );
connect( this, SIGNAL( forwardAvailable( bool ) ), historyForwardAct, SLOT( setEnabled( bool ) ) );

scaleComboBox = new QComboBox(toolBar);
zoomCustom = new QLineEdit();
//QValidator *validatorZoom = new QIntValidator(25, 400, this);
QValidator *validatorZoom = new QRegExpValidator(QRegExp("\\d{0,3}%?"), this);
zoomCustom->setValidator(validatorZoom);
scaleComboBox->setInsertPolicy(QComboBox::NoInsert);
scalePercents << "25%" << "50%" << "75%" << "100%" << "125%" << "150%" << "200%" << "300%" << "400%";
scaleComboBox->addItems(scalePercents);
scaleComboBox->setCurrentIndex(3);
scaleComboBox->setEditable(true);
scaleComboBox->setLineEdit(zoomCustom);
scaleComboBox->setStyleSheet(Theme::comboboxStyleSheet);
toolBar->addWidget(scaleComboBox);

toolBar->addSeparator();

searchLineEdit = new QLineEdit(toolBar);
searchLineEdit->setStyleSheet(Theme::lineeditStyleSheet);
toolBar->addWidget(searchLineEdit);

searchAct=new QAction(getIcon(":/images/pdffind.png"), tr("Find"), this);
toolBar->addAction(searchAct);
//findButton=new QPushButton(tr("Find"),toolBar);
//toolBar->addWidget(findButton);


toolBar->addSeparator();
QWidget* spacer = new QWidget();
spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
toolBar->addWidget(spacer);

printAct = new QAction(getIcon(":/images/print.png"), tr("Print"), this);
toolBar->addAction(printAct);

externAct = new QAction(getIcon(":/images/viewpdf.png"), tr("External Viewer"), this);
toolBar->addAction(externAct);



searchLineEdit->setEnabled(false);
searchAct->setEnabled(false);
//findButton->setEnabled(false);
scaleComboBox->setEnabled(false);
upAct->setEnabled(false);
downAct->setEnabled(false);
fitWithAct->setEnabled(false);
fitPageAct->setEnabled(false);
zoominAct->setEnabled(false);
zoomoutAct->setEnabled(false);
continuousModeAction->setEnabled(false);
twoPagesModeAction->setEnabled(false);
rotateLeftAction->setEnabled(false);
rotateRightAction->setEnabled(false);
presentationAction->setEnabled(false);

restoreState(windowstate, 0);
openFile(pdf_file,viewpdf_command,gswin32c_command);
}

PdfViewer::~PdfViewer()
{
if (scanner != NULL) synctex_scanner_free(scanner);
if (proc && proc->state()==QProcess::Running) 
	{
	proc->kill(); 
	delete proc ;
	}
}

void PdfViewer::closeEvent(QCloseEvent *e)
{
#ifdef USB_VERSION
QSettings config(QCoreApplication::applicationDirPath()+"/texmakerpdfview.ini",QSettings::IniFormat); //for USB-stick version 
#else
QSettings config(QSettings::IniFormat,QSettings::UserScope,"xm1","texmakerpdfview");
#endif
config.beginGroup( "pdfviewer" );
config.setValue("MainWindowState",saveState(0));
config.setValue("Geometries/MainwindowWidth", width() );
config.setValue("Geometries/MainwindowHeight", height() );
config.setValue("Geometries/MainwindowX", x() );
config.setValue("Geometries/MainwindowY", y() );
config.endGroup();
}

void PdfViewer::openFile(QString fn,QString ec,QString pc)
{
disconnectActions();
show();
if (windowState()==Qt::WindowMinimized) setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
qApp->setActiveWindow(this);
activateWindow();
path= QPainterPath();
altern=1;
currentScale=1;
pdf_file=fn;
viewpdf_command=ec;
gswin32c_command=pc;
int lpage=lastPage;
qreal lscale=lastScale;
if (scanner != NULL) 
{
synctex_scanner_free(scanner);
scanner = NULL;
}

if (pdfview->open(fn)) 
  {
  listpagesWidget->clear();
  lastSearchPos=-1;
  searchLineEdit->setEnabled(true);
  searchAct->setEnabled(true);
  //findButton->setEnabled(true);
  scaleComboBox->setEnabled(true);
  upAct->setEnabled(false);
  downAct->setEnabled(true);
  fitWithAct->setEnabled(true);
  fitPageAct->setEnabled(true);
  zoominAct->setEnabled(true);
  zoomoutAct->setEnabled(true);
  continuousModeAction->setEnabled(true);//
  rotateLeftAction->setEnabled(true);
  rotateRightAction->setEnabled(true);
  presentationAction->setEnabled(true);
  continuousModeAction->setChecked(islastContinuous);//
  pdfview->setContinousMode(islastContinuous);
    twoPagesModeAction->setEnabled(true);
  //pdfview->setTwoPagesMode(false);
  pdfview->show();
  
  StructureTreeView->setModel(pdfview->outlineModel());
  QString syncFile = QFileInfo(fn).canonicalFilePath();
  scanner = synctex_scanner_new_with_output_file(syncFile.toUtf8().data(), NULL, 1);
  if ((fn==lastFile) && (lpage <= pdfview->doc()->pageCount()) && (lpage>0))
    {
    currentPage=lpage;
    currentScale=lscale;
    if ((currentScale < 0.25) || (currentScale > 4)) 
      {
      pdfview->setScaleMode(DocumentView::FitToPageWidth);
      currentScale=pdfview->realScale();
      zoomCustom->setText(QString::number(int(currentScale*100)) + "%");

      }
    else 
      {
      zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
      pdfview->setScaleFactor(currentScale);
      }
    }
  else 
    {
    currentPage=1;
    lastPage=1;
    pdfview->setScaleMode(DocumentView::FitToPageWidth);
    currentScale=pdfview->realScale();
    zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
    }
  for (int i = 0; i < pdfview->doc()->pageCount(); ++i)
    {
    listpagesWidget->addItem(tr("Page")+" "+QString::number(i+1)); 
    }
  fileLoaded=true;
  lastFile=fn;
  setWindowTitle(QFileInfo(fn).fileName());
  clearHistory();
  if (showingListPages) ShowListPages();
  else ShowStructure();
  lastScale=currentScale;
  previousScale=currentScale;
  connectActions();
  gotoPage(currentPage);
  disconnect(pdfview,SIGNAL(doHScroll(int)), this, SLOT(setHpos(int)));
  if (pdfview->horizontalScrollBar()->isVisible())  {QTimer::singleShot( 500,this, SLOT(jumptoHpos()) );}
  connect(pdfview,SIGNAL(doHScroll(int)), this, SLOT(setHpos(int)));
  emit sendFocusToEditor();
  } 
else 
  {
  QMessageBox::warning( this,tr("Error"),tr("File not found"));
  fileLoaded=false;
  searchLineEdit->setEnabled(false);
  searchAct->setEnabled(false);
  //findButton->setEnabled(false);
  scaleComboBox->setEnabled(false);
  upAct->setEnabled(false);
  downAct->setEnabled(false);
  fitWithAct->setEnabled(false);
  fitPageAct->setEnabled(false);
  zoominAct->setEnabled(false);
  zoomoutAct->setEnabled(false);
  continuousModeAction->setEnabled(false);
  twoPagesModeAction->setEnabled(false);
  rotateLeftAction->setEnabled(false);
  rotateRightAction->setEnabled(false);
  presentationAction->setEnabled(false);
  disconnectActions();
  StructureTreeView->setModel(0);
  listpagesWidget->clear();
  pdfview->clearAll();
  }
}


void PdfViewer::connectActions()
{
connect(findAct, SIGNAL(triggered()), this, SLOT(enableSearch()));
connect(upAct, SIGNAL(triggered()), this, SLOT(pageUp()));
connect(downAct, SIGNAL(triggered()), this, SLOT(pageDown()));
connect(listpagesWidget, SIGNAL(itemActivated ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
connect(listpagesWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
connect(StructureTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(ClickedOnStructure(QModelIndex)));
connect(fitWithAct, SIGNAL(triggered()), this, SLOT(fitWidth()));
connect(fitPageAct, SIGNAL(triggered()), this, SLOT(fitPage()));
connect(zoominAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
connect(zoomoutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
connect(zoomCustom, SIGNAL(returnPressed()),this, SLOT(userZoom()));
connect(scaleComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(scaleDocumentZoom(QString)));
connect(searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchDocument()));
connect(searchAct, SIGNAL(triggered()), this, SLOT(searchDocument()));
//connect(findButton, SIGNAL(clicked()), this, SLOT(searchDocument()));
connect(historyBackAct, SIGNAL(triggered()), this, SLOT(historyBack()));
connect(historyForwardAct, SIGNAL(triggered()), this, SLOT(historyForward()));
connect(printAct, SIGNAL(triggered()), this, SLOT(printPdf()));
connect(externAct, SIGNAL(triggered()), this, SLOT(runExternalViewer()));
connect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
connect(pdfview, SIGNAL(gotoDest(int,qreal,qreal)), this, SLOT(jumpToDest(int,qreal,qreal)));
connect(pdfview, SIGNAL(syncpage(int, const QPointF&)), this, SLOT(jumpToEditor(int, const QPointF&)));
connect(pdfview, SIGNAL(requestZoomIn()), this, SLOT(zoomIn()));
connect(pdfview, SIGNAL(requestZoomOut()), this, SLOT(zoomOut()));
connect(pdfview, SIGNAL(continousModeChanged(bool)), this,SLOT(on_currentTab_continuousModeChanged(bool)));
connect(pdfview, SIGNAL(twoPagesModeChanged(bool)), this,SLOT(on_currentTab_twoPagesModeChanged(bool)));
connect(pdfview,SIGNAL(doHScroll(int)), this, SLOT(setHpos(int)));
connect(continuousModeAction, SIGNAL(triggered(bool)),this, SLOT(on_continuousMode_triggered(bool)));
connect(twoPagesModeAction, SIGNAL(triggered(bool)),this, SLOT(on_twoPagesMode_triggered(bool)));
connect(rotateLeftAction, SIGNAL(triggered()), this,SLOT(on_rotateLeft_triggered()));
connect(rotateRightAction, SIGNAL(triggered()), this,SLOT(on_rotateRight_triggered()));
connect(presentationAction, SIGNAL(triggered()),this, SLOT(on_presentation_triggered()));

//connect( this, SIGNAL( backwardAvailable( bool ) ), historyBackAct, SLOT( setEnabled( bool ) ) );
//connect( this, SIGNAL( forwardAvailable( bool ) ), historyForwardAct, SLOT( setEnabled( bool ) ) );
}

void PdfViewer::disconnectActions()
{
disconnect(findAct, SIGNAL(triggered()), this, SLOT(enableSearch()));
disconnect(upAct, SIGNAL(triggered()), this, SLOT(pageUp()));
disconnect(downAct, SIGNAL(triggered()), this, SLOT(pageDown()));
disconnect(listpagesWidget, SIGNAL(itemActivated ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
disconnect(listpagesWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
disconnect(StructureTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(ClickedOnStructure(QModelIndex)));
disconnect(fitWithAct, SIGNAL(triggered()), this, SLOT(fitWidth()));
disconnect(fitPageAct, SIGNAL(triggered()), this, SLOT(fitPage()));
disconnect(zoominAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
disconnect(zoomoutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
disconnect(zoomCustom, SIGNAL(returnPressed()),this, SLOT(userZoom()));
disconnect(scaleComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(scaleDocumentZoom(QString)));
disconnect(searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchDocument()));
disconnect(searchAct, SIGNAL(triggered()), this, SLOT(searchDocument()));
//disconnect(findButton, SIGNAL(clicked()), this, SLOT(searchDocument()));
disconnect(historyBackAct, SIGNAL(triggered()), this, SLOT(historyBack()));
disconnect(historyForwardAct, SIGNAL(triggered()), this, SLOT(historyForward()));
disconnect(printAct, SIGNAL(triggered()), this, SLOT(printPdf()));
disconnect(externAct, SIGNAL(triggered()), this, SLOT(runExternalViewer()));
disconnect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
disconnect(pdfview, SIGNAL(gotoDest(int,qreal,qreal)), this, SLOT(jumpToDest(int,qreal,qreal)));
disconnect(pdfview, SIGNAL(syncpage(int, const QPointF&)), this, SLOT(jumpToEditor(int, const QPointF&)));
disconnect(pdfview, SIGNAL(requestZoomIn()), this, SLOT(zoomIn()));
disconnect(pdfview, SIGNAL(requestZoomOut()), this, SLOT(zoomOut()));
disconnect(pdfview, SIGNAL(continousModeChanged(bool)), this,SLOT(on_currentTab_continuousModeChanged(bool)));
disconnect(pdfview, SIGNAL(twoPagesModeChanged(bool)), this,SLOT(on_currentTab_twoPagesModeChanged(bool)));
disconnect(pdfview,SIGNAL(doHScroll(int)), this, SLOT(setHpos(int)));
disconnect(continuousModeAction, SIGNAL(triggered(bool)), this,SLOT(on_continuousMode_triggered(bool)));
disconnect(twoPagesModeAction, SIGNAL(triggered(bool)), this,SLOT(on_twoPagesMode_triggered(bool)));
disconnect(rotateLeftAction, SIGNAL(triggered()), this,SLOT(on_rotateLeft_triggered()));
disconnect(rotateRightAction, SIGNAL(triggered()), this,SLOT(on_rotateRight_triggered()));
disconnect(presentationAction, SIGNAL(triggered()),this, SLOT(on_presentation_triggered()));
//disconnect( this, SIGNAL( backwardAvailable( bool ) ), historyBackAct, SLOT( setEnabled( bool ) ) );
//disconnect( this, SIGNAL( forwardAvailable( bool ) ), historyForwardAct, SLOT( setEnabled( bool ) ) );
}

void PdfViewer::jumpToPdfFromSource(QString sourceFile, int source_line, int source_col)
{
show();
if (windowState()==Qt::WindowMinimized) setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
qApp->setActiveWindow(this);
activateWindow();
if (!fileLoaded) return;
if (scanner == NULL) 
  {
  gotoPage(currentPage);
  return;
  }
const QFileInfo sourceFileInfo(sourceFile);
QDir curDir(QFileInfo(pdf_file).canonicalPath());
synctex_node_t node = synctex_scanner_input(scanner);
QString name;
bool found = false;
while (node != NULL) 
  {
  name = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
  const QFileInfo fi(curDir, name);
  if (fi == sourceFileInfo) 
    {
    found = true;
    break;
    }
  node = synctex_node_sibling(node);
  }
if (!found) 
  {
  gotoPage(currentPage);
  return;
  }
if (synctex_display_query(scanner, name.toUtf8().data(), source_line, 0) > 0) 
  {
  int page = -1;
  path= QPainterPath();
  while ((node = synctex_next_result(scanner)) != NULL) 
    {
    if (page == -1) page = synctex_node_page(node);
    if (synctex_node_page(node) != page) continue;
    QRectF nodeRect(synctex_node_box_visible_h(node),
				    synctex_node_box_visible_v(node) - synctex_node_box_visible_height(node),
				    synctex_node_box_visible_width(node),
				    synctex_node_box_visible_height(node) + synctex_node_box_visible_depth(node));
    path.addRect(nodeRect);
    }
  if (page > 0) 
    {
     disconnect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
    QRectF r = path.boundingRect();
    currentPage=page;
    lastPage=currentPage;
    pdfview->showRect(currentPage-1,r);
    path.setFillRule(Qt::WindingFill);
    QTimer::singleShot(500,this, SLOT(slotHighlight()) );
    updateCurrentPage();
    connect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
    }
  }
  else 
  {
    gotoPage(currentPage);
  }
}

void PdfViewer::slotHighlight()
{
pdfview->setHighlightPath(currentPage-1,path);
}

void PdfViewer::gotoPage(int page)
{
if (!fileLoaded) return;
if ((page <= pdfview->doc()->pageCount()) && (page>=1))
  {
  currentPage=page;
  lastPage=currentPage;
  pdfview->jumpToPage(currentPage,0.0, 0.0, false);
  disconnect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
  updateHistory(pdfview->verticalScrollBar()->value());
  updateCurrentPage();
  connect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
  }
}

void PdfViewer::checkPage(int value)
{
if (!fileLoaded) return;
currentPage=value;
lastPage=currentPage;
updateCurrentPage();
}

void PdfViewer::updateCurrentPage()
{
if (!fileLoaded) return;
if (currentPage==1) upAct->setEnabled(false);
else upAct->setEnabled(true);
if (currentPage==pdfview->doc()->pageCount()) downAct->setEnabled(false);
else downAct->setEnabled(true);
QList<QListWidgetItem *> fItems=listpagesWidget->findItems (tr("Page")+" "+QString::number(currentPage),Qt::MatchRecursive);
if ((fItems.size()>0 ) && (fItems.at(0))) listpagesWidget->setCurrentItem(fItems.at(0));
}

void PdfViewer::jumpToDest(int page,qreal left, qreal top)
{
if (!fileLoaded) return;
if ((page <= pdfview->doc()->pageCount()) && (page>=1))
  {
  currentPage=page;
  lastPage=currentPage;
  pdfview->jumpToPage(currentPage,left,top, false);
  disconnect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
  updateHistory(pdfview->verticalScrollBar()->value());
  updateCurrentPage();
  connect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
  }
}

void PdfViewer::userZoom()
{
previousScale=currentScale;
scaleDocumentZoom(zoomCustom->text());  
}

void PdfViewer::scaleDocumentZoom(QString zoom)
{
if (!fileLoaded) return;
if (zoom.contains("%")) zoom.remove("%");
if (zoom.toInt() > 0 && zoom.toInt() <= 400)
  {
    qreal s=1;
  currentScale=zoom.toFloat() / 100.0;
  if (currentScale < 0.25) currentScale = 0.25;
  else if (currentScale > 4) currentScale = 4;
  if (previousScale!=0)  s=currentScale/previousScale;
//   int newhpos=(int) (s*(pdfview->horizontalScrollBar()->value()+pdfview->viewport()->width()/2)-pdfview->viewport()->width()/2);
//   int newvpos=(int) (s*(pdfview->verticalScrollBar()->value()+pdfview->viewport()->height()/2)-pdfview->viewport()->height()/2);

  pdfview->setScaleMode(DocumentView::ScaleFactor);
  pdfview->setScaleFactor(s*pdfview->scaleFactor());
  currentScale=pdfview->realScale();
  lastScale=currentScale;
  previousScale=currentScale;
//   pdfview->horizontalScrollBar()->setValue(newhpos);
//   pdfview->verticalScrollBar()->setValue(newvpos);
   clearHistory();
  }
}

void PdfViewer::searchDocument()
{
if (!fileLoaded) return;
//QRectF location;
if (searchLineEdit->text().isEmpty()) return;
disconnect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
//QMetaObject::invokeMethod(this, "searchForwards", Qt::QueuedConnection,Q_ARG(QString,searchLineEdit->text()));
searchForwards(searchLineEdit->text());
connect(pdfview, SIGNAL(currentPageChanged(int)), this, SLOT(checkPage(int)));
//else location = searchBackwards(searchLineEdit->text());
//QPoint target = pdfWidget->matrix().mapRect(location).center().toPoint();
//scrollArea->ensureVisible(target.x(), target.y());
}

/*QRectF PdfViewer::searchBackwards(const QString &text)
{
    QRectF oldLocation = searchLocation;
    QPainterPath path;

    int page = currentPage;
    if (oldLocation.isNull())
        page -= 1;

    while (page > -1) {

        QList<QRectF> locations;
        searchLocation = QRectF();

        while (pdfview->doc()->page(page)->search(text, searchLocation,
            Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {

            if (searchLocation != oldLocation)
                locations.append(searchLocation);
            else
                break;
        }

        int index = locations.indexOf(oldLocation);
        if (index == -1 && !locations.isEmpty()) {
            searchLocation = locations.last();
            //showPage(page + 1);
            return searchLocation;
        } else if (index > 0) {
            searchLocation = locations[index - 1];
            //showPage(page + 1);
            return searchLocation;
        }

        oldLocation = QRectF();
        page -= 1;
    }

    if (currentPage == pdfview->doc()->pageCount() - 1)
        return QRectF();

    oldLocation = QRectF();
    page = pdfview->doc()->pageCount() - 1;

    while (page > currentPage) {

        QList<QRectF> locations;
        searchLocation = QRectF();

        while (pdfview->doc()->page(page)->search(text, searchLocation,
            Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {

            locations.append(searchLocation);
        }

        if (!locations.isEmpty()) {
            searchLocation = locations.last();
            //showPage(page + 1);
            return searchLocation;
        }
        page -= 1;
    }

    return QRectF();
}*/

void PdfViewer::searchForwards(const QString &text)
{
int page = currentPage-1;


while (page < pdfview->doc()->pageCount()) 
  {

  if (currentPage>=1) 
    {
    pdfview->clearPaths(currentPage-1);  
    }
    findItem searchItem =pdfview->doc()->find(page, text, lastSearchPos);
  if (!searchItem.rect.isNull()) 
      {
      lastSearchPos=searchItem.pos;
      path= QPainterPath();
      path.addRect(searchItem.rect);
      QRectF r = path.boundingRect();
      currentPage=page+1;
      lastPage=currentPage;
      pdfview->showRect(currentPage-1,r);
      path.setFillRule(Qt::WindingFill);
      pdfview->setSearchPath(currentPage-1,path);
      updateCurrentPage();
      return;
      }
    page += 1;
    lastSearchPos=-1;
  }
page = 0;
while (page < currentPage-1) 
  {
  if (currentPage>=1) 
    {
    pdfview->clearPaths(currentPage-1);    
    }
lastSearchPos=-1;
findItem searchItem =pdfview->doc()->find(page, text,lastSearchPos);
  if (!searchItem.rect.isNull()) 
      {
      lastSearchPos=searchItem.pos;
      path= QPainterPath();
      path.addRect(searchItem.rect);
      QRectF r = path.boundingRect();
      currentPage=page+1;
      lastPage=currentPage;
      pdfview->showRect(currentPage-1,r);
      path.setFillRule(Qt::WindingFill);
      pdfview->setSearchPath(currentPage-1,path);
      updateCurrentPage();
      return;
      }
  page += 1;
  }
}

void PdfViewer::enableSearch()
{
searchLineEdit->setFocus();
}

void PdfViewer::pageUp()
{
if (!fileLoaded) return;
if (currentPage>1)
  {
  currentPage--;
  lastPage=currentPage;
  gotoPage(currentPage);
  }
}

void PdfViewer::pageDown()
{
if (!fileLoaded) return;
if (currentPage<pdfview->doc()->pageCount())
  {
  currentPage++;
  lastPage=currentPage;
  gotoPage(currentPage);
  }
}

void PdfViewer::fitWidth()
{
if (!fileLoaded) return;
previousScale=currentScale;
pdfview->setScaleMode(DocumentView::FitToPageWidth);
currentScale=pdfview->realScale();
zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
if (currentScale < 0.25) 
  {
  currentScale = 0.25;
  zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
  scaleDocumentZoom(zoomCustom->text());
  }
else if (currentScale > 4) 
  {
  currentScale = 4;
  zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
  scaleDocumentZoom(zoomCustom->text());
  }
lastScale=currentScale;
zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
scaleDocumentZoom(zoomCustom->text());
}

void PdfViewer::fitPage()
{
if (!fileLoaded) return;
previousScale=currentScale;
pdfview->setScaleMode(DocumentView::FitToPageSize);
currentScale=pdfview->realScale();
zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
if (currentScale < 0.25) 
  {
  currentScale = 0.25;
  zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
  scaleDocumentZoom(zoomCustom->text());
  }
else if (currentScale > 4) 
  {
  currentScale = 4;
  zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
  scaleDocumentZoom(zoomCustom->text());
  }
lastScale=currentScale;
zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
scaleDocumentZoom(zoomCustom->text());
}

void PdfViewer::zoomIn()
{
if (!fileLoaded) return;
previousScale=currentScale;
currentScale+=0.1;
if (currentScale > 4) currentScale = 4;
lastScale=currentScale;
zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
scaleDocumentZoom(zoomCustom->text());
}

void PdfViewer::zoomOut()
{
if (!fileLoaded) return;
previousScale=currentScale;
currentScale-=0.1;
if (currentScale < 0.25) currentScale = 0.25;
lastScale=currentScale;
zoomCustom->setText(QString::number(int(currentScale*100)) + "%");
scaleDocumentZoom(zoomCustom->text());
}

void PdfViewer::runExternalViewer()
{
if (!fileLoaded) return;
QString command=viewpdf_command;
command.replace("%.pdf","\""+pdf_file+"\"");
QFileInfo fi(pdf_file);
if (fi.exists()) 
	{
	proc = new QProcess( this );
	proc->setWorkingDirectory(fi.absolutePath());
	proc->start(command);
	}
}




void PdfViewer::printPdf()
{
if (!fileLoaded) return;
QFileInfo fi(pdf_file);
if (!fi.exists()) return; 
QString command;

unsigned int firstPage, lastPage;
QPrinter printer(QPrinter::HighResolution);
QPrintDialog printDlg(&printer, this);
printer.setDocName(fi.baseName());
printDlg.setMinMax(1, pdfview->doc()->pageCount());
printDlg.setFromTo(1, pdfview->doc()->pageCount());
printDlg.setOption(QAbstractPrintDialog::PrintToFile, false);
printDlg.setOption(QAbstractPrintDialog::PrintSelection, false);
printDlg.setOption(QAbstractPrintDialog::PrintPageRange, true);
printDlg.setOption(QAbstractPrintDialog::PrintCollateCopies, true);

printDlg.setWindowTitle(tr("Print"));
if(printDlg.exec() != QDialog::Accepted) return;
switch(printDlg.printRange()) 
  {
  case QAbstractPrintDialog::PageRange:
	  firstPage = printDlg.fromPage();
	  lastPage = printDlg.toPage();
	  break;
  default:
	  firstPage = 1;
	  lastPage = pdfview->doc()->pageCount();
  }

//if(!printer.printerName().isEmpty()) 
//  {
#if defined(Q_OS_WIN32)
pdfview->print(&printer);
#else
  QStringList args;
  args << "lp";
  if(!printer.printerName().isEmpty()) 
  {
  if (!printer.printerName().contains(" ")) args << QString("-d %1").arg(printer.printerName());//.replace(" ","_"));
  }
  args << QString("-n %1").arg(printer.copyCount());
//  args << QString("-t \"%1\"").arg(printer.docName());
  args << QString("-P %1-%2").arg(firstPage).arg(lastPage);
  args << lp_options;
//  args << "-o fitplot";
  switch(printer.duplex()) 
      {
      case QPrinter::DuplexNone:
	      args << "-o sides=one-sided";
	      break;
      case QPrinter::DuplexShortSide:
	      args << "-o sides=two-sided-short-edge";
	      break;
      case QPrinter::DuplexLongSide:
	      args << "-o sides=two-sided-long-edge";
	      break;
      default:
	      break;
      }
  switch(printer.orientation()) 
      {
      case QPrinter::Landscape:
	      args << "-o landscape";
	      break;
      default:
	      break;
      }
  args << "--";
  args << QString("\"%1\"").arg(pdf_file);
  command=args.join(" ");
  if(QProcess::execute(command) == 0) return;
  else pdfview->print(&printer);
#endif
//  }
//else return;
}

void PdfViewer::slotItemClicked(QListWidgetItem* item)
{
QString txt=item->text().section(" ",-1);
currentPage=txt.toInt();
gotoPage(currentPage);
}

void PdfViewer::jumpToEditor(int page, const QPointF& pos)
{
if (scanner == NULL) return;
if (synctex_edit_query(scanner, page+1, pos.x(), pos.y()) > 0) 
  {
  synctex_node_t node;
  while ((node = synctex_next_result(scanner)) != NULL) 
    {
    QString filename = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
    QDir curDir(QFileInfo(pdf_file).canonicalPath());
    emit openDocAtLine(QFileInfo(curDir, filename).canonicalFilePath(),synctex_node_line(node),false);
    break;
    }
  }
}

void PdfViewer::setKeyEditorFocus(QKeySequence s)
{
KeySequenceEditorFocus=s;
}

void PdfViewer::setGSCommand(QString c)
{
gswin32c_command=c;
}

void PdfViewer::keyPressEvent ( QKeyEvent * e ) 
{
int qtKeyCode = e->key();
if(e->modifiers() & Qt::ShiftModifier) {
	qtKeyCode += Qt::SHIFT;
}
if(e->modifiers() & Qt::ControlModifier) {
	qtKeyCode += Qt::CTRL;
}
if(e->modifiers() & Qt::AltModifier) {
	qtKeyCode += Qt::ALT;
}
if(e->modifiers() & Qt::MetaModifier) {
	qtKeyCode += Qt::META;
}
QKeySequence s1 = QKeySequence(qtKeyCode);
if (s1.matches(KeySequenceEditorFocus)==QKeySequence::ExactMatch) emit sendFocusToEditor();
else QMainWindow::keyPressEvent(e);
}

void PdfViewer::ShowStructure()
{
showingListPages=false;
LeftPanelStackedWidget->setCurrentWidget(StructureTreeView);
StructureView->setWindowTitle(tr("Structure"));
}

void PdfViewer::ShowListPages()
{
showingListPages=true;
LeftPanelStackedWidget->setCurrentWidget(listpagesWidget);
StructureView->setWindowTitle(tr("Pages"));
}


void PdfViewer::ClickedOnStructure(const QModelIndex& index)
{
if (!fileLoaded) return;
if (!index.isValid()) return;

int page = index.data(QPdfBookmarkModel::PageNumberRole).toInt()+1;
        qreal left = 0.0;
        qreal top = 0.0;
	qreal destLeft=0;
	qreal destTop=0;
        page = page >= 1 ? page : 1;
        page = page <= pdfview->doc()->pageCount() ? page : pdfview->doc()->pageCount();

//         if(linkDestination->isChangeLeft())
//         {
//             left = linkDestination->left();
// 	    destLeft = left * m_document->page(page-1)->pageSizeF().width();
//             left = left >= 0.0 ? left : 0.0;
//             left = left <= 1.0 ? left : 1.0;
//         }
// 
//         if(linkDestination->isChangeTop())
//         {
//             top = linkDestination->top();
// 	    destTop = top  * m_document->page(page-1)->pageSizeF().height();
//             top = top >= 0.0 ? top : 0.0;
//             top = top <= 1.0 ? top : 1.0;
//         }
// int page = StructureTreeView->model()->data(index, Qt::UserRole + 1).toInt(&ok);
// qreal left = StructureTreeView->model()->data(index, Qt::UserRole + 2).toReal();
// qreal top = StructureTreeView->model()->data(index, Qt::UserRole + 3).toReal();
// qreal destLeft=StructureTreeView->model()->data(index, Qt::UserRole + 4).toReal();
// qreal destTop=StructureTreeView->model()->data(index, Qt::UserRole + 5).toReal();
// if(ok) 
//   {
   jumpToDest(page, left, top);
// 
   jumpToEditor(page-1,QPointF(destLeft,destTop));
}

void PdfViewer::historyBack()
{
if (stack.count() <= 1) return;
// Update the history entry
forwardStack.push(pdfview->verticalScrollBar()->value());
stack.pop(); // throw away the old version of the current entry
pdfview->verticalScrollBar()->setValue(stack.top()); // previous entry
emit backwardAvailable(stack.count() > 1);
emit forwardAvailable(true);
}

void PdfViewer::historyForward()
{
if (forwardStack.isEmpty()) return;
if (!stack.isEmpty()) 
  {
    // Update the history entry
    stack.top() =pdfview->verticalScrollBar()->value();
  }
stack.push(forwardStack.pop());
pdfview->verticalScrollBar()->setValue(stack.top()); // previous entry
emit backwardAvailable(true);
emit forwardAvailable(!forwardStack.isEmpty());
}

void PdfViewer::clearHistory()
{
forwardStack.clear();
if (!stack.isEmpty()) 
  {
  int tmp=stack.top();
  stack.resize(0);
  stack.push(tmp);
  }
emit forwardAvailable(false);
emit backwardAvailable(false);
}

void PdfViewer::updateHistory(int pos)
{
if (!stack.isEmpty() && stack.top()==pos) return;
stack.push(pos);
emit backwardAvailable(stack.count() > 1);
emit forwardAvailable(!forwardStack.isEmpty());
}

void PdfViewer::setHpos(int pos)
{
lastHpos=pos;
}

void PdfViewer::jumptoHpos()
{
pdfview->horizontalScrollBar()->setValue(lastHpos);
}

void PdfViewer::on_continuousMode_triggered(bool checked)
{
if (!fileLoaded) return;
pdfview->setContinousMode(checked);
}

void PdfViewer::on_twoPagesMode_triggered(bool checked)
{
if (!fileLoaded) return;
pdfview->setTwoPagesMode(checked);
}

void PdfViewer::on_rotateLeft_triggered()
{
if (!fileLoaded) return;
pdfview->rotateLeft();
}

void PdfViewer::on_rotateRight_triggered()
{
if (!fileLoaded) return;
pdfview->rotateRight();
}

void PdfViewer::on_presentation_triggered()
{
if (!fileLoaded) return;
pdfview->presentation();
}

void PdfViewer::on_currentTab_continuousModeChanged(bool continuousMode)
{
continuousModeAction->setChecked(continuousMode);
islastContinuous=continuousMode;
}

void PdfViewer::on_currentTab_twoPagesModeChanged(bool twoPagesMode)
{
twoPagesModeAction->setChecked(twoPagesMode);
}
