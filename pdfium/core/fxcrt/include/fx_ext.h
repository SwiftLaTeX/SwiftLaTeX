// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_INCLUDE_FX_EXT_H_
#define CORE_FXCRT_INCLUDE_FX_EXT_H_

#include <cctype>
#include <cwctype>

#include "core/fxcrt/include/fx_basic.h"

#define FX_INVALID_OFFSET static_cast<uint32_t>(-1)

// TODO(thestig) Using unique_ptr with ReleaseDeleter is still not ideal.
// Come up or wait for something better. This appears in this file rather
// than fx_stream.h due to include ordering restrictions.
using ScopedFileStream =
    std::unique_ptr<IFX_FileStream, ReleaseDeleter<IFX_FileStream>>;

FX_FLOAT FXSYS_tan(FX_FLOAT a);
FX_FLOAT FXSYS_logb(FX_FLOAT b, FX_FLOAT x);
FX_FLOAT FXSYS_strtof(const FX_CHAR* pcsStr,
                      int32_t iLength = -1,
                      int32_t* pUsedLen = nullptr);
FX_FLOAT FXSYS_wcstof(const FX_WCHAR* pwsStr,
                      int32_t iLength = -1,
                      int32_t* pUsedLen = nullptr);
FX_WCHAR* FXSYS_wcsncpy(FX_WCHAR* dstStr, const FX_WCHAR* srcStr, size_t count);
int32_t FXSYS_wcsnicmp(const FX_WCHAR* s1, const FX_WCHAR* s2, size_t count);
int32_t FXSYS_strnicmp(const FX_CHAR* s1, const FX_CHAR* s2, size_t count);

inline bool FXSYS_islower(int32_t ch) {
  return ch >= 'a' && ch <= 'z';
}
inline bool FXSYS_isupper(int32_t ch) {
  return ch >= 'A' && ch <= 'Z';
}
inline int32_t FXSYS_tolower(int32_t ch) {
  return ch < 'A' || ch > 'Z' ? ch : (ch + 0x20);
}
inline int32_t FXSYS_toupper(int32_t ch) {
  return ch < 'a' || ch > 'z' ? ch : (ch - 0x20);
}
inline bool FXSYS_iswalpha(wchar_t wch) {
  return (wch >= L'A' && wch <= L'Z') || (wch >= L'a' && wch <= L'z');
}
inline bool FXSYS_iswdigit(wchar_t wch) {
  return wch >= L'0' && wch <= L'9';
}
inline bool FXSYS_iswalnum(wchar_t wch) {
  return FXSYS_iswalpha(wch) || FXSYS_iswdigit(wch);
}
inline bool FXSYS_iswspace(FX_WCHAR c) {
  return (c == 0x20) || (c == 0x0d) || (c == 0x0a) || (c == 0x09);
}

inline int FXSYS_toHexDigit(const FX_CHAR c) {
  if (!std::isxdigit(c))
    return 0;
  char upchar = std::toupper(c);
  return upchar > '9' ? upchar - 'A' + 10 : upchar - '0';
}

inline bool FXSYS_isDecimalDigit(const FX_CHAR c) {
  return !!std::isdigit(c);
}

inline bool FXSYS_isDecimalDigit(const FX_WCHAR c) {
  return !!std::iswdigit(c);
}

inline int FXSYS_toDecimalDigit(const FX_CHAR c) {
  return std::isdigit(c) ? c - '0' : 0;
}

inline int FXSYS_toDecimalDigit(const FX_WCHAR c) {
  return std::iswdigit(c) ? c - L'0' : 0;
}

FX_FLOAT FXSYS_FractionalScale(size_t scale_factor, int value);
int FXSYS_FractionalScaleCount();

uint32_t FX_HashCode_GetA(const CFX_ByteStringC& str, bool bIgnoreCase);
uint32_t FX_HashCode_GetW(const CFX_WideStringC& Str, bool bIgnoreCase);

void* FX_Random_MT_Start(uint32_t dwSeed);

uint32_t FX_Random_MT_Generate(void* pContext);

void FX_Random_MT_Close(void* pContext);

void FX_Random_GenerateBase(uint32_t* pBuffer, int32_t iCount);

void FX_Random_GenerateMT(uint32_t* pBuffer, int32_t iCount);

void FX_Random_GenerateCrypto(uint32_t* pBuffer, int32_t iCount);

#ifdef PDF_ENABLE_XFA
typedef struct FX_GUID {
  uint32_t data1;
  uint16_t data2;
  uint16_t data3;
  uint8_t data4[8];
} FX_GUID, *FX_LPGUID;
typedef FX_GUID const* FX_LPCGUID;
void FX_GUID_CreateV4(FX_LPGUID pGUID);
void FX_GUID_ToString(FX_LPCGUID pGUID,
                      CFX_ByteString& bsStr,
                      FX_BOOL bSeparator = TRUE);
#endif  // PDF_ENABLE_XFA

template <class baseType>
class CFX_SSortTemplate {
 public:
  void ShellSort(baseType* pArray, int32_t iCount) {
    ASSERT(pArray && iCount > 0);
    int32_t i, j, gap;
    baseType v1, v2;
    gap = iCount >> 1;
    while (gap > 0) {
      for (i = gap; i < iCount; i++) {
        j = i - gap;
        v1 = pArray[i];
        while (j > -1 && (v2 = pArray[j]) > v1) {
          pArray[j + gap] = v2;
          j -= gap;
        }
        pArray[j + gap] = v1;
      }
      gap >>= 1;
    }
  }
};

#endif  // CORE_FXCRT_INCLUDE_FX_EXT_H_
