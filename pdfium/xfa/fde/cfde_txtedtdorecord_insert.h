// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TXTEDTDORECORD_INSERT_H_
#define XFA_FDE_CFDE_TXTEDTDORECORD_INSERT_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fde/ifde_txtedtdorecord.h"

class CFDE_TxtEdtEngine;

class CFDE_TxtEdtDoRecord_Insert : public IFDE_TxtEdtDoRecord {
 public:
  CFDE_TxtEdtDoRecord_Insert(CFDE_TxtEdtEngine* pEngine,
                             int32_t nCaret,
                             const FX_WCHAR* lpText,
                             int32_t nLength);
  ~CFDE_TxtEdtDoRecord_Insert() override;

  FX_BOOL Undo() const override;
  FX_BOOL Redo() const override;

 private:
  CFDE_TxtEdtEngine* m_pEngine;
  int32_t m_nCaret;
  CFX_WideString m_wsInsert;
};

#endif  // XFA_FDE_CFDE_TXTEDTDORECORD_INSERT_H_
