// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_XFA_FFDOCHANDLER_H_
#define XFA_FXFA_INCLUDE_XFA_FFDOCHANDLER_H_

#include "xfa/fxfa/include/fxfa.h"

class CXFA_ChecksumContext;

class CXFA_FFDocHandler {
 public:
  CXFA_FFDocHandler();
  ~CXFA_FFDocHandler();

  CFXJSE_Value* GetXFAScriptObject(CXFA_FFDoc* hDoc);
  XFA_ATTRIBUTEENUM GetRestoreState(CXFA_FFDoc* hDoc);

  FX_BOOL RunDocScript(CXFA_FFDoc* hDoc,
                       XFA_SCRIPTTYPE eScriptType,
                       const CFX_WideStringC& wsScript,
                       CFXJSE_Value* pRetValue,
                       CFXJSE_Value* pThisObject);

 protected:
};

#endif  // XFA_FXFA_INCLUDE_XFA_FFDOCHANDLER_H_
