// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_FIELD_H_
#define FPDFSDK_JAVASCRIPT_FIELD_H_

#include <string>
#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/pdfwindow/PWL_Wnd.h"  // For CPWL_Color.

class CPDFSDK_Widget;
class Document;

enum FIELD_PROP {
  FP_ALIGNMENT,
  FP_BORDERSTYLE,
  FP_BUTTONALIGNX,
  FP_BUTTONALIGNY,
  FP_BUTTONFITBOUNDS,
  FP_BUTTONPOSITION,
  FP_BUTTONSCALEHOW,
  FP_BUTTONSCALEWHEN,
  FP_CALCORDERINDEX,
  FP_CHARLIMIT,
  FP_COMB,
  FP_COMMITONSELCHANGE,
  FP_CURRENTVALUEINDICES,
  FP_DEFAULTVALUE,
  FP_DONOTSCROLL,
  FP_DISPLAY,
  FP_FILLCOLOR,
  FP_HIDDEN,
  FP_HIGHLIGHT,
  FP_LINEWIDTH,
  FP_MULTILINE,
  FP_MULTIPLESELECTION,
  FP_PASSWORD,
  FP_RECT,
  FP_RICHTEXT,
  FP_RICHVALUE,
  FP_ROTATION,
  FP_STROKECOLOR,
  FP_STYLE,
  FP_TEXTCOLOR,
  FP_TEXTFONT,
  FP_TEXTSIZE,
  FP_USERNAME,
  FP_VALUE
};

struct CJS_DelayData {
  CJS_DelayData(FIELD_PROP prop, int idx, const CFX_WideString& name);
  ~CJS_DelayData();

  FIELD_PROP eProp;
  int nControlIndex;
  CFX_WideString sFieldName;
  int32_t num;
  bool b;
  CFX_ByteString string;
  CFX_WideString widestring;
  CFX_FloatRect rect;
  CPWL_Color color;
  std::vector<uint32_t> wordarray;
  std::vector<CFX_WideString> widestringarray;
};

class Field : public CJS_EmbedObj {
 public:
  explicit Field(CJS_Object* pJSObject);
  ~Field() override;

  FX_BOOL alignment(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL borderStyle(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError);
  FX_BOOL buttonAlignX(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL buttonAlignY(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL buttonFitBounds(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError);
  FX_BOOL buttonPosition(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError);
  FX_BOOL buttonScaleHow(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError);
  FX_BOOL buttonScaleWhen(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError);
  FX_BOOL calcOrderIndex(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError);
  FX_BOOL charLimit(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL comb(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL commitOnSelChange(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError);
  FX_BOOL currentValueIndices(IJS_Context* cc,
                              CJS_PropValue& vp,
                              CFX_WideString& sError);
  FX_BOOL defaultStyle(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL defaultValue(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL doNotScroll(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError);
  FX_BOOL doNotSpellCheck(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError);
  FX_BOOL delay(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL display(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL doc(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL editable(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL exportValues(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL fileSelect(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL fillColor(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL hidden(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL highlight(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL lineWidth(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL multiline(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL multipleSelection(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError);
  FX_BOOL name(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL numItems(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL page(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL password(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL print(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL radiosInUnison(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError);
  FX_BOOL readonly(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL rect(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL required(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL richText(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL richValue(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL rotation(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL strokeColor(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError);
  FX_BOOL style(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL submitName(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL textColor(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL textFont(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL textSize(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL type(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL userName(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL value(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL valueAsString(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError);
  FX_BOOL source(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);

  FX_BOOL browseForFileToSubmit(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError);
  FX_BOOL buttonGetCaption(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError);
  FX_BOOL buttonGetIcon(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL buttonImportIcon(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError);
  FX_BOOL buttonSetCaption(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError);
  FX_BOOL buttonSetIcon(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL checkThisBox(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL clearItems(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError);
  FX_BOOL defaultIsChecked(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError);
  FX_BOOL deleteItemAt(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL getArray(IJS_Context* cc,
                   const std::vector<CJS_Value>& params,
                   CJS_Value& vRet,
                   CFX_WideString& sError);
  FX_BOOL getItemAt(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError);
  FX_BOOL getLock(IJS_Context* cc,
                  const std::vector<CJS_Value>& params,
                  CJS_Value& vRet,
                  CFX_WideString& sError);
  FX_BOOL insertItemAt(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL isBoxChecked(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL isDefaultChecked(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError);
  FX_BOOL setAction(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError);
  FX_BOOL setFocus(IJS_Context* cc,
                   const std::vector<CJS_Value>& params,
                   CJS_Value& vRet,
                   CFX_WideString& sError);
  FX_BOOL setItems(IJS_Context* cc,
                   const std::vector<CJS_Value>& params,
                   CJS_Value& vRet,
                   CFX_WideString& sError);
  FX_BOOL setLock(IJS_Context* cc,
                  const std::vector<CJS_Value>& params,
                  CJS_Value& vRet,
                  CFX_WideString& sError);
  FX_BOOL signatureGetModifications(IJS_Context* cc,
                                    const std::vector<CJS_Value>& params,
                                    CJS_Value& vRet,
                                    CFX_WideString& sError);
  FX_BOOL signatureGetSeedValue(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError);
  FX_BOOL signatureInfo(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL signatureSetSeedValue(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError);
  FX_BOOL signatureSign(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL signatureValidate(IJS_Context* cc,
                            const std::vector<CJS_Value>& params,
                            CJS_Value& vRet,
                            CFX_WideString& sError);

  static void SetAlignment(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           const CFX_ByteString& string);
  static void SetBorderStyle(CPDFSDK_Document* pDocument,
                             const CFX_WideString& swFieldName,
                             int nControlIndex,
                             const CFX_ByteString& string);
  static void SetButtonAlignX(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex,
                              int number);
  static void SetButtonAlignY(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex,
                              int number);
  static void SetButtonFitBounds(CPDFSDK_Document* pDocument,
                                 const CFX_WideString& swFieldName,
                                 int nControlIndex,
                                 bool b);
  static void SetButtonPosition(CPDFSDK_Document* pDocument,
                                const CFX_WideString& swFieldName,
                                int nControlIndex,
                                int number);
  static void SetButtonScaleHow(CPDFSDK_Document* pDocument,
                                const CFX_WideString& swFieldName,
                                int nControlIndex,
                                int number);
  static void SetButtonScaleWhen(CPDFSDK_Document* pDocument,
                                 const CFX_WideString& swFieldName,
                                 int nControlIndex,
                                 int number);
  static void SetCalcOrderIndex(CPDFSDK_Document* pDocument,
                                const CFX_WideString& swFieldName,
                                int nControlIndex,
                                int number);
  static void SetCharLimit(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           int number);
  static void SetComb(CPDFSDK_Document* pDocument,
                      const CFX_WideString& swFieldName,
                      int nControlIndex,
                      bool b);
  static void SetCommitOnSelChange(CPDFSDK_Document* pDocument,
                                   const CFX_WideString& swFieldName,
                                   int nControlIndex,
                                   bool b);
  static void SetCurrentValueIndices(CPDFSDK_Document* pDocument,
                                     const CFX_WideString& swFieldName,
                                     int nControlIndex,
                                     const std::vector<uint32_t>& array);
  static void SetDefaultStyle(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex);
  static void SetDefaultValue(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex,
                              const CFX_WideString& string);
  static void SetDoNotScroll(CPDFSDK_Document* pDocument,
                             const CFX_WideString& swFieldName,
                             int nControlIndex,
                             bool b);
  static void SetDisplay(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         int number);
  static void SetFillColor(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           const CPWL_Color& color);
  static void SetHidden(CPDFSDK_Document* pDocument,
                        const CFX_WideString& swFieldName,
                        int nControlIndex,
                        bool b);
  static void SetHighlight(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           const CFX_ByteString& string);
  static void SetLineWidth(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           int number);
  static void SetMultiline(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           bool b);
  static void SetMultipleSelection(CPDFSDK_Document* pDocument,
                                   const CFX_WideString& swFieldName,
                                   int nControlIndex,
                                   bool b);
  static void SetPassword(CPDFSDK_Document* pDocument,
                          const CFX_WideString& swFieldName,
                          int nControlIndex,
                          bool b);
  static void SetRect(CPDFSDK_Document* pDocument,
                      const CFX_WideString& swFieldName,
                      int nControlIndex,
                      const CFX_FloatRect& rect);
  static void SetRotation(CPDFSDK_Document* pDocument,
                          const CFX_WideString& swFieldName,
                          int nControlIndex,
                          int number);
  static void SetStrokeColor(CPDFSDK_Document* pDocument,
                             const CFX_WideString& swFieldName,
                             int nControlIndex,
                             const CPWL_Color& color);
  static void SetStyle(CPDFSDK_Document* pDocument,
                       const CFX_WideString& swFieldName,
                       int nControlIndex,
                       const CFX_ByteString& string);
  static void SetTextColor(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           const CPWL_Color& color);
  static void SetTextFont(CPDFSDK_Document* pDocument,
                          const CFX_WideString& swFieldName,
                          int nControlIndex,
                          const CFX_ByteString& string);
  static void SetTextSize(CPDFSDK_Document* pDocument,
                          const CFX_WideString& swFieldName,
                          int nControlIndex,
                          int number);
  static void SetUserName(CPDFSDK_Document* pDocument,
                          const CFX_WideString& swFieldName,
                          int nControlIndex,
                          const CFX_WideString& string);
  static void SetValue(CPDFSDK_Document* pDocument,
                       const CFX_WideString& swFieldName,
                       int nControlIndex,
                       const std::vector<CFX_WideString>& strArray);

  static void AddField(CPDFSDK_Document* pDocument,
                       int nPageIndex,
                       int nFieldType,
                       const CFX_WideString& sName,
                       const CFX_FloatRect& rcCoords);

  static void UpdateFormField(CPDFSDK_Document* pDocument,
                              CPDF_FormField* pFormField,
                              FX_BOOL bChangeMark,
                              FX_BOOL bResetAP,
                              FX_BOOL bRefresh);
  static void UpdateFormControl(CPDFSDK_Document* pDocument,
                                CPDF_FormControl* pFormControl,
                                FX_BOOL bChangeMark,
                                FX_BOOL bResetAP,
                                FX_BOOL bRefresh);

  static CPDFSDK_Widget* GetWidget(CPDFSDK_Document* pDocument,
                                   CPDF_FormControl* pFormControl);
  static std::vector<CPDF_FormField*> GetFormFields(
      CPDFSDK_Document* pDocument,
      const CFX_WideString& csFieldName);

  static void DoDelay(CPDFSDK_Document* pDocument, CJS_DelayData* pData);

  FX_BOOL AttachField(Document* pDocument, const CFX_WideString& csFieldName);
  void SetDelay(FX_BOOL bDelay);
  void SetIsolate(v8::Isolate* isolate) { m_isolate = isolate; }

 protected:
  void ParseFieldName(const std::wstring& strFieldNameParsed,
                      std::wstring& strFieldName,
                      int& iControlNo);
  std::vector<CPDF_FormField*> GetFormFields(
      const CFX_WideString& csFieldName) const;
  CPDF_FormControl* GetSmartFieldControl(CPDF_FormField* pFormField);
  FX_BOOL ValueIsOccur(CPDF_FormField* pFormField, CFX_WideString csOptLabel);

  void AddDelay_Int(FIELD_PROP prop, int32_t n);
  void AddDelay_Bool(FIELD_PROP prop, bool b);
  void AddDelay_String(FIELD_PROP prop, const CFX_ByteString& string);
  void AddDelay_WideString(FIELD_PROP prop, const CFX_WideString& string);
  void AddDelay_Rect(FIELD_PROP prop, const CFX_FloatRect& rect);
  void AddDelay_Color(FIELD_PROP prop, const CPWL_Color& color);
  void AddDelay_WordArray(FIELD_PROP prop, const std::vector<uint32_t>& array);
  void AddDelay_WideStringArray(FIELD_PROP prop,
                                const std::vector<CFX_WideString>& array);

  void DoDelay();

 public:
  Document* m_pJSDoc;
  CPDFSDK_Document* m_pDocument;
  CFX_WideString m_FieldName;
  int m_nFormControlIndex;
  FX_BOOL m_bCanSet;

  FX_BOOL m_bDelay;
  v8::Isolate* m_isolate;
};

class CJS_Field : public CJS_Object {
 public:
  explicit CJS_Field(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_Field() override {}

  void InitInstance(IJS_Runtime* pIRuntime) override;

  DECLARE_JS_CLASS();
  JS_STATIC_PROP(alignment, Field);
  JS_STATIC_PROP(borderStyle, Field);
  JS_STATIC_PROP(buttonAlignX, Field);
  JS_STATIC_PROP(buttonAlignY, Field);
  JS_STATIC_PROP(buttonFitBounds, Field);
  JS_STATIC_PROP(buttonPosition, Field);
  JS_STATIC_PROP(buttonScaleHow, Field);
  JS_STATIC_PROP(buttonScaleWhen, Field);
  JS_STATIC_PROP(calcOrderIndex, Field);
  JS_STATIC_PROP(charLimit, Field);
  JS_STATIC_PROP(comb, Field);
  JS_STATIC_PROP(commitOnSelChange, Field);
  JS_STATIC_PROP(currentValueIndices, Field);
  JS_STATIC_PROP(defaultStyle, Field);
  JS_STATIC_PROP(defaultValue, Field);
  JS_STATIC_PROP(doNotScroll, Field);
  JS_STATIC_PROP(doNotSpellCheck, Field);
  JS_STATIC_PROP(delay, Field);
  JS_STATIC_PROP(display, Field);
  JS_STATIC_PROP(doc, Field);
  JS_STATIC_PROP(editable, Field);
  JS_STATIC_PROP(exportValues, Field);
  JS_STATIC_PROP(fileSelect, Field);
  JS_STATIC_PROP(fillColor, Field);
  JS_STATIC_PROP(hidden, Field);
  JS_STATIC_PROP(highlight, Field);
  JS_STATIC_PROP(lineWidth, Field);
  JS_STATIC_PROP(multiline, Field);
  JS_STATIC_PROP(multipleSelection, Field);
  JS_STATIC_PROP(name, Field);
  JS_STATIC_PROP(numItems, Field);
  JS_STATIC_PROP(page, Field);
  JS_STATIC_PROP(password, Field);
  JS_STATIC_PROP(print, Field);
  JS_STATIC_PROP(radiosInUnison, Field);
  JS_STATIC_PROP(readonly, Field);
  JS_STATIC_PROP(rect, Field);
  JS_STATIC_PROP(required, Field);
  JS_STATIC_PROP(richText, Field);
  JS_STATIC_PROP(richValue, Field);
  JS_STATIC_PROP(rotation, Field);
  JS_STATIC_PROP(strokeColor, Field);
  JS_STATIC_PROP(style, Field);
  JS_STATIC_PROP(submitName, Field);
  JS_STATIC_PROP(textColor, Field);
  JS_STATIC_PROP(textFont, Field);
  JS_STATIC_PROP(textSize, Field);
  JS_STATIC_PROP(type, Field);
  JS_STATIC_PROP(userName, Field);
  JS_STATIC_PROP(value, Field);
  JS_STATIC_PROP(valueAsString, Field);
  JS_STATIC_PROP(source, Field);

  JS_STATIC_METHOD(browseForFileToSubmit, Field);
  JS_STATIC_METHOD(buttonGetCaption, Field);
  JS_STATIC_METHOD(buttonGetIcon, Field);
  JS_STATIC_METHOD(buttonImportIcon, Field);
  JS_STATIC_METHOD(buttonSetCaption, Field);
  JS_STATIC_METHOD(buttonSetIcon, Field);
  JS_STATIC_METHOD(checkThisBox, Field);
  JS_STATIC_METHOD(clearItems, Field);
  JS_STATIC_METHOD(defaultIsChecked, Field);
  JS_STATIC_METHOD(deleteItemAt, Field);
  JS_STATIC_METHOD(getArray, Field);
  JS_STATIC_METHOD(getItemAt, Field);
  JS_STATIC_METHOD(getLock, Field);
  JS_STATIC_METHOD(insertItemAt, Field);
  JS_STATIC_METHOD(isBoxChecked, Field);
  JS_STATIC_METHOD(isDefaultChecked, Field);
  JS_STATIC_METHOD(setAction, Field);
  JS_STATIC_METHOD(setFocus, Field);
  JS_STATIC_METHOD(setItems, Field);
  JS_STATIC_METHOD(setLock, Field);
  JS_STATIC_METHOD(signatureGetModifications, Field);
  JS_STATIC_METHOD(signatureGetSeedValue, Field);
  JS_STATIC_METHOD(signatureInfo, Field);
  JS_STATIC_METHOD(signatureSetSeedValue, Field);
  JS_STATIC_METHOD(signatureSign, Field);
  JS_STATIC_METHOD(signatureValidate, Field);
};

#endif  // FPDFSDK_JAVASCRIPT_FIELD_H_
