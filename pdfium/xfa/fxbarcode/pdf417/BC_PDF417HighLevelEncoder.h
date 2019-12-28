// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_PDF417_BC_PDF417HIGHLEVELENCODER_H_
#define XFA_FXBARCODE_PDF417_BC_PDF417HIGHLEVELENCODER_H_

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417Compaction.h"

class CBC_PDF417HighLevelEncoder {
 public:
  static CFX_WideString encodeHighLevel(CFX_WideString msg,
                                        Compaction compaction,
                                        int32_t& e);
  static void Inverse();
  static void Initialize();
  static void Finalize();

 private:
  static int32_t TEXT_COMPACTION;
  static int32_t BYTE_COMPACTION;
  static int32_t NUMERIC_COMPACTION;
  static int32_t SUBMODE_PUNCTUATION;
  static int32_t LATCH_TO_TEXT;
  static int32_t LATCH_TO_BYTE_PADDED;
  static int32_t LATCH_TO_NUMERIC;
  static int32_t SHIFT_TO_BYTE;
  static int32_t LATCH_TO_BYTE;
  static uint8_t TEXT_MIXED_RAW[];
  static uint8_t TEXT_PUNCTUATION_RAW[];
  static int32_t MIXED[128];
  static int32_t PUNCTUATION[128];
  static int32_t encodeText(CFX_WideString msg,
                            int32_t startpos,
                            int32_t count,
                            CFX_WideString& sb,
                            int32_t initialSubmode);
  static void encodeBinary(CFX_ByteArray* bytes,
                           int32_t startpos,
                           int32_t count,
                           int32_t startmode,
                           CFX_WideString& sb);
  static void encodeNumeric(CFX_WideString msg,
                            int32_t startpos,
                            int32_t count,
                            CFX_WideString& sb);
  static FX_BOOL isDigit(FX_WCHAR ch);
  static FX_BOOL isAlphaUpper(FX_WCHAR ch);
  static FX_BOOL isAlphaLower(FX_WCHAR ch);
  static FX_BOOL isMixed(FX_WCHAR ch);
  static FX_BOOL isPunctuation(FX_WCHAR ch);
  static FX_BOOL isText(FX_WCHAR ch);
  static int32_t determineConsecutiveDigitCount(CFX_WideString msg,
                                                int32_t startpos);
  static int32_t determineConsecutiveTextCount(CFX_WideString msg,
                                               int32_t startpos);
  static int32_t determineConsecutiveBinaryCount(CFX_WideString msg,
                                                 CFX_ByteArray* bytes,
                                                 int32_t startpos,
                                                 int32_t& e);

  friend class PDF417HighLevelEncoder_EncodeNumeric_Test;
  friend class PDF417HighLevelEncoder_EncodeBinary_Test;
  friend class PDF417HighLevelEncoder_EncodeText_Test;
  friend class PDF417HighLevelEncoder_ConsecutiveDigitCount_Test;
  friend class PDF417HighLevelEncoder_ConsecutiveTextCount_Test;
  friend class PDF417HighLevelEncoder_ConsecutiveBinaryCount_Test;
};

#endif  // XFA_FXBARCODE_PDF417_BC_PDF417HIGHLEVELENCODER_H_
