// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_DATAMATRIXWRITER_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_DATAMATRIXWRITER_H_

#include "xfa/fxbarcode/BC_TwoDimWriter.h"

class CBC_CommonByteMatrix;
class CBC_DefaultPlacement;
class CBC_SymbolInfo;

class CBC_DataMatrixWriter : public CBC_TwoDimWriter {
 public:
  CBC_DataMatrixWriter();
  ~CBC_DataMatrixWriter() override;

  virtual uint8_t* Encode(const CFX_WideString& contents,
                          int32_t& outWidth,
                          int32_t& outHeight,
                          int32_t& e);

  // CBC_TwoDimWriter
  FX_BOOL SetErrorCorrectionLevel(int32_t level) override;

 private:
  static CBC_CommonByteMatrix* encodeLowLevel(CBC_DefaultPlacement* placement,
                                              CBC_SymbolInfo* symbolInfo,
                                              int32_t& e);
  int32_t m_iCorrectLevel;
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_DATAMATRIXWRITER_H_
