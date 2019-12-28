/*

Copyright 2012 Adam Reichold

This file is part of qpdfview.

qpdfview is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

qpdfview is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qpdfview.  If not, see <http://www.gnu.org/licenses/>.

*/

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

#include "presentationview.h"
#include <QApplication>
#include <QtConcurrent>
#include <QToolTip>


PresentationView::PresentationView(QMutex* mutex, QPdfDocument* document) : QWidget(),
    m_mutex(0),
    m_document(0),
    m_numberOfPages(-1),
    m_currentPage(-1),
    m_returnToPage(-1),
//    m_links(),
    m_scaleFactor(1.0),
    m_normalizedTransform(),
    m_boundingRect(),
    m_image(),
    m_render(0)
{

    setMouseTracking(true);

    m_render = new QFutureWatcher< void >(this);
    connect(m_render, SIGNAL(finished()), SLOT(on_render_finished()));

    connect(this, SIGNAL(imageReady(QImage)), SLOT(on_imageReady(QImage)));

    m_mutex = mutex;
    m_document = document;

    m_numberOfPages = m_document->pageCount();
    m_currentPage = 1;
    m_returnToPage = -1;

    prepareView();
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowState(windowState() | Qt::WindowFullScreen);
}

PresentationView::~PresentationView()
{
    m_render->cancel();
    m_render->waitForFinished();

    //qDeleteAll(m_links);
}

int PresentationView::numberOfPages() const
{
    return m_numberOfPages;
}

int PresentationView::currentPage() const
{
    return m_currentPage;
}

void PresentationView::previousPage()
{
    jumpToPage(currentPage() - 1, false);
}

void PresentationView::nextPage()
{
    jumpToPage(currentPage() + 1, false);
}

void PresentationView::firstPage()
{
    jumpToPage(1);
}

void PresentationView::lastPage()
{
    jumpToPage(numberOfPages());
}

void PresentationView::jumpToPage(int page, bool returnTo)
{
    if(m_currentPage != page && page >= 1 && page <= m_numberOfPages)
    {
        if(returnTo)
        {
            m_returnToPage = m_currentPage;
        }

        m_currentPage = page;

        prepareView();
    }
}

void PresentationView::startRender()
{
    if(!m_render->isRunning())
    {
        m_render->setFuture(QtConcurrent::run(this, &PresentationView::render, m_currentPage - 1, m_scaleFactor));
    }
}

void PresentationView::cancelRender()
{
    m_render->cancel();

    m_image = QImage();
}

void PresentationView::on_render_finished()
{
    update();
}

void PresentationView::on_imageReady(QImage image)
{
    if(PageItem::invertColors())
    {
        image.invertPixels();
    }

    if(!m_render->isCanceled())
    {
        m_image = image;
    }
}

void PresentationView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    prepareView();
}

void PresentationView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), QBrush(PageItem::invertColors() ? Qt::white : Qt::black));

    if(!m_image.isNull())
    {
        painter.fillRect(m_boundingRect, QBrush(Qt::white));
        painter.drawImage(m_boundingRect.topLeft(), m_image);
    }
    else
    {
        startRender();
    }
}

void PresentationView::keyPressEvent(QKeyEvent* event)
{
    if(event->modifiers() == Qt::NoModifier)
    {
        switch(event->key())
        {
        case Qt::Key_PageUp:
        case Qt::Key_Up:
        case Qt::Key_Left:
        case Qt::Key_Backspace:
            previousPage();

            event->accept();
            return;
        case Qt::Key_PageDown:
        case Qt::Key_Down:
        case Qt::Key_Right:
        case Qt::Key_Space:
            nextPage();

            event->accept();
            return;
        case Qt::Key_Home:
            firstPage();

            event->accept();
            return;
        case Qt::Key_End:
            lastPage();

            event->accept();
            return;
        case Qt::Key_Return:
            jumpToPage(m_returnToPage);

            event->accept();
            return;
        case Qt::Key_F12:
        case Qt::Key_Escape:
            close();

            event->accept();
            return;
        }
    }

    QWidget::keyPressEvent(event);
}

void PresentationView::mouseMoveEvent(QMouseEvent* event)
{
    QApplication::restoreOverrideCursor();

//     foreach(Poppler::LinkGoto* link, m_links)
//     {
//         if(m_normalizedTransform.mapRect(link->linkArea().normalized()).contains(event->pos()))
//         {
//             QApplication::setOverrideCursor(Qt::PointingHandCursor);
//             //QToolTip::showText(event->globalPos(), tr("Go to page %1.").arg(link->destination().pageNumber()));
// 
//             return;
//         }
//     }

    QToolTip::hideText();
}

void PresentationView::mousePressEvent(QMouseEvent* event)
{
//     foreach(Poppler::LinkGoto* link, m_links)
//     {
//         if(m_normalizedTransform.mapRect(link->linkArea().normalized()).contains(event->pos()))
//         {
//             int page = link->destination().pageNumber();
// 
//             page = page >= 1 ? page : 1;
//             page = page <= m_numberOfPages ? page : m_numberOfPages;
// 
//             jumpToPage(page);
// 
//             return;
//         }
//     }
}

void PresentationView::prepareView()
{
    m_mutex->lock();

//     Poppler::Page* page = m_document->page(m_currentPage - 1);
// 
//     qDeleteAll(m_links);
// 
//     m_links.clear();
// 
//     foreach(Poppler::Link* link, page->links())
//     {
//         if(link->linkType() == Poppler::Link::Goto)
//         {
//             Poppler::LinkGoto* linkGoto = static_cast< Poppler::LinkGoto* >(link);
// 
//             if(!linkGoto->isExternal())
//             {
//                 m_links.append(linkGoto);
//                 continue;
//             }
//         }
// 
//         delete link;
//     }

    QSizeF size = m_document->pageSize(m_currentPage - 1);

    //delete page;

    m_mutex->unlock();

    {
        m_scaleFactor = qMin(width() / size.width(), height() / size.height());

        m_boundingRect.setLeft(0.5 * (width() - m_scaleFactor * size.width()));
        m_boundingRect.setTop(0.5 * (height() - m_scaleFactor * size.height()));
        m_boundingRect.setWidth(m_scaleFactor * size.width());
        m_boundingRect.setHeight(m_scaleFactor * size.height());

        m_normalizedTransform.reset();
        m_normalizedTransform.translate(m_boundingRect.left(), m_boundingRect.top());
        m_normalizedTransform.scale(m_boundingRect.width(), m_boundingRect.height());
    }

    cancelRender();

    update();
}

void PresentationView::render(int index, qreal scaleFactor)
{
    QMutexLocker mutexLocker(m_mutex);

    if(m_render->isCanceled())
    {
        return;
    }

    
qreal extra=1.0;

if (qApp->devicePixelRatio()>=2) extra=qApp->devicePixelRatio();

const QSizeF size = m_document->pageSize(index) * scaleFactor * extra ;
    //QImage image = m_document->render(index,QSize(size.width()* physicalDpiX()/72,size.height()* physicalDpiY()/72));
QImage image = m_document->render(index,QSize(size.width(),size.height()));

if (qApp->devicePixelRatio()>=2) image.setDevicePixelRatio(qApp->devicePixelRatio());


    //delete page;

    if(m_render->isCanceled())
    {
        return;
    }

    emit imageReady(image);
}
