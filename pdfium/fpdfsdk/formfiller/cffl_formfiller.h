// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_FORMFILLER_H_
#define FPDFSDK_FORMFILLER_CFFL_FORMFILLER_H_

#include <map>

#include "fpdfsdk/formfiller/cba_fontmap.h"
#include "fpdfsdk/formfiller/cffl_iformfiller.h"
#include "fpdfsdk/include/fsdk_baseform.h"

class CPDFDoc_Environment;
class CPDFSDK_Annot;
class CPDFSDK_PageView;
class CPDFSDK_Widget;

class CFFL_FormFiller : public IPWL_Provider, public CPWL_TimerHandler {
 public:
  CFFL_FormFiller(CPDFDoc_Environment* pApp, CPDFSDK_Annot* pAnnot);
  ~CFFL_FormFiller() override;

  virtual FX_RECT GetViewBBox(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot);
  virtual void OnDraw(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      CFX_RenderDevice* pDevice,
                      CFX_Matrix* pUser2Device,
                      uint32_t dwFlags);
  virtual void OnDrawDeactive(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              CFX_RenderDevice* pDevice,
                              CFX_Matrix* pUser2Device,
                              uint32_t dwFlags);

  virtual void OnCreate(CPDFSDK_Annot* pAnnot);
  virtual void OnLoad(CPDFSDK_Annot* pAnnot);
  virtual void OnDelete(CPDFSDK_Annot* pAnnot);

  virtual void OnMouseEnter(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot);
  virtual void OnMouseExit(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot);

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

  void SetFocusForAnnot(CPDFSDK_Annot* pAnnot, FX_UINT nFlag);
  void KillFocusForAnnot(CPDFSDK_Annot* pAnnot, FX_UINT nFlag);

  // CPWL_TimerHandler
  void TimerProc() override;
  CFX_SystemHandler* GetSystemHandler() const override;

  // IPWL_Provider
  CFX_Matrix GetWindowMatrix(void* pAttachedData) override;
  CFX_WideString LoadPopupMenuString(int nIndex) override;

  virtual void GetActionData(CPDFSDK_PageView* pPageView,
                             CPDF_AAction::AActionType type,
                             PDFSDK_FieldAction& fa);
  virtual void SetActionData(CPDFSDK_PageView* pPageView,
                             CPDF_AAction::AActionType type,
                             const PDFSDK_FieldAction& fa);
  virtual FX_BOOL IsActionDataChanged(CPDF_AAction::AActionType type,
                                      const PDFSDK_FieldAction& faOld,
                                      const PDFSDK_FieldAction& faNew);

  virtual void SaveState(CPDFSDK_PageView* pPageView);
  virtual void RestoreState(CPDFSDK_PageView* pPageView);

  virtual CPWL_Wnd* ResetPDFWindow(CPDFSDK_PageView* pPageView,
                                   FX_BOOL bRestoreValue);

  CFX_Matrix GetCurMatrix();

  CFX_FloatRect FFLtoPWL(const CFX_FloatRect& rect);
  CFX_FloatRect PWLtoFFL(const CFX_FloatRect& rect);
  CFX_FloatPoint FFLtoPWL(const CFX_FloatPoint& point);
  CFX_FloatPoint PWLtoFFL(const CFX_FloatPoint& point);

  CFX_FloatPoint WndtoPWL(CPDFSDK_PageView* pPageView,
                          const CFX_FloatPoint& pt);
  CFX_FloatRect FFLtoWnd(CPDFSDK_PageView* pPageView,
                         const CFX_FloatRect& rect);

  void SetWindowRect(CPDFSDK_PageView* pPageView,
                     const CFX_FloatRect& rcWindow);
  CFX_FloatRect GetWindowRect(CPDFSDK_PageView* pPageView);

  FX_BOOL CommitData(CPDFSDK_PageView* pPageView, FX_UINT nFlag);
  virtual FX_BOOL IsDataChanged(CPDFSDK_PageView* pPageView);
  virtual void SaveData(CPDFSDK_PageView* pPageView);

#ifdef PDF_ENABLE_XFA
  virtual FX_BOOL IsFieldFull(CPDFSDK_PageView* pPageView);
#endif  // PDF_ENABLE_XFA

  CPWL_Wnd* GetPDFWindow(CPDFSDK_PageView* pPageView, FX_BOOL bNew);
  void DestroyPDFWindow(CPDFSDK_PageView* pPageView);
  void EscapeFiller(CPDFSDK_PageView* pPageView, FX_BOOL bDestroyPDFWindow);

  virtual PWL_CREATEPARAM GetCreateParam();
  virtual CPWL_Wnd* NewPDFWindow(const PWL_CREATEPARAM& cp,
                                 CPDFSDK_PageView* pPageView) = 0;
  virtual CFX_FloatRect GetFocusBox(CPDFSDK_PageView* pPageView);

  FX_BOOL IsValid() const;
  CFX_FloatRect GetPDFWindowRect() const;

  CPDFSDK_PageView* GetCurPageView();
  void SetChangeMark();

  virtual void InvalidateRect(double left,
                              double top,
                              double right,
                              double bottom);
  CPDFDoc_Environment* GetApp() { return m_pApp; }
  CPDFSDK_Annot* GetSDKAnnot() { return m_pAnnot; }

 protected:
  using CFFL_PageView2PDFWindow = std::map<CPDFSDK_PageView*, CPWL_Wnd*>;

  CPDFDoc_Environment* m_pApp;
  CPDFSDK_Widget* m_pWidget;
  CPDFSDK_Annot* m_pAnnot;

  FX_BOOL m_bValid;
  CFFL_PageView2PDFWindow m_Maps;
  CFX_FloatPoint m_ptOldPos;
};

class CFFL_Button : public CFFL_FormFiller {
 public:
  CFFL_Button(CPDFDoc_Environment* pApp, CPDFSDK_Annot* pWidget);
  ~CFFL_Button() override;

  // CFFL_FormFiller
  void OnMouseEnter(CPDFSDK_PageView* pPageView,
                    CPDFSDK_Annot* pAnnot) override;
  void OnMouseExit(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot) override;
  FX_BOOL OnLButtonDown(CPDFSDK_PageView* pPageView,
                        CPDFSDK_Annot* pAnnot,
                        FX_UINT nFlags,
                        const CFX_FloatPoint& point) override;
  FX_BOOL OnLButtonUp(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      FX_UINT nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnMouseMove(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      FX_UINT nFlags,
                      const CFX_FloatPoint& point) override;
  void OnDraw(CPDFSDK_PageView* pPageView,
              CPDFSDK_Annot* pAnnot,
              CFX_RenderDevice* pDevice,
              CFX_Matrix* pUser2Device,
              uint32_t dwFlags) override;
  void OnDrawDeactive(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      CFX_RenderDevice* pDevice,
                      CFX_Matrix* pUser2Device,
                      uint32_t dwFlags) override;

 protected:
  FX_BOOL m_bMouseIn;
  FX_BOOL m_bMouseDown;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_FORMFILLER_H_
