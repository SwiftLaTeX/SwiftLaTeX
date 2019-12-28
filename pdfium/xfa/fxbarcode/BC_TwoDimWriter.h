// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_BC_TWODIMWRITER_H_
#define XFA_FXBARCODE_BC_TWODIMWRITER_H_

#include "xfa/fxbarcode/BC_Writer.h"

class CBC_CommonBitMatrix;
class CFX_RenderDevice;

class CBC_TwoDimWriter : public CBC_Writer {
 public:
  CBC_TwoDimWriter();
  ~CBC_TwoDimWriter() override;

  virtual void RenderResult(uint8_t* code,
                            int32_t codeWidth,
                            int32_t codeHeight,
                            int32_t& e);
  virtual void RenderBitmapResult(CFX_DIBitmap*& pOutBitmap, int32_t& e);
  virtual void RenderDeviceResult(CFX_RenderDevice* device,
                                  const CFX_Matrix* matrix);
  virtual FX_BOOL SetErrorCorrectionLevel(int32_t level) = 0;
  virtual int32_t GetErrorCorrectionLevel();

 protected:
  int32_t m_iCorrectLevel;
  FX_BOOL m_bFixedSize;
  CBC_CommonBitMatrix* m_output;
};

#endif  // XFA_FXBARCODE_BC_TWODIMWRITER_H_
