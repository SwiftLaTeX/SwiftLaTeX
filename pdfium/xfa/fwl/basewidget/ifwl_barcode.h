// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_BARCODE_H_
#define XFA_FWL_BASEWIDGET_IFWL_BARCODE_H_

#include "xfa/fxbarcode/include/BC_Library.h"
#include "xfa/fwl/basewidget/ifwl_edit.h"

class CFWL_WidgetImpProperties;

#define FWL_CLASS_Barcode L"FWL_BARCODE"

enum FWL_BCDAttribute {
  FWL_BCDATTRIBUTE_NONE = 0,
  FWL_BCDATTRIBUTE_CHARENCODING = 1 << 0,
  FWL_BCDATTRIBUTE_MODULEHEIGHT = 1 << 1,
  FWL_BCDATTRIBUTE_MODULEWIDTH = 1 << 2,
  FWL_BCDATTRIBUTE_DATALENGTH = 1 << 3,
  FWL_BCDATTRIBUTE_CALCHECKSUM = 1 << 4,
  FWL_BCDATTRIBUTE_PRINTCHECKSUM = 1 << 5,
  FWL_BCDATTRIBUTE_TEXTLOCATION = 1 << 6,
  FWL_BCDATTRIBUTE_WIDENARROWRATIO = 1 << 7,
  FWL_BCDATTRIBUTE_STARTCHAR = 1 << 8,
  FWL_BCDATTRIBUTE_ENDCHAR = 1 << 9,
  FWL_BCDATTRIBUTE_VERSION = 1 << 10,
  FWL_BCDATTRIBUTE_ECLEVEL = 1 << 11,
  FWL_BCDATTRIBUTE_TRUNCATED = 1 << 12
};

class IFWL_BarcodeDP : public IFWL_EditDP {
 public:
  virtual BC_CHAR_ENCODING GetCharEncoding() = 0;
  virtual int32_t GetModuleHeight() = 0;
  virtual int32_t GetModuleWidth() = 0;
  virtual int32_t GetDataLength() = 0;
  virtual int32_t GetCalChecksum() = 0;
  virtual FX_BOOL GetPrintChecksum() = 0;
  virtual BC_TEXT_LOC GetTextLocation() = 0;
  virtual int32_t GetWideNarrowRatio() = 0;
  virtual FX_CHAR GetStartChar() = 0;
  virtual FX_CHAR GetEndChar() = 0;
  virtual int32_t GetVersion() = 0;
  virtual int32_t GetErrorCorrectionLevel() = 0;
  virtual FX_BOOL GetTruncated() = 0;
  virtual uint32_t GetBarcodeAttributeMask() = 0;
};

class IFWL_Barcode : public IFWL_Edit {
 public:
  static IFWL_Barcode* Create(const CFWL_WidgetImpProperties& properties);
  void SetType(BC_TYPE type);
  FX_BOOL IsProtectedType();

 protected:
  IFWL_Barcode();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_BARCODE_H_
