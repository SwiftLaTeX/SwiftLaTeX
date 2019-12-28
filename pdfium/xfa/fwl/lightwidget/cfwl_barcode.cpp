// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_barcode.h"

#include <memory>

CFWL_Barcode* CFWL_Barcode::Create() {
  return new CFWL_Barcode;
}

FWL_Error CFWL_Barcode::Initialize(const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_Barcode> pBarcode(IFWL_Barcode::Create(
      m_pProperties->MakeWidgetImpProperties(&m_barcodeData)));
  FWL_Error ret = pBarcode->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pBarcode.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

CFWL_Barcode::CFWL_Barcode() {}

CFWL_Barcode::~CFWL_Barcode() {}

void CFWL_Barcode::SetType(BC_TYPE type) {
  if (!m_pIface)
    return;
  static_cast<IFWL_Barcode*>(m_pIface)->SetType(type);
}

FX_BOOL CFWL_Barcode::IsProtectedType() {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_Barcode*>(m_pIface)->IsProtectedType();
}

CFWL_Barcode::CFWL_BarcodeDP::CFWL_BarcodeDP()
    : m_dwAttributeMask(FWL_BCDATTRIBUTE_NONE) {}

FWL_Error CFWL_Barcode::CFWL_BarcodeDP::GetCaption(IFWL_Widget* pWidget,
                                                   CFX_WideString& wsCaption) {
  return FWL_Error::Succeeded;
}

BC_CHAR_ENCODING CFWL_Barcode::CFWL_BarcodeDP::GetCharEncoding() {
  return m_eCharEncoding;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetModuleHeight() {
  return m_nModuleHeight;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetModuleWidth() {
  return m_nModuleWidth;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetDataLength() {
  return m_nDataLength;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetCalChecksum() {
  return m_nCalChecksum;
}

FX_BOOL CFWL_Barcode::CFWL_BarcodeDP::GetPrintChecksum() {
  return m_bPrintChecksum;
}

BC_TEXT_LOC CFWL_Barcode::CFWL_BarcodeDP::GetTextLocation() {
  return m_eTextLocation;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetWideNarrowRatio() {
  return m_nWideNarrowRatio;
}

FX_CHAR CFWL_Barcode::CFWL_BarcodeDP::GetStartChar() {
  return m_cStartChar;
}

FX_CHAR CFWL_Barcode::CFWL_BarcodeDP::GetEndChar() {
  return m_cEndChar;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetVersion() {
  return m_nVersion;
}

int32_t CFWL_Barcode::CFWL_BarcodeDP::GetErrorCorrectionLevel() {
  return m_nECLevel;
}

FX_BOOL CFWL_Barcode::CFWL_BarcodeDP::GetTruncated() {
  return m_bTruncated;
}

uint32_t CFWL_Barcode::CFWL_BarcodeDP::GetBarcodeAttributeMask() {
  return m_dwAttributeMask;
}
