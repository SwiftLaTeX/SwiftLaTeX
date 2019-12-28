// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_INCLUDE_FSDK_ANNOTHANDLER_H_
#define FPDFSDK_INCLUDE_FSDK_ANNOTHANDLER_H_

#include <map>
#include <vector>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_coordinates.h"

#ifdef PDF_ENABLE_XFA
#define FSDK_XFAWIDGET_TYPENAME "XFAWidget"
#endif  // PDF_ENABLE_XFA

class CFFL_IFormFiller;
class CFX_RenderDevice;
class CPDFDoc_Environment;
class CPDFSDK_Annot;
class CPDFSDK_PageView;
class CPDF_Annot;
class CFX_Matrix;
class CFX_FloatPoint;
#ifdef PDF_ENABLE_XFA
class CXFA_FFWidget;
class CXFA_FFWidgetHandler;
#endif  // PDF_ENABLE_XFA

class IPDFSDK_AnnotHandler {
 public:
  virtual ~IPDFSDK_AnnotHandler() {}

  virtual CFX_ByteString GetType() = 0;
  virtual CFX_ByteString GetName() = 0;
  virtual FX_BOOL CanAnswer(CPDFSDK_Annot* pAnnot) = 0;
  virtual CPDFSDK_Annot* NewAnnot(CPDF_Annot* pAnnot,
                                  CPDFSDK_PageView* pPage) = 0;

#ifdef PDF_ENABLE_XFA
  virtual CPDFSDK_Annot* NewAnnot(CXFA_FFWidget* hWidget,
                                  CPDFSDK_PageView* pPage) = 0;
#endif  // PDF_ENABLE_XFA

  virtual void ReleaseAnnot(CPDFSDK_Annot* pAnnot) = 0;
  virtual void DeleteAnnot(CPDFSDK_Annot* pAnnot) = 0;
  virtual CFX_FloatRect GetViewBBox(CPDFSDK_PageView* pPageView,
                                    CPDFSDK_Annot* pAnnot) = 0;
  virtual FX_BOOL HitTest(CPDFSDK_PageView* pPageView,
                          CPDFSDK_Annot* pAnnot,
                          const CFX_FloatPoint& point) = 0;
  virtual void OnDraw(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      CFX_RenderDevice* pDevice,
                      CFX_Matrix* pUser2Device,
                      uint32_t dwFlags) = 0;
  virtual void OnDrawSleep(CPDFSDK_PageView* pPageView,
                           CPDFSDK_Annot* pAnnot,
                           CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_FloatRect& rcWindow,
                           uint32_t dwFlags) = 0;

  virtual void OnCreate(CPDFSDK_Annot* pAnnot) = 0;
  virtual void OnLoad(CPDFSDK_Annot* pAnnot) = 0;
  virtual void OnDelete(CPDFSDK_Annot* pAnnot) = 0;
  virtual void OnRelease(CPDFSDK_Annot* pAnnot) = 0;
  virtual void OnMouseEnter(CPDFSDK_PageView* pPageView,
                            CPDFSDK_Annot* pAnnot,
                            uint32_t nFlag) = 0;
  virtual void OnMouseExit(CPDFSDK_PageView* pPageView,
                           CPDFSDK_Annot* pAnnot,
                           uint32_t nFlag) = 0;
  virtual FX_BOOL OnLButtonDown(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                uint32_t nFlags,
                                const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnLButtonUp(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              uint32_t nFlags,
                              const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                  CPDFSDK_Annot* pAnnot,
                                  uint32_t nFlags,
                                  const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnMouseMove(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              uint32_t nFlags,
                              const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnMouseWheel(CPDFSDK_PageView* pPageView,
                               CPDFSDK_Annot* pAnnot,
                               uint32_t nFlags,
                               short zDelta,
                               const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnRButtonDown(CPDFSDK_PageView* pPageView,
                                CPDFSDK_Annot* pAnnot,
                                uint32_t nFlags,
                                const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnRButtonUp(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              uint32_t nFlags,
                              const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnRButtonDblClk(CPDFSDK_PageView* pPageView,
                                  CPDFSDK_Annot* pAnnot,
                                  uint32_t nFlags,
                                  const CFX_FloatPoint& point) = 0;
  virtual FX_BOOL OnChar(CPDFSDK_Annot* pAnnot,
                         uint32_t nChar,
                         uint32_t nFlags) = 0;
  virtual FX_BOOL OnKeyDown(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag) = 0;
  virtual FX_BOOL OnKeyUp(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag) = 0;
  virtual void OnDeSelected(CPDFSDK_Annot* pAnnot) = 0;
  virtual void OnSelected(CPDFSDK_Annot* pAnnot) = 0;
  virtual FX_BOOL OnSetFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag) = 0;
  virtual FX_BOOL OnKillFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag) = 0;
#ifdef PDF_ENABLE_XFA
  virtual FX_BOOL OnXFAChangedFocus(CPDFSDK_Annot* pOldAnnot,
                                    CPDFSDK_Annot* pNewAnnot) = 0;
#endif  // PDF_ENABLE_XFA
};

class CPDFSDK_BFAnnotHandler : public IPDFSDK_AnnotHandler {
 public:
  explicit CPDFSDK_BFAnnotHandler(CPDFDoc_Environment* pApp);
  ~CPDFSDK_BFAnnotHandler() override;

  // IPDFSDK_AnnotHandler
  CFX_ByteString GetType() override;
  CFX_ByteString GetName() override;
  FX_BOOL CanAnswer(CPDFSDK_Annot* pAnnot) override;
  CPDFSDK_Annot* NewAnnot(CPDF_Annot* pAnnot, CPDFSDK_PageView* pPage) override;
#ifdef PDF_ENABLE_XFA
  CPDFSDK_Annot* NewAnnot(CXFA_FFWidget* hWidget,
                          CPDFSDK_PageView* pPage) override;
#endif  // PDF_ENABLE_XFA
  void ReleaseAnnot(CPDFSDK_Annot* pAnnot) override;
  void DeleteAnnot(CPDFSDK_Annot* pAnnot) override;
  CFX_FloatRect GetViewBBox(CPDFSDK_PageView* pPageView,
                            CPDFSDK_Annot* pAnnot) override;
  FX_BOOL HitTest(CPDFSDK_PageView* pPageView,
                  CPDFSDK_Annot* pAnnot,
                  const CFX_FloatPoint& point) override;
  void OnDraw(CPDFSDK_PageView* pPageView,
              CPDFSDK_Annot* pAnnot,
              CFX_RenderDevice* pDevice,
              CFX_Matrix* pUser2Device,
              uint32_t dwFlags) override;
  void OnDrawSleep(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot* pAnnot,
                   CFX_RenderDevice* pDevice,
                   CFX_Matrix* pUser2Device,
                   const CFX_FloatRect& rcWindow,
                   uint32_t dwFlags) override;
  void OnCreate(CPDFSDK_Annot* pAnnot) override;
  void OnLoad(CPDFSDK_Annot* pAnnot) override;
  void OnDelete(CPDFSDK_Annot* pAnnot) override;
  void OnRelease(CPDFSDK_Annot* pAnnot) override;
  void OnMouseEnter(CPDFSDK_PageView* pPageView,
                    CPDFSDK_Annot* pAnnot,
                    uint32_t nFlag) override;
  void OnMouseExit(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot* pAnnot,
                   uint32_t nFlag) override;
  FX_BOOL OnLButtonDown(CPDFSDK_PageView* pPageView,
                        CPDFSDK_Annot* pAnnot,
                        uint32_t nFlags,
                        const CFX_FloatPoint& point) override;
  FX_BOOL OnLButtonUp(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      uint32_t nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                          CPDFSDK_Annot* pAnnot,
                          uint32_t nFlags,
                          const CFX_FloatPoint& point) override;
  FX_BOOL OnMouseMove(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      uint32_t nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnMouseWheel(CPDFSDK_PageView* pPageView,
                       CPDFSDK_Annot* pAnnot,
                       uint32_t nFlags,
                       short zDelta,
                       const CFX_FloatPoint& point) override;
  FX_BOOL OnRButtonDown(CPDFSDK_PageView* pPageView,
                        CPDFSDK_Annot* pAnnot,
                        uint32_t nFlags,
                        const CFX_FloatPoint& point) override;
  FX_BOOL OnRButtonUp(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      uint32_t nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnRButtonDblClk(CPDFSDK_PageView* pPageView,
                          CPDFSDK_Annot* pAnnot,
                          uint32_t nFlags,
                          const CFX_FloatPoint& point) override;
  FX_BOOL OnChar(CPDFSDK_Annot* pAnnot,
                 uint32_t nChar,
                 uint32_t nFlags) override;
  FX_BOOL OnKeyDown(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag) override;
  FX_BOOL OnKeyUp(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag) override;
  void OnDeSelected(CPDFSDK_Annot* pAnnot) override {}
  void OnSelected(CPDFSDK_Annot* pAnnot) override {}
  FX_BOOL OnSetFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag) override;
  FX_BOOL OnKillFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag) override;
#ifdef PDF_ENABLE_XFA
  FX_BOOL OnXFAChangedFocus(CPDFSDK_Annot* pOldAnnot,
                            CPDFSDK_Annot* pNewAnnot) override;
#endif  // PDF_ENABLE_XFA

  void SetFormFiller(CFFL_IFormFiller* pFiller) { m_pFormFiller = pFiller; }
  CFFL_IFormFiller* GetFormFiller() { return m_pFormFiller; }

 private:
  CPDFDoc_Environment* m_pApp;
  CFFL_IFormFiller* m_pFormFiller;
};

#ifdef PDF_ENABLE_XFA
class CPDFSDK_XFAAnnotHandler : public IPDFSDK_AnnotHandler {
 public:
  explicit CPDFSDK_XFAAnnotHandler(CPDFDoc_Environment* pApp);
  ~CPDFSDK_XFAAnnotHandler() override;

  // IPDFSDK_AnnotHandler
  CFX_ByteString GetType() override;
  CFX_ByteString GetName() override;
  FX_BOOL CanAnswer(CPDFSDK_Annot* pAnnot) override;
  CPDFSDK_Annot* NewAnnot(CPDF_Annot* pAnnot, CPDFSDK_PageView* pPage) override;
  CPDFSDK_Annot* NewAnnot(CXFA_FFWidget* pAnnot,
                          CPDFSDK_PageView* pPage) override;
  void ReleaseAnnot(CPDFSDK_Annot* pAnnot) override;
  void DeleteAnnot(CPDFSDK_Annot* pAnnot) override;
  CFX_FloatRect GetViewBBox(CPDFSDK_PageView* pPageView,
                            CPDFSDK_Annot* pAnnot) override;
  FX_BOOL HitTest(CPDFSDK_PageView* pPageView,
                  CPDFSDK_Annot* pAnnot,
                  const CFX_FloatPoint& point) override;
  void OnDraw(CPDFSDK_PageView* pPageView,
              CPDFSDK_Annot* pAnnot,
              CFX_RenderDevice* pDevice,
              CFX_Matrix* pUser2Device,
              uint32_t dwFlags) override;
  void OnDrawSleep(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot* pAnnot,
                   CFX_RenderDevice* pDevice,
                   CFX_Matrix* pUser2Device,
                   const CFX_FloatRect& rcWindow,
                   uint32_t dwFlags) override;
  void OnCreate(CPDFSDK_Annot* pAnnot) override;
  void OnLoad(CPDFSDK_Annot* pAnnot) override;
  void OnDelete(CPDFSDK_Annot* pAnnot) override;
  void OnRelease(CPDFSDK_Annot* pAnnot) override;
  void OnMouseEnter(CPDFSDK_PageView* pPageView,
                    CPDFSDK_Annot* pAnnot,
                    uint32_t nFlag) override;
  void OnMouseExit(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot* pAnnot,
                   uint32_t nFlag) override;
  FX_BOOL OnLButtonDown(CPDFSDK_PageView* pPageView,
                        CPDFSDK_Annot* pAnnot,
                        uint32_t nFlags,
                        const CFX_FloatPoint& point) override;
  FX_BOOL OnLButtonUp(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      uint32_t nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                          CPDFSDK_Annot* pAnnot,
                          uint32_t nFlags,
                          const CFX_FloatPoint& point) override;
  FX_BOOL OnMouseMove(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      uint32_t nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnMouseWheel(CPDFSDK_PageView* pPageView,
                       CPDFSDK_Annot* pAnnot,
                       uint32_t nFlags,
                       short zDelta,
                       const CFX_FloatPoint& point) override;
  FX_BOOL OnRButtonDown(CPDFSDK_PageView* pPageView,
                        CPDFSDK_Annot* pAnnot,
                        uint32_t nFlags,
                        const CFX_FloatPoint& point) override;
  FX_BOOL OnRButtonUp(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      uint32_t nFlags,
                      const CFX_FloatPoint& point) override;
  FX_BOOL OnRButtonDblClk(CPDFSDK_PageView* pPageView,
                          CPDFSDK_Annot* pAnnot,
                          uint32_t nFlags,
                          const CFX_FloatPoint& point) override;
  FX_BOOL OnChar(CPDFSDK_Annot* pAnnot,
                 uint32_t nChar,
                 uint32_t nFlags) override;
  FX_BOOL OnKeyDown(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag) override;
  FX_BOOL OnKeyUp(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag) override;
  void OnDeSelected(CPDFSDK_Annot* pAnnot) override;
  void OnSelected(CPDFSDK_Annot* pAnnot) override;
  FX_BOOL OnSetFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag) override;
  FX_BOOL OnKillFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag) override;
  FX_BOOL OnXFAChangedFocus(CPDFSDK_Annot* pOldAnnot,
                            CPDFSDK_Annot* pNewAnnot) override;

 private:
  CXFA_FFWidgetHandler* GetXFAWidgetHandler(CPDFSDK_Annot* pAnnot);
  uint32_t GetFWLFlags(uint32_t dwFlag);

 private:
  CPDFDoc_Environment* m_pApp;
};
#endif  // PDF_ENABLE_XFA

#define CBA_AnnotHandlerArray CFX_ArrayTemplate<IPDFSDK_AnnotHandler*>
class CPDFSDK_AnnotHandlerMgr {
 public:
  // Destroy the handler
  explicit CPDFSDK_AnnotHandlerMgr(CPDFDoc_Environment* pApp);
  virtual ~CPDFSDK_AnnotHandlerMgr();

 public:
  void RegisterAnnotHandler(IPDFSDK_AnnotHandler* pAnnotHandler);
  void UnRegisterAnnotHandler(IPDFSDK_AnnotHandler* pAnnotHandler);

  virtual CPDFSDK_Annot* NewAnnot(CPDF_Annot* pAnnot,
                                  CPDFSDK_PageView* pPageView);
#ifdef PDF_ENABLE_XFA
  virtual CPDFSDK_Annot* NewAnnot(CXFA_FFWidget* pAnnot,
                                  CPDFSDK_PageView* pPageView);
#endif  // PDF_ENABLE_XFA
  virtual void ReleaseAnnot(CPDFSDK_Annot* pAnnot);

  virtual void Annot_OnCreate(CPDFSDK_Annot* pAnnot);
  virtual void Annot_OnLoad(CPDFSDK_Annot* pAnnot);

 public:
  IPDFSDK_AnnotHandler* GetAnnotHandler(CPDFSDK_Annot* pAnnot) const;
  virtual void Annot_OnDraw(CPDFSDK_PageView* pPageView,
                            CPDFSDK_Annot* pAnnot,
                            CFX_RenderDevice* pDevice,
                            CFX_Matrix* pUser2Device,
                            uint32_t dwFlags);

  virtual void Annot_OnMouseEnter(CPDFSDK_PageView* pPageView,
                                  CPDFSDK_Annot* pAnnot,
                                  uint32_t nFlags);
  virtual void Annot_OnMouseExit(CPDFSDK_PageView* pPageView,
                                 CPDFSDK_Annot* pAnnot,
                                 uint32_t nFlags);

  virtual FX_BOOL Annot_OnLButtonDown(CPDFSDK_PageView* pPageView,
                                      CPDFSDK_Annot* pAnnot,
                                      uint32_t nFlags,
                                      const CFX_FloatPoint& point);
  virtual FX_BOOL Annot_OnLButtonUp(CPDFSDK_PageView* pPageView,
                                    CPDFSDK_Annot* pAnnot,
                                    uint32_t nFlags,
                                    const CFX_FloatPoint& point);
  virtual FX_BOOL Annot_OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                        CPDFSDK_Annot* pAnnot,
                                        uint32_t nFlags,
                                        const CFX_FloatPoint& point);

  virtual FX_BOOL Annot_OnMouseMove(CPDFSDK_PageView* pPageView,
                                    CPDFSDK_Annot* pAnnot,
                                    uint32_t nFlags,
                                    const CFX_FloatPoint& point);
  virtual FX_BOOL Annot_OnMouseWheel(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot,
                                     uint32_t nFlags,
                                     short zDelta,
                                     const CFX_FloatPoint& point);
  virtual FX_BOOL Annot_OnRButtonDown(CPDFSDK_PageView* pPageView,
                                      CPDFSDK_Annot* pAnnot,
                                      uint32_t nFlags,
                                      const CFX_FloatPoint& point);
  virtual FX_BOOL Annot_OnRButtonUp(CPDFSDK_PageView* pPageView,
                                    CPDFSDK_Annot* pAnnot,
                                    uint32_t nFlags,
                                    const CFX_FloatPoint& point);

  virtual FX_BOOL Annot_OnChar(CPDFSDK_Annot* pAnnot,
                               uint32_t nChar,
                               uint32_t nFlags);
  virtual FX_BOOL Annot_OnKeyDown(CPDFSDK_Annot* pAnnot,
                                  int nKeyCode,
                                  int nFlag);
  virtual FX_BOOL Annot_OnKeyUp(CPDFSDK_Annot* pAnnot, int nKeyCode, int nFlag);

  virtual FX_BOOL Annot_OnSetFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag);
  virtual FX_BOOL Annot_OnKillFocus(CPDFSDK_Annot* pAnnot, uint32_t nFlag);

#ifdef PDF_ENABLE_XFA
  virtual FX_BOOL Annot_OnChangeFocus(CPDFSDK_Annot* pSetAnnot,
                                      CPDFSDK_Annot* pKillAnnot);
#endif  // PDF_ENABLE_XFA

  virtual CFX_FloatRect Annot_OnGetViewBBox(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot* pAnnot);
  virtual FX_BOOL Annot_OnHitTest(CPDFSDK_PageView* pPageView,
                                  CPDFSDK_Annot* pAnnot,
                                  const CFX_FloatPoint& point);

 private:
  IPDFSDK_AnnotHandler* GetAnnotHandler(const CFX_ByteString& sType) const;
  CPDFSDK_Annot* GetNextAnnot(CPDFSDK_Annot* pSDKAnnot, FX_BOOL bNext);

 private:
  CBA_AnnotHandlerArray m_Handlers;
  std::map<CFX_ByteString, IPDFSDK_AnnotHandler*> m_mapType2Handler;
  CPDFDoc_Environment* m_pApp;
};

class CPDFSDK_AnnotIterator {
 public:
  CPDFSDK_AnnotIterator(CPDFSDK_PageView* pPageView, bool bReverse);
  ~CPDFSDK_AnnotIterator();

  CPDFSDK_Annot* Next();

 private:
  CPDFSDK_Annot* NextAnnot();
  CPDFSDK_Annot* PrevAnnot();

  std::vector<CPDFSDK_Annot*> m_iteratorAnnotList;
  const bool m_bReverse;
  size_t m_pos;
};

#endif  // FPDFSDK_INCLUDE_FSDK_ANNOTHANDLER_H_
