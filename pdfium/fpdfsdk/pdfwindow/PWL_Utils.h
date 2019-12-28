// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PDFWINDOW_PWL_UTILS_H_
#define FPDFSDK_PDFWINDOW_PWL_UTILS_H_

#include "core/fpdfdoc/include/cpvt_wordrange.h"
#include "fpdfsdk/pdfwindow/PWL_Wnd.h"

class CFX_Edit;
class CFX_PathData;

struct CPWL_Color;

template <class T>
T PWL_MIN(const T& i, const T& j) {
  return ((i < j) ? i : j);
}
template <class T>
T PWL_MAX(const T& i, const T& j) {
  return ((i > j) ? i : j);
}

#define PWL_PDF2WIN(color) (uint8_t(color * 255))
#define PWL_WIN2PDF(color) ((FX_FLOAT)((FX_FLOAT)color / 255.0f))

#define PWL_MAKEDWORD(low, high) \
  ((uint32_t)((uint16_t)(low) | (uint32_t)(((uint16_t)(high)) << 16)))
#define PWL_GETLOWWORD(dword) ((uint16_t)(dword))
#define PWL_GETHIGHWORD(dword) ((uint16_t)(dword >> 16))

#define PWL_ICONTYPE_CHECKMARK 0
#define PWL_ICONTYPE_CIRCLE 1
#define PWL_ICONTYPE_COMMENT 2
#define PWL_ICONTYPE_CROSS 3
#define PWL_ICONTYPE_HELP 4
#define PWL_ICONTYPE_INSERTTEXT 5
#define PWL_ICONTYPE_KEY 6
#define PWL_ICONTYPE_NEWPARAGRAPH 7
#define PWL_ICONTYPE_TEXTNOTE 8
#define PWL_ICONTYPE_PARAGRAPH 9
#define PWL_ICONTYPE_RIGHTARROW 10
#define PWL_ICONTYPE_RIGHTPOINTER 11
#define PWL_ICONTYPE_STAR 12
#define PWL_ICONTYPE_UPARROW 13
#define PWL_ICONTYPE_UPLEFTARROW 14

#define PWL_ICONTYPE_GRAPH 15
#define PWL_ICONTYPE_PAPERCLIP 16
#define PWL_ICONTYPE_ATTACHMENT 17
#define PWL_ICONTYPE_TAG 18

#define PWL_ICONTYPE_FOXIT 19

#define PWL_ICONTYPE_UNKNOWN -1

// checkbox & radiobutton style
#define PCS_CHECK 0
#define PCS_CIRCLE 1
#define PCS_CROSS 2
#define PCS_DIAMOND 3
#define PCS_SQUARE 4
#define PCS_STAR 5

// pushbutton layout style
#define PPBL_LABEL 0
#define PPBL_ICON 1
#define PPBL_ICONTOPLABELBOTTOM 2
#define PPBL_LABELTOPICONBOTTOM 3
#define PPBL_ICONLEFTLABELRIGHT 4
#define PPBL_LABELLEFTICONRIGHT 5
#define PPBL_LABELOVERICON 6

class CPWL_Point : public CFX_FloatPoint {
 public:
  CPWL_Point() : CFX_FloatPoint(0.0f, 0.0f) {}
  CPWL_Point(FX_FLOAT fx, FX_FLOAT fy) : CFX_FloatPoint(fx, fy) {}
  CPWL_Point(const CPWL_Point& point) : CFX_FloatPoint(point.x, point.y) {}
};

enum PWL_PATHDATA_TYPE {
  PWLPT_MOVETO,
  PWLPT_LINETO,
  PWLPT_BEZIERTO,
  PWLPT_UNKNOWN
};

enum PWL_PATH_TYPE { PWLPT_PATHDATA, PWLPT_STREAM };

class CPWL_PathData {
 public:
  CPWL_PathData() : point(), type(PWLPT_UNKNOWN) {}
  CPWL_PathData(const CPWL_Point& pt, PWL_PATHDATA_TYPE tp)
      : point(pt), type(tp) {}

  CPWL_Point point;
  PWL_PATHDATA_TYPE type;
};

class CPWL_Utils {
 public:
  static CFX_FloatRect InflateRect(const CFX_FloatRect& rcRect, FX_FLOAT fSize);
  static CFX_FloatRect DeflateRect(const CFX_FloatRect& rcRect, FX_FLOAT fSize);
  static FX_BOOL IntersectRect(const CFX_FloatRect& rect1,
                               const CFX_FloatRect& rect2);
  static FX_BOOL ContainsRect(const CFX_FloatRect& rcParent,
                              const CFX_FloatRect& rcChild);
  static CFX_FloatRect ScaleRect(const CFX_FloatRect& rcRect, FX_FLOAT fScale);
  static CPVT_WordRange OverlapWordRange(const CPVT_WordRange& wr1,
                                         const CPVT_WordRange& wr2);
  static CFX_FloatRect GetCenterSquare(const CFX_FloatRect& rect);
  static CPWL_Color SubstractColor(const CPWL_Color& sColor,
                                   FX_FLOAT fColorSub);
  static CPWL_Color DevideColor(const CPWL_Color& sColor,
                                FX_FLOAT fColorDevide);
  static CFX_FloatRect MaxRect(const CFX_FloatRect& rect1,
                               const CFX_FloatRect& rect2);
  static CFX_FloatRect OffsetRect(const CFX_FloatRect& rect,
                                  FX_FLOAT x,
                                  FX_FLOAT y);
  static CFX_FloatPoint OffsetPoint(const CFX_FloatPoint& point,
                                    FX_FLOAT x,
                                    FX_FLOAT y);
  static FX_COLORREF PWLColorToFXColor(const CPWL_Color& color,
                                       int32_t nTransparancy = 255);
  static FX_BOOL IsBlackOrWhite(const CPWL_Color& color);
  static CPWL_Color GetReverseColor(const CPWL_Color& color);

  static CFX_ByteString GetColorAppStream(const CPWL_Color& color,
                                          const FX_BOOL& bFillOrStroke = TRUE);
  static CFX_ByteString GetBorderAppStream(const CFX_FloatRect& rect,
                                           FX_FLOAT fWidth,
                                           const CPWL_Color& color,
                                           const CPWL_Color& crLeftTop,
                                           const CPWL_Color& crRightBottom,
                                           BorderStyle nStyle,
                                           const CPWL_Dash& dash);
  static CFX_ByteString GetCircleBorderAppStream(
      const CFX_FloatRect& rect,
      FX_FLOAT fWidth,
      const CPWL_Color& color,
      const CPWL_Color& crLeftTop,
      const CPWL_Color& crRightBottom,
      BorderStyle nStyle,
      const CPWL_Dash& dash);
  static CFX_ByteString GetRectFillAppStream(const CFX_FloatRect& rect,
                                             const CPWL_Color& color);
  static CFX_ByteString GetCircleFillAppStream(const CFX_FloatRect& rect,
                                               const CPWL_Color& color);

  static CFX_ByteString GetPushButtonAppStream(const CFX_FloatRect& rcBBox,
                                               IPVT_FontMap* pFontMap,
                                               CPDF_Stream* pIconStream,
                                               CPDF_IconFit& IconFit,
                                               const CFX_WideString& sLabel,
                                               const CPWL_Color& crText,
                                               FX_FLOAT fFontSize,
                                               int32_t nLayOut);
  static CFX_ByteString GetCheckBoxAppStream(const CFX_FloatRect& rcBBox,
                                             int32_t nStyle,
                                             const CPWL_Color& crText);
  static CFX_ByteString GetRadioButtonAppStream(const CFX_FloatRect& rcBBox,
                                                int32_t nStyle,
                                                const CPWL_Color& crText);

  static CFX_ByteString GetEditAppStream(CFX_Edit* pEdit,
                                         const CFX_FloatPoint& ptOffset,
                                         const CPVT_WordRange* pRange = nullptr,
                                         FX_BOOL bContinuous = TRUE,
                                         uint16_t SubWord = 0);
  static CFX_ByteString GetEditSelAppStream(
      CFX_Edit* pEdit,
      const CFX_FloatPoint& ptOffset,
      const CPVT_WordRange* pRange = nullptr);
  static CFX_ByteString GetTextAppStream(const CFX_FloatRect& rcBBox,
                                         IPVT_FontMap* pFontMap,
                                         const CFX_WideString& sText,
                                         int32_t nAlignmentH,
                                         int32_t nAlignmentV,
                                         FX_FLOAT fFontSize,
                                         FX_BOOL bMultiLine,
                                         FX_BOOL bAutoReturn,
                                         const CPWL_Color& crText);
  static CFX_ByteString GetDropButtonAppStream(const CFX_FloatRect& rcBBox);

  static void DrawFillRect(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_FloatRect& rect,
                           const CPWL_Color& color,
                           int32_t nTransparancy);
  static void DrawFillRect(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_FloatRect& rect,
                           const FX_COLORREF& color);
  static void DrawStrokeRect(CFX_RenderDevice* pDevice,
                             CFX_Matrix* pUser2Device,
                             const CFX_FloatRect& rect,
                             const FX_COLORREF& color,
                             FX_FLOAT fWidth);
  static void DrawStrokeLine(CFX_RenderDevice* pDevice,
                             CFX_Matrix* pUser2Device,
                             const CFX_FloatPoint& ptMoveTo,
                             const CFX_FloatPoint& ptLineTo,
                             const FX_COLORREF& color,
                             FX_FLOAT fWidth);
  static void DrawBorder(CFX_RenderDevice* pDevice,
                         CFX_Matrix* pUser2Device,
                         const CFX_FloatRect& rect,
                         FX_FLOAT fWidth,
                         const CPWL_Color& color,
                         const CPWL_Color& crLeftTop,
                         const CPWL_Color& crRightBottom,
                         BorderStyle nStyle,
                         int32_t nTransparancy);
  static void DrawFillArea(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           const CFX_FloatPoint* pPts,
                           int32_t nCount,
                           const FX_COLORREF& color);
  static void DrawShadow(CFX_RenderDevice* pDevice,
                         CFX_Matrix* pUser2Device,
                         FX_BOOL bVertical,
                         FX_BOOL bHorizontal,
                         CFX_FloatRect rect,
                         int32_t nTransparancy,
                         int32_t nStartGray,
                         int32_t nEndGray);

 public:
  static void ConvertCMYK2RGB(FX_FLOAT dC,
                              FX_FLOAT dM,
                              FX_FLOAT dY,
                              FX_FLOAT dK,
                              FX_FLOAT& dR,
                              FX_FLOAT& dG,
                              FX_FLOAT& dB);
  static void ConvertRGB2CMYK(FX_FLOAT dR,
                              FX_FLOAT dG,
                              FX_FLOAT dB,
                              FX_FLOAT& dC,
                              FX_FLOAT& dM,
                              FX_FLOAT& dY,
                              FX_FLOAT& dK);

  static void ConvertRGB2GRAY(FX_FLOAT dR,
                              FX_FLOAT dG,
                              FX_FLOAT dB,
                              FX_FLOAT& dGray);
  static void ConvertGRAY2RGB(FX_FLOAT dGray,
                              FX_FLOAT& dR,
                              FX_FLOAT& dG,
                              FX_FLOAT& dB);

  static void ConvertCMYK2GRAY(FX_FLOAT dC,
                               FX_FLOAT dM,
                               FX_FLOAT dY,
                               FX_FLOAT dK,
                               FX_FLOAT& dGray);
  static void ConvertGRAY2CMYK(FX_FLOAT dGray,
                               FX_FLOAT& dC,
                               FX_FLOAT& dM,
                               FX_FLOAT& dY,
                               FX_FLOAT& dK);

  static void PWLColorToARGB(const CPWL_Color& color,
                             int32_t& alpha,
                             FX_FLOAT& red,
                             FX_FLOAT& green,
                             FX_FLOAT& blue);

 public:
  static CFX_ByteString GetIconAppStream(
      int32_t nType,
      const CFX_FloatRect& rect,
      const CPWL_Color& crFill,
      const CPWL_Color& crStroke = PWL_DEFAULT_BLACKCOLOR);
  static void DrawIconAppStream(CFX_RenderDevice* pDevice,
                                CFX_Matrix* pUser2Device,
                                int32_t nType,
                                const CFX_FloatRect& rect,
                                const CPWL_Color& crFill,
                                const CPWL_Color& crStroke,
                                const int32_t nTransparancy);

 private:
  static CFX_ByteString GetAppStreamFromArray(const CPWL_PathData* pPathData,
                                              int32_t nCount);
  static void GetPathDataFromArray(CFX_PathData& path,
                                   const CPWL_PathData* pPathData,
                                   int32_t nCount);

  static CFX_ByteString GetAppStream_Check(const CFX_FloatRect& rcBBox,
                                           const CPWL_Color& crText);
  static CFX_ByteString GetAppStream_Circle(const CFX_FloatRect& rcBBox,
                                            const CPWL_Color& crText);
  static CFX_ByteString GetAppStream_Cross(const CFX_FloatRect& rcBBox,
                                           const CPWL_Color& crText);
  static CFX_ByteString GetAppStream_Diamond(const CFX_FloatRect& rcBBox,
                                             const CPWL_Color& crText);
  static CFX_ByteString GetAppStream_Square(const CFX_FloatRect& rcBBox,
                                            const CPWL_Color& crText);
  static CFX_ByteString GetAppStream_Star(const CFX_FloatRect& rcBBox,
                                          const CPWL_Color& crText);

  static CFX_ByteString GetAP_Check(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Circle(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Cross(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Diamond(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Square(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_Star(const CFX_FloatRect& crBBox);
  static CFX_ByteString GetAP_HalfCircle(const CFX_FloatRect& crBBox,
                                         FX_FLOAT fRotate);

  static void GetGraphics_Checkmark(CFX_ByteString& sPathData,
                                    CFX_PathData& path,
                                    const CFX_FloatRect& crBBox,
                                    const PWL_PATH_TYPE type);
  static void GetGraphics_Circle(CFX_ByteString& sPathData,
                                 CFX_PathData& path,
                                 const CFX_FloatRect& crBBox,
                                 const PWL_PATH_TYPE type);
  static void GetGraphics_Comment(CFX_ByteString& sPathData,
                                  CFX_PathData& path,
                                  const CFX_FloatRect& crBBox,
                                  const PWL_PATH_TYPE type);
  static void GetGraphics_Cross(CFX_ByteString& sPathData,
                                CFX_PathData& path,
                                const CFX_FloatRect& crBBox,
                                const PWL_PATH_TYPE type);
  static void GetGraphics_Help(CFX_ByteString& sPathData,
                               CFX_PathData& path,
                               const CFX_FloatRect& crBBox,
                               const PWL_PATH_TYPE type);
  static void GetGraphics_InsertText(CFX_ByteString& sPathData,
                                     CFX_PathData& path,
                                     const CFX_FloatRect& crBBox,
                                     const PWL_PATH_TYPE type);
  static void GetGraphics_Key(CFX_ByteString& sPathData,
                              CFX_PathData& path,
                              const CFX_FloatRect& crBBox,
                              const PWL_PATH_TYPE type);
  static void GetGraphics_NewParagraph(CFX_ByteString& sPathData,
                                       CFX_PathData& path,
                                       const CFX_FloatRect& crBBox,
                                       const PWL_PATH_TYPE type);
  static void GetGraphics_TextNote(CFX_ByteString& sPathData,
                                   CFX_PathData& path,
                                   const CFX_FloatRect& crBBox,
                                   const PWL_PATH_TYPE type);
  static void GetGraphics_Paragraph(CFX_ByteString& sPathData,
                                    CFX_PathData& path,
                                    const CFX_FloatRect& crBBox,
                                    const PWL_PATH_TYPE type);
  static void GetGraphics_RightArrow(CFX_ByteString& sPathData,
                                     CFX_PathData& path,
                                     const CFX_FloatRect& crBBox,
                                     const PWL_PATH_TYPE type);
  static void GetGraphics_RightPointer(CFX_ByteString& sPathData,
                                       CFX_PathData& path,
                                       const CFX_FloatRect& crBBox,
                                       const PWL_PATH_TYPE type);
  static void GetGraphics_Star(CFX_ByteString& sPathData,
                               CFX_PathData& path,
                               const CFX_FloatRect& crBBox,
                               const PWL_PATH_TYPE type);
  static void GetGraphics_UpArrow(CFX_ByteString& sPathData,
                                  CFX_PathData& path,
                                  const CFX_FloatRect& crBBox,
                                  const PWL_PATH_TYPE type);
  static void GetGraphics_UpLeftArrow(CFX_ByteString& sPathData,
                                      CFX_PathData& path,
                                      const CFX_FloatRect& crBBox,
                                      const PWL_PATH_TYPE type);
  static void GetGraphics_Graph(CFX_ByteString& sPathData,
                                CFX_PathData& path,
                                const CFX_FloatRect& crBBox,
                                const PWL_PATH_TYPE type);
  static void GetGraphics_Paperclip(CFX_ByteString& sPathData,
                                    CFX_PathData& path,
                                    const CFX_FloatRect& crBBox,
                                    const PWL_PATH_TYPE type);
  static void GetGraphics_Attachment(CFX_ByteString& sPathData,
                                     CFX_PathData& path,
                                     const CFX_FloatRect& crBBox,
                                     const PWL_PATH_TYPE type);
  static void GetGraphics_Tag(CFX_ByteString& sPathData,
                              CFX_PathData& path,
                              const CFX_FloatRect& crBBox,
                              const PWL_PATH_TYPE type);
  static void GetGraphics_Foxit(CFX_ByteString& sPathData,
                                CFX_PathData& path,
                                const CFX_FloatRect& crBBox,
                                const PWL_PATH_TYPE type);
};

#endif  // FPDFSDK_PDFWINDOW_PWL_UTILS_H_
