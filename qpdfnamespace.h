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

#ifndef QPDFNAMESPACE_H
#define QPDFNAMESPACE_H

//using namespace std;

#include <QObject>
#include <QtCore>

namespace QPdf {
//    Q_NAMESPACE

    enum Rotation {
        Rotate0,
        Rotate90,
        Rotate180,
        Rotate270
    };
    //Q_ENUM_NS(Rotation)

    enum RenderFlag {
        NoRenderFlags = 0x000,
        RenderAnnotations = 0x001,
        RenderOptimizedForLcd = 0x002,
        RenderGrayscale = 0x004,
        RenderForceHalftone = 0x008,
        RenderTextAliased = 0x010,
        RenderImageAliased = 0x020,
        RenderPathAliased = 0x040
    };
    //Q_FLAG_NS(RenderFlag)
    Q_DECLARE_FLAGS(RenderFlags, RenderFlag)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QPdf::RenderFlags)

#endif
