// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_font/ttgsubtable.h"

#include "core/fxge/include/fx_freetype.h"
#include "core/fxge/include/fx_ge.h"
#include "third_party/base/stl_util.h"

CFX_GlyphMap::CFX_GlyphMap() {}

CFX_GlyphMap::~CFX_GlyphMap() {}

extern "C" {
static int _CompareInt(const void* p1, const void* p2) {
  return (*(uint32_t*)p1) - (*(uint32_t*)p2);
}
};

struct _IntPair {
  int32_t key;
  int32_t value;
};

void CFX_GlyphMap::SetAt(int key, int value) {
  uint32_t count = m_Buffer.GetSize() / sizeof(_IntPair);
  _IntPair* buf = (_IntPair*)m_Buffer.GetBuffer();
  _IntPair pair = {key, value};
  if (count == 0 || key > buf[count - 1].key) {
    m_Buffer.AppendBlock(&pair, sizeof(_IntPair));
    return;
  }
  int low = 0, high = count - 1;
  while (low <= high) {
    int mid = (low + high) / 2;
    if (buf[mid].key < key) {
      low = mid + 1;
    } else if (buf[mid].key > key) {
      high = mid - 1;
    } else {
      buf[mid].value = value;
      return;
    }
  }
  m_Buffer.InsertBlock(low * sizeof(_IntPair), &pair, sizeof(_IntPair));
}

FX_BOOL CFX_GlyphMap::Lookup(int key, int& value) {
  void* pResult = FXSYS_bsearch(&key, m_Buffer.GetBuffer(),
                                m_Buffer.GetSize() / sizeof(_IntPair),
                                sizeof(_IntPair), _CompareInt);
  if (!pResult) {
    return FALSE;
  }
  value = ((uint32_t*)pResult)[1];
  return TRUE;
}

CFX_CTTGSUBTable::CFX_CTTGSUBTable()
    : m_bFeautureMapLoad(FALSE), loaded(false) {}

CFX_CTTGSUBTable::CFX_CTTGSUBTable(FT_Bytes gsub)
    : m_bFeautureMapLoad(FALSE), loaded(false) {
  LoadGSUBTable(gsub);
}

CFX_CTTGSUBTable::~CFX_CTTGSUBTable() {}

bool CFX_CTTGSUBTable::IsOk() const {
  return loaded;
}

bool CFX_CTTGSUBTable::LoadGSUBTable(FT_Bytes gsub) {
  header.Version = gsub[0] << 24 | gsub[1] << 16 | gsub[2] << 8 | gsub[3];
  if (header.Version != 0x00010000) {
    return false;
  }
  header.ScriptList = gsub[4] << 8 | gsub[5];
  header.FeatureList = gsub[6] << 8 | gsub[7];
  header.LookupList = gsub[8] << 8 | gsub[9];
  return Parse(&gsub[header.ScriptList], &gsub[header.FeatureList],
               &gsub[header.LookupList]);
}

bool CFX_CTTGSUBTable::GetVerticalGlyph(uint32_t glyphnum,
                                        uint32_t* vglyphnum) {
  uint32_t tag[] = {
      (uint8_t)'v' << 24 | (uint8_t)'r' << 16 | (uint8_t)'t' << 8 |
          (uint8_t)'2',
      (uint8_t)'v' << 24 | (uint8_t)'e' << 16 | (uint8_t)'r' << 8 |
          (uint8_t)'t',
  };
  if (!m_bFeautureMapLoad) {
    for (int i = 0; i < ScriptList.ScriptCount; i++) {
      for (int j = 0; j < ScriptList.ScriptRecord[i].Script.LangSysCount; ++j) {
        const auto& record = ScriptList.ScriptRecord[i].Script.LangSysRecord[j];
        for (int k = 0; k < record.LangSys.FeatureCount; ++k) {
          uint32_t index = record.LangSys.FeatureIndex[k];
          if (FeatureList.FeatureRecord[index].FeatureTag == tag[0] ||
              FeatureList.FeatureRecord[index].FeatureTag == tag[1]) {
            if (!pdfium::ContainsKey(m_featureMap, index)) {
              m_featureMap[index] = index;
            }
          }
        }
      }
    }
    if (m_featureMap.empty()) {
      for (int i = 0; i < FeatureList.FeatureCount; i++) {
        if (FeatureList.FeatureRecord[i].FeatureTag == tag[0] ||
            FeatureList.FeatureRecord[i].FeatureTag == tag[1]) {
          m_featureMap[i] = i;
        }
      }
    }
    m_bFeautureMapLoad = TRUE;
  }
  for (const auto& pair : m_featureMap) {
    if (GetVerticalGlyphSub(glyphnum, vglyphnum,
                            &FeatureList.FeatureRecord[pair.second].Feature)) {
      return true;
    }
  }
  return false;
}

bool CFX_CTTGSUBTable::GetVerticalGlyphSub(uint32_t glyphnum,
                                           uint32_t* vglyphnum,
                                           TFeature* Feature) const {
  for (int i = 0; i < Feature->LookupCount; i++) {
    int index = Feature->LookupListIndex[i];
    if (index < 0 || LookupList.LookupCount < index) {
      continue;
    }
    if (LookupList.Lookup[index].LookupType == 1) {
      if (GetVerticalGlyphSub2(glyphnum, vglyphnum,
                               &LookupList.Lookup[index])) {
        return true;
      }
    }
  }
  return false;
}

bool CFX_CTTGSUBTable::GetVerticalGlyphSub2(uint32_t glyphnum,
                                            uint32_t* vglyphnum,
                                            TLookup* Lookup) const {
  for (int i = 0; i < Lookup->SubTableCount; i++) {
    switch (Lookup->SubTable[i]->SubstFormat) {
      case 1: {
        TSingleSubstFormat1* tbl1 = (TSingleSubstFormat1*)Lookup->SubTable[i];
        if (GetCoverageIndex(tbl1->Coverage.get(), glyphnum) >= 0) {
          *vglyphnum = glyphnum + tbl1->DeltaGlyphID;
          return true;
        }
        break;
      }
      case 2: {
        TSingleSubstFormat2* tbl2 = (TSingleSubstFormat2*)Lookup->SubTable[i];
        int index = -1;
        index = GetCoverageIndex(tbl2->Coverage.get(), glyphnum);
        if (0 <= index && index < tbl2->GlyphCount) {
          *vglyphnum = tbl2->Substitute[index];
          return true;
        }
        break;
      }
    }
  }
  return false;
}

int CFX_CTTGSUBTable::GetCoverageIndex(TCoverageFormatBase* Coverage,
                                       uint32_t g) const {
  int i = 0;
  if (!Coverage) {
    return -1;
  }
  switch (Coverage->CoverageFormat) {
    case 1: {
      TCoverageFormat1* c1 = (TCoverageFormat1*)Coverage;
      for (i = 0; i < c1->GlyphCount; i++) {
        if ((uint32_t)c1->GlyphArray[i] == g) {
          return i;
        }
      }
      return -1;
    }
    case 2: {
      TCoverageFormat2* c2 = (TCoverageFormat2*)Coverage;
      for (i = 0; i < c2->RangeCount; i++) {
        uint32_t s = c2->RangeRecord[i].Start;
        uint32_t e = c2->RangeRecord[i].End;
        uint32_t si = c2->RangeRecord[i].StartCoverageIndex;
        if (s <= g && g <= e) {
          return si + g - s;
        }
      }
      return -1;
    }
  }
  return -1;
}

uint8_t CFX_CTTGSUBTable::GetUInt8(FT_Bytes& p) const {
  uint8_t ret = p[0];
  p += 1;
  return ret;
}

int16_t CFX_CTTGSUBTable::GetInt16(FT_Bytes& p) const {
  uint16_t ret = p[0] << 8 | p[1];
  p += 2;
  return *(int16_t*)&ret;
}

uint16_t CFX_CTTGSUBTable::GetUInt16(FT_Bytes& p) const {
  uint16_t ret = p[0] << 8 | p[1];
  p += 2;
  return ret;
}

int32_t CFX_CTTGSUBTable::GetInt32(FT_Bytes& p) const {
  uint32_t ret = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
  p += 4;
  return *(int32_t*)&ret;
}

uint32_t CFX_CTTGSUBTable::GetUInt32(FT_Bytes& p) const {
  uint32_t ret = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
  p += 4;
  return ret;
}

bool CFX_CTTGSUBTable::Parse(FT_Bytes scriptlist,
                             FT_Bytes featurelist,
                             FT_Bytes lookuplist) {
  ParseScriptList(scriptlist, &ScriptList);
  ParseFeatureList(featurelist, &FeatureList);
  ParseLookupList(lookuplist, &LookupList);
  return true;
}

void CFX_CTTGSUBTable::ParseScriptList(FT_Bytes raw, TScriptList* rec) {
  int i;
  FT_Bytes sp = raw;
  rec->ScriptCount = GetUInt16(sp);
  if (rec->ScriptCount <= 0) {
    return;
  }
  rec->ScriptRecord.reset(new TScriptRecord[rec->ScriptCount]);
  for (i = 0; i < rec->ScriptCount; i++) {
    rec->ScriptRecord[i].ScriptTag = GetUInt32(sp);
    uint16_t offset = GetUInt16(sp);
    ParseScript(&raw[offset], &rec->ScriptRecord[i].Script);
  }
}

void CFX_CTTGSUBTable::ParseScript(FT_Bytes raw, TScript* rec) {
  int i;
  FT_Bytes sp = raw;
  rec->DefaultLangSys = GetUInt16(sp);
  rec->LangSysCount = GetUInt16(sp);
  if (rec->LangSysCount <= 0) {
    return;
  }
  rec->LangSysRecord.reset(new TLangSysRecord[rec->LangSysCount]);
  for (i = 0; i < rec->LangSysCount; i++) {
    rec->LangSysRecord[i].LangSysTag = GetUInt32(sp);
    uint16_t offset = GetUInt16(sp);
    ParseLangSys(&raw[offset], &rec->LangSysRecord[i].LangSys);
  }
}

void CFX_CTTGSUBTable::ParseLangSys(FT_Bytes raw, TLangSys* rec) {
  FT_Bytes sp = raw;
  rec->LookupOrder = GetUInt16(sp);
  rec->ReqFeatureIndex = GetUInt16(sp);
  rec->FeatureCount = GetUInt16(sp);
  if (rec->FeatureCount <= 0) {
    return;
  }
  rec->FeatureIndex.reset(new uint16_t[rec->FeatureCount]);
  FXSYS_memset(rec->FeatureIndex.get(), 0,
               sizeof(uint16_t) * rec->FeatureCount);
  for (int i = 0; i < rec->FeatureCount; ++i) {
    rec->FeatureIndex[i] = GetUInt16(sp);
  }
}

void CFX_CTTGSUBTable::ParseFeatureList(FT_Bytes raw, TFeatureList* rec) {
  int i;
  FT_Bytes sp = raw;
  rec->FeatureCount = GetUInt16(sp);
  if (rec->FeatureCount <= 0) {
    return;
  }
  rec->FeatureRecord.reset(new TFeatureRecord[rec->FeatureCount]);
  for (i = 0; i < rec->FeatureCount; i++) {
    rec->FeatureRecord[i].FeatureTag = GetUInt32(sp);
    uint16_t offset = GetUInt16(sp);
    ParseFeature(&raw[offset], &rec->FeatureRecord[i].Feature);
  }
}

void CFX_CTTGSUBTable::ParseFeature(FT_Bytes raw, TFeature* rec) {
  int i;
  FT_Bytes sp = raw;
  rec->FeatureParams = GetUInt16(sp);
  rec->LookupCount = GetUInt16(sp);
  if (rec->LookupCount <= 0) {
    return;
  }
  rec->LookupListIndex.reset(new uint16_t[rec->LookupCount]);
  for (i = 0; i < rec->LookupCount; i++) {
    rec->LookupListIndex[i] = GetUInt16(sp);
  }
}

void CFX_CTTGSUBTable::ParseLookupList(FT_Bytes raw, TLookupList* rec) {
  int i;
  FT_Bytes sp = raw;
  rec->LookupCount = GetUInt16(sp);
  if (rec->LookupCount <= 0) {
    return;
  }
  rec->Lookup.reset(new TLookup[rec->LookupCount]);
  for (i = 0; i < rec->LookupCount; i++) {
    uint16_t offset = GetUInt16(sp);
    ParseLookup(&raw[offset], &rec->Lookup[i]);
  }
}

void CFX_CTTGSUBTable::ParseLookup(FT_Bytes raw, TLookup* rec) {
  int i;
  FT_Bytes sp = raw;
  rec->LookupType = GetUInt16(sp);
  rec->LookupFlag = GetUInt16(sp);
  rec->SubTableCount = GetUInt16(sp);
  if (rec->SubTableCount <= 0) {
    return;
  }
  rec->SubTable.reset(new TSubTableBase*[rec->SubTableCount]);
  for (i = 0; i < rec->SubTableCount; i++) {
    rec->SubTable[i] = nullptr;
  }
  if (rec->LookupType != 1) {
    return;
  }
  for (i = 0; i < rec->SubTableCount; i++) {
    uint16_t offset = GetUInt16(sp);
    ParseSingleSubst(&raw[offset], &rec->SubTable[i]);
  }
}

CFX_CTTGSUBTable::TCoverageFormatBase* CFX_CTTGSUBTable::ParseCoverage(
    FT_Bytes raw) {
  FT_Bytes sp = raw;
  uint16_t format = GetUInt16(sp);
  TCoverageFormatBase* rec = nullptr;
  if (format == 1) {
    rec = new TCoverageFormat1();
    ParseCoverageFormat1(raw, static_cast<TCoverageFormat1*>(rec));
  } else if (format == 2) {
    rec = new TCoverageFormat2();
    ParseCoverageFormat2(raw, static_cast<TCoverageFormat2*>(rec));
  }
  return rec;
}

void CFX_CTTGSUBTable::ParseCoverageFormat1(FT_Bytes raw,
                                            TCoverageFormat1* rec) {
  int i;
  FT_Bytes sp = raw;
  GetUInt16(sp);
  rec->GlyphCount = GetUInt16(sp);
  if (rec->GlyphCount <= 0) {
    return;
  }
  rec->GlyphArray.reset(new uint16_t[rec->GlyphCount]);
  for (i = 0; i < rec->GlyphCount; i++) {
    rec->GlyphArray[i] = GetUInt16(sp);
  }
}

void CFX_CTTGSUBTable::ParseCoverageFormat2(FT_Bytes raw,
                                            TCoverageFormat2* rec) {
  int i;
  FT_Bytes sp = raw;
  GetUInt16(sp);
  rec->RangeCount = GetUInt16(sp);
  if (rec->RangeCount <= 0) {
    return;
  }
  rec->RangeRecord.reset(new TRangeRecord[rec->RangeCount]);
  for (i = 0; i < rec->RangeCount; i++) {
    rec->RangeRecord[i].Start = GetUInt16(sp);
    rec->RangeRecord[i].End = GetUInt16(sp);
    rec->RangeRecord[i].StartCoverageIndex = GetUInt16(sp);
  }
}

void CFX_CTTGSUBTable::ParseSingleSubst(FT_Bytes raw, TSubTableBase** rec) {
  FT_Bytes sp = raw;
  uint16_t Format = GetUInt16(sp);
  switch (Format) {
    case 1:
      *rec = new TSingleSubstFormat1();
      ParseSingleSubstFormat1(raw, (TSingleSubstFormat1*)*rec);
      break;
    case 2:
      *rec = new TSingleSubstFormat2();
      ParseSingleSubstFormat2(raw, (TSingleSubstFormat2*)*rec);
      break;
  }
}

void CFX_CTTGSUBTable::ParseSingleSubstFormat1(FT_Bytes raw,
                                               TSingleSubstFormat1* rec) {
  FT_Bytes sp = raw;
  GetUInt16(sp);
  uint16_t offset = GetUInt16(sp);
  rec->Coverage.reset(ParseCoverage(&raw[offset]));
  rec->DeltaGlyphID = GetInt16(sp);
}

void CFX_CTTGSUBTable::ParseSingleSubstFormat2(FT_Bytes raw,
                                               TSingleSubstFormat2* rec) {
  int i;
  FT_Bytes sp = raw;
  GetUInt16(sp);
  uint16_t offset = GetUInt16(sp);
  rec->Coverage.reset(ParseCoverage(&raw[offset]));
  rec->GlyphCount = GetUInt16(sp);
  if (rec->GlyphCount <= 0) {
    return;
  }
  rec->Substitute.reset(new uint16_t[rec->GlyphCount]);
  for (i = 0; i < rec->GlyphCount; i++) {
    rec->Substitute[i] = GetUInt16(sp);
  }
}

CFX_CTTGSUBTable::TCoverageFormat1::TCoverageFormat1()
    : TCoverageFormatBase(1), GlyphCount(0) {}

CFX_CTTGSUBTable::TCoverageFormat1::~TCoverageFormat1() {}

CFX_CTTGSUBTable::TRangeRecord::TRangeRecord()
    : Start(0), End(0), StartCoverageIndex(0) {}

CFX_CTTGSUBTable::TCoverageFormat2::TCoverageFormat2()
    : TCoverageFormatBase(2), RangeCount(0) {}

CFX_CTTGSUBTable::TCoverageFormat2::~TCoverageFormat2() {}

CFX_CTTGSUBTable::TSingleSubstFormat1::TSingleSubstFormat1()
    : TSubTableBase(1), DeltaGlyphID(0) {}

CFX_CTTGSUBTable::TSingleSubstFormat1::~TSingleSubstFormat1() {}

CFX_CTTGSUBTable::TSingleSubstFormat2::TSingleSubstFormat2()
    : TSubTableBase(2), GlyphCount(0) {}

CFX_CTTGSUBTable::TSingleSubstFormat2::~TSingleSubstFormat2() {}

CFX_CTTGSUBTable::TLookup::TLookup()
    : LookupType(0), LookupFlag(0), SubTableCount(0) {}

CFX_CTTGSUBTable::TLookup::~TLookup() {
  if (SubTable) {
    for (int i = 0; i < SubTableCount; ++i)
      delete SubTable[i];
  }
}

CFX_CTTGSUBTable::TScript::TScript() : DefaultLangSys(0), LangSysCount(0) {}

CFX_CTTGSUBTable::TScript::~TScript() {}

CFX_CTTGSUBTable::TScriptList::TScriptList() : ScriptCount(0) {}

CFX_CTTGSUBTable::TScriptList::~TScriptList() {}

CFX_CTTGSUBTable::TFeature::TFeature() : FeatureParams(0), LookupCount(0) {}

CFX_CTTGSUBTable::TFeature::~TFeature() {}

CFX_CTTGSUBTable::TFeatureList::TFeatureList() : FeatureCount(0) {}

CFX_CTTGSUBTable::TFeatureList::~TFeatureList() {}

CFX_CTTGSUBTable::TLookupList::TLookupList() : LookupCount(0) {}

CFX_CTTGSUBTable::TLookupList::~TLookupList() {}

CFX_CTTGSUBTable::TLangSys::TLangSys()
    : LookupOrder(0), ReqFeatureIndex(0), FeatureCount(0) {}

CFX_CTTGSUBTable::TLangSys::~TLangSys() {}
