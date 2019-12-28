// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_TEXTLAYOUT_H_
#define XFA_FXFA_APP_XFA_TEXTLAYOUT_H_

#include <map>
#include <memory>

#include "xfa/fde/css/fde_css.h"
#include "xfa/fde/fde_gedevice.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fgas/layout/fgas_rtfbreak.h"
#include "xfa/fxfa/parser/xfa_object.h"

#define XFA_LOADERCNTXTFLG_FILTERSPACE 0x001

class CFDE_CSSStyleSelector;
class CXFA_Para;
class CXFA_Font;
class CXFA_TextProvider;
class CXFA_TextTabstopsContext;

class CXFA_CSSTagProvider {
 public:
  using AttributeMap = std::map<CFX_WideString, CFX_WideString>;

  CXFA_CSSTagProvider();
  ~CXFA_CSSTagProvider();

  CFX_WideString GetTagName() { return m_wsTagName; }

  AttributeMap::iterator begin() { return m_Attributes.begin(); }
  AttributeMap::iterator end() { return m_Attributes.end(); }

  bool empty() const { return m_Attributes.empty(); }

  void SetTagNameObj(const CFX_WideString& wsName) { m_wsTagName = wsName; }
  void SetAttribute(const CFX_WideString& wsAttr,
                    const CFX_WideString& wsValue) {
    m_Attributes.insert({wsAttr, wsValue});
  }

  FX_BOOL m_bTagAvailable;
  FX_BOOL m_bContent;

 protected:
  CFX_WideString m_wsTagName;
  AttributeMap m_Attributes;
};

class CXFA_TextParseContext : public CFX_Target {
 public:
  CXFA_TextParseContext();
  ~CXFA_TextParseContext() override;

  void SetDisplay(FDE_CSSDISPLAY eDisplay) { m_eDisplay = eDisplay; }
  FDE_CSSDISPLAY GetDisplay() const { return m_eDisplay; }
  void SetDecls(const CFDE_CSSDeclaration** ppDeclArray, int32_t iDeclCount);
  const CFDE_CSSDeclaration** GetDecls() {
    return const_cast<const CFDE_CSSDeclaration**>(m_ppMatchedDecls);
  }
  uint32_t CountDecls() const { return m_dwMatchedDecls; }

  IFDE_CSSComputedStyle* m_pParentStyle;

 protected:
  CFDE_CSSDeclaration** m_ppMatchedDecls;
  uint32_t m_dwMatchedDecls;
  FDE_CSSDISPLAY m_eDisplay;
};

class CXFA_TextParser {
 public:
  CXFA_TextParser();
  virtual ~CXFA_TextParser();

  void Reset();
  void DoParse(CFDE_XMLNode* pXMLContainer, CXFA_TextProvider* pTextProvider);
  IFDE_CSSComputedStyle* CreateRootStyle(CXFA_TextProvider* pTextProvider);
  IFDE_CSSComputedStyle* ComputeStyle(CFDE_XMLNode* pXMLNode,
                                      IFDE_CSSComputedStyle* pParentStyle);
  FX_BOOL IsParsed() const { return !!m_pAllocator; }

  int32_t GetVAlign(CXFA_TextProvider* pTextProvider) const;
  FX_FLOAT GetTabInterval(IFDE_CSSComputedStyle* pStyle) const;
  int32_t CountTabs(IFDE_CSSComputedStyle* pStyle) const;
  FX_BOOL IsSpaceRun(IFDE_CSSComputedStyle* pStyle) const;
  FX_BOOL GetTabstops(IFDE_CSSComputedStyle* pStyle,
                      CXFA_TextTabstopsContext* pTabstopContext);
  CFGAS_GEFont* GetFont(CXFA_TextProvider* pTextProvider,
                        IFDE_CSSComputedStyle* pStyle) const;
  FX_FLOAT GetFontSize(CXFA_TextProvider* pTextProvider,
                       IFDE_CSSComputedStyle* pStyle) const;
  int32_t GetHorScale(CXFA_TextProvider* pTextProvider,
                      IFDE_CSSComputedStyle* pStyle,
                      CFDE_XMLNode* pXMLNode) const;
  int32_t GetVerScale(CXFA_TextProvider* pTextProvider,
                      IFDE_CSSComputedStyle* pStyle) const;
  void GetUnderline(CXFA_TextProvider* pTextProvider,
                    IFDE_CSSComputedStyle* pStyle,
                    int32_t& iUnderline,
                    int32_t& iPeriod) const;
  void GetLinethrough(CXFA_TextProvider* pTextProvider,
                      IFDE_CSSComputedStyle* pStyle,
                      int32_t& iLinethrough) const;
  FX_ARGB GetColor(CXFA_TextProvider* pTextProvider,
                   IFDE_CSSComputedStyle* pStyle) const;
  FX_FLOAT GetBaseline(CXFA_TextProvider* pTextProvider,
                       IFDE_CSSComputedStyle* pStyle) const;
  FX_FLOAT GetLineHeight(CXFA_TextProvider* pTextProvider,
                         IFDE_CSSComputedStyle* pStyle,
                         FX_BOOL bFirst,
                         FX_FLOAT fVerScale) const;
  FX_BOOL GetEmbbedObj(CXFA_TextProvider* pTextProvider,
                       CFDE_XMLNode* pXMLNode,
                       CFX_WideString& wsValue);
  CXFA_TextParseContext* GetParseContextFromMap(CFDE_XMLNode* pXMLNode);

 protected:
  bool TagValidate(const CFX_WideString& str) const;

 private:
  void InitCSSData(CXFA_TextProvider* pTextProvider);
  void ParseRichText(CFDE_XMLNode* pXMLNode,
                     IFDE_CSSComputedStyle* pParentStyle);
  void ParseTagInfo(CFDE_XMLNode* pXMLNode, CXFA_CSSTagProvider& tagProvider);
  IFDE_CSSStyleSheet* LoadDefaultSheetStyle();
  IFDE_CSSComputedStyle* CreateStyle(IFDE_CSSComputedStyle* pParentStyle);
  std::unique_ptr<IFX_MemoryAllocator> m_pAllocator;
  std::unique_ptr<CFDE_CSSStyleSelector> m_pSelector;
  IFDE_CSSStyleSheet* m_pUASheet;
  CFX_MapPtrTemplate<CFDE_XMLNode*, CXFA_TextParseContext*>
      m_mapXMLNodeToParseContext;
};

class CXFA_LoaderContext {
 public:
  CXFA_LoaderContext();
  ~CXFA_LoaderContext();

  FX_BOOL m_bSaveLineHeight;
  FX_FLOAT m_fWidth;
  FX_FLOAT m_fHeight;
  FX_FLOAT m_fLastPos;
  FX_FLOAT m_fStartLineOffset;
  int32_t m_iChar;
  int32_t m_iLines;
  int32_t m_iTotalLines;
  CFDE_XMLNode* m_pXMLNode;
  CXFA_Node* m_pNode;
  IFDE_CSSComputedStyle* m_pParentStyle;
  CFX_ArrayTemplate<FX_FLOAT> m_lineHeights;
  uint32_t m_dwFlags;
  CFX_FloatArray m_BlocksHeight;
};

class CXFA_LinkUserData : public IFX_Retainable, public CFX_Target {
 public:
  CXFA_LinkUserData(IFX_MemoryAllocator* pAllocator, FX_WCHAR* pszText);
  ~CXFA_LinkUserData() override;

  // IFX_Retainable:
  uint32_t Retain() override;
  uint32_t Release() override;

  const FX_WCHAR* GetLinkURL();

 protected:
  IFX_MemoryAllocator* m_pAllocator;
  uint32_t m_dwRefCount;
  CFX_WideString m_wsURLContent;
};

class CXFA_TextUserData : public IFX_Retainable, public CFX_Target {
 public:
  CXFA_TextUserData(IFX_MemoryAllocator* pAllocator,
                    IFDE_CSSComputedStyle* pStyle);
  CXFA_TextUserData(IFX_MemoryAllocator* pAllocator,
                    IFDE_CSSComputedStyle* pStyle,
                    CXFA_LinkUserData* pLinkData);
  ~CXFA_TextUserData() override;

  // IFX_Retainable:
  uint32_t Retain() override;
  uint32_t Release() override;

  IFDE_CSSComputedStyle* m_pStyle;
  CXFA_LinkUserData* m_pLinkData;

 protected:
  IFX_MemoryAllocator* m_pAllocator;
  uint32_t m_dwRefCount;
};

class XFA_TextPiece : public CFX_Target {
 public:
  XFA_TextPiece();
  ~XFA_TextPiece() override;

  FX_WCHAR* pszText;
  int32_t iChars;
  int32_t* pWidths;
  int32_t iHorScale;
  int32_t iVerScale;
  int32_t iBidiLevel;
  int32_t iUnderline;
  int32_t iPeriod;
  int32_t iLineThrough;
  CFGAS_GEFont* pFont;
  FX_ARGB dwColor;
  FX_FLOAT fFontSize;
  CFX_RectF rtPiece;
  CXFA_LinkUserData* pLinkData;
};
typedef CFX_ArrayTemplate<XFA_TextPiece*> CXFA_PieceArray;

class CXFA_PieceLine : public CFX_Target {
 public:
  CXFA_PieceLine();
  ~CXFA_PieceLine() override;

  CXFA_PieceArray m_textPieces;
  CFX_Int32Array m_charCounts;
};
typedef CFX_ArrayTemplate<CXFA_PieceLine*> CXFA_PieceLineArray;

struct XFA_TABSTOPS {
  uint32_t dwAlign;
  FX_FLOAT fTabstops;
};

class CXFA_TextTabstopsContext {
 public:
  CXFA_TextTabstopsContext();
  ~CXFA_TextTabstopsContext();

  void Append(uint32_t dwAlign, FX_FLOAT fTabstops);
  void RemoveAll();
  void Reset();

  CFX_ArrayTemplate<XFA_TABSTOPS> m_tabstops;
  int32_t m_iTabCount;
  int32_t m_iTabIndex;
  FX_BOOL m_bTabstops;
  FX_FLOAT m_fTabWidth;
  FX_FLOAT m_fLeft;
};

class CXFA_TextLayout {
 public:
  explicit CXFA_TextLayout(CXFA_TextProvider* pTextProvider);
  ~CXFA_TextLayout();

  int32_t GetText(CFX_WideString& wsText);
  FX_FLOAT GetLayoutHeight();
  FX_FLOAT StartLayout(FX_FLOAT fWidth = -1);
  FX_BOOL DoLayout(int32_t iBlockIndex,
                   FX_FLOAT& fCalcHeight,
                   FX_FLOAT fContentAreaHeight = -1,
                   FX_FLOAT fTextHeight = -1);

  FX_BOOL CalcSize(const CFX_SizeF& minSize,
                   const CFX_SizeF& maxSize,
                   CFX_SizeF& defaultSize);
  FX_BOOL Layout(const CFX_SizeF& size, FX_FLOAT* fHeight = nullptr);
  void ItemBlocks(const CFX_RectF& rtText, int32_t iBlockIndex);
  FX_BOOL DrawString(CFX_RenderDevice* pFxDevice,
                     const CFX_Matrix& tmDoc2Device,
                     const CFX_RectF& rtClip,
                     int32_t iBlock = 0);
  FX_BOOL IsLoaded() const { return m_pieceLines.GetSize() > 0; }
  void Unload();
  const CXFA_PieceLineArray* GetPieceLines();

  FX_BOOL m_bHasBlock;
  CFX_Int32Array m_Blocks;

 private:
  void GetTextDataNode();
  CFDE_XMLNode* GetXMLContainerNode();
  CFX_RTFBreak* CreateBreak(FX_BOOL bDefault);
  void InitBreak(FX_FLOAT fLineWidth);
  void InitBreak(IFDE_CSSComputedStyle* pStyle,
                 FDE_CSSDISPLAY eDisplay,
                 FX_FLOAT fLineWidth,
                 CFDE_XMLNode* pXMLNode,
                 IFDE_CSSComputedStyle* pParentStyle = nullptr);
  FX_BOOL Loader(const CFX_SizeF& szText,
                 FX_FLOAT& fLinePos,
                 FX_BOOL bSavePieces = TRUE);
  void LoadText(CXFA_Node* pNode,
                const CFX_SizeF& szText,
                FX_FLOAT& fLinePos,
                FX_BOOL bSavePieces);
  FX_BOOL LoadRichText(CFDE_XMLNode* pXMLNode,
                       const CFX_SizeF& szText,
                       FX_FLOAT& fLinePos,
                       IFDE_CSSComputedStyle* pParentStyle,
                       FX_BOOL bSavePieces,
                       CXFA_LinkUserData* pLinkData = nullptr,
                       FX_BOOL bEndBreak = TRUE,
                       FX_BOOL bIsOl = FALSE,
                       int32_t iLiCount = 0);
  FX_BOOL AppendChar(const CFX_WideString& wsText,
                     FX_FLOAT& fLinePos,
                     FX_FLOAT fSpaceAbove,
                     FX_BOOL bSavePieces);
  void AppendTextLine(uint32_t dwStatus,
                      FX_FLOAT& fLinePos,
                      FX_BOOL bSavePieces,
                      FX_BOOL bEndBreak = FALSE);
  void EndBreak(uint32_t dwStatus, FX_FLOAT& fLinePos, FX_BOOL bDefault);
  FX_BOOL IsEnd(FX_BOOL bSavePieces);
  void ProcessText(CFX_WideString& wsText);
  void UpdateAlign(FX_FLOAT fHeight, FX_FLOAT fBottom);
  void RenderString(CFDE_RenderDevice* pDevice,
                    CFDE_Brush* pBrush,
                    CXFA_PieceLine* pPieceLine,
                    int32_t iPiece,
                    FXTEXT_CHARPOS* pCharPos,
                    const CFX_Matrix& tmDoc2Device);
  void RenderPath(CFDE_RenderDevice* pDevice,
                  CFDE_Pen* pPen,
                  CXFA_PieceLine* pPieceLine,
                  int32_t iPiece,
                  FXTEXT_CHARPOS* pCharPos,
                  const CFX_Matrix& tmDoc2Device);
  int32_t GetDisplayPos(const XFA_TextPiece* pPiece,
                        FXTEXT_CHARPOS* pCharPos,
                        FX_BOOL bCharCode = FALSE);
  FX_BOOL ToRun(const XFA_TextPiece* pPiece, FX_RTFTEXTOBJ& tr);
  void DoTabstops(IFDE_CSSComputedStyle* pStyle, CXFA_PieceLine* pPieceLine);
  FX_BOOL Layout(int32_t iBlock);
  int32_t CountBlocks() const;

  CXFA_TextProvider* m_pTextProvider;
  CXFA_Node* m_pTextDataNode;
  FX_BOOL m_bRichText;
  std::unique_ptr<IFX_MemoryAllocator> m_pAllocator;
  std::unique_ptr<CFX_RTFBreak> m_pBreak;
  std::unique_ptr<CXFA_LoaderContext> m_pLoader;
  int32_t m_iLines;
  FX_FLOAT m_fMaxWidth;
  CXFA_TextParser m_textParser;
  CXFA_PieceLineArray m_pieceLines;
  std::unique_ptr<CXFA_TextTabstopsContext> m_pTabstopContext;
  FX_BOOL m_bBlockContinue;
};

#endif  // XFA_FXFA_APP_XFA_TEXTLAYOUT_H_
