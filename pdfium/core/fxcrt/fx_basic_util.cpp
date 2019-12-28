// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_ext.h"

#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
#include <dirent.h>
#include <sys/types.h>
#else
#include <direct.h>
#endif

#include <algorithm>
#include <cctype>
#include <memory>

bool FX_atonum(const CFX_ByteStringC& strc, void* pData) {
  if (strc.Find('.') != -1) {
    FX_FLOAT* pFloat = static_cast<FX_FLOAT*>(pData);
    *pFloat = FX_atof(strc);
    return false;
  }

  int cc = 0;
  pdfium::base::CheckedNumeric<int> integer = 0;
  bool bNegative = false;
  if (strc[0] == '+') {
    cc++;
  } else if (strc[0] == '-') {
    bNegative = true;
    cc++;
  }
  while (cc < strc.GetLength() && std::isdigit(strc[cc])) {
    integer = integer * 10 + FXSYS_toDecimalDigit(strc.CharAt(cc));
    if (!integer.IsValid())
      break;
    cc++;
  }
  if (bNegative)
    integer = -integer;

  int* pInt = static_cast<int*>(pData);
  *pInt = integer.ValueOrDefault(0);
  return true;
}

static const FX_FLOAT fraction_scales[] = {
    0.1f,         0.01f,         0.001f,        0.0001f,
    0.00001f,     0.000001f,     0.0000001f,    0.00000001f,
    0.000000001f, 0.0000000001f, 0.00000000001f};
int FXSYS_FractionalScaleCount() {
  return FX_ArraySize(fraction_scales);
}

FX_FLOAT FXSYS_FractionalScale(size_t scale_factor, int value) {
  return fraction_scales[scale_factor] * value;
}

FX_FLOAT FX_atof(const CFX_ByteStringC& strc) {
  if (strc.IsEmpty())
    return 0.0;

  int cc = 0;
  bool bNegative = false;
  int len = strc.GetLength();
  if (strc[0] == '+') {
    cc++;
  } else if (strc[0] == '-') {
    bNegative = TRUE;
    cc++;
  }
  while (cc < len) {
    if (strc[cc] != '+' && strc[cc] != '-') {
      break;
    }
    cc++;
  }
  FX_FLOAT value = 0;
  while (cc < len) {
    if (strc[cc] == '.') {
      break;
    }
    value = value * 10 + FXSYS_toDecimalDigit(strc.CharAt(cc));
    cc++;
  }
  int scale = 0;
  if (cc < len && strc[cc] == '.') {
    cc++;
    while (cc < len) {
      value +=
          FXSYS_FractionalScale(scale, FXSYS_toDecimalDigit(strc.CharAt(cc)));
      scale++;
      if (scale == FXSYS_FractionalScaleCount())
        break;
      cc++;
    }
  }
  return bNegative ? -value : value;
}

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_ && _MSC_VER < 1900
void FXSYS_snprintf(char* str,
                    size_t size,
                    _Printf_format_string_ const char* fmt,
                    ...) {
  va_list ap;
  va_start(ap, fmt);
  FXSYS_vsnprintf(str, size, fmt, ap);
  va_end(ap);
}
void FXSYS_vsnprintf(char* str, size_t size, const char* fmt, va_list ap) {
  (void)_vsnprintf(str, size, fmt, ap);
  if (size) {
    str[size - 1] = 0;
  }
}
#endif  // _FXM_PLATFORM_WINDOWS_ && _MSC_VER < 1900

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
class CFindFileData {
 public:
  virtual ~CFindFileData() {}
  HANDLE m_Handle;
  FX_BOOL m_bEnd;
};

class CFindFileDataA : public CFindFileData {
 public:
  ~CFindFileDataA() override {}
  WIN32_FIND_DATAA m_FindData;
};

class CFindFileDataW : public CFindFileData {
 public:
  ~CFindFileDataW() override {}
  WIN32_FIND_DATAW m_FindData;
};
#endif

void* FX_OpenFolder(const FX_CHAR* path) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  std::unique_ptr<CFindFileDataA> pData(new CFindFileDataA);
  pData->m_Handle = FindFirstFileExA((CFX_ByteString(path) + "/*.*").c_str(),
                                     FindExInfoStandard, &pData->m_FindData,
                                     FindExSearchNameMatch, nullptr, 0);
  if (pData->m_Handle == INVALID_HANDLE_VALUE)
    return nullptr;

  pData->m_bEnd = FALSE;
  return pData.release();
#else
  DIR* dir = opendir(path);
  return dir;
#endif
}

void* FX_OpenFolder(const FX_WCHAR* path) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  std::unique_ptr<CFindFileDataW> pData(new CFindFileDataW);
  pData->m_Handle = FindFirstFileExW((CFX_WideString(path) + L"/*.*").c_str(),
                                     FindExInfoStandard, &pData->m_FindData,
                                     FindExSearchNameMatch, nullptr, 0);
  if (pData->m_Handle == INVALID_HANDLE_VALUE)
    return nullptr;

  pData->m_bEnd = FALSE;
  return pData.release();
#else
  DIR* dir = opendir(CFX_ByteString::FromUnicode(path).c_str());
  return dir;
#endif
}
FX_BOOL FX_GetNextFile(void* handle,
                       CFX_ByteString& filename,
                       FX_BOOL& bFolder) {
  if (!handle) {
    return FALSE;
  }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  CFindFileDataA* pData = (CFindFileDataA*)handle;
  if (pData->m_bEnd)
    return FALSE;

  filename = pData->m_FindData.cFileName;
  bFolder = pData->m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
  if (!FindNextFileA(pData->m_Handle, &pData->m_FindData))
    pData->m_bEnd = TRUE;
  return TRUE;
#elif defined(__native_client__)
  abort();
  return FALSE;
#else
  struct dirent* de = readdir((DIR*)handle);
  if (!de) {
    return FALSE;
  }
  filename = de->d_name;
  bFolder = de->d_type == DT_DIR;
  return TRUE;
#endif
}
FX_BOOL FX_GetNextFile(void* handle,
                       CFX_WideString& filename,
                       FX_BOOL& bFolder) {
  if (!handle) {
    return FALSE;
  }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  CFindFileDataW* pData = (CFindFileDataW*)handle;
  if (pData->m_bEnd) {
    return FALSE;
  }
  filename = pData->m_FindData.cFileName;
  bFolder = pData->m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
  if (!FindNextFileW(pData->m_Handle, &pData->m_FindData)) {
    pData->m_bEnd = TRUE;
  }
  return TRUE;
#elif defined(__native_client__)
  abort();
  return FALSE;
#else
  struct dirent* de = readdir((DIR*)handle);
  if (!de) {
    return FALSE;
  }
  filename = CFX_WideString::FromLocal(de->d_name);
  bFolder = de->d_type == DT_DIR;
  return TRUE;
#endif
}
void FX_CloseFolder(void* handle) {
  if (!handle) {
    return;
  }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  CFindFileData* pData = (CFindFileData*)handle;
  FindClose(pData->m_Handle);
  delete pData;
#else
  closedir((DIR*)handle);
#endif
}
FX_WCHAR FX_GetFolderSeparator() {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  return '\\';
#else
  return '/';
#endif
}

CFX_Matrix_3by3 CFX_Matrix_3by3::Inverse() {
  FX_FLOAT det =
      a * (e * i - f * h) - b * (i * d - f * g) + c * (d * h - e * g);
  if (FXSYS_fabs(det) < 0.0000001)
    return CFX_Matrix_3by3();

  return CFX_Matrix_3by3(
      (e * i - f * h) / det, -(b * i - c * h) / det, (b * f - c * e) / det,
      -(d * i - f * g) / det, (a * i - c * g) / det, -(a * f - c * d) / det,
      (d * h - e * g) / det, -(a * h - b * g) / det, (a * e - b * d) / det);
}

CFX_Matrix_3by3 CFX_Matrix_3by3::Multiply(const CFX_Matrix_3by3& m) {
  return CFX_Matrix_3by3(
      a * m.a + b * m.d + c * m.g, a * m.b + b * m.e + c * m.h,
      a * m.c + b * m.f + c * m.i, d * m.a + e * m.d + f * m.g,
      d * m.b + e * m.e + f * m.h, d * m.c + e * m.f + f * m.i,
      g * m.a + h * m.d + i * m.g, g * m.b + h * m.e + i * m.h,
      g * m.c + h * m.f + i * m.i);
}

CFX_Vector_3by1 CFX_Matrix_3by3::TransformVector(const CFX_Vector_3by1& v) {
  return CFX_Vector_3by1(a * v.a + b * v.b + c * v.c,
                         d * v.a + e * v.b + f * v.c,
                         g * v.a + h * v.b + i * v.c);
}

uint32_t GetBits32(const uint8_t* pData, int bitpos, int nbits) {
  ASSERT(0 < nbits && nbits <= 32);
  const uint8_t* dataPtr = &pData[bitpos / 8];
  int bitShift;
  int bitMask;
  int dstShift;
  int bitCount = bitpos & 0x07;
  if (nbits < 8 && nbits + bitCount <= 8) {
    bitShift = 8 - nbits - bitCount;
    bitMask = (1 << nbits) - 1;
    dstShift = 0;
  } else {
    bitShift = 0;
    int bitOffset = 8 - bitCount;
    bitMask = (1 << std::min(bitOffset, nbits)) - 1;
    dstShift = nbits - bitOffset;
  }
  uint32_t result = (uint32_t)(*dataPtr++ >> bitShift & bitMask) << dstShift;
  while (dstShift >= 8) {
    dstShift -= 8;
    result |= *dataPtr++ << dstShift;
  }
  if (dstShift > 0) {
    bitShift = 8 - dstShift;
    bitMask = (1 << dstShift) - 1;
    result |= *dataPtr++ >> bitShift & bitMask;
  }
  return result;
}
