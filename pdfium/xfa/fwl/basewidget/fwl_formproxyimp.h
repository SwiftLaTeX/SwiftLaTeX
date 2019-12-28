// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_FORMPROXYIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_FORMPROXYIMP_H_

#include "xfa/fwl/core/fwl_formimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"

class CFWL_WidgetImpProperties;
class CFWL_FormProxyImpDelegate;

class CFWL_FormProxyImp : public CFWL_FormImp {
 public:
  CFWL_FormProxyImp(const CFWL_WidgetImpProperties& properties,
                    IFWL_Widget* pOuter);
  ~CFWL_FormProxyImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FX_BOOL IsInstance(const CFX_WideStringC& wsClass) const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error Update() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  friend class CFWL_FormProxyImpDelegate;
};

class CFWL_FormProxyImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_FormProxyImpDelegate(CFWL_FormProxyImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;

 protected:
  CFWL_FormProxyImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_FORMPROXYIMP_H_
