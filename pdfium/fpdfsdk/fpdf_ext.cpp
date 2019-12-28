// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_ext.h"

#include <memory>

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_xml.h"
#include "fpdfsdk/include/fsdk_define.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#endif  // PDF_ENABLE_XFA

#define FPDFSDK_UNSUPPORT_CALL 100

class CFSDK_UnsupportInfo_Adapter : public CFX_Deletable {
 public:
  explicit CFSDK_UnsupportInfo_Adapter(UNSUPPORT_INFO* unsp_info)
      : m_unsp_info(unsp_info) {}

  void ReportError(int nErrorType);

 private:
  UNSUPPORT_INFO* const m_unsp_info;
};

void CFSDK_UnsupportInfo_Adapter::ReportError(int nErrorType) {
  if (m_unsp_info && m_unsp_info->FSDK_UnSupport_Handler) {
    m_unsp_info->FSDK_UnSupport_Handler(m_unsp_info, nErrorType);
  }
}

FX_BOOL FPDF_UnSupportError(int nError) {
  CFSDK_UnsupportInfo_Adapter* pAdapter =
      static_cast<CFSDK_UnsupportInfo_Adapter*>(
          CPDF_ModuleMgr::Get()->GetUnsupportInfoAdapter());
  if (!pAdapter)
    return FALSE;

  pAdapter->ReportError(nError);
  return TRUE;
}

DLLEXPORT FPDF_BOOL STDCALL
FSDK_SetUnSpObjProcessHandler(UNSUPPORT_INFO* unsp_info) {
  if (!unsp_info || unsp_info->version != 1)
    return FALSE;

  CPDF_ModuleMgr::Get()->SetUnsupportInfoAdapter(std::unique_ptr<CFX_Deletable>(
      new CFSDK_UnsupportInfo_Adapter(unsp_info)));
  return TRUE;
}

void CheckUnSupportAnnot(CPDF_Document* pDoc, const CPDF_Annot* pPDFAnnot) {
  CFX_ByteString cbSubType = pPDFAnnot->GetSubType();
  if (cbSubType.Compare("3D") == 0) {
    FPDF_UnSupportError(FPDF_UNSP_ANNOT_3DANNOT);
  } else if (cbSubType.Compare("Screen") == 0) {
    const CPDF_Dictionary* pAnnotDict = pPDFAnnot->GetAnnotDict();
    CFX_ByteString cbString;
    if (pAnnotDict->KeyExist("IT"))
      cbString = pAnnotDict->GetStringBy("IT");
    if (cbString.Compare("Img") != 0)
      FPDF_UnSupportError(FPDF_UNSP_ANNOT_SCREEN_MEDIA);
  } else if (cbSubType.Compare("Movie") == 0) {
    FPDF_UnSupportError(FPDF_UNSP_ANNOT_MOVIE);
  } else if (cbSubType.Compare("Sound") == 0) {
    FPDF_UnSupportError(FPDF_UNSP_ANNOT_SOUND);
  } else if (cbSubType.Compare("RichMedia") == 0) {
    FPDF_UnSupportError(FPDF_UNSP_ANNOT_SCREEN_RICHMEDIA);
  } else if (cbSubType.Compare("FileAttachment") == 0) {
    FPDF_UnSupportError(FPDF_UNSP_ANNOT_ATTACHMENT);
  } else if (cbSubType.Compare("Widget") == 0) {
    const CPDF_Dictionary* pAnnotDict = pPDFAnnot->GetAnnotDict();
    CFX_ByteString cbString;
    if (pAnnotDict->KeyExist("FT")) {
      cbString = pAnnotDict->GetStringBy("FT");
    }
    if (cbString.Compare("Sig") == 0) {
      FPDF_UnSupportError(FPDF_UNSP_ANNOT_SIG);
    }
  }
}

FX_BOOL CheckSharedForm(const CXML_Element* pElement, CFX_ByteString cbName) {
  int count = pElement->CountAttrs();
  int i = 0;
  for (i = 0; i < count; i++) {
    CFX_ByteString space, name;
    CFX_WideString value;
    pElement->GetAttrByIndex(i, space, name, value);
    if (space == "xmlns" && name == "adhocwf" &&
        value == L"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/") {
      CXML_Element* pVersion =
          pElement->GetElement("adhocwf", cbName.AsStringC());
      if (!pVersion)
        continue;
      CFX_WideString wsContent = pVersion->GetContent(0);
      int nType = wsContent.GetInteger();
      switch (nType) {
        case 1:
          FPDF_UnSupportError(FPDF_UNSP_DOC_SHAREDFORM_ACROBAT);
          break;
        case 2:
          FPDF_UnSupportError(FPDF_UNSP_DOC_SHAREDFORM_FILESYSTEM);
          break;
        case 0:
          FPDF_UnSupportError(FPDF_UNSP_DOC_SHAREDFORM_EMAIL);
          break;
      }
    }
  }

  uint32_t nCount = pElement->CountChildren();
  for (i = 0; i < (int)nCount; i++) {
    CXML_Element::ChildType childType = pElement->GetChildType(i);
    if (childType == CXML_Element::Element) {
      CXML_Element* pChild = pElement->GetElement(i);
      if (CheckSharedForm(pChild, cbName))
        return TRUE;
    }
  }
  return FALSE;
}

void CheckUnSupportError(CPDF_Document* pDoc, uint32_t err_code) {
  // Security
  if (err_code == FPDF_ERR_SECURITY) {
    FPDF_UnSupportError(FPDF_UNSP_DOC_SECURITY);
    return;
  }
  if (!pDoc)
    return;

  // Portfolios and Packages
  CPDF_Dictionary* pRootDict = pDoc->GetRoot();
  if (pRootDict) {
    CFX_ByteString cbString;
    if (pRootDict->KeyExist("Collection")) {
      FPDF_UnSupportError(FPDF_UNSP_DOC_PORTABLECOLLECTION);
      return;
    }
    if (pRootDict->KeyExist("Names")) {
      CPDF_Dictionary* pNameDict = pRootDict->GetDictBy("Names");
      if (pNameDict && pNameDict->KeyExist("EmbeddedFiles")) {
        FPDF_UnSupportError(FPDF_UNSP_DOC_ATTACHMENT);
        return;
      }
      if (pNameDict && pNameDict->KeyExist("JavaScript")) {
        CPDF_Dictionary* pJSDict = pNameDict->GetDictBy("JavaScript");
        CPDF_Array* pArray = pJSDict ? pJSDict->GetArrayBy("Names") : nullptr;
        if (pArray) {
          for (size_t i = 0; i < pArray->GetCount(); i++) {
            CFX_ByteString cbStr = pArray->GetStringAt(i);
            if (cbStr.Compare("com.adobe.acrobat.SharedReview.Register") == 0) {
              FPDF_UnSupportError(FPDF_UNSP_DOC_SHAREDREVIEW);
              return;
            }
          }
        }
      }
    }
  }

  // SharedForm
  CPDF_Metadata metaData(pDoc);
  const CXML_Element* pElement = metaData.GetRoot();
  if (pElement)
    CheckSharedForm(pElement, "workflowType");

#ifndef PDF_ENABLE_XFA
  // XFA Forms
  CPDF_InterForm interform(pDoc);
  if (interform.HasXFAForm())
    FPDF_UnSupportError(FPDF_UNSP_DOC_XFAFORM);
#endif  // PDF_ENABLE_XFA
}

DLLEXPORT int STDCALL FPDFDoc_GetPageMode(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return PAGEMODE_UNKNOWN;

  CPDF_Dictionary* pRoot = pDoc->GetRoot();
  if (!pRoot)
    return PAGEMODE_UNKNOWN;

  CPDF_Object* pName = pRoot->GetObjectBy("PageMode");
  if (!pName)
    return PAGEMODE_USENONE;

  CFX_ByteString strPageMode = pName->GetString();
  if (strPageMode.IsEmpty() || strPageMode.EqualNoCase("UseNone"))
    return PAGEMODE_USENONE;
  if (strPageMode.EqualNoCase("UseOutlines"))
    return PAGEMODE_USEOUTLINES;
  if (strPageMode.EqualNoCase("UseThumbs"))
    return PAGEMODE_USETHUMBS;
  if (strPageMode.EqualNoCase("FullScreen"))
    return PAGEMODE_FULLSCREEN;
  if (strPageMode.EqualNoCase("UseOC"))
    return PAGEMODE_USEOC;
  if (strPageMode.EqualNoCase("UseAttachments"))
    return PAGEMODE_USEATTACHMENTS;

  return PAGEMODE_UNKNOWN;
}
