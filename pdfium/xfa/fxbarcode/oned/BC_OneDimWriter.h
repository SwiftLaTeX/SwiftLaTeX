// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_ONED_BC_ONEDIMWRITER_H_
#define XFA_FXBARCODE_ONED_BC_ONEDIMWRITER_H_

#include "core/fxge/include/fx_ge.h"
#include "xfa/fxbarcode/BC_Writer.h"
#include "xfa/fxbarcode/include/BC_Library.h"

class CBC_CommonBitMatrix;
class CFX_Font;
class CFX_RenderDevice;

class CBC_OneDimWriter : public CBC_Writer {
 public:
  CBC_OneDimWriter();
  ~CBC_OneDimWriter() override;

  virtual uint8_t* Encode(const CFX_ByteString& contents,
                          BCFORMAT format,
                          int32_t& outWidth,
                          int32_t& outHeight,
                          int32_t& e);
  virtual uint8_t* Encode(const CFX_ByteString& contents,
                          BCFORMAT format,
                          int32_t& outWidth,
                          int32_t& outHeight,
                          int32_t hints,
                          int32_t& e);
  virtual uint8_t* Encode(const CFX_ByteString& contents,
                          int32_t& outLength,
                          int32_t& e);

  virtual void RenderResult(const CFX_WideStringC& contents,
                            uint8_t* code,
                            int32_t codeLength,
                            FX_BOOL isDevice,
                            int32_t& e);
  virtual void RenderBitmapResult(CFX_DIBitmap*& pOutBitmap,
                                  const CFX_WideStringC& contents,
                                  int32_t& e);
  virtual void RenderDeviceResult(CFX_RenderDevice* device,
                                  const CFX_Matrix* matrix,
                                  const CFX_WideStringC& contents,
                                  int32_t& e);
  virtual FX_BOOL CheckContentValidity(const CFX_WideStringC& contents);
  virtual CFX_WideString FilterContents(const CFX_WideStringC& contents);
  virtual CFX_WideString RenderTextContents(const CFX_WideStringC& contents);
  virtual void SetPrintChecksum(FX_BOOL checksum);
  virtual void SetDataLength(int32_t length);
  virtual void SetCalcChecksum(int32_t state);
  virtual void SetFontSize(FX_FLOAT size);
  virtual void SetFontStyle(int32_t style);
  virtual void SetFontColor(FX_ARGB color);
  FX_BOOL SetFont(CFX_Font* cFont);

 protected:
  virtual void CalcTextInfo(const CFX_ByteString& text,
                            FXTEXT_CHARPOS* charPos,
                            CFX_Font* cFont,
                            FX_FLOAT geWidth,
                            int32_t fontSize,
                            FX_FLOAT& charsLen);
  virtual void ShowChars(const CFX_WideStringC& contents,
                         CFX_DIBitmap* pOutBitmap,
                         CFX_RenderDevice* device,
                         const CFX_Matrix* matrix,
                         int32_t barWidth,
                         int32_t multiple,
                         int32_t& e);
  virtual void ShowBitmapChars(CFX_DIBitmap* pOutBitmap,
                               const CFX_ByteString str,
                               FX_FLOAT geWidth,
                               FXTEXT_CHARPOS* pCharPos,
                               FX_FLOAT locX,
                               FX_FLOAT locY,
                               int32_t barWidth);
  virtual void ShowDeviceChars(CFX_RenderDevice* device,
                               const CFX_Matrix* matrix,
                               const CFX_ByteString str,
                               FX_FLOAT geWidth,
                               FXTEXT_CHARPOS* pCharPos,
                               FX_FLOAT locX,
                               FX_FLOAT locY,
                               int32_t barWidth);
  virtual int32_t AppendPattern(uint8_t* target,
                                int32_t pos,
                                const int32_t* pattern,
                                int32_t patternLength,
                                int32_t startColor,
                                int32_t& e);

  FX_WCHAR Upper(FX_WCHAR ch);

  FX_BOOL m_bPrintChecksum;
  int32_t m_iDataLenth;
  FX_BOOL m_bCalcChecksum;
  CFX_Font* m_pFont;
  FX_FLOAT m_fFontSize;
  int32_t m_iFontStyle;
  uint32_t m_fontColor;
  BC_TEXT_LOC m_locTextLoc;
  int32_t m_iContentLen;
  FX_BOOL m_bLeftPadding;
  FX_BOOL m_bRightPadding;
  CBC_CommonBitMatrix* m_output;
  int32_t m_barWidth;
  int32_t m_multiple;
  FX_FLOAT m_outputHScale;
};

#endif  // XFA_FXBARCODE_ONED_BC_ONEDIMWRITER_H_
