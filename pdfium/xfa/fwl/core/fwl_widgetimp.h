// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_FWL_WIDGETIMP_H_
#define XFA_FWL_CORE_FWL_WIDGETIMP_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/core/cfwl_themepart.h"
#include "xfa/fwl/core/ifwl_widgetdelegate.h"
#include "xfa/fwl/core/include/fwl_widgethit.h"
#include "xfa/fwl/theme/cfwl_widgettp.h"

class CFWL_AppImp;
class CFWL_MsgKey;
class CFWL_WidgetImpProperties;
class CFWL_WidgetMgr;
class IFWL_App;
class IFWL_DataProvider;
class IFWL_ThemeProvider;
class IFWL_Widget;
enum class FWL_Type;

class CFWL_WidgetImp {
 public:
  virtual ~CFWL_WidgetImp();

  virtual FWL_Error Initialize();
  virtual FWL_Error Finalize();
  virtual FWL_Error GetClassName(CFX_WideString& wsClass) const;
  virtual FWL_Type GetClassID() const = 0;
  virtual FX_BOOL IsInstance(const CFX_WideStringC& wsClass) const;

  virtual FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE);
  virtual FWL_Error GetGlobalRect(CFX_RectF& rect);
  virtual FWL_Error SetWidgetRect(const CFX_RectF& rect);
  virtual FWL_Error GetClientRect(CFX_RectF& rect);
  virtual IFWL_Widget* GetParent();
  virtual FWL_Error SetParent(IFWL_Widget* pParent);
  virtual IFWL_Widget* GetOwner();
  virtual FWL_Error SetOwner(IFWL_Widget* pOwner);
  virtual IFWL_Widget* GetOuter();
  virtual uint32_t GetStyles();
  virtual FWL_Error ModifyStyles(uint32_t dwStylesAdded,
                                 uint32_t dwStylesRemoved);
  virtual uint32_t GetStylesEx();
  virtual FWL_Error ModifyStylesEx(uint32_t dwStylesExAdded,
                                   uint32_t dwStylesExRemoved);
  virtual uint32_t GetStates();
  virtual void SetStates(uint32_t dwStates, FX_BOOL bSet = TRUE);
  virtual FWL_Error Update();
  virtual FWL_Error LockUpdate();
  virtual FWL_Error UnlockUpdate();
  virtual FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy);
  virtual FWL_Error TransformTo(IFWL_Widget* pWidget,
                                FX_FLOAT& fx,
                                FX_FLOAT& fy);
  virtual FWL_Error TransformTo(IFWL_Widget* pWidget, CFX_RectF& rt);
  virtual FWL_Error GetMatrix(CFX_Matrix& matrix, FX_BOOL bGlobal = FALSE);
  virtual FWL_Error SetMatrix(const CFX_Matrix& matrix);
  virtual FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                               const CFX_Matrix* pMatrix = nullptr);
  virtual IFWL_ThemeProvider* GetThemeProvider();
  virtual FWL_Error SetThemeProvider(IFWL_ThemeProvider* pThemeProvider);
  virtual FWL_Error SetDataProvider(IFWL_DataProvider* pDataProvider);
  virtual IFWL_WidgetDelegate* SetDelegate(IFWL_WidgetDelegate* pDelegate);
  virtual IFWL_App* GetOwnerApp() const;

  FWL_Error SetOwnerApp(CFWL_AppImp* pOwnerApp);
  IFWL_Widget* GetInterface() const;
  void SetInterface(IFWL_Widget* pInterface);
  CFX_SizeF GetOffsetFromParent(IFWL_Widget* pParent);
  uint32_t GetEventKey() const;
  void SetEventKey(uint32_t key);
  void* GetLayoutItem() const;
  void SetLayoutItem(void* pItem);
  void* GetAssociateWidget() const;
  void SetAssociateWidget(void* pAssociate);

 protected:
  friend class CFWL_WidgetImpDelegate;

  CFWL_WidgetImp(const CFWL_WidgetImpProperties& properties,
                 IFWL_Widget* pOuter);

  FX_BOOL IsEnabled() const;
  FX_BOOL IsVisible() const;
  FX_BOOL IsActive() const;
  FX_BOOL IsOverLapper() const;
  FX_BOOL IsPopup() const;
  FX_BOOL IsChild() const;
  FX_BOOL IsLocked() const;
  FX_BOOL IsOffscreen() const;
  FX_BOOL HasBorder() const;
  FX_BOOL HasEdge() const;
  void GetEdgeRect(CFX_RectF& rtEdge);
  FX_FLOAT GetBorderSize(FX_BOOL bCX = TRUE);
  FX_FLOAT GetEdgeWidth();
  void GetRelativeRect(CFX_RectF& rect);
  void* GetThemeCapacity(CFWL_WidgetCapacity dwCapacity);
  IFWL_ThemeProvider* GetAvailableTheme();
  CFWL_WidgetImp* GetRootOuter();
  CFX_SizeF CalcTextSize(const CFX_WideString& wsText,
                         IFWL_ThemeProvider* pTheme,
                         FX_BOOL bMultiLine = FALSE,
                         int32_t iLineWidth = -1);
  void CalcTextRect(const CFX_WideString& wsText,
                    IFWL_ThemeProvider* pTheme,
                    uint32_t dwTTOStyles,
                    int32_t iTTOAlign,
                    CFX_RectF& rect);
  void SetFocus(FX_BOOL bFocus);
  void SetGrab(FX_BOOL bSet);
  FX_BOOL GetPopupPos(FX_FLOAT fMinHeight,
                      FX_FLOAT fMaxHeight,
                      const CFX_RectF& rtAnchor,
                      CFX_RectF& rtPopup);
  FX_BOOL GetPopupPosMenu(FX_FLOAT fMinHeight,
                          FX_FLOAT fMaxHeight,
                          const CFX_RectF& rtAnchor,
                          CFX_RectF& rtPopup);
  FX_BOOL GetPopupPosComboBox(FX_FLOAT fMinHeight,
                              FX_FLOAT fMaxHeight,
                              const CFX_RectF& rtAnchor,
                              CFX_RectF& rtPopup);
  FX_BOOL GetPopupPosGeneral(FX_FLOAT fMinHeight,
                             FX_FLOAT fMaxHeight,
                             const CFX_RectF& rtAnchor,
                             CFX_RectF& rtPopup);
  FX_BOOL GetScreenSize(FX_FLOAT& fx, FX_FLOAT& fy);
  void RegisterEventTarget(IFWL_Widget* pEventSource = nullptr,
                           uint32_t dwFilter = FWL_EVENT_ALL_MASK);
  void UnregisterEventTarget();
  void DispatchKeyEvent(CFWL_MsgKey* pNote);
  void DispatchEvent(CFWL_Event* pEvent);
  void Repaint(const CFX_RectF* pRect = nullptr);
  void DrawBackground(CFX_Graphics* pGraphics,
                      CFWL_Part iPartBk,
                      IFWL_ThemeProvider* pTheme,
                      const CFX_Matrix* pMatrix = nullptr);
  void DrawBorder(CFX_Graphics* pGraphics,
                  CFWL_Part iPartBorder,
                  IFWL_ThemeProvider* pTheme,
                  const CFX_Matrix* pMatrix = nullptr);
  void DrawEdge(CFX_Graphics* pGraphics,
                CFWL_Part iPartEdge,
                IFWL_ThemeProvider* pTheme,
                const CFX_Matrix* pMatrix = nullptr);
  void NotifyDriver();

  FX_BOOL IsParent(IFWL_Widget* pParent);

  CFWL_WidgetMgr* m_pWidgetMgr;
  CFWL_AppImp* m_pOwnerApp;
  CFWL_WidgetImpProperties* m_pProperties;
  IFWL_WidgetDelegate* m_pDelegate;
  IFWL_WidgetDelegate* m_pCurDelegate;
  IFWL_Widget* m_pOuter;
  IFWL_Widget* m_pInterface;
  void* m_pLayoutItem;
  void* m_pAssociate;
  int32_t m_iLock;
  uint32_t m_nEventKey;
};

class CFWL_WidgetImpDelegate : public IFWL_WidgetDelegate {
 public:
  CFWL_WidgetImpDelegate();
  ~CFWL_WidgetImpDelegate() override {}
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;
};

#endif  // XFA_FWL_CORE_FWL_WIDGETIMP_H_
