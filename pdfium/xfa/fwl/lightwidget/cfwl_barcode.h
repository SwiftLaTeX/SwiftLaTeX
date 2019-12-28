// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_BARCODE_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_BARCODE_H_

#include "xfa/fwl/basewidget/ifwl_barcode.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/lightwidget/cfwl_edit.h"

class CFWL_Widget;
class CFWL_WidgetProperties;

class CFWL_Barcode : public CFWL_Edit {
 public:
  static CFWL_Barcode* Create();
  FWL_Error Initialize(const CFWL_WidgetProperties* pProperties = nullptr);
  void SetType(BC_TYPE type);
  FX_BOOL IsProtectedType();

  void SetCharEncoding(BC_CHAR_ENCODING encoding) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_CHARENCODING;
    m_barcodeData.m_eCharEncoding = encoding;
  }
  void SetModuleHeight(int32_t height) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_MODULEHEIGHT;
    m_barcodeData.m_nModuleHeight = height;
  }
  void SetModuleWidth(int32_t width) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_MODULEWIDTH;
    m_barcodeData.m_nModuleWidth = width;
  }
  void SetDataLength(int32_t dataLength) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_DATALENGTH;
    m_barcodeData.m_nDataLength = dataLength;
    static_cast<IFWL_Barcode*>(m_pIface)->SetLimit(dataLength);
  }
  void SetCalChecksum(int32_t calChecksum) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_CALCHECKSUM;
    m_barcodeData.m_nCalChecksum = calChecksum;
  }
  void SetPrintChecksum(FX_BOOL printChecksum) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_PRINTCHECKSUM;
    m_barcodeData.m_bPrintChecksum = printChecksum;
  }
  void SetTextLocation(BC_TEXT_LOC location) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_TEXTLOCATION;
    m_barcodeData.m_eTextLocation = location;
  }
  void SetWideNarrowRatio(int32_t ratio) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_WIDENARROWRATIO;
    m_barcodeData.m_nWideNarrowRatio = ratio;
  }
  void SetStartChar(FX_CHAR startChar) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_STARTCHAR;
    m_barcodeData.m_cStartChar = startChar;
  }
  void SetEndChar(FX_CHAR endChar) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_ENDCHAR;
    m_barcodeData.m_cEndChar = endChar;
  }
  void SetVersion(int32_t version) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_VERSION;
    m_barcodeData.m_nVersion = version;
  }
  void SetErrorCorrectionLevel(int32_t ecLevel) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_ECLEVEL;
    m_barcodeData.m_nECLevel = ecLevel;
  }
  void SetTruncated(FX_BOOL truncated) {
    m_barcodeData.m_dwAttributeMask |= FWL_BCDATTRIBUTE_TRUNCATED;
    m_barcodeData.m_bTruncated = truncated;
  }
  void ResetBarcodeAttributes() {
    m_barcodeData.m_dwAttributeMask = FWL_BCDATTRIBUTE_NONE;
  }

 protected:
  CFWL_Barcode();
  ~CFWL_Barcode() override;

  class CFWL_BarcodeDP : public IFWL_BarcodeDP {
   public:
    CFWL_BarcodeDP();

    // IFWL_DataProvider
    FWL_Error GetCaption(IFWL_Widget* pWidget,
                         CFX_WideString& wsCaption) override;

    // IFWL_BarcodeDP
    BC_CHAR_ENCODING GetCharEncoding() override;
    int32_t GetModuleHeight() override;
    int32_t GetModuleWidth() override;
    int32_t GetDataLength() override;
    int32_t GetCalChecksum() override;
    FX_BOOL GetPrintChecksum() override;
    BC_TEXT_LOC GetTextLocation() override;
    int32_t GetWideNarrowRatio() override;
    FX_CHAR GetStartChar() override;
    FX_CHAR GetEndChar() override;
    int32_t GetVersion() override;
    int32_t GetErrorCorrectionLevel() override;
    FX_BOOL GetTruncated() override;
    uint32_t GetBarcodeAttributeMask() override;

    BC_CHAR_ENCODING m_eCharEncoding;
    int32_t m_nModuleHeight, m_nModuleWidth;
    int32_t m_nDataLength;
    int32_t m_nCalChecksum;
    FX_BOOL m_bPrintChecksum;
    BC_TEXT_LOC m_eTextLocation;
    int32_t m_nWideNarrowRatio;
    FX_CHAR m_cStartChar, m_cEndChar;
    int32_t m_nVersion;
    int32_t m_nECLevel;
    FX_BOOL m_bTruncated;
    uint32_t m_dwAttributeMask;
  };

  CFWL_BarcodeDP m_barcodeData;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_BARCODE_H_
