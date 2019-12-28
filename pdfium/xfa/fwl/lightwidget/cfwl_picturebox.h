// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_PICTUREBOX_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_PICTUREBOX_H_

#include "xfa/fwl/basewidget/ifwl_picturebox.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/lightwidget/cfwl_widget.h"

class CFWL_PictureBox : public CFWL_Widget {
 public:
  CFWL_PictureBox();
  ~CFWL_PictureBox() override;

  static CFWL_PictureBox* Create();

  FWL_Error Initialize(const CFWL_WidgetProperties* pProperties = nullptr);
  CFX_DIBitmap* GetPicture();
  FWL_Error SetPicture(CFX_DIBitmap* pBitmap);
  FX_FLOAT GetRotation();
  FWL_Error SetRotation(FX_FLOAT fRotation);
  int32_t GetFlipMode();
  FWL_Error SetFlipMode(int32_t iFlipMode);
  int32_t GetOpacity();
  FWL_Error SetOpacity(int32_t iOpacity);
  FWL_Error GetScale(FX_FLOAT& fScaleX, FX_FLOAT& fScaleY);
  FWL_Error SetScale(FX_FLOAT fScaleX, FX_FLOAT fScaleY);
  FWL_Error GetOffset(FX_FLOAT& fx, FX_FLOAT& fy);
  FWL_Error SetOffset(FX_FLOAT fx, FX_FLOAT fy);

 protected:
  class CFWL_PictureBoxDP : public IFWL_PictureBoxDP {
   public:
    CFWL_PictureBoxDP();

    // IFWL_DataProvider
    FWL_Error GetCaption(IFWL_Widget* pWidget,
                         CFX_WideString& wsCaption) override;

    // IFWL_PictureBoxDP
    CFX_DIBitmap* GetPicture(IFWL_Widget* pWidget) override;
    CFX_DIBitmap* GetErrorPicture(IFWL_Widget* pWidget) override;
    CFX_DIBitmap* GetInitialPicture(IFWL_Widget* pWidget) override;
    int32_t GetOpacity(IFWL_Widget* pWidget) override;
    int32_t GetFlipMode(IFWL_Widget* pWidget) override;
    FWL_Error GetMatrix(IFWL_Widget* pWidget, CFX_Matrix& matrix) override;

    CFX_DIBitmap* m_pBitmap;
    int32_t m_iOpacity;
    int32_t m_iFlipMode;
    FX_FLOAT m_fRotation;
    FX_FLOAT m_fScaleX;
    FX_FLOAT m_fScaleY;
    FX_FLOAT m_fOffSetX;
    FX_FLOAT m_fOffSetY;
    CFX_WideString m_wsData;
  };

  CFWL_PictureBoxDP m_PictureBoxDP;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_PICTUREBOX_H_
