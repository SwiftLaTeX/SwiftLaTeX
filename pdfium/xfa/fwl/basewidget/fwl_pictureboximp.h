// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_PICTUREBOXIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_PICTUREBOXIMP_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_widget.h"

class CFWL_WidgetImpProperties;
class IFWL_Widget;
class CFWL_PictureBoxImpDelegate;

class CFWL_PictureBoxImp : public CFWL_WidgetImp {
 public:
  CFWL_PictureBoxImp(const CFWL_WidgetImpProperties& properties,
                     IFWL_Widget* pOuter);
  ~CFWL_PictureBoxImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error Update() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  friend class CFWL_PictureBoxImpDelegate;

  void DrawBkground(CFX_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    const CFX_Matrix* pMatrix = nullptr);

  FX_BOOL VStyle(FX_BOOL dwStyle);
  CFX_RectF m_rtClient;
  CFX_RectF m_rtImage;
  CFX_Matrix m_matrix;
  FX_BOOL m_bTop;
  FX_BOOL m_bVCenter;
  FX_BOOL m_bButton;
};

class CFWL_PictureBoxImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_PictureBoxImpDelegate(CFWL_PictureBoxImp* pOwner);
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  CFWL_PictureBoxImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_PICTUREBOXIMP_H_
