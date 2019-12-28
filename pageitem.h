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

#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsObject>


#include "qpdfdocument.h"

class PageItem : public QGraphicsObject
{
    Q_OBJECT

public:
    static int cacheSize();
    static void setCacheSize(int cacheSize);

    static bool decoratePages();
    static void setDecoratePages(bool decoratePages);

    static bool decorateLinks();
    static void setDecorateLinks(bool decorateLinks);

    static bool invertColors();
    static void setInvertColors(bool invertColors);

    PageItem(QMutex* mutex, QPdfDocument* doc, int index, QGraphicsItem* parent = 0);
    ~PageItem();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

    int index() const;
    QSizeF size() const;

    const QList< QRectF >& highlights() const;
    void setHighlights(const QList< QRectF >& highlights);

    int physicalDpiX() const;
    int physicalDpiY() const;
    void setPhysicalDpi(int physicalDpiX, int physicalDpiY);

    qreal scaleFactor() const;
    void setScaleFactor(qreal scaleFactor);

    QPdf::Rotation rotation() const;
    void setRotation(QPdf::Rotation rotation);

    const QTransform& transform() const;
    const QTransform& normalizedTransform() const;
    
    void setHighlightPath(const QPainterPath& path);
    void setSearchPath(const QPainterPath& path);
    void clearPaths();
    QImage exportImagePage();

signals:
    void imageReady(QImage image, bool prefetch);

    void linkClicked(int page, qreal left, qreal top);
    void linkClicked(const QString& url);
    void wantNumWords();
    void wantPngExport(int);
    void syncpage(int, const QPointF&);
    void wantNumPageWords(int);
    void wantOpenLocation();
 

public slots:
    void refresh();
   void clearHighlight();
    void startRender(bool prefetch = false);
    void cancelRender();

protected slots:
    void on_render_finished();
    void on_imageReady(QImage image, bool prefetch);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    static QCache< PageItem*, QImage > s_cache;

    static bool s_decoratePages;
    static bool s_decorateLinks;

    static bool s_invertColors;

    QMutex* m_mutex;
    QPdfDocument* m_doc;

    int m_index;
    QSizeF m_size;


    QList< QRectF > m_highlights;
    QRectF m_rubberBand;

    void copyToClipboard(const QPoint& screenPos);

    // geometry

    int m_physicalDpiX;
    int m_physicalDpiY;

    qreal m_scaleFactor;
    QPdf::Rotation m_rotation;

    QTransform m_transform;
    QTransform m_normalizedTransform;
    QRectF m_boundingRect;

    QImage m_image;

    void prepareGeometry();

    // render

    QFutureWatcher< void >* m_render;
    void render(int physicalDpiX, int physicalDpiY, qreal scaleFactor, QPdf::Rotation rotation, bool prefetch);

    QPainterPath highlightPath, searchPath;
    QTimer highlightRemover;

    private slots:
    void jumpToSourceFromPdf();
    void requestNumWords();
    void requestPngExport();
    void requestPageWords();
    void requestOpenLocation();
    
};


#endif // PAGEITEM_H
