// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARKITEM_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARKITEM_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Dictionary;

class CPDF_ContentMarkItem {
 public:
  enum ParamType { None, PropertiesDict, DirectDict };

  CPDF_ContentMarkItem();
  CPDF_ContentMarkItem(const CPDF_ContentMarkItem& src);
  ~CPDF_ContentMarkItem();

  const CFX_ByteString& GetName() const { return m_MarkName; }
  ParamType GetParamType() const { return m_ParamType; }
  CPDF_Dictionary* GetParam() const { return m_pParam; }
  FX_BOOL HasMCID() const;
  void SetName(const CFX_ByteString& name) { m_MarkName = name; }
  void SetParam(ParamType type, CPDF_Dictionary* param) {
    m_ParamType = type;
    m_pParam = param;
  }

 private:
  CFX_ByteString m_MarkName;
  ParamType m_ParamType;
  CPDF_Dictionary* m_pParam;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARKITEM_H_
