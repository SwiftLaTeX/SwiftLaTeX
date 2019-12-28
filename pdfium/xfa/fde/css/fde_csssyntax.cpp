// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/css/fde_csssyntax.h"

#include "xfa/fde/css/fde_cssdatatable.h"
#include "xfa/fgas/crt/fgas_codepage.h"

namespace {

bool FDE_IsSelectorStart(FX_WCHAR wch) {
  return wch == '.' || wch == '#' || wch == '*' || (wch >= 'a' && wch <= 'z') ||
         (wch >= 'A' && wch <= 'Z');
}

}  // namespace

CFDE_CSSSyntaxParser::CFDE_CSSSyntaxParser()
    : m_pStream(nullptr),
      m_iStreamPos(0),
      m_iPlaneSize(0),
      m_iTextDatLen(0),
      m_dwCheck((uint32_t)-1),
      m_eMode(FDE_CSSSYNTAXMODE_RuleSet),
      m_eStatus(FDE_CSSSYNTAXSTATUS_None) {}
CFDE_CSSSyntaxParser::~CFDE_CSSSyntaxParser() {
  m_TextData.Reset();
  m_TextPlane.Reset();
}
FX_BOOL CFDE_CSSSyntaxParser::Init(IFX_Stream* pStream,
                                   int32_t iCSSPlaneSize,
                                   int32_t iTextDataSize,
                                   FX_BOOL bOnlyDeclaration) {
  ASSERT(pStream && iCSSPlaneSize > 0 && iTextDataSize > 0);
  Reset(bOnlyDeclaration);
  if (!m_TextData.EstimateSize(iTextDataSize)) {
    return FALSE;
  }
  uint8_t bom[4];
  m_pStream = pStream;
  m_iStreamPos = m_pStream->GetBOM(bom);
  m_iPlaneSize = iCSSPlaneSize;
  return TRUE;
}
FX_BOOL CFDE_CSSSyntaxParser::Init(const FX_WCHAR* pBuffer,
                                   int32_t iBufferSize,
                                   int32_t iTextDatSize,
                                   FX_BOOL bOnlyDeclaration) {
  ASSERT(pBuffer && iBufferSize > 0 && iTextDatSize > 0);
  Reset(bOnlyDeclaration);
  if (!m_TextData.EstimateSize(iTextDatSize)) {
    return FALSE;
  }
  return m_TextPlane.AttachBuffer(pBuffer, iBufferSize);
}
void CFDE_CSSSyntaxParser::Reset(FX_BOOL bOnlyDeclaration) {
  m_TextPlane.Reset();
  m_TextData.Reset();
  m_pStream = nullptr;
  m_iStreamPos = 0;
  m_iTextDatLen = 0;
  m_dwCheck = (uint32_t)-1;
  m_eStatus = FDE_CSSSYNTAXSTATUS_None;
  m_eMode = bOnlyDeclaration ? FDE_CSSSYNTAXMODE_PropertyName
                             : FDE_CSSSYNTAXMODE_RuleSet;
}
FDE_CSSSYNTAXSTATUS CFDE_CSSSyntaxParser::DoSyntaxParse() {
  while (m_eStatus >= FDE_CSSSYNTAXSTATUS_None) {
    if (m_TextPlane.IsEOF()) {
      if (!m_pStream) {
        if (m_eMode == FDE_CSSSYNTAXMODE_PropertyValue &&
            m_TextData.GetLength() > 0) {
          SaveTextData();
          return m_eStatus = FDE_CSSSYNTAXSTATUS_PropertyValue;
        }
        return m_eStatus = FDE_CSSSYNTAXSTATUS_EOS;
      }
      FX_BOOL bEOS;
      int32_t iLen = m_TextPlane.LoadFromStream(m_pStream, m_iStreamPos,
                                                m_iPlaneSize, bEOS);
      m_iStreamPos = m_pStream->GetPosition();
      if (iLen < 1) {
        if (m_eMode == FDE_CSSSYNTAXMODE_PropertyValue &&
            m_TextData.GetLength() > 0) {
          SaveTextData();
          return m_eStatus = FDE_CSSSYNTAXSTATUS_PropertyValue;
        }
        return m_eStatus = FDE_CSSSYNTAXSTATUS_EOS;
      }
    }
    FX_WCHAR wch;
    while (!m_TextPlane.IsEOF()) {
      wch = m_TextPlane.GetChar();
      switch (m_eMode) {
        case FDE_CSSSYNTAXMODE_RuleSet:
          switch (wch) {
            case '@':
              m_TextPlane.MoveNext();
              SwitchMode(FDE_CSSSYNTAXMODE_AtRule);
              break;
            case '}':
              m_TextPlane.MoveNext();
              if (RestoreMode()) {
                return FDE_CSSSYNTAXSTATUS_DeclClose;
              } else {
                return m_eStatus = FDE_CSSSYNTAXSTATUS_Error;
              }
              break;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                m_ModeStack.Push(m_eMode);
                SwitchMode(FDE_CSSSYNTAXMODE_Comment);
                break;
              }
            default:
              if (wch <= ' ') {
                m_TextPlane.MoveNext();
              } else if (FDE_IsSelectorStart(wch)) {
                SwitchMode(FDE_CSSSYNTAXMODE_Selector);
                return FDE_CSSSYNTAXSTATUS_StyleRule;
              } else {
                return m_eStatus = FDE_CSSSYNTAXSTATUS_Error;
              }
              break;
          }
          break;
        case FDE_CSSSYNTAXMODE_Selector:
          switch (wch) {
            case ',':
              m_TextPlane.MoveNext();
              SwitchMode(FDE_CSSSYNTAXMODE_Selector);
              if (m_iTextDatLen > 0) {
                return FDE_CSSSYNTAXSTATUS_Selector;
              }
              break;
            case '{':
              if (m_TextData.GetLength() > 0) {
                SaveTextData();
                return FDE_CSSSYNTAXSTATUS_Selector;
              } else {
                m_TextPlane.MoveNext();
                m_ModeStack.Push(FDE_CSSSYNTAXMODE_RuleSet);
                SwitchMode(FDE_CSSSYNTAXMODE_PropertyName);
                return FDE_CSSSYNTAXSTATUS_DeclOpen;
              }
              break;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0) {
                  return FDE_CSSSYNTAXSTATUS_Selector;
                }
                break;
              }
            default:
              AppendChar(wch);
              break;
          }
          break;
        case FDE_CSSSYNTAXMODE_PropertyName:
          switch (wch) {
            case ':':
              m_TextPlane.MoveNext();
              SwitchMode(FDE_CSSSYNTAXMODE_PropertyValue);
              return FDE_CSSSYNTAXSTATUS_PropertyName;
            case '}':
              m_TextPlane.MoveNext();
              if (RestoreMode()) {
                return FDE_CSSSYNTAXSTATUS_DeclClose;
              } else {
                return m_eStatus = FDE_CSSSYNTAXSTATUS_Error;
              }
              break;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0) {
                  return FDE_CSSSYNTAXSTATUS_PropertyName;
                }
                break;
              }
            default:
              AppendChar(wch);
              break;
          }
          break;
        case FDE_CSSSYNTAXMODE_PropertyValue:
          switch (wch) {
            case ';':
              m_TextPlane.MoveNext();
            case '}':
              SwitchMode(FDE_CSSSYNTAXMODE_PropertyName);
              return FDE_CSSSYNTAXSTATUS_PropertyValue;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0) {
                  return FDE_CSSSYNTAXSTATUS_PropertyValue;
                }
                break;
              }
            default:
              AppendChar(wch);
              break;
          }
          break;
        case FDE_CSSSYNTAXMODE_Comment:
          if (wch == '/' && m_TextData.GetLength() > 0 &&
              m_TextData.GetAt(m_TextData.GetLength() - 1) == '*') {
            RestoreMode();
          } else {
            m_TextData.AppendChar(wch);
          }
          m_TextPlane.MoveNext();
          break;
        case FDE_CSSSYNTAXMODE_MediaType:
          switch (wch) {
            case ',':
              m_TextPlane.MoveNext();
              SwitchMode(FDE_CSSSYNTAXMODE_MediaType);
              if (m_iTextDatLen > 0) {
                return FDE_CSSSYNTAXSTATUS_MediaType;
              }
              break;
            case '{': {
              FDE_CSSSYNTAXMODE* pMode = m_ModeStack.GetTopElement();
              if (!pMode || *pMode != FDE_CSSSYNTAXMODE_MediaRule)
                return m_eStatus = FDE_CSSSYNTAXSTATUS_Error;

              if (m_TextData.GetLength() > 0) {
                SaveTextData();
                return FDE_CSSSYNTAXSTATUS_MediaType;
              } else {
                m_TextPlane.MoveNext();
                *pMode = FDE_CSSSYNTAXMODE_RuleSet;
                SwitchMode(FDE_CSSSYNTAXMODE_RuleSet);
                return FDE_CSSSYNTAXSTATUS_DeclOpen;
              }
            } break;
            case ';': {
              FDE_CSSSYNTAXMODE* pMode = m_ModeStack.GetTopElement();
              if (!pMode || *pMode != FDE_CSSSYNTAXMODE_Import)
                return m_eStatus = FDE_CSSSYNTAXSTATUS_Error;

              if (m_TextData.GetLength() > 0) {
                SaveTextData();
                if (IsImportEnabled()) {
                  return FDE_CSSSYNTAXSTATUS_MediaType;
                }
              } else {
                FX_BOOL bEnabled = IsImportEnabled();
                m_TextPlane.MoveNext();
                m_ModeStack.Pop();
                SwitchMode(FDE_CSSSYNTAXMODE_RuleSet);
                if (bEnabled) {
                  DisableImport();
                  return FDE_CSSSYNTAXSTATUS_ImportClose;
                }
              }
            } break;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0) {
                  return FDE_CSSSYNTAXSTATUS_MediaType;
                }
                break;
              }
            default:
              AppendChar(wch);
              break;
          }
          break;
        case FDE_CSSSYNTAXMODE_URI: {
          FDE_CSSSYNTAXMODE* pMode = m_ModeStack.GetTopElement();
          if (!pMode || *pMode != FDE_CSSSYNTAXMODE_Import)
            return m_eStatus = FDE_CSSSYNTAXSTATUS_Error;

          if (wch <= ' ' || wch == ';') {
            int32_t iURIStart, iURILength = m_TextData.GetLength();
            if (iURILength > 0 &&
                FDE_ParseCSSURI(m_TextData.GetBuffer(), iURILength, iURIStart,
                                iURILength)) {
              m_TextData.Subtract(iURIStart, iURILength);
              SwitchMode(FDE_CSSSYNTAXMODE_MediaType);
              if (IsImportEnabled()) {
                return FDE_CSSSYNTAXSTATUS_URI;
              } else {
                break;
              }
            }
          }
          AppendChar(wch);
        } break;
        case FDE_CSSSYNTAXMODE_AtRule:
          if (wch > ' ') {
            AppendChar(wch);
          } else {
            int32_t iLen = m_TextData.GetLength();
            const FX_WCHAR* psz = m_TextData.GetBuffer();
            if (FXSYS_wcsncmp(L"charset", psz, iLen) == 0) {
              SwitchMode(FDE_CSSSYNTAXMODE_Charset);
            } else if (FXSYS_wcsncmp(L"import", psz, iLen) == 0) {
              m_ModeStack.Push(FDE_CSSSYNTAXMODE_Import);
              SwitchMode(FDE_CSSSYNTAXMODE_URI);
              if (IsImportEnabled()) {
                return FDE_CSSSYNTAXSTATUS_ImportRule;
              } else {
                break;
              }
            } else if (FXSYS_wcsncmp(L"media", psz, iLen) == 0) {
              m_ModeStack.Push(FDE_CSSSYNTAXMODE_MediaRule);
              SwitchMode(FDE_CSSSYNTAXMODE_MediaType);
              return FDE_CSSSYNTAXSTATUS_MediaRule;
            } else if (FXSYS_wcsncmp(L"font-face", psz, iLen) == 0) {
              SwitchMode(FDE_CSSSYNTAXMODE_Selector);
              return FDE_CSSSYNTAXSTATUS_FontFaceRule;
            } else if (FXSYS_wcsncmp(L"page", psz, iLen) == 0) {
              SwitchMode(FDE_CSSSYNTAXMODE_Selector);
              return FDE_CSSSYNTAXSTATUS_PageRule;
            } else {
              SwitchMode(FDE_CSSSYNTAXMODE_UnknownRule);
            }
          }
          break;
        case FDE_CSSSYNTAXMODE_Charset:
          if (wch == ';') {
            m_TextPlane.MoveNext();
            SwitchMode(FDE_CSSSYNTAXMODE_RuleSet);
            if (IsCharsetEnabled()) {
              DisableCharset();
              if (m_iTextDatLen > 0) {
                if (m_pStream) {
                  uint16_t wCodePage = FX_GetCodePageFromStringW(
                      m_TextData.GetBuffer(), m_iTextDatLen);
                  if (wCodePage < 0xFFFF &&
                      m_pStream->GetCodePage() != wCodePage) {
                    m_pStream->SetCodePage(wCodePage);
                  }
                }
                return FDE_CSSSYNTAXSTATUS_Charset;
              }
            }
          } else {
            AppendChar(wch);
          }
          break;
        case FDE_CSSSYNTAXMODE_UnknownRule:
          if (wch == ';') {
            SwitchMode(FDE_CSSSYNTAXMODE_RuleSet);
          }
          m_TextPlane.MoveNext();
          break;
        default:
          ASSERT(FALSE);
          break;
      }
    }
  }
  return m_eStatus;
}
FX_BOOL CFDE_CSSSyntaxParser::IsImportEnabled() const {
  if ((m_dwCheck & FDE_CSSSYNTAXCHECK_AllowImport) == 0) {
    return FALSE;
  }
  if (m_ModeStack.GetSize() > 1) {
    return FALSE;
  }
  return TRUE;
}
FX_BOOL CFDE_CSSSyntaxParser::AppendChar(FX_WCHAR wch) {
  m_TextPlane.MoveNext();
  if (m_TextData.GetLength() > 0 || wch > ' ') {
    m_TextData.AppendChar(wch);
    return TRUE;
  }
  return FALSE;
}
int32_t CFDE_CSSSyntaxParser::SaveTextData() {
  m_iTextDatLen = m_TextData.TrimEnd();
  m_TextData.Clear();
  return m_iTextDatLen;
}
void CFDE_CSSSyntaxParser::SwitchMode(FDE_CSSSYNTAXMODE eMode) {
  m_eMode = eMode;
  SaveTextData();
}
int32_t CFDE_CSSSyntaxParser::SwitchToComment() {
  int32_t iLength = m_TextData.GetLength();
  m_ModeStack.Push(m_eMode);
  SwitchMode(FDE_CSSSYNTAXMODE_Comment);
  return iLength;
}
FX_BOOL CFDE_CSSSyntaxParser::RestoreMode() {
  FDE_CSSSYNTAXMODE* pMode = m_ModeStack.GetTopElement();
  if (!pMode)
    return FALSE;

  SwitchMode(*pMode);
  m_ModeStack.Pop();
  return TRUE;
}
const FX_WCHAR* CFDE_CSSSyntaxParser::GetCurrentString(int32_t& iLength) const {
  iLength = m_iTextDatLen;
  return m_TextData.GetBuffer();
}
CFDE_CSSTextBuf::CFDE_CSSTextBuf()
    : m_bExtBuf(FALSE),
      m_pBuffer(nullptr),
      m_iBufLen(0),
      m_iDatLen(0),
      m_iDatPos(0) {}
CFDE_CSSTextBuf::~CFDE_CSSTextBuf() {
  Reset();
}
void CFDE_CSSTextBuf::Reset() {
  if (!m_bExtBuf) {
    FX_Free(m_pBuffer);
    m_pBuffer = nullptr;
  }
  m_iDatPos = m_iDatLen = m_iBufLen;
}
FX_BOOL CFDE_CSSTextBuf::AttachBuffer(const FX_WCHAR* pBuffer,
                                      int32_t iBufLen) {
  Reset();
  m_pBuffer = const_cast<FX_WCHAR*>(pBuffer);
  m_iDatLen = m_iBufLen = iBufLen;
  return m_bExtBuf = TRUE;
}
FX_BOOL CFDE_CSSTextBuf::EstimateSize(int32_t iAllocSize) {
  ASSERT(iAllocSize > 0);
  Clear();
  m_bExtBuf = FALSE;
  return ExpandBuf(iAllocSize);
}
int32_t CFDE_CSSTextBuf::LoadFromStream(IFX_Stream* pTxtStream,
                                        int32_t iStreamOffset,
                                        int32_t iMaxChars,
                                        FX_BOOL& bEOS) {
  ASSERT(iStreamOffset >= 0 && iMaxChars > 0);
  Clear();
  m_bExtBuf = FALSE;
  if (!ExpandBuf(iMaxChars)) {
    return 0;
  }
  if (pTxtStream->GetPosition() != iStreamOffset) {
    pTxtStream->Seek(FX_STREAMSEEK_Begin, iStreamOffset);
  }
  m_iDatLen = pTxtStream->ReadString(m_pBuffer, iMaxChars, bEOS);
  return m_iDatLen;
}
FX_BOOL CFDE_CSSTextBuf::ExpandBuf(int32_t iDesiredSize) {
  if (m_bExtBuf) {
    return FALSE;
  }
  if (!m_pBuffer) {
    m_pBuffer = FX_Alloc(FX_WCHAR, iDesiredSize);
  } else if (m_iBufLen != iDesiredSize) {
    m_pBuffer = FX_Realloc(FX_WCHAR, m_pBuffer, iDesiredSize);
  } else {
    return TRUE;
  }
  if (!m_pBuffer) {
    m_iBufLen = 0;
    return FALSE;
  }
  m_iBufLen = iDesiredSize;
  return TRUE;
}
void CFDE_CSSTextBuf::Subtract(int32_t iStart, int32_t iLength) {
  ASSERT(iStart >= 0 && iLength > 0);
  if (iLength > m_iDatLen - iStart) {
    iLength = m_iDatLen - iStart;
  }
  if (iLength < 0) {
    iLength = 0;
  } else {
    FXSYS_memmove(m_pBuffer, m_pBuffer + iStart, iLength * sizeof(FX_WCHAR));
  }
  m_iDatLen = iLength;
}
