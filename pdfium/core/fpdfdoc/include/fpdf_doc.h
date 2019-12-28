// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_INCLUDE_FPDF_DOC_H_
#define CORE_FPDFDOC_INCLUDE_FPDF_DOC_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"
#include "core/fxge/include/fx_dib.h"

class CFDF_Document;
class CFieldTree;
class CFX_RenderDevice;
class CPDF_AAction;
class CPDF_Action;
class CPDF_ActionFields;
class CPDF_Annot;
class CPDF_AnnotList;
class CPDF_ApSettings;
class CPDF_Bookmark;
class CPDF_BookmarkTree;
class CPDF_DefaultAppearance;
class CPDF_Dest;
class CPDF_DocJSActions;
class CPDF_Document;
class CPDF_FileSpec;
class CPDF_Font;
class CPDF_Form;
class CPDF_FormControl;
class CPDF_FormField;
class CPDF_IconFit;
class CPDF_Link;
class CPDF_Metadata;
class CPDF_OCContext;
class CPDF_Page;
class CPDF_PageObject;
class CPDF_RenderContext;
class CPDF_RenderOptions;
class CXML_Element;
class IPDF_FormNotify;

enum class BorderStyle { SOLID, DASH, BEVELED, INSET, UNDERLINE };

class CPDF_NameTree {
 public:
  explicit CPDF_NameTree(CPDF_Dictionary* pRoot) : m_pRoot(pRoot) {}
  CPDF_NameTree(CPDF_Document* pDoc, const CFX_ByteString& category);

  CPDF_Object* LookupValue(int nIndex, CFX_ByteString& csName) const;
  CPDF_Object* LookupValue(const CFX_ByteString& csName) const;
  CPDF_Array* LookupNamedDest(CPDF_Document* pDoc, const CFX_ByteString& sName);
  int GetIndex(const CFX_ByteString& csName) const;
  size_t GetCount() const;
  CPDF_Dictionary* GetRoot() const { return m_pRoot; }

 protected:
  CPDF_Dictionary* m_pRoot;
};

class CPDF_BookmarkTree {
 public:
  explicit CPDF_BookmarkTree(CPDF_Document* pDoc) : m_pDocument(pDoc) {}

  CPDF_Bookmark GetFirstChild(const CPDF_Bookmark& parent) const;
  CPDF_Bookmark GetNextSibling(const CPDF_Bookmark& bookmark) const;
  CPDF_Document* GetDocument() const { return m_pDocument; }

 protected:
  CPDF_Document* const m_pDocument;
};

#define PDFBOOKMARK_ITALIC 1
#define PDFBOOKMARK_BOLD 2
class CPDF_Bookmark {
 public:
  CPDF_Bookmark() : m_pDict(nullptr) {}
  explicit CPDF_Bookmark(CPDF_Dictionary* pDict) : m_pDict(pDict) {}

  CPDF_Dictionary* GetDict() const { return m_pDict; }
  uint32_t GetColorRef() const;
  uint32_t GetFontStyle() const;
  CFX_WideString GetTitle() const;
  CPDF_Dest GetDest(CPDF_Document* pDocument) const;
  CPDF_Action GetAction() const;

 protected:
  CPDF_Dictionary* m_pDict;
};

#define PDFZOOM_XYZ 1
#define PDFZOOM_FITPAGE 2
#define PDFZOOM_FITHORZ 3
#define PDFZOOM_FITVERT 4
#define PDFZOOM_FITRECT 5
#define PDFZOOM_FITBBOX 6
#define PDFZOOM_FITBHORZ 7
#define PDFZOOM_FITBVERT 8
class CPDF_Dest {
 public:
  CPDF_Dest() : m_pObj(nullptr) {}
  explicit CPDF_Dest(CPDF_Object* pObj) : m_pObj(pObj) {}

  CPDF_Object* GetObject() const { return m_pObj; }
  CFX_ByteString GetRemoteName();
  int GetPageIndex(CPDF_Document* pDoc);
  uint32_t GetPageObjNum();
  int GetZoomMode();
  FX_FLOAT GetParam(int index);

 protected:
  CPDF_Object* m_pObj;
};

class CPDF_OCContext {
 public:
  enum UsageType { View = 0, Design, Print, Export };

  CPDF_OCContext(CPDF_Document* pDoc, UsageType eUsageType);
  ~CPDF_OCContext();

  bool CheckOCGVisible(const CPDF_Dictionary* pOCGDict);
  bool CheckObjectVisible(const CPDF_PageObject* pObj);

 protected:
  bool LoadOCGStateFromConfig(const CFX_ByteString& csConfig,
                              const CPDF_Dictionary* pOCGDict) const;
  bool LoadOCGState(const CPDF_Dictionary* pOCGDict) const;
  bool GetOCGVisible(const CPDF_Dictionary* pOCGDict);
  bool GetOCGVE(CPDF_Array* pExpression, int nLevel);
  bool LoadOCMDState(const CPDF_Dictionary* pOCMDDict);

  CPDF_Document* const m_pDocument;
  const UsageType m_eUsageType;
  std::unordered_map<const CPDF_Dictionary*, bool> m_OCGStates;
};

class CPDF_ActionFields {
 public:
  explicit CPDF_ActionFields(const CPDF_Action* pAction) : m_pAction(pAction) {}

  size_t GetFieldsCount() const;
  std::vector<CPDF_Object*> GetAllFields() const;
  CPDF_Object* GetField(size_t iIndex) const;

 protected:
  const CPDF_Action* const m_pAction;
};

#define PDFNAMED_NEXTPAGE 1
#define PDFNAMED_PREVPAGE 2
#define PDFNAMED_FIRSTPAGE 3
#define PDFNAMED_LASTPAGE 4
#define PDFJS_MAXLENGTH 64
class CPDF_Action {
 public:
  enum ActionType {
    Unknown = 0,
    GoTo,
    GoToR,
    GoToE,
    Launch,
    Thread,
    URI,
    Sound,
    Movie,
    Hide,
    Named,
    SubmitForm,
    ResetForm,
    ImportData,
    JavaScript,
    SetOCGState,
    Rendition,
    Trans,
    GoTo3DView
  };

  CPDF_Action() : m_pDict(nullptr) {}
  explicit CPDF_Action(CPDF_Dictionary* pDict) : m_pDict(pDict) {}

  CPDF_Dictionary* GetDict() const { return m_pDict; }
  ActionType GetType() const;
  CPDF_Dest GetDest(CPDF_Document* pDoc) const;
  CFX_WideString GetFilePath() const;
  CFX_ByteString GetURI(CPDF_Document* pDoc) const;
  bool GetHideStatus() const { return m_pDict->GetBooleanBy("H", true); }
  CFX_ByteString GetNamedAction() const { return m_pDict->GetStringBy("N"); }
  uint32_t GetFlags() const { return m_pDict->GetIntegerBy("Flags"); }
  CFX_WideString GetJavaScript() const;
  size_t GetSubActionsCount() const;
  CPDF_Action GetSubAction(size_t iIndex) const;

 protected:
  CPDF_Dictionary* const m_pDict;
};

class CPDF_AAction {
 public:
  enum AActionType {
    CursorEnter = 0,
    CursorExit,
    ButtonDown,
    ButtonUp,
    GetFocus,
    LoseFocus,
    PageOpen,
    PageClose,
    PageVisible,
    PageInvisible,
    OpenPage,
    ClosePage,
    KeyStroke,
    Format,
    Validate,
    Calculate,
    CloseDocument,
    SaveDocument,
    DocumentSaved,
    PrintDocument,
    DocumentPrinted
  };

  CPDF_AAction() : m_pDict(nullptr) {}
  explicit CPDF_AAction(CPDF_Dictionary* pDict) : m_pDict(pDict) {}

  FX_BOOL ActionExist(AActionType eType) const;
  CPDF_Action GetAction(AActionType eType) const;
  CPDF_Dictionary* GetDict() const { return m_pDict; }

 protected:
  CPDF_Dictionary* const m_pDict;
};

class CPDF_DocJSActions {
 public:
  explicit CPDF_DocJSActions(CPDF_Document* pDoc);

  int CountJSActions() const;
  CPDF_Action GetJSAction(int index, CFX_ByteString& csName) const;
  CPDF_Action GetJSAction(const CFX_ByteString& csName) const;
  int FindJSAction(const CFX_ByteString& csName) const;
  CPDF_Document* GetDocument() const { return m_pDocument; }

 protected:
  CPDF_Document* const m_pDocument;
};

class CPDF_FileSpec {
 public:
  CPDF_FileSpec();
  explicit CPDF_FileSpec(CPDF_Object* pObj) : m_pObj(pObj) {}

  // Convert a platform dependent file name into pdf format.
  static CFX_WideString EncodeFileName(const CFX_WideStringC& filepath);

  // Convert a pdf file name into platform dependent format.
  static CFX_WideString DecodeFileName(const CFX_WideStringC& filepath);

  CPDF_Object* GetObj() const { return m_pObj; }
  bool GetFileName(CFX_WideString* wsFileName) const;

  // Set this file spec to refer to a file name (not a url).
  void SetFileName(const CFX_WideStringC& wsFileName);

 protected:
  CPDF_Object* m_pObj;
};

class CPDF_LinkList {
 public:
  CPDF_LinkList();
  ~CPDF_LinkList();

  CPDF_Link GetLinkAtPoint(CPDF_Page* pPage,
                           FX_FLOAT pdf_x,
                           FX_FLOAT pdf_y,
                           int* z_order);

 private:
  const std::vector<CPDF_Dictionary*>* GetPageLinks(CPDF_Page* pPage);
  void LoadPageLinks(CPDF_Page* pPage, std::vector<CPDF_Dictionary*>* pList);

  std::map<uint32_t, std::vector<CPDF_Dictionary*>> m_PageMap;
};

class CPDF_Link {
 public:
  CPDF_Link() : m_pDict(nullptr) {}
  explicit CPDF_Link(CPDF_Dictionary* pDict) : m_pDict(pDict) {}

  CPDF_Dictionary* GetDict() const { return m_pDict; }

  CFX_FloatRect GetRect();
  CPDF_Dest GetDest(CPDF_Document* pDoc);
  CPDF_Action GetAction();

 protected:
  CPDF_Dictionary* m_pDict;
};

#define ANNOTFLAG_INVISIBLE 0x0001
#define ANNOTFLAG_HIDDEN 0x0002
#define ANNOTFLAG_PRINT 0x0004
#define ANNOTFLAG_NOZOOM 0x0008
#define ANNOTFLAG_NOROTATE 0x0010
#define ANNOTFLAG_NOVIEW 0x0020
#define ANNOTFLAG_READONLY 0x0040
#define ANNOTFLAG_LOCKED 0x0080
#define ANNOTFLAG_TOGGLENOVIEW 0x0100

class CPDF_Annot {
 public:
  enum AppearanceMode { Normal, Rollover, Down };

  CPDF_Annot(CPDF_Dictionary* pDict, CPDF_AnnotList* pList);
  ~CPDF_Annot();

  CFX_ByteString GetSubType() const;
  uint32_t GetFlags() const;
  void GetRect(CFX_FloatRect& rect) const;
  const CPDF_Dictionary* GetAnnotDict() const { return m_pAnnotDict; }
  CPDF_Dictionary* GetAnnotDict() { return m_pAnnotDict; }
  FX_BOOL DrawAppearance(CPDF_Page* pPage,
                         CFX_RenderDevice* pDevice,
                         const CFX_Matrix* pUser2Device,
                         AppearanceMode mode,
                         const CPDF_RenderOptions* pOptions);
  FX_BOOL DrawInContext(const CPDF_Page* pPage,
                        CPDF_RenderContext* pContext,
                        const CFX_Matrix* pUser2Device,
                        AppearanceMode mode);
  void ClearCachedAP();
  void DrawBorder(CFX_RenderDevice* pDevice,
                  const CFX_Matrix* pUser2Device,
                  const CPDF_RenderOptions* pOptions);
  CPDF_Form* GetAPForm(const CPDF_Page* pPage, AppearanceMode mode);

 private:
  CPDF_Dictionary* const m_pAnnotDict;
  CPDF_AnnotList* const m_pList;
  const CFX_ByteString m_sSubtype;
  std::map<CPDF_Stream*, CPDF_Form*> m_APMap;
};

class CPDF_AnnotList {
 public:
  explicit CPDF_AnnotList(CPDF_Page* pPage);
  ~CPDF_AnnotList();

  void DisplayAnnots(CPDF_Page* pPage,
                     CPDF_RenderContext* pContext,
                     FX_BOOL bPrinting,
                     CFX_Matrix* pMatrix,
                     FX_BOOL bShowWidget,
                     CPDF_RenderOptions* pOptions) {
    DisplayAnnots(pPage, nullptr, pContext, bPrinting, pMatrix,
                  bShowWidget ? 3 : 1, pOptions, nullptr);
  }

  void DisplayAnnots(CPDF_Page* pPage,
                     CFX_RenderDevice* pDevice,
                     CPDF_RenderContext* pContext,
                     FX_BOOL bPrinting,
                     CFX_Matrix* pMatrix,
                     uint32_t dwAnnotFlags,
                     CPDF_RenderOptions* pOptions,
                     FX_RECT* pClipRect);
  size_t Count() const { return m_AnnotList.size(); }
  CPDF_Annot* GetAt(size_t index) const { return m_AnnotList[index].get(); }
  const std::vector<std::unique_ptr<CPDF_Annot>>& All() const {
    return m_AnnotList;
  }
  CPDF_Document* GetDocument() const { return m_pDocument; }

 protected:
  void DisplayPass(CPDF_Page* pPage,
                   CFX_RenderDevice* pDevice,
                   CPDF_RenderContext* pContext,
                   FX_BOOL bPrinting,
                   CFX_Matrix* pMatrix,
                   FX_BOOL bWidget,
                   CPDF_RenderOptions* pOptions,
                   FX_RECT* clip_rect);

  CPDF_Document* const m_pDocument;
  std::vector<std::unique_ptr<CPDF_Annot>> m_AnnotList;
};

#define COLORTYPE_TRANSPARENT 0
#define COLORTYPE_GRAY 1
#define COLORTYPE_RGB 2
#define COLORTYPE_CMYK 3
class CPDF_DefaultAppearance {
 public:
  CPDF_DefaultAppearance() {}
  explicit CPDF_DefaultAppearance(const CFX_ByteString& csDA) : m_csDA(csDA) {}

  CPDF_DefaultAppearance(const CPDF_DefaultAppearance& cDA) {
    m_csDA = cDA.GetStr();
  }

  CFX_ByteString GetStr() const { return m_csDA; }
  FX_BOOL HasFont();
  CFX_ByteString GetFontString();
  void GetFont(CFX_ByteString& csFontNameTag, FX_FLOAT& fFontSize);
  FX_BOOL HasColor(FX_BOOL bStrokingOperation = FALSE);
  CFX_ByteString GetColorString(FX_BOOL bStrokingOperation = FALSE);
  void GetColor(int& iColorType,
                FX_FLOAT fc[4],
                FX_BOOL bStrokingOperation = FALSE);
  void GetColor(FX_ARGB& color,
                int& iColorType,
                FX_BOOL bStrokingOperation = FALSE);
  FX_BOOL HasTextMatrix();
  CFX_ByteString GetTextMatrixString();
  CFX_Matrix GetTextMatrix();

 protected:
  CFX_ByteString m_csDA;
};

#define FIELDTYPE_UNKNOWN 0
#define FIELDTYPE_PUSHBUTTON 1
#define FIELDTYPE_CHECKBOX 2
#define FIELDTYPE_RADIOBUTTON 3
#define FIELDTYPE_COMBOBOX 4
#define FIELDTYPE_LISTBOX 5
#define FIELDTYPE_TEXTFIELD 6
#define FIELDTYPE_SIGNATURE 7

class CPDF_InterForm {
 public:
  explicit CPDF_InterForm(CPDF_Document* pDocument);
  ~CPDF_InterForm();

  static void SetUpdateAP(FX_BOOL bUpdateAP);
  static FX_BOOL IsUpdateAPEnabled();
  static CFX_ByteString GenerateNewResourceName(const CPDF_Dictionary* pResDict,
                                                const FX_CHAR* csType,
                                                int iMinLen = 2,
                                                const FX_CHAR* csPrefix = "");
  static CPDF_Font* AddStandardFont(CPDF_Document* pDocument,
                                    CFX_ByteString csFontName);
  static CFX_ByteString GetNativeFont(uint8_t iCharSet,
                                      void* pLogFont = nullptr);
  static CFX_ByteString GetNativeFont(void* pLogFont = nullptr);
  static uint8_t GetNativeCharSet();
  static CPDF_Font* AddNativeFont(uint8_t iCharSet, CPDF_Document* pDocument);
  static CPDF_Font* AddNativeFont(CPDF_Document* pDocument);

  FX_BOOL ValidateFieldName(CFX_WideString& csNewFieldName, int iType);
  FX_BOOL ValidateFieldName(const CPDF_FormField* pField,
                            CFX_WideString& csNewFieldName);
  FX_BOOL ValidateFieldName(const CPDF_FormControl* pControl,
                            CFX_WideString& csNewFieldName);

  uint32_t CountFields(const CFX_WideString& csFieldName = L"");
  CPDF_FormField* GetField(uint32_t index,
                           const CFX_WideString& csFieldName = L"");
  CPDF_FormField* GetFieldByDict(CPDF_Dictionary* pFieldDict) const;

  CPDF_FormControl* GetControlAtPoint(CPDF_Page* pPage,
                                      FX_FLOAT pdf_x,
                                      FX_FLOAT pdf_y,
                                      int* z_order) const;
  CPDF_FormControl* GetControlByDict(const CPDF_Dictionary* pWidgetDict) const;

  CPDF_Document* GetDocument() const { return m_pDocument; }
  CPDF_Dictionary* GetFormDict() const { return m_pFormDict; }
  FX_BOOL NeedConstructAP() const;
  int CountFieldsInCalculationOrder();
  CPDF_FormField* GetFieldInCalculationOrder(int index);
  int FindFieldInCalculationOrder(const CPDF_FormField* pField);

  uint32_t CountFormFonts();
  CPDF_Font* GetFormFont(uint32_t index, CFX_ByteString& csNameTag);
  CPDF_Font* GetFormFont(CFX_ByteString csNameTag);
  CPDF_Font* GetFormFont(CFX_ByteString csFontName, CFX_ByteString& csNameTag);
  CPDF_Font* GetNativeFormFont(uint8_t iCharSet, CFX_ByteString& csNameTag);
  CPDF_Font* GetNativeFormFont(CFX_ByteString& csNameTag);
  FX_BOOL FindFormFont(const CPDF_Font* pFont, CFX_ByteString& csNameTag);
  FX_BOOL FindFormFont(CFX_ByteString csFontName,
                       CPDF_Font*& pFont,
                       CFX_ByteString& csNameTag);

  FX_BOOL FindFormFont(CFX_WideString csFontName,
                       CPDF_Font*& pFont,
                       CFX_ByteString& csNameTag) {
    return FindFormFont(PDF_EncodeText(csFontName), pFont, csNameTag);
  }

  void AddFormFont(const CPDF_Font* pFont, CFX_ByteString& csNameTag);
  CPDF_Font* AddNativeFormFont(uint8_t iCharSet, CFX_ByteString& csNameTag);
  CPDF_Font* AddNativeFormFont(CFX_ByteString& csNameTag);

  void RemoveFormFont(const CPDF_Font* pFont);
  void RemoveFormFont(CFX_ByteString csNameTag);

  CPDF_DefaultAppearance GetDefaultAppearance();
  CPDF_Font* GetDefaultFormFont();
  int GetFormAlignment();

  CPDF_FormField* CheckRequiredFields(
      const std::vector<CPDF_FormField*>* fields,
      bool bIncludeOrExclude) const;

  CFDF_Document* ExportToFDF(const CFX_WideStringC& pdf_path,
                             bool bSimpleFileSpec = false) const;
  CFDF_Document* ExportToFDF(const CFX_WideStringC& pdf_path,
                             const std::vector<CPDF_FormField*>& fields,
                             bool bIncludeOrExclude = true,
                             bool bSimpleFileSpec = false) const;
  FX_BOOL ImportFromFDF(const CFDF_Document* pFDFDoc, FX_BOOL bNotify = FALSE);

  bool ResetForm(const std::vector<CPDF_FormField*>& fields,
                 bool bIncludeOrExclude = true,
                 bool bNotify = false);
  bool ResetForm(bool bNotify = false);

  void SetFormNotify(IPDF_FormNotify* pNotify);
  FX_BOOL HasXFAForm() const;
  void FixPageFields(const CPDF_Page* pPage);

 protected:
  static FX_BOOL s_bUpdateAP;

  void LoadField(CPDF_Dictionary* pFieldDict, int nLevel = 0);
  CPDF_Object* GetFieldAttr(CPDF_Dictionary* pFieldDict, const FX_CHAR* name);
  CPDF_FormField* AddTerminalField(CPDF_Dictionary* pFieldDict);
  CPDF_FormControl* AddControl(CPDF_FormField* pField,
                               CPDF_Dictionary* pWidgetDict);
  void FDF_ImportField(CPDF_Dictionary* pField,
                       const CFX_WideString& parent_name,
                       FX_BOOL bNotify = FALSE,
                       int nLevel = 0);
  FX_BOOL ValidateFieldName(CFX_WideString& csNewFieldName,
                            int iType,
                            const CPDF_FormField* pExcludedField,
                            const CPDF_FormControl* pExcludedControl);
  int CompareFieldName(const CFX_WideString& name1,
                       const CFX_WideString& name2);
  int CompareFieldName(const CFX_ByteString& name1,
                       const CFX_ByteString& name2);

  CPDF_Document* const m_pDocument;
  CPDF_Dictionary* m_pFormDict;
  std::map<const CPDF_Dictionary*, CPDF_FormControl*> m_ControlMap;
  std::unique_ptr<CFieldTree> m_pFieldTree;
  CFX_ByteString m_bsEncoding;
  IPDF_FormNotify* m_pFormNotify;

  friend class CPDF_FormControl;
  friend class CPDF_FormField;
};

#define FORMFIELD_READONLY 0x01
#define FORMFIELD_REQUIRED 0x02
#define FORMFIELD_NOEXPORT 0x04
#define FORMRADIO_NOTOGGLEOFF 0x100
#define FORMRADIO_UNISON 0x200
#define FORMTEXT_MULTILINE 0x100
#define FORMTEXT_PASSWORD 0x200
#define FORMTEXT_NOSCROLL 0x400
#define FORMTEXT_COMB 0x800
#define FORMCOMBO_EDIT 0x100
#define FORMLIST_MULTISELECT 0x100

class CPDF_FormField {
 public:
  enum Type {
    Unknown,
    PushButton,
    RadioButton,
    CheckBox,
    Text,
    RichText,
    File,
    ListBox,
    ComboBox,
    Sign
  };

  CFX_WideString GetFullName() const;

  Type GetType() const { return m_Type; }
  uint32_t GetFlags() const { return m_Flags; }

  CPDF_Dictionary* GetFieldDict() const { return m_pDict; }
  void SetFieldDict(CPDF_Dictionary* pDict) { m_pDict = pDict; }

  FX_BOOL ResetField(FX_BOOL bNotify = FALSE);

  int CountControls() const { return m_ControlList.GetSize(); }

  CPDF_FormControl* GetControl(int index) const {
    return m_ControlList.GetAt(index);
  }

  int GetControlIndex(const CPDF_FormControl* pControl) const;
  int GetFieldType() const;

  CPDF_AAction GetAdditionalAction() const;
  CFX_WideString GetAlternateName() const;
  CFX_WideString GetMappingName() const;

  uint32_t GetFieldFlags() const;
  CFX_ByteString GetDefaultStyle() const;
  CFX_WideString GetRichTextString() const;

  CFX_WideString GetValue() const;
  CFX_WideString GetDefaultValue() const;
  FX_BOOL SetValue(const CFX_WideString& value, FX_BOOL bNotify = FALSE);

  int GetMaxLen() const;
  int CountSelectedItems() const;
  int GetSelectedIndex(int index) const;

  FX_BOOL ClearSelection(FX_BOOL bNotify = FALSE);
  FX_BOOL IsItemSelected(int index) const;
  FX_BOOL SetItemSelection(int index,
                           FX_BOOL bSelected,
                           FX_BOOL bNotify = FALSE);

  FX_BOOL IsItemDefaultSelected(int index) const;

  int GetDefaultSelectedItem() const;
  int CountOptions() const;

  CFX_WideString GetOptionLabel(int index) const;
  CFX_WideString GetOptionValue(int index) const;

  int FindOption(CFX_WideString csOptLabel) const;
  int FindOptionValue(const CFX_WideString& csOptValue) const;

  FX_BOOL CheckControl(int iControlIndex, bool bChecked, bool bNotify = false);

  int GetTopVisibleIndex() const;
  int CountSelectedOptions() const;

  int GetSelectedOptionIndex(int index) const;
  FX_BOOL IsOptionSelected(int iOptIndex) const;

  FX_BOOL SelectOption(int iOptIndex,
                       FX_BOOL bSelected,
                       FX_BOOL bNotify = FALSE);

  FX_BOOL ClearSelectedOptions(FX_BOOL bNotify = FALSE);

#ifdef PDF_ENABLE_XFA
  FX_BOOL ClearOptions(FX_BOOL bNotify = FALSE);

  int InsertOption(CFX_WideString csOptLabel,
                   int index = -1,
                   FX_BOOL bNotify = FALSE);
#endif  // PDF_ENABLE_XFA

  FX_FLOAT GetFontSize() const { return m_FontSize; }
  CPDF_Font* GetFont() const { return m_pFont; }

 protected:
  CPDF_FormField(CPDF_InterForm* pForm, CPDF_Dictionary* pDict);
  ~CPDF_FormField();

  CFX_WideString GetValue(FX_BOOL bDefault) const;
  FX_BOOL SetValue(const CFX_WideString& value,
                   FX_BOOL bDefault,
                   FX_BOOL bNotify);

  void SyncFieldFlags();
  int FindListSel(CPDF_String* str);
  CFX_WideString GetOptionText(int index, int sub_index) const;

  void LoadDA();
  CFX_WideString GetCheckValue(FX_BOOL bDefault) const;
  FX_BOOL SetCheckValue(const CFX_WideString& value,
                        FX_BOOL bDefault,
                        FX_BOOL bNotify);

  bool NotifyBeforeSelectionChange(const CFX_WideString& value);
  void NotifyAfterSelectionChange();

  bool NotifyBeforeValueChange(const CFX_WideString& value);
  void NotifyAfterValueChange();

  bool NotifyListOrComboBoxBeforeChange(const CFX_WideString& value);
  void NotifyListOrComboBoxAfterChange();

  CPDF_FormField::Type m_Type;
  uint32_t m_Flags;
  CPDF_InterForm* m_pForm;
  CPDF_Dictionary* m_pDict;
  CFX_ArrayTemplate<CPDF_FormControl*> m_ControlList;
  FX_FLOAT m_FontSize;
  CPDF_Font* m_pFont;

  friend class CPDF_InterForm;
  friend class CPDF_FormControl;
};

CPDF_Object* FPDF_GetFieldAttr(CPDF_Dictionary* pFieldDict,
                               const FX_CHAR* name,
                               int nLevel = 0);
class CPDF_IconFit {
 public:
  enum ScaleMethod { Always = 0, Bigger, Smaller, Never };

  explicit CPDF_IconFit(const CPDF_Dictionary* pDict) : m_pDict(pDict) {}

  ScaleMethod GetScaleMethod();
  FX_BOOL IsProportionalScale();
  void GetIconPosition(FX_FLOAT& fLeft, FX_FLOAT& fBottom);
  bool GetFittingBounds();
  const CPDF_Dictionary* GetDict() const { return m_pDict; }

 protected:
  const CPDF_Dictionary* const m_pDict;
};

#define TEXTPOS_CAPTION 0
#define TEXTPOS_ICON 1
#define TEXTPOS_BELOW 2
#define TEXTPOS_ABOVE 3
#define TEXTPOS_RIGHT 4
#define TEXTPOS_LEFT 5
#define TEXTPOS_OVERLAID 6

class CPDF_FormControl {
 public:
  enum HighlightingMode { None = 0, Invert, Outline, Push, Toggle };

  CPDF_FormField::Type GetType() const { return m_pField->GetType(); }
  CPDF_InterForm* GetInterForm() const { return m_pForm; }
  CPDF_FormField* GetField() const { return m_pField; }
  CPDF_Dictionary* GetWidget() const { return m_pWidgetDict; }
  CFX_FloatRect GetRect() const { return m_pWidgetDict->GetRectBy("Rect"); }

  void DrawControl(CFX_RenderDevice* pDevice,
                   CFX_Matrix* pMatrix,
                   CPDF_Page* pPage,
                   CPDF_Annot::AppearanceMode mode,
                   const CPDF_RenderOptions* pOptions = nullptr);

  CFX_ByteString GetCheckedAPState();
  CFX_WideString GetExportValue() const;

  bool IsChecked() const;
  bool IsDefaultChecked() const;

  HighlightingMode GetHighlightingMode();
  bool HasMKEntry(const CFX_ByteString& csEntry) const;
  int GetRotation();

  FX_ARGB GetBorderColor(int& iColorType) { return GetColor(iColorType, "BC"); }

  FX_FLOAT GetOriginalBorderColor(int index) {
    return GetOriginalColor(index, "BC");
  }

  void GetOriginalBorderColor(int& iColorType, FX_FLOAT fc[4]) {
    GetOriginalColor(iColorType, fc, "BC");
  }

  FX_ARGB GetBackgroundColor(int& iColorType) {
    return GetColor(iColorType, "BG");
  }

  FX_FLOAT GetOriginalBackgroundColor(int index) {
    return GetOriginalColor(index, "BG");
  }

  void GetOriginalBackgroundColor(int& iColorType, FX_FLOAT fc[4]) {
    GetOriginalColor(iColorType, fc, "BG");
  }

  CFX_WideString GetNormalCaption() { return GetCaption("CA"); }
  CFX_WideString GetRolloverCaption() { return GetCaption("RC"); }
  CFX_WideString GetDownCaption() { return GetCaption("AC"); }

  CPDF_Stream* GetNormalIcon() { return GetIcon("I"); }
  CPDF_Stream* GetRolloverIcon() { return GetIcon("RI"); }
  CPDF_Stream* GetDownIcon() { return GetIcon("IX"); }
  CPDF_IconFit GetIconFit();

  int GetTextPosition();
  CPDF_Action GetAction();
  CPDF_AAction GetAdditionalAction();
  CPDF_DefaultAppearance GetDefaultAppearance();

  CPDF_Font* GetDefaultControlFont();
  int GetControlAlignment();

 protected:
  CPDF_FormControl(CPDF_FormField* pField, CPDF_Dictionary* pWidgetDict);

  CFX_ByteString GetOnStateName() const;
  void SetOnStateName(const CFX_ByteString& csOn);
  void CheckControl(FX_BOOL bChecked);
  FX_ARGB GetColor(int& iColorType, const CFX_ByteString& csEntry);
  FX_FLOAT GetOriginalColor(int index, const CFX_ByteString& csEntry);
  void GetOriginalColor(int& iColorType,
                        FX_FLOAT fc[4],
                        const CFX_ByteString& csEntry);

  CFX_WideString GetCaption(const CFX_ByteString& csEntry);
  CPDF_Stream* GetIcon(const CFX_ByteString& csEntry);
  CPDF_ApSettings GetMK() const;

  CPDF_FormField* const m_pField;
  CPDF_Dictionary* const m_pWidgetDict;
  CPDF_InterForm* const m_pForm;

  friend class CPDF_InterForm;
  friend class CPDF_FormField;
};

class IPDF_FormNotify {
 public:
  virtual ~IPDF_FormNotify();

  virtual int BeforeValueChange(CPDF_FormField* pField,
                                const CFX_WideString& csValue);
  virtual void AfterValueChange(CPDF_FormField* pField);
  virtual int BeforeSelectionChange(CPDF_FormField* pField,
                                    const CFX_WideString& csValue);
  virtual void AfterSelectionChange(CPDF_FormField* pField);
  virtual void AfterCheckedStatusChange(CPDF_FormField* pField);
  virtual int BeforeFormReset(CPDF_InterForm* pForm);
  virtual void AfterFormReset(CPDF_InterForm* pForm);
  virtual int BeforeFormImportData(CPDF_InterForm* pForm);
  virtual void AfterFormImportData(CPDF_InterForm* pForm);
};

class CPDF_PageLabel {
 public:
  explicit CPDF_PageLabel(CPDF_Document* pDocument);

  CFX_WideString GetLabel(int nPage) const;
  int32_t GetPageByLabel(const CFX_ByteStringC& bsLabel) const;
  int32_t GetPageByLabel(const CFX_WideStringC& wsLabel) const;

 protected:
  CPDF_Document* const m_pDocument;
};

class CPDF_Metadata {
 public:
  explicit CPDF_Metadata(CPDF_Document* pDoc);
  ~CPDF_Metadata();

  const CXML_Element* GetRoot() const;

 private:
  std::unique_ptr<CXML_Element> m_pXmlElement;
};

class CPDF_ViewerPreferences {
 public:
  explicit CPDF_ViewerPreferences(CPDF_Document* pDoc);
  ~CPDF_ViewerPreferences();

  FX_BOOL IsDirectionR2L() const;
  FX_BOOL PrintScaling() const;
  int32_t NumCopies() const;
  CPDF_Array* PrintPageRange() const;
  CFX_ByteString Duplex() const;

 protected:
  CPDF_Dictionary* GetViewerPreferences() const;

  CPDF_Document* const m_pDoc;
};

class CPDF_ApSettings {
 public:
  explicit CPDF_ApSettings(CPDF_Dictionary* pDict);

  bool HasMKEntry(const CFX_ByteString& csEntry) const;
  int GetRotation() const;

  FX_ARGB GetBorderColor(int& iColorType) const {
    return GetColor(iColorType, "BC");
  }

  FX_FLOAT GetOriginalBorderColor(int index) const {
    return GetOriginalColor(index, "BC");
  }

  void GetOriginalBorderColor(int& iColorType, FX_FLOAT fc[4]) const {
    GetOriginalColor(iColorType, fc, "BC");
  }

  FX_ARGB GetBackgroundColor(int& iColorType) const {
    return GetColor(iColorType, "BG");
  }

  FX_FLOAT GetOriginalBackgroundColor(int index) const {
    return GetOriginalColor(index, "BG");
  }

  void GetOriginalBackgroundColor(int& iColorType, FX_FLOAT fc[4]) const {
    GetOriginalColor(iColorType, fc, "BG");
  }

  CFX_WideString GetNormalCaption() const { return GetCaption("CA"); }
  CFX_WideString GetRolloverCaption() const { return GetCaption("RC"); }
  CFX_WideString GetDownCaption() const { return GetCaption("AC"); }
  CPDF_Stream* GetNormalIcon() const { return GetIcon("I"); }
  CPDF_Stream* GetRolloverIcon() const { return GetIcon("RI"); }
  CPDF_Stream* GetDownIcon() const { return GetIcon("IX"); }
  CPDF_IconFit GetIconFit() const;
  int GetTextPosition() const;

 protected:
  friend class CPDF_FormControl;

  FX_ARGB GetColor(int& iColorType, const CFX_ByteString& csEntry) const;
  FX_FLOAT GetOriginalColor(int index, const CFX_ByteString& csEntry) const;
  void GetOriginalColor(int& iColorType,
                        FX_FLOAT fc[4],
                        const CFX_ByteString& csEntry) const;

  CFX_WideString GetCaption(const CFX_ByteString& csEntry) const;
  CPDF_Stream* GetIcon(const CFX_ByteString& csEntry) const;

  CPDF_Dictionary* const m_pDict;
};

CPDF_Stream* FPDFDOC_GetAnnotAP(CPDF_Dictionary* pAnnotDict,
                                CPDF_Annot::AppearanceMode mode);

#endif  // CORE_FPDFDOC_INCLUDE_FPDF_DOC_H_
