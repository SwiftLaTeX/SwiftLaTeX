// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_TAGGED_INT_H_
#define CORE_FPDFDOC_TAGGED_INT_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fpdfdoc/include/fpdf_tagged.h"
#include "core/fxcrt/include/cfx_retain_ptr.h"
#include "third_party/base/stl_util.h"

class CPDF_StructElementImpl;

class CPDF_StructTreeImpl final : public IPDF_StructTree {
 public:
  explicit CPDF_StructTreeImpl(const CPDF_Document* pDoc);
  ~CPDF_StructTreeImpl() override;

  // IPDF_StructTree:
  int CountTopElements() const override;
  IPDF_StructElement* GetTopElement(int i) const override;

  void LoadDocTree();
  void LoadPageTree(const CPDF_Dictionary* pPageDict);
  CPDF_StructElementImpl* AddPageNode(
      CPDF_Dictionary* pElement,
      std::map<CPDF_Dictionary*, CPDF_StructElementImpl*>& map,
      int nLevel = 0);
  FX_BOOL AddTopLevelNode(CPDF_Dictionary* pDict,
                          CPDF_StructElementImpl* pElement);

 protected:
  const CPDF_Dictionary* const m_pTreeRoot;
  const CPDF_Dictionary* const m_pRoleMap;
  const CPDF_Dictionary* m_pPage;
  std::vector<CFX_RetainPtr<CPDF_StructElementImpl>> m_Kids;

  friend class CPDF_StructElementImpl;
};

class CPDF_StructElementImpl final : public IPDF_StructElement {
 public:
  CPDF_StructElementImpl(CPDF_StructTreeImpl* pTree,
                         CPDF_StructElementImpl* pParent,
                         CPDF_Dictionary* pDict);

  // IPDF_StructElement
  IPDF_StructTree* GetTree() const override;
  const CFX_ByteString& GetType() const override;
  IPDF_StructElement* GetParent() const override;
  CPDF_Dictionary* GetDict() const override;
  int CountKids() const override;
  const CPDF_StructKid& GetKid(int index) const override;
  CPDF_Object* GetAttr(const CFX_ByteStringC& owner,
                       const CFX_ByteStringC& name,
                       FX_BOOL bInheritable = FALSE,
                       FX_FLOAT fLevel = 0.0F) override;
  CFX_ByteString GetName(const CFX_ByteStringC& owner,
                         const CFX_ByteStringC& name,
                         const CFX_ByteStringC& default_value,
                         FX_BOOL bInheritable = FALSE,
                         int subindex = -1) override;
  FX_ARGB GetColor(const CFX_ByteStringC& owner,
                   const CFX_ByteStringC& name,
                   FX_ARGB default_value,
                   FX_BOOL bInheritable = FALSE,
                   int subindex = -1) override;
  FX_FLOAT GetNumber(const CFX_ByteStringC& owner,
                     const CFX_ByteStringC& name,
                     FX_FLOAT default_value,
                     FX_BOOL bInheritable = FALSE,
                     int subindex = -1) override;
  int GetInteger(const CFX_ByteStringC& owner,
                 const CFX_ByteStringC& name,
                 int default_value,
                 FX_BOOL bInheritable = FALSE,
                 int subindex = -1) override;

  void LoadKids(CPDF_Dictionary* pDict);
  void LoadKid(uint32_t PageObjNum, CPDF_Object* pObj, CPDF_StructKid* pKid);
  CPDF_Object* GetAttr(const CFX_ByteStringC& owner,
                       const CFX_ByteStringC& name,
                       FX_BOOL bInheritable,
                       int subindex);
  CPDF_StructElementImpl* Retain();
  void Release();

 protected:
  ~CPDF_StructElementImpl() override;

  int m_RefCount;
  CPDF_StructTreeImpl* const m_pTree;
  CPDF_StructElementImpl* const m_pParent;
  CPDF_Dictionary* const m_pDict;
  CFX_ByteString m_Type;
  std::vector<CPDF_StructKid> m_Kids;

  friend class CPDF_StructTreeImpl;
};

#endif  // CORE_FPDFDOC_TAGGED_INT_H_
