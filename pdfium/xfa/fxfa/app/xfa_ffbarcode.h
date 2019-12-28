// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFBARCODE_H_
#define XFA_FXFA_APP_XFA_FFBARCODE_H_

#include "xfa/fxbarcode/include/BC_Library.h"
#include "xfa/fxfa/app/xfa_fftextedit.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"

class CXFA_FFBarcode : public CXFA_FFTextEdit {
 public:
  CXFA_FFBarcode(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFBarcode() override;

  // CXFA_FFTextEdit
  FX_BOOL LoadWidget() override;
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
  void UpdateWidgetProperty() override;
  FX_BOOL OnLButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
  FX_BOOL OnRButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
};

enum XFA_BARCODETYPEENUM {
  XFA_BARCODETYPE_aztec,
  XFA_BARCODETYPE_codabar,
  XFA_BARCODETYPE_code11,
  XFA_BARCODETYPE_code128,
  XFA_BARCODETYPE_code128A,
  XFA_BARCODETYPE_code128B,
  XFA_BARCODETYPE_code128C,
  XFA_BARCODETYPE_code128SSCC,
  XFA_BARCODETYPE_code2Of5Industrial,
  XFA_BARCODETYPE_code2Of5Interleaved,
  XFA_BARCODETYPE_code2Of5Matrix,
  XFA_BARCODETYPE_code2Of5Standard,
  XFA_BARCODETYPE_code3Of9,
  XFA_BARCODETYPE_code3Of9extended,
  XFA_BARCODETYPE_code49,
  XFA_BARCODETYPE_code93,
  XFA_BARCODETYPE_dataMatrix,
  XFA_BARCODETYPE_ean13,
  XFA_BARCODETYPE_ean13add2,
  XFA_BARCODETYPE_ean13add5,
  XFA_BARCODETYPE_ean13pwcd,
  XFA_BARCODETYPE_ean8,
  XFA_BARCODETYPE_ean8add2,
  XFA_BARCODETYPE_ean8add5,
  XFA_BARCODETYPE_fim,
  XFA_BARCODETYPE_logmars,
  XFA_BARCODETYPE_maxicode,
  XFA_BARCODETYPE_msi,
  XFA_BARCODETYPE_pdf417,
  XFA_BARCODETYPE_pdf417macro,
  XFA_BARCODETYPE_plessey,
  XFA_BARCODETYPE_postAUSCust2,
  XFA_BARCODETYPE_postAUSCust3,
  XFA_BARCODETYPE_postAUSReplyPaid,
  XFA_BARCODETYPE_postAUSStandard,
  XFA_BARCODETYPE_postUKRM4SCC,
  XFA_BARCODETYPE_postUS5Zip,
  XFA_BARCODETYPE_postUSDPBC,
  XFA_BARCODETYPE_postUSIMB,
  XFA_BARCODETYPE_postUSStandard,
  XFA_BARCODETYPE_QRCode,
  XFA_BARCODETYPE_rfid,
  XFA_BARCODETYPE_rss14,
  XFA_BARCODETYPE_rss14Expanded,
  XFA_BARCODETYPE_rss14Limited,
  XFA_BARCODETYPE_rss14Stacked,
  XFA_BARCODETYPE_rss14StackedOmni,
  XFA_BARCODETYPE_rss14Truncated,
  XFA_BARCODETYPE_telepen,
  XFA_BARCODETYPE_ucc128,
  XFA_BARCODETYPE_ucc128random,
  XFA_BARCODETYPE_ucc128sscc,
  XFA_BARCODETYPE_upcA,
  XFA_BARCODETYPE_upcAadd2,
  XFA_BARCODETYPE_upcAadd5,
  XFA_BARCODETYPE_upcApwcd,
  XFA_BARCODETYPE_upcE,
  XFA_BARCODETYPE_upcEadd2,
  XFA_BARCODETYPE_upcEadd5,
  XFA_BARCODETYPE_upcean2,
  XFA_BARCODETYPE_upcean5,
  XFA_BARCODETYPE_upsMaxicode
};
struct XFA_BARCODETYPEENUMINFO {
  uint32_t uHash;
  const FX_WCHAR* pName;
  XFA_BARCODETYPEENUM eName;
  BC_TYPE eBCType;
};
typedef XFA_BARCODETYPEENUMINFO const* XFA_LPCBARCODETYPEENUMINFO;

#endif  // XFA_FXFA_APP_XFA_FFBARCODE_H_
