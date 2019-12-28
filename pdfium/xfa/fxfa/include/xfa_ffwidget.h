// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_XFA_FFWIDGET_H_
#define XFA_FXFA_INCLUDE_XFA_FFWIDGET_H_

#include <vector>

#include "core/fxcodec/include/fx_codec_def.h"
#include "core/fxge/include/fx_ge.h"
#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"

class CXFA_FFPageView;
class CXFA_FFDocView;
class CXFA_FFDoc;
class CXFA_FFApp;
enum class FWL_WidgetHit;

inline FX_FLOAT XFA_UnitPx2Pt(FX_FLOAT fPx, FX_FLOAT fDpi) {
  return fPx * 72.0f / fDpi;
}
#define XFA_FLOAT_PERCISION 0.001f
enum XFA_WIDGETITEM {
  XFA_WIDGETITEM_Parent,
  XFA_WIDGETITEM_FirstChild,
  XFA_WIDGETITEM_NextSibling,
  XFA_WIDGETITEM_PrevSibling,
};

class CXFA_CalcData {
 public:
  CXFA_CalcData();
  ~CXFA_CalcData();

  CFX_ArrayTemplate<CXFA_WidgetAcc*> m_Globals;
  int32_t m_iRefCount;
};

class CXFA_FFWidget : public CXFA_ContentLayoutItem {
 public:
  CXFA_FFWidget(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFWidget() override;

  virtual FX_BOOL GetBBox(CFX_RectF& rtBox,
                          uint32_t dwStatus,
                          FX_BOOL bDrawFocus = FALSE);
  virtual void RenderWidget(CFX_Graphics* pGS,
                            CFX_Matrix* pMatrix,
                            uint32_t dwStatus);
  virtual FX_BOOL IsLoaded();
  virtual FX_BOOL LoadWidget();
  virtual void UnloadWidget();
  virtual FX_BOOL PerformLayout();
  virtual FX_BOOL UpdateFWLData();
  virtual void UpdateWidgetProperty();
  virtual FX_BOOL OnMouseEnter();
  virtual FX_BOOL OnMouseExit();
  virtual FX_BOOL OnLButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnLButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnLButtonDblClk(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnMouseMove(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnMouseWheel(uint32_t dwFlags,
                               int16_t zDelta,
                               FX_FLOAT fx,
                               FX_FLOAT fy);
  virtual FX_BOOL OnRButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnRButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnRButtonDblClk(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);

  virtual FX_BOOL OnSetFocus(CXFA_FFWidget* pOldWidget);
  virtual FX_BOOL OnKillFocus(CXFA_FFWidget* pNewWidget);
  virtual FX_BOOL OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags);
  virtual FX_BOOL OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags);
  virtual FX_BOOL OnChar(uint32_t dwChar, uint32_t dwFlags);
  virtual FWL_WidgetHit OnHitTest(FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL OnSetCursor(FX_FLOAT fx, FX_FLOAT fy);
  virtual FX_BOOL CanUndo();
  virtual FX_BOOL CanRedo();
  virtual FX_BOOL Undo();
  virtual FX_BOOL Redo();
  virtual FX_BOOL CanCopy();
  virtual FX_BOOL CanCut();
  virtual FX_BOOL CanPaste();
  virtual FX_BOOL CanSelectAll();
  virtual FX_BOOL CanDelete();
  virtual FX_BOOL CanDeSelect();
  virtual FX_BOOL Copy(CFX_WideString& wsCopy);
  virtual FX_BOOL Cut(CFX_WideString& wsCut);
  virtual FX_BOOL Paste(const CFX_WideString& wsPaste);
  virtual FX_BOOL SelectAll();
  virtual FX_BOOL Delete();
  virtual FX_BOOL DeSelect();
  virtual FX_BOOL GetSuggestWords(CFX_PointF pointf,
                                  std::vector<CFX_ByteString>& sSuggest);
  virtual FX_BOOL ReplaceSpellCheckWord(CFX_PointF pointf,
                                        const CFX_ByteStringC& bsReplace);

  CXFA_FFPageView* GetPageView();
  void SetPageView(CXFA_FFPageView* pPageView);
  void GetWidgetRect(CFX_RectF& rtWidget);
  CFX_RectF ReCacheWidgetRect();
  uint32_t GetStatus();
  void ModifyStatus(uint32_t dwAdded, uint32_t dwRemoved);

  CXFA_WidgetAcc* GetDataAcc();
  FX_BOOL GetToolTip(CFX_WideString& wsToolTip);

  CXFA_FFDocView* GetDocView();
  void SetDocView(CXFA_FFDocView* pDocView);
  CXFA_FFDoc* GetDoc();
  CXFA_FFApp* GetApp();
  IXFA_AppProvider* GetAppProvider();
  void InvalidateWidget(const CFX_RectF* pRect = nullptr);
  void AddInvalidateRect(const CFX_RectF* pRect = nullptr);
  FX_BOOL GetCaptionText(CFX_WideString& wsCap);
  bool IsFocused();
  void Rotate2Normal(FX_FLOAT& fx, FX_FLOAT& fy);
  void GetRotateMatrix(CFX_Matrix& mt);
  FX_BOOL IsLayoutRectEmpty();
  CXFA_FFWidget* GetParent();
  FX_BOOL IsAncestorOf(CXFA_FFWidget* pWidget);

 protected:
  virtual FX_BOOL PtInActiveRect(FX_FLOAT fx, FX_FLOAT fy);

  void DrawBorder(CFX_Graphics* pGS,
                  CXFA_Box box,
                  const CFX_RectF& rtBorder,
                  CFX_Matrix* pMatrix,
                  uint32_t dwFlags = 0);
  void GetMinMaxWidth(FX_FLOAT fMinWidth, FX_FLOAT fMaxWidth);
  void GetMinMaxHeight(FX_FLOAT fMinHeight, FX_FLOAT fMaxHeight);
  void GetRectWithoutRotate(CFX_RectF& rtWidget);
  bool IsMatchVisibleStatus(uint32_t dwStatus);
  void EventKillFocus();
  FX_BOOL IsButtonDown();
  void SetButtonDown(FX_BOOL bSet);

  CXFA_FFDocView* m_pDocView;
  CXFA_FFPageView* m_pPageView;
  CXFA_WidgetAcc* m_pDataAcc;
  CFX_RectF m_rtWidget;
};

int32_t XFA_StrokeTypeSetLineDash(CFX_Graphics* pGraphics,
                                  int32_t iStrokeType,
                                  int32_t iCapType);
CFX_GraphStateData::LineCap XFA_LineCapToFXGE(int32_t iLineCap);
void XFA_DrawImage(CFX_Graphics* pGS,
                   const CFX_RectF& rtImage,
                   CFX_Matrix* pMatrix,
                   CFX_DIBitmap* pDIBitmap,
                   int32_t iAspect,
                   int32_t iImageXDpi,
                   int32_t iImageYDpi,
                   int32_t iHorzAlign = XFA_ATTRIBUTEENUM_Left,
                   int32_t iVertAlign = XFA_ATTRIBUTEENUM_Top);
CFX_DIBitmap* XFA_LoadImageData(CXFA_FFDoc* pDoc,
                                CXFA_Image* pImage,
                                FX_BOOL& bNameImage,
                                int32_t& iImageXDpi,
                                int32_t& iImageYDpi);
CFX_DIBitmap* XFA_LoadImageFromBuffer(IFX_FileRead* pImageFileRead,
                                      FXCODEC_IMAGE_TYPE type,
                                      int32_t& iImageXDpi,
                                      int32_t& iImageYDpi);
FXCODEC_IMAGE_TYPE XFA_GetImageType(const CFX_WideString& wsType);
FX_CHAR* XFA_Base64Encode(const uint8_t* buf, int32_t buf_len);
void XFA_RectWidthoutMargin(CFX_RectF& rt,
                            const CXFA_Margin& mg,
                            FX_BOOL bUI = FALSE);
CXFA_FFWidget* XFA_GetWidgetFromLayoutItem(CXFA_LayoutItem* pLayoutItem);
FX_BOOL XFA_IsCreateWidget(XFA_Element iType);
#define XFA_DRAWBOX_ForceRound 1
#define XFA_DRAWBOX_Lowered3D 2
void XFA_DrawBox(CXFA_Box box,
                 CFX_Graphics* pGS,
                 const CFX_RectF& rtWidget,
                 CFX_Matrix* pMatrix,
                 uint32_t dwFlags = 0);

#endif  // XFA_FXFA_INCLUDE_XFA_FFWIDGET_H_
