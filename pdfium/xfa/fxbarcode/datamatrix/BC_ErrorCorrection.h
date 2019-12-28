// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_ERRORCORRECTION_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_ERRORCORRECTION_H_

class CBC_SymbolInfo;

class CBC_ErrorCorrection {
 public:
  CBC_ErrorCorrection();
  virtual ~CBC_ErrorCorrection();

  static void Initialize();
  static void Finalize();
  static CFX_WideString encodeECC200(CFX_WideString codewords,
                                     CBC_SymbolInfo* symbolInfo,
                                     int32_t& e);

 private:
  static int32_t MODULO_VALUE;
  static int32_t LOG[256];
  static int32_t ALOG[256];

 private:
  static CFX_WideString createECCBlock(CFX_WideString codewords,
                                       int32_t numECWords,
                                       int32_t& e);
  static CFX_WideString createECCBlock(CFX_WideString codewords,
                                       int32_t start,
                                       int32_t len,
                                       int32_t numECWords,
                                       int32_t& e);
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_ERRORCORRECTION_H_
