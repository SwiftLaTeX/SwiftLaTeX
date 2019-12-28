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

#ifndef QPDFDOCUMENT_P_H
#define QPDFDOCUMENT_P_H

#include "qpdfdocument.h"

#include "public/fpdfview.h"
#include "public/fpdf_dataavail.h"

#include <qbuffer.h>
#include <qmutex.h>
#include <qnetworkreply.h>
#include <qpointer.h>

QT_BEGIN_NAMESPACE

class QPdfMutexLocker : public QMutexLocker
{
public:
    QPdfMutexLocker();
};

class QPdfDocumentPrivate: public FPDF_FILEACCESS, public FX_FILEAVAIL, public FX_DOWNLOADHINTS
{
public:
    QPdfDocumentPrivate();
    ~QPdfDocumentPrivate();

    QPdfDocument *q;

    FPDF_AVAIL avail;
    FPDF_DOCUMENT doc;
    bool loadComplete;

    QPointer<QIODevice> device;
    QScopedPointer<QIODevice> ownDevice;
    QBuffer asyncBuffer;
    QPointer<QIODevice> sequentialSourceDevice;
    QByteArray password;

    QPdfDocument::Status status;
    QPdfDocument::DocumentError lastError;
    int pageCount;

    void clear();

    void load(QIODevice *device, bool ownDevice);
    void loadAsync(QIODevice *device);

    void _q_tryLoadingWithSizeFromContentHeader();
    void initiateAsyncLoadWithTotalSizeKnown(quint64 totalSize);
    void _q_copyFromSequentialSourceDevice();
    void tryLoadDocument();
    void checkComplete();
    void setStatus(QPdfDocument::Status status);

    static FPDF_BOOL fpdf_IsDataAvail(struct _FX_FILEAVAIL* pThis, size_t offset, size_t size);
    static int fpdf_GetBlock(void* param, unsigned long position, unsigned char* pBuf, unsigned long size);
    static void fpdf_AddSegment(struct _FX_DOWNLOADHINTS* pThis, size_t offset, size_t size);
    void updateLastError();
};

QT_END_NAMESPACE

#endif // QPDFDOCUMENT_P_H
