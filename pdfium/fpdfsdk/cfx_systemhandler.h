// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CFX_SYSTEMHANDLER_H_
#define FPDFSDK_CFX_SYSTEMHANDLER_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

using FX_HWND = void*;
using FX_HMENU = void*;
using TimerCallback = void (*)(int32_t idEvent);

struct FX_SYSTEMTIME {
  FX_SYSTEMTIME()
      : wYear(0),
        wMonth(0),
        wDayOfWeek(0),
        wDay(0),
        wHour(0),
        wMinute(0),
        wSecond(0),
        wMilliseconds(0) {}

  uint16_t wYear;
  uint16_t wMonth;
  uint16_t wDayOfWeek;
  uint16_t wDay;
  uint16_t wHour;
  uint16_t wMinute;
  uint16_t wSecond;
  uint16_t wMilliseconds;
};

// Cursor style. These must match the values in public/fpdf_formfill.h
#define FXCT_ARROW 0
#define FXCT_NESW 1
#define FXCT_NWSE 2
#define FXCT_VBEAM 3
#define FXCT_HBEAM 4
#define FXCT_HAND 5

class CPDF_Document;
class CPDF_Font;
class CPDFDoc_Environment;

class CFX_SystemHandler {
 public:
  explicit CFX_SystemHandler(CPDFDoc_Environment* pEnv) : m_pEnv(pEnv) {}
  ~CFX_SystemHandler() {}

  void InvalidateRect(FX_HWND hWnd, FX_RECT rect);
  void OutputSelectedRect(void* pFormFiller, CFX_FloatRect& rect);
  bool IsSelectionImplemented() const;

  void SetCursor(int32_t nCursorType);

  bool FindNativeTrueTypeFont(CFX_ByteString sFontFaceName);
  CPDF_Font* AddNativeTrueTypeFontToPDF(CPDF_Document* pDoc,
                                        CFX_ByteString sFontFaceName,
                                        uint8_t nCharset);
  int32_t SetTimer(int32_t uElapse, TimerCallback lpTimerFunc);
  void KillTimer(int32_t nID);
  bool IsSHIFTKeyDown(uint32_t nFlag) const;
  bool IsCTRLKeyDown(uint32_t nFlag) const;
  bool IsALTKeyDown(uint32_t nFlag) const;

  FX_SYSTEMTIME GetLocalTime();

 private:
  CPDFDoc_Environment* const m_pEnv;
};

#endif  // FPDFSDK_CFX_SYSTEMHANDLER_H_
