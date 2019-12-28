// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2010 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "xfa/fxbarcode/BC_Writer.h"
#include "xfa/fxbarcode/oned/BC_OneDimWriter.h"
#include "xfa/fxbarcode/oned/BC_OnedEAN13Writer.h"
#include "xfa/fxbarcode/oned/BC_OnedUPCAWriter.h"

CBC_OnedUPCAWriter::CBC_OnedUPCAWriter() {
  m_subWriter = nullptr;
  m_bLeftPadding = TRUE;
  m_bRightPadding = TRUE;
}
void CBC_OnedUPCAWriter::Init() {
  m_subWriter = new CBC_OnedEAN13Writer;
}
CBC_OnedUPCAWriter::~CBC_OnedUPCAWriter() {
  delete m_subWriter;
}

FX_BOOL CBC_OnedUPCAWriter::CheckContentValidity(
    const CFX_WideStringC& contents) {
  for (FX_STRSIZE i = 0; i < contents.GetLength(); ++i) {
    if (contents.GetAt(i) < '0' || contents.GetAt(i) > '9')
      return FALSE;
  }
  return TRUE;
}

CFX_WideString CBC_OnedUPCAWriter::FilterContents(
    const CFX_WideStringC& contents) {
  CFX_WideString filtercontents;
  FX_WCHAR ch;
  for (int32_t i = 0; i < contents.GetLength(); i++) {
    ch = contents.GetAt(i);
    if (ch > 175) {
      i++;
      continue;
    }
    if (ch >= '0' && ch <= '9') {
      filtercontents += ch;
    }
  }
  return filtercontents;
}
int32_t CBC_OnedUPCAWriter::CalcChecksum(const CFX_ByteString& contents) {
  int32_t odd = 0;
  int32_t even = 0;
  int32_t j = 1;
  for (int32_t i = contents.GetLength() - 1; i >= 0; i--) {
    if (j % 2) {
      odd += FXSYS_atoi(contents.Mid(i, 1).c_str());
    } else {
      even += FXSYS_atoi(contents.Mid(i, 1).c_str());
    }
    j++;
  }
  int32_t checksum = (odd * 3 + even) % 10;
  checksum = (10 - checksum) % 10;
  return (checksum);
}
uint8_t* CBC_OnedUPCAWriter::Encode(const CFX_ByteString& contents,
                                    BCFORMAT format,
                                    int32_t& outWidth,
                                    int32_t& outHeight,
                                    int32_t& e) {
  uint8_t* ret = Encode(contents, format, outWidth, outHeight, 0, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return ret;
}
uint8_t* CBC_OnedUPCAWriter::Encode(const CFX_ByteString& contents,
                                    BCFORMAT format,
                                    int32_t& outWidth,
                                    int32_t& outHeight,
                                    int32_t hints,
                                    int32_t& e) {
  if (format != BCFORMAT_UPC_A) {
    e = BCExceptionOnlyEncodeUPC_A;
    return nullptr;
  }
  CFX_ByteString toEAN13String = '0' + contents;
  m_iDataLenth = 13;
  uint8_t* ret = m_subWriter->Encode(toEAN13String, BCFORMAT_EAN_13, outWidth,
                                     outHeight, hints, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return ret;
}

uint8_t* CBC_OnedUPCAWriter::Encode(const CFX_ByteString& contents,
                                    int32_t& outLength,
                                    int32_t& e) {
  return nullptr;
}

void CBC_OnedUPCAWriter::ShowChars(const CFX_WideStringC& contents,
                                   CFX_DIBitmap* pOutBitmap,
                                   CFX_RenderDevice* device,
                                   const CFX_Matrix* matrix,
                                   int32_t barWidth,
                                   int32_t multiple,
                                   int32_t& e) {
  if (!device && !pOutBitmap) {
    e = BCExceptionIllegalArgument;
    return;
  }

  int32_t leftPadding = 7 * multiple;
  int32_t leftPosition = 10 * multiple + leftPadding;
  CFX_ByteString str = FX_UTF8Encode(contents);
  int32_t iLen = str.GetLength();
  FXTEXT_CHARPOS* pCharPos = FX_Alloc(FXTEXT_CHARPOS, iLen);
  FXSYS_memset(pCharPos, 0, sizeof(FXTEXT_CHARPOS) * iLen);
  CFX_ByteString tempStr = str.Mid(1, 5);
  FX_FLOAT strWidth = (FX_FLOAT)35 * multiple;
  FX_FLOAT blank = 0.0;
  CFX_FxgeDevice geBitmap;
  if (pOutBitmap)
    geBitmap.Attach(pOutBitmap, false, nullptr, false);

  iLen = tempStr.GetLength();
  int32_t iFontSize = (int32_t)fabs(m_fFontSize);
  int32_t iTextHeight = iFontSize + 1;
  if (!pOutBitmap) {
    CFX_Matrix matr(m_outputHScale, 0.0, 0.0, 1.0, 0.0, 0.0);
    CFX_FloatRect rect(
        (FX_FLOAT)leftPosition, (FX_FLOAT)(m_Height - iTextHeight),
        (FX_FLOAT)(leftPosition + strWidth - 0.5), (FX_FLOAT)m_Height);
    matr.Concat(*matrix);
    matr.TransformRect(rect);
    FX_RECT re = rect.GetOutterRect();
    device->FillRect(&re, m_backgroundColor);
    CFX_Matrix matr1(m_outputHScale, 0.0, 0.0, 1.0, 0.0, 0.0);
    CFX_FloatRect rect1(
        (FX_FLOAT)(leftPosition + 40 * multiple),
        (FX_FLOAT)(m_Height - iTextHeight),
        (FX_FLOAT)((leftPosition + 40 * multiple) + strWidth - 0.5),
        (FX_FLOAT)m_Height);
    matr1.Concat(*matrix);
    matr1.TransformRect(rect1);
    re = rect1.GetOutterRect();
    device->FillRect(&re, m_backgroundColor);
    FX_FLOAT strWidth1 = (FX_FLOAT)multiple * 7;
    CFX_Matrix matr2(m_outputHScale, 0.0, 0.0, 1.0, 0.0, 0.0);
    CFX_FloatRect rect2(0.0, (FX_FLOAT)(m_Height - iTextHeight),
                        (FX_FLOAT)strWidth1 - 1, (FX_FLOAT)m_Height);
    matr2.Concat(*matrix);
    matr2.TransformRect(rect2);
    re = rect2.GetOutterRect();
    device->FillRect(&re, m_backgroundColor);
    CFX_Matrix matr3(m_outputHScale, 0.0, 0.0, 1.0, 0.0, 0.0);
    CFX_FloatRect rect3(
        (FX_FLOAT)(leftPosition + 85 * multiple),
        (FX_FLOAT)(m_Height - iTextHeight),
        (FX_FLOAT)((leftPosition + 85 * multiple) + strWidth1 - 0.5),
        (FX_FLOAT)m_Height);
    matr3.Concat(*matrix);
    matr3.TransformRect(rect3);
    re = rect3.GetOutterRect();
    device->FillRect(&re, m_backgroundColor);
  }
  if (!pOutBitmap)
    strWidth = strWidth * m_outputHScale;

  CalcTextInfo(tempStr, pCharPos + 1, m_pFont, strWidth, iFontSize, blank);
  CFX_Matrix affine_matrix(1.0, 0.0, 0.0, -1.0, 0.0, (FX_FLOAT)iFontSize);
  CFX_FxgeDevice ge;
  if (pOutBitmap) {
    ge.Create((int)strWidth, iTextHeight, FXDIB_Argb, nullptr);
    ge.GetBitmap()->Clear(m_backgroundColor);
    ge.DrawNormalText(iLen, pCharPos + 1, m_pFont,
                      CFX_GEModule::Get()->GetFontCache(), (FX_FLOAT)iFontSize,
                      &affine_matrix, m_fontColor, FXTEXT_CLEARTYPE);
    geBitmap.SetDIBits(ge.GetBitmap(), leftPosition, m_Height - iTextHeight);
  } else {
    CFX_Matrix affine_matrix1(1.0, 0.0, 0.0, -1.0,
                              (FX_FLOAT)leftPosition * m_outputHScale,
                              (FX_FLOAT)(m_Height - iTextHeight + iFontSize));
    if (matrix) {
      affine_matrix1.Concat(*matrix);
    }
    device->DrawNormalText(
        iLen, pCharPos + 1, m_pFont, CFX_GEModule::Get()->GetFontCache(),
        (FX_FLOAT)iFontSize, &affine_matrix1, m_fontColor, FXTEXT_CLEARTYPE);
  }
  tempStr = str.Mid(6, 5);
  iLen = tempStr.GetLength();
  CalcTextInfo(tempStr, pCharPos + 6, m_pFont, strWidth, iFontSize, blank);
  if (pOutBitmap) {
    FX_RECT rect2(0, 0, (int)strWidth, iTextHeight);
    ge.FillRect(&rect2, m_backgroundColor);
    ge.DrawNormalText(iLen, pCharPos + 6, m_pFont,
                      CFX_GEModule::Get()->GetFontCache(), (FX_FLOAT)iFontSize,
                      &affine_matrix, m_fontColor, FXTEXT_CLEARTYPE);
    geBitmap.SetDIBits(ge.GetBitmap(), leftPosition + 40 * multiple,
                       m_Height - iTextHeight);
  } else {
    CFX_Matrix affine_matrix1(
        1.0, 0.0, 0.0, -1.0,
        (FX_FLOAT)(leftPosition + 40 * multiple) * m_outputHScale,
        (FX_FLOAT)(m_Height - iTextHeight + iFontSize));
    if (matrix) {
      affine_matrix1.Concat(*matrix);
    }
    device->DrawNormalText(
        iLen, pCharPos + 6, m_pFont, CFX_GEModule::Get()->GetFontCache(),
        (FX_FLOAT)iFontSize, &affine_matrix1, m_fontColor, FXTEXT_CLEARTYPE);
  }
  tempStr = str.Mid(0, 1);
  iLen = tempStr.GetLength();
  strWidth = (FX_FLOAT)multiple * 7;
  if (!pOutBitmap)
    strWidth = strWidth * m_outputHScale;

  CalcTextInfo(tempStr, pCharPos, m_pFont, strWidth, iFontSize, blank);
  if (pOutBitmap) {
    delete ge.GetBitmap();
    ge.Create((int)strWidth, iTextHeight, FXDIB_Argb, nullptr);
    ge.GetBitmap()->Clear(m_backgroundColor);
    ge.DrawNormalText(iLen, pCharPos, m_pFont,
                      CFX_GEModule::Get()->GetFontCache(), (FX_FLOAT)iFontSize,
                      &affine_matrix, m_fontColor, FXTEXT_CLEARTYPE);
    geBitmap.SetDIBits(ge.GetBitmap(), 0, m_Height - iTextHeight);
  } else {
    CFX_Matrix affine_matrix1(1.0, 0.0, 0.0, -1.0, 0,
                              (FX_FLOAT)(m_Height - iTextHeight + iFontSize));
    if (matrix) {
      affine_matrix1.Concat(*matrix);
    }
    device->DrawNormalText(
        iLen, pCharPos, m_pFont, CFX_GEModule::Get()->GetFontCache(),
        (FX_FLOAT)iFontSize, &affine_matrix1, m_fontColor, FXTEXT_CLEARTYPE);
  }
  tempStr = str.Mid(11, 1);
  iLen = tempStr.GetLength();
  CalcTextInfo(tempStr, pCharPos + 11, m_pFont, strWidth, iFontSize, blank);
  if (pOutBitmap) {
    delete ge.GetBitmap();
    ge.Create((int)strWidth, iTextHeight, FXDIB_Argb, nullptr);
    ge.GetBitmap()->Clear(m_backgroundColor);
    ge.DrawNormalText(iLen, pCharPos + 11, m_pFont,
                      CFX_GEModule::Get()->GetFontCache(), (FX_FLOAT)iFontSize,
                      &affine_matrix, m_fontColor, FXTEXT_CLEARTYPE);
    geBitmap.SetDIBits(ge.GetBitmap(), leftPosition + 85 * multiple,
                       m_Height - iTextHeight);
  } else {
    CFX_Matrix affine_matrix1(
        1.0, 0.0, 0.0, -1.0,
        (FX_FLOAT)(leftPosition + 85 * multiple) * m_outputHScale,
        (FX_FLOAT)(m_Height - iTextHeight + iFontSize));
    if (matrix) {
      affine_matrix1.Concat(*matrix);
    }
    device->DrawNormalText(
        iLen, pCharPos + 11, m_pFont, CFX_GEModule::Get()->GetFontCache(),
        (FX_FLOAT)iFontSize, &affine_matrix1, m_fontColor, FXTEXT_CLEARTYPE);
  }
  FX_Free(pCharPos);
}

void CBC_OnedUPCAWriter::RenderResult(const CFX_WideStringC& contents,
                                      uint8_t* code,
                                      int32_t codeLength,
                                      FX_BOOL isDevice,
                                      int32_t& e) {
  CBC_OneDimWriter::RenderResult(contents, code, codeLength, isDevice, e);
}
