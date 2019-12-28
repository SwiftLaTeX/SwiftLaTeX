// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_INCLUDE_FSDK_BASEANNOT_H_
#define FPDFSDK_INCLUDE_FSDK_BASEANNOT_H_

#if _FX_OS_ == _FX_ANDROID_
#include "time.h"
#else
#include <ctime>
#endif

#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fxcrt/include/fx_basic.h"
#include "fpdfsdk/cfx_systemhandler.h"
#include "fpdfsdk/include/fsdk_common.h"
#include "fpdfsdk/include/fsdk_define.h"

class CPDFSDK_PageView;
class CPDF_Annot;
class CPDF_Page;
class CFX_Matrix;
class CPDF_RenderOptions;
class CFX_RenderDevice;

class CPDFSDK_DateTime {
 public:
  CPDFSDK_DateTime();
  explicit CPDFSDK_DateTime(const CFX_ByteString& dtStr);
  explicit CPDFSDK_DateTime(const FX_SYSTEMTIME& st);
  CPDFSDK_DateTime(const CPDFSDK_DateTime& datetime);

  CPDFSDK_DateTime& operator=(const CPDFSDK_DateTime& datetime);
  CPDFSDK_DateTime& operator=(const FX_SYSTEMTIME& st);
  bool operator==(const CPDFSDK_DateTime& datetime) const;
  bool operator!=(const CPDFSDK_DateTime& datetime) const;

  CPDFSDK_DateTime& FromPDFDateTimeString(const CFX_ByteString& dtStr);
  CFX_ByteString ToCommonDateTimeString();
  CFX_ByteString ToPDFDateTimeString();
  void ToSystemTime(FX_SYSTEMTIME& st);
  time_t ToTime_t() const;
  CPDFSDK_DateTime ToGMT() const;
  CPDFSDK_DateTime& AddDays(short days);
  CPDFSDK_DateTime& AddSeconds(int seconds);

  void ResetDateTime();

  struct FX_DATETIME {
    int16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    int8_t tzHour;
    uint8_t tzMinute;
  } dt;
};

class CPDFSDK_Annot {
 public:
  explicit CPDFSDK_Annot(CPDFSDK_PageView* pPageView);
  virtual ~CPDFSDK_Annot();

#ifdef PDF_ENABLE_XFA
  virtual FX_BOOL IsXFAField();
  virtual CXFA_FFWidget* GetXFAWidget() const;
#endif  // PDF_ENABLE_XFA

  virtual FX_FLOAT GetMinWidth() const;
  virtual FX_FLOAT GetMinHeight() const;
  // define layout order to 5.
  virtual int GetLayoutOrder() const;
  virtual CPDF_Annot* GetPDFAnnot() const;
  virtual CFX_ByteString GetType() const;
  virtual CFX_ByteString GetSubType() const;
  virtual CFX_FloatRect GetRect() const;

  virtual void SetRect(const CFX_FloatRect& rect);
  virtual void Annot_OnDraw(CFX_RenderDevice* pDevice,
                            CFX_Matrix* pUser2Device,
                            CPDF_RenderOptions* pOptions);

  UnderlyingPageType* GetUnderlyingPage();
  CPDF_Page* GetPDFPage();
#ifdef PDF_ENABLE_XFA
  CPDFXFA_Page* GetPDFXFAPage();
#endif  // PDF_ENABLE_XFA

  void SetPage(CPDFSDK_PageView* pPageView);
  CPDFSDK_PageView* GetPageView() const { return m_pPageView; }

  // Tab Order
  int GetTabOrder();
  void SetTabOrder(int iTabOrder);

  // Selection
  FX_BOOL IsSelected();
  void SetSelected(FX_BOOL bSelected);

 protected:
  CPDFSDK_PageView* m_pPageView;
  FX_BOOL m_bSelected;
  int m_nTabOrder;
};

class CPDFSDK_BAAnnot : public CPDFSDK_Annot {
 public:
  CPDFSDK_BAAnnot(CPDF_Annot* pAnnot, CPDFSDK_PageView* pPageView);
  ~CPDFSDK_BAAnnot() override;

  // CPDFSDK_Annot
  CFX_ByteString GetType() const override;
  CFX_ByteString GetSubType() const override;
  void SetRect(const CFX_FloatRect& rect) override;
  CFX_FloatRect GetRect() const override;
  CPDF_Annot* GetPDFAnnot() const override;
  void Annot_OnDraw(CFX_RenderDevice* pDevice,
                    CFX_Matrix* pUser2Device,
                    CPDF_RenderOptions* pOptions) override;

  CPDF_Dictionary* GetAnnotDict() const;

  void SetContents(const CFX_WideString& sContents);
  CFX_WideString GetContents() const;

  void SetAnnotName(const CFX_WideString& sName);
  CFX_WideString GetAnnotName() const;

  void SetModifiedDate(const FX_SYSTEMTIME& st);
  FX_SYSTEMTIME GetModifiedDate() const;

  void SetFlags(uint32_t nFlags);
  uint32_t GetFlags() const;

  void SetAppState(const CFX_ByteString& str);
  CFX_ByteString GetAppState() const;

  void SetStructParent(int key);
  int GetStructParent() const;

  void SetBorderWidth(int nWidth);
  int GetBorderWidth() const;

  void SetBorderStyle(BorderStyle nStyle);
  BorderStyle GetBorderStyle() const;

  void SetColor(FX_COLORREF color);
  void RemoveColor();
  FX_BOOL GetColor(FX_COLORREF& color) const;

  FX_BOOL IsVisible() const;

  CPDF_Action GetAction() const;
  void SetAction(const CPDF_Action& a);
  void RemoveAction();

  CPDF_AAction GetAAction() const;
  void SetAAction(const CPDF_AAction& aa);
  void RemoveAAction();

  virtual CPDF_Action GetAAction(CPDF_AAction::AActionType eAAT);
  virtual FX_BOOL IsAppearanceValid();
  virtual FX_BOOL IsAppearanceValid(CPDF_Annot::AppearanceMode mode);
  virtual void DrawAppearance(CFX_RenderDevice* pDevice,
                              const CFX_Matrix* pUser2Device,
                              CPDF_Annot::AppearanceMode mode,
                              const CPDF_RenderOptions* pOptions);

  void DrawBorder(CFX_RenderDevice* pDevice,
                  const CFX_Matrix* pUser2Device,
                  const CPDF_RenderOptions* pOptions);

  void ClearCachedAP();

  void WriteAppearance(const CFX_ByteString& sAPType,
                       const CFX_FloatRect& rcBBox,
                       const CFX_Matrix& matrix,
                       const CFX_ByteString& sContents,
                       const CFX_ByteString& sAPState = "");

 protected:
  CPDF_Annot* m_pAnnot;

 private:
  FX_BOOL CreateFormFiller();
};

#endif  // FPDFSDK_INCLUDE_FSDK_BASEANNOT_H_
