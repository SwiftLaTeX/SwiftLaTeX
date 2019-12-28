// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_IFDE_TXTEDTDORECORD_H_
#define XFA_FDE_IFDE_TXTEDTDORECORD_H_

#include "core/fxcrt/include/fx_system.h"

class IFDE_TxtEdtDoRecord {
 public:
  virtual ~IFDE_TxtEdtDoRecord() {}

  virtual FX_BOOL Redo() const = 0;
  virtual FX_BOOL Undo() const = 0;
};

#endif  // XFA_FDE_IFDE_TXTEDTDORECORD_H_
