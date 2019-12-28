// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_FXFA_WIDGET_H_
#define XFA_FXFA_INCLUDE_FXFA_WIDGET_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxge/include/fx_dib.h"
#include "xfa/fxfa/parser/cxfa_box.h"
#include "xfa/fxfa/parser/cxfa_event.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_script.h"
#include "xfa/fxfa/parser/cxfa_value.h"
#include "xfa/fxfa/parser/cxfa_widgetdata.h"

class CFGAS_GEFont;
class CXFA_EventParam;
class CXFA_FFApp;
class CXFA_FFDoc;
class CXFA_FFDocView;
class CXFA_FFWidget;
class CXFA_Node;
class CXFA_TextLayout;
class CXFA_WidgetLayoutData;
class IXFA_AppProvider;

class CXFA_WidgetAcc : public CXFA_WidgetData {
 public:
  CXFA_WidgetAcc(CXFA_FFDocView* pDocView, CXFA_Node* pNode);
  ~CXFA_WidgetAcc();

  FX_BOOL GetName(CFX_WideString& wsName, int32_t iNameType = 0);
  FX_BOOL ProcessValueChanged();
  void ResetData();

  void SetImageEdit(const CFX_WideString& wsContentType,
                    const CFX_WideString& wsHref,
                    const CFX_WideString& wsData);

  CXFA_WidgetAcc* GetExclGroup();
  CXFA_FFDocView* GetDocView();
  CXFA_FFDoc* GetDoc();
  CXFA_FFApp* GetApp();
  IXFA_AppProvider* GetAppProvider();

  int32_t ProcessEvent(int32_t iActivity, CXFA_EventParam* pEventParam);
  int32_t ProcessEvent(CXFA_Event& event, CXFA_EventParam* pEventParam);
  int32_t ProcessCalculate();
  int32_t ProcessValidate(int32_t iFlags = 0);
  int32_t ExecuteScript(CXFA_Script script,
                        CXFA_EventParam* pEventParam,
                        CFXJSE_Value** pRetValue = nullptr);

  CXFA_FFWidget* GetNextWidget(CXFA_FFWidget* pWidget);
  void StartWidgetLayout(FX_FLOAT& fCalcWidth, FX_FLOAT& fCalcHeight);
  FX_BOOL FindSplitPos(int32_t iBlockIndex, FX_FLOAT& fCalcHeight);
  FX_BOOL LoadCaption();
  void LoadText();
  FX_BOOL LoadImageImage();
  FX_BOOL LoadImageEditImage();
  void GetImageDpi(int32_t& iImageXDpi, int32_t& iImageYDpi);
  void GetImageEditDpi(int32_t& iImageXDpi, int32_t& iImageYDpi);
  CXFA_TextLayout* GetCaptionTextLayout();
  CXFA_TextLayout* GetTextLayout();
  CFX_DIBitmap* GetImageImage();
  CFX_DIBitmap* GetImageEditImage();
  void SetImageImage(CFX_DIBitmap* newImage);
  void SetImageEditImage(CFX_DIBitmap* newImage);
  void UpdateUIDisplay(CXFA_FFWidget* pExcept = nullptr);

  CXFA_Node* GetDatasets();
  CFGAS_GEFont* GetFDEFont();
  FX_FLOAT GetFontSize();
  FX_ARGB GetTextColor();
  FX_FLOAT GetLineHeight();
  CXFA_WidgetLayoutData* GetWidgetLayoutData();

 protected:
  void ProcessScriptTestValidate(CXFA_Validate validate,
                                 int32_t iRet,
                                 CFXJSE_Value* pRetValue,
                                 FX_BOOL bVersionFlag);
  int32_t ProcessFormatTestValidate(CXFA_Validate validate,
                                    FX_BOOL bVersionFlag);
  int32_t ProcessNullTestValidate(CXFA_Validate validate,
                                  int32_t iFlags,
                                  FX_BOOL bVersionFlag);
  void GetValidateCaptionName(CFX_WideString& wsCaptionName,
                              FX_BOOL bVersionFlag);
  void GetValidateMessage(IXFA_AppProvider* pAppProvider,
                          CFX_WideString& wsMessage,
                          FX_BOOL bError,
                          FX_BOOL bVersionFlag);
  void CalcCaptionSize(CFX_SizeF& szCap);
  FX_BOOL CalculateFieldAutoSize(CFX_SizeF& size);
  FX_BOOL CalculateWidgetAutoSize(CFX_SizeF& size);
  FX_BOOL CalculateTextEditAutoSize(CFX_SizeF& size);
  FX_BOOL CalculateCheckButtonAutoSize(CFX_SizeF& size);
  FX_BOOL CalculatePushButtonAutoSize(CFX_SizeF& size);
  FX_BOOL CalculateImageEditAutoSize(CFX_SizeF& size);
  FX_BOOL CalculateImageAutoSize(CFX_SizeF& size);
  FX_BOOL CalculateTextAutoSize(CFX_SizeF& size);
  FX_FLOAT CalculateWidgetAutoHeight(FX_FLOAT fHeightCalc);
  FX_FLOAT CalculateWidgetAutoWidth(FX_FLOAT fWidthCalc);
  FX_FLOAT GetWidthWithoutMargin(FX_FLOAT fWidthCalc);
  FX_FLOAT GetHeightWithoutMargin(FX_FLOAT fHeightCalc);
  void CalculateTextContentSize(CFX_SizeF& size);
  void CalculateAccWidthAndHeight(XFA_Element eUIType,
                                  FX_FLOAT& fWidth,
                                  FX_FLOAT& fCalcHeight);
  void InitLayoutData();
  void StartTextLayout(FX_FLOAT& fCalcWidth, FX_FLOAT& fCalcHeight);

  CXFA_FFDocView* m_pDocView;
  std::unique_ptr<CXFA_WidgetLayoutData> m_pLayoutData;
  uint32_t m_nRecursionDepth;
};

#endif  // XFA_FXFA_INCLUDE_FXFA_WIDGET_H_
