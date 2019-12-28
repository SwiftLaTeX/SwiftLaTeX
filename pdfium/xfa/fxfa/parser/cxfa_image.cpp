// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_image.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Image::CXFA_Image(CXFA_Node* pNode, FX_BOOL bDefValue)
    : CXFA_Data(pNode), m_bDefValue(bDefValue) {}

int32_t CXFA_Image::GetAspect() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Aspect);
}

FX_BOOL CXFA_Image::GetContentType(CFX_WideString& wsContentType) {
  return m_pNode->TryCData(XFA_ATTRIBUTE_ContentType, wsContentType);
}

FX_BOOL CXFA_Image::GetHref(CFX_WideString& wsHref) {
  if (m_bDefValue)
    return m_pNode->TryCData(XFA_ATTRIBUTE_Href, wsHref);
  return m_pNode->GetAttribute(FX_WSTRC(L"href"), wsHref);
}

int32_t CXFA_Image::GetTransferEncoding() {
  if (m_bDefValue)
    return m_pNode->GetEnum(XFA_ATTRIBUTE_TransferEncoding);
  return XFA_ATTRIBUTEENUM_Base64;
}

FX_BOOL CXFA_Image::GetContent(CFX_WideString& wsText) {
  return m_pNode->TryContent(wsText);
}

FX_BOOL CXFA_Image::SetContentType(const CFX_WideString& wsContentType) {
  return m_pNode->SetCData(XFA_ATTRIBUTE_ContentType, wsContentType);
}

FX_BOOL CXFA_Image::SetHref(const CFX_WideString& wsHref) {
  if (m_bDefValue)
    return m_pNode->SetCData(XFA_ATTRIBUTE_Href, wsHref);
  return m_pNode->SetAttribute(XFA_ATTRIBUTE_Href, wsHref.AsStringC());
}

FX_BOOL CXFA_Image::SetTransferEncoding(int32_t iTransferEncoding) {
  if (m_bDefValue) {
    return m_pNode->SetEnum(XFA_ATTRIBUTE_TransferEncoding,
                            (XFA_ATTRIBUTEENUM)iTransferEncoding);
  }
  return TRUE;
}
