// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_FORM_H_
#define XFA_FWL_CORE_IFWL_FORM_H_

#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

#define FWL_CLASS_Form L"FWL_FORM"
#define FWL_CLASS_FormProxy L"FWL_FORMPROXY"
#define FWL_STYLEEXT_FRM_Resize (1L << 0)
#define FWL_STYLEEXT_FRM_NativeBorder (1L << 1)
#define FWL_STYLEEXT_FRM_RoundCorner (2L << 1)
#define FWL_STYLEEXT_FRM_RoundCorner4 (3L << 1)
#define FWL_STYLEEXT_FRM_NoDrawClient (1L << 3)
#define FWL_STYLEEXT_FRM_BorderCornerMask (3L << 1)
#define FWL_STYLEEXT_FRM_Max (3)

#if (_FX_OS_ == _FX_MACOSX_)
#define FWL_UseMacSystemBorder
#endif

enum FWL_FORMSIZE {
  FWL_FORMSIZE_Manual = 0,
  FWL_FORMSIZE_Width,
  FWL_FORMSIZE_Height,
  FWL_FORMSIZE_All,
};

class CFX_DIBitmap;
class CFX_WideString;
class CFX_Path;
class IFWL_Widget;
class IFWL_Form;

class IFWL_FormDP : public IFWL_DataProvider {
 public:
  virtual CFX_DIBitmap* GetIcon(IFWL_Widget* pWidget, FX_BOOL bBig) = 0;
};

class IFWL_Form : public IFWL_Widget {
 public:
  static IFWL_Form* CreateFormProxy(CFWL_WidgetImpProperties& properties,
                                    CFX_WideString* classname,
                                    IFWL_Widget* pOuter);

  FWL_FORMSIZE GetFormSize();
  FWL_Error SetFormSize(FWL_FORMSIZE eFormSize);
  IFWL_Widget* DoModal();
  IFWL_Widget* DoModal(uint32_t& dwCommandID);
  FWL_Error EndDoModal();
  FWL_Error SetBorderRegion(CFX_Path* pPath);

 protected:
  IFWL_Form();
};

#endif  // XFA_FWL_CORE_IFWL_FORM_H_
