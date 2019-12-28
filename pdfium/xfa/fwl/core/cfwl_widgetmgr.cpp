// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/core/cfwl_widgetmgr.h"

#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/fwl_appimp.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_app.h"
#include "xfa/fwl/core/ifwl_form.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/include/xfa_ffapp.h"

namespace {

const int kNeedRepaintHitPoints = 12;
const int kNeedRepaintHitPiece = 3;

struct FWL_NEEDREPAINTHITDATA {
  CFX_PointF hitPoint;
  bool bNotNeedRepaint;
  bool bNotContainByDirty;
};

}  // namespace

FX_BOOL FWL_UseOffscreen(IFWL_Widget* pWidget) {
#if (_FX_OS_ == _FX_MACOSX_)
  return FALSE;
#else
  return pWidget->GetStyles() & FWL_WGTSTYLE_Offscreen;
#endif
}

// static
CFWL_WidgetMgr* CFWL_WidgetMgr::GetInstance() {
  IFWL_App* pApp = FWL_GetApp();
  return pApp ? pApp->GetWidgetMgr() : nullptr;
}

CFWL_WidgetMgr::CFWL_WidgetMgr(CXFA_FFApp* pAdapterNative)
    : m_dwCapability(0),
      m_pDelegate(new CFWL_WidgetMgrDelegate(this)),
      m_pAdapter(pAdapterNative->GetWidgetMgr(m_pDelegate.get())) {
  ASSERT(m_pAdapter);
  m_mapWidgetItem[nullptr].reset(new CFWL_WidgetMgrItem);
#if (_FX_OS_ == _FX_WIN32_DESKTOP_) || (_FX_OS_ == _FX_WIN64_)
  m_rtScreen.Reset();
#endif
}

CFWL_WidgetMgr::~CFWL_WidgetMgr() {}

IFWL_Widget* CFWL_WidgetMgr::GetParentWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pParent ? pItem->pParent->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetOwnerWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pOwner ? pItem->pOwner->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetFirstSiblingWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  pItem = pItem ? pItem->pPrevious : nullptr;  // Not self.
  while (pItem && pItem->pPrevious)
    pItem = pItem->pPrevious;

  return pItem ? pItem->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetPriorSiblingWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pPrevious ? pItem->pPrevious->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetNextSiblingWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pNext ? pItem->pNext->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetLastSiblingWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  pItem = pItem ? pItem->pNext : nullptr;  // Not self.
  while (pItem && pItem->pNext)
    pItem = pItem->pNext;

  return pItem ? pItem->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetFirstChildWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pChild ? pItem->pChild->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetLastChildWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  pItem = pItem ? pItem->pChild : nullptr;
  while (pItem && pItem->pNext)
    pItem = pItem->pNext;

  return pItem ? pItem->pWidget : nullptr;
}

IFWL_Widget* CFWL_WidgetMgr::GetSystemFormWidget(IFWL_Widget* pWidget) const {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  while (pItem) {
    if (IsAbleNative(pItem->pWidget))
      return pItem->pWidget;
    pItem = pItem->pParent;
  }
  return nullptr;
}

FX_BOOL CFWL_WidgetMgr::SetWidgetIndex(IFWL_Widget* pWidget, int32_t nIndex) {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem)
    return FALSE;
  if (!pItem->pParent)
    return FALSE;
  CFWL_WidgetMgrItem* pChild = pItem->pParent->pChild;
  int32_t i = 0;
  while (pChild) {
    if (pChild == pItem) {
      if (i == nIndex) {
        return TRUE;
      }
      if (pChild->pPrevious) {
        pChild->pPrevious->pNext = pChild->pNext;
      }
      if (pChild->pNext) {
        pChild->pNext->pPrevious = pChild->pPrevious;
      }
      if (pItem->pParent->pChild == pItem) {
        pItem->pParent->pChild = pItem->pNext;
      }
      pItem->pNext = nullptr;
      pItem->pPrevious = nullptr;
      break;
    }
    if (!pChild->pNext) {
      break;
    }
    pChild = pChild->pNext;
    ++i;
  }
  pChild = pItem->pParent->pChild;
  if (pChild) {
    if (nIndex < 0) {
      while (pChild->pNext) {
        pChild = pChild->pNext;
      }
      pChild->pNext = pItem;
      pItem->pPrevious = pChild;
      pItem->pNext = nullptr;
      return TRUE;
    }
    i = 0;
    while (i < nIndex && pChild->pNext) {
      pChild = pChild->pNext;
      ++i;
    }
    if (!pChild->pNext) {
      pChild->pNext = pItem;
      pItem->pPrevious = pChild;
      pItem->pNext = nullptr;
      return TRUE;
    }
    if (pChild->pPrevious) {
      pItem->pPrevious = pChild->pPrevious;
      pChild->pPrevious->pNext = pItem;
    }
    pChild->pPrevious = pItem;
    pItem->pNext = pChild;
    if (pItem->pParent->pChild == pChild) {
      pItem->pParent->pChild = pItem;
    }
  } else {
    pItem->pParent->pChild = pItem;
    pItem->pPrevious = nullptr;
    pItem->pNext = nullptr;
  }
  return TRUE;
}
FWL_Error CFWL_WidgetMgr::RepaintWidget(IFWL_Widget* pWidget,
                                        const CFX_RectF* pRect) {
  if (!m_pAdapter)
    return FWL_Error::Indefinite;
  IFWL_Widget* pNative = pWidget;
  CFX_RectF rect(*pRect);
  if (IsFormDisabled()) {
    IFWL_Widget* pOuter = pWidget->GetOuter();
    while (pOuter) {
      CFX_RectF rtTemp;
      pNative->GetWidgetRect(rtTemp);
      rect.left += rtTemp.left;
      rect.top += rtTemp.top;
      pNative = pOuter;
      pOuter = pOuter->GetOuter();
    }
  } else if (!IsAbleNative(pWidget)) {
    pNative = GetSystemFormWidget(pWidget);
    if (!pNative)
      return FWL_Error::Indefinite;
    pWidget->TransformTo(pNative, rect.left, rect.top);
  }
  AddRedrawCounts(pNative);
  return m_pAdapter->RepaintWidget(pNative, &rect);
}
void CFWL_WidgetMgr::AddWidget(IFWL_Widget* pWidget) {
  CFWL_WidgetMgrItem* pParentItem = GetWidgetMgrItem(nullptr);
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem) {
    pItem = new CFWL_WidgetMgrItem(pWidget);
    m_mapWidgetItem[pWidget].reset(pItem);
  }
  if (pItem->pParent && pItem->pParent != pParentItem) {
    if (pItem->pPrevious) {
      pItem->pPrevious->pNext = pItem->pNext;
    }
    if (pItem->pNext) {
      pItem->pNext->pPrevious = pItem->pPrevious;
    }
    if (pItem->pParent->pChild == pItem) {
      pItem->pParent->pChild = pItem->pNext;
    }
  }
  pItem->pParent = pParentItem;
  SetWidgetIndex(pWidget, -1);
}
void CFWL_WidgetMgr::InsertWidget(IFWL_Widget* pParent,
                                  IFWL_Widget* pChild,
                                  int32_t nIndex) {
  CFWL_WidgetMgrItem* pParentItem = GetWidgetMgrItem(pParent);
  if (!pParentItem) {
    pParentItem = new CFWL_WidgetMgrItem(pParent);
    m_mapWidgetItem[pParent].reset(pParentItem);
    pParentItem->pParent = GetWidgetMgrItem(nullptr);
    SetWidgetIndex(pParent, -1);
  }
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pChild);
  if (!pItem) {
    pItem = new CFWL_WidgetMgrItem(pChild);
    m_mapWidgetItem[pChild].reset(pItem);
  }
  if (pItem->pParent && pItem->pParent != pParentItem) {
    if (pItem->pPrevious) {
      pItem->pPrevious->pNext = pItem->pNext;
    }
    if (pItem->pNext) {
      pItem->pNext->pPrevious = pItem->pPrevious;
    }
    if (pItem->pParent->pChild == pItem) {
      pItem->pParent->pChild = pItem->pNext;
    }
  }
  pItem->pParent = pParentItem;
  SetWidgetIndex(pChild, nIndex);
}
void CFWL_WidgetMgr::RemoveWidget(IFWL_Widget* pWidget) {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem) {
    return;
  }
  if (pItem->pPrevious) {
    pItem->pPrevious->pNext = pItem->pNext;
  }
  if (pItem->pNext) {
    pItem->pNext->pPrevious = pItem->pPrevious;
  }
  if (pItem->pParent && pItem->pParent->pChild == pItem) {
    pItem->pParent->pChild = pItem->pNext;
  }
  CFWL_WidgetMgrItem* pChild = pItem->pChild;
  while (pChild) {
    CFWL_WidgetMgrItem* pNext = pChild->pNext;
    RemoveWidget(pChild->pWidget);
    pChild = pNext;
  }
  m_mapWidgetItem.erase(pWidget);
}
void CFWL_WidgetMgr::SetOwner(IFWL_Widget* pOwner, IFWL_Widget* pOwned) {
  CFWL_WidgetMgrItem* pParentItem = GetWidgetMgrItem(pOwner);
  if (!pParentItem) {
    pParentItem = new CFWL_WidgetMgrItem(pOwner);
    m_mapWidgetItem[pOwner].reset(pParentItem);
    pParentItem->pParent = GetWidgetMgrItem(nullptr);
    SetWidgetIndex(pOwner, -1);
  }
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pOwned);
  if (!pItem) {
    pItem = new CFWL_WidgetMgrItem(pOwned);
    m_mapWidgetItem[pOwned].reset(pItem);
  }
  pItem->pOwner = pParentItem;
}
void CFWL_WidgetMgr::SetParent(IFWL_Widget* pParent, IFWL_Widget* pChild) {
  CFWL_WidgetMgrItem* pParentItem = GetWidgetMgrItem(pParent);
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pChild);
  if (!pItem)
    return;
  if (pItem->pParent && pItem->pParent != pParentItem) {
    if (pItem->pPrevious) {
      pItem->pPrevious->pNext = pItem->pNext;
    }
    if (pItem->pNext) {
      pItem->pNext->pPrevious = pItem->pPrevious;
    }
    if (pItem->pParent->pChild == pItem) {
      pItem->pParent->pChild = pItem->pNext;
    }
    pItem->pNext = nullptr;
    pItem->pPrevious = nullptr;
  }
  pItem->pParent = pParentItem;
  SetWidgetIndex(pChild, -1);
}

FX_BOOL CFWL_WidgetMgr::IsChild(IFWL_Widget* pChild, IFWL_Widget* pParent) {
  IFWL_Widget* pTemp = pChild;
  do {
    if (pTemp == pParent)
      return TRUE;
    pTemp = GetParentWidget(pTemp);
  } while (pTemp);
  return FALSE;
}

FWL_Error CFWL_WidgetMgr::SetWidgetRect_Native(IFWL_Widget* pWidget,
                                               const CFX_RectF& rect) {
  if (FWL_UseOffscreen(pWidget)) {
    CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
    pItem->iRedrawCounter++;
    if (pItem->pOffscreen) {
      CFX_RenderDevice* pDevice = pItem->pOffscreen->GetRenderDevice();
      if (pDevice && pDevice->GetBitmap()) {
        CFX_DIBitmap* pBitmap = pDevice->GetBitmap();
        if (pBitmap->GetWidth() - rect.width > 1 ||
            pBitmap->GetHeight() - rect.height > 1) {
          pItem->pOffscreen.reset();
        }
      }
    }
#if (_FX_OS_ == _FX_WIN32_DESKTOP_) || (_FX_OS_ == _FX_WIN64_)
    pItem->bOutsideChanged = !m_rtScreen.Contains(rect);
#endif
  }
  return FWL_Error::Succeeded;
}

IFWL_Widget* CFWL_WidgetMgr::GetWidgetAtPoint(IFWL_Widget* parent,
                                              FX_FLOAT x,
                                              FX_FLOAT y) {
  if (!parent)
    return nullptr;
  FX_FLOAT x1;
  FX_FLOAT y1;
  IFWL_Widget* child = GetLastChildWidget(parent);
  while (child) {
    if ((child->GetStates() & FWL_WGTSTATE_Invisible) == 0) {
      x1 = x;
      y1 = y;
      CFX_Matrix matrixOnParent;
      child->GetMatrix(matrixOnParent);
      CFX_Matrix m;
      m.SetIdentity();
      m.SetReverse(matrixOnParent);
      m.TransformPoint(x1, y1);
      CFX_RectF bounds;
      child->GetWidgetRect(bounds);
      if (bounds.Contains(x1, y1)) {
        x1 -= bounds.left;
        y1 -= bounds.top;
        return GetWidgetAtPoint(child, x1, y1);
      }
    }
    child = GetPriorSiblingWidget(child);
  }
  return parent;
}

void CFWL_WidgetMgr::NotifySizeChanged(IFWL_Widget* pForm,
                                       FX_FLOAT fx,
                                       FX_FLOAT fy) {
  if (FWL_UseOffscreen(pForm))
    GetWidgetMgrItem(pForm)->pOffscreen.reset();
}

IFWL_Widget* CFWL_WidgetMgr::nextTab(IFWL_Widget* parent,
                                     IFWL_Widget* focus,
                                     FX_BOOL& bFind) {
  CFWL_WidgetMgr* pMgr = CFWL_WidgetMgr::GetInstance();
  IFWL_Widget* child = pMgr->GetFirstChildWidget(parent);
  while (child) {
    if (focus == child)
      bFind = TRUE;

    if ((child->GetStyles() & FWL_WGTSTYLE_TabStop) &&
        (!focus || (focus != child && bFind))) {
      return child;
    }
    IFWL_Widget* bRet = nextTab(child, focus, bFind);
    if (bRet)
      return bRet;

    child = pMgr->GetNextSiblingWidget(child);
  }
  return nullptr;
}

int32_t CFWL_WidgetMgr::CountRadioButtonGroup(IFWL_Widget* pFirst) {
  int32_t iRet = 0;
  IFWL_Widget* pChild = pFirst;
  while (pChild) {
    pChild = GetNextSiblingWidget(pChild);
    ++iRet;
  }
  return iRet;
}
IFWL_Widget* CFWL_WidgetMgr::GetSiblingRadioButton(IFWL_Widget* pWidget,
                                                   FX_BOOL bNext) {
  return nullptr;
}
IFWL_Widget* CFWL_WidgetMgr::GetRadioButtonGroupHeader(
    IFWL_Widget* pRadioButton) {
  IFWL_Widget* pNext = pRadioButton;
  while (pNext) {
    if (pNext->GetStyles() & FWL_WGTSTYLE_Group)
      return pNext;
    pNext = GetSiblingRadioButton(pNext, FALSE);
  }
  pNext = GetLastSiblingWidget(pRadioButton);
  while ((pNext = GetSiblingRadioButton(pNext, FALSE)) != nullptr &&
         pNext != pRadioButton) {
    if (pNext->GetStyles() & FWL_WGTSTYLE_Group)
      return pNext;
  }
  pNext = GetFirstSiblingWidget(pRadioButton);
  return GetSiblingRadioButton(pNext, TRUE);
}
void CFWL_WidgetMgr::GetSameGroupRadioButton(
    IFWL_Widget* pRadioButton,
    CFX_ArrayTemplate<IFWL_Widget*>& group) {
  IFWL_Widget* pFirst = GetFirstSiblingWidget(pRadioButton);
  if (!pFirst) {
    pFirst = pRadioButton;
  }
  int32_t iGroup = CountRadioButtonGroup(pFirst);
  if (iGroup < 2) {
    IFWL_Widget* pNext = pFirst;
    while ((pNext = GetSiblingRadioButton(pNext, TRUE)) != nullptr) {
      group.Add(pNext);
    }
    return;
  }
  IFWL_Widget* pNext = GetRadioButtonGroupHeader(pRadioButton);
  do {
    group.Add(pNext);
    pNext = GetSiblingRadioButton(pNext, TRUE);
    if (!pNext)
      pNext = GetSiblingRadioButton(pFirst, TRUE);
  } while (pNext && ((pNext->GetStyles() & FWL_WGTSTYLE_Group) == 0));
}
IFWL_Widget* CFWL_WidgetMgr::GetDefaultButton(IFWL_Widget* pParent) {
  if ((pParent->GetClassID() == FWL_Type::PushButton) &&
      (pParent->GetStates() & (1 << (FWL_WGTSTATE_MAX + 2)))) {
    return pParent;
  }
  IFWL_Widget* child =
      CFWL_WidgetMgr::GetInstance()->GetFirstChildWidget(pParent);
  while (child) {
    if ((child->GetClassID() == FWL_Type::PushButton) &&
        (child->GetStates() & (1 << (FWL_WGTSTATE_MAX + 2)))) {
      return child;
    }
    IFWL_Widget* find = GetDefaultButton(child);
    if (find) {
      return find;
    }
    child = CFWL_WidgetMgr::GetInstance()->GetNextSiblingWidget(child);
  }
  return nullptr;
}
void CFWL_WidgetMgr::AddRedrawCounts(IFWL_Widget* pWidget) {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  (pItem->iRedrawCounter)++;
}
void CFWL_WidgetMgr::ResetRedrawCounts(IFWL_Widget* pWidget) {
  CFWL_WidgetMgrItem* pItem = GetWidgetMgrItem(pWidget);
  pItem->iRedrawCounter = 0;
}
CFWL_WidgetMgrItem* CFWL_WidgetMgr::GetWidgetMgrItem(
    IFWL_Widget* pWidget) const {
  auto it = m_mapWidgetItem.find(pWidget);
  return it != m_mapWidgetItem.end()
             ? static_cast<CFWL_WidgetMgrItem*>(it->second.get())
             : nullptr;
}
int32_t CFWL_WidgetMgr::TravelWidgetMgr(CFWL_WidgetMgrItem* pParent,
                                        int32_t* pIndex,
                                        CFWL_WidgetMgrItem* pItem,
                                        IFWL_Widget** pWidget) {
  if (!pParent) {
    return 0;
  }
  int32_t iCount = 0;
  CFWL_WidgetMgrItem* pChild = pParent->pChild;
  while (pChild) {
    iCount++;
    if (pIndex) {
      if (*pIndex == 0) {
        *pWidget = pChild->pWidget;
        return iCount;
      }
      pIndex--;
    }
    if (pItem && pItem == pChild) {
      return iCount - 1;
    }
    pChild = pChild->pNext;
  }
  if (pIndex) {
    return 0;
  } else if (pItem) {
    return -1;
  }
  return iCount - 1;
}

FX_BOOL CFWL_WidgetMgr::IsAbleNative(IFWL_Widget* pWidget) const {
  if (!pWidget)
    return FALSE;
  if (!pWidget->IsInstance(FX_WSTRC(FWL_CLASS_Form))) {
    return FALSE;
  }
  uint32_t dwStyles = pWidget->GetStyles();
  return ((dwStyles & FWL_WGTSTYLE_WindowTypeMask) ==
          FWL_WGTSTYLE_OverLapper) ||
         (dwStyles & FWL_WGTSTYLE_Popup);
}

bool CFWL_WidgetMgr::IsThreadEnabled() {
  return !(m_dwCapability & FWL_WGTMGR_DisableThread);
}

bool CFWL_WidgetMgr::IsFormDisabled() {
  return !!(m_dwCapability & FWL_WGTMGR_DisableForm);
}

FX_BOOL CFWL_WidgetMgr::GetAdapterPopupPos(IFWL_Widget* pWidget,
                                           FX_FLOAT fMinHeight,
                                           FX_FLOAT fMaxHeight,
                                           const CFX_RectF& rtAnchor,
                                           CFX_RectF& rtPopup) {
  CXFA_FWLAdapterWidgetMgr* pSDApapter = GetAdapterWidgetMgr();
  return pSDApapter->GetPopupPos(pWidget, fMinHeight, fMaxHeight, rtAnchor,
                                 rtPopup);
}
CFWL_WidgetMgrDelegate::CFWL_WidgetMgrDelegate(CFWL_WidgetMgr* pWidgetMgr)
    : m_pWidgetMgr(pWidgetMgr) {}
FWL_Error CFWL_WidgetMgrDelegate::OnSetCapability(uint32_t dwCapability) {
  m_pWidgetMgr->m_dwCapability = dwCapability;
  return FWL_Error::Succeeded;
}

void CFWL_WidgetMgrDelegate::OnProcessMessageToForm(CFWL_Message* pMessage) {
  if (!pMessage)
    return;
  if (!pMessage->m_pDstTarget)
    return;

  IFWL_Widget* pDstWidget = pMessage->m_pDstTarget;
  IFWL_App* pApp = pDstWidget->GetOwnerApp();
  if (!pApp)
    return;

  CFWL_NoteDriver* pNoteDriver =
      static_cast<CFWL_NoteDriver*>(pApp->GetNoteDriver());
  if (!pNoteDriver)
    return;

  if (m_pWidgetMgr->IsThreadEnabled())
    pMessage = static_cast<CFWL_Message*>(pMessage->Clone());
  if (m_pWidgetMgr->IsFormDisabled())
    pNoteDriver->ProcessMessage(pMessage);
  else
    pNoteDriver->QueueMessage(pMessage);

#if (_FX_OS_ == _FX_MACOSX_)
  CFWL_NoteLoop* pTopLoop = pNoteDriver->GetTopLoop();
  if (pTopLoop)
    pNoteDriver->UnqueueMessage(pTopLoop);
#endif

  if (m_pWidgetMgr->IsThreadEnabled())
    pMessage->Release();

  return;
}

void CFWL_WidgetMgrDelegate::OnDrawWidget(IFWL_Widget* pWidget,
                                          CFX_Graphics* pGraphics,
                                          const CFX_Matrix* pMatrix) {
  if (!pWidget || !pGraphics)
    return;

  CFX_Graphics* pTemp = DrawWidgetBefore(pWidget, pGraphics, pMatrix);
  CFX_RectF clipCopy;
  pWidget->GetWidgetRect(clipCopy);
  clipCopy.left = clipCopy.top = 0;
  if (bUseOffscreenDirect(pWidget)) {
    DrawWidgetAfter(pWidget, pGraphics, clipCopy, pMatrix);
    return;
  }
  CFX_RectF clipBounds;

#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_ || \
    _FX_OS_ == _FX_LINUX_DESKTOP_ || _FX_OS_ == _FX_ANDROID_
  IFWL_WidgetDelegate* pDelegate = pWidget->SetDelegate(nullptr);
  pDelegate->OnDrawWidget(pTemp, pMatrix);
  pGraphics->GetClipRect(clipBounds);
  clipCopy = clipBounds;
#elif _FX_OS_ == _FX_MACOSX_
  if (m_pWidgetMgr->IsFormDisabled()) {
    IFWL_WidgetDelegate* pDelegate = pWidget->SetDelegate(nullptr);
    pDelegate->OnDrawWidget(pTemp, pMatrix);
    pGraphics->GetClipRect(clipBounds);
    clipCopy = clipBounds;
  } else {
    clipBounds.Set(pMatrix->a, pMatrix->b, pMatrix->c, pMatrix->d);
    const_cast<CFX_Matrix*>(pMatrix)->SetIdentity();  // FIXME: const cast.
    IFWL_WidgetDelegate* pDelegate = pWidget->SetDelegate(nullptr);
    pDelegate->OnDrawWidget(pTemp, pMatrix);
  }
#endif  // _FX_OS_ == _FX_MACOSX_

  if (!m_pWidgetMgr->IsFormDisabled()) {
    CFX_RectF rtClient;
    pWidget->GetClientRect(rtClient);
    clipBounds.Intersect(rtClient);
  }
  if (!clipBounds.IsEmpty())
    DrawChild(pWidget, clipBounds, pTemp, pMatrix);

  DrawWidgetAfter(pWidget, pGraphics, clipCopy, pMatrix);
  m_pWidgetMgr->ResetRedrawCounts(pWidget);
}

void CFWL_WidgetMgrDelegate::DrawChild(IFWL_Widget* parent,
                                       const CFX_RectF& rtClip,
                                       CFX_Graphics* pGraphics,
                                       const CFX_Matrix* pMatrix) {
  if (!parent)
    return;

  FX_BOOL bFormDisable = m_pWidgetMgr->IsFormDisabled();
  IFWL_Widget* pNextChild = m_pWidgetMgr->GetFirstChildWidget(parent);
  while (pNextChild) {
    IFWL_Widget* child = pNextChild;
    pNextChild = m_pWidgetMgr->GetNextSiblingWidget(child);
    if (child->GetStates() & FWL_WGTSTATE_Invisible)
      continue;

    CFX_RectF rtWidget;
    child->GetWidgetRect(rtWidget);
    if (rtWidget.IsEmpty())
      continue;

    CFX_Matrix widgetMatrix;
    CFX_RectF clipBounds(rtWidget);
    if (!bFormDisable)
      child->GetMatrix(widgetMatrix, TRUE);
    if (pMatrix)
      widgetMatrix.Concat(*pMatrix);

    if (!bFormDisable) {
      widgetMatrix.TransformPoint(clipBounds.left, clipBounds.top);
      clipBounds.Intersect(rtClip);
      if (clipBounds.IsEmpty())
        continue;

      pGraphics->SaveGraphState();
      pGraphics->SetClipRect(clipBounds);
    }
    widgetMatrix.Translate(rtWidget.left, rtWidget.top, TRUE);
    IFWL_WidgetDelegate* pDelegate = child->SetDelegate(nullptr);
    if (pDelegate) {
      if (m_pWidgetMgr->IsFormDisabled() ||
          IsNeedRepaint(child, &widgetMatrix, rtClip)) {
        pDelegate->OnDrawWidget(pGraphics, &widgetMatrix);
      }
    }
    if (!bFormDisable)
      pGraphics->RestoreGraphState();

    DrawChild(child, clipBounds, pGraphics,
              bFormDisable ? &widgetMatrix : pMatrix);
    child = m_pWidgetMgr->GetNextSiblingWidget(child);
  }
}

CFX_Graphics* CFWL_WidgetMgrDelegate::DrawWidgetBefore(
    IFWL_Widget* pWidget,
    CFX_Graphics* pGraphics,
    const CFX_Matrix* pMatrix) {
  if (!FWL_UseOffscreen(pWidget))
    return pGraphics;

  CFWL_WidgetMgrItem* pItem = m_pWidgetMgr->GetWidgetMgrItem(pWidget);
  if (!pItem->pOffscreen) {
    pItem->pOffscreen.reset(new CFX_Graphics);
    CFX_RectF rect;
    pWidget->GetWidgetRect(rect);
    pItem->pOffscreen->Create((int32_t)rect.width, (int32_t)rect.height,
                              FXDIB_Argb);
  }
  CFX_RectF rect;
  pGraphics->GetClipRect(rect);
  pItem->pOffscreen->SetClipRect(rect);
  return pItem->pOffscreen.get();
}

void CFWL_WidgetMgrDelegate::DrawWidgetAfter(IFWL_Widget* pWidget,
                                             CFX_Graphics* pGraphics,
                                             CFX_RectF& rtClip,
                                             const CFX_Matrix* pMatrix) {
  if (FWL_UseOffscreen(pWidget)) {
    CFWL_WidgetMgrItem* pItem = m_pWidgetMgr->GetWidgetMgrItem(pWidget);
    pGraphics->Transfer(pItem->pOffscreen.get(), rtClip.left, rtClip.top,
                        rtClip, pMatrix);
#ifdef _WIN32
    pItem->pOffscreen->ClearClip();
#endif
  }
  CFWL_WidgetMgrItem* pItem = m_pWidgetMgr->GetWidgetMgrItem(pWidget);
  pItem->iRedrawCounter = 0;
}

FX_BOOL CFWL_WidgetMgrDelegate::IsNeedRepaint(IFWL_Widget* pWidget,
                                              CFX_Matrix* pMatrix,
                                              const CFX_RectF& rtDirty) {
  CFWL_WidgetMgrItem* pItem = m_pWidgetMgr->GetWidgetMgrItem(pWidget);
  if (pItem && pItem->iRedrawCounter > 0) {
    pItem->iRedrawCounter = 0;
    return TRUE;
  }
  CFX_RectF rtWidget;
  pWidget->GetWidgetRect(rtWidget);
  rtWidget.left = rtWidget.top = 0;
  pMatrix->TransformRect(rtWidget);
  if (!rtWidget.IntersectWith(rtDirty))
    return FALSE;

  IFWL_Widget* pChild =
      CFWL_WidgetMgr::GetInstance()->GetFirstChildWidget(pWidget);
  if (!pChild)
    return TRUE;

  CFX_RectF rtChilds;
  rtChilds.Empty();
  FX_BOOL bChildIntersectWithDirty = FALSE;
  FX_BOOL bOrginPtIntersectWidthChild = FALSE;
  FX_BOOL bOrginPtIntersectWidthDirty =
      rtDirty.Contains(rtWidget.left, rtWidget.top);
  static FWL_NEEDREPAINTHITDATA hitPoint[kNeedRepaintHitPoints];
  FXSYS_memset(hitPoint, 0, sizeof(hitPoint));
  FX_FLOAT fxPiece = rtWidget.width / kNeedRepaintHitPiece;
  FX_FLOAT fyPiece = rtWidget.height / kNeedRepaintHitPiece;
  hitPoint[2].hitPoint.x = hitPoint[6].hitPoint.x = rtWidget.left;
  hitPoint[0].hitPoint.x = hitPoint[3].hitPoint.x = hitPoint[7].hitPoint.x =
      hitPoint[10].hitPoint.x = fxPiece + rtWidget.left;
  hitPoint[1].hitPoint.x = hitPoint[4].hitPoint.x = hitPoint[8].hitPoint.x =
      hitPoint[11].hitPoint.x = fxPiece * 2 + rtWidget.left;
  hitPoint[5].hitPoint.x = hitPoint[9].hitPoint.x =
      rtWidget.width + rtWidget.left;
  hitPoint[0].hitPoint.y = hitPoint[1].hitPoint.y = rtWidget.top;
  hitPoint[2].hitPoint.y = hitPoint[3].hitPoint.y = hitPoint[4].hitPoint.y =
      hitPoint[5].hitPoint.y = fyPiece + rtWidget.top;
  hitPoint[6].hitPoint.y = hitPoint[7].hitPoint.y = hitPoint[8].hitPoint.y =
      hitPoint[9].hitPoint.y = fyPiece * 2 + rtWidget.top;
  hitPoint[10].hitPoint.y = hitPoint[11].hitPoint.y =
      rtWidget.height + rtWidget.top;
  do {
    CFX_RectF rect;
    pChild->GetWidgetRect(rect);
    CFX_RectF r = rect;
    r.left += rtWidget.left;
    r.top += rtWidget.top;
    if (r.IsEmpty())
      continue;
    if (r.Contains(rtDirty))
      return FALSE;
    if (!bChildIntersectWithDirty && r.IntersectWith(rtDirty))
      bChildIntersectWithDirty = TRUE;
    if (bOrginPtIntersectWidthDirty && !bOrginPtIntersectWidthChild)
      bOrginPtIntersectWidthChild = rect.Contains(0, 0);

    if (rtChilds.IsEmpty())
      rtChilds = rect;
    else if (!(pChild->GetStates() & FWL_WGTSTATE_Invisible))
      rtChilds.Union(rect);

    for (int32_t i = 0; i < kNeedRepaintHitPoints; i++) {
      if (hitPoint[i].bNotContainByDirty || hitPoint[i].bNotNeedRepaint)
        continue;
      if (!rtDirty.Contains(hitPoint[i].hitPoint)) {
        hitPoint[i].bNotContainByDirty = true;
        continue;
      }
      if (r.Contains(hitPoint[i].hitPoint))
        hitPoint[i].bNotNeedRepaint = true;
    }
    pChild = CFWL_WidgetMgr::GetInstance()->GetNextSiblingWidget(pChild);
  } while (pChild);

  if (!bChildIntersectWithDirty)
    return TRUE;
  if (bOrginPtIntersectWidthDirty && !bOrginPtIntersectWidthChild)
    return TRUE;
  if (rtChilds.IsEmpty())
    return TRUE;

  int32_t repaintPoint = kNeedRepaintHitPoints;
  for (int32_t i = 0; i < kNeedRepaintHitPoints; i++) {
    if (hitPoint[i].bNotNeedRepaint)
      repaintPoint--;
  }
  if (repaintPoint > 0)
    return TRUE;

  pMatrix->TransformRect(rtChilds);
  if (rtChilds.Contains(rtDirty) || rtChilds.Contains(rtWidget))
    return FALSE;
  return TRUE;
}

FX_BOOL CFWL_WidgetMgrDelegate::bUseOffscreenDirect(IFWL_Widget* pWidget) {
  CFWL_WidgetMgrItem* pItem = m_pWidgetMgr->GetWidgetMgrItem(pWidget);
  if (!FWL_UseOffscreen(pWidget) || !(pItem->pOffscreen))
    return FALSE;

#if (_FX_OS_ == _FX_WIN32_DESKTOP_) || (_FX_OS_ == _FX_WIN64_)
  if (pItem->bOutsideChanged) {
    CFX_RectF r;
    pWidget->GetWidgetRect(r);
    CFX_RectF temp(m_pWidgetMgr->m_rtScreen);
    temp.Deflate(50, 50);
    if (!temp.Contains(r))
      return FALSE;

    pItem->bOutsideChanged = FALSE;
  }
#endif

  return pItem->iRedrawCounter == 0;
}

CFWL_WidgetMgrItem::CFWL_WidgetMgrItem() : CFWL_WidgetMgrItem(nullptr) {}

CFWL_WidgetMgrItem::CFWL_WidgetMgrItem(IFWL_Widget* widget)
    : pParent(nullptr),
      pOwner(nullptr),
      pChild(nullptr),
      pPrevious(nullptr),
      pNext(nullptr),
      pWidget(widget),
      iRedrawCounter(0)
#if (_FX_OS_ == _FX_WIN32_DESKTOP_) || (_FX_OS_ == _FX_WIN64_)
      ,
      bOutsideChanged(FALSE)
#endif
{
}

CFWL_WidgetMgrItem::~CFWL_WidgetMgrItem() {}
