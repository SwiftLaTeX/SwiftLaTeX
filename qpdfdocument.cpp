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

#include "qpdfdocument.h"
#include "qpdfdocument_p.h"

#include "pdfium/public/fpdf_text.h"

//#include "pdfium/public/fpdf_doc.h"

#include <QFile>
#include <QMutex>

QT_BEGIN_NAMESPACE

// The library is not thread-safe at all, it has a lot of global variables.
Q_GLOBAL_STATIC_WITH_ARGS(QMutex, pdfMutex, (QMutex::Recursive));
static int libraryRefCount;

QPdfMutexLocker::QPdfMutexLocker()
    : QMutexLocker(pdfMutex())
{
}

QPdfDocumentPrivate::QPdfDocumentPrivate()
    : avail(nullptr)
    , doc(nullptr)
    , loadComplete(false)
    , status(QPdfDocument::Null)
    , lastError(QPdfDocument::NoError)
    , pageCount(0)
{
    asyncBuffer.setData(QByteArray());
    asyncBuffer.open(QIODevice::ReadWrite);

    const QPdfMutexLocker lock;

    if (libraryRefCount == 0)
        FPDF_InitLibrary();
    ++libraryRefCount;

    // FPDF_FILEACCESS setup
    m_Param = this;
    m_GetBlock = fpdf_GetBlock;

    // FX_FILEAVAIL setup
    FX_FILEAVAIL::version = 1;
    IsDataAvail = fpdf_IsDataAvail;

    // FX_DOWNLOADHINTS setup
    FX_DOWNLOADHINTS::version = 1;
    AddSegment = fpdf_AddSegment;
}

QPdfDocumentPrivate::~QPdfDocumentPrivate()
{
    q->close();

    const QPdfMutexLocker lock;

    if (!--libraryRefCount)
        FPDF_DestroyLibrary();
}

void QPdfDocumentPrivate::clear()
{
    QPdfMutexLocker lock;

    if (doc)
        FPDF_CloseDocument(doc);
    doc = nullptr;

    if (avail)
        FPDFAvail_Destroy(avail);
    avail = nullptr;
    lock.unlock();

    if (pageCount != 0) {
        pageCount = 0;
        emit q->pageCountChanged(pageCount);
    }

    loadComplete = false;

    asyncBuffer.close();
    asyncBuffer.setData(QByteArray());
    asyncBuffer.open(QIODevice::ReadWrite);

    if (sequentialSourceDevice)
        sequentialSourceDevice->disconnect(q);
}

void QPdfDocumentPrivate::updateLastError()
{
    if (doc) {
        lastError = QPdfDocument::NoError;
        return;
    }

    QPdfMutexLocker lock;
    const unsigned long error = FPDF_GetLastError();
    lock.unlock();

    switch (error) {
    case FPDF_ERR_SUCCESS: lastError = QPdfDocument::NoError; break;
    case FPDF_ERR_UNKNOWN: lastError = QPdfDocument::UnknownError; break;
    case FPDF_ERR_FILE: lastError = QPdfDocument::FileNotFoundError; break;
    case FPDF_ERR_FORMAT: lastError = QPdfDocument::InvalidFileFormatError; break;
    case FPDF_ERR_PASSWORD: lastError = QPdfDocument::IncorrectPasswordError; break;
    case FPDF_ERR_SECURITY: lastError = QPdfDocument::UnsupportedSecuritySchemeError; break;
    default:
        Q_UNREACHABLE();
    }
}

void QPdfDocumentPrivate::load(QIODevice *newDevice, bool transferDeviceOwnership)
{
    if (transferDeviceOwnership)
        ownDevice.reset(newDevice);
    else
        ownDevice.reset();

    if (newDevice->isSequential()) {
        sequentialSourceDevice = newDevice;
        device = &asyncBuffer;
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sequentialSourceDevice);

        if (!reply) {
            setStatus(QPdfDocument::Error);
            qWarning() << "QPdfDocument: Loading from sequential devices only supported with QNetworkAccessManager.";
            return;
        }

        if (reply->isFinished() && reply->error() != QNetworkReply::NoError) {
            setStatus(QPdfDocument::Error);
            return;
        }

        QObject::connect(reply, &QNetworkReply::finished, q, [this, reply](){
            if (reply->error() != QNetworkReply::NoError || reply->bytesAvailable() == 0) {
                this->setStatus(QPdfDocument::Error);
            }
        });

        if (reply->header(QNetworkRequest::ContentLengthHeader).isValid())
            _q_tryLoadingWithSizeFromContentHeader();
        else
            QObject::connect(reply, SIGNAL(metaDataChanged()), q, SLOT(_q_tryLoadingWithSizeFromContentHeader()));
    } else {
        device = newDevice;
        initiateAsyncLoadWithTotalSizeKnown(device->size());
        checkComplete();
    }
}

void QPdfDocumentPrivate::_q_tryLoadingWithSizeFromContentHeader()
{
    if (avail)
        return;

    const QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(sequentialSourceDevice);
    if (!networkReply) {
        setStatus(QPdfDocument::Error);
        return;
    }

    const QVariant contentLength = networkReply->header(QNetworkRequest::ContentLengthHeader);
    if (!contentLength.isValid()) {
        setStatus(QPdfDocument::Error);
        return;
    }

    QObject::connect(sequentialSourceDevice, SIGNAL(readyRead()), q, SLOT(_q_copyFromSequentialSourceDevice()));

    initiateAsyncLoadWithTotalSizeKnown(contentLength.toULongLong());

    if (sequentialSourceDevice->bytesAvailable())
        _q_copyFromSequentialSourceDevice();
}

void QPdfDocumentPrivate::initiateAsyncLoadWithTotalSizeKnown(quint64 totalSize)
{
    // FPDF_FILEACCESS setup
    m_FileLen = totalSize;

    const QPdfMutexLocker lock;

    avail = FPDFAvail_Create(this, this);
}

void QPdfDocumentPrivate::_q_copyFromSequentialSourceDevice()
{
    if (loadComplete)
        return;

    const QByteArray data = sequentialSourceDevice->read(sequentialSourceDevice->bytesAvailable());
    if (data.isEmpty())
        return;

    asyncBuffer.seek(asyncBuffer.size());
    asyncBuffer.write(data);

    checkComplete();
}

void QPdfDocumentPrivate::tryLoadDocument()
{
    QPdfMutexLocker lock;

    if (!FPDFAvail_IsDocAvail(avail, this))
        return;

    Q_ASSERT(!doc);

    doc = FPDFAvail_GetDocument(avail, password);
    lock.unlock();

    updateLastError();

    if (lastError == QPdfDocument::IncorrectPasswordError) {
        FPDF_CloseDocument(doc);
        doc = nullptr;

        setStatus(QPdfDocument::Error);
        emit q->passwordRequired();
    }
}

void QPdfDocumentPrivate::checkComplete()
{
    if (!avail || loadComplete)
        return;

    if (!doc)
        tryLoadDocument();

    if (!doc)
        return;

    loadComplete = true;

    QPdfMutexLocker lock;

    const int newPageCount = FPDF_GetPageCount(doc);
    for (int i = 0; i < newPageCount; ++i) {
        int result = PDF_DATA_NOTAVAIL;
        while (result == PDF_DATA_NOTAVAIL) {
            result = FPDFAvail_IsPageAvail(avail, i, this);
        }

        if (result == PDF_DATA_ERROR)
            loadComplete = false;
    }

    lock.unlock();

    if (loadComplete) {
        if (newPageCount != pageCount) {
            pageCount = newPageCount;
            emit q->pageCountChanged(pageCount);
        }

        setStatus(QPdfDocument::Ready);
    }
}

void QPdfDocumentPrivate::setStatus(QPdfDocument::Status documentStatus)
{
    if (status == documentStatus)
        return;

    status = documentStatus;
    emit q->statusChanged(status);
}

FPDF_BOOL QPdfDocumentPrivate::fpdf_IsDataAvail(_FX_FILEAVAIL *pThis, size_t offset, size_t size)
{
    QPdfDocumentPrivate *d = static_cast<QPdfDocumentPrivate*>(pThis);
    return offset + size <= static_cast<quint64>(d->device->size());
}

int QPdfDocumentPrivate::fpdf_GetBlock(void *param, unsigned long position, unsigned char *pBuf, unsigned long size)
{
    QPdfDocumentPrivate *d = static_cast<QPdfDocumentPrivate*>(reinterpret_cast<FPDF_FILEACCESS*>(param));
    d->device->seek(position);
    return qMax(qint64(0), d->device->read(reinterpret_cast<char *>(pBuf), size));

}

void QPdfDocumentPrivate::fpdf_AddSegment(_FX_DOWNLOADHINTS *pThis, size_t offset, size_t size)
{
    Q_UNUSED(pThis);
    Q_UNUSED(offset);
    Q_UNUSED(size);
}

QPdfDocument::QPdfDocument(QObject *parent)
    : QObject(parent)
    , d(new QPdfDocumentPrivate)
{
    d->q = this;
}

/*!
    Destroys the document.
*/
QPdfDocument::~QPdfDocument()
{
}

QPdfDocument::DocumentError QPdfDocument::load(const QString &fileName)
{
    close();

    d->setStatus(QPdfDocument::Loading);

    QScopedPointer<QFile> f(new QFile(fileName));
    if (!f->open(QIODevice::ReadOnly)) {
        d->lastError = FileNotFoundError;
        d->setStatus(QPdfDocument::Error);
    } else {
        d->load(f.take(), /*transfer ownership*/true);
    }
    return d->lastError;
}

/*!
    \enum QPdfDocument::Status

    This enum describes the current status of the document.

    \value Null The initial status after the document has been created or after it has been closed.
    \value Loading The status after load() has been called and before the document is fully loaded.
    \value Ready The status when the document is fully loaded and its data can be accessed.
    \value Unloading The status after close() has been called on an open document.
                     At this point the document is still valid and all its data can be accessed.
    \value Error The status after Loading, if loading has failed.

    \sa QPdfDocument::status()
*/

/*!
    Returns the current status of the document.
*/
QPdfDocument::Status QPdfDocument::status() const
{
    return d->status;
}

void QPdfDocument::load(QIODevice *device)
{
    close();

    d->setStatus(QPdfDocument::Loading);

    d->load(device, /*transfer ownership*/false);
}

void QPdfDocument::setPassword(const QString &password)
{
    const QByteArray newPassword = password.toUtf8();

    if (d->password == newPassword)
        return;

    d->password = newPassword;
    emit passwordChanged();
}

QString QPdfDocument::password() const
{
    return QString::fromUtf8(d->password);
}

/*!
    \enum QPdfDocument::MetaDataField

    This enum describes the available fields of meta data.

    \value Title The document's title as QString.
    \value Author The name of the person who created the document as QString.
    \value Subject The subject of the document as QString.
    \value Keywords Keywords associated with the document as QString.
    \value Creator If the document was converted to PDF from another format,
                   the name of the conforming product that created the original document
                   from which it was converted as QString.
    \value Producer If the document was converted to PDF from another format,
                    the name of the conforming product that converted it to PDF as QString.
    \value CreationDate The date and time the document was created as QDateTime.
    \value ModificationDate The date and time the document was most recently modified as QDateTime.

    \sa QPdfDocument::metaData()
*/

/*!
    Returns the meta data of the document for the given field.
*/
QVariant QPdfDocument::metaData(MetaDataField field) const
{
    if (!d->doc)
        return QString();

    QByteArray fieldName;
    switch (field) {
    case Title:
        fieldName = "Title";
        break;
    case Subject:
        fieldName = "Subject";
        break;
    case Author:
        fieldName = "Author";
        break;
    case Keywords:
        fieldName = "Keywords";
        break;
    case Producer:
        fieldName = "Producer";
        break;
    case Creator:
        fieldName = "Creator";
        break;
    case CreationDate:
        fieldName = "CreationDate";
        break;
    case ModificationDate:
        fieldName = "ModDate";
        break;
    }

    QPdfMutexLocker lock;
    const unsigned long len = FPDF_GetMetaText(d->doc, fieldName.constData(), nullptr, 0);

    QVector<ushort> buf(len);
    FPDF_GetMetaText(d->doc, fieldName.constData(), buf.data(), buf.length());
    lock.unlock();

    QString text = QString::fromUtf16(buf.data());

    switch (field) {
    case Title: // fall through
    case Subject:
    case Author:
    case Keywords:
    case Producer:
    case Creator:
        return text;
    case CreationDate: // fall through
    case ModificationDate:
        // convert a "D:YYYYMMDDHHmmSSOHH'mm'" into "YYYY-MM-DDTHH:mm:ss+HH:mm"
        if (text.startsWith(QLatin1String("D:")))
            text = text.mid(2);
        text.insert(4, QLatin1Char('-'));
        text.insert(7, QLatin1Char('-'));
        text.insert(10, QLatin1Char('T'));
        text.insert(13, QLatin1Char(':'));
        text.insert(16, QLatin1Char(':'));
        text.replace(QLatin1Char('\''), QLatin1Char(':'));
        if (text.endsWith(QLatin1Char(':')))
            text.chop(1);

        return QDateTime::fromString(text, Qt::ISODate);
    }

    return QVariant();
}

QPdfDocument::DocumentError QPdfDocument::error() const
{
    return d->lastError;
}

/*!
    \fn void QPdfDocument::aboutToBeClosed()

    This signal is emitted whenever the document is closed.

    \sa close()
*/

/*!
  Closes the document.
*/
void QPdfDocument::close()
{
    if (!d->doc)
        return;

    d->setStatus(Unloading);

    d->clear();

    if (!d->password.isEmpty()) {
        d->password.clear();
        emit passwordChanged();
    }

    d->setStatus(Null);
}

int QPdfDocument::pageCount() const
{
    return d->pageCount;
}

QSizeF QPdfDocument::pageSize(int page) const
{
    QSizeF result;
    if (!d->doc)
        return result;

    const QPdfMutexLocker lock;

    FPDF_GetPageSizeByIndex(d->doc, page, &result.rwidth(), &result.rheight());
    return result;
}

/*!
    Renders the \a page into a QImage of size \a imageSize according to the
    provided \a renderOptions.

    Returns the rendered page or an empty image in case of an error.

    Note: If the \a imageSize does not match the aspect ratio of the page in the
    PDF document, the page is rendered scaled, so that it covers the
    complete \a imageSize.
*/
QImage QPdfDocument::render(int page, QSize imageSize, QPdfDocumentRenderOptions renderOptions)
{
    if (!d->doc)
        return QImage();

    const QPdfMutexLocker lock;

    FPDF_PAGE pdfPage = FPDF_LoadPage(d->doc, page);
    if (!pdfPage)
        return QImage();

    QImage result(imageSize, QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(result.width(), result.height(), FPDFBitmap_BGRA, result.bits(), result.bytesPerLine());

    int rotation = 0;
    switch (renderOptions.rotation()) {
    case QPdf::Rotate0:
        rotation = 0;
        break;
    case QPdf::Rotate90:
        rotation = 1;
        break;
    case QPdf::Rotate180:
        rotation = 2;
        break;
    case QPdf::Rotate270:
        rotation = 3;
        break;
    }

    const QPdf::RenderFlags renderFlags = renderOptions.renderFlags();
    int flags = 0;
    if (renderFlags & QPdf::RenderAnnotations)
        flags |= FPDF_ANNOT;
    if (renderFlags & QPdf::RenderOptimizedForLcd)
        flags |= FPDF_LCD_TEXT;
    if (renderFlags & QPdf::RenderGrayscale)
        flags |= FPDF_GRAYSCALE;
    if (renderFlags & QPdf::RenderForceHalftone)
        flags |= FPDF_RENDER_FORCEHALFTONE;
    if (renderFlags & QPdf::RenderTextAliased)
        flags |= FPDF_RENDER_NO_SMOOTHTEXT;
    if (renderFlags & QPdf::RenderImageAliased)
        flags |= FPDF_RENDER_NO_SMOOTHIMAGE;
    if (renderFlags & QPdf::RenderPathAliased)
        flags |= FPDF_RENDER_NO_SMOOTHPATH;

    FPDF_RenderPageBitmap(bitmap, pdfPage, 0, 0, result.width(), result.height(), rotation, flags);

    FPDFBitmap_Destroy(bitmap);

    FPDF_ClosePage(pdfPage);

    return result;
}

// QString QPdfDocument::getTextPage(int index)
// {
// QString result=QString("");
// if (!d->doc) return result;
// 
// const QPdfMutexLocker lock;
// 
// FPDF_PAGE  page=FPDF_LoadPage(d->doc,index);
// if (!page) return result;
// FPDF_TEXTPAGE textPage=FPDFText_LoadPage(page);
// int txtSize = FPDFText_CountChars(textPage);
// qDebug() << index << txtSize;
// unsigned short* txtBuf = new unsigned short[txtSize];
// //FPDFText_GetText(textPage,0,txtSize,txtBuf);
// // result=QString::fromUtf16(txtBuf);
// return result;
// }

QString QPdfDocument::checkTextAt(int index, QRectF boundingRect, QRectF selectionRect)
{

QString result=QString("");
if (!d->doc) return result;

const QPdfMutexLocker lock;

FPDF_PAGE  page=FPDF_LoadPage(d->doc,index);
if (!page) return result;

double page_left;
double page_top;
double page_right;
double page_bottom;
double page_x;
double page_y;
FPDF_DeviceToPage(page, 0, 0, boundingRect.width(), boundingRect.height(), 0, selectionRect.left(),selectionRect.top() , &page_left, &page_top);
FPDF_DeviceToPage(page, 0, 0, boundingRect.width(), boundingRect.height(), 0, selectionRect.right(),selectionRect.bottom() , &page_right, &page_bottom);
FPDF_TEXTPAGE textPage=FPDFText_LoadPage(page);
int txtSize=FPDFText_GetBoundedText(textPage,page_left,page_top,page_right,page_bottom,NULL,0);
unsigned short* txtBuf = new unsigned short[txtSize];
FPDFText_GetBoundedText(textPage,page_left,page_top,page_right,page_bottom,txtBuf,txtSize);
result=QString::fromUtf16(txtBuf);
FPDF_ClosePage(page);
return result;
}

struct linkItem QPdfDocument::checkLinkAt(int index, QRectF boundingRect, int x, int y)
{
struct linkItem result;
result.foundLink=false;
result.actionLink=0;
result.uriLink=QString("");
result.destPageLink=0;

if (!d->doc) return result;

const QPdfMutexLocker lock;

FPDF_PAGE  page=FPDF_LoadPage(d->doc,index);
if (!page) return result;

double page_x;
double page_y;
FPDF_DeviceToPage(page, 0, 0, boundingRect.width(), boundingRect.height(), 0, x, y, &page_x, &page_y);
const FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, page_x, page_y);
if (link) 
    {
    FPDF_DEST dest = FPDFLink_GetDest(d->doc, link);
    if (dest)
        {
        result.foundLink=true;
        result.actionLink=1;
        result.destPageLink=FPDFDest_GetPageIndex(d->doc, dest);
        }
    else
        {
        FPDF_ACTION action = FPDFLink_GetAction(link);
        if (action)
            {
            unsigned long actionType = FPDFAction_GetType(action);
            switch (actionType)
                {
                case PDFACTION_GOTO:
                    dest = FPDFAction_GetDest(d->doc, action);
                    if (dest)
                        {
                        result.foundLink=true;
                        result.actionLink=1;
                        result.destPageLink=FPDFDest_GetPageIndex(d->doc, dest);
                        }
                    break;
                case PDFACTION_URI:
                    int uriSize = FPDFAction_GetURIPath(d->doc, action, NULL, 0);
                    char* uriBuf = new char[uriSize];
                    FPDFAction_GetURIPath(d->doc, action, uriBuf, uriSize);
                    QString qstr(uriBuf);
                    result.foundLink=true;
                    result.actionLink=2;
                    result.uriLink=qstr;
                    break;
                }
             
            }
        }
    }
FPDF_ClosePage(page);
return result;
}

struct findItem QPdfDocument::find(int index, QString text, int lastpos)
{
struct findItem result;
result.pos=0;
result.rect=QRectF();

if (!d->doc) return result;
const QPdfMutexLocker lock;
FPDF_PAGE  page=FPDF_LoadPage(d->doc,index);
if (!page) return result;
FPDF_TEXTPAGE textPage=FPDFText_LoadPage(page);
FPDF_SCHHANDLE find = FPDFText_FindStart(textPage,text.utf16(),0,0);
while (FPDFText_FindNext(find)) 
    {
    int charIndex=FPDFText_GetSchResultIndex(find);
    int charCount=FPDFText_GetSchCount(find);
    if (charIndex>lastpos) 
        {
        int count = FPDFText_CountRects(textPage, charIndex, charCount);
        double left, top, right, bottom;
        FPDFText_GetRect(textPage, 0, &left, &top, &right, &bottom);
        int screenLeft, screenTop, screenRight, screenBottom;
        FPDF_PageToDevice(page, 0, 0, pageSize(index).width(), pageSize(index).height(), 0, left, top, &screenLeft, &screenTop);
        FPDF_PageToDevice(page, 0, 0, pageSize(index).width(), pageSize(index).height(), 0, right, bottom,&screenRight, &screenBottom);
        result.rect.setLeft(screenLeft);
        result.rect.setTop(screenTop);
        result.rect.setRight(screenRight);
        result.rect.setBottom(screenBottom);
        result.pos=charIndex;
        break;
        }
    }
FPDFText_FindClose(find);
FPDF_ClosePage(page);
return result;
}

QT_END_NAMESPACE

#include "moc_qpdfdocument.cpp"
