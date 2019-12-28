// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_SCRIPT_LAYOUTPSEUDOMODEL_H_
#define XFA_FXFA_PARSER_XFA_SCRIPT_LAYOUTPSEUDOMODEL_H_

#include "fxjs/include/cfxjse_arguments.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_object.h"

enum XFA_LAYOUTMODEL_HWXY {
  XFA_LAYOUTMODEL_H,
  XFA_LAYOUTMODEL_W,
  XFA_LAYOUTMODEL_X,
  XFA_LAYOUTMODEL_Y
};

class CScript_LayoutPseudoModel : public CXFA_Object {
 public:
  explicit CScript_LayoutPseudoModel(CXFA_Document* pDocument);
  ~CScript_LayoutPseudoModel() override;

  void Ready(CFXJSE_Value* pValue, FX_BOOL bSetting, XFA_ATTRIBUTE eAttribute);

  void HWXY(CFXJSE_Arguments* pArguments, XFA_LAYOUTMODEL_HWXY layoutModel);
  void H(CFXJSE_Arguments* pArguments);
  void W(CFXJSE_Arguments* pArguments);
  void X(CFXJSE_Arguments* pArguments);
  void Y(CFXJSE_Arguments* pArguments);
  void NumberedPageCount(CFXJSE_Arguments* pArguments, FX_BOOL bNumbered);
  void PageCount(CFXJSE_Arguments* pArguments);
  void PageSpan(CFXJSE_Arguments* pArguments);
  void Page(CFXJSE_Arguments* pArguments);
  void PageContent(CFXJSE_Arguments* pArguments);
  void AbsPageCount(CFXJSE_Arguments* pArguments);
  void AbsPageCountInBatch(CFXJSE_Arguments* pArguments);
  void SheetCountInBatch(CFXJSE_Arguments* pArguments);
  void Relayout(CFXJSE_Arguments* pArguments);
  void AbsPageSpan(CFXJSE_Arguments* pArguments);
  void AbsPageInBatch(CFXJSE_Arguments* pArguments);
  void SheetInBatch(CFXJSE_Arguments* pArguments);
  void Sheet(CFXJSE_Arguments* pArguments);
  void RelayoutPageArea(CFXJSE_Arguments* pArguments);
  void SheetCount(CFXJSE_Arguments* pArguments);
  void AbsPage(CFXJSE_Arguments* pArguments);

 protected:
  void GetObjArray(CXFA_LayoutProcessor* pDocLayout,
                   int32_t iPageNo,
                   const CFX_WideString& wsType,
                   FX_BOOL bOnPageArea,
                   CXFA_NodeArray& retArray);
  void PageImp(CFXJSE_Arguments* pArguments, FX_BOOL bAbsPage);
};

#endif  // XFA_FXFA_PARSER_XFA_SCRIPT_LAYOUTPSEUDOMODEL_H_
