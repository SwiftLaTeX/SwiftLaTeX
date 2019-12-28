// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_INCLUDE_FX_GE_WIN32_H_
#define CORE_FXGE_INCLUDE_FX_GE_WIN32_H_

#ifdef _WIN32
#ifndef _WINDOWS_
#include <windows.h>
#endif
#define WINDIB_OPEN_MEMORY 0x1
#define WINDIB_OPEN_PATHNAME 0x2

class IFX_RenderDeviceDriver;

typedef struct WINDIB_Open_Args_ {
  int flags;

  const uint8_t* memory_base;

  size_t memory_size;

  const FX_WCHAR* path_name;
} WINDIB_Open_Args_;

class CFX_WindowsDIB : public CFX_DIBitmap {
 public:
  CFX_WindowsDIB(HDC hDC, int width, int height);
  ~CFX_WindowsDIB() override;

  static CFX_ByteString GetBitmapInfo(const CFX_DIBitmap* pBitmap);
  static CFX_DIBitmap* LoadFromBuf(BITMAPINFO* pbmi, void* pData);
  static HBITMAP GetDDBitmap(const CFX_DIBitmap* pBitmap, HDC hDC);
  static CFX_DIBitmap* LoadFromDDB(HDC hDC,
                                   HBITMAP hBitmap,
                                   uint32_t* pPalette = nullptr,
                                   uint32_t size = 256);
  static CFX_DIBitmap* LoadFromFile(const FX_WCHAR* filename);
  static CFX_DIBitmap* LoadFromFile(const FX_CHAR* filename);
  static CFX_DIBitmap* LoadDIBitmap(WINDIB_Open_Args_ args);

  HDC GetDC() const { return m_hMemDC; }
  HBITMAP GetWindowsBitmap() const { return m_hBitmap; }

  void LoadFromDevice(HDC hDC, int left, int top);
  void SetToDevice(HDC hDC, int left, int top);

 protected:
  HDC m_hMemDC;
  HBITMAP m_hBitmap;
  HBITMAP m_hOldBitmap;
};

class CFX_WindowsDevice : public CFX_RenderDevice {
 public:
  static IFX_RenderDeviceDriver* CreateDriver(HDC hDC);

  explicit CFX_WindowsDevice(HDC hDC);
  ~CFX_WindowsDevice() override;

  HDC GetDC() const;
};

class CFX_WinBitmapDevice : public CFX_RenderDevice {
 public:
  CFX_WinBitmapDevice(int width, int height, FXDIB_Format format);
  ~CFX_WinBitmapDevice() override;

  HDC GetDC() { return m_hDC; }

 protected:
  HBITMAP m_hBitmap;

  HBITMAP m_hOldBitmap;

  HDC m_hDC;
};
#endif

#endif  // CORE_FXGE_INCLUDE_FX_GE_WIN32_H_
