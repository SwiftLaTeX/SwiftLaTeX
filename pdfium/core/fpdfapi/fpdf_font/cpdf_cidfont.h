// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_FONT_CPDF_CIDFONT_H_
#define CORE_FPDFAPI_FPDF_FONT_CPDF_CIDFONT_H_

#include <memory>

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

enum CIDSet {
  CIDSET_UNKNOWN,
  CIDSET_GB1,
  CIDSET_CNS1,
  CIDSET_JAPAN1,
  CIDSET_KOREA1,
  CIDSET_UNICODE,
  CIDSET_NUM_SETS
};

class CFX_CTTGSUBTable;
class CPDF_Array;
class CPDF_CID2UnicodeMap;
class CPDF_CMap;
class CPDF_StreamAcc;

class CPDF_CIDFont : public CPDF_Font {
 public:
  CPDF_CIDFont();
  ~CPDF_CIDFont() override;

  static FX_FLOAT CIDTransformToFloat(uint8_t ch);

  // CPDF_Font:
  bool IsCIDFont() const override;
  const CPDF_CIDFont* AsCIDFont() const override;
  CPDF_CIDFont* AsCIDFont() override;
  int GlyphFromCharCode(uint32_t charcode, bool* pVertGlyph) override;
  int GetCharWidthF(uint32_t charcode, int level = 0) override;
  FX_RECT GetCharBBox(uint32_t charcode, int level = 0) override;
  uint32_t GetNextChar(const FX_CHAR* pString,
                       int nStrLen,
                       int& offset) const override;
  int CountChar(const FX_CHAR* pString, int size) const override;
  int AppendChar(FX_CHAR* str, uint32_t charcode) const override;
  int GetCharSize(uint32_t charcode) const override;
  FX_BOOL IsVertWriting() const override;
  FX_BOOL IsUnicodeCompatible() const override;
  FX_BOOL Load() override;
  CFX_WideString UnicodeFromCharCode(uint32_t charcode) const override;
  uint32_t CharCodeFromUnicode(FX_WCHAR Unicode) const override;

  uint16_t CIDFromCharCode(uint32_t charcode) const;
  const uint8_t* GetCIDTransform(uint16_t CID) const;
  short GetVertWidth(uint16_t CID) const;
  void GetVertOrigin(uint16_t CID, short& vx, short& vy) const;

 protected:
  void LoadGB2312();
  int GetGlyphIndex(uint32_t unicodeb, bool* pVertGlyph);
  int GetVerticalGlyph(int index, bool* pVertGlyph);
  void LoadMetricsArray(CPDF_Array* pArray,
                        CFX_ArrayTemplate<uint32_t>& result,
                        int nElements);
  void LoadSubstFont();
  FX_WCHAR GetUnicodeFromCharCode(uint32_t charcode) const;

  CPDF_CMap* m_pCMap;
  std::unique_ptr<CPDF_CMap> m_pAllocatedCMap;
  CPDF_CID2UnicodeMap* m_pCID2UnicodeMap;
  CIDSet m_Charset;
  FX_BOOL m_bType1;
  FX_BOOL m_bCIDIsGID;
  uint16_t m_DefaultWidth;
  std::unique_ptr<CPDF_StreamAcc> m_pStreamAcc;
  bool m_bAnsiWidthsFixed;
  FX_SMALL_RECT m_CharBBox[256];
  CFX_ArrayTemplate<uint32_t> m_WidthList;
  short m_DefaultVY;
  short m_DefaultW1;
  CFX_ArrayTemplate<uint32_t> m_VertMetrics;
  FX_BOOL m_bAdobeCourierStd;
  std::unique_ptr<CFX_CTTGSUBTable> m_pTTGSUBTable;
};

#endif  // CORE_FPDFAPI_FPDF_FONT_CPDF_CIDFONT_H_
