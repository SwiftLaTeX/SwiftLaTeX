// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/pageint.h"

#include <limits.h>

#include "core/fpdfapi/fpdf_font/cpdf_type3char.h"
#include "core/fpdfapi/fpdf_page/cpdf_allstates.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_path.h"
#include "core/fpdfapi/fpdf_parser/cpdf_boolean.h"
#include "core/fpdfapi/fpdf_parser/cpdf_null.h"
#include "core/fpdfapi/fpdf_parser/fpdf_parser_utility.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_safe_types.h"

namespace {

const uint32_t kMaxNestedArrayLevel = 512;
const uint32_t kMaxWordBuffer = 256;
const FX_STRSIZE kMaxStringLength = 32767;

}  // namespace

CPDF_StreamParser::CPDF_StreamParser(const uint8_t* pData, uint32_t dwSize) {
  m_pBuf = pData;
  m_Size = dwSize;
  m_Pos = 0;
  m_pLastObj = nullptr;
}

CPDF_StreamParser::~CPDF_StreamParser() {
  if (m_pLastObj) {
    m_pLastObj->Release();
  }
}

uint32_t DecodeAllScanlines(CCodec_ScanlineDecoder* pDecoder,
                            uint8_t*& dest_buf,
                            uint32_t& dest_size) {
  if (!pDecoder) {
    return FX_INVALID_OFFSET;
  }
  int ncomps = pDecoder->CountComps();
  int bpc = pDecoder->GetBPC();
  int width = pDecoder->GetWidth();
  int height = pDecoder->GetHeight();
  int pitch = (width * ncomps * bpc + 7) / 8;
  if (height == 0 || pitch > (1 << 30) / height) {
    delete pDecoder;
    return FX_INVALID_OFFSET;
  }
  dest_buf = FX_Alloc2D(uint8_t, pitch, height);
  dest_size = pitch * height;  // Safe since checked alloc returned.
  for (int row = 0; row < height; row++) {
    const uint8_t* pLine = pDecoder->GetScanline(row);
    if (!pLine)
      break;

    FXSYS_memcpy(dest_buf + row * pitch, pLine, pitch);
  }
  uint32_t srcoff = pDecoder->GetSrcOffset();
  delete pDecoder;
  return srcoff;
}

CCodec_ScanlineDecoder* FPDFAPI_CreateFaxDecoder(
    const uint8_t* src_buf,
    uint32_t src_size,
    int width,
    int height,
    const CPDF_Dictionary* pParams);

uint32_t PDF_DecodeInlineStream(const uint8_t* src_buf,
                                uint32_t limit,
                                int width,
                                int height,
                                CFX_ByteString& decoder,
                                CPDF_Dictionary* pParam,
                                uint8_t*& dest_buf,
                                uint32_t& dest_size) {
  if (decoder == "CCITTFaxDecode" || decoder == "CCF") {
    CCodec_ScanlineDecoder* pDecoder =
        FPDFAPI_CreateFaxDecoder(src_buf, limit, width, height, pParam);
    return DecodeAllScanlines(pDecoder, dest_buf, dest_size);
  }
  if (decoder == "ASCII85Decode" || decoder == "A85") {
    return A85Decode(src_buf, limit, dest_buf, dest_size);
  }
  if (decoder == "ASCIIHexDecode" || decoder == "AHx") {
    return HexDecode(src_buf, limit, dest_buf, dest_size);
  }
  if (decoder == "FlateDecode" || decoder == "Fl") {
    return FPDFAPI_FlateOrLZWDecode(FALSE, src_buf, limit, pParam, dest_size,
                                    dest_buf, dest_size);
  }
  if (decoder == "LZWDecode" || decoder == "LZW") {
    return FPDFAPI_FlateOrLZWDecode(TRUE, src_buf, limit, pParam, 0, dest_buf,
                                    dest_size);
  }
  if (decoder == "DCTDecode" || decoder == "DCT") {
    CCodec_ScanlineDecoder* pDecoder =
        CPDF_ModuleMgr::Get()->GetJpegModule()->CreateDecoder(
            src_buf, limit, width, height, 0,
            pParam ? pParam->GetIntegerBy("ColorTransform", 1) : 1);
    return DecodeAllScanlines(pDecoder, dest_buf, dest_size);
  }
  if (decoder == "RunLengthDecode" || decoder == "RL") {
    return RunLengthDecode(src_buf, limit, dest_buf, dest_size);
  }
  dest_size = 0;
  dest_buf = 0;
  return (uint32_t)-1;
}

CPDF_Stream* CPDF_StreamParser::ReadInlineStream(CPDF_Document* pDoc,
                                                 CPDF_Dictionary* pDict,
                                                 CPDF_Object* pCSObj) {
  if (m_Pos == m_Size)
    return nullptr;

  if (PDFCharIsWhitespace(m_pBuf[m_Pos]))
    m_Pos++;

  CFX_ByteString Decoder;
  CPDF_Dictionary* pParam = nullptr;
  CPDF_Object* pFilter = pDict->GetDirectObjectBy("Filter");
  if (pFilter) {
    if (CPDF_Array* pArray = pFilter->AsArray()) {
      Decoder = pArray->GetStringAt(0);
      CPDF_Array* pParams = pDict->GetArrayBy("DecodeParms");
      if (pParams)
        pParam = pParams->GetDictAt(0);
    } else {
      Decoder = pFilter->GetString();
      pParam = pDict->GetDictBy("DecodeParms");
    }
  }
  uint32_t width = pDict->GetIntegerBy("Width");
  uint32_t height = pDict->GetIntegerBy("Height");
  uint32_t OrigSize = 0;
  if (pCSObj) {
    uint32_t bpc = pDict->GetIntegerBy("BitsPerComponent");
    uint32_t nComponents = 1;
    CPDF_ColorSpace* pCS = pDoc->LoadColorSpace(pCSObj);
    if (!pCS) {
      nComponents = 3;
    } else {
      nComponents = pCS->CountComponents();
      pDoc->GetPageData()->ReleaseColorSpace(pCSObj);
    }
    uint32_t pitch = width;
    if (bpc && pitch > INT_MAX / bpc) {
      return nullptr;
    }
    pitch *= bpc;
    if (nComponents && pitch > INT_MAX / nComponents) {
      return nullptr;
    }
    pitch *= nComponents;
    if (pitch > INT_MAX - 7) {
      return nullptr;
    }
    pitch += 7;
    pitch /= 8;
    OrigSize = pitch;
  } else {
    if (width > INT_MAX - 7) {
      return nullptr;
    }
    OrigSize = ((width + 7) / 8);
  }
  if (height && OrigSize > INT_MAX / height) {
    return nullptr;
  }
  OrigSize *= height;
  uint8_t* pData = nullptr;
  uint32_t dwStreamSize;
  if (Decoder.IsEmpty()) {
    if (OrigSize > m_Size - m_Pos) {
      OrigSize = m_Size - m_Pos;
    }
    pData = FX_Alloc(uint8_t, OrigSize);
    FXSYS_memcpy(pData, m_pBuf + m_Pos, OrigSize);
    dwStreamSize = OrigSize;
    m_Pos += OrigSize;
  } else {
    uint32_t dwDestSize = OrigSize;
    dwStreamSize =
        PDF_DecodeInlineStream(m_pBuf + m_Pos, m_Size - m_Pos, width, height,
                               Decoder, pParam, pData, dwDestSize);
    FX_Free(pData);
    if ((int)dwStreamSize < 0)
      return nullptr;

    uint32_t dwSavePos = m_Pos;
    m_Pos += dwStreamSize;
    while (1) {
      uint32_t dwPrevPos = m_Pos;
      CPDF_StreamParser::SyntaxType type = ParseNextElement();
      if (type == CPDF_StreamParser::EndOfData)
        break;

      if (type != CPDF_StreamParser::Keyword) {
        dwStreamSize += m_Pos - dwPrevPos;
        continue;
      }
      if (GetWordSize() == 2 && GetWordBuf()[0] == 'E' &&
          GetWordBuf()[1] == 'I') {
        m_Pos = dwPrevPos;
        break;
      }
      dwStreamSize += m_Pos - dwPrevPos;
    }
    m_Pos = dwSavePos;
    pData = FX_Alloc(uint8_t, dwStreamSize);
    FXSYS_memcpy(pData, m_pBuf + m_Pos, dwStreamSize);
    m_Pos += dwStreamSize;
  }
  pDict->SetAtInteger("Length", (int)dwStreamSize);
  return new CPDF_Stream(pData, dwStreamSize, pDict);
}

CPDF_StreamParser::SyntaxType CPDF_StreamParser::ParseNextElement() {
  if (m_pLastObj) {
    m_pLastObj->Release();
    m_pLastObj = nullptr;
  }

  m_WordSize = 0;
  FX_BOOL bIsNumber = TRUE;
  if (!PositionIsInBounds())
    return EndOfData;

  int ch = m_pBuf[m_Pos++];
  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!PositionIsInBounds())
        return EndOfData;

      ch = m_pBuf[m_Pos++];
    }

    if (ch != '%')
      break;

    while (1) {
      if (!PositionIsInBounds())
        return EndOfData;

      ch = m_pBuf[m_Pos++];
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  if (PDFCharIsDelimiter(ch) && ch != '/') {
    m_Pos--;
    m_pLastObj = ReadNextObject(false, 0);
    return Others;
  }

  while (1) {
    if (m_WordSize < kMaxWordBuffer)
      m_WordBuffer[m_WordSize++] = ch;

    if (!PDFCharIsNumeric(ch))
      bIsNumber = FALSE;

    if (!PositionIsInBounds())
      break;

    ch = m_pBuf[m_Pos++];

    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }

  m_WordBuffer[m_WordSize] = 0;
  if (bIsNumber)
    return Number;

  if (m_WordBuffer[0] == '/')
    return Name;

  if (m_WordSize == 4) {
    if (memcmp(m_WordBuffer, "true", 4) == 0) {
      m_pLastObj = new CPDF_Boolean(TRUE);
      return Others;
    }
    if (memcmp(m_WordBuffer, "null", 4) == 0) {
      m_pLastObj = new CPDF_Null;
      return Others;
    }
  } else if (m_WordSize == 5) {
    if (memcmp(m_WordBuffer, "false", 5) == 0) {
      m_pLastObj = new CPDF_Boolean(FALSE);
      return Others;
    }
  }
  return Keyword;
}

CPDF_Object* CPDF_StreamParser::ReadNextObject(bool bAllowNestedArray,
                                               uint32_t dwInArrayLevel) {
  FX_BOOL bIsNumber;
  GetNextWord(bIsNumber);
  if (!m_WordSize)
    return nullptr;

  if (bIsNumber) {
    m_WordBuffer[m_WordSize] = 0;
    return new CPDF_Number(CFX_ByteStringC(m_WordBuffer, m_WordSize));
  }

  int first_char = m_WordBuffer[0];
  if (first_char == '/') {
    return new CPDF_Name(
        PDF_NameDecode(CFX_ByteStringC(m_WordBuffer + 1, m_WordSize - 1)));
  }

  if (first_char == '(')
    return new CPDF_String(ReadString(), FALSE);

  if (first_char == '<') {
    if (m_WordSize == 1)
      return new CPDF_String(ReadHexString(), TRUE);

    CPDF_Dictionary* pDict = new CPDF_Dictionary;
    while (1) {
      GetNextWord(bIsNumber);
      if (m_WordSize == 2 && m_WordBuffer[0] == '>')
        break;

      if (!m_WordSize || m_WordBuffer[0] != '/') {
        pDict->Release();
        return nullptr;
      }

      CFX_ByteString key =
          PDF_NameDecode(CFX_ByteStringC(m_WordBuffer + 1, m_WordSize - 1));
      CPDF_Object* pObj = ReadNextObject(true, 0);
      if (!pObj) {
        pDict->Release();
        return nullptr;
      }

      if (key.IsEmpty())
        pObj->Release();
      else
        pDict->SetAt(key, pObj);
    }
    return pDict;
  }

  if (first_char == '[') {
    if ((!bAllowNestedArray && dwInArrayLevel) ||
        dwInArrayLevel > kMaxNestedArrayLevel) {
      return nullptr;
    }

    CPDF_Array* pArray = new CPDF_Array;
    while (1) {
      CPDF_Object* pObj = ReadNextObject(bAllowNestedArray, dwInArrayLevel + 1);
      if (pObj) {
        pArray->Add(pObj);
        continue;
      }

      if (!m_WordSize || m_WordBuffer[0] == ']')
        break;
    }
    return pArray;
  }

  if (m_WordSize == 5 && !memcmp(m_WordBuffer, "false", 5))
    return new CPDF_Boolean(FALSE);

  if (m_WordSize == 4) {
    if (memcmp(m_WordBuffer, "true", 4) == 0)
      return new CPDF_Boolean(TRUE);

    if (memcmp(m_WordBuffer, "null", 4) == 0)
      return new CPDF_Null;
  }

  return nullptr;
}

void CPDF_StreamParser::GetNextWord(FX_BOOL& bIsNumber) {
  m_WordSize = 0;
  bIsNumber = TRUE;
  if (!PositionIsInBounds())
    return;

  int ch = m_pBuf[m_Pos++];
  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!PositionIsInBounds()) {
        return;
      }
      ch = m_pBuf[m_Pos++];
    }

    if (ch != '%')
      break;

    while (1) {
      if (!PositionIsInBounds())
        return;
      ch = m_pBuf[m_Pos++];
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  if (PDFCharIsDelimiter(ch)) {
    bIsNumber = FALSE;
    m_WordBuffer[m_WordSize++] = ch;
    if (ch == '/') {
      while (1) {
        if (!PositionIsInBounds())
          return;
        ch = m_pBuf[m_Pos++];
        if (!PDFCharIsOther(ch) && !PDFCharIsNumeric(ch)) {
          m_Pos--;
          return;
        }

        if (m_WordSize < kMaxWordBuffer)
          m_WordBuffer[m_WordSize++] = ch;
      }
    } else if (ch == '<') {
      if (!PositionIsInBounds())
        return;
      ch = m_pBuf[m_Pos++];
      if (ch == '<')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    } else if (ch == '>') {
      if (!PositionIsInBounds())
        return;
      ch = m_pBuf[m_Pos++];
      if (ch == '>')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    }
    return;
  }

  while (1) {
    if (m_WordSize < kMaxWordBuffer)
      m_WordBuffer[m_WordSize++] = ch;
    if (!PDFCharIsNumeric(ch))
      bIsNumber = FALSE;

    if (!PositionIsInBounds())
      return;
    ch = m_pBuf[m_Pos++];
    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }
}

CFX_ByteString CPDF_StreamParser::ReadString() {
  if (!PositionIsInBounds())
    return CFX_ByteString();

  uint8_t ch = m_pBuf[m_Pos++];
  CFX_ByteTextBuf buf;
  int parlevel = 0;
  int status = 0;
  int iEscCode = 0;
  while (1) {
    switch (status) {
      case 0:
        if (ch == ')') {
          if (parlevel == 0) {
            if (buf.GetLength() > kMaxStringLength) {
              return CFX_ByteString(buf.GetBuffer(), kMaxStringLength);
            }
            return buf.MakeString();
          }
          parlevel--;
          buf.AppendChar(')');
        } else if (ch == '(') {
          parlevel++;
          buf.AppendChar('(');
        } else if (ch == '\\') {
          status = 1;
        } else {
          buf.AppendChar((char)ch);
        }
        break;
      case 1:
        if (ch >= '0' && ch <= '7') {
          iEscCode = FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          status = 2;
          break;
        }
        if (ch == 'n') {
          buf.AppendChar('\n');
        } else if (ch == 'r') {
          buf.AppendChar('\r');
        } else if (ch == 't') {
          buf.AppendChar('\t');
        } else if (ch == 'b') {
          buf.AppendChar('\b');
        } else if (ch == 'f') {
          buf.AppendChar('\f');
        } else if (ch == '\r') {
          status = 4;
          break;
        } else if (ch == '\n') {
        } else {
          buf.AppendChar(ch);
        }
        status = 0;
        break;
      case 2:
        if (ch >= '0' && ch <= '7') {
          iEscCode =
              iEscCode * 8 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          status = 3;
        } else {
          buf.AppendChar(iEscCode);
          status = 0;
          continue;
        }
        break;
      case 3:
        if (ch >= '0' && ch <= '7') {
          iEscCode =
              iEscCode * 8 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          buf.AppendChar(iEscCode);
          status = 0;
        } else {
          buf.AppendChar(iEscCode);
          status = 0;
          continue;
        }
        break;
      case 4:
        status = 0;
        if (ch != '\n') {
          continue;
        }
        break;
    }
    if (!PositionIsInBounds())
      break;

    ch = m_pBuf[m_Pos++];
  }
  if (PositionIsInBounds())
    ++m_Pos;

  if (buf.GetLength() > kMaxStringLength) {
    return CFX_ByteString(buf.GetBuffer(), kMaxStringLength);
  }
  return buf.MakeString();
}

CFX_ByteString CPDF_StreamParser::ReadHexString() {
  if (!PositionIsInBounds())
    return CFX_ByteString();

  CFX_ByteTextBuf buf;
  bool bFirst = true;
  int code = 0;
  while (PositionIsInBounds()) {
    int ch = m_pBuf[m_Pos++];

    if (ch == '>')
      break;

    if (!std::isxdigit(ch))
      continue;

    int val = FXSYS_toHexDigit(ch);
    if (bFirst) {
      code = val * 16;
    } else {
      code += val;
      buf.AppendByte((uint8_t)code);
    }
    bFirst = !bFirst;
  }
  if (!bFirst)
    buf.AppendChar((char)code);

  if (buf.GetLength() > kMaxStringLength)
    return CFX_ByteString(buf.GetBuffer(), kMaxStringLength);

  return buf.MakeString();
}

bool CPDF_StreamParser::PositionIsInBounds() const {
  return m_Pos < m_Size;
}

CPDF_ContentParser::CPDF_ContentParser()
    : m_Status(Ready),
      m_InternalStage(STAGE_GETCONTENT),
      m_pObjectHolder(nullptr),
      m_bForm(false),
      m_pType3Char(nullptr),
      m_pData(nullptr),
      m_Size(0),
      m_CurrentOffset(0) {}

CPDF_ContentParser::~CPDF_ContentParser() {
  if (!m_pSingleStream)
    FX_Free(m_pData);
}

void CPDF_ContentParser::Start(CPDF_Page* pPage) {
  if (m_Status != Ready || !pPage || !pPage->m_pDocument ||
      !pPage->m_pFormDict) {
    m_Status = Done;
    return;
  }
  m_pObjectHolder = pPage;
  m_bForm = FALSE;
  m_Status = ToBeContinued;
  m_InternalStage = STAGE_GETCONTENT;
  m_CurrentOffset = 0;

  CPDF_Object* pContent = pPage->m_pFormDict->GetDirectObjectBy("Contents");
  if (!pContent) {
    m_Status = Done;
    return;
  }
  if (CPDF_Stream* pStream = pContent->AsStream()) {
    m_nStreams = 0;
    m_pSingleStream.reset(new CPDF_StreamAcc);
    m_pSingleStream->LoadAllData(pStream, FALSE);
  } else if (CPDF_Array* pArray = pContent->AsArray()) {
    m_nStreams = pArray->GetCount();
    if (m_nStreams)
      m_StreamArray.resize(m_nStreams);
    else
      m_Status = Done;
  } else {
    m_Status = Done;
  }
}

void CPDF_ContentParser::Start(CPDF_Form* pForm,
                               CPDF_AllStates* pGraphicStates,
                               const CFX_Matrix* pParentMatrix,
                               CPDF_Type3Char* pType3Char,
                               int level) {
  m_pType3Char = pType3Char;
  m_pObjectHolder = pForm;
  m_bForm = TRUE;
  CFX_Matrix form_matrix = pForm->m_pFormDict->GetMatrixBy("Matrix");
  if (pGraphicStates) {
    form_matrix.Concat(pGraphicStates->m_CTM);
  }
  CPDF_Array* pBBox = pForm->m_pFormDict->GetArrayBy("BBox");
  CFX_FloatRect form_bbox;
  CPDF_Path ClipPath;
  if (pBBox) {
    form_bbox = pBBox->GetRect();
    ClipPath.New();
    ClipPath.AppendRect(form_bbox.left, form_bbox.bottom, form_bbox.right,
                        form_bbox.top);
    ClipPath.Transform(&form_matrix);
    if (pParentMatrix) {
      ClipPath.Transform(pParentMatrix);
    }
    form_bbox.Transform(&form_matrix);
    if (pParentMatrix) {
      form_bbox.Transform(pParentMatrix);
    }
  }
  CPDF_Dictionary* pResources = pForm->m_pFormDict->GetDictBy("Resources");
  m_pParser.reset(new CPDF_StreamContentParser(
      pForm->m_pDocument, pForm->m_pPageResources, pForm->m_pResources,
      pParentMatrix, pForm, pResources, &form_bbox, pGraphicStates, level));
  m_pParser->GetCurStates()->m_CTM = form_matrix;
  m_pParser->GetCurStates()->m_ParentMatrix = form_matrix;
  if (ClipPath.NotNull()) {
    m_pParser->GetCurStates()->m_ClipPath.AppendPath(ClipPath, FXFILL_WINDING,
                                                     TRUE);
  }
  if (pForm->m_Transparency & PDFTRANS_GROUP) {
    CPDF_GeneralStateData* pData =
        m_pParser->GetCurStates()->m_GeneralState.GetModify();
    pData->m_BlendType = FXDIB_BLEND_NORMAL;
    pData->m_StrokeAlpha = 1.0f;
    pData->m_FillAlpha = 1.0f;
    pData->m_pSoftMask = nullptr;
  }
  m_nStreams = 0;
  m_pSingleStream.reset(new CPDF_StreamAcc);
  m_pSingleStream->LoadAllData(pForm->m_pFormStream, FALSE);
  m_pData = (uint8_t*)m_pSingleStream->GetData();
  m_Size = m_pSingleStream->GetSize();
  m_Status = ToBeContinued;
  m_InternalStage = STAGE_PARSE;
  m_CurrentOffset = 0;
}

void CPDF_ContentParser::Continue(IFX_Pause* pPause) {
  int steps = 0;
  while (m_Status == ToBeContinued) {
    if (m_InternalStage == STAGE_GETCONTENT) {
      if (m_CurrentOffset == m_nStreams) {
        if (!m_StreamArray.empty()) {
          FX_SAFE_UINT32 safeSize = 0;
          for (const auto& stream : m_StreamArray) {
            safeSize += stream->GetSize();
            safeSize += 1;
          }
          if (!safeSize.IsValid()) {
            m_Status = Done;
            return;
          }
          m_Size = safeSize.ValueOrDie();
          m_pData = FX_Alloc(uint8_t, m_Size);
          uint32_t pos = 0;
          for (const auto& stream : m_StreamArray) {
            FXSYS_memcpy(m_pData + pos, stream->GetData(), stream->GetSize());
            pos += stream->GetSize();
            m_pData[pos++] = ' ';
          }
          m_StreamArray.clear();
        } else {
          m_pData = (uint8_t*)m_pSingleStream->GetData();
          m_Size = m_pSingleStream->GetSize();
        }
        m_InternalStage = STAGE_PARSE;
        m_CurrentOffset = 0;
      } else {
        CPDF_Array* pContent =
            m_pObjectHolder->m_pFormDict->GetArrayBy("Contents");
        m_StreamArray[m_CurrentOffset].reset(new CPDF_StreamAcc);
        CPDF_Stream* pStreamObj = ToStream(
            pContent ? pContent->GetDirectObjectAt(m_CurrentOffset) : nullptr);
        m_StreamArray[m_CurrentOffset]->LoadAllData(pStreamObj, FALSE);
        m_CurrentOffset++;
      }
    }
    if (m_InternalStage == STAGE_PARSE) {
      if (!m_pParser) {
        m_pParser.reset(new CPDF_StreamContentParser(
            m_pObjectHolder->m_pDocument, m_pObjectHolder->m_pPageResources,
            nullptr, nullptr, m_pObjectHolder, m_pObjectHolder->m_pResources,
            &m_pObjectHolder->m_BBox, nullptr, 0));
        m_pParser->GetCurStates()->m_ColorState.GetModify()->Default();
      }
      if (m_CurrentOffset >= m_Size) {
        m_InternalStage = STAGE_CHECKCLIP;
      } else {
        m_CurrentOffset +=
            m_pParser->Parse(m_pData + m_CurrentOffset,
                             m_Size - m_CurrentOffset, PARSE_STEP_LIMIT);
      }
    }
    if (m_InternalStage == STAGE_CHECKCLIP) {
      if (m_pType3Char) {
        m_pType3Char->m_bColored = m_pParser->IsColored();
        m_pType3Char->m_Width =
            FXSYS_round(m_pParser->GetType3Data()[0] * 1000);
        m_pType3Char->m_BBox.left =
            FXSYS_round(m_pParser->GetType3Data()[2] * 1000);
        m_pType3Char->m_BBox.bottom =
            FXSYS_round(m_pParser->GetType3Data()[3] * 1000);
        m_pType3Char->m_BBox.right =
            FXSYS_round(m_pParser->GetType3Data()[4] * 1000);
        m_pType3Char->m_BBox.top =
            FXSYS_round(m_pParser->GetType3Data()[5] * 1000);
      }
      for (auto& pObj : *m_pObjectHolder->GetPageObjectList()) {
        if (pObj->m_ClipPath.IsNull()) {
          continue;
        }
        if (pObj->m_ClipPath.GetPathCount() != 1) {
          continue;
        }
        if (pObj->m_ClipPath.GetTextCount()) {
          continue;
        }
        CPDF_Path ClipPath = pObj->m_ClipPath.GetPath(0);
        if (!ClipPath.IsRect() || pObj->IsShading()) {
          continue;
        }
        CFX_FloatRect old_rect(ClipPath.GetPointX(0), ClipPath.GetPointY(0),
                               ClipPath.GetPointX(2), ClipPath.GetPointY(2));
        CFX_FloatRect obj_rect(pObj->m_Left, pObj->m_Bottom, pObj->m_Right,
                               pObj->m_Top);
        if (old_rect.Contains(obj_rect)) {
          pObj->m_ClipPath.SetNull();
        }
      }
      m_Status = Done;
      return;
    }
    steps++;
    if (pPause && pPause->NeedToPauseNow()) {
      break;
    }
  }
}
