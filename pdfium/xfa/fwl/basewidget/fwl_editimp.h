// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_EDITIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_EDITIMP_H_

#include <deque>
#include <memory>
#include <vector>

#include "xfa/fde/ifde_txtedtdorecord.h"
#include "xfa/fde/ifde_txtedtengine.h"
#include "xfa/fwl/basewidget/ifwl_scrollbar.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fxgraphics/cfx_path.h"

class CFWL_EditImp;
class CFWL_EditImpDelegate;
class CFWL_MsgActivate;
class CFWL_MsgDeactivate;
class CFWL_MsgMouse;
class CFWL_WidgetImpDelegate;
class CFWL_WidgetImpProperties;
class IFWL_Caret;

class CFWL_EditImp : public CFWL_WidgetImp {
 public:
  CFWL_EditImp(const CFWL_WidgetImpProperties& properties, IFWL_Widget* pOuter);
  ~CFWL_EditImp() override;

  // CFWL_WidgetImp:
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error SetWidgetRect(const CFX_RectF& rect) override;
  FWL_Error Update() override;
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy) override;
  void SetStates(uint32_t dwStates, FX_BOOL bSet = TRUE) override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;
  FWL_Error SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) override;

  virtual FWL_Error SetText(const CFX_WideString& wsText);
  virtual int32_t GetTextLength() const;
  virtual FWL_Error GetText(CFX_WideString& wsText,
                            int32_t nStart = 0,
                            int32_t nCount = -1) const;
  virtual FWL_Error ClearText();
  virtual int32_t GetCaretPos() const;
  virtual int32_t SetCaretPos(int32_t nIndex, FX_BOOL bBefore = TRUE);
  virtual FWL_Error AddSelRange(int32_t nStart, int32_t nCount = -1);
  virtual int32_t CountSelRanges();
  virtual int32_t GetSelRange(int32_t nIndex, int32_t& nStart);
  virtual FWL_Error ClearSelections();
  virtual int32_t GetLimit();
  virtual FWL_Error SetLimit(int32_t nLimit);
  virtual FWL_Error SetAliasChar(FX_WCHAR wAlias);
  virtual FWL_Error Insert(int32_t nStart,
                           const FX_WCHAR* lpText,
                           int32_t nLen);
  virtual FWL_Error DeleteSelections();
  virtual FWL_Error DeleteRange(int32_t nStart, int32_t nCount = -1);
  virtual FWL_Error Replace(int32_t nStart,
                            int32_t nLen,
                            const CFX_WideStringC& wsReplace);
  virtual FWL_Error DoClipboard(int32_t iCmd);
  virtual FX_BOOL Copy(CFX_WideString& wsCopy);
  virtual FX_BOOL Cut(CFX_WideString& wsCut);
  virtual FX_BOOL Paste(const CFX_WideString& wsPaste);
  virtual FX_BOOL Delete();
  virtual FX_BOOL Redo(const IFDE_TxtEdtDoRecord* pRecord);
  virtual FX_BOOL Undo(const IFDE_TxtEdtDoRecord* pRecord);
  virtual FX_BOOL Undo();
  virtual FX_BOOL Redo();
  virtual FX_BOOL CanUndo();
  virtual FX_BOOL CanRedo();
  virtual FWL_Error SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant);
  virtual FWL_Error SetOuter(IFWL_Widget* pOuter);
  virtual FWL_Error SetNumberRange(int32_t iMin, int32_t iMax);
  virtual FWL_Error SetBackgroundColor(uint32_t color);
  virtual FWL_Error SetFont(const CFX_WideString& wsFont, FX_FLOAT fSize);

  void On_CaretChanged(CFDE_TxtEdtEngine* pEdit,
                       int32_t nPage,
                       FX_BOOL bVisible = true);
  void On_TextChanged(CFDE_TxtEdtEngine* pEdit,
                      FDE_TXTEDT_TEXTCHANGE_INFO& ChangeInfo);
  void On_SelChanged(CFDE_TxtEdtEngine* pEdit);
  FX_BOOL On_PageLoad(CFDE_TxtEdtEngine* pEdit,
                      int32_t nPageIndex,
                      int32_t nPurpose);
  FX_BOOL On_PageUnload(CFDE_TxtEdtEngine* pEdit,
                        int32_t nPageIndex,
                        int32_t nPurpose);
  void On_AddDoRecord(CFDE_TxtEdtEngine* pEdit, IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL On_Validate(CFDE_TxtEdtEngine* pEdit, CFX_WideString& wsText);
  void SetScrollOffset(FX_FLOAT fScrollOffset);
  FX_BOOL GetSuggestWords(CFX_PointF pointf,
                          std::vector<CFX_ByteString>& sSuggest);
  FX_BOOL ReplaceSpellCheckWord(CFX_PointF pointf,
                                const CFX_ByteStringC& bsReplace);

 protected:
  friend class CFWL_TxtEdtEventSink;
  friend class CFWL_EditImpDelegate;

  void DrawTextBk(CFX_Graphics* pGraphics,
                  IFWL_ThemeProvider* pTheme,
                  const CFX_Matrix* pMatrix = nullptr);
  void DrawContent(CFX_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix = nullptr);
  void UpdateEditEngine();
  void UpdateEditParams();
  void UpdateEditLayout();
  FX_BOOL UpdateOffset();
  FX_BOOL UpdateOffset(IFWL_ScrollBar* pScrollBar, FX_FLOAT fPosChanged);
  void UpdateVAlignment();
  void UpdateCaret();
  IFWL_ScrollBar* UpdateScroll();
  void Layout();
  void LayoutScrollBar();
  void DeviceToEngine(CFX_PointF& pt);
  void InitScrollBar(FX_BOOL bVert = TRUE);
  void InitEngine();
  virtual void ShowCaret(FX_BOOL bVisible, CFX_RectF* pRect = nullptr);
  FX_BOOL ValidateNumberChar(FX_WCHAR cNum);
  void InitCaret();
  void ClearRecord();
  FX_BOOL IsShowScrollBar(FX_BOOL bVert);
  FX_BOOL IsContentHeightOverflow();
  int32_t AddDoRecord(IFDE_TxtEdtDoRecord* pRecord);
  void ProcessInsertError(int32_t iError);

  void DrawSpellCheck(CFX_Graphics* pGraphics,
                      const CFX_Matrix* pMatrix = nullptr);
  void AddSpellCheckObj(CFX_Path& PathData,
                        int32_t nStart,
                        int32_t nCount,
                        FX_FLOAT fOffSetX,
                        FX_FLOAT fOffSetY);
  int32_t GetWordAtPoint(CFX_PointF pointf, int32_t& nCount);

  CFX_RectF m_rtClient;
  CFX_RectF m_rtEngine;
  CFX_RectF m_rtStatic;
  FX_FLOAT m_fVAlignOffset;
  FX_FLOAT m_fScrollOffsetX;
  FX_FLOAT m_fScrollOffsetY;
  CFDE_TxtEdtEngine* m_pEdtEngine;
  FX_BOOL m_bLButtonDown;
  int32_t m_nSelStart;
  int32_t m_nLimit;
  FX_FLOAT m_fSpaceAbove;
  FX_FLOAT m_fSpaceBelow;
  FX_FLOAT m_fFontSize;
  FX_ARGB m_argbSel;
  FX_BOOL m_bSetRange;
  int32_t m_iMin;
  int32_t m_iMax;
  std::unique_ptr<IFWL_ScrollBar> m_pVertScrollBar;
  std::unique_ptr<IFWL_ScrollBar> m_pHorzScrollBar;
  std::unique_ptr<IFWL_Caret> m_pCaret;
  CFX_WideString m_wsCache;
  uint32_t m_backColor;
  FX_BOOL m_updateBackColor;
  CFX_WideString m_wsFont;
  std::deque<std::unique_ptr<IFDE_TxtEdtDoRecord>> m_DoRecords;
  int32_t m_iCurRecord;
  int32_t m_iMaxRecord;
};

class CFWL_EditImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_EditImpDelegate(CFWL_EditImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void DoActivate(CFWL_MsgActivate* pMsg);
  void DoDeactivate(CFWL_MsgDeactivate* pMsg);
  void DoButtonDown(CFWL_MsgMouse* pMsg);
  void OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnButtonDblClk(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnKeyDown(CFWL_MsgKey* pMsg);
  void OnChar(CFWL_MsgKey* pMsg);
  FX_BOOL OnScroll(IFWL_ScrollBar* pScrollBar, uint32_t dwCode, FX_FLOAT fPos);
  void DoCursor(CFWL_MsgMouse* pMsg);
  CFWL_EditImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_EDITIMP_H_
