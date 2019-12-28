// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_XFA_FFWIDGETHANDLER_H_
#define XFA_FXFA_INCLUDE_XFA_FFWIDGETHANDLER_H_

#include <vector>

#include "xfa/fxfa/include/cxfa_eventparam.h"
#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/parser/xfa_document.h"

class CXFA_FFDocView;
enum class FWL_WidgetHit;

class CXFA_FFWidgetHandler {
 public:
  CXFA_FFWidgetHandler(CXFA_FFDocView* pDocView);
  ~CXFA_FFWidgetHandler();

  CXFA_FFWidget* CreateWidget(CXFA_FFWidget* hParent,
                              XFA_WIDGETTYPE eType,
                              CXFA_FFWidget* hBefore = nullptr);

  FX_BOOL OnMouseEnter(CXFA_FFWidget* hWidget);
  FX_BOOL OnMouseExit(CXFA_FFWidget* hWidget);
  FX_BOOL OnLButtonDown(CXFA_FFWidget* hWidget,
                        uint32_t dwFlags,
                        FX_FLOAT fx,
                        FX_FLOAT fy);
  FX_BOOL OnLButtonUp(CXFA_FFWidget* hWidget,
                      uint32_t dwFlags,
                      FX_FLOAT fx,
                      FX_FLOAT fy);
  FX_BOOL OnLButtonDblClk(CXFA_FFWidget* hWidget,
                          uint32_t dwFlags,
                          FX_FLOAT fx,
                          FX_FLOAT fy);
  FX_BOOL OnMouseMove(CXFA_FFWidget* hWidget,
                      uint32_t dwFlags,
                      FX_FLOAT fx,
                      FX_FLOAT fy);
  FX_BOOL OnMouseWheel(CXFA_FFWidget* hWidget,
                       uint32_t dwFlags,
                       int16_t zDelta,
                       FX_FLOAT fx,
                       FX_FLOAT fy);
  FX_BOOL OnRButtonDown(CXFA_FFWidget* hWidget,
                        uint32_t dwFlags,
                        FX_FLOAT fx,
                        FX_FLOAT fy);
  FX_BOOL OnRButtonUp(CXFA_FFWidget* hWidget,
                      uint32_t dwFlags,
                      FX_FLOAT fx,
                      FX_FLOAT fy);
  FX_BOOL OnRButtonDblClk(CXFA_FFWidget* hWidget,
                          uint32_t dwFlags,
                          FX_FLOAT fx,
                          FX_FLOAT fy);

  FX_BOOL OnKeyDown(CXFA_FFWidget* hWidget,
                    uint32_t dwKeyCode,
                    uint32_t dwFlags);
  FX_BOOL OnKeyUp(CXFA_FFWidget* hWidget, uint32_t dwKeyCode, uint32_t dwFlags);
  FX_BOOL OnChar(CXFA_FFWidget* hWidget, uint32_t dwChar, uint32_t dwFlags);
  FWL_WidgetHit OnHitTest(CXFA_FFWidget* hWidget, FX_FLOAT fx, FX_FLOAT fy);
  FX_BOOL OnSetCursor(CXFA_FFWidget* hWidget, FX_FLOAT fx, FX_FLOAT fy);
  void RenderWidget(CXFA_FFWidget* hWidget,
                    CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    FX_BOOL bHighlight);
  FX_BOOL HasEvent(CXFA_WidgetAcc* pWidgetAcc, XFA_EVENTTYPE eEventType);
  int32_t ProcessEvent(CXFA_WidgetAcc* pWidgetAcc, CXFA_EventParam* pParam);

 protected:
  CXFA_Node* CreateWidgetFormItem(XFA_WIDGETTYPE eType,
                                  CXFA_Node* pParent,
                                  CXFA_Node* pBefore) const;

  CXFA_Node* CreatePushButton(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateCheckButton(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateExclGroup(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateRadioButton(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDatetimeEdit(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDecimalField(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateNumericField(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateSignature(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateTextEdit(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDropdownList(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateListBox(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateImageField(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreatePasswordEdit(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateField(XFA_Element eElement,
                         CXFA_Node* pParent,
                         CXFA_Node* pBefore) const;
  CXFA_Node* CreateArc(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateRectangle(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateImage(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateLine(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateText(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDraw(XFA_Element eElement,
                        CXFA_Node* pParent,
                        CXFA_Node* pBefore) const;

  CXFA_Node* CreateSubform(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateFormItem(XFA_Element eElement,
                            CXFA_Node* pParent,
                            CXFA_Node* pBefore) const;
  CXFA_Node* CreateCopyNode(XFA_Element eElement,
                            CXFA_Node* pParent,
                            CXFA_Node* pBefore = nullptr) const;
  CXFA_Node* CreateTemplateNode(XFA_Element eElement,
                                CXFA_Node* pParent,
                                CXFA_Node* pBefore) const;
  CXFA_Node* CreateFontNode(CXFA_Node* pParent) const;
  CXFA_Node* CreateMarginNode(CXFA_Node* pParent,
                              uint32_t dwFlags,
                              FX_FLOAT fInsets[4]) const;
  CXFA_Node* CreateValueNode(XFA_Element eValue, CXFA_Node* pParent) const;
  CXFA_Document* GetObjFactory() const;
  CXFA_Document* GetXFADoc() const;

  CXFA_FFDocView* m_pDocView;
};

#endif  //  XFA_FXFA_INCLUDE_XFA_FFWIDGETHANDLER_H_
