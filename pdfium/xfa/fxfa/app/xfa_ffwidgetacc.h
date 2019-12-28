// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFWIDGETACC_H_
#define XFA_FXFA_APP_XFA_FFWIDGETACC_H_

#include "xfa/fxfa/app/xfa_textlayout.h"

enum XFA_TEXTPROVIDERTYPE {
  XFA_TEXTPROVIDERTYPE_Text,
  XFA_TEXTPROVIDERTYPE_Datasets,
  XFA_TEXTPROVIDERTYPE_Caption,
  XFA_TEXTPROVIDERTYPE_Rollover,
  XFA_TEXTPROVIDERTYPE_Down,
};

class CXFA_TextProvider {
 public:
  CXFA_TextProvider(CXFA_WidgetAcc* pWidgetAcc,
                    XFA_TEXTPROVIDERTYPE eType,
                    CXFA_Node* pTextNode = nullptr)
      : m_pWidgetAcc(pWidgetAcc), m_eType(eType), m_pTextNode(pTextNode) {
    ASSERT(m_pWidgetAcc);
  }
  ~CXFA_TextProvider() {}

  CXFA_Node* GetTextNode(FX_BOOL& bRichText);
  CXFA_Para GetParaNode();
  CXFA_Font GetFontNode();
  FX_BOOL IsCheckButtonAndAutoWidth();
  CXFA_FFDoc* GetDocNode() { return m_pWidgetAcc->GetDoc(); }
  FX_BOOL GetEmbbedObj(FX_BOOL bURI,
                       FX_BOOL bRaw,
                       const CFX_WideString& wsAttr,
                       CFX_WideString& wsValue);

 protected:
  CXFA_WidgetAcc* m_pWidgetAcc;
  XFA_TEXTPROVIDERTYPE m_eType;
  CXFA_Node* m_pTextNode;
};

#endif  // XFA_FXFA_APP_XFA_FFWIDGETACC_H_
