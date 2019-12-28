// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_BARCODEIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_BARCODEIMP_H_

#include <memory>

#include "xfa/fwl/basewidget/fwl_editimp.h"
#include "xfa/fwl/basewidget/ifwl_barcode.h"
#include "xfa/fwl/basewidget/ifwl_scrollbar.h"

class CFWL_WidgetImpProperties;
class CFWL_BarcodeImpDelegate;
class CFX_Barcode;
class IFWL_Widget;

#define XFA_BCS_NeedUpdate 0x0001
#define XFA_BCS_EncodeSuccess 0x0002

class CFWL_BarcodeImp : public CFWL_EditImp {
 public:
  CFWL_BarcodeImp(const CFWL_WidgetImpProperties& properties,
                  IFWL_Widget* pOuter);
  ~CFWL_BarcodeImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error Update() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;
  FWL_Error SetText(const CFX_WideString& wsText) override;

  void SetType(BC_TYPE type);
  FX_BOOL IsProtectedType();

 protected:
  friend class CFWL_BarcodeImpDelegate;

  void GenerateBarcodeImageCache();
  void CreateBarcodeEngine();

  std::unique_ptr<CFX_Barcode> m_pBarcodeEngine;
  uint32_t m_dwStatus;
  BC_TYPE m_type;
};

class CFWL_BarcodeImpDelegate : public CFWL_EditImpDelegate {
 public:
  explicit CFWL_BarcodeImpDelegate(CFWL_BarcodeImp* pOwner);
  void OnProcessEvent(CFWL_Event* pEvent) override;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_BARCODEIMP_H_
