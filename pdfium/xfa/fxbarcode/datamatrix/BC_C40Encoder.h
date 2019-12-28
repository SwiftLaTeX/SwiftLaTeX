// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_C40ENCODER_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_C40ENCODER_H_

#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"

class CBC_C40Encoder : public CBC_Encoder {
 public:
  CBC_C40Encoder();
  ~CBC_C40Encoder() override;

  // CBC_Encoder
  int32_t getEncodingMode() override;
  void Encode(CBC_EncoderContext& context, int32_t& e) override;

  static void writeNextTriplet(CBC_EncoderContext& context,
                               CFX_WideString& buffer);

  virtual void handleEOD(CBC_EncoderContext& context,
                         CFX_WideString& buffer,
                         int32_t& e);
  virtual int32_t encodeChar(FX_WCHAR c, CFX_WideString& sb, int32_t& e);

 private:
  int32_t backtrackOneCharacter(CBC_EncoderContext& context,
                                CFX_WideString& buffer,
                                CFX_WideString& removed,
                                int32_t lastCharSize,
                                int32_t& e);
  static CFX_WideString encodeToCodewords(CFX_WideString sb, int32_t startPos);
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_C40ENCODER_H_
