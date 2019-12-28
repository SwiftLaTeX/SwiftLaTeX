// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_INCLUDE_FSDK_BASEFORM_H_
#define FPDFSDK_INCLUDE_FSDK_BASEFORM_H_

#include <map>
#include <vector>

#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxge/include/fx_dib.h"
#include "fpdfsdk/include/fsdk_baseannot.h"

#if _FX_OS_ == _FX_ANDROID_
#include "time.h"
#else
#include <ctime>
#endif

class CPDFSDK_Annot;
class CPDFSDK_Document;
class CPDFSDK_InterForm;
class CPDFSDK_PageView;
class CPDF_Action;
class CPDF_FormField;
struct CPWL_Color;

#ifdef PDF_ENABLE_XFA
class CXFA_FFWidgetHandler;

typedef enum _PDFSDK_XFAAActionType {
  PDFSDK_XFA_Click = 0,
  PDFSDK_XFA_Full,
  PDFSDK_XFA_PreOpen,
  PDFSDK_XFA_PostOpen
} PDFSDK_XFAAActionType;
#endif  // PDF_ENABLE_XFA

struct PDFSDK_FieldAction {
  PDFSDK_FieldAction();
  PDFSDK_FieldAction(const PDFSDK_FieldAction& other) = delete;

  FX_BOOL bModifier;         // in
  FX_BOOL bShift;            // in
  int nCommitKey;            // in
  CFX_WideString sChange;    // in[out]
  CFX_WideString sChangeEx;  // in
  FX_BOOL bKeyDown;          // in
  int nSelEnd;               // in[out]
  int nSelStart;             // in[out]
  CFX_WideString sValue;     // in[out]
  FX_BOOL bWillCommit;       // in
  FX_BOOL bFieldFull;        // in
  FX_BOOL bRC;               // in[out]
};

class CPDFSDK_Widget : public CPDFSDK_BAAnnot {
 public:
#ifdef PDF_ENABLE_XFA
  CXFA_FFWidget* GetMixXFAWidget() const;
  CXFA_FFWidget* GetGroupMixXFAWidget();
  CXFA_FFWidgetHandler* GetXFAWidgetHandler() const;

  FX_BOOL HasXFAAAction(PDFSDK_XFAAActionType eXFAAAT);
  FX_BOOL OnXFAAAction(PDFSDK_XFAAActionType eXFAAAT,
                       PDFSDK_FieldAction& data,
                       CPDFSDK_PageView* pPageView);

  void Synchronize(FX_BOOL bSynchronizeElse);
  void SynchronizeXFAValue();
  void SynchronizeXFAItems();

  static void SynchronizeXFAValue(CXFA_FFDocView* pXFADocView,
                                  CXFA_FFWidget* hWidget,
                                  CPDF_FormField* pFormField,
                                  CPDF_FormControl* pFormControl);
  static void SynchronizeXFAItems(CXFA_FFDocView* pXFADocView,
                                  CXFA_FFWidget* hWidget,
                                  CPDF_FormField* pFormField,
                                  CPDF_FormControl* pFormControl);
#endif  // PDF_ENABLE_XFA

  CPDFSDK_Widget(CPDF_Annot* pAnnot,
                 CPDFSDK_PageView* pPageView,
                 CPDFSDK_InterForm* pInterForm);
  ~CPDFSDK_Widget() override;

  // CPDFSDK_Annot
  CFX_ByteString GetSubType() const override;
  CPDF_Action GetAAction(CPDF_AAction::AActionType eAAT) override;
  FX_BOOL IsAppearanceValid() override;

  int GetLayoutOrder() const override;

  int GetFieldType() const;

  // Possible values from PDF 32000-1:2008, table 221.
  // FIELDFLAG_READONLY
  // FIELDFLAG_REQUIRED
  // FIELDFLAG_NOEXPORT
  int GetFieldFlags() const;
  int GetRotate() const;

  FX_BOOL GetFillColor(FX_COLORREF& color) const;
  FX_BOOL GetBorderColor(FX_COLORREF& color) const;
  FX_BOOL GetTextColor(FX_COLORREF& color) const;
  FX_FLOAT GetFontSize() const;

  int GetSelectedIndex(int nIndex) const;
#ifndef PDF_ENABLE_XFA
  CFX_WideString GetValue() const;
#else
  CFX_WideString GetValue(FX_BOOL bDisplay = TRUE) const;
#endif  // PDF_ENABLE_XFA
  CFX_WideString GetDefaultValue() const;
  CFX_WideString GetOptionLabel(int nIndex) const;
  int CountOptions() const;
  FX_BOOL IsOptionSelected(int nIndex) const;
  int GetTopVisibleIndex() const;
  bool IsChecked() const;
  /*
  BF_ALIGN_LEFT
  BF_ALIGN_MIDDL
  BF_ALIGN_RIGHT
  */
  int GetAlignment() const;
  int GetMaxLen() const;
#ifdef PDF_ENABLE_XFA
  CFX_WideString GetName() const;
#endif  // PDF_ENABLE_XFA
  CFX_WideString GetAlternateName() const;

  // Set Properties.
  void SetCheck(bool bChecked, bool bNotify);
  void SetValue(const CFX_WideString& sValue, FX_BOOL bNotify);
  void SetDefaultValue(const CFX_WideString& sValue);
  void SetOptionSelection(int index, FX_BOOL bSelected, FX_BOOL bNotify);
  void ClearSelection(FX_BOOL bNotify);
  void SetTopVisibleIndex(int index);

#ifdef PDF_ENABLE_XFA
  void ResetAppearance(FX_BOOL bValueChanged);
#endif  // PDF_ENABLE_XFA
  void ResetAppearance(const FX_WCHAR* sValue, FX_BOOL bValueChanged);
  void ResetFieldAppearance(FX_BOOL bValueChanged);
  void UpdateField();
  CFX_WideString OnFormat(FX_BOOL& bFormated);

  // Message.
  FX_BOOL OnAAction(CPDF_AAction::AActionType type,
                    PDFSDK_FieldAction& data,
                    CPDFSDK_PageView* pPageView);

  CPDFSDK_InterForm* GetInterForm() const { return m_pInterForm; }
  CPDF_FormField* GetFormField() const;
  CPDF_FormControl* GetFormControl() const;
  static CPDF_FormControl* GetFormControl(CPDF_InterForm* pInterForm,
                                          const CPDF_Dictionary* pAnnotDict);

  void DrawShadow(CFX_RenderDevice* pDevice, CPDFSDK_PageView* pPageView);

  void SetAppModified();
  void ClearAppModified();
  FX_BOOL IsAppModified() const;

  int32_t GetAppearanceAge() const;
  int32_t GetValueAge() const;

 private:
  void ResetAppearance_PushButton();
  void ResetAppearance_CheckBox();
  void ResetAppearance_RadioButton();
  void ResetAppearance_ComboBox(const FX_WCHAR* sValue);
  void ResetAppearance_ListBox();
  void ResetAppearance_TextField(const FX_WCHAR* sValue);

  CFX_FloatRect GetClientRect() const;
  CFX_FloatRect GetRotatedRect() const;

  CFX_ByteString GetBackgroundAppStream() const;
  CFX_ByteString GetBorderAppStream() const;
  CFX_Matrix GetMatrix() const;

  CPWL_Color GetTextPWLColor() const;
  CPWL_Color GetBorderPWLColor() const;
  CPWL_Color GetFillPWLColor() const;

  void AddImageToAppearance(const CFX_ByteString& sAPType, CPDF_Stream* pImage);
  void RemoveAppearance(const CFX_ByteString& sAPType);

 public:
  FX_BOOL IsWidgetAppearanceValid(CPDF_Annot::AppearanceMode mode);
  void DrawAppearance(CFX_RenderDevice* pDevice,
                      const CFX_Matrix* pUser2Device,
                      CPDF_Annot::AppearanceMode mode,
                      const CPDF_RenderOptions* pOptions) override;

  FX_BOOL HitTest(FX_FLOAT pageX, FX_FLOAT pageY);

 private:
  CPDFSDK_InterForm* const m_pInterForm;
  FX_BOOL m_bAppModified;
  int32_t m_nAppAge;
  int32_t m_nValueAge;

#ifdef PDF_ENABLE_XFA
  mutable CXFA_FFWidget* m_hMixXFAWidget;
  mutable CXFA_FFWidgetHandler* m_pWidgetHandler;
#endif  // PDF_ENABLE_XFA
};

#ifdef PDF_ENABLE_XFA
class CPDFSDK_XFAWidget : public CPDFSDK_Annot {
 public:
  CPDFSDK_XFAWidget(CXFA_FFWidget* pAnnot,
                    CPDFSDK_PageView* pPageView,
                    CPDFSDK_InterForm* pInterForm);
  ~CPDFSDK_XFAWidget() override {}

  FX_BOOL IsXFAField() override;
  CXFA_FFWidget* GetXFAWidget() const override;
  CFX_ByteString GetType() const override;
  CFX_ByteString GetSubType() const override;
  CFX_FloatRect GetRect() const override;

  CPDFSDK_InterForm* GetInterForm() { return m_pInterForm; }

 private:
  CPDFSDK_InterForm* m_pInterForm;
  CXFA_FFWidget* m_hXFAWidget;
};
#endif  // PDF_ENABLE_XFA

class CPDFSDK_InterForm : public IPDF_FormNotify {
 public:
  explicit CPDFSDK_InterForm(CPDFSDK_Document* pDocument);
  ~CPDFSDK_InterForm() override;

  CPDF_InterForm* GetInterForm() const { return m_pInterForm.get(); }
  CPDFSDK_Document* GetDocument() const { return m_pDocument; }

  FX_BOOL HighlightWidgets();

  CPDFSDK_Widget* GetSibling(CPDFSDK_Widget* pWidget, FX_BOOL bNext) const;
  CPDFSDK_Widget* GetWidget(CPDF_FormControl* pControl) const;
  void GetWidgets(const CFX_WideString& sFieldName,
                  std::vector<CPDFSDK_Widget*>* widgets) const;
  void GetWidgets(CPDF_FormField* pField,
                  std::vector<CPDFSDK_Widget*>* widgets) const;

  void AddMap(CPDF_FormControl* pControl, CPDFSDK_Widget* pWidget);
  void RemoveMap(CPDF_FormControl* pControl);

  void EnableCalculate(FX_BOOL bEnabled);
  FX_BOOL IsCalculateEnabled() const;

#ifdef PDF_ENABLE_XFA
  void AddXFAMap(CXFA_FFWidget* hWidget, CPDFSDK_XFAWidget* pWidget);
  void RemoveXFAMap(CXFA_FFWidget* hWidget);
  CPDFSDK_XFAWidget* GetXFAWidget(CXFA_FFWidget* hWidget);
  void XfaEnableCalculate(FX_BOOL bEnabled);
  FX_BOOL IsXfaCalculateEnabled() const;
  FX_BOOL IsXfaValidationsEnabled();
  void XfaSetValidationsEnabled(FX_BOOL bEnabled);
#endif  // PDF_ENABLE_XFA

  FX_BOOL OnKeyStrokeCommit(CPDF_FormField* pFormField,
                            const CFX_WideString& csValue);
  FX_BOOL OnValidate(CPDF_FormField* pFormField, const CFX_WideString& csValue);
  void OnCalculate(CPDF_FormField* pFormField = nullptr);
  CFX_WideString OnFormat(CPDF_FormField* pFormField, FX_BOOL& bFormated);

  void ResetFieldAppearance(CPDF_FormField* pFormField,
                            const FX_WCHAR* sValue,
                            FX_BOOL bValueChanged);
  void UpdateField(CPDF_FormField* pFormField);

  FX_BOOL DoAction_Hide(const CPDF_Action& action);
  FX_BOOL DoAction_SubmitForm(const CPDF_Action& action);
  FX_BOOL DoAction_ResetForm(const CPDF_Action& action);
  FX_BOOL DoAction_ImportData(const CPDF_Action& action);

  std::vector<CPDF_FormField*> GetFieldFromObjects(
      const std::vector<CPDF_Object*>& objects) const;
  FX_BOOL IsValidField(CPDF_Dictionary* pFieldDict);
  FX_BOOL SubmitFields(const CFX_WideString& csDestination,
                       const std::vector<CPDF_FormField*>& fields,
                       bool bIncludeOrExclude,
                       bool bUrlEncoded);
  FX_BOOL SubmitForm(const CFX_WideString& sDestination, FX_BOOL bUrlEncoded);
  FX_BOOL ExportFormToFDFTextBuf(CFX_ByteTextBuf& textBuf);
  FX_BOOL ExportFieldsToFDFTextBuf(const std::vector<CPDF_FormField*>& fields,
                                   bool bIncludeOrExclude,
                                   CFX_ByteTextBuf& textBuf);
  CFX_WideString GetTemporaryFileName(const CFX_WideString& sFileExt);

#ifdef PDF_ENABLE_XFA
  void SynchronizeField(CPDF_FormField* pFormField, FX_BOOL bSynchronizeElse);
#endif  // PDF_ENABLE_XFA

 private:
  // IPDF_FormNotify:
  int BeforeValueChange(CPDF_FormField* pField,
                        const CFX_WideString& csValue) override;
  void AfterValueChange(CPDF_FormField* pField) override;
  int BeforeSelectionChange(CPDF_FormField* pField,
                            const CFX_WideString& csValue) override;
  void AfterSelectionChange(CPDF_FormField* pField) override;
  void AfterCheckedStatusChange(CPDF_FormField* pField) override;
  int BeforeFormReset(CPDF_InterForm* pForm) override;
  void AfterFormReset(CPDF_InterForm* pForm) override;
  int BeforeFormImportData(CPDF_InterForm* pForm) override;
  void AfterFormImportData(CPDF_InterForm* pForm) override;

  FX_BOOL FDFToURLEncodedData(CFX_WideString csFDFFile,
                              CFX_WideString csTxtFile);
  FX_BOOL FDFToURLEncodedData(uint8_t*& pBuf, FX_STRSIZE& nBufSize);
  int GetPageIndexByAnnotDict(CPDF_Document* pDocument,
                              CPDF_Dictionary* pAnnotDict) const;

  using CPDFSDK_WidgetMap = std::map<CPDF_FormControl*, CPDFSDK_Widget*>;

  CPDFSDK_Document* m_pDocument;
  std::unique_ptr<CPDF_InterForm> m_pInterForm;
  CPDFSDK_WidgetMap m_Map;
#ifdef PDF_ENABLE_XFA
  std::map<CXFA_FFWidget*, CPDFSDK_XFAWidget*> m_XFAMap;
  FX_BOOL m_bXfaCalculate;
  FX_BOOL m_bXfaValidationsEnabled;
#endif  // PDF_ENABLE_XFA
  FX_BOOL m_bCalculate;
  FX_BOOL m_bBusy;

 public:
  FX_BOOL IsNeedHighLight(int nFieldType);
  void RemoveAllHighLight();
  void SetHighlightAlpha(uint8_t alpha) { m_iHighlightAlpha = alpha; }
  uint8_t GetHighlightAlpha() { return m_iHighlightAlpha; }
  void SetHighlightColor(FX_COLORREF clr, int nFieldType);
  FX_COLORREF GetHighlightColor(int nFieldType);

 private:
#ifndef PDF_ENABLE_XFA
  static const int kNumFieldTypes = 6;
#else   // PDF_ENABLE_XFA
  static const int kNumFieldTypes = 7;
#endif  // PDF_ENABLE_XFA

  FX_COLORREF m_aHighlightColor[kNumFieldTypes];
  uint8_t m_iHighlightAlpha;
  FX_BOOL m_bNeedHightlight[kNumFieldTypes];
};

class CBA_AnnotIterator {
 public:
  enum TabOrder { STRUCTURE = 0, ROW, COLUMN };

  CBA_AnnotIterator(CPDFSDK_PageView* pPageView,
                    const CFX_ByteString& sType,
                    const CFX_ByteString& sSubType);
  ~CBA_AnnotIterator();

  CPDFSDK_Annot* GetFirstAnnot();
  CPDFSDK_Annot* GetLastAnnot();
  CPDFSDK_Annot* GetNextAnnot(CPDFSDK_Annot* pAnnot);
  CPDFSDK_Annot* GetPrevAnnot(CPDFSDK_Annot* pAnnot);

 private:
  void GenerateResults();
  static CFX_FloatRect GetAnnotRect(const CPDFSDK_Annot* pAnnot);

  // Function signature compatible with std::sort().
  static bool CompareByLeftAscending(const CPDFSDK_Annot* p1,
                                     const CPDFSDK_Annot* p2);
  static bool CompareByTopDescending(const CPDFSDK_Annot* p1,
                                     const CPDFSDK_Annot* p2);

  TabOrder m_eTabOrder;
  CPDFSDK_PageView* m_pPageView;
  CFX_ByteString m_sType;
  CFX_ByteString m_sSubType;
  std::vector<CPDFSDK_Annot*> m_Annots;
};

#endif  // FPDFSDK_INCLUDE_FSDK_BASEFORM_H_
