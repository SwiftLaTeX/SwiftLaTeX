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

#ifndef QPDFBOOKMARKMODEL_H
#define QPDFBOOKMARKMODEL_H

//#include "qtpdfglobal.h"

#include <QAbstractItemModel>

QT_BEGIN_NAMESPACE

class QPdfDocument;
class QPdfBookmarkModelPrivate;

class QPdfBookmarkModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QPdfDocument* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(StructureMode structureMode READ structureMode WRITE setStructureMode NOTIFY structureModeChanged)

public:
    enum StructureMode
    {
        TreeMode,
        ListMode
    };
    Q_ENUM(StructureMode)

    enum Role
    {
        TitleRole = Qt::DisplayRole,
        LevelRole = Qt::UserRole,
        PageNumberRole
    };
    Q_ENUM(Role)

    explicit QPdfBookmarkModel(QObject *parent = nullptr);

    QPdfDocument* document() const;
    void setDocument(QPdfDocument *document);

    StructureMode structureMode() const;
    void setStructureMode(StructureMode mode);

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void documentChanged(QPdfDocument *document);
    void structureModeChanged(QPdfBookmarkModel::StructureMode structureMode);

private:
    Q_DECLARE_PRIVATE(QPdfBookmarkModel)

    Q_PRIVATE_SLOT(d_func(), void _q_documentStatusChanged())
};

QT_END_NAMESPACE

#endif
