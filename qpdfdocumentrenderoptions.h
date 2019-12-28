/****************************************************************************
**
** Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Tobias König <tobias.koenig@kdab.com>
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

#ifndef QPDFDOCUMENTRENDEROPTIONS_H
#define QPDFDOCUMENTRENDEROPTIONS_H

#include "qpdfnamespace.h"

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QPdfDocumentRenderOptions
{
public:
    Q_DECL_CONSTEXPR QPdfDocumentRenderOptions() Q_DECL_NOTHROW : data(0) {}

    Q_DECL_CONSTEXPR QPdf::Rotation rotation() const Q_DECL_NOTHROW { return static_cast<QPdf::Rotation>(bits.rotation); }
    Q_DECL_RELAXED_CONSTEXPR void setRotation(QPdf::Rotation _rotation) Q_DECL_NOTHROW { bits.rotation = _rotation; }

    Q_DECL_CONSTEXPR QPdf::RenderFlags renderFlags() const Q_DECL_NOTHROW { return static_cast<QPdf::RenderFlags>(bits.renderFlags); }
    Q_DECL_RELAXED_CONSTEXPR void setRenderFlags(QPdf::RenderFlags _renderFlags) Q_DECL_NOTHROW { bits.renderFlags = _renderFlags; }

private:
    friend Q_DECL_CONSTEXPR inline bool operator==(QPdfDocumentRenderOptions lhs, QPdfDocumentRenderOptions rhs) Q_DECL_NOTHROW;


    struct Bits {
        quint32 renderFlags : 8;
        quint32 rotation    : 3;
        quint32 reserved    : 21;
        quint32 reserved2   : 32;
    };

    union {
        Bits bits;
        quint64 data;
    };
};

Q_DECLARE_TYPEINFO(QPdfDocumentRenderOptions, Q_PRIMITIVE_TYPE);

Q_DECL_CONSTEXPR inline bool operator==(QPdfDocumentRenderOptions lhs, QPdfDocumentRenderOptions rhs) Q_DECL_NOTHROW
{
    return lhs.data == rhs.data;
}

Q_DECL_CONSTEXPR inline bool operator!=(QPdfDocumentRenderOptions lhs, QPdfDocumentRenderOptions rhs) Q_DECL_NOTHROW
{
    return !operator==(lhs, rhs);
}

QT_END_NAMESPACE

#endif // QPDFDOCUMENTRENDEROPTIONS_H
