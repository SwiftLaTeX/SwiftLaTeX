/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtPDF module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpdfbookmarkmodel.h"

#include "qpdfdocument.h"
#include "qpdfdocument_p.h"

#include "public/fpdf_doc.h"
#include "public/fpdfview.h"

#include <QPointer>
#include <QScopedPointer>
#include <private/qabstractitemmodel_p.h>

QT_BEGIN_NAMESPACE

class BookmarkNode
{
public:
    explicit BookmarkNode(BookmarkNode *parentNode = nullptr)
        : m_parentNode(parentNode)
        , m_level(0)
        , m_pageNumber(0)
    {
    }

    ~BookmarkNode()
    {
        clear();
    }

    void clear()
    {
        qDeleteAll(m_childNodes);
        m_childNodes.clear();
    }

    void appendChild(BookmarkNode *child)
    {
        m_childNodes.append(child);
    }

    BookmarkNode *child(int row) const
    {
        return m_childNodes.at(row);
    }

    int childCount() const
    {
        return m_childNodes.count();
    }

    int row() const
    {
        if (m_parentNode)
            return m_parentNode->m_childNodes.indexOf(const_cast<BookmarkNode*>(this));

        return 0;
    }

    BookmarkNode *parentNode() const
    {
        return m_parentNode;
    }

    QString title() const
    {
        return m_title;
    }

    void setTitle(const QString &title)
    {
        m_title = title;
    }

    int level() const
    {
        return m_level;
    }

    void setLevel(int level)
    {
        m_level = level;
    }

    int pageNumber() const
    {
        return m_pageNumber;
    }

    void setPageNumber(int pageNumber)
    {
        m_pageNumber = pageNumber;
    }

private:
    QVector<BookmarkNode*> m_childNodes;
    BookmarkNode *m_parentNode;

    QString m_title;
    int m_level;
    int m_pageNumber;
};


class QPdfBookmarkModelPrivate : public QAbstractItemModelPrivate
{
public:
    QPdfBookmarkModelPrivate()
        : QAbstractItemModelPrivate()
        , m_rootNode(new BookmarkNode(nullptr))
        , m_document(nullptr)
        , m_structureMode(QPdfBookmarkModel::TreeMode)
    {
    }

    void rebuild()
    {
        Q_Q(QPdfBookmarkModel);

        const bool documentAvailable = (m_document && m_document->status() == QPdfDocument::Ready);

        if (documentAvailable) {
            q->beginResetModel();
            m_rootNode->clear();
            QPdfMutexLocker lock;
            appendChildNode(m_rootNode.data(), nullptr, 0, m_document->d->doc);
            lock.unlock();
            q->endResetModel();
        } else {
            if (m_rootNode->childCount() == 0) {
                return;
            } else {
                q->beginResetModel();
                m_rootNode->clear();
                q->endResetModel();
            }
        }
    }

    void appendChildNode(BookmarkNode *parentBookmarkNode, FPDF_BOOKMARK parentBookmark, int level, FPDF_DOCUMENT document)
    {
        FPDF_BOOKMARK bookmark = FPDFBookmark_GetFirstChild(document, parentBookmark);

        while (bookmark) {
            BookmarkNode *childBookmarkNode = nullptr;

            if (m_structureMode == QPdfBookmarkModel::TreeMode) {
                childBookmarkNode = new BookmarkNode(parentBookmarkNode);
                parentBookmarkNode->appendChild(childBookmarkNode);
            } else if (m_structureMode == QPdfBookmarkModel::ListMode) {
                childBookmarkNode = new BookmarkNode(m_rootNode.data());
                m_rootNode->appendChild(childBookmarkNode);
            }

            const unsigned long titleLength = FPDFBookmark_GetTitle(bookmark, nullptr, 0);

            QVector<ushort> titleBuffer(titleLength);
            FPDFBookmark_GetTitle(bookmark, titleBuffer.data(), titleBuffer.length());

            const FPDF_DEST dest = FPDFBookmark_GetDest(document, bookmark);
            const int pageNumber = FPDFDest_GetPageIndex(document, dest);
//             float x=0.0;
//             float y=0.0;
//             FPDFDest_GetLocationInPage(dest,&x,&y);
            


            childBookmarkNode->setTitle(QString::fromUtf16(titleBuffer.data()));
            childBookmarkNode->setLevel(level);
            childBookmarkNode->setPageNumber(pageNumber);

            // recurse down
            appendChildNode(childBookmarkNode, bookmark, level + 1, document);

            bookmark = FPDFBookmark_GetNextSibling(document, bookmark);
        }
    }

    void _q_documentStatusChanged()
    {
        rebuild();
    }

    Q_DECLARE_PUBLIC(QPdfBookmarkModel)

    QScopedPointer<BookmarkNode> m_rootNode;
    QPointer<QPdfDocument> m_document;
    QPdfBookmarkModel::StructureMode m_structureMode;
};


QPdfBookmarkModel::QPdfBookmarkModel(QObject *parent)
    : QAbstractItemModel(*new QPdfBookmarkModelPrivate, parent)
{
}

QPdfDocument* QPdfBookmarkModel::document() const
{
    Q_D(const QPdfBookmarkModel);

    return d->m_document;
}

void QPdfBookmarkModel::setDocument(QPdfDocument *document)
{
    Q_D(QPdfBookmarkModel);

    if (d->m_document == document)
        return;

    if (d->m_document)
        disconnect(d->m_document, SIGNAL(statusChanged(QPdfDocument::Status)), this, SLOT(_q_documentStatusChanged()));

    d->m_document = document;
    emit documentChanged(d->m_document);

    if (d->m_document)
        connect(d->m_document, SIGNAL(statusChanged(QPdfDocument::Status)), this, SLOT(_q_documentStatusChanged()));

    d->rebuild();
}

QPdfBookmarkModel::StructureMode QPdfBookmarkModel::structureMode() const
{
    Q_D(const QPdfBookmarkModel);

    return d->m_structureMode;
}

void QPdfBookmarkModel::setStructureMode(StructureMode mode)
{
    Q_D(QPdfBookmarkModel);

    if (d->m_structureMode == mode)
        return;

    d->m_structureMode = mode;
    emit structureModeChanged(d->m_structureMode);

    d->rebuild();
}

int QPdfBookmarkModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QHash<int, QByteArray> QPdfBookmarkModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[TitleRole] = "title";
    names[LevelRole] = "level";
    names[PageNumberRole] = "pageNumber";

    return names;
}

QVariant QPdfBookmarkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const BookmarkNode *node = static_cast<BookmarkNode*>(index.internalPointer());
    switch (role) {
    case TitleRole:
        return node->title();
    case LevelRole:
        return node->level();
    case PageNumberRole:
        return node->pageNumber();
    default:
        return QVariant();
    }
}

QModelIndex QPdfBookmarkModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const QPdfBookmarkModel);

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    BookmarkNode *parentNode;

    if (!parent.isValid())
        parentNode = d->m_rootNode.data();
    else
        parentNode = static_cast<BookmarkNode*>(parent.internalPointer());

    BookmarkNode *childNode = parentNode->child(row);
    if (childNode)
        return createIndex(row, column, childNode);
    else
        return QModelIndex();
}

QModelIndex QPdfBookmarkModel::parent(const QModelIndex &index) const
{
    Q_D(const QPdfBookmarkModel);

    if (!index.isValid())
        return QModelIndex();

    const BookmarkNode *childNode = static_cast<BookmarkNode*>(index.internalPointer());
    BookmarkNode *parentNode = childNode->parentNode();

    if (parentNode == d->m_rootNode.data())
        return QModelIndex();

    return createIndex(parentNode->row(), 0, parentNode);
}

int QPdfBookmarkModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QPdfBookmarkModel);

    if (parent.column() > 0)
        return 0;

    BookmarkNode *parentNode = nullptr;

    if (!parent.isValid())
        parentNode = d->m_rootNode.data();
    else
        parentNode = static_cast<BookmarkNode*>(parent.internalPointer());

    return parentNode->childCount();
}

QT_END_NAMESPACE

#include "moc_qpdfbookmarkmodel.cpp"
