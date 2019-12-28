// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_CFWL_WIDGETMGR_H_
#define XFA_FWL_CORE_CFWL_WIDGETMGR_H_

#include <map>
#include <memory>

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

#define FWL_WGTMGR_DisableThread 0x00000001
#define FWL_WGTMGR_DisableForm 0x00000002

class CFWL_Message;
class CFWL_WidgetMgrDelegate;
class CXFA_FFApp;
class CXFA_FWLAdapterWidgetMgr;
class CFX_Graphics;
class CFX_Matrix;
class IFWL_Widget;

class CFWL_WidgetMgrItem {
 public:
  CFWL_WidgetMgrItem();
  explicit CFWL_WidgetMgrItem(IFWL_Widget* widget);
  ~CFWL_WidgetMgrItem();

  CFWL_WidgetMgrItem* pParent;
  CFWL_WidgetMgrItem* pOwner;
  CFWL_WidgetMgrItem* pChild;
  CFWL_WidgetMgrItem* pPrevious;
  CFWL_WidgetMgrItem* pNext;
  IFWL_Widget* const pWidget;
  std::unique_ptr<CFX_Graphics> pOffscreen;
  int32_t iRedrawCounter;
#if (_FX_OS_ == _FX_WIN32_DESKTOP_) || (_FX_OS_ == _FX_WIN64_)
  FX_BOOL bOutsideChanged;
#endif
};

class CFWL_WidgetMgr {
 public:
  static CFWL_WidgetMgr* GetInstance();

  explicit CFWL_WidgetMgr(CXFA_FFApp* pAdapterNative);
  ~CFWL_WidgetMgr();

  IFWL_Widget* GetParentWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetOwnerWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetFirstSiblingWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetPriorSiblingWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetNextSiblingWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetLastSiblingWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetFirstChildWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetLastChildWidget(IFWL_Widget* pWidget) const;
  IFWL_Widget* GetSystemFormWidget(IFWL_Widget* pWidget) const;

  FX_BOOL SetWidgetIndex(IFWL_Widget* pWidget, int32_t nIndex);
  FWL_Error RepaintWidget(IFWL_Widget* pWidget,
                          const CFX_RectF* pRect = nullptr);

  void AddWidget(IFWL_Widget* pWidget);
  void InsertWidget(IFWL_Widget* pParent,
                    IFWL_Widget* pChild,
                    int32_t nIndex = -1);
  void RemoveWidget(IFWL_Widget* pWidget);
  void SetOwner(IFWL_Widget* pOwner, IFWL_Widget* pOwned);
  void SetParent(IFWL_Widget* pParent, IFWL_Widget* pChild);
  FX_BOOL IsChild(IFWL_Widget* pChild, IFWL_Widget* pParent);
  FWL_Error SetWidgetRect_Native(IFWL_Widget* pWidget, const CFX_RectF& rect);
  IFWL_Widget* GetWidgetAtPoint(IFWL_Widget* pParent, FX_FLOAT fx, FX_FLOAT fy);
  void NotifySizeChanged(IFWL_Widget* pForm, FX_FLOAT fx, FX_FLOAT fy);
  IFWL_Widget* nextTab(IFWL_Widget* parent, IFWL_Widget* focus, FX_BOOL& bFind);
  int32_t CountRadioButtonGroup(IFWL_Widget* pFirst);
  IFWL_Widget* GetSiblingRadioButton(IFWL_Widget* pWidget, FX_BOOL bNext);
  IFWL_Widget* GetRadioButtonGroupHeader(IFWL_Widget* pRadioButton);
  void GetSameGroupRadioButton(IFWL_Widget* pRadioButton,
                               CFX_ArrayTemplate<IFWL_Widget*>& group);
  IFWL_Widget* GetDefaultButton(IFWL_Widget* pParent);
  void AddRedrawCounts(IFWL_Widget* pWidget);
  void ResetRedrawCounts(IFWL_Widget* pWidget);
  CXFA_FWLAdapterWidgetMgr* GetAdapterWidgetMgr() const { return m_pAdapter; }
  CFWL_WidgetMgrDelegate* GetDelegate() const { return m_pDelegate.get(); }
  CFWL_WidgetMgrItem* GetWidgetMgrItem(IFWL_Widget* pWidget) const;
  bool IsThreadEnabled();
  bool IsFormDisabled();
  FX_BOOL GetAdapterPopupPos(IFWL_Widget* pWidget,
                             FX_FLOAT fMinHeight,
                             FX_FLOAT fMaxHeight,
                             const CFX_RectF& rtAnchor,
                             CFX_RectF& rtPopup);

 protected:
  int32_t TravelWidgetMgr(CFWL_WidgetMgrItem* pParent,
                          int32_t* pIndex,
                          CFWL_WidgetMgrItem* pItem,
                          IFWL_Widget** pWidget = nullptr);
  FX_BOOL IsAbleNative(IFWL_Widget* pWidget) const;

  uint32_t m_dwCapability;
  std::unique_ptr<CFWL_WidgetMgrDelegate> m_pDelegate;
  std::map<IFWL_Widget*, std::unique_ptr<CFWL_WidgetMgrItem>> m_mapWidgetItem;
  CXFA_FWLAdapterWidgetMgr* const m_pAdapter;
#if (_FX_OS_ == _FX_WIN32_DESKTOP_) || (_FX_OS_ == _FX_WIN64_)
  CFX_RectF m_rtScreen;
#endif
  friend class CFWL_WidgetMgrDelegate;
};

class CFWL_WidgetMgrDelegate {
 public:
  explicit CFWL_WidgetMgrDelegate(CFWL_WidgetMgr* pWidgetMgr);
  ~CFWL_WidgetMgrDelegate() {}

  FWL_Error OnSetCapability(uint32_t dwCapability = FWL_WGTMGR_DisableThread);
  void OnProcessMessageToForm(CFWL_Message* pMessage);
  void OnDrawWidget(IFWL_Widget* pWidget,
                    CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix);

 protected:
  void DrawChild(IFWL_Widget* pParent,
                 const CFX_RectF& rtClip,
                 CFX_Graphics* pGraphics,
                 const CFX_Matrix* pMatrix);
  CFX_Graphics* DrawWidgetBefore(IFWL_Widget* pWidget,
                                 CFX_Graphics* pGraphics,
                                 const CFX_Matrix* pMatrix);
  void DrawWidgetAfter(IFWL_Widget* pWidget,
                       CFX_Graphics* pGraphics,
                       CFX_RectF& rtClip,
                       const CFX_Matrix* pMatrix);
  FX_BOOL IsNeedRepaint(IFWL_Widget* pWidget,
                        CFX_Matrix* pMatrix,
                        const CFX_RectF& rtDirty);
  FX_BOOL bUseOffscreenDirect(IFWL_Widget* pWidget);

  CFWL_WidgetMgr* m_pWidgetMgr;
};

#endif  // XFA_FWL_CORE_CFWL_WIDGETMGR_H_
