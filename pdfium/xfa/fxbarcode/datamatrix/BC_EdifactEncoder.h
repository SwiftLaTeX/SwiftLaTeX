// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_EDIFACTENCODER_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_EDIFACTENCODER_H_

#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"

class CBC_EdifactEncoder : public CBC_Encoder {
 public:
  CBC_EdifactEncoder();
  ~CBC_EdifactEncoder() override;

  // CBC_Encoder
  int32_t getEncodingMode() override;
  void Encode(CBC_EncoderContext& context, int32_t& e) override;

 private:
  static void handleEOD(CBC_EncoderContext& context,
                        CFX_WideString buffer,
                        int32_t& e);
  static void encodeChar(FX_WCHAR c, CFX_WideString& sb, int32_t& e);
  static CFX_WideString encodeToCodewords(CFX_WideString sb,
                                          int32_t startPos,
                                          int32_t& e);
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_EDIFACTENCODER_H_
