// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffbarcode.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/ifwl_app.h"
#include "xfa/fwl/lightwidget/cfwl_barcode.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_fftextedit.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"

namespace {

const XFA_BARCODETYPEENUMINFO g_XFABarCodeTypeEnumData[] = {
    {0x7fb4a18, L"ean13", XFA_BARCODETYPE_ean13, BC_EAN13},
    {0x8d13a3d, L"code11", XFA_BARCODETYPE_code11, BC_UNKNOWN},
    {0x8d149a8, L"code49", XFA_BARCODETYPE_code49, BC_UNKNOWN},
    {0x8d16347, L"code93", XFA_BARCODETYPE_code93, BC_UNKNOWN},
    {0x91a92e2, L"upsMaxicode", XFA_BARCODETYPE_upsMaxicode, BC_UNKNOWN},
    {0xa7d48dc, L"fim", XFA_BARCODETYPE_fim, BC_UNKNOWN},
    {0xb359fe9, L"msi", XFA_BARCODETYPE_msi, BC_UNKNOWN},
    {0x121f738c, L"code2Of5Matrix", XFA_BARCODETYPE_code2Of5Matrix, BC_UNKNOWN},
    {0x15358616, L"ucc128", XFA_BARCODETYPE_ucc128, BC_UNKNOWN},
    {0x1f4bfa05, L"rfid", XFA_BARCODETYPE_rfid, BC_UNKNOWN},
    {0x1fda71bc, L"rss14Stacked", XFA_BARCODETYPE_rss14Stacked, BC_UNKNOWN},
    {0x22065087, L"ean8add2", XFA_BARCODETYPE_ean8add2, BC_UNKNOWN},
    {0x2206508a, L"ean8add5", XFA_BARCODETYPE_ean8add5, BC_UNKNOWN},
    {0x2278366c, L"codabar", XFA_BARCODETYPE_codabar, BC_CODABAR},
    {0x2a039a8d, L"telepen", XFA_BARCODETYPE_telepen, BC_UNKNOWN},
    {0x323ed337, L"upcApwcd", XFA_BARCODETYPE_upcApwcd, BC_UNKNOWN},
    {0x347a1846, L"postUSIMB", XFA_BARCODETYPE_postUSIMB, BC_UNKNOWN},
    {0x391bb836, L"code128", XFA_BARCODETYPE_code128, BC_CODE128},
    {0x398eddaf, L"dataMatrix", XFA_BARCODETYPE_dataMatrix, BC_DATAMATRIX},
    {0x3cff60a8, L"upcEadd2", XFA_BARCODETYPE_upcEadd2, BC_UNKNOWN},
    {0x3cff60ab, L"upcEadd5", XFA_BARCODETYPE_upcEadd5, BC_UNKNOWN},
    {0x402cb188, L"code2Of5Standard", XFA_BARCODETYPE_code2Of5Standard,
     BC_UNKNOWN},
    {0x411764f7, L"aztec", XFA_BARCODETYPE_aztec, BC_UNKNOWN},
    {0x44d4e84c, L"ean8", XFA_BARCODETYPE_ean8, BC_EAN8},
    {0x48468902, L"ucc128sscc", XFA_BARCODETYPE_ucc128sscc, BC_UNKNOWN},
    {0x4880aea4, L"upcAadd2", XFA_BARCODETYPE_upcAadd2, BC_UNKNOWN},
    {0x4880aea7, L"upcAadd5", XFA_BARCODETYPE_upcAadd5, BC_UNKNOWN},
    {0x54f18256, L"code2Of5Industrial", XFA_BARCODETYPE_code2Of5Industrial,
     BC_UNKNOWN},
    {0x58e15f25, L"rss14Limited", XFA_BARCODETYPE_rss14Limited, BC_UNKNOWN},
    {0x5c08d1b9, L"postAUSReplyPaid", XFA_BARCODETYPE_postAUSReplyPaid,
     BC_UNKNOWN},
    {0x5fa700bd, L"rss14", XFA_BARCODETYPE_rss14, BC_UNKNOWN},
    {0x631a7e35, L"logmars", XFA_BARCODETYPE_logmars, BC_UNKNOWN},
    {0x6a236236, L"pdf417", XFA_BARCODETYPE_pdf417, BC_PDF417},
    {0x6d098ece, L"upcean2", XFA_BARCODETYPE_upcean2, BC_UNKNOWN},
    {0x6d098ed1, L"upcean5", XFA_BARCODETYPE_upcean5, BC_UNKNOWN},
    {0x76b04eed, L"code3Of9extended", XFA_BARCODETYPE_code3Of9extended,
     BC_UNKNOWN},
    {0x7c7db84a, L"maxicode", XFA_BARCODETYPE_maxicode, BC_UNKNOWN},
    {0x8266f7f7, L"ucc128random", XFA_BARCODETYPE_ucc128random, BC_UNKNOWN},
    {0x83eca147, L"postUSDPBC", XFA_BARCODETYPE_postUSDPBC, BC_UNKNOWN},
    {0x8dd71de0, L"postAUSStandard", XFA_BARCODETYPE_postAUSStandard,
     BC_UNKNOWN},
    {0x98adad85, L"plessey", XFA_BARCODETYPE_plessey, BC_UNKNOWN},
    {0x9f84cce6, L"ean13pwcd", XFA_BARCODETYPE_ean13pwcd, BC_UNKNOWN},
    {0xb514fbe9, L"upcA", XFA_BARCODETYPE_upcA, BC_UPCA},
    {0xb514fbed, L"upcE", XFA_BARCODETYPE_upcE, BC_UNKNOWN},
    {0xb5c6a853, L"ean13add2", XFA_BARCODETYPE_ean13add2, BC_UNKNOWN},
    {0xb5c6a856, L"ean13add5", XFA_BARCODETYPE_ean13add5, BC_UNKNOWN},
    {0xb81fc512, L"postUKRM4SCC", XFA_BARCODETYPE_postUKRM4SCC, BC_UNKNOWN},
    {0xbad34b22, L"code128SSCC", XFA_BARCODETYPE_code128SSCC, BC_UNKNOWN},
    {0xbfbe0cf6, L"postUS5Zip", XFA_BARCODETYPE_postUS5Zip, BC_UNKNOWN},
    {0xc56618e8, L"pdf417macro", XFA_BARCODETYPE_pdf417macro, BC_UNKNOWN},
    {0xca730f8a, L"code2Of5Interleaved", XFA_BARCODETYPE_code2Of5Interleaved,
     BC_UNKNOWN},
    {0xd0097ac6, L"rss14Expanded", XFA_BARCODETYPE_rss14Expanded, BC_UNKNOWN},
    {0xd25a0240, L"postAUSCust2", XFA_BARCODETYPE_postAUSCust2, BC_UNKNOWN},
    {0xd25a0241, L"postAUSCust3", XFA_BARCODETYPE_postAUSCust3, BC_UNKNOWN},
    {0xd53ed3e7, L"rss14Truncated", XFA_BARCODETYPE_rss14Truncated, BC_UNKNOWN},
    {0xe72bcd57, L"code128A", XFA_BARCODETYPE_code128A, BC_UNKNOWN},
    {0xe72bcd58, L"code128B", XFA_BARCODETYPE_code128B, BC_CODE128_B},
    {0xe72bcd59, L"code128C", XFA_BARCODETYPE_code128C, BC_CODE128_C},
    {0xee83c50f, L"rss14StackedOmni", XFA_BARCODETYPE_rss14StackedOmni,
     BC_UNKNOWN},
    {0xf2a18f7e, L"QRCode", XFA_BARCODETYPE_QRCode, BC_QR_CODE},
    {0xfaeaf37f, L"postUSStandard", XFA_BARCODETYPE_postUSStandard, BC_UNKNOWN},
    {0xfb48155c, L"code3Of9", XFA_BARCODETYPE_code3Of9, BC_CODE39},
};
const int32_t g_iXFABarcodeTypeCount =
    sizeof(g_XFABarCodeTypeEnumData) / sizeof(XFA_BARCODETYPEENUMINFO);

XFA_LPCBARCODETYPEENUMINFO XFA_GetBarcodeTypeByName(
    const CFX_WideStringC& wsName) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t uHash = FX_HashCode_GetW(wsName, true);
  int32_t iStart = 0;
  int32_t iEnd = g_iXFABarcodeTypeCount - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    XFA_LPCBARCODETYPEENUMINFO pInfo = g_XFABarCodeTypeEnumData + iMid;
    if (uHash == pInfo->uHash) {
      return pInfo;
    } else if (uHash < pInfo->uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}

}  // namespace.

CXFA_FFBarcode::CXFA_FFBarcode(CXFA_FFPageView* pPageView,
                               CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFTextEdit(pPageView, pDataAcc) {}
CXFA_FFBarcode::~CXFA_FFBarcode() {}
FX_BOOL CXFA_FFBarcode::LoadWidget() {
  CFWL_Barcode* pFWLBarcode = CFWL_Barcode::Create();
  if (pFWLBarcode) {
    pFWLBarcode->Initialize();
  }
  m_pNormalWidget = pFWLBarcode;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pWidget, pWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Display);
  pFWLBarcode->SetText(wsText);
  UpdateWidgetProperty();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFBarcode::RenderWidget(CFX_Graphics* pGS,
                                  CFX_Matrix* pMatrix,
                                  uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  CFX_Matrix mtRotate;
  GetRotateMatrix(mtRotate);
  if (pMatrix) {
    mtRotate.Concat(*pMatrix);
  }
  CXFA_FFWidget::RenderWidget(pGS, &mtRotate, dwStatus);
  CXFA_Border borderUI = m_pDataAcc->GetUIBorder();
  DrawBorder(pGS, borderUI, m_rtUI, &mtRotate);
  RenderCaption(pGS, &mtRotate);
  CFX_RectF rtWidget;
  m_pNormalWidget->GetWidgetRect(rtWidget);
  CFX_Matrix mt;
  mt.Set(1, 0, 0, 1, rtWidget.left, rtWidget.top);
  mt.Concat(mtRotate);
  m_pNormalWidget->DrawWidget(pGS, &mt);
}
void CXFA_FFBarcode::UpdateWidgetProperty() {
  CXFA_FFTextEdit::UpdateWidgetProperty();
  CFWL_Barcode* pBarCodeWidget = (CFWL_Barcode*)m_pNormalWidget;
  CFX_WideString wsType = GetDataAcc()->GetBarcodeType();
  XFA_LPCBARCODETYPEENUMINFO pBarcodeTypeInfo =
      XFA_GetBarcodeTypeByName(wsType.AsStringC());
  pBarCodeWidget->SetType(pBarcodeTypeInfo->eBCType);
  CXFA_WidgetAcc* pAcc = GetDataAcc();
  int32_t intVal;
  FX_CHAR charVal;
  FX_BOOL boolVal;
  FX_FLOAT floatVal;
  if (pAcc->GetBarcodeAttribute_CharEncoding(intVal)) {
    pBarCodeWidget->SetCharEncoding((BC_CHAR_ENCODING)intVal);
  }
  if (pAcc->GetBarcodeAttribute_Checksum(intVal)) {
    pBarCodeWidget->SetCalChecksum(intVal);
  }
  if (pAcc->GetBarcodeAttribute_DataLength(intVal)) {
    pBarCodeWidget->SetDataLength(intVal);
  }
  if (pAcc->GetBarcodeAttribute_StartChar(charVal)) {
    pBarCodeWidget->SetStartChar(charVal);
  }
  if (pAcc->GetBarcodeAttribute_EndChar(charVal)) {
    pBarCodeWidget->SetEndChar(charVal);
  }
  if (pAcc->GetBarcodeAttribute_ECLevel(intVal)) {
    pBarCodeWidget->SetErrorCorrectionLevel(intVal);
  }
  if (pAcc->GetBarcodeAttribute_ModuleWidth(intVal)) {
    pBarCodeWidget->SetModuleWidth(intVal);
  }
  if (pAcc->GetBarcodeAttribute_ModuleHeight(intVal)) {
    pBarCodeWidget->SetModuleHeight(intVal);
  }
  if (pAcc->GetBarcodeAttribute_PrintChecksum(boolVal)) {
    pBarCodeWidget->SetPrintChecksum(boolVal);
  }
  if (pAcc->GetBarcodeAttribute_TextLocation(intVal)) {
    pBarCodeWidget->SetTextLocation((BC_TEXT_LOC)intVal);
  }
  if (pAcc->GetBarcodeAttribute_Truncate(boolVal)) {
    pBarCodeWidget->SetTruncated(boolVal);
  }
  if (pAcc->GetBarcodeAttribute_WideNarrowRatio(floatVal)) {
    pBarCodeWidget->SetWideNarrowRatio((int32_t)floatVal);
  }
  if (pBarcodeTypeInfo->eName == XFA_BARCODETYPE_code3Of9 ||
      pBarcodeTypeInfo->eName == XFA_BARCODETYPE_ean8 ||
      pBarcodeTypeInfo->eName == XFA_BARCODETYPE_ean13 ||
      pBarcodeTypeInfo->eName == XFA_BARCODETYPE_upcA) {
    pBarCodeWidget->SetPrintChecksum(TRUE);
  }
}
FX_BOOL CXFA_FFBarcode::OnLButtonDown(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  CFWL_Barcode* pBarCodeWidget = (CFWL_Barcode*)m_pNormalWidget;
  if (!pBarCodeWidget || pBarCodeWidget->IsProtectedType()) {
    return FALSE;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open) {
    return FALSE;
  }
  return CXFA_FFTextEdit::OnLButtonDown(dwFlags, fx, fy);
}
FX_BOOL CXFA_FFBarcode::OnRButtonDown(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  CFWL_Barcode* pBarCodeWidget = (CFWL_Barcode*)m_pNormalWidget;
  if (!pBarCodeWidget || pBarCodeWidget->IsProtectedType()) {
    return FALSE;
  }
  return CXFA_FFTextEdit::OnRButtonDown(dwFlags, fx, fy);
}
