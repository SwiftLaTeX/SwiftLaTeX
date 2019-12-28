// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_PICTUREBOX_H_
#define XFA_FWL_BASEWIDGET_IFWL_PICTUREBOX_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"
#include "xfa/fwl/core/fwl_error.h"

#define FWL_CLASS_PictureBox L"FWL_PICTUREBOX"
#define FWL_STYLEEXT_PTB_Left 0L << 0
#define FWL_STYLEEXT_PTB_Center 1L << 0
#define FWL_STYLEEXT_PTB_Right 2L << 0
#define FWL_STYLEEXT_PTB_Top 0L << 2
#define FWL_STYLEEXT_PTB_Vcenter 1L << 2
#define FWL_STYLEEXT_PTB_Bottom 2L << 2
#define FWL_STYLEEXT_PTB_Normal 0L << 4
#define FWL_STYLEEXT_PTB_AutoSize 1L << 4
#define FWL_STYLEEXT_PTB_StretchImage 2L << 4
#define FWL_STYLEEXT_PTB_StretchHImage 3L << 4
#define FWL_STYLEEXT_PTB_StretchVImage 4L << 4
#define FWL_STYLEEXT_PTB_HAlignMask 3L << 0
#define FWL_STYLEEXT_PTB_VAlignMask 3L << 2
#define FWL_STYLEEXT_PTB_StretchAlignMask 7L << 4

class CFX_DIBitmap;

class IFWL_PictureBoxDP : public IFWL_DataProvider {
 public:
  virtual CFX_DIBitmap* GetPicture(IFWL_Widget* pWidget) = 0;
  virtual CFX_DIBitmap* GetErrorPicture(IFWL_Widget* pWidget) = 0;
  virtual CFX_DIBitmap* GetInitialPicture(IFWL_Widget* pWidget) = 0;
  virtual int32_t GetOpacity(IFWL_Widget* pWidget) = 0;
  virtual int32_t GetFlipMode(IFWL_Widget* pWidget) = 0;
  virtual FWL_Error GetMatrix(IFWL_Widget* pWidget, CFX_Matrix& matrix) = 0;
};

class IFWL_PictureBox : public IFWL_Widget {
 public:
  static IFWL_PictureBox* Create(const CFWL_WidgetImpProperties& properties,
                                 IFWL_Widget* pOuter);

 protected:
  IFWL_PictureBox();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_PICTUREBOX_H_
