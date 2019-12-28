/***************************************************************************
 *   copyright       : (C) 2012-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *   based on qpdfview  Copyright 2012 Adam Reichold GPL                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*

Copyright 2012 Adam Reichold

This file is part of qpdfview.

modified by Pascal Brachet

*/

#include "documentview.h"
#include <QProgressDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QProgressBar>
#include <QScrollBar>
#include <QApplication>
#include <QInputDialog>
#include <QLineEdit>

bool DocumentView::s_openUrl = false;

bool DocumentView::s_autoRefresh = false;

bool DocumentView::s_antialiasing = true;
bool DocumentView::s_textAntialiasing = true;
bool DocumentView::s_textHinting = false;

bool DocumentView::s_prefetch = false;

qreal DocumentView::s_pageSpacing = 5.0;

qreal DocumentView::s_minimumScaleFactor = 0.1;
qreal DocumentView::s_maximumScaleFactor = 10.0;

bool DocumentView::openUrl()
{
    return s_openUrl;
}

void DocumentView::setOpenUrl(bool openUrl)
{
    s_openUrl = openUrl;
}

bool DocumentView::autoRefresh()
{
    return s_autoRefresh;
}

void DocumentView::setAutoRefresh(bool autoRefresh)
{
    s_autoRefresh = autoRefresh;
}

bool DocumentView::antialiasing()
{
    return s_antialiasing;
}

void DocumentView::setAntialiasing(bool antialiasing)
{
    s_antialiasing = antialiasing;
}

bool DocumentView::textAntialiasing()
{
    return s_textAntialiasing;
}

void DocumentView::setTextAntialiasing(bool textAntialiasing)
{
    s_textAntialiasing = textAntialiasing;
}

bool DocumentView::textHinting()
{
    return s_textHinting;
}

void DocumentView::setTextHinting(bool textHinting)
{
    s_textHinting = textHinting;
}

bool DocumentView::prefetch()
{
    return s_prefetch;
}

void DocumentView::setPrefetch(bool prefetch)
{
    s_prefetch = prefetch;
}

qreal DocumentView::pageSpacing()
{
    return s_pageSpacing;
}

void DocumentView::setPageSpacing(qreal pageSpacing)
{
    if(pageSpacing >= 0.0)
    {
        s_pageSpacing = pageSpacing;
    }
}

qreal DocumentView::minimumScaleFactor()
{
    return s_minimumScaleFactor;
}

qreal DocumentView::maximumScaleFactor()
{
    return s_maximumScaleFactor;
}

DocumentView::DocumentView(QWidget* parent) : QGraphicsView(parent),
    m_autoRefreshWatcher(0),
    m_autoRefreshTimer(0),
    m_prefetchTimer(0),
    m_mutex(),
    m_document(0),
    m_filePath(),
    m_numberOfPages(-1),
    m_currentPage(-1),
    m_returnToPage(-1),
    m_returnToLeft(0.0),
    m_returnToTop(0.0),
    m_continuousMode(false),
    m_twoPagesMode(false),
    m_scaleMode(ScaleFactor),
    m_scaleFactor(0.1),
    m_rotation(QPdf::Rotate0),
    m_pagesScene(0),
    m_pages(),
    m_heightToIndex(),
    m_highlight(0),
    m_outlineModel(0),
    m_propertiesModel(0)
{
    m_pagesScene = new QGraphicsScene(this);
    m_outlineModel = new QPdfBookmarkModel(this);
    m_propertiesModel = new QStandardItemModel(this);

    m_filePath="";
    m_currentPage=-1;
    m_returnToPage=-1;
    
    setScene(m_pagesScene);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setCursor(Qt::ArrowCursor);
//    setLineWidth(1);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
//QApplication::setOverrideCursor(Qt::ArrowCursor);
    setAcceptDrops(false);

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this,SLOT(on_verticalScrollBar_valueChanged(int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this,SLOT(on_horizontalScrollBar_valueChanged(int)));

    // highlight

    m_highlight = new QGraphicsRectItem();

    m_highlight->setVisible(false);
    scene()->addItem(m_highlight);

    QColor highlightColor = QApplication::palette().color(QPalette::Highlight);

    highlightColor.setAlpha(127);
    m_highlight->setBrush(QBrush(highlightColor));

    highlightColor.setAlpha(255);
    m_highlight->setPen(QPen(highlightColor));


    // auto-refresh

    m_autoRefreshWatcher = new QFileSystemWatcher(this);
    m_autoRefreshTimer = new QTimer(this);

    m_autoRefreshTimer->setInterval(500);
    m_autoRefreshTimer->setSingleShot(true);

    connect(m_autoRefreshWatcher, SIGNAL(fileChanged(QString)), m_autoRefreshTimer, SLOT(start()));
    connect(m_autoRefreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));

    // prefetch

    m_prefetchTimer = new QTimer(this);
    m_prefetchTimer->setInterval(250);
    m_prefetchTimer->setSingleShot(true);

    connect(this, SIGNAL(currentPageChanged(int)), m_prefetchTimer, SLOT(start()));
    connect(this, SIGNAL(scaleModeChanged(DocumentView::ScaleMode)), m_prefetchTimer, SLOT(start()));
    connect(this, SIGNAL(scaleFactorChanged(qreal)), m_prefetchTimer, SLOT(start()));
    connect(this, SIGNAL(rotationChanged(QPdf::Rotation)), m_prefetchTimer, SLOT(start()));

    connect(m_prefetchTimer, SIGNAL(timeout()), this,SLOT(on_prefetch_timeout()));
}

DocumentView::~DocumentView()
{
    qDeleteAll(m_pages);


}

void DocumentView::clearAll()
{
disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this,SLOT(on_verticalScrollBar_valueChanged(int)));
disconnect(horizontalScrollBar(), SIGNAL(valueChanged(int)),this, SLOT(on_horizontalScrollBar_valueChanged(int)));
disconnect(m_autoRefreshWatcher, SIGNAL(fileChanged(QString)), m_autoRefreshTimer, SLOT(start()));
disconnect(m_autoRefreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
disconnect(this, SIGNAL(currentPageChanged(int)), m_prefetchTimer, SLOT(start()));
disconnect(this, SIGNAL(scaleModeChanged(DocumentView::ScaleMode)), m_prefetchTimer, SLOT(start()));
disconnect(this, SIGNAL(scaleFactorChanged(qreal)), m_prefetchTimer, SLOT(start()));
disconnect(this, SIGNAL(rotationChanged(QPdf::Rotation)), m_prefetchTimer, SLOT(start()));
disconnect(m_prefetchTimer, SIGNAL(timeout()), this,SLOT(on_prefetch_timeout()));
m_pages.clear();
m_numberOfPages=-1;
scene()->clear();
m_pagesScene = new QGraphicsScene(this);
m_filePath="";
m_currentPage=-1;
m_returnToPage=-1;
setScene(m_pagesScene);
m_highlight = new QGraphicsRectItem();
m_highlight->setVisible(false);
scene()->addItem(m_highlight);
prepareScene();
prepareView(0,0);
QColor highlightColor = QApplication::palette().color(QPalette::Highlight);
highlightColor.setAlpha(127);
m_highlight->setBrush(QBrush(highlightColor));
highlightColor.setAlpha(255);
m_highlight->setPen(QPen(highlightColor));
update();
connect(verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(on_verticalScrollBar_valueChanged(int)));
connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(on_horizontalScrollBar_valueChanged(int)));
connect(m_autoRefreshWatcher, SIGNAL(fileChanged(QString)), m_autoRefreshTimer, SLOT(start()));
connect(m_autoRefreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
connect(this, SIGNAL(currentPageChanged(int)), m_prefetchTimer, SLOT(start()));
connect(this, SIGNAL(scaleModeChanged(DocumentView::ScaleMode)), m_prefetchTimer, SLOT(start()));
connect(this, SIGNAL(scaleFactorChanged(qreal)), m_prefetchTimer, SLOT(start()));
connect(this, SIGNAL(rotationChanged(QPdf::Rotation)), m_prefetchTimer, SLOT(start()));
connect(m_prefetchTimer, SIGNAL(timeout()), SLOT(on_prefetch_timeout()));
}

const QString& DocumentView::filePath() const
{
    return m_filePath;
}

int DocumentView::numberOfPages() const
{
    return m_numberOfPages;
}

int DocumentView::currentPage() const
{
    return m_currentPage;
}

bool DocumentView::continousMode() const
{
    return m_continuousMode;
}

void DocumentView::setContinousMode(bool continousMode)
{
if (m_pages.count()<1) return;
    if(m_continuousMode != continousMode)
    {
        m_continuousMode = continousMode;

        qreal left = 0.0, top = 0.0;
        saveLeftAndTop(left, top);

        prepareView(left, top);

        emit continousModeChanged(m_continuousMode);
    }
}

bool DocumentView::twoPagesMode() const
{
    return m_twoPagesMode;
}

void DocumentView::setTwoPagesMode(bool twoPagesMode)
{
if (m_pages.count()<1) return;
    if(m_twoPagesMode != twoPagesMode)
    {
        m_twoPagesMode = twoPagesMode;

        if(m_twoPagesMode)
        {
            if(m_currentPage != (m_currentPage % 2 != 0 ? m_currentPage :  m_currentPage - 1))
            {
                m_currentPage = m_currentPage % 2 != 0 ? m_currentPage :  m_currentPage - 1;

                emit currentPageChanged(m_currentPage);
            }
        }

        prepareScene();
        prepareView();

        emit twoPagesModeChanged(m_twoPagesMode);
    }
}

DocumentView::ScaleMode DocumentView::scaleMode() const
{
    return m_scaleMode;
}

void DocumentView::setScaleMode(ScaleMode scaleMode)
{
if (m_pages.count()<1) return;
    if(m_scaleMode != scaleMode)
    {
        m_scaleMode = scaleMode;

        qreal left = 0.0, top = 0.0;
        saveLeftAndTop(left, top);

        prepareScene();
        prepareView(left, top);

        emit scaleModeChanged(m_scaleMode);
    }
}

qreal DocumentView::scaleFactor() const
{
    return m_scaleFactor;
}

qreal DocumentView::realScale() const
{
if (m_pages.count()<1) return 1;
return m_pages.at(0)->scaleFactor();
}

void DocumentView::setScaleFactor(qreal scaleFactor)
{
if (m_pages.count()<1) return;
    if(m_scaleFactor != scaleFactor && scaleFactor >= s_minimumScaleFactor && scaleFactor <= s_maximumScaleFactor)
    {
        m_scaleFactor = scaleFactor;

        if(m_scaleMode == ScaleFactor)
        {
            qreal left = 0.0, top = 0.0;
            saveLeftAndTop(left, top);

            prepareScene();
            prepareView(left, top);
// #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
// 	    viewport()->update();
// #endif
        }

        emit scaleFactorChanged(m_scaleFactor);
    }
}

QPdfDocument* DocumentView::doc() const
{
   return m_document;
}

QPdf::Rotation DocumentView::rotation() const
{
    return m_rotation;
}

void DocumentView::setRotation(QPdf::Rotation rotation)
{
if (m_pages.count()<1) return;
    if(m_rotation != rotation)
    {
        m_rotation = rotation;

        prepareScene();
        prepareView();

        emit rotationChanged(m_rotation);
    }
}


void DocumentView::clearPaths(int index)
{
if ((m_pages.count()<1) || (index>=m_pages.count())) return;
m_pages.at(index)->clearPaths();
}

void DocumentView::setHighlightPath(int index, const QPainterPath& path)
{
if ((m_pages.count()<1) || (index>=m_pages.count())) return;
m_pages.at(index)->setHighlightPath(path);
}

void DocumentView::setSearchPath(int index, const QPainterPath& path)
{
if ((m_pages.count()<1) || (index>=m_pages.count())) return;
m_pages.at(index)->setSearchPath(path);
}

QPdfBookmarkModel* DocumentView::outlineModel() const
{
    return m_outlineModel;
}

QStandardItemModel* DocumentView::propertiesModel() const
{
    return m_propertiesModel;
}



void DocumentView::show()
{
    QGraphicsView::show();

    prepareView();
}

bool DocumentView::open(const QString& filePath)
{
    QPdfDocument* document=new QPdfDocument(this);
    QPdfDocument::DocumentError result=document->load(filePath);

    if (result== QPdfDocument::NoError) 
    {
        m_filePath = filePath;
        m_numberOfPages = document->pageCount();
        m_currentPage = 1;
        m_returnToPage = -1;
        prepareDocument(document);
        prepareScene();
        prepareView();
        emit filePathChanged(m_filePath);
        emit numberOfPagesChanged(m_numberOfPages);
        emit currentPageChanged(m_currentPage);
    }

    return result== QPdfDocument::NoError ;
}

bool DocumentView::refresh()
{
    QPdfDocument* document=new QPdfDocument(this);
    QPdfDocument::DocumentError result=document->load(m_filePath);

    if (result== QPdfDocument::NoError) 
    {

        qreal left = 0.0, top = 0.0;
        saveLeftAndTop(left, top);

        m_numberOfPages = document->pageCount();
        m_currentPage = m_currentPage <= m_numberOfPages ? m_currentPage : 1;

        m_returnToPage = m_returnToPage <= m_numberOfPages ? m_returnToPage : -1;

        prepareDocument(document);

        prepareScene();
        prepareView(left, top);

        emit numberOfPagesChanged(m_numberOfPages);
        emit currentPageChanged(m_currentPage);
    }

    return result== QPdfDocument::NoError ;
}

bool DocumentView::print(QPrinter* printer)
{
    int fromPage = printer->fromPage() != 0 ? printer->fromPage() : 1;
    int toPage = printer->toPage() != 0 ? printer->toPage() : m_numberOfPages;

    QProgressDialog* progressDialog = new QProgressDialog(this);
    progressDialog->setLabelText(tr("Printing '%1'...").arg(m_filePath));
    progressDialog->setRange(fromPage - 1, toPage);

    QPainter painter;
    painter.begin(printer);

    for(int index = fromPage - 1; index <= toPage - 1; index++)
    {
        progressDialog->setValue(index);

        QApplication::processEvents();

        {
            m_mutex.lock();

            const QSizeF size = m_document->pageSize(index);

            qreal pageWidth =  printer->physicalDpiX() / 72.0 * size.width();
            qreal pageHeight = printer->physicalDpiY() / 72.0 * size.height();
            
            ////????????????????????????
            QImage image = m_document->render(index,QSize(size.width()*printer-> physicalDpiX()/72,size.height()*printer-> physicalDpiY()/72));

            //QImage image = page->renderToImage(printer->physicalDpiX(), printer->physicalDpiY());

            

            m_mutex.unlock();

            qreal scaleFactor = qMin(printer->width() / pageWidth, printer->height() / pageHeight);

            painter.setTransform(QTransform::fromScale(scaleFactor, scaleFactor));
            painter.drawImage(QPointF(), image);
        }

        if(index < toPage - 1)
        {
            printer->newPage();
        }

        QApplication::processEvents();

        if(progressDialog->wasCanceled())
        {
            delete progressDialog;
            return false;
        }
    }

    painter.end();

    delete progressDialog;
    return true;
}

void DocumentView::previousPage()
{
if (m_pages.count()<1) return;
    jumpToPage(currentPage() - (m_twoPagesMode ? 2 : 1), 0.0, 0.0, false);
}

void DocumentView::nextPage()
{
if (m_pages.count()<1) return;
    jumpToPage(currentPage() + (m_twoPagesMode ? 2 : 1), 0.0, 0.0, false);
}

void DocumentView::firstPage()
{
if (m_pages.count()<1) return;
    jumpToPage(1);
}

void DocumentView::lastPage()
{
if (m_pages.count()<1) return;
    jumpToPage(numberOfPages());
}

void DocumentView::jumpToPage(int page, qreal changeLeft, qreal changeTop, bool returnTo)
{
if (m_pages.count()<1) return;
    if(page >= 1 && page <= m_numberOfPages)
    {
        qreal left = 0.0, top = 0.0;
        saveLeftAndTop(left, top);

        if(m_currentPage != currentPageForPage(page) || !qFuzzyCompare(1.0 + left, 1.0 + changeLeft) || !qFuzzyCompare(1.0 + top, 1.0 + changeTop))
        {
            if(returnTo)
            {
                m_returnToPage = m_currentPage;
                m_returnToLeft = left; m_returnToTop = top;
            }

            m_currentPage = currentPageForPage(page);

            prepareView(changeLeft, changeTop);

            emit currentPageChanged(m_currentPage);
        }
    }
}



void DocumentView::zoomIn()
{
if (m_pages.count()<1) return;
    if(scaleMode() != ScaleFactor)
    {
        PageItem* page = m_pages.at(m_currentPage - 1);

        setScaleFactor(qMin(page->scaleFactor() + 0.1f, s_maximumScaleFactor));
        setScaleMode(ScaleFactor);
    }
    else
    {
        setScaleFactor(qMin(scaleFactor() + 0.1f, s_maximumScaleFactor));
    }
}

void DocumentView::zoomOut()
{
if (m_pages.count()<1) return;
    if(scaleMode() != ScaleFactor)
    {
        PageItem* page = m_pages.at(m_currentPage - 1);

        setScaleFactor(qMax(page->scaleFactor() - 0.1f, s_minimumScaleFactor));
        setScaleMode(ScaleFactor);
    }
    else
    {
        setScaleFactor(qMax(scaleFactor() - 0.1f, s_minimumScaleFactor));
    }
}

void DocumentView::originalSize()
{
    setScaleFactor(1.0);
    setScaleMode(ScaleFactor);
}

void DocumentView::rotateLeft()
{
    switch(rotation())
    {
    case QPdf::Rotate0:
        setRotation(QPdf::Rotate270);
        break;
    case QPdf::Rotate90:
        setRotation(QPdf::Rotate0);
        break;
    case QPdf::Rotate180:
        setRotation(QPdf::Rotate90);
        break;
    case QPdf::Rotate270:
        setRotation(QPdf::Rotate180);
        break;
    }
}

void DocumentView::rotateRight()
{
    switch(rotation())
    {
    case QPdf::Rotate0:
        setRotation(QPdf::Rotate90);
        break;
    case QPdf::Rotate90:
        setRotation(QPdf::Rotate180);
        break;
    case QPdf::Rotate180:
        setRotation(QPdf::Rotate270);
        break;
    case QPdf::Rotate270:
        setRotation(QPdf::Rotate0);
        break;
    }
}

void DocumentView::presentation()
{
if (m_pages.count()<1) return;
    PresentationView* presentationView = new PresentationView(&m_mutex, m_document);

    presentationView->jumpToPage(currentPage());

    presentationView->show();
    presentationView->setAttribute(Qt::WA_DeleteOnClose);
}

void DocumentView::on_horizontalScrollBar_valueChanged(int value)
{
if (m_pages.count()<1) return;
if ( horizontalScrollBar()->maximum()!=0) emit doHScroll(value); 
}

void DocumentView::on_verticalScrollBar_valueChanged(int value)
{
if (m_pages.count()<1) return;
    if(m_continuousMode)
    {
        QRectF visibleRect = mapToScene(viewport()->rect()).boundingRect();

        foreach(PageItem* page, m_pages)
        {
            if(!page->boundingRect().translated(page->pos()).intersects(visibleRect))
            {
                page->cancelRender();
            }
        }

        QMap< qreal, int >::const_iterator lowerBound = m_heightToIndex.lowerBound(-value);

        if(lowerBound != m_heightToIndex.end())
        {
            int page = lowerBound.value() + 1;

            if(m_currentPage != page)
            {
                m_currentPage = page;

                emit currentPageChanged(m_currentPage);
            }
        }
    }
}


void DocumentView::on_prefetch_timeout()
{
if (m_pages.count()<1) return;
    int fromPage = m_currentPage - (m_twoPagesMode ? 2 : 1);
    int toPage = m_currentPage + (m_twoPagesMode ? 3 : 1);

    fromPage = fromPage >= 1 ? fromPage : 1;
    toPage = toPage <= m_numberOfPages ? toPage : m_numberOfPages;

    for(int index = fromPage - 1; index <= toPage - 1; index++)
    {
        PageItem* page = m_pages.at(index);

        page->startRender(true);
    }
}

void DocumentView::on_pages_linkClicked(int page, qreal left, qreal top)
{
if (m_pages.count()<1) return;
    page = page >= 1 ? page : 1;
    page = page <= m_numberOfPages ? page : m_numberOfPages;

    left = left >= 0.0 ? left : 0.0;
    left = left <= 1.0 ? left : 1.0;

    top = top >= 0.0 ? top : 0.0;
    top = top <= 1.0 ? top : 1.0;

    emit gotoDest(page, left, top);
}

void DocumentView::on_pages_linkClicked(const QString& url)
{
	QUrl theurl = QUrl::fromEncoded(url.toLatin1());
	QDesktopServices::openUrl(theurl);
}

void DocumentView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    if(m_scaleMode != ScaleFactor)
    {
        qreal left = 0.0, top = 0.0;
        saveLeftAndTop(left, top);

        prepareScene();
        prepareView(left, top);
    }
}

void DocumentView::keyPressEvent(QKeyEvent* event)
{
if (m_pages.count()<1) return;
    if(event->modifiers() == Qt::NoModifier)
    {
        if(event->key() == Qt::Key_Return)
        {
            jumpToPage(m_returnToPage, m_returnToLeft, m_returnToTop);

            event->accept();
            return;
        }

        if(!m_continuousMode)
        {
            if(event->key() == Qt::Key_PageUp && verticalScrollBar()->value() == verticalScrollBar()->minimum() && m_currentPage > 1)
            {
                previousPage();

                verticalScrollBar()->setValue(verticalScrollBar()->maximum());

                event->accept();
                return;
            }
            else if(event->key() == Qt::Key_PageDown && verticalScrollBar()->value() == verticalScrollBar()->maximum() && m_currentPage != currentPageForPage(m_numberOfPages))
            {
                nextPage();

                verticalScrollBar()->setValue(verticalScrollBar()->minimum());

                event->accept();
                return;
            }
        }
    }
int qtKeyCode = event->key();

if(event->modifiers() & Qt::ShiftModifier) {
	qtKeyCode += Qt::SHIFT;
}
if(event->modifiers() & Qt::ControlModifier) {
	qtKeyCode += Qt::CTRL;
}
if(event->modifiers() & Qt::AltModifier) {
	qtKeyCode += Qt::ALT;
}
if(event->modifiers() & Qt::MetaModifier) {
	qtKeyCode += Qt::META;
}
QKeySequence s1 = QKeySequence(qtKeyCode);
if (hasFocus() && (s1.matches(QKeySequence(Qt::CTRL + Qt::Key_Left))==QKeySequence::ExactMatch)) rotateLeft();
else if (hasFocus() && (s1.matches(QKeySequence(Qt::CTRL + Qt::Key_Right))==QKeySequence::ExactMatch)) rotateRight();
else QGraphicsView::keyPressEvent(event);
}


void DocumentView::mouseReleaseEvent(QMouseEvent* event)
{
  QApplication::setOverrideCursor(Qt::ArrowCursor);
    QGraphicsView::mouseReleaseEvent(event);
}

void DocumentView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    //QApplication::setOverrideCursor(Qt::ArrowCursor);

//     if(!event->isAccepted() && event->modifiers() == Qt::NoModifier && event->button() == Qt::RightButton)
//     {
//         QMenu* menu = new QMenu();
// 
//         QAction* returnToPageAction = menu->addAction(tr("&Return to page %1").arg(m_returnToPage));
//         returnToPageAction->setShortcut(QKeySequence(Qt::Key_Return));
//         returnToPageAction->setIcon(QIcon::fromTheme("go-jump", QIcon(":icons/go-jump.svg")));
//         returnToPageAction->setIconVisibleInMenu(true);
// 
//         returnToPageAction->setVisible(m_returnToPage != -1);
// 
//         QAction* previousPageAction = menu->addAction(tr("&Previous page"));
//         previousPageAction->setShortcut(QKeySequence(Qt::Key_Backspace));
//         previousPageAction->setIcon(QIcon::fromTheme("go-previous", QIcon(":icons/go-previous.svg")));
//         previousPageAction->setIconVisibleInMenu(true);
// 
//         QAction* nextPageAction = menu->addAction(tr("&Next page"));
//         nextPageAction->setShortcut(QKeySequence(Qt::Key_Space));
//         nextPageAction->setIcon(QIcon::fromTheme("go-next", QIcon(":icons/go-next.svg")));
//         nextPageAction->setIconVisibleInMenu(true);
// 
//         QAction* refreshAction = menu->addAction(tr("&Refresh"));
//         refreshAction->setShortcut(QKeySequence::Refresh);
//         refreshAction->setIcon(QIcon::fromTheme("view-refresh", QIcon(":icons/view-refresh.svg")));
//         refreshAction->setIconVisibleInMenu(true);
// 
//         QAction* action = menu->exec(event->globalPos());
// 
//         if(action == returnToPageAction)
//         {
//             jumpToPage(m_returnToPage, m_returnToLeft, m_returnToTop);
//         }
//         else if(action == previousPageAction)
//         {
//             previousPage();
//         }
//         else if(action == nextPageAction)
//         {
//             nextPage();
//         }
//         else if(action == refreshAction)
//         {
//             refresh();
//         }
// 
//         delete menu;
//     }
}

void DocumentView::wheelEvent(QWheelEvent* event)
{
if (m_pages.count()<1) return;
    if(event->modifiers() == Qt::ControlModifier)
    {
        if(event->delta() > 0)
        {
            emit requestZoomIn();//zoomIn();
        }
        else
        {
            emit requestZoomOut();//zoomOut();
        }

        event->accept();
        return;
    }
    else if(event->modifiers() == Qt::ShiftModifier)
    {
        if(event->delta() > 0)
        {
            rotateLeft();
        }
        else
        {
            rotateRight();
        }

        event->accept();
        return;
    }
    else if(event->modifiers() == Qt::NoModifier)
    {
        if(!m_continuousMode)
        {
            if(event->delta() > 0 && verticalScrollBar()->value() == verticalScrollBar()->minimum() && m_currentPage > 1)
            {
                previousPage();

                verticalScrollBar()->setValue(verticalScrollBar()->maximum());

                event->accept();
                return;
            }
            else if(event->delta() < 0 && verticalScrollBar()->value() == verticalScrollBar()->maximum() && m_currentPage != currentPageForPage(m_numberOfPages))
            {
                nextPage();

                verticalScrollBar()->setValue(verticalScrollBar()->minimum());

                event->accept();
                return;
            }
        }
    }

    QGraphicsView::wheelEvent(event);
}

int DocumentView::currentPageForPage(int page)
{
    if(m_twoPagesMode)
    {
        return page % 2 != 0 ? page : page - 1;
    }
    else
    {
        return page;
    }
}

void DocumentView::saveLeftAndTop(qreal& left, qreal& top)
{
if ((m_pages.count()>0) && (m_currentPage>=1))
    {
    PageItem* page = m_pages.at(m_currentPage - 1);

    QRectF boundingRect = page->boundingRect().translated(page->pos());
    QPointF topLeft = mapToScene(viewport()->rect().topLeft());

    left = (topLeft.x() - boundingRect.x()) / boundingRect.width();
    top = (topLeft.y() - boundingRect.y()) / boundingRect.height();
    }
}

void DocumentView::prepareDocument(QPdfDocument* document)
{
    m_prefetchTimer->blockSignals(true);
    m_prefetchTimer->stop();

    qDeleteAll(m_pages);

 

    if(m_document != 0)
    {
        delete m_document;

        if(!m_autoRefreshWatcher->files().isEmpty())
        {
            m_autoRefreshWatcher->removePaths(m_autoRefreshWatcher->files());
        }
    }

    m_document = document;

    if(s_autoRefresh)
    {
        m_autoRefreshWatcher->addPath(m_filePath);
    }

//     m_document->setRenderHint(Poppler::Document::Antialiasing, s_antialiasing);
//     m_document->setRenderHint(Poppler::Document::TextAntialiasing, s_textAntialiasing);
//     m_document->setRenderHint(Poppler::Document::TextHinting, s_textHinting);

    preparePages();
    prepareOutline();
    prepareProperties();

    if(s_prefetch)
    {
        m_prefetchTimer->blockSignals(false);
        m_prefetchTimer->start();
    }
}

void DocumentView::preparePages()
{
    m_pages.clear();
    m_pages.reserve(m_numberOfPages);
       
   QProgressDialog* progressDialog = new QProgressDialog(this);
//     QProgressBar* progress = new QProgressBar();
//     progress->setFormat(" ");
//     progress->setMaximum(0);
//     progress->setMinimum(0);
//    progressDialog->setBar(progress);
    progressDialog->setLabelText(tr("Loading pdf..."));
    progressDialog->setCancelButton(0);
    progressDialog->setRange(0,m_numberOfPages-1);
//    progressDialog->setMinimumDuration(0);
//    progressDialog->show();
//    progressDialog->raise();

    for(int index = 0; index < m_numberOfPages; index++)
    {
      progressDialog->setValue(index);
       //QApplication::processEvents();
        PageItem* page = new PageItem(&m_mutex, m_document, index);
const int dpr = int(qApp->devicePixelRatio());
if (qApp->devicePixelRatio()>=2) page->setPhysicalDpi(physicalDpiX()*dpr, physicalDpiY()*dpr);
else page->setPhysicalDpi(physicalDpiX(), physicalDpiY());

        m_pagesScene->addItem(page);
        m_pages.append(page);

        connect(page, SIGNAL(linkClicked(int,qreal,qreal)), SLOT(on_pages_linkClicked(int,qreal,qreal)));
        connect(page, SIGNAL(linkClicked(QString)), SLOT(on_pages_linkClicked(QString)));
	connect(page, SIGNAL(syncpage(int, const QPointF&)), this, SLOT(on_syncpage(int, const QPointF&)));
	connect(page, SIGNAL(wantPngExport(int)), this, SLOT(pngExport(int)));
	connect(page, SIGNAL(wantNumWords()), this, SLOT(countWords()));
	connect(page, SIGNAL(wantNumPageWords(int)), this, SLOT(countPageWords(int)));
	connect(page, SIGNAL(wantOpenLocation()), this, SLOT(openFileBrowser()));
    }

progressDialog->cancel();
delete progressDialog;
    if(PageItem::decoratePages())
    {
        m_pagesScene->setBackgroundBrush(QBrush(Qt::darkGray));
    }
    else
    {
        m_pagesScene->setBackgroundBrush(QBrush(PageItem::invertColors() ? Qt::black : Qt::white));
    }
}


void DocumentView::prepareOutline()
{
     //m_outlineModel->clear();
     m_outlineModel->setDocument(m_document);
}



void DocumentView::prepareProperties()
{
//     m_propertiesModel->clear();
// 
//     QStringList keys = m_document->infoKeys();
// 
//     m_propertiesModel->setRowCount(keys.count());
//     m_propertiesModel->setColumnCount(2);
// 
//     for(int index = 0; index < keys.count(); index++)
//     {
//         QString key = keys.at(index);
//         QString value = m_document->info(key);
// 
//         if(value.startsWith("D:"))
//         {
//             value = m_document->date(key).toString();
//         }
// 
//         m_propertiesModel->setItem(index, 0, new QStandardItem(key));
//         m_propertiesModel->setItem(index, 1, new QStandardItem(value));
//     }
}

void DocumentView::prepareScene()
{
    // prepare scale factor and rotation

    for(int index = 0; index < m_numberOfPages; index++)
    {
        PageItem* page = m_pages.at(index);
        QSizeF size = page->size();

        if(m_scaleMode != ScaleFactor)
        {
            qreal visibleWidth = 0.0;
            qreal visibleHeight = 0.0;

            qreal pageWidth = 0.0;
            qreal pageHeight = 0.0;

            qreal scaleFactor = 1.0;

            if(m_twoPagesMode)
            {
                visibleWidth = 0.5 * (viewport()->width() - 6.0 - 3.0 * s_pageSpacing);
            }
            else
            {
                visibleWidth = viewport()->width() - 6.0 - 2.0 * s_pageSpacing;
            }
            visibleHeight = viewport()->height() - 2.0 * s_pageSpacing;

            switch(m_rotation)
            {
            case QPdf::Rotate0:
            case QPdf::Rotate180:
                pageWidth = physicalDpiX() / 72.0 * size.width();
                pageHeight = physicalDpiY() / 72.0 * size.height();
                break;
            case QPdf::Rotate90:
            case QPdf::Rotate270:
                pageWidth = physicalDpiX() / 72.0 * size.height();
                pageHeight = physicalDpiY() / 72.0 * size.width();
                break;
            }

            switch(m_scaleMode)
            {
            case ScaleFactor:
                break;
            case FitToPageWidth:
                scaleFactor = visibleWidth / pageWidth;
                break;
            case FitToPageSize:
                scaleFactor = qMin(visibleWidth / pageWidth, visibleHeight / pageHeight);
                break;
            }

            page->setScaleFactor(scaleFactor);
        }
        else
        {
            page->setScaleFactor(m_scaleFactor);
        }

        page->setRotation(m_rotation);
    }

    // prepare layout

    m_heightToIndex.clear();

    qreal pageHeight = 0.0;

    qreal left = 0.0;
    qreal right = 0.0;
    qreal height = s_pageSpacing;

    for(int index = 0; index < m_numberOfPages; index++)
    {
        PageItem* page = m_pages.at(index);
        QRectF boundingRect = page->boundingRect();

        if(m_twoPagesMode)
        {
            if(index % 2 == 0)
            {
                page->setPos(-boundingRect.left() - boundingRect.width() - 0.5 * s_pageSpacing, height - boundingRect.top());

                m_heightToIndex.insert(-height + s_pageSpacing + 0.3 * pageHeight, index);

                pageHeight = boundingRect.height();

                left = qMin(left, -boundingRect.width() - 1.5f * s_pageSpacing);
            }
            else
            {
                page->setPos(-boundingRect.left() + 0.5 * s_pageSpacing, height - boundingRect.top());

                pageHeight = qMax(pageHeight, boundingRect.height());

                right = qMax(right, boundingRect.width() + 1.5f * s_pageSpacing);
                height += pageHeight + s_pageSpacing;
            }
        }
        else
        {
            page->setPos(-boundingRect.left() - 0.5 * boundingRect.width(), height - boundingRect.top());

            m_heightToIndex.insert(-height + s_pageSpacing + 0.3 * pageHeight, index);

            pageHeight = boundingRect.height();

            left = qMin(left, -0.5f * boundingRect.width() - s_pageSpacing);
            right = qMax(right, 0.5f * boundingRect.width() + s_pageSpacing);
            height += pageHeight + s_pageSpacing;
        }
    }

    if(m_twoPagesMode && m_numberOfPages % 2 != 0)
    {
        right = qMax(right, 0.5f * s_pageSpacing);
        height += pageHeight + s_pageSpacing;
    }

    m_pagesScene->setSceneRect(left, 0.0, right - left, height);
}

void DocumentView::prepareView(qreal changeLeft, qreal changeTop)
{
    qreal left = m_pagesScene->sceneRect().left();
    qreal top = m_pagesScene->sceneRect().top();
    qreal width = m_pagesScene->sceneRect().width();
    qreal height = m_pagesScene->sceneRect().height();

    int horizontalValue = 0;
    int verticalValue = 0;

    for(int index = 0; index < m_pages.count(); index++)
    {
        PageItem* page = m_pages.at(index);

        if(m_continuousMode)
        {
            page->setVisible(true);

            if(index == m_currentPage - 1)
            {
                QRectF boundingRect = page->boundingRect().translated(page->pos());

                horizontalValue = qFloor(boundingRect.left() + changeLeft * boundingRect.width());
                verticalValue = qFloor(boundingRect.top() + changeTop * boundingRect.height());
            }
        }
        else
        {
            if(index == m_currentPage - 1)
            {
                page->setVisible(true);

                QRectF boundingRect = page->boundingRect().translated(page->pos());

                top = boundingRect.top() - s_pageSpacing;
                height = boundingRect.height() + 2.0 * s_pageSpacing;

                horizontalValue = qFloor(boundingRect.left() + changeLeft * boundingRect.width());
                verticalValue = qFloor(boundingRect.top() + changeTop * boundingRect.height());
            }
            else if(m_twoPagesMode && index == m_currentPage)
            {
                page->setVisible(true);

                QRectF boundingRect = page->boundingRect().translated(page->pos());

                top = qMin(top, boundingRect.top() - s_pageSpacing);
                height = qMax(height, boundingRect.height() + 2.0f * s_pageSpacing);
            }
            else
            {
                page->setVisible(false);

                page->cancelRender();
            }
        }

    }

    setSceneRect(left, top, width, height);

    horizontalScrollBar()->setValue(horizontalValue);
    verticalScrollBar()->setValue(verticalValue);
    viewport()->update();
}

void DocumentView::prepareHighlight()
{

        m_highlight->setVisible(false);
}

void DocumentView::showRect(int index, const QRectF &r)
{
QGraphicsRectItem* h=new QGraphicsRectItem();
jumpToPage(index+1);
if (index >= m_pages.count()) return;
PageItem* page = m_pages.at(index);
h->setPos(page->pos());
h->setTransform(page->transform());
h->setRect(r.normalized());
h->setVisible(true);
disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(on_verticalScrollBar_valueChanged(int)));
centerOn(h);
connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(on_verticalScrollBar_valueChanged(int)));

}

void DocumentView::on_syncpage(int index, const QPointF& pagePos)
{
QPointF pos((pagePos.x()) / scaleFactor() / physicalDpiX() * 72.0,(pagePos.y()) / scaleFactor() / physicalDpiY() * 72.0 );
emit syncpage(index, pos);  
}

void DocumentView::countWords()
{
QString pdf_text="";
int numwords=0;
int pagewords=0;
QProgressDialog progress("",tr("Cancel"), 0, m_document->pageCount(), this);
progress.setWindowTitle("Texmaker");
progress.setWindowModality(Qt::WindowModal);
for (int i = 0; i < m_document->pageCount(); ++i)
  {
  progress.setValue(i);
  qApp->processEvents();
  if (progress.wasCanceled()) return;
  pagewords=0;
  pdf_text=m_document->checkTextAt(i, m_pages.at(i)->boundingRect(), m_pages.at(i)->boundingRect());
  pdf_text=pdf_text.simplified();
  pagewords=pdf_text.count(" ");
  if (!pdf_text.isEmpty()) pagewords++;
  numwords+=pagewords;
  }
progress.setValue(m_document->pageCount());
QMessageBox::information( this,"Texmaker",tr("Number of words in the document")+QString(" : ")+QString::number(numwords));
}

void DocumentView::pngExport(int page)
{
QImage image = m_pages.at(page)->exportImagePage();
if (image.isNull()) return;
QString currentDir=QDir::homePath();
QFileInfo fi(m_filePath);
if (fi.exists() && fi.isReadable()) currentDir=fi.absolutePath();
QString fn = QFileDialog::getSaveFileName(this,tr("Save As"),currentDir,"Png Image (*.png)");
if ( !fn.isEmpty() )
  {
  image.save(fn,"PNG");  
  }  
}

void DocumentView::countPageWords(int page)
{
QString pdf_text="";
int numwords=0;
int pagewords=0;
pdf_text=m_document->checkTextAt(page, m_pages.at(page)->boundingRect(), m_pages.at(page)->boundingRect());
pdf_text=pdf_text.simplified();
pagewords=pdf_text.count(" ");
if (!pdf_text.isEmpty()) pagewords++;
numwords+=pagewords;
QMessageBox::information( this,"Texmaker",tr("Number of words in the page")+QString(" : ")+QString::number(numwords));
}

void DocumentView::openFileBrowser()
{
QString currentDir="file://";
QFileInfo fi(m_filePath);
if (fi.exists() && fi.isReadable()) currentDir+=fi.absolutePath();
QDesktopServices::openUrl(QUrl(currentDir));
}


