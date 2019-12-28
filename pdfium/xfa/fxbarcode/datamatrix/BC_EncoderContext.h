// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_ENCODERCONTEXT_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_ENCODERCONTEXT_H_

#include "xfa/fxbarcode/datamatrix/BC_SymbolShapeHint.h"

class CBC_SymbolInfo;
class CBC_Dimension;

class CBC_EncoderContext : public CBC_SymbolShapeHint {
 public:
  CBC_EncoderContext(const CFX_WideString msg,
                     CFX_WideString ecLevel,
                     int32_t& e);
  ~CBC_EncoderContext() override;

  void setSymbolShape(SymbolShapeHint shape);
  void setSizeConstraints(CBC_Dimension* minSize, CBC_Dimension* maxSize);
  CFX_WideString getMessage();
  void setSkipAtEnd(int32_t count);
  FX_WCHAR getCurrentChar();
  FX_WCHAR getCurrent();
  void writeCodewords(CFX_WideString codewords);
  void writeCodeword(FX_WCHAR codeword);
  int32_t getCodewordCount();
  void signalEncoderChange(int32_t encoding);
  void resetEncoderSignal();
  FX_BOOL hasMoreCharacters();
  int32_t getRemainingCharacters();
  void updateSymbolInfo(int32_t& e);
  void updateSymbolInfo(int32_t len, int32_t& e);
  void resetSymbolInfo();

 public:
  CFX_WideString m_msg;
  CFX_WideString m_codewords;
  int32_t m_pos;
  int32_t m_newEncoding;
  CBC_SymbolInfo* m_symbolInfo;

 private:
  int32_t getTotalMessageCharCount();

 private:
  SymbolShapeHint m_shape;
  CBC_Dimension* m_minSize;
  CBC_Dimension* m_maxSize;
  int32_t m_skipAtEnd;
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_ENCODERCONTEXT_H_
