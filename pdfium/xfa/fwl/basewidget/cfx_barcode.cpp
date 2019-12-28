// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/cfx_barcode.h"

#include "xfa/fxbarcode/cbc_codabar.h"
#include "xfa/fxbarcode/cbc_code128.h"
#include "xfa/fxbarcode/cbc_code39.h"
#include "xfa/fxbarcode/cbc_codebase.h"
#include "xfa/fxbarcode/cbc_datamatrix.h"
#include "xfa/fxbarcode/cbc_ean13.h"
#include "xfa/fxbarcode/cbc_ean8.h"
#include "xfa/fxbarcode/cbc_pdf417i.h"
#include "xfa/fxbarcode/cbc_qrcode.h"
#include "xfa/fxbarcode/cbc_upca.h"
#include "xfa/fxbarcode/utils.h"

namespace {

CBC_CodeBase* CreateBarCodeEngineObject(BC_TYPE type) {
  switch (type) {
    case BC_CODE39:
      return new CBC_Code39();
    case BC_CODABAR:
      return new CBC_Codabar();
    case BC_CODE128:
      return new CBC_Code128(BC_CODE128_B);
    case BC_CODE128_B:
      return new CBC_Code128(BC_CODE128_B);
    case BC_CODE128_C:
      return new CBC_Code128(BC_CODE128_C);
    case BC_EAN8:
      return new CBC_EAN8();
    case BC_UPCA:
      return new CBC_UPCA();
    case BC_EAN13:
      return new CBC_EAN13();
    case BC_QR_CODE:
      return new CBC_QRCode();
    case BC_PDF417:
      return new CBC_PDF417I();
    case BC_DATAMATRIX:
      return new CBC_DataMatrix();
    case BC_UNKNOWN:
    default:
      return nullptr;
  }
}

}  // namespace

CFX_Barcode::CFX_Barcode() {}

CFX_Barcode::~CFX_Barcode() {
  delete m_pBCEngine;
}

FX_BOOL CFX_Barcode::Create(BC_TYPE type) {
  m_pBCEngine = CreateBarCodeEngineObject(type);
  return !!m_pBCEngine;
}
BC_TYPE CFX_Barcode::GetType() {
  return m_pBCEngine ? m_pBCEngine->GetType() : BC_UNKNOWN;
}
FX_BOOL CFX_Barcode::SetCharEncoding(BC_CHAR_ENCODING encoding) {
  return m_pBCEngine ? m_pBCEngine->SetCharEncoding(encoding) : FALSE;
}
FX_BOOL CFX_Barcode::SetModuleHeight(int32_t moduleHeight) {
  return m_pBCEngine ? m_pBCEngine->SetModuleHeight(moduleHeight) : FALSE;
}
FX_BOOL CFX_Barcode::SetModuleWidth(int32_t moduleWidth) {
  return m_pBCEngine ? m_pBCEngine->SetModuleWidth(moduleWidth) : FALSE;
}
FX_BOOL CFX_Barcode::SetHeight(int32_t height) {
  return m_pBCEngine ? m_pBCEngine->SetHeight(height) : FALSE;
}
FX_BOOL CFX_Barcode::SetWidth(int32_t width) {
  return m_pBCEngine ? m_pBCEngine->SetWidth(width) : FALSE;
}
FX_BOOL CFX_Barcode::CheckContentValidity(const CFX_WideStringC& contents) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine
                 ? static_cast<CBC_OneCode*>(m_pBCEngine)
                       ->CheckContentValidity(contents)
                 : TRUE;
    default:
      return TRUE;
  }
}
FX_BOOL CFX_Barcode::SetPrintChecksum(FX_BOOL checksum) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine)
                                ->SetPrintChecksum(checksum),
                            TRUE)
                         : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetDataLength(int32_t length) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine)
                                ->SetDataLength(length),
                            TRUE)
                         : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetCalChecksum(int32_t state) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine)
                                ->SetCalChecksum(state),
                            TRUE)
                         : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetFont(CFX_Font* pFont) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine
                 ? static_cast<CBC_OneCode*>(m_pBCEngine)->SetFont(pFont)
                 : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetFontSize(FX_FLOAT size) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine
                 ? (static_cast<CBC_OneCode*>(m_pBCEngine)->SetFontSize(size),
                    TRUE)
                 : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetFontStyle(int32_t style) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine
                 ? (static_cast<CBC_OneCode*>(m_pBCEngine)->SetFontStyle(style),
                    TRUE)
                 : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetFontColor(FX_ARGB color) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine
                 ? (static_cast<CBC_OneCode*>(m_pBCEngine)->SetFontColor(color),
                    TRUE)
                 : FALSE;
    default:
      return FALSE;
  }
}
FX_BOOL CFX_Barcode::SetTextLocation(BC_TEXT_LOC location) {
  typedef FX_BOOL (CBC_CodeBase::*memptrtype)(BC_TEXT_LOC);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_CODE39:
      memptr = (memptrtype)&CBC_Code39::SetTextLocation;
      break;
    case BC_CODABAR:
      memptr = (memptrtype)&CBC_Codabar::SetTextLocation;
      break;
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
      memptr = (memptrtype)&CBC_Code128::SetTextLocation;
      break;
    default:
      break;
  }
  return m_pBCEngine && memptr ? (m_pBCEngine->*memptr)(location) : FALSE;
}
FX_BOOL CFX_Barcode::SetWideNarrowRatio(int32_t ratio) {
  typedef FX_BOOL (CBC_CodeBase::*memptrtype)(int32_t);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_CODE39:
      memptr = (memptrtype)&CBC_Code39::SetWideNarrowRatio;
      break;
    case BC_CODABAR:
      memptr = (memptrtype)&CBC_Codabar::SetWideNarrowRatio;
      break;
    default:
      break;
  }
  return m_pBCEngine && memptr ? (m_pBCEngine->*memptr)(ratio) : FALSE;
}
FX_BOOL CFX_Barcode::SetStartChar(FX_CHAR start) {
  typedef FX_BOOL (CBC_CodeBase::*memptrtype)(FX_CHAR);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_CODABAR:
      memptr = (memptrtype)&CBC_Codabar::SetStartChar;
      break;
    default:
      break;
  }
  return m_pBCEngine && memptr ? (m_pBCEngine->*memptr)(start) : FALSE;
}
FX_BOOL CFX_Barcode::SetEndChar(FX_CHAR end) {
  typedef FX_BOOL (CBC_CodeBase::*memptrtype)(FX_CHAR);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_CODABAR:
      memptr = (memptrtype)&CBC_Codabar::SetEndChar;
      break;
    default:
      break;
  }
  return m_pBCEngine && memptr ? (m_pBCEngine->*memptr)(end) : FALSE;
}
FX_BOOL CFX_Barcode::SetVersion(int32_t version) {
  typedef FX_BOOL (CBC_CodeBase::*memptrtype)(int32_t);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_QR_CODE:
      memptr = (memptrtype)&CBC_QRCode::SetVersion;
      break;
    default:
      break;
  }
  return m_pBCEngine && memptr ? (m_pBCEngine->*memptr)(version) : FALSE;
}
FX_BOOL CFX_Barcode::SetErrorCorrectionLevel(int32_t level) {
  typedef FX_BOOL (CBC_CodeBase::*memptrtype)(int32_t);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_QR_CODE:
      memptr = (memptrtype)&CBC_QRCode::SetErrorCorrectionLevel;
      break;
    case BC_PDF417:
      memptr = (memptrtype)&CBC_PDF417I::SetErrorCorrectionLevel;
      break;
    default:
      return FALSE;
  }
  return m_pBCEngine && memptr ? (m_pBCEngine->*memptr)(level) : FALSE;
}
FX_BOOL CFX_Barcode::SetTruncated(FX_BOOL truncated) {
  typedef void (CBC_CodeBase::*memptrtype)(FX_BOOL);
  memptrtype memptr = nullptr;
  switch (GetType()) {
    case BC_PDF417:
      memptr = (memptrtype)&CBC_PDF417I::SetTruncated;
      break;
    default:
      break;
  }
  return m_pBCEngine && memptr ? ((m_pBCEngine->*memptr)(truncated), TRUE)
                               : FALSE;
}

FX_BOOL CFX_Barcode::Encode(const CFX_WideStringC& contents,
                            FX_BOOL isDevice,
                            int32_t& e) {
  if (!m_pBCEngine) {
    return FALSE;
  }
  return m_pBCEngine->Encode(contents, isDevice, e);
}
FX_BOOL CFX_Barcode::RenderDevice(CFX_RenderDevice* device,
                                  const CFX_Matrix* matrix,
                                  int32_t& e) {
  if (!m_pBCEngine) {
    return FALSE;
  }
  return m_pBCEngine->RenderDevice(device, matrix, e);
}
FX_BOOL CFX_Barcode::RenderBitmap(CFX_DIBitmap*& pOutBitmap, int32_t& e) {
  if (!m_pBCEngine) {
    return FALSE;
  }
  return m_pBCEngine->RenderBitmap(pOutBitmap, e);
}
