// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CSS_FDE_CSSSTYLESHEET_H_
#define XFA_FDE_CSS_FDE_CSSSTYLESHEET_H_

#include <unordered_map>

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fde/css/fde_cssdeclaration.h"

class CFDE_CSSSyntaxParser;

class CFDE_CSSSelector : public CFX_Target {
 public:
  CFDE_CSSSelector(FDE_CSSSELECTORTYPE eType,
                   const FX_WCHAR* psz,
                   int32_t iLen,
                   bool bIgnoreCase);

  virtual FDE_CSSSELECTORTYPE GetType() const;
  virtual uint32_t GetNameHash() const;
  virtual CFDE_CSSSelector* GetNextSelector() const;

  static CFDE_CSSSelector* FromString(IFX_MemoryAllocator* pStaticStore,
                                      const FX_WCHAR* psz,
                                      int32_t iLen);

  void SetNext(CFDE_CSSSelector* pNext) { m_pNext = pNext; }

 protected:
  void SetType(FDE_CSSSELECTORTYPE eType) { m_eType = eType; }

  FDE_CSSSELECTORTYPE m_eType;
  uint32_t m_dwHash;
  CFDE_CSSSelector* m_pNext;
};

typedef CFX_ArrayTemplate<CFDE_CSSSelector*> CFDE_CSSSelectorArray;

class CFDE_CSSStyleRule : public IFDE_CSSStyleRule, public CFX_Target {
 public:
  CFDE_CSSStyleRule();

  // IFDE_CSSStyleRule
  int32_t CountSelectorLists() const override;
  CFDE_CSSSelector* GetSelectorList(int32_t index) const override;
  CFDE_CSSDeclaration* GetDeclaration() override;

  CFDE_CSSDeclaration& GetDeclImp() { return m_Declaration; }
  void SetSelector(IFX_MemoryAllocator* pStaticStore,
                   const CFDE_CSSSelectorArray& list);

 protected:
  CFDE_CSSDeclaration m_Declaration;
  CFDE_CSSSelector** m_ppSelector;
  int32_t m_iSelectors;
};

class CFDE_CSSMediaRule : public IFDE_CSSMediaRule, public CFX_Target {
 public:
  explicit CFDE_CSSMediaRule(uint32_t dwMediaList);
  ~CFDE_CSSMediaRule() override;

  // IFDE_CSSMediaRule
  uint32_t GetMediaList() const override;
  int32_t CountRules() const override;
  IFDE_CSSRule* GetRule(int32_t index) override;

  CFDE_CSSRuleArray& GetArray() { return m_RuleArray; }

 protected:
  uint32_t m_dwMediaList;
  CFDE_CSSRuleArray m_RuleArray;
};

class CFDE_CSSFontFaceRule : public IFDE_CSSFontFaceRule, public CFX_Target {
 public:
  // IFDE_CSSFontFaceRule
  CFDE_CSSDeclaration* GetDeclaration() override;

  CFDE_CSSDeclaration& GetDeclImp() { return m_Declaration; }

 protected:
  CFDE_CSSDeclaration m_Declaration;
};

#define FDE_CSSSWITCHDEFAULTS()     \
  case FDE_CSSSYNTAXSTATUS_EOS:     \
    return FDE_CSSSYNTAXSTATUS_EOS; \
  case FDE_CSSSYNTAXSTATUS_Error:   \
  default:                          \
    return FDE_CSSSYNTAXSTATUS_Error;

class CFDE_CSSStyleSheet : public IFDE_CSSStyleSheet, public CFX_Target {
 public:
  CFDE_CSSStyleSheet(uint32_t dwMediaList);
  ~CFDE_CSSStyleSheet() override;

  // IFX_Retainable:
  uint32_t Retain() override;
  uint32_t Release() override;

  // IFDE_CSSStyleSheet:
  FX_BOOL GetUrl(CFX_WideString& szUrl) override;
  uint32_t GetMediaList() const override;
  uint16_t GetCodePage() const override;
  int32_t CountRules() const override;
  IFDE_CSSRule* GetRule(int32_t index) override;

  FX_BOOL LoadFromStream(const CFX_WideString& szUrl,
                         IFX_Stream* pStream,
                         uint16_t wCodePage);
  FX_BOOL LoadFromBuffer(const CFX_WideString& szUrl,
                         const FX_WCHAR* pBuffer,
                         int32_t iBufSize,
                         uint16_t wCodePage);

 protected:
  void Reset();
  FX_BOOL LoadFromSyntax(CFDE_CSSSyntaxParser* pSyntax);
  FDE_CSSSYNTAXSTATUS LoadStyleRule(CFDE_CSSSyntaxParser* pSyntax,
                                    CFDE_CSSRuleArray& ruleArray);
  FDE_CSSSYNTAXSTATUS LoadImportRule(CFDE_CSSSyntaxParser* pSyntax);
  FDE_CSSSYNTAXSTATUS LoadPageRule(CFDE_CSSSyntaxParser* pSyntax);
  FDE_CSSSYNTAXSTATUS LoadMediaRule(CFDE_CSSSyntaxParser* pSyntax);
  FDE_CSSSYNTAXSTATUS LoadFontFaceRule(CFDE_CSSSyntaxParser* pSyntax,
                                       CFDE_CSSRuleArray& ruleArray);
  FDE_CSSSYNTAXSTATUS SkipRuleSet(CFDE_CSSSyntaxParser* pSyntax);
  uint16_t m_wCodePage;
  uint16_t m_wRefCount;
  uint32_t m_dwMediaList;
  IFX_MemoryAllocator* m_pAllocator;
  CFDE_CSSRuleArray m_RuleArray;
  CFX_WideString m_szUrl;
  CFDE_CSSSelectorArray m_Selectors;
  std::unordered_map<uint32_t, FX_WCHAR*> m_StringCache;
};

#endif  // XFA_FDE_CSS_FDE_CSSSTYLESHEET_H_
