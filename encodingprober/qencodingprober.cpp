/*
    This file is part of the KDE libraries

    Copyright (C) 2008 Wang Hoi (zealot.hoi@gmail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "qencodingprober.h"

#include "nsCharSetProber.h"
#include "nsUniversalDetector.h"
#include "ChineseGroupProber.h"
#include "JapaneseGroupProber.h"
#include "UnicodeGroupProber.h"
#include "nsSBCSGroupProber.h"
#include "nsMBCSGroupProber.h"

#include <string.h>

class QEncodingProberPrivate
{
public:
    QEncodingProberPrivate(): prober(NULL), mStart(true) {};
    ~QEncodingProberPrivate()
    {
        delete prober;
    }
    void setProberType(QEncodingProber::ProberType pType)
    {
        proberType = pType;
        /* handle multi-byte encodings carefully , because they're hard to detect,
        *   and have to use some Stastics methods.
        * for single-byte encodings (most western encodings), nsSBCSGroupProber is ok,
        *   because encoding state machine can detect many such encodings.
        */

        delete prober;

        switch (proberType) {
            case QEncodingProber::None:
                prober = NULL;
                break;
            case QEncodingProber::Arabic:
            case QEncodingProber::Baltic:
            case QEncodingProber::CentralEuropean:
            case QEncodingProber::Cyrillic:
            case QEncodingProber::Greek:
            case QEncodingProber::Hebrew:
            case QEncodingProber::NorthernSaami:
            case QEncodingProber::Other:
            case QEncodingProber::SouthEasternEurope:
            case QEncodingProber::Thai:
            case QEncodingProber::Turkish:
            case QEncodingProber::WesternEuropean:
                prober = new qencodingprober::nsSBCSGroupProber();
                break;
            case QEncodingProber::ChineseSimplified:
            case QEncodingProber::ChineseTraditional:
                prober = new qencodingprober::ChineseGroupProber();
                break;
            case QEncodingProber::Japanese:
                prober = new qencodingprober::JapaneseGroupProber();
                break;
            case QEncodingProber::Korean:
                prober = new qencodingprober::nsMBCSGroupProber();
                break;
            case QEncodingProber::Unicode:
                prober = new qencodingprober::UnicodeGroupProber();
                break;
            case QEncodingProber::Universal:
                prober = new qencodingprober::nsUniversalDetector();
                break;
            default:
                prober = NULL;
        }
    }
    void unicodeTest(const char *aBuf, int aLen)
    {
        if (mStart)
        {
            mStart = false;
            if (aLen > 3)
            switch (aBuf[0])
            {
                case '\xEF':
                    if (('\xBB' == aBuf[1]) && ('\xBF' == aBuf[2]))
                    // EF BB BF  UTF-8 encoded BOM
                    proberState = QEncodingProber::FoundIt;
                    break;
                case '\xFE':
                    if (('\xFF' == aBuf[1]) && ('\x00' == aBuf[2]) && ('\x00' == aBuf[3]))
                        // FE FF 00 00  UCS-4, unusual octet order BOM (3412)
                        proberState = QEncodingProber::FoundIt;
                    else if ('\xFF' == aBuf[1])
                        // FE FF  UTF-16, big endian BOM
                        proberState = QEncodingProber::FoundIt;
                        break;
                case '\x00':
                    if (('\x00' == aBuf[1]) && ('\xFE' == aBuf[2]) && ('\xFF' == aBuf[3]))
                        // 00 00 FE FF  UTF-32, big-endian BOM
                        proberState = QEncodingProber::FoundIt;
                    else if (('\x00' == aBuf[1]) && ('\xFF' == aBuf[2]) && ('\xFE' == aBuf[3]))
                        // 00 00 FF FE  UCS-4, unusual octet order BOM (2143)
                        proberState = QEncodingProber::FoundIt;
                        break;
                case '\xFF':
                    if (('\xFE' == aBuf[1]) && ('\x00' == aBuf[2]) && ('\x00' == aBuf[3]))
                        // FF FE 00 00  UTF-32, little-endian BOM
                        proberState = QEncodingProber::FoundIt;
                    else if ('\xFE' == aBuf[1])
                        // FF FE  UTF-16, little endian BOM
                        proberState = QEncodingProber::FoundIt;
                        break;
            }  // switch

        }
    }
    QEncodingProber::ProberType proberType;
    QEncodingProber::ProberState proberState;
    qencodingprober::nsCharSetProber *prober;
    bool mStart;
};

QEncodingProber::QEncodingProber(QEncodingProber::ProberType proberType): d(new QEncodingProberPrivate())
{
    setProberType(proberType);
}

QEncodingProber::~QEncodingProber()
{
    delete d;
}

void QEncodingProber::reset()
{
    d->proberState = QEncodingProber::Probing;
    d->mStart = true;
}

QEncodingProber::ProberState QEncodingProber::feed(const QByteArray &data)
{
    return feed(data.data(), data.size());
}

QEncodingProber::ProberState QEncodingProber::feed(const char* data, int len)
{
    if (!d->prober)
        return d->proberState;
    if (d->proberState == Probing) {
        if (d->mStart) {
            d->unicodeTest(data, len);
            if (d->proberState == FoundIt)
                return d->proberState;
        }
        d->prober->HandleData(data, len);
        switch (d->prober->GetState())
        {
            case qencodingprober::eNotMe:
                d->proberState = NotMe;
                break;
            case qencodingprober::eFoundIt:
                d->proberState = FoundIt;
                break;
            default:
                d->proberState = Probing;
                break;
        }
    }
#ifdef DEBUG_PROBE
    d->prober->DumpStatus();
#endif
    return d->proberState;
}

QEncodingProber::ProberState QEncodingProber::state() const
{
    return d->proberState;
}

//DEPRECATED, do *not* use
//#ifndef KDE_NO_DEPRECATED
//const char* QEncodingProber::encodingName() const
//{
//    return qstrdup(encoding().constData());
//}
//#endif

QByteArray QEncodingProber::encoding() const
{
    if (!d->prober)
        return QByteArray("UTF-8");

    return QByteArray(d->prober->GetCharSetName());
}

float QEncodingProber::confidence() const
{
    if (!d->prober)
        return 0.0;

    return d->prober->GetConfidence();
}

QEncodingProber::ProberType QEncodingProber::proberType() const
{
    return d->proberType;
}

void QEncodingProber::setProberType(QEncodingProber::ProberType proberType)
{
    d->setProberType(proberType);
    reset();
}

QEncodingProber::ProberType QEncodingProber::proberTypeForName(const QString& lang)
{
    if (lang.isEmpty())
        return QEncodingProber::Universal;
    else if (lang=="Disabled")
        return QEncodingProber::None;
    else if (lang=="Universal")
        return QEncodingProber::Universal;
    else if (lang=="Unicode")
        return QEncodingProber::Unicode;
    else if (lang=="Cyrillic")
        return QEncodingProber::Cyrillic;
    else if (lang=="Western European")
        return QEncodingProber::WesternEuropean;
    else if (lang=="Central European")
        return QEncodingProber::CentralEuropean;
    else if (lang=="Greek")
        return QEncodingProber::Greek;
    else if (lang=="Hebrew")
        return QEncodingProber::Hebrew;
    else if (lang=="Turkish")
        return QEncodingProber::Turkish;
    else if (lang=="Japanese")
        return QEncodingProber::Japanese;
    else if (lang=="Baltic")
        return QEncodingProber::Baltic;
    else if (lang=="Chinese Traditional")
        return QEncodingProber::ChineseTraditional;
    else if (lang=="Chinese Simplified")
        return QEncodingProber::ChineseSimplified;
    else if (lang=="Arabic")
        return QEncodingProber::Arabic;

    return QEncodingProber::Universal;
}

QString QEncodingProber::nameForProberType(QEncodingProber::ProberType proberType)
{
    switch (proberType)
    {
        case QEncodingProber::None:
            return "Disabled";
            break;
        case QEncodingProber::Universal:
            return "Universal";
            break;
        case QEncodingProber::Arabic:
            return "Arabic";
            break;
        case QEncodingProber::Baltic:
            return "Baltic";
            break;
        case QEncodingProber::CentralEuropean:
            return "Central European";
            break;
        case QEncodingProber::Cyrillic:
            return "Cyrillic";
            break;
        case QEncodingProber::Greek:
            return "Greek";
            break;
        case QEncodingProber::Hebrew:
            return "Hebrew";
            break;
        case QEncodingProber::Japanese:
            return "Japanese";
            break;
        case QEncodingProber::Turkish:
            return "Turkish";
            break;
        case QEncodingProber::WesternEuropean:
            return "Western European";
            break;
        case QEncodingProber::ChineseTraditional:
            return "Chinese Traditional";
            break;
        case QEncodingProber::ChineseSimplified:
            return "Chinese Simplified";
            break;
        case QEncodingProber::Korean:
            return "Korean";
            break;
        case QEncodingProber::Thai:
            return "Thai";
            break;
        case QEncodingProber::Unicode:
            return "Unicode";
            break;
        default:
            return QString();
        }
}
