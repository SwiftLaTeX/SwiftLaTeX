// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_DOC_UTILS_H_
#define CORE_FPDFDOC_DOC_UTILS_H_

#include <vector>

#include "core/fpdfapi/fpdf_parser/include/cpdf_parser.h"

class CPDF_Dictionary;
class CPDF_FormField;

class CPDF_NumberTree {
 public:
  CPDF_NumberTree(CPDF_Dictionary* pRoot) : m_pRoot(pRoot) {}
  CPDF_Object* LookupValue(int num) const;

 protected:
  CPDF_Dictionary* const m_pRoot;
};

CFX_WideString GetFullName(CPDF_Dictionary* pFieldDict);
void InitInterFormDict(CPDF_Dictionary*& pFormDict, CPDF_Document* pDocument);
uint32_t CountInterFormFonts(CPDF_Dictionary* pFormDict);
CPDF_Font* GetInterFormFont(CPDF_Dictionary* pFormDict,
                            CPDF_Document* pDocument,
                            uint32_t index,
                            CFX_ByteString& csNameTag);
CPDF_Font* GetInterFormFont(CPDF_Dictionary* pFormDict,
                            CPDF_Document* pDocument,
                            CFX_ByteString csNameTag);
CPDF_Font* GetInterFormFont(CPDF_Dictionary* pFormDict,
                            CPDF_Document* pDocument,
                            CFX_ByteString csFontName,
                            CFX_ByteString& csNameTag);
CPDF_Font* GetNativeInterFormFont(CPDF_Dictionary* pFormDict,
                                  CPDF_Document* pDocument,
                                  uint8_t charSet,
                                  CFX_ByteString& csNameTag);
CPDF_Font* GetNativeInterFormFont(CPDF_Dictionary* pFormDict,
                                  CPDF_Document* pDocument,
                                  CFX_ByteString& csNameTag);
FX_BOOL FindInterFormFont(CPDF_Dictionary* pFormDict,
                          const CPDF_Font* pFont,
                          CFX_ByteString& csNameTag);
FX_BOOL FindInterFormFont(CPDF_Dictionary* pFormDict,
                          CPDF_Document* pDocument,
                          CFX_ByteString csFontName,
                          CPDF_Font*& pFont,
                          CFX_ByteString& csNameTag);
void AddInterFormFont(CPDF_Dictionary*& pFormDict,
                      CPDF_Document* pDocument,
                      const CPDF_Font* pFont,
                      CFX_ByteString& csNameTag);
CPDF_Font* AddNativeInterFormFont(CPDF_Dictionary*& pFormDict,
                                  CPDF_Document* pDocument,
                                  uint8_t charSet,
                                  CFX_ByteString& csNameTag);
CPDF_Font* AddNativeInterFormFont(CPDF_Dictionary*& pFormDict,
                                  CPDF_Document* pDocument,
                                  CFX_ByteString& csNameTag);
void RemoveInterFormFont(CPDF_Dictionary* pFormDict, const CPDF_Font* pFont);
void RemoveInterFormFont(CPDF_Dictionary* pFormDict, CFX_ByteString csNameTag);
CPDF_Font* GetDefaultInterFormFont(CPDF_Dictionary* pFormDict,
                                   CPDF_Document* pDocument);
void SetDefaultInterFormFont(CPDF_Dictionary*& pFormDict,
                             CPDF_Document* pDocument,
                             const CPDF_Font* pFont);
std::vector<bool> SaveCheckedFieldStatus(CPDF_FormField* pField);
FX_BOOL NeedPDFEncodeForFieldFullName(const CFX_WideString& csFieldName);
FX_BOOL NeedPDFEncodeForFieldTree(CPDF_Dictionary* pFieldDict, int nLevel = 0);
void EncodeFieldName(const CFX_WideString& csName, CFX_ByteString& csT);
void UpdateEncodeFieldName(CPDF_Dictionary* pFieldDict, int nLevel = 0);

#endif  // CORE_FPDFDOC_DOC_UTILS_H_
