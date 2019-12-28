// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxbarcode/BC_Writer.h"

CBC_Writer::CBC_Writer() {
  m_CharEncoding = 0;
  m_ModuleHeight = 1;
  m_ModuleWidth = 1;
  m_Height = 320;
  m_Width = 640;
  m_colorSpace = FXDIB_Argb;
  m_barColor = 0xff000000;
  m_backgroundColor = 0xffffffff;
}
CBC_Writer::~CBC_Writer() {}
FX_BOOL CBC_Writer::SetCharEncoding(int32_t encoding) {
  m_CharEncoding = encoding;
  return TRUE;
}
FX_BOOL CBC_Writer::SetModuleHeight(int32_t moduleHeight) {
  if (moduleHeight > 10 || moduleHeight < 1) {
    return FALSE;
  }
  m_ModuleHeight = moduleHeight;
  return TRUE;
}
FX_BOOL CBC_Writer::SetModuleWidth(int32_t moduleWidth) {
  if (moduleWidth > 10 || moduleWidth < 1) {
    return FALSE;
  }
  m_ModuleWidth = moduleWidth;
  return TRUE;
}
FX_BOOL CBC_Writer::SetHeight(int32_t height) {
  m_Height = height;
  return TRUE;
}
FX_BOOL CBC_Writer::SetWidth(int32_t width) {
  m_Width = width;
  return TRUE;
}
void CBC_Writer::SetBackgroundColor(FX_ARGB backgroundColor) {
  m_backgroundColor = backgroundColor;
}
void CBC_Writer::SetBarcodeColor(FX_ARGB foregroundColor) {
  m_barColor = foregroundColor;
}
CFX_DIBitmap* CBC_Writer::CreateDIBitmap(int32_t width, int32_t height) {
  CFX_DIBitmap* pDIBitmap = new CFX_DIBitmap;
  pDIBitmap->Create(width, height, m_colorSpace);
  return pDIBitmap;
}
