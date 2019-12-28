// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_DATA_AVAIL_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_DATA_AVAIL_H_

#include "core/fpdfapi/fpdf_parser/cpdf_syntax_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_parser.h"
#include "core/fxcrt/include/fx_basic.h"

class CPDF_Dictionary;
class CPDF_HintTables;
class CPDF_IndirectObjectHolder;
class CPDF_Parser;

enum PDF_DATAAVAIL_STATUS {
  PDF_DATAAVAIL_HEADER = 0,
  PDF_DATAAVAIL_FIRSTPAGE,
  PDF_DATAAVAIL_FIRSTPAGE_PREPARE,
  PDF_DATAAVAIL_HINTTABLE,
  PDF_DATAAVAIL_END,
  PDF_DATAAVAIL_CROSSREF,
  PDF_DATAAVAIL_CROSSREF_ITEM,
  PDF_DATAAVAIL_CROSSREF_STREAM,
  PDF_DATAAVAIL_TRAILER,
  PDF_DATAAVAIL_LOADALLCROSSREF,
  PDF_DATAAVAIL_ROOT,
  PDF_DATAAVAIL_INFO,
  PDF_DATAAVAIL_ACROFORM,
  PDF_DATAAVAIL_ACROFORM_SUBOBJECT,
  PDF_DATAAVAIL_PAGETREE,
  PDF_DATAAVAIL_PAGE,
  PDF_DATAAVAIL_PAGE_LATERLOAD,
  PDF_DATAAVAIL_RESOURCES,
  PDF_DATAAVAIL_DONE,
  PDF_DATAAVAIL_ERROR,
  PDF_DATAAVAIL_LOADALLFILE,
  PDF_DATAAVAIL_TRAILER_APPEND
};

enum PDF_PAGENODE_TYPE {
  PDF_PAGENODE_UNKNOWN = 0,
  PDF_PAGENODE_PAGE,
  PDF_PAGENODE_PAGES,
  PDF_PAGENODE_ARRAY,
};

class CPDF_DataAvail final {
 public:
  // Must match PDF_DATA_* definitions in public/fpdf_dataavail.h, but cannot
  // #include that header. fpdfsdk/fpdf_dataavail.cpp has static_asserts
  // to make sure the two sets of values match.
  enum DocAvailStatus {
    DataError = -1,        // PDF_DATA_ERROR
    DataNotAvailable = 0,  // PDF_DATA_NOTAVAIL
    DataAvailable = 1,     // PDF_DATA_AVAIL
  };

  // Must match PDF_*LINEAR* definitions in public/fpdf_dataavail.h, but cannot
  // #include that header. fpdfsdk/fpdf_dataavail.cpp has static_asserts
  // to make sure the two sets of values match.
  enum DocLinearizationStatus {
    LinearizationUnknown = -1,  // PDF_LINEARIZATION_UNKNOWN
    NotLinearized = 0,          // PDF_NOT_LINEARIZED
    Linearized = 1,             // PDF_LINEARIZED
  };

  // Must match PDF_FORM_* definitions in public/fpdf_dataavail.h, but cannot
  // #include that header. fpdfsdk/fpdf_dataavail.cpp has static_asserts
  // to make sure the two sets of values match.
  enum DocFormStatus {
    FormError = -1,        // PDF_FORM_ERROR
    FormNotAvailable = 0,  // PDF_FORM_NOTAVAIL
    FormAvailable = 1,     // PDF_FORM_AVAIL
    FormNotExist = 2,      // PDF_FORM_NOTEXIST
  };

  class FileAvail {
   public:
    virtual ~FileAvail();
    virtual FX_BOOL IsDataAvail(FX_FILESIZE offset, uint32_t size) = 0;
  };

  class DownloadHints {
   public:
    virtual ~DownloadHints();
    virtual void AddSegment(FX_FILESIZE offset, uint32_t size) = 0;
  };

  CPDF_DataAvail(FileAvail* pFileAvail,
                 IFX_FileRead* pFileRead,
                 FX_BOOL bSupportHintTable);
  ~CPDF_DataAvail();

  DocAvailStatus IsDocAvail(DownloadHints* pHints);
  void SetDocument(CPDF_Document* pDoc);
  DocAvailStatus IsPageAvail(int iPage, DownloadHints* pHints);
  DocFormStatus IsFormAvail(DownloadHints* pHints);
  DocLinearizationStatus IsLinearizedPDF();
  FX_BOOL IsLinearized();
  void GetLinearizedMainXRefInfo(FX_FILESIZE* pPos, uint32_t* pSize);
  IFX_FileRead* GetFileRead() const { return m_pFileRead; }
  int GetPageCount() const;
  CPDF_Dictionary* GetPage(int index);

  friend class CPDF_HintTables;

 protected:
  class PageNode {
   public:
    PageNode();
    ~PageNode();

    PDF_PAGENODE_TYPE m_type;
    uint32_t m_dwPageNo;
    CFX_ArrayTemplate<PageNode*> m_childNode;
  };

  static const int kMaxDataAvailRecursionDepth = 64;
  static int s_CurrentDataAvailRecursionDepth;
  static const int kMaxPageRecursionDepth = 1024;

  uint32_t GetObjectSize(uint32_t objnum, FX_FILESIZE& offset);
  FX_BOOL IsObjectsAvail(CFX_ArrayTemplate<CPDF_Object*>& obj_array,
                         FX_BOOL bParsePage,
                         DownloadHints* pHints,
                         CFX_ArrayTemplate<CPDF_Object*>& ret_array);
  FX_BOOL CheckDocStatus(DownloadHints* pHints);
  FX_BOOL CheckHeader(DownloadHints* pHints);
  FX_BOOL CheckFirstPage(DownloadHints* pHints);
  FX_BOOL CheckHintTables(DownloadHints* pHints);
  FX_BOOL CheckEnd(DownloadHints* pHints);
  FX_BOOL CheckCrossRef(DownloadHints* pHints);
  FX_BOOL CheckCrossRefItem(DownloadHints* pHints);
  FX_BOOL CheckTrailer(DownloadHints* pHints);
  FX_BOOL CheckRoot(DownloadHints* pHints);
  FX_BOOL CheckInfo(DownloadHints* pHints);
  FX_BOOL CheckPages(DownloadHints* pHints);
  FX_BOOL CheckPage(DownloadHints* pHints);
  FX_BOOL CheckResources(DownloadHints* pHints);
  FX_BOOL CheckAnnots(DownloadHints* pHints);
  FX_BOOL CheckAcroForm(DownloadHints* pHints);
  FX_BOOL CheckAcroFormSubObject(DownloadHints* pHints);
  FX_BOOL CheckTrailerAppend(DownloadHints* pHints);
  FX_BOOL CheckPageStatus(DownloadHints* pHints);
  FX_BOOL CheckAllCrossRefStream(DownloadHints* pHints);

  int32_t CheckCrossRefStream(DownloadHints* pHints, FX_FILESIZE& xref_offset);
  FX_BOOL IsLinearizedFile(uint8_t* pData, uint32_t dwLen);
  void SetStartOffset(FX_FILESIZE dwOffset);
  FX_BOOL GetNextToken(CFX_ByteString& token);
  FX_BOOL GetNextChar(uint8_t& ch);
  CPDF_Object* ParseIndirectObjectAt(
      FX_FILESIZE pos,
      uint32_t objnum,
      CPDF_IndirectObjectHolder* pObjList = nullptr);
  CPDF_Object* GetObject(uint32_t objnum,
                         DownloadHints* pHints,
                         FX_BOOL* pExistInFile);
  FX_BOOL GetPageKids(CPDF_Parser* pParser, CPDF_Object* pPages);
  FX_BOOL PreparePageItem();
  FX_BOOL LoadPages(DownloadHints* pHints);
  FX_BOOL LoadAllXref(DownloadHints* pHints);
  FX_BOOL LoadAllFile(DownloadHints* pHints);
  DocAvailStatus CheckLinearizedData(DownloadHints* pHints);
  FX_BOOL CheckPageAnnots(int iPage, DownloadHints* pHints);

  DocAvailStatus CheckLinearizedFirstPage(int iPage, DownloadHints* pHints);
  FX_BOOL HaveResourceAncestor(CPDF_Dictionary* pDict);
  FX_BOOL CheckPage(int32_t iPage, DownloadHints* pHints);
  FX_BOOL LoadDocPages(DownloadHints* pHints);
  FX_BOOL LoadDocPage(int32_t iPage, DownloadHints* pHints);
  FX_BOOL CheckPageNode(PageNode& pageNodes,
                        int32_t iPage,
                        int32_t& iCount,
                        DownloadHints* pHints,
                        int level);
  FX_BOOL CheckUnkownPageNode(uint32_t dwPageNo,
                              PageNode* pPageNode,
                              DownloadHints* pHints);
  FX_BOOL CheckArrayPageNode(uint32_t dwPageNo,
                             PageNode* pPageNode,
                             DownloadHints* pHints);
  FX_BOOL CheckPageCount(DownloadHints* pHints);
  bool IsFirstCheck(int iPage);
  void ResetFirstCheck(int iPage);
  FX_BOOL IsDataAvail(FX_FILESIZE offset, uint32_t size, DownloadHints* pHints);

  FileAvail* const m_pFileAvail;
  IFX_FileRead* const m_pFileRead;
  CPDF_Parser m_parser;
  CPDF_SyntaxParser m_syntaxParser;
  CPDF_Object* m_pRoot;
  uint32_t m_dwRootObjNum;
  uint32_t m_dwInfoObjNum;
  CPDF_Object* m_pLinearized;
  CPDF_Object* m_pTrailer;
  FX_BOOL m_bDocAvail;
  FX_FILESIZE m_dwHeaderOffset;
  FX_FILESIZE m_dwLastXRefOffset;
  FX_FILESIZE m_dwXRefOffset;
  FX_FILESIZE m_dwTrailerOffset;
  FX_FILESIZE m_dwCurrentOffset;
  PDF_DATAAVAIL_STATUS m_docStatus;
  FX_FILESIZE m_dwFileLen;
  CPDF_Document* m_pDocument;
  std::set<uint32_t> m_ObjectSet;
  CFX_ArrayTemplate<CPDF_Object*> m_objs_array;
  FX_FILESIZE m_Pos;
  FX_FILESIZE m_bufferOffset;
  uint32_t m_bufferSize;
  CFX_ByteString m_WordBuf;
  uint8_t m_bufferData[512];
  CFX_ArrayTemplate<uint32_t> m_XRefStreamList;
  CFX_ArrayTemplate<uint32_t> m_PageObjList;
  uint32_t m_PagesObjNum;
  FX_BOOL m_bLinearized;
  uint32_t m_dwFirstPageNo;
  FX_BOOL m_bLinearedDataOK;
  FX_BOOL m_bMainXRefLoadTried;
  FX_BOOL m_bMainXRefLoadedOK;
  FX_BOOL m_bPagesTreeLoad;
  FX_BOOL m_bPagesLoad;
  CPDF_Parser* m_pCurrentParser;
  FX_FILESIZE m_dwCurrentXRefSteam;
  FX_BOOL m_bAnnotsLoad;
  FX_BOOL m_bHaveAcroForm;
  uint32_t m_dwAcroFormObjNum;
  FX_BOOL m_bAcroFormLoad;
  CPDF_Object* m_pAcroForm;
  CFX_ArrayTemplate<CPDF_Object*> m_arrayAcroforms;
  CPDF_Dictionary* m_pPageDict;
  CPDF_Object* m_pPageResource;
  FX_BOOL m_bNeedDownLoadResource;
  FX_BOOL m_bPageLoadedOK;
  FX_BOOL m_bLinearizedFormParamLoad;
  CFX_ArrayTemplate<CPDF_Object*> m_PagesArray;
  uint32_t m_dwEncryptObjNum;
  FX_FILESIZE m_dwPrevXRefOffset;
  FX_BOOL m_bTotalLoadPageTree;
  FX_BOOL m_bCurPageDictLoadOK;
  PageNode m_pageNodes;
  std::set<uint32_t> m_pageMapCheckState;
  std::set<uint32_t> m_pagesLoadState;
  std::unique_ptr<CPDF_HintTables> m_pHintTables;
  FX_BOOL m_bSupportHintTable;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_DATA_AVAIL_H_
