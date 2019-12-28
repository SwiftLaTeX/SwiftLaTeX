// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_UTILS_H_
#define XFA_FXBARCODE_UTILS_H_

#include "core/fxcrt/include/fx_basic.h"

typedef CFX_MapPtrTemplate<void*, int32_t> CFX_PtrToInt32;
FX_BOOL BC_FX_ByteString_Replace(CFX_ByteString& dst,
                                 uint32_t first,
                                 uint32_t last,
                                 int32_t count,
                                 FX_CHAR c);
void BC_FX_ByteString_Append(CFX_ByteString& dst, int32_t count, FX_CHAR c);
void BC_FX_ByteString_Append(CFX_ByteString& dst, const CFX_ByteArray& ba);

#if (_FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_)
#include <limits>
#elif(_FX_OS_ == _FX_MACOSX_ || _FX_OS_ == _FX_LINUX_DESKTOP_ || \
      _FX_OS_ == _FX_IOS_)
#include <limits.h>
#endif
#if (_FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_)
#define FXSYS_isnan(x) _isnan(x)
#elif(_FX_OS_ == _FX_MACOSX_ || _FX_OS_ == _FX_IOS_ || \
      _FX_OS_ == _FX_LINUX_DESKTOP_ || _FX_OS_ == _FX_ANDROID_)
#include <cmath>
#define FXSYS_isnan(x) std::isnan(x)
#endif
#if (_FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_)
#define FXSYS_nan() (std::numeric_limits<float>::quiet_NaN())
#elif(_FX_OS_ == _FX_MACOSX_ || _FX_OS_ == _FX_LINUX_DESKTOP_ || \
      _FX_OS_ == _FX_IOS_ || _FX_OS_ == _FX_ANDROID_)
#define FXSYS_nan() NAN
#endif
enum BCFORMAT {
  BCFORMAT_UNSPECIFY = -1,
  BCFORMAT_CODABAR,
  BCFORMAT_CODE_39,
  BCFORMAT_CODE_128,
  BCFORMAT_CODE_128B,
  BCFORMAT_CODE_128C,
  BCFORMAT_EAN_8,
  BCFORMAT_UPC_A,
  BCFORMAT_EAN_13,
  BCFORMAT_PDF_417,
  BCFORMAT_DATAMATRIX,
  BCFORMAT_QR_CODE
};
#define BCFORMAT_ECLEVEL_L 0
#define BCFORMAT_ECLEVEL_M 1
#define BCFORMAT_ECLEVEL_Q 2
#define BCFORMAT_ECLEVEL_H 3
#include <ctype.h>
#define FXSYS_IntMax INT_MAX
#define FXSYS_Isdigit isdigit
#define BCExceptionNO 0
#define BCExceptionNotFound 1
#define BCExceptionEndLessThanStart 2
#define BCExceptionUnknownDecoder 3
#define BCExceptionRotateNotSupported 4
#define BCExceptionHeightAndWidthMustBeAtLeast1 5
#define BCExceptionRegionMustFitInsideMatrix 6
#define BCExceptionCanNotCallGetDimensionOnNonSquareMatrix 7
#define BCExceptionFormatException 8
#define BCExceptionIllegalArgumentMustMatchVersionSize 9
#define BCExceptionChecksumException 10
#define BCExceptionIllegalArgumentInvalidFirstDigit 11
#define BCExceptionIllegalArgumentInvalidSecondDigit 12
#define BCExceptionRuntimeDecodingInvalidISO_IEC 13
#define BCExceptionRuntimeDecodingInvalidAlphanumeric 14
#define BCExceptionLeftAndTopMustBeNonnegative 15
#define BCExceptionIllegalArgument 16
#define BCExceptionBadECI 17
#define BCExceptionUnSupportedBarcode 18
#define BCExceptionUnSupportedString 19
#define BCExceptionDigitLengthMustBe8 20
#define BCExceptionDataCheckException 21
#define BCExceptionExtractNumberValueFromBitArray 22
#define BCExceptionRead 23
#define BCExceptionRequestedRowIsOutSizeTheImage 24
#define BCExceptionNoContents 26
#define BCExceptionUnSupportEclevel 27
#define BCExceptionUnSupportMode 28
#define BCExceptionReferenceMustBeBetween0And7 29
#define BCExceptionBadErrorLocation 30
#define BCExceptionDegreeIsNegative 31
#define BCExceptionDivideByZero 32
#define BCExceptionCoefficientsSizeIsNull 33
#define BCExceptionNoCorrectionBytes 34
#define BCExceptionNoDataBytesProvided 35
#define BCExceptionR_I_1IsZero 36
#define BCExceptionAIsZero 37
#define BCExceptionIsZero 38
#define BCExceptionDegreeNotMatchRoots 39
#define BCExceptionContentsLengthShouldBetween1and80 40
#define BCExceptionOnlyEncodeCODE_128 41
#define BCExceptionOnlyEncodeCODE_39 42
#define BCExceptionOnlyEncodeEAN_13 43
#define BCExceptionOnlyEncodeEAN_8 44
#define BCExceptionOnlyEncodeITF 45
#define BCExceptionDigitLengthShould13 46
#define BCExceptionDigitLengthMustBe6or8or10or12or14or16or20or24or44 47
#define BCExceptionOnlyEncodeUPC_A 48
#define BCExceptionDigitLengthShouldBe12 49
#define BCExceptionValueMustBeEither0or1 50
#define BCExceptionReedsolomnDecodeException 51
#define BCExceptionBadIndexException 52
#define BCExceptionBadValueException 53
#define BCExceptionBadNumBitsException 54
#define BCExceptioncanNotOperatexorOperator 55
#define BCExceptionVersionMust1_40 56
#define BCExceptionUnknown 57
#define BCExceptionNoSuchVersion 58
#define BCExceptionCannotFindBlockInfo 59
#define BCExceptionDataTooBig 60
#define BCExceptionInvalidQRCode 61
#define BCExceptionDataTooMany 62
#define BCExceptionBitsNotEqualCacity 63
#define BCExceptionUnsupportedMode 64
#define BCExceptionInvalidateCharacter 65
#define BCExceptionBytesNotMatchOffset 66
#define BCExceptionSizeInBytesDiffer 67
#define BCExceptionInvalidateMaskPattern 68
#define BCExceptionNullPointer 69
#define BCExceptionBadMask 70
#define BCExceptionBitSizeNot15 71
#define BCExceptionBitSizeNot18 72
#define BCExceptionInvalidateImageData 73
#define BCExceptionHeight_8BeZero 74
#define BCExceptionCharacterNotThisMode 75
#define BCExceptionBitsBytesNotMatch 76
#define BCExceptionInvalidateData 77
#define BCExceptionLoadFile 78
#define BCExceptionPDF417EncodeFail 79
#define BCExceptionFailToCreateBitmap 80
#define BCExceptionLoadFontFail 81
#define BCExceptionOnlyEncodeCODEBAR 82
#define BCExceptionCodabarShouldStartWithOneOfABCD 83
#define BCExceptionCodabarShouldEndWithOneOfTNE 84
#define BCExceptionCodabarEncodeCharsInvalid 85
#define BCExceptionOnlyEncodeDATAMATRIX 86
#define BCExceptionCharactersOutsideISO88591Encoding 87
#define BCExceptionIllegalDataCodewords 88
#define BCExceptionCannotHandleThisNumberOfDataRegions 89
#define BCExceptionIllegalStateUnexpectedCase 90
#define BCExceptionIllegalStateCountMustNotExceed4 91
#define BCExceptionIllegalStateMessageLengthInvalid 92
#define BCExceptionIllegalArgumentNotGigits 93
#define BCExceptionIllegalStateIllegalMode 94
#define BCExceptionOnlyEncodePDF417 95
#define BCExceptionNonEncodableCharacterDetected 96
#define BCExceptionErrorCorrectionLevelMustBeBetween0And8 97
#define BCExceptionNoRecommendationPossible 98
#define BCExceptionIllegalArgumentnMustBeAbove0 99
#define BCExceptionUnableToFitMessageInColumns 100
#define BCExceptionEncodedMessageContainsTooManyCodeWords 101
#define BCExceptionBitmapSizeError 102
#define BCExceptionFormatInstance 102
#define BCExceptionChecksumInstance 103
#define BCExceptiontNotFoundInstance 104
#define BCExceptionNotFoundInstance 105
#define BCExceptionCannotMetadata 106
#define TWO_DIGIT_DATA_LENGTH_SIZE 24
#define THREE_DIGIT_DATA_LENGTH_SIZE 23
#define THREE_DIGIT_PLUS_DIGIT_DATA_LENGTH_SIZE 57
#define FOUR_DIGIT_DATA_LENGTH_SIZE 17
#define BC_EXCEPTION_CHECK_ReturnVoid(e) \
  if (e != BCExceptionNO)                \
    return;
#define BC_EXCEPTION_CHECK_ReturnValue(e, v) \
  if (e != BCExceptionNO)                    \
    return v;

#endif  // XFA_FXBARCODE_UTILS_H_
