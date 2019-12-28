// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_PAGEINT_H_
#define CORE_FPDFAPI_FPDF_PAGE_PAGEINT_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>

#include "core/fpdfapi/fpdf_page/cpdf_contentmark.h"
#include "core/fpdfapi/fpdf_page/cpdf_countedobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobjectholder.h"
#include "core/fxge/include/fx_ge.h"

class CPDF_AllStates;
class CPDF_ColorSpace;
class CPDF_ExpIntFunc;
class CPDF_Font;
class CPDF_FontEncoding;
class CPDF_Form;
class CPDF_IccProfile;
class CPDF_Image;
class CPDF_ImageObject;
class CPDF_Page;
class CPDF_Pattern;
class CPDF_SampledFunc;
class CPDF_StitchFunc;
class CPDF_StreamAcc;
class CPDF_TextObject;
class CPDF_Type3Char;

#define PARSE_STEP_LIMIT 100

class CPDF_StreamParser {
 public:
  enum SyntaxType { EndOfData, Number, Keyword, Name, Others };

  CPDF_StreamParser(const uint8_t* pData, uint32_t dwSize);
  ~CPDF_StreamParser();

  CPDF_Stream* ReadInlineStream(CPDF_Document* pDoc,
                                CPDF_Dictionary* pDict,
                                CPDF_Object* pCSObj);
  SyntaxType ParseNextElement();
  uint8_t* GetWordBuf() { return m_WordBuffer; }
  uint32_t GetWordSize() const { return m_WordSize; }
  CPDF_Object* GetObject() {
    CPDF_Object* pObj = m_pLastObj;
    m_pLastObj = nullptr;
    return pObj;
  }
  uint32_t GetPos() const { return m_Pos; }
  void SetPos(uint32_t pos) { m_Pos = pos; }
  CPDF_Object* ReadNextObject(bool bAllowNestedArray, uint32_t dwInArrayLevel);

 protected:
  friend class fpdf_page_parser_old_ReadHexString_Test;

  void GetNextWord(FX_BOOL& bIsNumber);
  CFX_ByteString ReadString();
  CFX_ByteString ReadHexString();
  const uint8_t* m_pBuf;

  // Length in bytes of m_pBuf.
  uint32_t m_Size;

  // Current byte position within m_pBuf.
  uint32_t m_Pos;

  uint8_t m_WordBuffer[256];
  uint32_t m_WordSize;
  CPDF_Object* m_pLastObj;

 private:
  bool PositionIsInBounds() const;
};

#define PARAM_BUF_SIZE 16
struct ContentParam {
  enum Type { OBJECT = 0, NUMBER, NAME };
  Type m_Type;
  union {
    struct {
      bool m_bInteger;
      union {
        int m_Integer;
        FX_FLOAT m_Float;
      };
    } m_Number;
    CPDF_Object* m_pObject;
    struct {
      int m_Len;
      char m_Buffer[32];
    } m_Name;
  };
};
#define _FPDF_MAX_FORM_LEVEL_ 30
#define _FPDF_MAX_TYPE3_FORM_LEVEL_ 4
class CPDF_StreamContentParser {
 public:
  CPDF_StreamContentParser(CPDF_Document* pDoc,
                           CPDF_Dictionary* pPageResources,
                           CPDF_Dictionary* pParentResources,
                           const CFX_Matrix* pmtContentToUser,
                           CPDF_PageObjectHolder* pObjectHolder,
                           CPDF_Dictionary* pResources,
                           CFX_FloatRect* pBBox,
                           CPDF_AllStates* pAllStates,
                           int level);
  ~CPDF_StreamContentParser();

  CPDF_PageObjectHolder* GetPageObjectHolder() const { return m_pObjectHolder; }
  CPDF_AllStates* GetCurStates() const { return m_pCurStates.get(); }
  FX_BOOL IsColored() const { return m_bColored; }
  const FX_FLOAT* GetType3Data() const { return m_Type3Data; }

  void AddNumberParam(const FX_CHAR* str, int len);
  void AddObjectParam(CPDF_Object* pObj);
  void AddNameParam(const FX_CHAR* name, int size);
  int GetNextParamPos();
  void ClearAllParams();
  CPDF_Object* GetObject(uint32_t index);
  CFX_ByteString GetString(uint32_t index);
  FX_FLOAT GetNumber(uint32_t index);
  int GetInteger(uint32_t index) { return (int32_t)(GetNumber(index)); }
  void OnOperator(const FX_CHAR* op);
  void BigCaseCaller(int index);
  uint32_t GetParsePos() { return m_pSyntax->GetPos(); }
  void AddTextObject(CFX_ByteString* pText,
                     FX_FLOAT fInitKerning,
                     FX_FLOAT* pKerning,
                     int count);

  void ConvertUserSpace(FX_FLOAT& x, FX_FLOAT& y);
  void ConvertTextSpace(FX_FLOAT& x, FX_FLOAT& y);
  void OnChangeTextMatrix();
  uint32_t Parse(const uint8_t* pData, uint32_t dwSize, uint32_t max_cost);
  void ParsePathObject();
  void AddPathPoint(FX_FLOAT x, FX_FLOAT y, int flag);
  void AddPathRect(FX_FLOAT x, FX_FLOAT y, FX_FLOAT w, FX_FLOAT h);
  void AddPathObject(int FillType, FX_BOOL bStroke);
  CPDF_ImageObject* AddImage(CPDF_Stream* pStream,
                             CPDF_Image* pImage,
                             FX_BOOL bInline);
  void AddDuplicateImage();
  void AddForm(CPDF_Stream* pStream);
  void SetGraphicStates(CPDF_PageObject* pObj,
                        FX_BOOL bColor,
                        FX_BOOL bText,
                        FX_BOOL bGraph);
  void SaveStates(CPDF_AllStates* pState);
  void RestoreStates(CPDF_AllStates* pState);
  CPDF_Font* FindFont(const CFX_ByteString& name);
  CPDF_ColorSpace* FindColorSpace(const CFX_ByteString& name);
  CPDF_Pattern* FindPattern(const CFX_ByteString& name, bool bShading);
  CPDF_Object* FindResourceObj(const CFX_ByteString& type,
                               const CFX_ByteString& name);

 protected:
  using OpCodes =
      std::unordered_map<uint32_t, void (CPDF_StreamContentParser::*)()>;
  static OpCodes InitializeOpCodes();

  void Handle_CloseFillStrokePath();
  void Handle_FillStrokePath();
  void Handle_CloseEOFillStrokePath();
  void Handle_EOFillStrokePath();
  void Handle_BeginMarkedContent_Dictionary();
  void Handle_BeginImage();
  void Handle_BeginMarkedContent();
  void Handle_BeginText();
  void Handle_CurveTo_123();
  void Handle_ConcatMatrix();
  void Handle_SetColorSpace_Fill();
  void Handle_SetColorSpace_Stroke();
  void Handle_SetDash();
  void Handle_SetCharWidth();
  void Handle_SetCachedDevice();
  void Handle_ExecuteXObject();
  void Handle_MarkPlace_Dictionary();
  void Handle_EndImage();
  void Handle_EndMarkedContent();
  void Handle_EndText();
  void Handle_FillPath();
  void Handle_FillPathOld();
  void Handle_EOFillPath();
  void Handle_SetGray_Fill();
  void Handle_SetGray_Stroke();
  void Handle_SetExtendGraphState();
  void Handle_ClosePath();
  void Handle_SetFlat();
  void Handle_BeginImageData();
  void Handle_SetLineJoin();
  void Handle_SetLineCap();
  void Handle_SetCMYKColor_Fill();
  void Handle_SetCMYKColor_Stroke();
  void Handle_LineTo();
  void Handle_MoveTo();
  void Handle_SetMiterLimit();
  void Handle_MarkPlace();
  void Handle_EndPath();
  void Handle_SaveGraphState();
  void Handle_RestoreGraphState();
  void Handle_Rectangle();
  void Handle_SetRGBColor_Fill();
  void Handle_SetRGBColor_Stroke();
  void Handle_SetRenderIntent();
  void Handle_CloseStrokePath();
  void Handle_StrokePath();
  void Handle_SetColor_Fill();
  void Handle_SetColor_Stroke();
  void Handle_SetColorPS_Fill();
  void Handle_SetColorPS_Stroke();
  void Handle_ShadeFill();
  void Handle_SetCharSpace();
  void Handle_MoveTextPoint();
  void Handle_MoveTextPoint_SetLeading();
  void Handle_SetFont();
  void Handle_ShowText();
  void Handle_ShowText_Positioning();
  void Handle_SetTextLeading();
  void Handle_SetTextMatrix();
  void Handle_SetTextRenderMode();
  void Handle_SetTextRise();
  void Handle_SetWordSpace();
  void Handle_SetHorzScale();
  void Handle_MoveToNextLine();
  void Handle_CurveTo_23();
  void Handle_SetLineWidth();
  void Handle_Clip();
  void Handle_EOClip();
  void Handle_CurveTo_13();
  void Handle_NextLineShowText();
  void Handle_NextLineShowText_Space();
  void Handle_Invalid();

  CPDF_Document* const m_pDocument;
  CPDF_Dictionary* m_pPageResources;
  CPDF_Dictionary* m_pParentResources;
  CPDF_Dictionary* m_pResources;
  CPDF_PageObjectHolder* m_pObjectHolder;
  int m_Level;
  CFX_Matrix m_mtContentToUser;
  CFX_FloatRect m_BBox;
  ContentParam m_ParamBuf[PARAM_BUF_SIZE];
  uint32_t m_ParamStartPos;
  uint32_t m_ParamCount;
  CPDF_StreamParser* m_pSyntax;
  std::unique_ptr<CPDF_AllStates> m_pCurStates;
  CPDF_ContentMark m_CurContentMark;
  std::vector<std::unique_ptr<CPDF_TextObject>> m_ClipTextList;
  CPDF_TextObject* m_pLastTextObject;
  FX_FLOAT m_DefFontSize;
  FX_PATHPOINT* m_pPathPoints;
  int m_PathPointCount;
  int m_PathAllocSize;
  FX_FLOAT m_PathStartX;
  FX_FLOAT m_PathStartY;
  FX_FLOAT m_PathCurrentX;
  FX_FLOAT m_PathCurrentY;
  uint8_t m_PathClipType;
  CFX_ByteString m_LastImageName;
  CPDF_Image* m_pLastImage;
  CFX_BinaryBuf m_LastImageDict;
  CFX_BinaryBuf m_LastImageData;
  CPDF_Dictionary* m_pLastImageDict;
  CPDF_Dictionary* m_pLastCloneImageDict;
  FX_BOOL m_bReleaseLastDict;
  FX_BOOL m_bSameLastDict;
  FX_BOOL m_bColored;
  FX_FLOAT m_Type3Data[6];
  FX_BOOL m_bResourceMissing;
  std::vector<std::unique_ptr<CPDF_AllStates>> m_StateStack;
};
class CPDF_ContentParser {
 public:
  enum ParseStatus { Ready, ToBeContinued, Done };

  CPDF_ContentParser();
  ~CPDF_ContentParser();

  ParseStatus GetStatus() const { return m_Status; }
  void Start(CPDF_Page* pPage);
  void Start(CPDF_Form* pForm,
             CPDF_AllStates* pGraphicStates,
             const CFX_Matrix* pParentMatrix,
             CPDF_Type3Char* pType3Char,
             int level);
  void Continue(IFX_Pause* pPause);

 private:
  enum InternalStage {
    STAGE_GETCONTENT = 1,
    STAGE_PARSE,
    STAGE_CHECKCLIP,
  };

  ParseStatus m_Status;
  InternalStage m_InternalStage;
  CPDF_PageObjectHolder* m_pObjectHolder;
  FX_BOOL m_bForm;
  CPDF_Type3Char* m_pType3Char;
  uint32_t m_nStreams;
  std::unique_ptr<CPDF_StreamAcc> m_pSingleStream;
  std::vector<std::unique_ptr<CPDF_StreamAcc>> m_StreamArray;
  uint8_t* m_pData;
  uint32_t m_Size;
  uint32_t m_CurrentOffset;
  std::unique_ptr<CPDF_StreamContentParser> m_pParser;
};

class CPDF_DocPageData {
 public:
  explicit CPDF_DocPageData(CPDF_Document* pPDFDoc);
  ~CPDF_DocPageData();

  void Clear(FX_BOOL bRelease = FALSE);
  CPDF_Font* GetFont(CPDF_Dictionary* pFontDict, FX_BOOL findOnly);
  CPDF_Font* GetStandardFont(const CFX_ByteString& fontName,
                             CPDF_FontEncoding* pEncoding);
  void ReleaseFont(CPDF_Dictionary* pFontDict);
  CPDF_ColorSpace* GetColorSpace(CPDF_Object* pCSObj,
                                 const CPDF_Dictionary* pResources);
  CPDF_ColorSpace* GetCopiedColorSpace(CPDF_Object* pCSObj);
  void ReleaseColorSpace(CPDF_Object* pColorSpace);
  CPDF_Pattern* GetPattern(CPDF_Object* pPatternObj,
                           FX_BOOL bShading,
                           const CFX_Matrix& matrix);
  void ReleasePattern(CPDF_Object* pPatternObj);
  CPDF_Image* GetImage(CPDF_Object* pImageStream);
  void ReleaseImage(CPDF_Object* pImageStream);
  CPDF_IccProfile* GetIccProfile(CPDF_Stream* pIccProfileStream);
  void ReleaseIccProfile(CPDF_IccProfile* pIccProfile);
  CPDF_StreamAcc* GetFontFileStreamAcc(CPDF_Stream* pFontStream);
  void ReleaseFontFileStreamAcc(CPDF_Stream* pFontStream,
                                FX_BOOL bForce = FALSE);
  FX_BOOL IsForceClear() const { return m_bForceClear; }
  CPDF_CountedColorSpace* FindColorSpacePtr(CPDF_Object* pCSObj) const;
  CPDF_CountedPattern* FindPatternPtr(CPDF_Object* pPatternObj) const;

 private:
  using CPDF_CountedFont = CPDF_CountedObject<CPDF_Font>;
  using CPDF_CountedIccProfile = CPDF_CountedObject<CPDF_IccProfile>;
  using CPDF_CountedImage = CPDF_CountedObject<CPDF_Image>;
  using CPDF_CountedStreamAcc = CPDF_CountedObject<CPDF_StreamAcc>;

  using CPDF_ColorSpaceMap = std::map<CPDF_Object*, CPDF_CountedColorSpace*>;
  using CPDF_FontFileMap = std::map<CPDF_Stream*, CPDF_CountedStreamAcc*>;
  using CPDF_FontMap = std::map<CPDF_Dictionary*, CPDF_CountedFont*>;
  using CPDF_IccProfileMap = std::map<CPDF_Stream*, CPDF_CountedIccProfile*>;
  using CPDF_ImageMap = std::map<uint32_t, CPDF_CountedImage*>;
  using CPDF_PatternMap = std::map<CPDF_Object*, CPDF_CountedPattern*>;

  CPDF_ColorSpace* GetColorSpaceImpl(CPDF_Object* pCSObj,
                                     const CPDF_Dictionary* pResources,
                                     std::set<CPDF_Object*>* pVisited);

  CPDF_Document* const m_pPDFDoc;
  FX_BOOL m_bForceClear;
  std::map<CFX_ByteString, CPDF_Stream*> m_HashProfileMap;
  CPDF_ColorSpaceMap m_ColorSpaceMap;
  CPDF_FontFileMap m_FontFileMap;
  CPDF_FontMap m_FontMap;
  CPDF_IccProfileMap m_IccProfileMap;
  CPDF_ImageMap m_ImageMap;
  CPDF_PatternMap m_PatternMap;
};

class CPDF_Function {
 public:
  enum class Type {
    kTypeInvalid = -1,
    kType0Sampled = 0,
    kType2ExpotentialInterpolation = 2,
    kType3Stitching = 3,
    kType4PostScript = 4,
  };

  static std::unique_ptr<CPDF_Function> Load(CPDF_Object* pFuncObj);
  static Type IntegerToFunctionType(int iType);

  virtual ~CPDF_Function();
  FX_BOOL Call(FX_FLOAT* inputs,
               uint32_t ninputs,
               FX_FLOAT* results,
               int& nresults) const;
  uint32_t CountInputs() const { return m_nInputs; }
  uint32_t CountOutputs() const { return m_nOutputs; }
  FX_FLOAT GetDomain(int i) const { return m_pDomains[i]; }
  FX_FLOAT GetRange(int i) const { return m_pRanges[i]; }

  const CPDF_SampledFunc* ToSampledFunc() const;
  const CPDF_ExpIntFunc* ToExpIntFunc() const;
  const CPDF_StitchFunc* ToStitchFunc() const;

 protected:
  explicit CPDF_Function(Type type);

  FX_BOOL Init(CPDF_Object* pObj);
  virtual FX_BOOL v_Init(CPDF_Object* pObj) = 0;
  virtual FX_BOOL v_Call(FX_FLOAT* inputs, FX_FLOAT* results) const = 0;

  uint32_t m_nInputs;
  uint32_t m_nOutputs;
  FX_FLOAT* m_pDomains;
  FX_FLOAT* m_pRanges;
  const Type m_Type;
};

class CPDF_ExpIntFunc : public CPDF_Function {
 public:
  CPDF_ExpIntFunc();
  ~CPDF_ExpIntFunc() override;

  // CPDF_Function
  FX_BOOL v_Init(CPDF_Object* pObj) override;
  FX_BOOL v_Call(FX_FLOAT* inputs, FX_FLOAT* results) const override;

  uint32_t m_nOrigOutputs;
  FX_FLOAT m_Exponent;
  FX_FLOAT* m_pBeginValues;
  FX_FLOAT* m_pEndValues;
};

class CPDF_SampledFunc : public CPDF_Function {
 public:
  struct SampleEncodeInfo {
    FX_FLOAT encode_max;
    FX_FLOAT encode_min;
    uint32_t sizes;
  };

  struct SampleDecodeInfo {
    FX_FLOAT decode_max;
    FX_FLOAT decode_min;
  };

  CPDF_SampledFunc();
  ~CPDF_SampledFunc() override;

  // CPDF_Function
  FX_BOOL v_Init(CPDF_Object* pObj) override;
  FX_BOOL v_Call(FX_FLOAT* inputs, FX_FLOAT* results) const override;

  const std::vector<SampleEncodeInfo>& GetEncodeInfo() const {
    return m_EncodeInfo;
  }
  uint32_t GetBitsPerSample() const { return m_nBitsPerSample; }
  const CPDF_StreamAcc* GetSampleStream() const {
    return m_pSampleStream.get();
  }

 private:
  std::vector<SampleEncodeInfo> m_EncodeInfo;
  std::vector<SampleDecodeInfo> m_DecodeInfo;
  uint32_t m_nBitsPerSample;
  uint32_t m_SampleMax;
  std::unique_ptr<CPDF_StreamAcc> m_pSampleStream;
};

class CPDF_StitchFunc : public CPDF_Function {
 public:
  CPDF_StitchFunc();
  ~CPDF_StitchFunc() override;

  // CPDF_Function
  FX_BOOL v_Init(CPDF_Object* pObj) override;
  FX_BOOL v_Call(FX_FLOAT* inputs, FX_FLOAT* results) const override;

  const std::vector<std::unique_ptr<CPDF_Function>>& GetSubFunctions() const {
    return m_pSubFunctions;
  }
  FX_FLOAT GetBound(size_t i) const { return m_pBounds[i]; }

 private:
  std::vector<std::unique_ptr<CPDF_Function>> m_pSubFunctions;
  FX_FLOAT* m_pBounds;
  FX_FLOAT* m_pEncode;

  static const uint32_t kRequiredNumInputs = 1;
};

class CPDF_IccProfile {
 public:
  CPDF_IccProfile(const uint8_t* pData, uint32_t dwSize);
  ~CPDF_IccProfile();
  uint32_t GetComponents() const { return m_nSrcComponents; }
  FX_BOOL m_bsRGB;
  void* m_pTransform;

 private:
  uint32_t m_nSrcComponents;
};

class CPDF_DeviceCS : public CPDF_ColorSpace {
 public:
  CPDF_DeviceCS(CPDF_Document* pDoc, int family);

  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  FX_BOOL SetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT R,
                 FX_FLOAT G,
                 FX_FLOAT B) const override;
  FX_BOOL v_GetCMYK(FX_FLOAT* pBuf,
                    FX_FLOAT& c,
                    FX_FLOAT& m,
                    FX_FLOAT& y,
                    FX_FLOAT& k) const override;
  FX_BOOL v_SetCMYK(FX_FLOAT* pBuf,
                    FX_FLOAT c,
                    FX_FLOAT m,
                    FX_FLOAT y,
                    FX_FLOAT k) const override;
  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          FX_BOOL bTransMask = FALSE) const override;
};

class CPDF_PatternCS : public CPDF_ColorSpace {
 public:
  explicit CPDF_PatternCS(CPDF_Document* pDoc);
  ~CPDF_PatternCS() override;
  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;
  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  CPDF_ColorSpace* GetBaseCS() const override;

 private:
  CPDF_ColorSpace* m_pBaseCS;
  CPDF_CountedColorSpace* m_pCountedBaseCS;
};

#define MAX_PATTERN_COLORCOMPS 16
struct PatternValue {
  CPDF_Pattern* m_pPattern;
  CPDF_CountedPattern* m_pCountedPattern;
  int m_nComps;
  FX_FLOAT m_Comps[MAX_PATTERN_COLORCOMPS];
};

CFX_ByteStringC PDF_FindKeyAbbreviationForTesting(const CFX_ByteStringC& abbr);
CFX_ByteStringC PDF_FindValueAbbreviationForTesting(
    const CFX_ByteStringC& abbr);

void PDF_ReplaceAbbr(CPDF_Object* pObj);
bool IsPathOperator(const uint8_t* buf, size_t len);

#endif  // CORE_FPDFAPI_FPDF_PAGE_PAGEINT_H_
