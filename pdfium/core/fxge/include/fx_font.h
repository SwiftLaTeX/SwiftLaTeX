// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_INCLUDE_FX_FONT_H_
#define CORE_FXGE_INCLUDE_FX_FONT_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"
#include "core/fxge/include/fx_freetype.h"

typedef struct FT_FaceRec_* FXFT_Face;
typedef void* FXFT_Library;

class CFX_FaceCache;
class CFX_FontMapper;
class CFX_PathData;
class CFX_SizeGlyphCache;
class CFX_SubstFont;
class CTTFontDesc;
class IFX_SystemFontInfo;

#ifdef _SKIA_SUPPORT_
class SkTypeface;

using CFX_TypeFace = SkTypeface;
#endif

#define FXFONT_FIXED_PITCH 0x01
#define FXFONT_SERIF 0x02
#define FXFONT_SYMBOLIC 0x04
#define FXFONT_SCRIPT 0x08
#define FXFONT_ITALIC 0x40
#define FXFONT_BOLD 0x40000
#define FXFONT_USEEXTERNATTR 0x80000
#define FXFONT_CIDFONT 0x100000
#ifdef PDF_ENABLE_XFA
#define FXFONT_EXACTMATCH 0x80000000
#endif  // PDF_ENABLE_XFA
#define FXFONT_ANSI_CHARSET 0
#define FXFONT_DEFAULT_CHARSET 1
#define FXFONT_SYMBOL_CHARSET 2
#define FXFONT_SHIFTJIS_CHARSET 128
#define FXFONT_HANGEUL_CHARSET 129
#define FXFONT_GB2312_CHARSET 134
#define FXFONT_CHINESEBIG5_CHARSET 136
#define FXFONT_THAI_CHARSET 222
#define FXFONT_EASTEUROPE_CHARSET 238
#define FXFONT_RUSSIAN_CHARSET 204
#define FXFONT_GREEK_CHARSET 161
#define FXFONT_TURKISH_CHARSET 162
#define FXFONT_HEBREW_CHARSET 177
#define FXFONT_ARABIC_CHARSET 178
#define FXFONT_BALTIC_CHARSET 186
#define FXFONT_FF_FIXEDPITCH 1
#define FXFONT_FF_ROMAN (1 << 4)
#define FXFONT_FF_SCRIPT (4 << 4)
#define FXFONT_FW_NORMAL 400
#define FXFONT_FW_BOLD 700

#define CHARSET_FLAG_ANSI 1
#define CHARSET_FLAG_SYMBOL 2
#define CHARSET_FLAG_SHIFTJIS 4
#define CHARSET_FLAG_BIG5 8
#define CHARSET_FLAG_GB 16
#define CHARSET_FLAG_KOREAN 32

class CFX_Font {
 public:
  CFX_Font();
  ~CFX_Font();

  void LoadSubst(const CFX_ByteString& face_name,
                 FX_BOOL bTrueType,
                 uint32_t flags,
                 int weight,
                 int italic_angle,
                 int CharsetCP,
                 FX_BOOL bVertical = FALSE);

  FX_BOOL LoadEmbedded(const uint8_t* data, uint32_t size);
  FXFT_Face GetFace() const { return m_Face; }
  CFX_SubstFont* GetSubstFont() const { return m_pSubstFont; }

#ifdef PDF_ENABLE_XFA
  FX_BOOL LoadFile(IFX_FileRead* pFile,
                   int nFaceIndex = 0,
                   int* pFaceCount = nullptr);

  FX_BOOL LoadClone(const CFX_Font* pFont);
  void SetFace(FXFT_Face face) { m_Face = face; }
  void SetSubstFont(CFX_SubstFont* subst) { m_pSubstFont = subst; }
#endif  // PDF_ENABLE_XFA

  CFX_PathData* LoadGlyphPath(uint32_t glyph_index, int dest_width = 0);
  int GetGlyphWidth(uint32_t glyph_index);
  int GetAscent() const;
  int GetDescent() const;
  FX_BOOL GetGlyphBBox(uint32_t glyph_index, FX_RECT& bbox);
  FX_BOOL IsItalic() const;
  FX_BOOL IsBold() const;
  FX_BOOL IsFixedWidth() const;
  FX_BOOL IsVertical() const { return m_bVertical; }
  CFX_ByteString GetPsName() const;
  CFX_ByteString GetFamilyName() const;
  CFX_ByteString GetFaceName() const;
  FX_BOOL IsTTFont() const;
  FX_BOOL GetBBox(FX_RECT& bbox);
  int GetHeight() const;
  int GetULPos() const;
  int GetULthickness() const;
  int GetMaxAdvanceWidth() const;
  FX_BOOL IsEmbedded() const { return m_bEmbedded; }
  uint8_t* GetSubData() const { return m_pGsubData; }
  void SetSubData(uint8_t* data) { m_pGsubData = data; }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  void* GetPlatformFont() const { return m_pPlatformFont; }
  void SetPlatformFont(void* font) { m_pPlatformFont = font; }
#endif
  uint8_t* GetFontData() const { return m_pFontData; }
  uint32_t GetSize() const { return m_dwSize; }
  void AdjustMMParams(int glyph_index, int width, int weight);

#ifdef PDF_ENABLE_XFA
 protected:
  CFX_BinaryBuf m_OtfFontData;
  FX_BOOL m_bLogic;
  void* m_pOwnedStream;
#endif  // PDF_ENABLE_XFA

 private:
  void ReleasePlatformResource();
  void DeleteFace();

  FXFT_Face m_Face;
  CFX_SubstFont* m_pSubstFont;
  uint8_t* m_pFontDataAllocation;
  uint8_t* m_pFontData;
  uint8_t* m_pGsubData;
  uint32_t m_dwSize;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  void* m_pPlatformFont;
#endif
  FX_BOOL m_bEmbedded;
  FX_BOOL m_bVertical;
};

#define ENCODING_INTERNAL 0
#define ENCODING_UNICODE 1

#ifdef PDF_ENABLE_XFA
#define FXFM_ENC_TAG(a, b, c, d)                                          \
  (((uint32_t)(a) << 24) | ((uint32_t)(b) << 16) | ((uint32_t)(c) << 8) | \
   (uint32_t)(d))
#define FXFM_ENCODING_NONE FXFM_ENC_TAG(0, 0, 0, 0)
#define FXFM_ENCODING_MS_SYMBOL FXFM_ENC_TAG('s', 'y', 'm', 'b')
#define FXFM_ENCODING_UNICODE FXFM_ENC_TAG('u', 'n', 'i', 'c')
#define FXFM_ENCODING_MS_SJIS FXFM_ENC_TAG('s', 'j', 'i', 's')
#define FXFM_ENCODING_MS_GB2312 FXFM_ENC_TAG('g', 'b', ' ', ' ')
#define FXFM_ENCODING_MS_BIG5 FXFM_ENC_TAG('b', 'i', 'g', '5')
#define FXFM_ENCODING_MS_WANSUNG FXFM_ENC_TAG('w', 'a', 'n', 's')
#define FXFM_ENCODING_MS_JOHAB FXFM_ENC_TAG('j', 'o', 'h', 'a')
#define FXFM_ENCODING_ADOBE_STANDARD FXFM_ENC_TAG('A', 'D', 'O', 'B')
#define FXFM_ENCODING_ADOBE_EXPERT FXFM_ENC_TAG('A', 'D', 'B', 'E')
#define FXFM_ENCODING_ADOBE_CUSTOM FXFM_ENC_TAG('A', 'D', 'B', 'C')
#define FXFM_ENCODING_ADOBE_LATIN_1 FXFM_ENC_TAG('l', 'a', 't', '1')
#define FXFM_ENCODING_OLD_LATIN_2 FXFM_ENC_TAG('l', 'a', 't', '2')
#define FXFM_ENCODING_APPLE_ROMAN FXFM_ENC_TAG('a', 'r', 'm', 'n')
#endif  // PDF_ENABLE_XFA

class CFX_UnicodeEncoding {
 public:
  explicit CFX_UnicodeEncoding(CFX_Font* pFont);
  virtual ~CFX_UnicodeEncoding();

  virtual uint32_t GlyphFromCharCode(uint32_t charcode);

 protected:
  // Unowned, not nullptr.
  CFX_Font* m_pFont;
};

#ifdef PDF_ENABLE_XFA
class CFX_UnicodeEncodingEx : public CFX_UnicodeEncoding {
 public:
  CFX_UnicodeEncodingEx(CFX_Font* pFont, uint32_t EncodingID);
  ~CFX_UnicodeEncodingEx() override;

  // CFX_UnicodeEncoding:
  uint32_t GlyphFromCharCode(uint32_t charcode) override;

  uint32_t CharCodeFromUnicode(FX_WCHAR Unicode) const;

 private:
  uint32_t m_nEncodingID;
};
CFX_UnicodeEncodingEx* FX_CreateFontEncodingEx(
    CFX_Font* pFont,
    uint32_t nEncodingID = FXFM_ENCODING_NONE);
#endif  // PDF_ENABLE_XFA

#define FXFONT_SUBST_MM 0x01
#define FXFONT_SUBST_GLYPHPATH 0x04
#define FXFONT_SUBST_CLEARTYPE 0x08
#define FXFONT_SUBST_TRANSFORM 0x10
#define FXFONT_SUBST_NONSYMBOL 0x20
#define FXFONT_SUBST_EXACT 0x40
#define FXFONT_SUBST_STANDARD 0x80

class CFX_SubstFont {
 public:
  CFX_SubstFont();

  void* m_ExtHandle;
  CFX_ByteString m_Family;
  int m_Charset;
  uint32_t m_SubstFlags;
  int m_Weight;
  int m_ItalicAngle;
  bool m_bSubstCJK;
  int m_WeightCJK;
  bool m_bItalicCJK;
};

#define FX_FONT_FLAG_SERIF 0x01
#define FX_FONT_FLAG_FIXEDPITCH 0x02
#define FX_FONT_FLAG_ITALIC 0x04
#define FX_FONT_FLAG_BOLD 0x08
#define FX_FONT_FLAG_SYMBOLIC_SYMBOL 0x10
#define FX_FONT_FLAG_SYMBOLIC_DINGBATS 0x20
#define FX_FONT_FLAG_MULTIPLEMASTER 0x40

class CFX_FontMgr {
 public:
  CFX_FontMgr();
  ~CFX_FontMgr();

  void InitFTLibrary();

  FXFT_Face GetCachedFace(const CFX_ByteString& face_name,
                          int weight,
                          FX_BOOL bItalic,
                          uint8_t*& pFontData);
  FXFT_Face AddCachedFace(const CFX_ByteString& face_name,
                          int weight,
                          FX_BOOL bItalic,
                          uint8_t* pData,
                          uint32_t size,
                          int face_index);
  FXFT_Face GetCachedTTCFace(int ttc_size,
                             uint32_t checksum,
                             int font_offset,
                             uint8_t*& pFontData);
  FXFT_Face AddCachedTTCFace(int ttc_size,
                             uint32_t checksum,
                             uint8_t* pData,
                             uint32_t size,
                             int font_offset);
  FXFT_Face GetFileFace(const FX_CHAR* filename, int face_index);
  FXFT_Face GetFixedFace(const uint8_t* pData, uint32_t size, int face_index);
  void ReleaseFace(FXFT_Face face);
  void SetSystemFontInfo(std::unique_ptr<IFX_SystemFontInfo> pFontInfo);
  FXFT_Face FindSubstFont(const CFX_ByteString& face_name,
                          FX_BOOL bTrueType,
                          uint32_t flags,
                          int weight,
                          int italic_angle,
                          int CharsetCP,
                          CFX_SubstFont* pSubstFont);
  bool GetBuiltinFont(size_t index, const uint8_t** pFontData, uint32_t* size);
  CFX_FontMapper* GetBuiltinMapper() const { return m_pBuiltinMapper.get(); }
  FXFT_Library GetFTLibrary() const { return m_FTLibrary; }
  bool FTLibrarySupportsHinting() const { return m_FTLibrarySupportsHinting; }

 private:
  std::unique_ptr<CFX_FontMapper> m_pBuiltinMapper;
  std::map<CFX_ByteString, CTTFontDesc*> m_FaceMap;
  FXFT_Library m_FTLibrary;
  bool m_FTLibrarySupportsHinting;
};

class CFX_FontMapper {
 public:
  explicit CFX_FontMapper(CFX_FontMgr* mgr);
  ~CFX_FontMapper();

  void SetSystemFontInfo(std::unique_ptr<IFX_SystemFontInfo> pFontInfo);
  IFX_SystemFontInfo* GetSystemFontInfo() { return m_pFontInfo.get(); }
  void AddInstalledFont(const CFX_ByteString& name, int charset);
  void LoadInstalledFonts();

  FXFT_Face FindSubstFont(const CFX_ByteString& face_name,
                          FX_BOOL bTrueType,
                          uint32_t flags,
                          int weight,
                          int italic_angle,
                          int CharsetCP,
                          CFX_SubstFont* pSubstFont);
#ifdef PDF_ENABLE_XFA
  FXFT_Face FindSubstFontByUnicode(uint32_t dwUnicode,
                                   uint32_t flags,
                                   int weight,
                                   int italic_angle);
#endif  // PDF_ENABLE_XFA
  FX_BOOL IsBuiltinFace(const FXFT_Face face) const;
  int GetFaceSize() const;
  CFX_ByteString GetFaceName(int index) const {
    return m_FaceArray[index].name;
  }

  std::vector<CFX_ByteString> m_InstalledTTFonts;

 private:
  static const size_t MM_FACE_COUNT = 2;
  static const size_t FOXIT_FACE_COUNT = 14;

  CFX_ByteString GetPSNameFromTT(void* hFont);
  CFX_ByteString MatchInstalledFonts(const CFX_ByteString& norm_name);
  FXFT_Face UseInternalSubst(CFX_SubstFont* pSubstFont,
                             int iBaseFont,
                             int italic_angle,
                             int weight,
                             int picthfamily);

  struct FaceData {
    CFX_ByteString name;
    uint32_t charset;
  };

  FX_BOOL m_bListLoaded;
  FXFT_Face m_MMFaces[MM_FACE_COUNT];
  CFX_ByteString m_LastFamily;
  std::vector<FaceData> m_FaceArray;
  std::unique_ptr<IFX_SystemFontInfo> m_pFontInfo;
  FXFT_Face m_FoxitFaces[FOXIT_FACE_COUNT];
  CFX_FontMgr* const m_pFontMgr;
};

class IFX_SystemFontInfo {
 public:
  static std::unique_ptr<IFX_SystemFontInfo> CreateDefault(
      const char** pUserPaths);

  virtual ~IFX_SystemFontInfo() {}

  virtual FX_BOOL EnumFontList(CFX_FontMapper* pMapper) = 0;
  virtual void* MapFont(int weight,
                        FX_BOOL bItalic,
                        int charset,
                        int pitch_family,
                        const FX_CHAR* face,
                        int& iExact) = 0;

#ifdef PDF_ENABLE_XFA
  virtual void* MapFontByUnicode(uint32_t dwUnicode,
                                 int weight,
                                 FX_BOOL bItalic,
                                 int pitch_family);
#endif  // PDF_ENABLE_XFA

  virtual void* GetFont(const FX_CHAR* face) = 0;
  virtual uint32_t GetFontData(void* hFont,
                               uint32_t table,
                               uint8_t* buffer,
                               uint32_t size) = 0;
  virtual FX_BOOL GetFaceName(void* hFont, CFX_ByteString& name) = 0;
  virtual FX_BOOL GetFontCharset(void* hFont, int& charset) = 0;
  virtual int GetFaceIndex(void* hFont);
  virtual void DeleteFont(void* hFont) = 0;
  virtual void* RetainFont(void* hFont);
};

class CTTFontDesc {
 public:
  CTTFontDesc() {
    m_Type = 0;
    m_pFontData = nullptr;
    m_RefCount = 0;
  }
  ~CTTFontDesc();
  // ret < 0, releaseface not appropriate for this object.
  // ret == 0, object released
  // ret > 0, object still alive, other referrers.
  int ReleaseFace(FXFT_Face face);
  int m_Type;
  union {
    struct {
      FX_BOOL m_bItalic;
      FX_BOOL m_bBold;
      FXFT_Face m_pFace;
    } m_SingleFace;
    struct {
      FXFT_Face m_pFaces[16];
    } m_TTCFace;
  };
  uint8_t* m_pFontData;
  int m_RefCount;
};

class CFX_FontFaceInfo {
 public:
  CFX_FontFaceInfo(CFX_ByteString filePath,
                   CFX_ByteString faceName,
                   CFX_ByteString fontTables,
                   uint32_t fontOffset,
                   uint32_t fileSize);

  const CFX_ByteString m_FilePath;
  const CFX_ByteString m_FaceName;
  const CFX_ByteString m_FontTables;
  const uint32_t m_FontOffset;
  const uint32_t m_FileSize;
  uint32_t m_Styles;
  uint32_t m_Charsets;
};

class CFX_FolderFontInfo : public IFX_SystemFontInfo {
 public:
  CFX_FolderFontInfo();
  ~CFX_FolderFontInfo() override;

  void AddPath(const CFX_ByteStringC& path);

  // IFX_SytemFontInfo:
  FX_BOOL EnumFontList(CFX_FontMapper* pMapper) override;
  void* MapFont(int weight,
                FX_BOOL bItalic,
                int charset,
                int pitch_family,
                const FX_CHAR* face,
                int& bExact) override;
#ifdef PDF_ENABLE_XFA
  void* MapFontByUnicode(uint32_t dwUnicode,
                         int weight,
                         FX_BOOL bItalic,
                         int pitch_family) override;
#endif  // PDF_ENABLE_XFA
  void* GetFont(const FX_CHAR* face) override;
  uint32_t GetFontData(void* hFont,
                       uint32_t table,
                       uint8_t* buffer,
                       uint32_t size) override;
  void DeleteFont(void* hFont) override;
  FX_BOOL GetFaceName(void* hFont, CFX_ByteString& name) override;
  FX_BOOL GetFontCharset(void* hFont, int& charset) override;

 protected:
  void ScanPath(const CFX_ByteString& path);
  void ScanFile(const CFX_ByteString& path);
  void ReportFace(const CFX_ByteString& path,
                  FXSYS_FILE* pFile,
                  uint32_t filesize,
                  uint32_t offset);
  void* GetSubstFont(const CFX_ByteString& face);
  void* FindFont(int weight,
                 FX_BOOL bItalic,
                 int charset,
                 int pitch_family,
                 const FX_CHAR* family,
                 FX_BOOL bMatchName);

  std::map<CFX_ByteString, CFX_FontFaceInfo*> m_FontList;
  std::vector<CFX_ByteString> m_PathList;
  CFX_FontMapper* m_pMapper;
};

class CFX_CountedFaceCache {
 public:
  CFX_FaceCache* m_Obj;
  uint32_t m_nCount;
};

class CFX_FontCache {
 public:
  CFX_FontCache();
  ~CFX_FontCache();
  CFX_FaceCache* GetCachedFace(CFX_Font* pFont);
  void ReleaseCachedFace(CFX_Font* pFont);
  void FreeCache(FX_BOOL bRelease = FALSE);
#ifdef _SKIA_SUPPORT_
  CFX_TypeFace* GetDeviceCache(CFX_Font* pFont);
#endif

 private:
  using CFX_FTCacheMap = std::map<FXFT_Face, CFX_CountedFaceCache*>;
  CFX_FTCacheMap m_FTFaceMap;
  CFX_FTCacheMap m_ExtFaceMap;
};

class CFX_AutoFontCache {
 public:
  CFX_AutoFontCache(CFX_FontCache* pFontCache, CFX_Font* pFont)
      : m_pFontCache(pFontCache), m_pFont(pFont) {}
  ~CFX_AutoFontCache() { m_pFontCache->ReleaseCachedFace(m_pFont); }
  CFX_FontCache* m_pFontCache;
  CFX_Font* m_pFont;
};
#define FX_FONTCACHE_DEFINE(pFontCache, pFont) \
  CFX_AutoFontCache autoFontCache((pFontCache), (pFont))
class CFX_GlyphBitmap {
 public:
  int m_Top;
  int m_Left;
  CFX_DIBitmap m_Bitmap;
};
class CFX_FaceCache {
 public:
  explicit CFX_FaceCache(FXFT_Face face);
  ~CFX_FaceCache();
  const CFX_GlyphBitmap* LoadGlyphBitmap(CFX_Font* pFont,
                                         uint32_t glyph_index,
                                         FX_BOOL bFontStyle,
                                         const CFX_Matrix* pMatrix,
                                         int dest_width,
                                         int anti_alias,
                                         int& text_flags);
  const CFX_PathData* LoadGlyphPath(CFX_Font* pFont,
                                    uint32_t glyph_index,
                                    int dest_width);

#ifdef _SKIA_SUPPORT_
  CFX_TypeFace* GetDeviceCache(CFX_Font* pFont);
#endif

 private:
  CFX_GlyphBitmap* RenderGlyph(CFX_Font* pFont,
                               uint32_t glyph_index,
                               FX_BOOL bFontStyle,
                               const CFX_Matrix* pMatrix,
                               int dest_width,
                               int anti_alias);
  CFX_GlyphBitmap* RenderGlyph_Nativetext(CFX_Font* pFont,
                                          uint32_t glyph_index,
                                          const CFX_Matrix* pMatrix,
                                          int dest_width,
                                          int anti_alias);
  CFX_GlyphBitmap* LookUpGlyphBitmap(CFX_Font* pFont,
                                     const CFX_Matrix* pMatrix,
                                     const CFX_ByteString& FaceGlyphsKey,
                                     uint32_t glyph_index,
                                     FX_BOOL bFontStyle,
                                     int dest_width,
                                     int anti_alias);
  void InitPlatform();
  void DestroyPlatform();

  FXFT_Face const m_Face;
  std::map<CFX_ByteString, CFX_SizeGlyphCache*> m_SizeMap;
  std::map<uint32_t, CFX_PathData*> m_PathMap;
  CFX_DIBitmap* m_pBitmap;
#ifdef _SKIA_SUPPORT_
  CFX_TypeFace* m_pTypeface;
#endif
};

struct FXTEXT_GLYPHPOS {
  const CFX_GlyphBitmap* m_pGlyph;
  int m_OriginX;
  int m_OriginY;
  FX_FLOAT m_fOriginX;
  FX_FLOAT m_fOriginY;
};

FX_RECT FXGE_GetGlyphsBBox(const std::vector<FXTEXT_GLYPHPOS>& glyphs,
                           int anti_alias,
                           FX_FLOAT retinaScaleX = 1.0f,
                           FX_FLOAT retinaScaleY = 1.0f);

CFX_ByteString GetNameFromTT(const uint8_t* name_table,
                             uint32_t name_table_size,
                             uint32_t name);

int PDF_GetStandardFontName(CFX_ByteString* name);

#endif  // CORE_FXGE_INCLUDE_FX_FONT_H_
