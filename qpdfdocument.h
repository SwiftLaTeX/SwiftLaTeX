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

#ifndef QPDFDOCUMENT_H
#define QPDFDOCUMENT_H


#include "qpdfdocumentrenderoptions.h"

#include <QImage>
#include <QObject>

#include "pdfium/public/fpdf_doc.h"

struct linkItem {
bool foundLink;
int actionLink;
QString uriLink;
int destPageLink;
};

struct findItem {
int pos;
QRectF rect;
};

QT_BEGIN_NAMESPACE

class QPdfDocumentPrivate;
class QNetworkReply;

class QPdfDocument : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged FINAL)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged FINAL)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged FINAL)

public:
    enum Status {
        Null,
        Loading,
        Ready,
        Unloading,
        Error
    };
    Q_ENUM(Status)

    enum DocumentError {
        NoError,
        UnknownError,
        FileNotFoundError,
        InvalidFileFormatError,
        IncorrectPasswordError,
        UnsupportedSecuritySchemeError
    };
    Q_ENUM(DocumentError)

    enum MetaDataField {
        Title,
        Subject,
        Author,
        Keywords,
        Producer,
        Creator,
        CreationDate,
        ModificationDate
    };
    Q_ENUM(MetaDataField)

    explicit QPdfDocument(QObject *parent = nullptr);
    ~QPdfDocument();

    DocumentError load(const QString &fileName);

    Status status() const;

    void load(QIODevice *device);
    void setPassword(const QString &password);
    QString password() const;

    QVariant metaData(MetaDataField field) const;

    DocumentError error() const;

    void close();

    int pageCount() const;

    QSizeF pageSize(int page) const;

    QImage render(int page, QSize imageSize, QPdfDocumentRenderOptions options = QPdfDocumentRenderOptions());
    
    //QString getTextPage(int index);
    QString checkTextAt(int index, QRectF boundingRect, QRectF selectionRect);
    struct linkItem checkLinkAt(int index, QRectF boundingRect, int x, int y);
    struct findItem find(int index, QString text, int lastpos);

Q_SIGNALS:
    void passwordChanged();
    void passwordRequired();
    void statusChanged(QPdfDocument::Status status);
    void pageCountChanged(int pageCount);

private:
    friend class QPdfBookmarkModelPrivate;

    Q_PRIVATE_SLOT(d, void _q_tryLoadingWithSizeFromContentHeader())
    Q_PRIVATE_SLOT(d, void _q_copyFromSequentialSourceDevice())
    QScopedPointer<QPdfDocumentPrivate> d;
};

QT_END_NAMESPACE

#endif // QPDFDOCUMENT_H
