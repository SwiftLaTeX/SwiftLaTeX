// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_IJS_RUNTIME_H_
#define FPDFSDK_JAVASCRIPT_IJS_RUNTIME_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

#ifdef PDF_ENABLE_XFA
#include "fxjs/include/fxjse.h"
#endif  // PDF_ENABLE_XFA

class CPDFDoc_Environment;
class CPDFSDK_Document;
class IJS_Context;

// Owns the FJXS objects needed to actually execute JS.
class IJS_Runtime {
 public:
  static void Initialize(unsigned int slot, void* isolate);
  static void Destroy();
  static IJS_Runtime* Create(CPDFDoc_Environment* pEnv);
  virtual ~IJS_Runtime() {}

  virtual IJS_Context* NewContext() = 0;
  virtual void ReleaseContext(IJS_Context* pContext) = 0;
  virtual IJS_Context* GetCurrentContext() = 0;
  virtual void SetReaderDocument(CPDFSDK_Document* pReaderDoc) = 0;
  virtual CPDFSDK_Document* GetReaderDocument() = 0;
  virtual int Execute(const CFX_WideString& script, CFX_WideString* info) = 0;

#ifdef PDF_ENABLE_XFA
  virtual FX_BOOL GetValueByName(const CFX_ByteStringC& utf8Name,
                                 CFXJSE_Value* pValue) = 0;
  virtual FX_BOOL SetValueByName(const CFX_ByteStringC& utf8Name,
                                 CFXJSE_Value* pValue) = 0;
#endif  // PDF_ENABLE_XFA

 protected:
  IJS_Runtime() {}
};

#endif  // FPDFSDK_JAVASCRIPT_IJS_RUNTIME_H_
