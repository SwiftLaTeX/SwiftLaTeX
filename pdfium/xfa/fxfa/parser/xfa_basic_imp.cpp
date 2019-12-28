// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_basic_imp.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/crt/fgas_system.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

const XFA_PACKETINFO* XFA_GetPacketByName(const CFX_WideStringC& wsName) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t uHash = FX_HashCode_GetW(wsName, false);
  int32_t iStart = 0;
  int32_t iEnd = g_iXFAPacketCount - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    const XFA_PACKETINFO* pInfo = g_XFAPacketData + iMid;
    if (uHash == pInfo->uHash) {
      return pInfo;
    } else if (uHash < pInfo->uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}

const XFA_PACKETINFO* XFA_GetPacketByID(uint32_t dwPacket) {
  int32_t iStart = 0, iEnd = g_iXFAPacketCount - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    uint32_t dwFind = (g_XFAPacketData + iMid)->eName;
    if (dwPacket == dwFind) {
      return g_XFAPacketData + iMid;
    } else if (dwPacket < dwFind) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}

const XFA_PACKETINFO* XFA_GetPacketByIndex(XFA_PACKET ePacket) {
  return g_XFAPacketData + ePacket;
}

const XFA_ATTRIBUTEENUMINFO* XFA_GetAttributeEnumByName(
    const CFX_WideStringC& wsName) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t uHash = FX_HashCode_GetW(wsName, false);
  int32_t iStart = 0;
  int32_t iEnd = g_iXFAEnumCount - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    const XFA_ATTRIBUTEENUMINFO* pInfo = g_XFAEnumData + iMid;
    if (uHash == pInfo->uHash) {
      return pInfo;
    } else if (uHash < pInfo->uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}
const XFA_ATTRIBUTEENUMINFO* XFA_GetAttributeEnumByID(XFA_ATTRIBUTEENUM eName) {
  return g_XFAEnumData + eName;
}

const XFA_ATTRIBUTEINFO* XFA_GetAttributeByName(const CFX_WideStringC& wsName) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t uHash = FX_HashCode_GetW(wsName, false);
  int32_t iStart = 0;
  int32_t iEnd = g_iXFAAttributeCount - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    const XFA_ATTRIBUTEINFO* pInfo = g_XFAAttributeData + iMid;
    if (uHash == pInfo->uHash) {
      return pInfo;
    } else if (uHash < pInfo->uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}
const XFA_ATTRIBUTEINFO* XFA_GetAttributeByID(XFA_ATTRIBUTE eName) {
  return (eName < g_iXFAAttributeCount) ? (g_XFAAttributeData + eName)
                                        : nullptr;
}
FX_BOOL XFA_GetAttributeDefaultValue(void*& pValue,
                                     XFA_Element eElement,
                                     XFA_ATTRIBUTE eAttribute,
                                     XFA_ATTRIBUTETYPE eType,
                                     uint32_t dwPacket) {
  const XFA_ATTRIBUTEINFO* pInfo = XFA_GetAttributeByID(eAttribute);
  if (!pInfo) {
    return FALSE;
  }
  if (dwPacket && (dwPacket & pInfo->dwPackets) == 0) {
    return FALSE;
  }
  if (pInfo->eType == eType) {
    pValue = pInfo->pDefValue;
    return TRUE;
  } else if (pInfo->eType == XFA_ATTRIBUTETYPE_NOTSURE) {
    const XFA_NOTSUREATTRIBUTE* pAttr =
        XFA_GetNotsureAttribute(eElement, eAttribute, eType);
    if (pAttr) {
      pValue = pAttr->pValue;
      return TRUE;
    }
  }
  return FALSE;
}
XFA_ATTRIBUTEENUM XFA_GetAttributeDefaultValue_Enum(XFA_Element eElement,
                                                    XFA_ATTRIBUTE eAttribute,
                                                    uint32_t dwPacket) {
  void* pValue;
  if (XFA_GetAttributeDefaultValue(pValue, eElement, eAttribute,
                                   XFA_ATTRIBUTETYPE_Enum, dwPacket)) {
    return (XFA_ATTRIBUTEENUM)(uintptr_t)pValue;
  }
  return XFA_ATTRIBUTEENUM_Unknown;
}
CFX_WideStringC XFA_GetAttributeDefaultValue_Cdata(XFA_Element eElement,
                                                   XFA_ATTRIBUTE eAttribute,
                                                   uint32_t dwPacket) {
  void* pValue;
  if (XFA_GetAttributeDefaultValue(pValue, eElement, eAttribute,
                                   XFA_ATTRIBUTETYPE_Cdata, dwPacket)) {
    return (const FX_WCHAR*)pValue;
  }
  return nullptr;
}
FX_BOOL XFA_GetAttributeDefaultValue_Boolean(XFA_Element eElement,
                                             XFA_ATTRIBUTE eAttribute,
                                             uint32_t dwPacket) {
  void* pValue;
  if (XFA_GetAttributeDefaultValue(pValue, eElement, eAttribute,
                                   XFA_ATTRIBUTETYPE_Boolean, dwPacket)) {
    return (FX_BOOL)(uintptr_t)pValue;
  }
  return FALSE;
}

CXFA_Measurement XFA_GetAttributeDefaultValue_Measure(XFA_Element eElement,
                                                      XFA_ATTRIBUTE eAttribute,
                                                      uint32_t dwPacket) {
  void* pValue;
  if (XFA_GetAttributeDefaultValue(pValue, eElement, eAttribute,
                                   XFA_ATTRIBUTETYPE_Measure, dwPacket)) {
    return *(CXFA_Measurement*)pValue;
  }
  return CXFA_Measurement();
}

XFA_Element XFA_GetElementTypeForName(const CFX_WideStringC& wsName) {
  if (wsName.IsEmpty())
    return XFA_Element::Unknown;

  uint32_t uHash = FX_HashCode_GetW(wsName, false);
  const XFA_ELEMENTINFO* pEnd = g_XFAElementData + g_iXFAElementCount;
  auto pInfo = std::lower_bound(g_XFAElementData, pEnd, uHash,
                                [](const XFA_ELEMENTINFO& info, uint32_t hash) {
                                  return info.uHash < hash;
                                });
  if (pInfo < pEnd && pInfo->uHash == uHash)
    return pInfo->eName;
  return XFA_Element::Unknown;
}

const XFA_ELEMENTINFO* XFA_GetElementByID(XFA_Element eName) {
  return eName == XFA_Element::Unknown
             ? nullptr
             : g_XFAElementData + static_cast<int32_t>(eName);
}

const uint8_t* XFA_GetElementAttributes(XFA_Element eElement, int32_t& iCount) {
  if (eElement == XFA_Element::Unknown)
    return nullptr;

  const XFA_ELEMENTHIERARCHY* pElement =
      g_XFAElementAttributeIndex + static_cast<int32_t>(eElement);
  iCount = pElement->wCount;
  return g_XFAElementAttributeData + pElement->wStart;
}

const XFA_ATTRIBUTEINFO* XFA_GetAttributeOfElement(XFA_Element eElement,
                                                   XFA_ATTRIBUTE eAttribute,
                                                   uint32_t dwPacket) {
  int32_t iCount = 0;
  const uint8_t* pAttr = XFA_GetElementAttributes(eElement, iCount);
  if (!pAttr || iCount < 1)
    return nullptr;

  if (!std::binary_search(pAttr, pAttr + iCount, eAttribute))
    return nullptr;

  const XFA_ATTRIBUTEINFO* pInfo = XFA_GetAttributeByID(eAttribute);
  ASSERT(pInfo);
  if (dwPacket == XFA_XDPPACKET_UNKNOWN)
    return pInfo;
  return (dwPacket & pInfo->dwPackets) ? pInfo : nullptr;
}

const XFA_PROPERTY* XFA_GetElementProperties(XFA_Element eElement,
                                             int32_t& iCount) {
  if (eElement == XFA_Element::Unknown)
    return nullptr;

  const XFA_ELEMENTHIERARCHY* pElement =
      g_XFAElementPropertyIndex + static_cast<int32_t>(eElement);
  iCount = pElement->wCount;
  return g_XFAElementPropertyData + pElement->wStart;
}

const XFA_PROPERTY* XFA_GetPropertyOfElement(XFA_Element eElement,
                                             XFA_Element eProperty,
                                             uint32_t dwPacket) {
  int32_t iCount = 0;
  const XFA_PROPERTY* pProperties = XFA_GetElementProperties(eElement, iCount);
  if (!pProperties || iCount < 1)
    return nullptr;

  auto it = std::find_if(pProperties, pProperties + iCount,
                         [eProperty](const XFA_PROPERTY& prop) {
                           return prop.eName == eProperty;
                         });
  if (it == pProperties + iCount)
    return nullptr;

  const XFA_ELEMENTINFO* pInfo = XFA_GetElementByID(eProperty);
  ASSERT(pInfo);
  if (dwPacket != XFA_XDPPACKET_UNKNOWN && !(dwPacket & pInfo->dwPackets))
    return nullptr;
  return it;
}

const XFA_NOTSUREATTRIBUTE* XFA_GetNotsureAttribute(XFA_Element eElement,
                                                    XFA_ATTRIBUTE eAttribute,
                                                    XFA_ATTRIBUTETYPE eType) {
  int32_t iStart = 0, iEnd = g_iXFANotsureCount - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    const XFA_NOTSUREATTRIBUTE* pAttr = g_XFANotsureAttributes + iMid;
    if (eElement == pAttr->eElement) {
      if (pAttr->eAttribute == eAttribute) {
        if (eType == XFA_ATTRIBUTETYPE_NOTSURE || eType == pAttr->eType) {
          return pAttr;
        }
        return nullptr;
      } else {
        int32_t iBefore = iMid - 1;
        if (iBefore >= 0) {
          pAttr = g_XFANotsureAttributes + iBefore;
          while (eElement == pAttr->eElement) {
            if (pAttr->eAttribute == eAttribute) {
              if (eType == XFA_ATTRIBUTETYPE_NOTSURE || eType == pAttr->eType) {
                return pAttr;
              }
              return nullptr;
            }
            iBefore--;
            if (iBefore < 0) {
              break;
            }
            pAttr = g_XFANotsureAttributes + iBefore;
          }
        }
        int32_t iAfter = iMid + 1;
        if (iAfter <= g_iXFANotsureCount - 1) {
          pAttr = g_XFANotsureAttributes + iAfter;
          while (eElement == pAttr->eElement) {
            if (pAttr->eAttribute == eAttribute) {
              if (eType == XFA_ATTRIBUTETYPE_NOTSURE || eType == pAttr->eType) {
                return pAttr;
              }
              return nullptr;
            }
            iAfter++;
            if (iAfter > g_iXFANotsureCount - 1) {
              break;
            }
            pAttr = g_XFANotsureAttributes + iAfter;
          }
        }
        return nullptr;
      }
    } else if (eElement < pAttr->eElement) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}

const XFA_METHODINFO* XFA_GetMethodByName(XFA_Element eElement,
                                          const CFX_WideStringC& wsMethodName) {
  if (wsMethodName.IsEmpty())
    return nullptr;

  int32_t iElementIndex = static_cast<int32_t>(eElement);
  while (iElementIndex != -1) {
    const XFA_SCRIPTHIERARCHY* scriptIndex = g_XFAScriptIndex + iElementIndex;
    int32_t icount = scriptIndex->wMethodCount;
    if (icount == 0) {
      iElementIndex = scriptIndex->wParentIndex;
      continue;
    }
    uint32_t uHash = FX_HashCode_GetW(wsMethodName, false);
    int32_t iStart = scriptIndex->wMethodStart;
    int32_t iEnd = iStart + icount - 1;
    do {
      int32_t iMid = (iStart + iEnd) / 2;
      const XFA_METHODINFO* pInfo = g_SomMethodData + iMid;
      if (uHash == pInfo->uHash) {
        return pInfo;
      } else if (uHash < pInfo->uHash) {
        iEnd = iMid - 1;
      } else {
        iStart = iMid + 1;
      }
    } while (iStart <= iEnd);
    iElementIndex = scriptIndex->wParentIndex;
  }
  return nullptr;
}
const XFA_SCRIPTATTRIBUTEINFO* XFA_GetScriptAttributeByName(
    XFA_Element eElement,
    const CFX_WideStringC& wsAttributeName) {
  if (wsAttributeName.IsEmpty())
    return nullptr;

  int32_t iElementIndex = static_cast<int32_t>(eElement);
  while (iElementIndex != -1) {
    const XFA_SCRIPTHIERARCHY* scriptIndex = g_XFAScriptIndex + iElementIndex;
    int32_t icount = scriptIndex->wAttributeCount;
    if (icount == 0) {
      iElementIndex = scriptIndex->wParentIndex;
      continue;
    }
    uint32_t uHash = FX_HashCode_GetW(wsAttributeName, false);
    int32_t iStart = scriptIndex->wAttributeStart, iEnd = iStart + icount - 1;
    do {
      int32_t iMid = (iStart + iEnd) / 2;
      const XFA_SCRIPTATTRIBUTEINFO* pInfo = g_SomAttributeData + iMid;
      if (uHash == pInfo->uHash) {
        return pInfo;
      } else if (uHash < pInfo->uHash) {
        iEnd = iMid - 1;
      } else {
        iStart = iMid + 1;
      }
    } while (iStart <= iEnd);
    iElementIndex = scriptIndex->wParentIndex;
  }
  return nullptr;
}
void CXFA_Measurement::Set(const CFX_WideStringC& wsMeasure) {
  if (wsMeasure.IsEmpty()) {
    m_fValue = 0;
    m_eUnit = XFA_UNIT_Unknown;
    return;
  }
  int32_t iUsedLen = 0;
  int32_t iOffset = (wsMeasure.GetAt(0) == L'=') ? 1 : 0;
  FX_FLOAT fValue = FX_wcstof(wsMeasure.c_str() + iOffset,
                              wsMeasure.GetLength() - iOffset, &iUsedLen);
  XFA_UNIT eUnit = GetUnit(wsMeasure.Mid(iOffset + iUsedLen));
  Set(fValue, eUnit);
}
FX_BOOL CXFA_Measurement::ToString(CFX_WideString& wsMeasure) const {
  switch (GetUnit()) {
    case XFA_UNIT_Mm:
      wsMeasure.Format(L"%.8gmm", GetValue());
      return TRUE;
    case XFA_UNIT_Pt:
      wsMeasure.Format(L"%.8gpt", GetValue());
      return TRUE;
    case XFA_UNIT_In:
      wsMeasure.Format(L"%.8gin", GetValue());
      return TRUE;
    case XFA_UNIT_Cm:
      wsMeasure.Format(L"%.8gcm", GetValue());
      return TRUE;
    case XFA_UNIT_Mp:
      wsMeasure.Format(L"%.8gmp", GetValue());
      return TRUE;
    case XFA_UNIT_Pc:
      wsMeasure.Format(L"%.8gpc", GetValue());
      return TRUE;
    case XFA_UNIT_Em:
      wsMeasure.Format(L"%.8gem", GetValue());
      return TRUE;
    case XFA_UNIT_Percent:
      wsMeasure.Format(L"%.8g%%", GetValue());
      return TRUE;
    default:
      wsMeasure.Format(L"%.8g", GetValue());
      return FALSE;
  }
}
FX_BOOL CXFA_Measurement::ToUnit(XFA_UNIT eUnit, FX_FLOAT& fValue) const {
  fValue = GetValue();
  XFA_UNIT eFrom = GetUnit();
  if (eFrom == eUnit) {
    return TRUE;
  }
  switch (eFrom) {
    case XFA_UNIT_Pt:
      break;
    case XFA_UNIT_Mm:
      fValue *= 72 / 2.54f / 10;
      break;
    case XFA_UNIT_In:
      fValue *= 72;
      break;
    case XFA_UNIT_Cm:
      fValue *= 72 / 2.54f;
      break;
    case XFA_UNIT_Mp:
      fValue *= 0.001f;
      break;
    case XFA_UNIT_Pc:
      fValue *= 12.0f;
      break;
    default:
      fValue = 0;
      return FALSE;
  }
  switch (eUnit) {
    case XFA_UNIT_Pt:
      return TRUE;
    case XFA_UNIT_Mm:
      fValue /= 72 / 2.54f / 10;
      return TRUE;
    case XFA_UNIT_In:
      fValue /= 72;
      return TRUE;
    case XFA_UNIT_Cm:
      fValue /= 72 / 2.54f;
      return TRUE;
    case XFA_UNIT_Mp:
      fValue /= 0.001f;
      return TRUE;
    case XFA_UNIT_Pc:
      fValue /= 12.0f;
      return TRUE;
    default:
      fValue = 0;
      return FALSE;
  }
}
XFA_UNIT CXFA_Measurement::GetUnit(const CFX_WideStringC& wsUnit) {
  if (wsUnit == FX_WSTRC(L"mm")) {
    return XFA_UNIT_Mm;
  } else if (wsUnit == FX_WSTRC(L"pt")) {
    return XFA_UNIT_Pt;
  } else if (wsUnit == FX_WSTRC(L"in")) {
    return XFA_UNIT_In;
  } else if (wsUnit == FX_WSTRC(L"cm")) {
    return XFA_UNIT_Cm;
  } else if (wsUnit == FX_WSTRC(L"pc")) {
    return XFA_UNIT_Pc;
  } else if (wsUnit == FX_WSTRC(L"mp")) {
    return XFA_UNIT_Mp;
  } else if (wsUnit == FX_WSTRC(L"em")) {
    return XFA_UNIT_Em;
  } else if (wsUnit == FX_WSTRC(L"%")) {
    return XFA_UNIT_Percent;
  } else {
    return XFA_UNIT_Unknown;
  }
}
IFX_Stream* XFA_CreateWideTextRead(const CFX_WideString& wsBuffer) {
  return new CXFA_WideTextRead(wsBuffer);
}
CXFA_WideTextRead::CXFA_WideTextRead(const CFX_WideString& wsBuffer)
    : m_wsBuffer(wsBuffer), m_iPosition(0), m_iRefCount(1) {}
void CXFA_WideTextRead::Release() {
  if (--m_iRefCount < 1) {
    delete this;
  }
}
IFX_Stream* CXFA_WideTextRead::Retain() {
  m_iRefCount++;
  return this;
}
uint32_t CXFA_WideTextRead::GetAccessModes() const {
  return FX_STREAMACCESS_Read | FX_STREAMACCESS_Text;
}
int32_t CXFA_WideTextRead::GetLength() const {
  return m_wsBuffer.GetLength() * sizeof(FX_WCHAR);
}
int32_t CXFA_WideTextRead::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  switch (eSeek) {
    case FX_STREAMSEEK_Begin:
      m_iPosition = iOffset;
      break;
    case FX_STREAMSEEK_Current:
      m_iPosition += iOffset;
      break;
    case FX_STREAMSEEK_End:
      m_iPosition = m_wsBuffer.GetLength() + iOffset;
      break;
  }
  if (m_iPosition < 0) {
    m_iPosition = 0;
  }
  if (m_iPosition > m_wsBuffer.GetLength()) {
    m_iPosition = m_wsBuffer.GetLength();
  }
  return GetPosition();
}
int32_t CXFA_WideTextRead::GetPosition() {
  return m_iPosition * sizeof(FX_WCHAR);
}
FX_BOOL CXFA_WideTextRead::IsEOF() const {
  return m_iPosition >= m_wsBuffer.GetLength();
}
int32_t CXFA_WideTextRead::ReadData(uint8_t* pBuffer, int32_t iBufferSize) {
  return 0;
}
int32_t CXFA_WideTextRead::ReadString(FX_WCHAR* pStr,
                                      int32_t iMaxLength,
                                      FX_BOOL& bEOS,
                                      int32_t const* pByteSize) {
  iMaxLength = std::min(iMaxLength, m_wsBuffer.GetLength() - m_iPosition);
  if (iMaxLength == 0)
    return 0;

  FXSYS_wcsncpy(pStr, m_wsBuffer.c_str() + m_iPosition, iMaxLength);
  m_iPosition += iMaxLength;
  bEOS = IsEOF();
  return iMaxLength;
}
int32_t CXFA_WideTextRead::WriteData(const uint8_t* pBuffer,
                                     int32_t iBufferSize) {
  return 0;
}
int32_t CXFA_WideTextRead::WriteString(const FX_WCHAR* pStr, int32_t iLength) {
  return 0;
}
FX_BOOL CXFA_WideTextRead::SetLength(int32_t iLength) {
  return FALSE;
}
int32_t CXFA_WideTextRead::GetBOM(uint8_t bom[4]) const {
  return 0;
}
uint16_t CXFA_WideTextRead::GetCodePage() const {
  return (sizeof(FX_WCHAR) == 2) ? FX_CODEPAGE_UTF16LE : FX_CODEPAGE_UTF32LE;
}
uint16_t CXFA_WideTextRead::SetCodePage(uint16_t wCodePage) {
  return GetCodePage();
}

IFX_Stream* CXFA_WideTextRead::CreateSharedStream(uint32_t dwAccess,
                                                  int32_t iOffset,
                                                  int32_t iLength) {
  return nullptr;
}

void CXFA_WideTextRead::Lock() {}

void CXFA_WideTextRead::Unlock() {}

CFX_WideString CXFA_WideTextRead::GetSrcText() const {
  return m_wsBuffer;
}
