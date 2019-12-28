// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODERMATRIXUTIL_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODERMATRIXUTIL_H_

class CBC_CommonByteMatrix;
class CBC_QRCoderErrorCorrectionLevel;
class CBC_QRCoderBitVector;
class CBC_QRCoderMatrixUtil {
 private:
  static const int32_t POSITION_DETECTION_PATTERN[7][7];
  static const int32_t VERTICAL_SEPARATION_PATTERN[7][1];
  static const int32_t HORIZONTAL_SEPARATION_PATTERN[1][8];
  static const int32_t POSITION_ADJUSTMENT_PATTERN[5][5];
  static const int32_t POSITION_ADJUSTMENT_PATTERN_COORDINATE_TABLE[40][7];
  static const int32_t TYPE_INFO_COORDINATES[15][2];
  static const int32_t VERSION_INFO_POLY;
  static const int32_t TYPE_INFO_POLY;
  static const int32_t TYPE_INFO_MASK_PATTERN;

 public:
  CBC_QRCoderMatrixUtil();
  virtual ~CBC_QRCoderMatrixUtil();
  static void ClearMatrix(CBC_CommonByteMatrix* matrix, int32_t& e);
  static void BuildMatrix(CBC_QRCoderBitVector* dataBits,
                          CBC_QRCoderErrorCorrectionLevel* ecLevel,
                          int32_t version,
                          int32_t maskPattern,
                          CBC_CommonByteMatrix* matrix,
                          int32_t& e);
  static void EmbedBasicPatterns(int32_t version,
                                 CBC_CommonByteMatrix* matrix,
                                 int32_t& e);
  static void EmbedTypeInfo(CBC_QRCoderErrorCorrectionLevel* ecLevel,
                            int32_t maskPattern,
                            CBC_CommonByteMatrix* matrix,
                            int32_t& e);
  static void EmbedDataBits(CBC_QRCoderBitVector* dataBits,
                            int32_t maskPattern,
                            CBC_CommonByteMatrix* matrix,
                            int32_t& e);
  static void MaybeEmbedVersionInfo(int32_t version,
                                    CBC_CommonByteMatrix* matrix,
                                    int32_t& e);
  static int32_t FindMSBSet(int32_t value);
  static int32_t CalculateBCHCode(int32_t code, int32_t poly);
  static void MakeTypeInfoBits(CBC_QRCoderErrorCorrectionLevel* ecLevel,
                               int32_t maskPattern,
                               CBC_QRCoderBitVector* bits,
                               int32_t& e);
  static void MakeVersionInfoBits(int32_t version,
                                  CBC_QRCoderBitVector* bits,
                                  int32_t& e);
  static FX_BOOL IsEmpty(int32_t value);
  static FX_BOOL IsValidValue(int32_t value);
  static void EmbedTimingPatterns(CBC_CommonByteMatrix* matrix, int32_t& e);
  static void EmbedDarkDotAtLeftBottomCorner(CBC_CommonByteMatrix* matrix,
                                             int32_t& e);
  static void EmbedHorizontalSeparationPattern(int32_t xStart,
                                               int32_t yStart,
                                               CBC_CommonByteMatrix* matrix,
                                               int32_t& e);
  static void EmbedVerticalSeparationPattern(int32_t xStart,
                                             int32_t yStart,
                                             CBC_CommonByteMatrix* matrix,
                                             int32_t& e);
  static void EmbedPositionAdjustmentPattern(int32_t xStart,
                                             int32_t yStart,
                                             CBC_CommonByteMatrix* matrix,
                                             int32_t& e);
  static void EmbedPositionDetectionPattern(int32_t xStart,
                                            int32_t yStart,
                                            CBC_CommonByteMatrix* matrix,
                                            int32_t& e);
  static void EmbedPositionDetectionPatternsAndSeparators(
      CBC_CommonByteMatrix* matrix,
      int32_t& e);
  static void MaybeEmbedPositionAdjustmentPatterns(int32_t version,
                                                   CBC_CommonByteMatrix* matrix,
                                                   int32_t& e);
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODERMATRIXUTIL_H_
