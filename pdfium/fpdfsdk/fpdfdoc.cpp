// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_doc.h"

#include <set>

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "third_party/base/stl_util.h"

namespace {

CPDF_Bookmark FindBookmark(const CPDF_BookmarkTree& tree,
                           CPDF_Bookmark bookmark,
                           const CFX_WideString& title,
                           std::set<CPDF_Dictionary*>* visited) {
  // Return if already checked to avoid circular calling.
  if (pdfium::ContainsKey(*visited, bookmark.GetDict()))
    return CPDF_Bookmark();
  visited->insert(bookmark.GetDict());

  if (bookmark.GetDict() &&
      bookmark.GetTitle().CompareNoCase(title.c_str()) == 0) {
    // First check this item.
    return bookmark;
  }

  // Go into children items.
  CPDF_Bookmark child = tree.GetFirstChild(bookmark);
  while (child.GetDict() && !pdfium::ContainsKey(*visited, child.GetDict())) {
    // Check this item and its children.
    CPDF_Bookmark found = FindBookmark(tree, child, title, visited);
    if (found.GetDict())
      return found;
    child = tree.GetNextSibling(child);
  }
  return CPDF_Bookmark();
}

CPDF_LinkList* GetLinkList(CPDF_Page* page) {
  if (!page)
    return nullptr;

  CPDF_Document* pDoc = page->m_pDocument;
  std::unique_ptr<CPDF_LinkList>* pHolder = pDoc->LinksContext();
  if (!pHolder->get())
    pHolder->reset(new CPDF_LinkList);
  return pHolder->get();
}

}  // namespace

DLLEXPORT FPDF_BOOKMARK STDCALL
FPDFBookmark_GetFirstChild(FPDF_DOCUMENT document, FPDF_BOOKMARK pDict) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_BookmarkTree tree(pDoc);
  CPDF_Bookmark bookmark =
      CPDF_Bookmark(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  return tree.GetFirstChild(bookmark).GetDict();
}

DLLEXPORT FPDF_BOOKMARK STDCALL
FPDFBookmark_GetNextSibling(FPDF_DOCUMENT document, FPDF_BOOKMARK pDict) {
  if (!pDict)
    return nullptr;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_BookmarkTree tree(pDoc);
  CPDF_Bookmark bookmark =
      CPDF_Bookmark(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  return tree.GetNextSibling(bookmark).GetDict();
}

DLLEXPORT unsigned long STDCALL FPDFBookmark_GetTitle(FPDF_BOOKMARK pDict,
                                                      void* buffer,
                                                      unsigned long buflen) {
  if (!pDict)
    return 0;
  CPDF_Bookmark bookmark(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  CFX_WideString title = bookmark.GetTitle();
  CFX_ByteString encodedTitle = title.UTF16LE_Encode();
  unsigned long len = encodedTitle.GetLength();
  if (buffer && buflen >= len) {
    FXSYS_memcpy(buffer, encodedTitle.c_str(), len);
  }
  return len;
}

DLLEXPORT FPDF_BOOKMARK STDCALL FPDFBookmark_Find(FPDF_DOCUMENT document,
                                                  FPDF_WIDESTRING title) {
  if (!title || title[0] == 0)
    return nullptr;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_BookmarkTree tree(pDoc);
  FX_STRSIZE len = CFX_WideString::WStringLength(title);
  CFX_WideString encodedTitle = CFX_WideString::FromUTF16LE(title, len);
  std::set<CPDF_Dictionary*> visited;
  return FindBookmark(tree, CPDF_Bookmark(), encodedTitle, &visited).GetDict();
}

DLLEXPORT FPDF_DEST STDCALL FPDFBookmark_GetDest(FPDF_DOCUMENT document,
                                                 FPDF_BOOKMARK pDict) {
  if (!pDict)
    return nullptr;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_Bookmark bookmark(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  CPDF_Dest dest = bookmark.GetDest(pDoc);
  if (dest.GetObject())
    return dest.GetObject();
  // If this bookmark is not directly associated with a dest, we try to get
  // action
  CPDF_Action action = bookmark.GetAction();
  if (!action.GetDict())
    return nullptr;
  return action.GetDest(pDoc).GetObject();
}

DLLEXPORT FPDF_ACTION STDCALL FPDFBookmark_GetAction(FPDF_BOOKMARK pDict) {
  if (!pDict)
    return nullptr;
  CPDF_Bookmark bookmark(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  return bookmark.GetAction().GetDict();
}

DLLEXPORT unsigned long STDCALL FPDFAction_GetType(FPDF_ACTION pDict) {
  if (!pDict)
    return PDFACTION_UNSUPPORTED;

  CPDF_Action action(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  CPDF_Action::ActionType type = action.GetType();
  switch (type) {
    case CPDF_Action::GoTo:
      return PDFACTION_GOTO;
    case CPDF_Action::GoToR:
      return PDFACTION_REMOTEGOTO;
    case CPDF_Action::URI:
      return PDFACTION_URI;
    case CPDF_Action::Launch:
      return PDFACTION_LAUNCH;
    default:
      return PDFACTION_UNSUPPORTED;
  }
}

DLLEXPORT FPDF_DEST STDCALL FPDFAction_GetDest(FPDF_DOCUMENT document,
                                               FPDF_ACTION pDict) {
  if (!pDict)
    return nullptr;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_Action action(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  return action.GetDest(pDoc).GetObject();
}

DLLEXPORT unsigned long STDCALL FPDFAction_GetFilePath(FPDF_ACTION pDict,
                                                       void* buffer,
                                                       unsigned long buflen) {
  unsigned long type = FPDFAction_GetType(pDict);
  if (type != PDFACTION_REMOTEGOTO && type != PDFACTION_LAUNCH)
    return 0;

  CPDF_Action action(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  CFX_ByteString path = action.GetFilePath().UTF8Encode();
  unsigned long len = path.GetLength() + 1;
  if (buffer && buflen >= len)
    FXSYS_memcpy(buffer, path.c_str(), len);
  return len;
}

DLLEXPORT unsigned long STDCALL FPDFAction_GetURIPath(FPDF_DOCUMENT document,
                                                      FPDF_ACTION pDict,
                                                      void* buffer,
                                                      unsigned long buflen) {
  if (!pDict)
    return 0;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return 0;
  CPDF_Action action(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  CFX_ByteString path = action.GetURI(pDoc);
  unsigned long len = path.GetLength() + 1;
  if (buffer && buflen >= len)
    FXSYS_memcpy(buffer, path.c_str(), len);
  return len;
}

DLLEXPORT unsigned long STDCALL FPDFDest_GetPageIndex(FPDF_DOCUMENT document,
                                                      FPDF_DEST pDict) {
  if (!pDict)
    return 0;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return 0;
  CPDF_Dest dest(static_cast<CPDF_Array*>(pDict));
  return dest.GetPageIndex(pDoc);
}

DLLEXPORT FPDF_BOOL STDCALL FPDFDest_GetLocationInPage(FPDF_DEST pDict,
                                                       FS_FLOAT* x,
                                                       FS_FLOAT* y) {

  if (!pDict || !x || !y)
    return FALSE;

  CPDF_Array* dest = static_cast<CPDF_Array*>(pDict);

  if (dest->GetCount() < 4)

    return FALSE;



  const CPDF_Name* xyz = ToName(dest->GetDirectObjectAt(1));

  const CPDF_Number* numX = ToNumber(dest->GetDirectObjectAt(2));

  const CPDF_Number* numY = ToNumber(dest->GetDirectObjectAt(3));

  if (!xyz || xyz->GetString() != "XYZ" || !numX || !numY)

    return FALSE;



  *x = numX->GetNumber();

  *y = numY->GetNumber();

  return TRUE;

}

DLLEXPORT FPDF_LINK STDCALL FPDFLink_GetLinkAtPoint(FPDF_PAGE page,
                                                    double x,
                                                    double y) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return nullptr;

  CPDF_LinkList* pLinkList = GetLinkList(pPage);
  if (!pLinkList)
    return nullptr;

  return pLinkList->GetLinkAtPoint(pPage, (FX_FLOAT)x, (FX_FLOAT)y, nullptr)
      .GetDict();
}

DLLEXPORT int STDCALL FPDFLink_GetLinkZOrderAtPoint(FPDF_PAGE page,
                                                    double x,
                                                    double y) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return -1;

  CPDF_LinkList* pLinkList = GetLinkList(pPage);
  if (!pLinkList)
    return -1;

  int z_order = -1;
  pLinkList->GetLinkAtPoint(pPage, (FX_FLOAT)x, (FX_FLOAT)y, &z_order);
  return z_order;
}

DLLEXPORT FPDF_DEST STDCALL FPDFLink_GetDest(FPDF_DOCUMENT document,
                                             FPDF_LINK pDict) {
  if (!pDict)
    return nullptr;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_Link link(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  FPDF_DEST dest = link.GetDest(pDoc).GetObject();
  if (dest)
    return dest;
  // If this link is not directly associated with a dest, we try to get action
  CPDF_Action action = link.GetAction();
  if (!action.GetDict())
    return nullptr;
  return action.GetDest(pDoc).GetObject();
}

DLLEXPORT FPDF_ACTION STDCALL FPDFLink_GetAction(FPDF_LINK pDict) {
  if (!pDict)
    return nullptr;

  CPDF_Link link(ToDictionary(static_cast<CPDF_Object*>(pDict)));
  return link.GetAction().GetDict();
}

DLLEXPORT FPDF_BOOL STDCALL FPDFLink_Enumerate(FPDF_PAGE page,
                                               int* startPos,
                                               FPDF_LINK* linkAnnot) {
  if (!startPos || !linkAnnot)
    return FALSE;
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage || !pPage->m_pFormDict)
    return FALSE;
  CPDF_Array* pAnnots = pPage->m_pFormDict->GetArrayBy("Annots");
  if (!pAnnots)
    return FALSE;
  for (size_t i = *startPos; i < pAnnots->GetCount(); i++) {
    CPDF_Dictionary* pDict =
        ToDictionary(static_cast<CPDF_Object*>(pAnnots->GetDirectObjectAt(i)));
    if (!pDict)
      continue;
    if (pDict->GetStringBy("Subtype") == "Link") {
      *startPos = static_cast<int>(i + 1);
      *linkAnnot = static_cast<FPDF_LINK>(pDict);
      return TRUE;
    }
  }
  return FALSE;
}

DLLEXPORT FPDF_BOOL STDCALL FPDFLink_GetAnnotRect(FPDF_LINK linkAnnot,
                                                  FS_RECTF* rect) {
  if (!linkAnnot || !rect)
    return FALSE;
  CPDF_Dictionary* pAnnotDict =
      ToDictionary(static_cast<CPDF_Object*>(linkAnnot));
  CFX_FloatRect rt = pAnnotDict->GetRectBy("Rect");
  rect->left = rt.left;
  rect->bottom = rt.bottom;
  rect->right = rt.right;
  rect->top = rt.top;
  return TRUE;
}

DLLEXPORT int STDCALL FPDFLink_CountQuadPoints(FPDF_LINK linkAnnot) {
  if (!linkAnnot)
    return 0;
  CPDF_Dictionary* pAnnotDict =
      ToDictionary(static_cast<CPDF_Object*>(linkAnnot));
  CPDF_Array* pArray = pAnnotDict->GetArrayBy("QuadPoints");
  if (!pArray)
    return 0;
  return static_cast<int>(pArray->GetCount() / 8);
}

DLLEXPORT FPDF_BOOL STDCALL FPDFLink_GetQuadPoints(FPDF_LINK linkAnnot,
                                                   int quadIndex,
                                                   FS_QUADPOINTSF* quadPoints) {
  if (!linkAnnot || !quadPoints)
    return FALSE;
  CPDF_Dictionary* pAnnotDict =
      ToDictionary(static_cast<CPDF_Object*>(linkAnnot));
  CPDF_Array* pArray = pAnnotDict->GetArrayBy("QuadPoints");
  if (pArray) {
    if (quadIndex < 0 ||
        static_cast<size_t>(quadIndex) >= pArray->GetCount() / 8 ||
        (static_cast<size_t>(quadIndex * 8 + 7) >= pArray->GetCount()))
      return FALSE;
    quadPoints->x1 = pArray->GetNumberAt(quadIndex * 8);
    quadPoints->y1 = pArray->GetNumberAt(quadIndex * 8 + 1);
    quadPoints->x2 = pArray->GetNumberAt(quadIndex * 8 + 2);
    quadPoints->y2 = pArray->GetNumberAt(quadIndex * 8 + 3);
    quadPoints->x3 = pArray->GetNumberAt(quadIndex * 8 + 4);
    quadPoints->y3 = pArray->GetNumberAt(quadIndex * 8 + 5);
    quadPoints->x4 = pArray->GetNumberAt(quadIndex * 8 + 6);
    quadPoints->y4 = pArray->GetNumberAt(quadIndex * 8 + 7);
    return TRUE;
  }
  return FALSE;
}

DLLEXPORT unsigned long STDCALL FPDF_GetMetaText(FPDF_DOCUMENT doc,
                                                 FPDF_BYTESTRING tag,
                                                 void* buffer,
                                                 unsigned long buflen) {
  if (!tag)
    return 0;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(doc);
  if (!pDoc)
    return 0;
  CPDF_Dictionary* pInfo = pDoc->GetInfo();
  if (!pInfo)
    return 0;
  CFX_WideString text = pInfo->GetUnicodeTextBy(tag);
  // Use UTF-16LE encoding
  CFX_ByteString encodedText = text.UTF16LE_Encode();
  unsigned long len = encodedText.GetLength();
  if (buffer && buflen >= len) {
    FXSYS_memcpy(buffer, encodedText.c_str(), len);
  }
  return len;
}
