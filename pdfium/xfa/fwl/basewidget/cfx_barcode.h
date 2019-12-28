// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_CFX_BARCODE_H_
#define XFA_FWL_BASEWIDGET_CFX_BARCODE_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"
#include "xfa/fxbarcode/include/BC_Library.h"

class CBC_CodeBase;
class CFX_Font;
class CFX_RenderDevice;
class CFX_Matrix;

class CFX_Barcode {
 public:
  CFX_Barcode();
  ~CFX_Barcode();

  FX_BOOL Create(BC_TYPE type);
  BC_TYPE GetType();
  FX_BOOL Encode(const CFX_WideStringC& contents, FX_BOOL isDevice, int32_t& e);
  FX_BOOL RenderDevice(CFX_RenderDevice* device,
                       const CFX_Matrix* matrix,
                       int32_t& e);
  FX_BOOL RenderBitmap(CFX_DIBitmap*& pOutBitmap, int32_t& e);
  FX_BOOL SetCharEncoding(BC_CHAR_ENCODING encoding);
  FX_BOOL SetModuleHeight(int32_t moduleHeight);
  FX_BOOL SetModuleWidth(int32_t moduleWidth);
  FX_BOOL SetHeight(int32_t height);
  FX_BOOL SetWidth(int32_t width);
  FX_BOOL CheckContentValidity(const CFX_WideStringC& contents);
  FX_BOOL SetPrintChecksum(FX_BOOL checksum);
  FX_BOOL SetDataLength(int32_t length);
  FX_BOOL SetCalChecksum(int32_t state);
  FX_BOOL SetFont(CFX_Font* pFont);
  FX_BOOL SetFontSize(FX_FLOAT size);
  FX_BOOL SetFontStyle(int32_t style);
  FX_BOOL SetFontColor(FX_ARGB color);
  FX_BOOL SetTextLocation(BC_TEXT_LOC location);
  FX_BOOL SetWideNarrowRatio(int32_t ratio);
  FX_BOOL SetStartChar(FX_CHAR start);
  FX_BOOL SetEndChar(FX_CHAR end);
  FX_BOOL SetVersion(int32_t version);
  FX_BOOL SetErrorCorrectionLevel(int32_t level);
  FX_BOOL SetTruncated(FX_BOOL truncated);

 protected:
  CBC_CodeBase* m_pBCEngine;
};

#endif  // XFA_FWL_BASEWIDGET_CFX_BARCODE_H_
