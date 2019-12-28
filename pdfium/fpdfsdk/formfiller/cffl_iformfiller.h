// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_IFORMFILLER_H_
#define FPDFSDK_FORMFILLER_CFFL_IFORMFILLER_H_

#include <map>

#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/pdfwindow/PWL_Edit.h"

class CFFL_FormFiller;
class CPDFDoc_Environment;
class CPDFSDK_Annot;
class CPDFSDK_PageView;
class CPDFSDK_Widget;

class CFFL_IFormFiller : public IPWL_Filler_Notify {
 public:
  explicit CFFL_IFormFiller(CPDFDoc_Environment* pApp);
  ~CFFL_IFormFiller() override;

  virtual FX_BOOL Annot_HitTest(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                CFX_FloatPoint point);
  virtual FX_RECT GetViewBBox(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot);
  virtual void OnDraw(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      CFX_RenderDevice* pDevice,
                      CFX_Matrix* pUser2Device,
                      uint32_t dwFlags);

  virtual void OnCreate(CPDFSDK_Annot* pAnnot);
  virtual void OnLoad(CPDFSDK_Annot* pAnnot);
  virtual void OnDelete(CPDFSDK_Annot* pAnnot);

  virtual void OnMouseEnter(CPDFSDK_PageView* pPageView,
                            CPDFSDK_Annot* pAnnot,
                            FX_UINT nFlag);
  virtual void OnMouseExit(CPDFSDK_PageView* pPageView,
                           CPDFSDK_Annot* pAnnot,
                           FX_UINT nFlag);

  virtual FX_BOOL OnLButtonDown(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                FX_UINT nFlags,
                                const CFX_FloatPoint& point);
  virtual FX_BOOL OnLButtonUp(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              FX_UINT nFlags,
                              const CFX_FloatPoint& point);
  virtual FX_BOOL OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                  CPDFSDK_Annot* pAnnot,
                                  FX_UINT nFlags,
                                  const CFX_FloatPoint& point);
  virtual FX_BOOL OnMouseMove(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              FX_UINT nFlags,
                              const CFX_FloatPoint& point);
  virtual FX_BOOL OnMouseWheel(CPDFSDK_PageView* pPageView,
                               CPDFSDK_Annot* pAnnot,
                               FX_UINT nFlags,
                               short zDelta,
                               const CFX_FloatPoint& point);
  virtual FX_BOOL OnRButtonDown(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                FX_UINT nFlags,
                                const CFX_FloatPoint& point);
  virtual FX_BOOL OnRButtonUp(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              FX_UINT nFlags,
                              const CFX_FloatPoint& point);

  virtual FX_BOOL OnKeyDown(CPDFSDK_Annot* pAnnot,
                            FX_UINT nKeyCode,
                            FX_UINT nFlags);
  virtual FX_BOOL OnChar(CPDFSDK_Annot* pAnnot, FX_UINT nChar, FX_UINT nFlags);

  virtual FX_BOOL OnSetFocus(CPDFSDK_Annot* pAnnot, FX_UINT nFlag);
  virtual FX_BOOL OnKillFocus(CPDFSDK_Annot* pAnnot, FX_UINT nFlag);

  CFFL_FormFiller* GetFormFiller(CPDFSDK_Annot* pAnnot, FX_BOOL bRegister);
  void RemoveFormFiller(CPDFSDK_Annot* pAnnot);

  static FX_BOOL IsVisible(CPDFSDK_Widget* pWidget);
  static FX_BOOL IsReadOnly(CPDFSDK_Widget* pWidget);
  static FX_BOOL IsFillingAllowed(CPDFSDK_Widget* pWidget);
  static FX_BOOL IsValidAnnot(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot);

  void OnKeyStrokeCommit(CPDFSDK_Widget* pWidget,
                         CPDFSDK_PageView* pPageView,
                         FX_BOOL& bRC,
                         FX_BOOL& bExit,
                         uint32_t nFlag);
  void OnValidate(CPDFSDK_Widget* pWidget,
                  CPDFSDK_PageView* pPageView,
                  FX_BOOL& bRC,
                  FX_BOOL& bExit,
                  uint32_t nFlag);

  void OnCalculate(CPDFSDK_Widget* pWidget,
                   CPDFSDK_PageView* pPageView,
                   FX_BOOL& bExit,
                   uint32_t nFlag);
  void OnFormat(CPDFSDK_Widget* pWidget,
                CPDFSDK_PageView* pPageView,
                FX_BOOL& bExit,
                uint32_t nFlag);
  void OnButtonUp(CPDFSDK_Widget* pWidget,
                  CPDFSDK_PageView* pPageView,
                  FX_BOOL& bReset,
                  FX_BOOL& bExit,
                  FX_UINT nFlag);
#ifdef PDF_ENABLE_XFA
  void OnClick(CPDFSDK_Widget* pWidget,
               CPDFSDK_PageView* pPageView,
               FX_BOOL& bReset,
               FX_BOOL& bExit,
               FX_UINT nFlag);
  void OnFull(CPDFSDK_Widget* pWidget,
              CPDFSDK_PageView* pPageView,
              FX_BOOL& bReset,
              FX_BOOL& bExit,
              FX_UINT nFlag);
  void OnPreOpen(CPDFSDK_Widget* pWidget,
                 CPDFSDK_PageView* pPageView,
                 FX_BOOL& bReset,
                 FX_BOOL& bExit,
                 FX_UINT nFlag);
  void OnPostOpen(CPDFSDK_Widget* pWidget,
                  CPDFSDK_PageView* pPageView,
                  FX_BOOL& bReset,
                  FX_BOOL& bExit,
                  FX_UINT nFlag);
#endif  // PDF_ENABLE_XFA

 private:
  using CFFL_Widget2Filler = std::map<CPDFSDK_Annot*, CFFL_FormFiller*>;

  // IPWL_Filler_Notify:
  void QueryWherePopup(void* pPrivateData,
                       FX_FLOAT fPopupMin,
                       FX_FLOAT fPopupMax,
                       int32_t& nRet,
                       FX_FLOAT& fPopupRet) override;
  void OnBeforeKeyStroke(void* pPrivateData,
                         CFX_WideString& strChange,
                         const CFX_WideString& strChangeEx,
                         int nSelStart,
                         int nSelEnd,
                         FX_BOOL bKeyDown,
                         FX_BOOL& bRC,
                         FX_BOOL& bExit,
                         uint32_t nFlag) override;
#ifdef PDF_ENABLE_XFA
  void OnPopupPreOpen(void* pPrivateData,
                      FX_BOOL& bExit,
                      uint32_t nFlag) override;
  void OnPopupPostOpen(void* pPrivateData,
                       FX_BOOL& bExit,
                       uint32_t nFlag) override;
  void SetFocusAnnotTab(CPDFSDK_Annot* pWidget,
                        FX_BOOL bSameField,
                        FX_BOOL bNext);
#endif  // PDF_ENABLE_XFA
  void UnRegisterFormFiller(CPDFSDK_Annot* pAnnot);

  CPDFDoc_Environment* m_pApp;
  CFFL_Widget2Filler m_Maps;
  FX_BOOL m_bNotifying;
};

class CFFL_PrivateData {
 public:
  CPDFSDK_Widget* pWidget;
  CPDFSDK_PageView* pPageView;
  int nWidgetAge;
  int nValueAge;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_IFORMFILLER_H_
