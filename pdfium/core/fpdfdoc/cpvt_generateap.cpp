// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpvt_generateap.h"

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfdoc/cpvt_color.h"
#include "core/fpdfdoc/cpvt_fontmap.h"
#include "core/fpdfdoc/include/cpvt_word.h"
#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fpdfdoc/pdf_vt.h"

namespace {

FX_BOOL GenerateWidgetAP(CPDF_Document* pDoc,
                         CPDF_Dictionary* pAnnotDict,
                         const int32_t& nWidgetType) {
  CPDF_Dictionary* pFormDict = nullptr;
  if (CPDF_Dictionary* pRootDict = pDoc->GetRoot())
    pFormDict = pRootDict->GetDictBy("AcroForm");
  if (!pFormDict)
    return FALSE;

  CFX_ByteString DA;
  if (CPDF_Object* pDAObj = FPDF_GetFieldAttr(pAnnotDict, "DA"))
    DA = pDAObj->GetString();
  if (DA.IsEmpty())
    DA = pFormDict->GetStringBy("DA");
  if (DA.IsEmpty())
    return FALSE;

  CPDF_SimpleParser syntax(DA.AsStringC());
  syntax.FindTagParamFromStart("Tf", 2);
  CFX_ByteString sFontName(syntax.GetWord());
  sFontName = PDF_NameDecode(sFontName);
  if (sFontName.IsEmpty())
    return FALSE;

  FX_FLOAT fFontSize = FX_atof(syntax.GetWord());
  CPVT_Color crText = CPVT_Color::ParseColor(DA);
  FX_BOOL bUseFormRes = FALSE;
  CPDF_Dictionary* pFontDict = nullptr;
  CPDF_Dictionary* pDRDict = pAnnotDict->GetDictBy("DR");
  if (!pDRDict) {
    pDRDict = pFormDict->GetDictBy("DR");
    bUseFormRes = TRUE;
  }
  CPDF_Dictionary* pDRFontDict = pDRDict ? pDRDict->GetDictBy("Font") : nullptr;
  if (pDRFontDict) {
    pFontDict = pDRFontDict->GetDictBy(sFontName.Mid(1));
    if (!pFontDict && !bUseFormRes) {
      pDRDict = pFormDict->GetDictBy("DR");
      pDRFontDict = pDRDict->GetDictBy("Font");
      if (pDRFontDict)
        pFontDict = pDRFontDict->GetDictBy(sFontName.Mid(1));
    }
  }
  if (!pDRFontDict)
    return FALSE;

  if (!pFontDict) {
    pFontDict = new CPDF_Dictionary;
    pFontDict->SetAtName("Type", "Font");
    pFontDict->SetAtName("Subtype", "Type1");
    pFontDict->SetAtName("BaseFont", "Helvetica");
    pFontDict->SetAtName("Encoding", "WinAnsiEncoding");
    pDoc->AddIndirectObject(pFontDict);
    pDRFontDict->SetAtReference(sFontName.Mid(1), pDoc, pFontDict);
  }
  CPDF_Font* pDefFont = pDoc->LoadFont(pFontDict);
  if (!pDefFont)
    return FALSE;

  CFX_FloatRect rcAnnot = pAnnotDict->GetRectBy("Rect");
  int32_t nRotate = 0;
  if (CPDF_Dictionary* pMKDict = pAnnotDict->GetDictBy("MK"))
    nRotate = pMKDict->GetIntegerBy("R");

  CFX_FloatRect rcBBox;
  CFX_Matrix matrix;
  switch (nRotate % 360) {
    case 0:
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.right - rcAnnot.left,
                             rcAnnot.top - rcAnnot.bottom);
      break;
    case 90:
      matrix = CFX_Matrix(0, 1, -1, 0, rcAnnot.right - rcAnnot.left, 0);
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.top - rcAnnot.bottom,
                             rcAnnot.right - rcAnnot.left);
      break;
    case 180:
      matrix = CFX_Matrix(-1, 0, 0, -1, rcAnnot.right - rcAnnot.left,
                          rcAnnot.top - rcAnnot.bottom);
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.right - rcAnnot.left,
                             rcAnnot.top - rcAnnot.bottom);
      break;
    case 270:
      matrix = CFX_Matrix(0, -1, 1, 0, 0, rcAnnot.top - rcAnnot.bottom);
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.top - rcAnnot.bottom,
                             rcAnnot.right - rcAnnot.left);
      break;
  }

  BorderStyle nBorderStyle = BorderStyle::SOLID;
  FX_FLOAT fBorderWidth = 1;
  CPVT_Dash dsBorder(3, 0, 0);
  CPVT_Color crLeftTop, crRightBottom;
  if (CPDF_Dictionary* pBSDict = pAnnotDict->GetDictBy("BS")) {
    if (pBSDict->KeyExist("W"))
      fBorderWidth = pBSDict->GetNumberBy("W");

    if (CPDF_Array* pArray = pBSDict->GetArrayBy("D")) {
      dsBorder = CPVT_Dash(pArray->GetIntegerAt(0), pArray->GetIntegerAt(1),
                           pArray->GetIntegerAt(2));
    }
    switch (pBSDict->GetStringBy("S").GetAt(0)) {
      case 'S':
        nBorderStyle = BorderStyle::SOLID;
        break;
      case 'D':
        nBorderStyle = BorderStyle::DASH;
        break;
      case 'B':
        nBorderStyle = BorderStyle::BEVELED;
        fBorderWidth *= 2;
        crLeftTop = CPVT_Color(CPVT_Color::kGray, 1);
        crRightBottom = CPVT_Color(CPVT_Color::kGray, 0.5);
        break;
      case 'I':
        nBorderStyle = BorderStyle::INSET;
        fBorderWidth *= 2;
        crLeftTop = CPVT_Color(CPVT_Color::kGray, 0.5);
        crRightBottom = CPVT_Color(CPVT_Color::kGray, 0.75);
        break;
      case 'U':
        nBorderStyle = BorderStyle::UNDERLINE;
        break;
    }
  }
  CPVT_Color crBorder, crBG;
  if (CPDF_Dictionary* pMKDict = pAnnotDict->GetDictBy("MK")) {
    if (CPDF_Array* pArray = pMKDict->GetArrayBy("BC"))
      crBorder = CPVT_Color::ParseColor(*pArray);
    if (CPDF_Array* pArray = pMKDict->GetArrayBy("BG"))
      crBG = CPVT_Color::ParseColor(*pArray);
  }
  CFX_ByteTextBuf sAppStream;
  CFX_ByteString sBG = CPVT_GenerateAP::GenerateColorAP(crBG, TRUE);
  if (sBG.GetLength() > 0) {
    sAppStream << "q\n" << sBG << rcBBox.left << " " << rcBBox.bottom << " "
               << rcBBox.Width() << " " << rcBBox.Height() << " re f\n"
               << "Q\n";
  }
  CFX_ByteString sBorderStream = CPVT_GenerateAP::GenerateBorderAP(
      rcBBox, fBorderWidth, crBorder, crLeftTop, crRightBottom, nBorderStyle,
      dsBorder);
  if (sBorderStream.GetLength() > 0)
    sAppStream << "q\n" << sBorderStream << "Q\n";

  CFX_FloatRect rcBody =
      CFX_FloatRect(rcBBox.left + fBorderWidth, rcBBox.bottom + fBorderWidth,
                    rcBBox.right - fBorderWidth, rcBBox.top - fBorderWidth);
  rcBody.Normalize();
  CPDF_Dictionary* pAPDict = pAnnotDict->GetDictBy("AP");
  if (!pAPDict) {
    pAPDict = new CPDF_Dictionary;
    pAnnotDict->SetAt("AP", pAPDict);
  }
  CPDF_Stream* pNormalStream = pAPDict->GetStreamBy("N");
  if (!pNormalStream) {
    pNormalStream = new CPDF_Stream(nullptr, 0, nullptr);
    int32_t objnum = pDoc->AddIndirectObject(pNormalStream);
    pAnnotDict->GetDictBy("AP")->SetAtReference("N", pDoc, objnum);
  }
  CPDF_Dictionary* pStreamDict = pNormalStream->GetDict();
  if (pStreamDict) {
    pStreamDict->SetAtMatrix("Matrix", matrix);
    pStreamDict->SetAtRect("BBox", rcBBox);
    CPDF_Dictionary* pStreamResList = pStreamDict->GetDictBy("Resources");
    if (pStreamResList) {
      CPDF_Dictionary* pStreamResFontList = pStreamResList->GetDictBy("Font");
      if (!pStreamResFontList) {
        pStreamResFontList = new CPDF_Dictionary;
        pStreamResList->SetAt("Font", pStreamResFontList);
      }
      if (!pStreamResFontList->KeyExist(sFontName))
        pStreamResFontList->SetAtReference(sFontName, pDoc, pFontDict);
    } else {
      pStreamDict->SetAt("Resources", pFormDict->GetDictBy("DR")->Clone());
      pStreamResList = pStreamDict->GetDictBy("Resources");
    }
  }
  switch (nWidgetType) {
    case 0: {
      CFX_WideString swValue =
          FPDF_GetFieldAttr(pAnnotDict, "V")
              ? FPDF_GetFieldAttr(pAnnotDict, "V")->GetUnicodeText()
              : CFX_WideString();
      int32_t nAlign = FPDF_GetFieldAttr(pAnnotDict, "Q")
                           ? FPDF_GetFieldAttr(pAnnotDict, "Q")->GetInteger()
                           : 0;
      uint32_t dwFlags = FPDF_GetFieldAttr(pAnnotDict, "Ff")
                             ? FPDF_GetFieldAttr(pAnnotDict, "Ff")->GetInteger()
                             : 0;
      uint32_t dwMaxLen =
          FPDF_GetFieldAttr(pAnnotDict, "MaxLen")
              ? FPDF_GetFieldAttr(pAnnotDict, "MaxLen")->GetInteger()
              : 0;
      CPVT_FontMap map(
          pDoc, pStreamDict ? pStreamDict->GetDictBy("Resources") : nullptr,
          pDefFont, sFontName.Right(sFontName.GetLength() - 1));
      CPDF_VariableText::Provider prd(&map);
      CPDF_VariableText vt;
      vt.SetProvider(&prd);
      vt.SetPlateRect(rcBody);
      vt.SetAlignment(nAlign);
      if (IsFloatZero(fFontSize))
        vt.SetAutoFontSize(TRUE);
      else
        vt.SetFontSize(fFontSize);

      FX_BOOL bMultiLine = (dwFlags >> 12) & 1;
      if (bMultiLine) {
        vt.SetMultiLine(TRUE);
        vt.SetAutoReturn(TRUE);
      }
      uint16_t subWord = 0;
      if ((dwFlags >> 13) & 1) {
        subWord = '*';
        vt.SetPasswordChar(subWord);
      }
      FX_BOOL bCharArray = (dwFlags >> 24) & 1;
      if (bCharArray)
        vt.SetCharArray(dwMaxLen);
      else
        vt.SetLimitChar(dwMaxLen);

      vt.Initialize();
      vt.SetText(swValue.c_str());
      vt.RearrangeAll();
      CFX_FloatRect rcContent = vt.GetContentRect();
      CFX_FloatPoint ptOffset(0.0f, 0.0f);
      if (!bMultiLine) {
        ptOffset =
            CFX_FloatPoint(0.0f, (rcContent.Height() - rcBody.Height()) / 2.0f);
      }
      CFX_ByteString sBody = CPVT_GenerateAP::GenerateEditAP(
          &map, vt.GetIterator(), ptOffset, !bCharArray, subWord);
      if (sBody.GetLength() > 0) {
        sAppStream << "/Tx BMC\n"
                   << "q\n";
        if (rcContent.Width() > rcBody.Width() ||
            rcContent.Height() > rcBody.Height()) {
          sAppStream << rcBody.left << " " << rcBody.bottom << " "
                     << rcBody.Width() << " " << rcBody.Height()
                     << " re\nW\nn\n";
        }
        sAppStream << "BT\n" << CPVT_GenerateAP::GenerateColorAP(crText, TRUE)
                   << sBody << "ET\n"
                   << "Q\nEMC\n";
      }
    } break;
    case 1: {
      CFX_WideString swValue =
          FPDF_GetFieldAttr(pAnnotDict, "V")
              ? FPDF_GetFieldAttr(pAnnotDict, "V")->GetUnicodeText()
              : CFX_WideString();
      CPVT_FontMap map(
          pDoc, pStreamDict ? pStreamDict->GetDictBy("Resources") : nullptr,
          pDefFont, sFontName.Right(sFontName.GetLength() - 1));
      CPDF_VariableText::Provider prd(&map);
      CPDF_VariableText vt;
      vt.SetProvider(&prd);
      CFX_FloatRect rcButton = rcBody;
      rcButton.left = rcButton.right - 13;
      rcButton.Normalize();
      CFX_FloatRect rcEdit = rcBody;
      rcEdit.right = rcButton.left;
      rcEdit.Normalize();
      vt.SetPlateRect(rcEdit);
      if (IsFloatZero(fFontSize))
        vt.SetAutoFontSize(TRUE);
      else
        vt.SetFontSize(fFontSize);

      vt.Initialize();
      vt.SetText(swValue.c_str());
      vt.RearrangeAll();
      CFX_FloatRect rcContent = vt.GetContentRect();
      CFX_FloatPoint ptOffset =
          CFX_FloatPoint(0.0f, (rcContent.Height() - rcEdit.Height()) / 2.0f);
      CFX_ByteString sEdit = CPVT_GenerateAP::GenerateEditAP(
          &map, vt.GetIterator(), ptOffset, TRUE, 0);
      if (sEdit.GetLength() > 0) {
        sAppStream << "/Tx BMC\n"
                   << "q\n";
        sAppStream << rcEdit.left << " " << rcEdit.bottom << " "
                   << rcEdit.Width() << " " << rcEdit.Height() << " re\nW\nn\n";
        sAppStream << "BT\n" << CPVT_GenerateAP::GenerateColorAP(crText, TRUE)
                   << sEdit << "ET\n"
                   << "Q\nEMC\n";
      }
      CFX_ByteString sButton = CPVT_GenerateAP::GenerateColorAP(
          CPVT_Color(CPVT_Color::kRGB, 220.0f / 255.0f, 220.0f / 255.0f,
                     220.0f / 255.0f),
          TRUE);
      if (sButton.GetLength() > 0 && !rcButton.IsEmpty()) {
        sAppStream << "q\n" << sButton;
        sAppStream << rcButton.left << " " << rcButton.bottom << " "
                   << rcButton.Width() << " " << rcButton.Height() << " re f\n";
        sAppStream << "Q\n";
        CFX_ByteString sButtonBorder = CPVT_GenerateAP::GenerateBorderAP(
            rcButton, 2, CPVT_Color(CPVT_Color::kGray, 0),
            CPVT_Color(CPVT_Color::kGray, 1),
            CPVT_Color(CPVT_Color::kGray, 0.5), BorderStyle::BEVELED,
            CPVT_Dash(3, 0, 0));
        if (sButtonBorder.GetLength() > 0)
          sAppStream << "q\n" << sButtonBorder << "Q\n";

        CFX_FloatPoint ptCenter =
            CFX_FloatPoint((rcButton.left + rcButton.right) / 2,
                           (rcButton.top + rcButton.bottom) / 2);
        if (IsFloatBigger(rcButton.Width(), 6) &&
            IsFloatBigger(rcButton.Height(), 6)) {
          sAppStream << "q\n"
                     << " 0 g\n";
          sAppStream << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " m\n";
          sAppStream << ptCenter.x + 3 << " " << ptCenter.y + 1.5f << " l\n";
          sAppStream << ptCenter.x << " " << ptCenter.y - 1.5f << " l\n";
          sAppStream << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " l f\n";
          sAppStream << sButton << "Q\n";
        }
      }
    } break;
    case 2: {
      CPVT_FontMap map(
          pDoc, pStreamDict ? pStreamDict->GetDictBy("Resources") : nullptr,
          pDefFont, sFontName.Right(sFontName.GetLength() - 1));
      CPDF_VariableText::Provider prd(&map);
      CPDF_Array* pOpts = ToArray(FPDF_GetFieldAttr(pAnnotDict, "Opt"));
      CPDF_Array* pSels = ToArray(FPDF_GetFieldAttr(pAnnotDict, "I"));
      CPDF_Object* pTi = FPDF_GetFieldAttr(pAnnotDict, "TI");
      int32_t nTop = pTi ? pTi->GetInteger() : 0;
      CFX_ByteTextBuf sBody;
      if (pOpts) {
        FX_FLOAT fy = rcBody.top;
        for (size_t i = nTop, sz = pOpts->GetCount(); i < sz; i++) {
          if (IsFloatSmaller(fy, rcBody.bottom))
            break;

          if (CPDF_Object* pOpt = pOpts->GetDirectObjectAt(i)) {
            CFX_WideString swItem;
            if (pOpt->IsString())
              swItem = pOpt->GetUnicodeText();
            else if (CPDF_Array* pArray = pOpt->AsArray())
              swItem = pArray->GetDirectObjectAt(1)->GetUnicodeText();

            FX_BOOL bSelected = FALSE;
            if (pSels) {
              for (size_t s = 0, ssz = pSels->GetCount(); s < ssz; s++) {
                int value = pSels->GetIntegerAt(s);
                if (value >= 0 && i == static_cast<size_t>(value)) {
                  bSelected = TRUE;
                  break;
                }
              }
            }
            CPDF_VariableText vt;
            vt.SetProvider(&prd);
            vt.SetPlateRect(
                CFX_FloatRect(rcBody.left, 0.0f, rcBody.right, 0.0f));
            vt.SetFontSize(IsFloatZero(fFontSize) ? 12.0f : fFontSize);

            vt.Initialize();
            vt.SetText(swItem.c_str());
            vt.RearrangeAll();
            FX_FLOAT fItemHeight = vt.GetContentRect().Height();
            if (bSelected) {
              CFX_FloatRect rcItem = CFX_FloatRect(
                  rcBody.left, fy - fItemHeight, rcBody.right, fy);
              sBody << "q\n" << CPVT_GenerateAP::GenerateColorAP(
                                    CPVT_Color(CPVT_Color::kRGB, 0,
                                               51.0f / 255.0f, 113.0f / 255.0f),
                                    TRUE)
                    << rcItem.left << " " << rcItem.bottom << " "
                    << rcItem.Width() << " " << rcItem.Height() << " re f\n"
                    << "Q\n";
              sBody << "BT\n" << CPVT_GenerateAP::GenerateColorAP(
                                     CPVT_Color(CPVT_Color::kGray, 1), TRUE)
                    << CPVT_GenerateAP::GenerateEditAP(&map, vt.GetIterator(),
                                                       CFX_FloatPoint(0.0f, fy),
                                                       TRUE, 0)
                    << "ET\n";
            } else {
              sBody << "BT\n" << CPVT_GenerateAP::GenerateColorAP(crText, TRUE)
                    << CPVT_GenerateAP::GenerateEditAP(&map, vt.GetIterator(),
                                                       CFX_FloatPoint(0.0f, fy),
                                                       TRUE, 0)
                    << "ET\n";
            }
            fy -= fItemHeight;
          }
        }
      }
      if (sBody.GetSize() > 0) {
        sAppStream << "/Tx BMC\nq\n"
                   << rcBody.left << " " << rcBody.bottom << " "
                   << rcBody.Width() << " " << rcBody.Height() << " re\nW\nn\n"
                   << sBody.AsStringC() << "Q\nEMC\n";
      }
    } break;
  }
  if (pNormalStream) {
    pNormalStream->SetData((uint8_t*)sAppStream.GetBuffer(),
                           sAppStream.GetSize(), FALSE, FALSE);
    pStreamDict = pNormalStream->GetDict();
    if (pStreamDict) {
      pStreamDict->SetAtMatrix("Matrix", matrix);
      pStreamDict->SetAtRect("BBox", rcBBox);
      CPDF_Dictionary* pStreamResList = pStreamDict->GetDictBy("Resources");
      if (pStreamResList) {
        CPDF_Dictionary* pStreamResFontList = pStreamResList->GetDictBy("Font");
        if (!pStreamResFontList) {
          pStreamResFontList = new CPDF_Dictionary;
          pStreamResList->SetAt("Font", pStreamResFontList);
        }
        if (!pStreamResFontList->KeyExist(sFontName))
          pStreamResFontList->SetAtReference(sFontName, pDoc, pFontDict);
      } else {
        pStreamDict->SetAt("Resources", pFormDict->GetDictBy("DR")->Clone());
        pStreamResList = pStreamDict->GetDictBy("Resources");
      }
    }
  }
  return TRUE;
}

}  // namespace

FX_BOOL FPDF_GenerateAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  if (!pAnnotDict || pAnnotDict->GetStringBy("Subtype") != "Widget") {
    return FALSE;
  }
  CFX_ByteString field_type = FPDF_GetFieldAttr(pAnnotDict, "FT")->GetString();
  uint32_t flags = FPDF_GetFieldAttr(pAnnotDict, "Ff")
                       ? FPDF_GetFieldAttr(pAnnotDict, "Ff")->GetInteger()
                       : 0;
  if (field_type == "Tx") {
    return CPVT_GenerateAP::GenerateTextFieldAP(pDoc, pAnnotDict);
  }
  if (field_type == "Ch") {
    return (flags & (1 << 17))
               ? CPVT_GenerateAP::GenerateComboBoxAP(pDoc, pAnnotDict)
               : CPVT_GenerateAP::GenerateListBoxAP(pDoc, pAnnotDict);
  }
  if (field_type == "Btn") {
    if (!(flags & (1 << 16))) {
      if (!pAnnotDict->KeyExist("AS")) {
        if (CPDF_Dictionary* pParentDict = pAnnotDict->GetDictBy("Parent")) {
          if (pParentDict->KeyExist("AS")) {
            pAnnotDict->SetAtString("AS", pParentDict->GetStringBy("AS"));
          }
        }
      }
    }
  }
  return FALSE;
}

// Static.
FX_BOOL CPVT_GenerateAP::GenerateTextFieldAP(CPDF_Document* pDoc,
                                             CPDF_Dictionary* pAnnotDict) {
  return GenerateWidgetAP(pDoc, pAnnotDict, 0);
}

// Static.
FX_BOOL CPVT_GenerateAP::GenerateComboBoxAP(CPDF_Document* pDoc,
                                            CPDF_Dictionary* pAnnotDict) {
  return GenerateWidgetAP(pDoc, pAnnotDict, 1);
}

// Static.
FX_BOOL CPVT_GenerateAP::GenerateListBoxAP(CPDF_Document* pDoc,
                                           CPDF_Dictionary* pAnnotDict) {
  return GenerateWidgetAP(pDoc, pAnnotDict, 2);
}

// Static.
CFX_ByteString CPVT_GenerateAP::GenerateEditAP(
    IPVT_FontMap* pFontMap,
    CPDF_VariableText::Iterator* pIterator,
    const CFX_FloatPoint& ptOffset,
    FX_BOOL bContinuous,
    uint16_t SubWord) {
  CFX_ByteTextBuf sEditStream;
  CFX_ByteTextBuf sLineStream;
  CFX_ByteTextBuf sWords;
  CFX_FloatPoint ptOld(0.0f, 0.0f);
  CFX_FloatPoint ptNew(0.0f, 0.0f);
  int32_t nCurFontIndex = -1;

  pIterator->SetAt(0);

  CPVT_WordPlace oldplace;
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();
    if (bContinuous) {
      if (place.LineCmp(oldplace) != 0) {
        if (sWords.GetSize() > 0) {
          sLineStream << GetWordRenderString(sWords.MakeString());
          sEditStream << sLineStream;
          sLineStream.Clear();
          sWords.Clear();
        }
        CPVT_Word word;
        if (pIterator->GetWord(word)) {
          ptNew = CFX_FloatPoint(word.ptWord.x + ptOffset.x,
                                 word.ptWord.y + ptOffset.y);
        } else {
          CPVT_Line line;
          pIterator->GetLine(line);
          ptNew = CFX_FloatPoint(line.ptLine.x + ptOffset.x,
                                 line.ptLine.y + ptOffset.y);
        }
        if (ptNew.x != ptOld.x || ptNew.y != ptOld.y) {
          sLineStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y
                      << " Td\n";
          ptOld = ptNew;
        }
      }
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        if (word.nFontIndex != nCurFontIndex) {
          if (sWords.GetSize() > 0) {
            sLineStream << GetWordRenderString(sWords.MakeString());
            sWords.Clear();
          }
          sLineStream << GetFontSetString(pFontMap, word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }
        sWords << GetPDFWordString(pFontMap, nCurFontIndex, word.Word, SubWord);
      }
      oldplace = place;
    } else {
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        ptNew = CFX_FloatPoint(word.ptWord.x + ptOffset.x,
                               word.ptWord.y + ptOffset.y);
        if (ptNew.x != ptOld.x || ptNew.y != ptOld.y) {
          sEditStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y
                      << " Td\n";
          ptOld = ptNew;
        }
        if (word.nFontIndex != nCurFontIndex) {
          sEditStream << GetFontSetString(pFontMap, word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }
        sEditStream << GetWordRenderString(
            GetPDFWordString(pFontMap, nCurFontIndex, word.Word, SubWord));
      }
    }
  }
  if (sWords.GetSize() > 0) {
    sLineStream << GetWordRenderString(sWords.MakeString());
    sEditStream << sLineStream;
    sWords.Clear();
  }
  return sEditStream.MakeString();
}

// Static.
CFX_ByteString CPVT_GenerateAP::GenerateBorderAP(
    const CFX_FloatRect& rect,
    FX_FLOAT fWidth,
    const CPVT_Color& color,
    const CPVT_Color& crLeftTop,
    const CPVT_Color& crRightBottom,
    BorderStyle nStyle,
    const CPVT_Dash& dash) {
  CFX_ByteTextBuf sAppStream;
  CFX_ByteString sColor;
  FX_FLOAT fLeft = rect.left;
  FX_FLOAT fRight = rect.right;
  FX_FLOAT fTop = rect.top;
  FX_FLOAT fBottom = rect.bottom;
  if (fWidth > 0.0f) {
    FX_FLOAT fHalfWidth = fWidth / 2.0f;
    switch (nStyle) {
      default:
      case BorderStyle::SOLID:
        sColor = GenerateColorAP(color, TRUE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " re\n";
          sAppStream << fLeft + fWidth << " " << fBottom + fWidth << " "
                     << fRight - fLeft - fWidth * 2 << " "
                     << fTop - fBottom - fWidth * 2 << " re\n";
          sAppStream << "f*\n";
        }
        break;
      case BorderStyle::DASH:
        sColor = GenerateColorAP(color, FALSE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " w"
                     << " [" << dash.nDash << " " << dash.nGap << "] "
                     << dash.nPhase << " d\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2
                     << " m\n";
          sAppStream << fLeft + fWidth / 2 << " " << fTop - fWidth / 2
                     << " l\n";
          sAppStream << fRight - fWidth / 2 << " " << fTop - fWidth / 2
                     << " l\n";
          sAppStream << fRight - fWidth / 2 << " " << fBottom + fWidth / 2
                     << " l\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2
                     << " l S\n";
        }
        break;
      case BorderStyle::BEVELED:
      case BorderStyle::INSET:
        sColor = GenerateColorAP(crLeftTop, TRUE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth
                     << " m\n";
          sAppStream << fLeft + fHalfWidth << " " << fTop - fHalfWidth
                     << " l\n";
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth
                     << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l f\n";
        }
        sColor = GenerateColorAP(crRightBottom, TRUE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth
                     << " m\n";
          sAppStream << fRight - fHalfWidth << " " << fBottom + fHalfWidth
                     << " l\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l f\n";
        }
        sColor = GenerateColorAP(color, TRUE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " re\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth << " "
                     << fRight - fLeft - fHalfWidth * 2 << " "
                     << fTop - fBottom - fHalfWidth * 2 << " re f*\n";
        }
        break;
      case BorderStyle::UNDERLINE:
        sColor = GenerateColorAP(color, FALSE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " w\n";
          sAppStream << fLeft << " " << fBottom + fWidth / 2 << " m\n";
          sAppStream << fRight << " " << fBottom + fWidth / 2 << " l S\n";
        }
        break;
    }
  }
  return sAppStream.MakeString();
}

// Static.
CFX_ByteString CPVT_GenerateAP::GenerateColorAP(const CPVT_Color& color,
                                                const FX_BOOL& bFillOrStroke) {
  CFX_ByteTextBuf sColorStream;
  switch (color.nColorType) {
    case CPVT_Color::kRGB:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " " << (bFillOrStroke ? "rg" : "RG")
                   << "\n";
      break;
    case CPVT_Color::kGray:
      sColorStream << color.fColor1 << " " << (bFillOrStroke ? "g" : "G")
                   << "\n";
      break;
    case CPVT_Color::kCMYK:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " " << color.fColor4 << " "
                   << (bFillOrStroke ? "k" : "K") << "\n";
      break;
    case CPVT_Color::kTransparent:
      break;
  }
  return sColorStream.MakeString();
}

// Static.
CFX_ByteString CPVT_GenerateAP::GetPDFWordString(IPVT_FontMap* pFontMap,
                                                 int32_t nFontIndex,
                                                 uint16_t Word,
                                                 uint16_t SubWord) {
  CFX_ByteString sWord;
  if (SubWord > 0) {
    sWord.Format("%c", SubWord);
    return sWord;
  }

  if (!pFontMap)
    return sWord;

  if (CPDF_Font* pPDFFont = pFontMap->GetPDFFont(nFontIndex)) {
    if (pPDFFont->GetBaseFont().Compare("Symbol") == 0 ||
        pPDFFont->GetBaseFont().Compare("ZapfDingbats") == 0) {
      sWord.Format("%c", Word);
    } else {
      uint32_t dwCharCode = pPDFFont->CharCodeFromUnicode(Word);
      if (dwCharCode != CPDF_Font::kInvalidCharCode)
        pPDFFont->AppendChar(sWord, dwCharCode);
    }
  }
  return sWord;
}

// Static.
CFX_ByteString CPVT_GenerateAP::GetWordRenderString(
    const CFX_ByteString& strWords) {
  if (strWords.GetLength() > 0)
    return PDF_EncodeString(strWords) + " Tj\n";
  return "";
}

// Static.
CFX_ByteString CPVT_GenerateAP::GetFontSetString(IPVT_FontMap* pFontMap,
                                                 int32_t nFontIndex,
                                                 FX_FLOAT fFontSize) {
  CFX_ByteTextBuf sRet;
  if (pFontMap) {
    CFX_ByteString sFontAlias = pFontMap->GetPDFFontAlias(nFontIndex);
    if (sFontAlias.GetLength() > 0 && fFontSize > 0)
      sRet << "/" << sFontAlias << " " << fFontSize << " Tf\n";
  }
  return sRet.MakeString();
}
