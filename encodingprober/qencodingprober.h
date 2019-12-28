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
#ifndef QENCODINGPROBER_H
#define QENCODINGPROBER_H

// enable debug of private probers
// #define DEBUG_PROBE

#include <QtCore/QString>

class QEncodingProberPrivate;

/**
 * @short Provides encoding detection(probe) capabilities.
 *
 * Probe the encoding of raw data only.
 * In the case it can't find it, return the most possible encoding it guessed.
 *
 * Always do Unicode probe regardless the ProberType
 *
 * Feed data to it several times with feed() until ProberState changes to FoundIt/NotMe,
 * or confidence() returns a value you find acceptable.
 *
 * Intended lifetime of the object: one instance per ProberType.
 *
 * Typical use:
 * \code
 * QByteArray data, moredata;
 * ...
 * QEncodingProber prober(QEncodingProber::Chinese);
 * prober.feed(data);
 * prober.feed(moredata);
 * if (prober.confidence() > 0.6)
 *    QString out = QTextCodec::codecForName(prober.encoding())->toUnicode(data);
 * \endcode
 *
 * At least 256 characters are needed to change the ProberState from Probing to FoundIt.
 * If you don't have so many characters to probe,
 * decide whether to accept the encoding it guessed so far according to the Confidence by yourself.
 *
 * @short Guess encoding of char array
 *
 */
class QEncodingProber
{
public:

    enum ProberState {
        FoundIt,  /**< Sure find the encoding */
        NotMe,    /**< Sure not included in current ProberType's all supported encodings  */
        Probing   /**< Need more data to make a decision */
    };

    enum ProberType {
        None,
        Universal,
        Arabic,
        Baltic,
        CentralEuropean,
        ChineseSimplified,
        ChineseTraditional,
        Cyrillic,
        Greek,
        Hebrew,
        Japanese,
        Korean,
        NorthernSaami,
        Other,
        SouthEasternEurope,
        Thai,
        Turkish,
        Unicode,
        WesternEuropean
    };

    /**
     * Default ProberType is Universal(detect all possibe encodings)
     */
    QEncodingProber(ProberType proberType=Universal);

    ~QEncodingProber();

    /**
     * reset the prober's internal state and data.
     */
    void reset();

    /**
     * The main class method
     *
     * feed data to the prober
     *
     * @returns the ProberState after probing the fed data.
     */
    ProberState feed(const QByteArray &data);
    ProberState feed(const char* data, int len);

    /**
     * @returns the prober's current ProberState
     *
     */
    ProberState state() const;

    /**
     * @returns the name of the best encoding it has guessed so far
     * @warning The returned string is allocated with strdup, so some memory is leaked with every call.
     * @deprecated Use encoding() instead, which returns a QByteArray.
     */
//#ifndef KDE_NO_DEPRECATED
//    KDE_DEPRECATED const char* encodingName() const;
//#endif

    /**
     * @returns a QByteArray with the name of the best encoding it has guessed so far
     * @since 4.2.2
     */
    QByteArray encoding() const;

    /**
     * @returns the confidence(sureness) of encoding it guessed so far (0.0 ~ 0.99), not very reliable for single byte encodings
     */
    float confidence() const;

    ProberType proberType() const;

    /**
     * change current prober's ProberType and reset the prober
     */
    void setProberType(ProberType proberType);

    /**
     * @return the ProberType for lang (eg. proberTypeForName("Chinese Simplified") will return QEncodingProber::ChineseSimplified
     */
    static ProberType proberTypeForName(const QString& lang);

    /**
     * map ProberType to language string
     */
    static QString nameForProberType(ProberType proberType);

private:
    QEncodingProberPrivate* const d;
};

#endif
