// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_FONT_TTGSUBTABLE_H_
#define CORE_FPDFAPI_FPDF_FONT_TTGSUBTABLE_H_

#include <stdint.h>

#include <map>
#include <memory>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxge/include/fx_font.h"
#include "core/fxge/include/fx_freetype.h"

class CFX_GlyphMap {
 public:
  CFX_GlyphMap();
  ~CFX_GlyphMap();

  void SetAt(int key, int value);
  FX_BOOL Lookup(int key, int& value);

 protected:
  CFX_BinaryBuf m_Buffer;
};

class CFX_CTTGSUBTable {
 public:
  CFX_CTTGSUBTable();
  explicit CFX_CTTGSUBTable(FT_Bytes gsub);
  virtual ~CFX_CTTGSUBTable();

  bool IsOk() const;
  bool LoadGSUBTable(FT_Bytes gsub);
  bool GetVerticalGlyph(uint32_t glyphnum, uint32_t* vglyphnum);

 private:
  struct tt_gsub_header {
    uint32_t Version;
    uint16_t ScriptList;
    uint16_t FeatureList;
    uint16_t LookupList;
  };
  struct TLangSys {
    TLangSys();
    ~TLangSys();

    uint16_t LookupOrder;
    uint16_t ReqFeatureIndex;
    uint16_t FeatureCount;
    std::unique_ptr<uint16_t[]> FeatureIndex;

   private:
    TLangSys(const TLangSys&);
    TLangSys& operator=(const TLangSys&);
  };
  struct TLangSysRecord {
    TLangSysRecord() : LangSysTag(0) {}

    uint32_t LangSysTag;
    TLangSys LangSys;

   private:
    TLangSysRecord(const TLangSysRecord&);
    TLangSysRecord& operator=(const TLangSysRecord&);
  };
  struct TScript {
    TScript();
    ~TScript();

    uint16_t DefaultLangSys;
    uint16_t LangSysCount;
    std::unique_ptr<TLangSysRecord[]> LangSysRecord;

   private:
    TScript(const TScript&);
    TScript& operator=(const TScript&);
  };
  struct TScriptRecord {
    TScriptRecord() : ScriptTag(0) {}

    uint32_t ScriptTag;
    TScript Script;

   private:
    TScriptRecord(const TScriptRecord&);
    TScriptRecord& operator=(const TScriptRecord&);
  };
  struct TScriptList {
    TScriptList();
    ~TScriptList();

    uint16_t ScriptCount;
    std::unique_ptr<TScriptRecord[]> ScriptRecord;

   private:
    TScriptList(const TScriptList&);
    TScriptList& operator=(const TScriptList&);
  };
  struct TFeature {
    TFeature();
    ~TFeature();

    uint16_t FeatureParams;
    int LookupCount;
    std::unique_ptr<uint16_t[]> LookupListIndex;

   private:
    TFeature(const TFeature&);
    TFeature& operator=(const TFeature&);
  };
  struct TFeatureRecord {
    TFeatureRecord() : FeatureTag(0) {}

    uint32_t FeatureTag;
    TFeature Feature;

   private:
    TFeatureRecord(const TFeatureRecord&);
    TFeatureRecord& operator=(const TFeatureRecord&);
  };
  struct TFeatureList {
    TFeatureList();
    ~TFeatureList();

    int FeatureCount;
    std::unique_ptr<TFeatureRecord[]> FeatureRecord;

   private:
    TFeatureList(const TFeatureList&);
    TFeatureList& operator=(const TFeatureList&);
  };
  enum TLookupFlag {
    LOOKUPFLAG_RightToLeft = 0x0001,
    LOOKUPFLAG_IgnoreBaseGlyphs = 0x0002,
    LOOKUPFLAG_IgnoreLigatures = 0x0004,
    LOOKUPFLAG_IgnoreMarks = 0x0008,
    LOOKUPFLAG_Reserved = 0x00F0,
    LOOKUPFLAG_MarkAttachmentType = 0xFF00,
  };
  struct TCoverageFormatBase {
    TCoverageFormatBase() : CoverageFormat(0) {}
    explicit TCoverageFormatBase(uint16_t format) : CoverageFormat(format) {}
    virtual ~TCoverageFormatBase() {}

    uint16_t CoverageFormat;
    CFX_GlyphMap m_glyphMap;

   private:
    TCoverageFormatBase(const TCoverageFormatBase&);
    TCoverageFormatBase& operator=(const TCoverageFormatBase&);
  };
  struct TCoverageFormat1 : public TCoverageFormatBase {
    TCoverageFormat1();
    ~TCoverageFormat1() override;

    uint16_t GlyphCount;
    std::unique_ptr<uint16_t[]> GlyphArray;

   private:
    TCoverageFormat1(const TCoverageFormat1&);
    TCoverageFormat1& operator=(const TCoverageFormat1&);
  };
  struct TRangeRecord {
    TRangeRecord();

    friend bool operator>(const TRangeRecord& r1, const TRangeRecord& r2) {
      return r1.Start > r2.Start;
    }

    uint16_t Start;
    uint16_t End;
    uint16_t StartCoverageIndex;

   private:
    TRangeRecord(const TRangeRecord&);
  };
  struct TCoverageFormat2 : public TCoverageFormatBase {
    TCoverageFormat2();
    ~TCoverageFormat2() override;

    uint16_t RangeCount;
    std::unique_ptr<TRangeRecord[]> RangeRecord;

   private:
    TCoverageFormat2(const TCoverageFormat2&);
    TCoverageFormat2& operator=(const TCoverageFormat2&);
  };
  struct TDevice {
    TDevice() : StartSize(0), EndSize(0), DeltaFormat(0) {}

    uint16_t StartSize;
    uint16_t EndSize;
    uint16_t DeltaFormat;

   private:
    TDevice(const TDevice&);
    TDevice& operator=(const TDevice&);
  };
  struct TSubTableBase {
    TSubTableBase() : SubstFormat(0) {}
    explicit TSubTableBase(uint16_t format) : SubstFormat(format) {}
    virtual ~TSubTableBase() {}

    uint16_t SubstFormat;

   private:
    TSubTableBase(const TSubTableBase&);
    TSubTableBase& operator=(const TSubTableBase&);
  };
  struct TSingleSubstFormat1 : public TSubTableBase {
    TSingleSubstFormat1();
    ~TSingleSubstFormat1() override;

    std::unique_ptr<TCoverageFormatBase> Coverage;
    int16_t DeltaGlyphID;

   private:
    TSingleSubstFormat1(const TSingleSubstFormat1&);
    TSingleSubstFormat1& operator=(const TSingleSubstFormat1&);
  };
  struct TSingleSubstFormat2 : public TSubTableBase {
    TSingleSubstFormat2();
    ~TSingleSubstFormat2() override;

    std::unique_ptr<TCoverageFormatBase> Coverage;
    uint16_t GlyphCount;
    std::unique_ptr<uint16_t[]> Substitute;

   private:
    TSingleSubstFormat2(const TSingleSubstFormat2&);
    TSingleSubstFormat2& operator=(const TSingleSubstFormat2&);
  };
  struct TLookup {
    TLookup();
    ~TLookup();

    uint16_t LookupType;
    uint16_t LookupFlag;
    uint16_t SubTableCount;
    std::unique_ptr<TSubTableBase* []> SubTable;

   private:
    TLookup(const TLookup&);
    TLookup& operator=(const TLookup&);
  };
  struct TLookupList {
    TLookupList();
    ~TLookupList();

    int LookupCount;
    std::unique_ptr<TLookup[]> Lookup;

   private:
    TLookupList(const TLookupList&);
    TLookupList& operator=(const TLookupList&);
  };

  bool Parse(FT_Bytes scriptlist, FT_Bytes featurelist, FT_Bytes lookuplist);
  void ParseScriptList(FT_Bytes raw, TScriptList* rec);
  void ParseScript(FT_Bytes raw, TScript* rec);
  void ParseLangSys(FT_Bytes raw, TLangSys* rec);
  void ParseFeatureList(FT_Bytes raw, TFeatureList* rec);
  void ParseFeature(FT_Bytes raw, TFeature* rec);
  void ParseLookupList(FT_Bytes raw, TLookupList* rec);
  void ParseLookup(FT_Bytes raw, TLookup* rec);
  TCoverageFormatBase* ParseCoverage(FT_Bytes raw);
  void ParseCoverageFormat1(FT_Bytes raw, TCoverageFormat1* rec);
  void ParseCoverageFormat2(FT_Bytes raw, TCoverageFormat2* rec);
  void ParseSingleSubst(FT_Bytes raw, TSubTableBase** rec);
  void ParseSingleSubstFormat1(FT_Bytes raw, TSingleSubstFormat1* rec);
  void ParseSingleSubstFormat2(FT_Bytes raw, TSingleSubstFormat2* rec);

  bool GetVerticalGlyphSub(uint32_t glyphnum,
                           uint32_t* vglyphnum,
                           TFeature* Feature) const;
  bool GetVerticalGlyphSub2(uint32_t glyphnum,
                            uint32_t* vglyphnum,
                            TLookup* Lookup) const;
  int GetCoverageIndex(TCoverageFormatBase* Coverage, uint32_t g) const;

  uint8_t GetUInt8(FT_Bytes& p) const;
  int16_t GetInt16(FT_Bytes& p) const;
  uint16_t GetUInt16(FT_Bytes& p) const;
  int32_t GetInt32(FT_Bytes& p) const;
  uint32_t GetUInt32(FT_Bytes& p) const;

  std::map<uint32_t, uint32_t> m_featureMap;
  FX_BOOL m_bFeautureMapLoad;
  bool loaded;
  tt_gsub_header header;
  TScriptList ScriptList;
  TFeatureList FeatureList;
  TLookupList LookupList;
};

#endif  // CORE_FPDFAPI_FPDF_FONT_TTGSUBTABLE_H_
