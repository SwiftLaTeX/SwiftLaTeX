// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_DATAPROVIDER_H_
#define XFA_FWL_CORE_IFWL_DATAPROVIDER_H_

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fwl/core/fwl_error.h"

class IFWL_Widget;

class IFWL_DataProvider {
 public:
  virtual ~IFWL_DataProvider() {}
  virtual FWL_Error GetCaption(IFWL_Widget* pWidget,
                               CFX_WideString& wsCaption) = 0;
};

#endif  // XFA_FWL_CORE_IFWL_DATAPROVIDER_H_
