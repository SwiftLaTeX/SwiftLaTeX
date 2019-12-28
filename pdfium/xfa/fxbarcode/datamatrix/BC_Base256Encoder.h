// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_BASE256ENCODER_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_BASE256ENCODER_H_

#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"

class CBC_Base256Encoder : public CBC_Encoder {
 public:
  CBC_Base256Encoder();
  ~CBC_Base256Encoder() override;

  // CBC_Encoder
  int32_t getEncodingMode() override;
  void Encode(CBC_EncoderContext& context, int32_t& e) override;

 private:
  static FX_WCHAR randomize255State(FX_WCHAR ch, int32_t codewordPosition);
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_BASE256ENCODER_H_
