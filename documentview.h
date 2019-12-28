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

#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QtCore>
#include <QtXml>
#include <QtGui>
#include <QGraphicsView>
#include <QPrinter>

#include "qpdfdocument.h"
#include "pageitem.h"
#include "presentationview.h"
#include "qpdfbookmarkmodel.h"

class DocumentView : public QGraphicsView
{
    Q_OBJECT

public:
    static bool openUrl();
    static void setOpenUrl(bool openUrl);

    static bool autoRefresh();
    static void setAutoRefresh(bool autoRefresh);

    static bool antialiasing();
    static void setAntialiasing(bool antialiasing);

    static bool textAntialiasing();
    static void setTextAntialiasing(bool textAntialiasing);

    static bool textHinting();
    static void setTextHinting(bool textHinting);

    static bool prefetch();
    static void setPrefetch(bool prefetch);

    static qreal pageSpacing();
    static void setPageSpacing(qreal pageSpacing);

    static qreal minimumScaleFactor();
    static qreal maximumScaleFactor();

    DocumentView(QWidget* parent = 0);
    ~DocumentView();

    const QString& filePath() const;
    int numberOfPages() const;
    int currentPage() const;

    bool continousMode() const;
    void setContinousMode(bool continousMode);

    bool twoPagesMode() const;
    void setTwoPagesMode(bool twoPagesMode);

    enum ScaleMode
    {
        ScaleFactor = 0,
        FitToPageWidth = 1,
        FitToPageSize = 2
    };

    ScaleMode scaleMode() const;
    void setScaleMode(ScaleMode scaleMode);

    qreal scaleFactor() const;
    void setScaleFactor(qreal scaleFactor);

        qreal realScale() const;
    
    QPdf::Rotation rotation() const;
    void setRotation(QPdf::Rotation rotation);

   
    void clearPaths(int index);
    void setSearchPath(int index, const QPainterPath& path);
    void setHighlightPath(int index, const QPainterPath& path);
    void showRect(int index, const QRectF &r);

    void clearAll();

    QPdfBookmarkModel* outlineModel() const;
    QStandardItemModel* propertiesModel() const;

    
    QPdfDocument* doc() const;

signals:
    void filePathChanged(const QString& filePath);
    void numberOfPagesChanged(int numberOfPages);
    void currentPageChanged(int currentPage);

    void continousModeChanged(bool continousMode);
    void twoPagesModeChanged(bool twoPagesMode);
    void scaleModeChanged(DocumentView::ScaleMode scaleMode);
    void scaleFactorChanged(qreal scaleFactor);
    void rotationChanged(QPdf::Rotation rotation);

    void searchProgressed(int progress);
    void searchFinished();
    void searchCanceled();
    void doHScroll(int);
    
    void gotoDest(int,qreal,qreal);
    void syncpage(int, const QPointF&);
    void requestZoomIn();
    void requestZoomOut();


public slots:
    
    void show();

    bool open(const QString& filePath);
    bool refresh();
    bool print(QPrinter* printer);

    void previousPage();
    void nextPage();
    void firstPage();
    void lastPage();

    void jumpToPage(int page, qreal changeLeft = 0.0, qreal changeTop = 0.0, bool returnTo = true);


    void zoomIn();
    void zoomOut();
    void originalSize();

    void rotateLeft();
    void rotateRight();

    void presentation();
    void on_syncpage(int index, const QPointF& pagePos);
    void countWords();
    void pngExport(int page);
    void countPageWords(int page);
    void openFileBrowser();

protected slots:
    void on_verticalScrollBar_valueChanged(int value);
    void on_horizontalScrollBar_valueChanged(int value);


    void on_prefetch_timeout();

    void on_pages_linkClicked(int page, qreal left, qreal top);
    void on_pages_linkClicked(const QString& url);


protected:
    void resizeEvent(QResizeEvent* event);

    void keyPressEvent(QKeyEvent* event);
    //void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
  
    static bool s_openUrl;

    static bool s_autoRefresh;

    static bool s_antialiasing;
    static bool s_textAntialiasing;
    static bool s_textHinting;

    static bool s_prefetch;

    static qreal s_pageSpacing;

    static qreal s_minimumScaleFactor;
    static qreal s_maximumScaleFactor;

    QFileSystemWatcher* m_autoRefreshWatcher;
    QTimer* m_autoRefreshTimer;

    QTimer* m_prefetchTimer;

    QMutex m_mutex;
    QPdfDocument* m_document;

    QString m_filePath;
    int m_numberOfPages;
    int m_currentPage;

    int m_returnToPage;
    qreal m_returnToLeft;
    qreal m_returnToTop;

    int currentPageForPage(int page);
    void saveLeftAndTop(qreal& left, qreal& top);

    bool m_continuousMode;
    bool m_twoPagesMode;
    ScaleMode m_scaleMode;
    qreal m_scaleFactor;
    QPdf::Rotation m_rotation;

    QGraphicsScene* m_pagesScene;
    QList< PageItem* > m_pages;

    QMap< qreal, int > m_heightToIndex;


    QGraphicsRectItem* m_highlight;

    QPdfBookmarkModel* m_outlineModel;
    QStandardItemModel* m_propertiesModel;

    void prepareDocument(QPdfDocument* document);

    void preparePages();

    void prepareOutline();

    void prepareProperties();

    void prepareScene();
    void prepareView(qreal changeLeft = 0.0, qreal changeTop = 0.0);

    void prepareHighlight();




};

#endif // DOCUMENTVIEW_H
