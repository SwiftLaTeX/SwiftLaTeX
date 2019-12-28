// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <algorithm>

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fxcrt/include/fx_ext.h"
#include "fpdfsdk/include/fsdk_baseannot.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#endif  // PDF_ENABLE_XFA

namespace {

const float kMinWidth = 1.0f;
const float kMinHeight = 1.0f;

int gAfxGetTimeZoneInSeconds(int8_t tzhour, uint8_t tzminute) {
  return (int)tzhour * 3600 + (int)tzminute * (tzhour >= 0 ? 60 : -60);
}

bool gAfxIsLeapYear(int16_t year) {
  return ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)));
}

uint16_t gAfxGetYearDays(int16_t year) {
  return (gAfxIsLeapYear(year) ? 366 : 365);
}

uint8_t gAfxGetMonthDays(int16_t year, uint8_t month) {
  uint8_t mDays;
  switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      mDays = 31;
      break;

    case 4:
    case 6:
    case 9:
    case 11:
      mDays = 30;
      break;

    case 2:
      if (gAfxIsLeapYear(year))
        mDays = 29;
      else
        mDays = 28;
      break;

    default:
      mDays = 0;
      break;
  }

  return mDays;
}

}  // namespace

CPDFSDK_DateTime::CPDFSDK_DateTime() {
  ResetDateTime();
}

CPDFSDK_DateTime::CPDFSDK_DateTime(const CFX_ByteString& dtStr) {
  ResetDateTime();

  FromPDFDateTimeString(dtStr);
}

CPDFSDK_DateTime::CPDFSDK_DateTime(const CPDFSDK_DateTime& datetime) {
  operator=(datetime);
}

CPDFSDK_DateTime::CPDFSDK_DateTime(const FX_SYSTEMTIME& st) {
  operator=(st);
}

void CPDFSDK_DateTime::ResetDateTime() {
  tzset();

  time_t curTime;
  time(&curTime);
  struct tm* newtime = localtime(&curTime);

  dt.year = newtime->tm_year + 1900;
  dt.month = newtime->tm_mon + 1;
  dt.day = newtime->tm_mday;
  dt.hour = newtime->tm_hour;
  dt.minute = newtime->tm_min;
  dt.second = newtime->tm_sec;
}

CPDFSDK_DateTime& CPDFSDK_DateTime::operator=(
    const CPDFSDK_DateTime& datetime) {
  FXSYS_memcpy(&dt, &datetime.dt, sizeof(FX_DATETIME));
  return *this;
}

CPDFSDK_DateTime& CPDFSDK_DateTime::operator=(const FX_SYSTEMTIME& st) {
  tzset();

  dt.year = static_cast<int16_t>(st.wYear);
  dt.month = static_cast<uint8_t>(st.wMonth);
  dt.day = static_cast<uint8_t>(st.wDay);
  dt.hour = static_cast<uint8_t>(st.wHour);
  dt.minute = static_cast<uint8_t>(st.wMinute);
  dt.second = static_cast<uint8_t>(st.wSecond);
  return *this;
}

bool CPDFSDK_DateTime::operator==(const CPDFSDK_DateTime& datetime) const {
  return (FXSYS_memcmp(&dt, &datetime.dt, sizeof(FX_DATETIME)) == 0);
}

bool CPDFSDK_DateTime::operator!=(const CPDFSDK_DateTime& datetime) const {
  return !(*this == datetime);
}

time_t CPDFSDK_DateTime::ToTime_t() const {
  struct tm newtime;

  newtime.tm_year = dt.year - 1900;
  newtime.tm_mon = dt.month - 1;
  newtime.tm_mday = dt.day;
  newtime.tm_hour = dt.hour;
  newtime.tm_min = dt.minute;
  newtime.tm_sec = dt.second;

  return mktime(&newtime);
}

CPDFSDK_DateTime& CPDFSDK_DateTime::FromPDFDateTimeString(
    const CFX_ByteString& dtStr) {
  int strLength = dtStr.GetLength();
  if (strLength <= 0)
    return *this;

  int i = 0;
  while (i < strLength && !std::isdigit(dtStr[i]))
    ++i;

  if (i >= strLength)
    return *this;

  int j = 0;
  int k = 0;
  FX_CHAR ch;
  while (i < strLength && j < 4) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.year = static_cast<int16_t>(k);
  if (i >= strLength || j < 4)
    return *this;

  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.month = static_cast<uint8_t>(k);
  if (i >= strLength || j < 2)
    return *this;

  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.day = static_cast<uint8_t>(k);
  if (i >= strLength || j < 2)
    return *this;

  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.hour = static_cast<uint8_t>(k);
  if (i >= strLength || j < 2)
    return *this;

  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.minute = static_cast<uint8_t>(k);
  if (i >= strLength || j < 2)
    return *this;

  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.second = static_cast<uint8_t>(k);
  if (i >= strLength || j < 2)
    return *this;

  ch = dtStr[i++];
  if (ch != '-' && ch != '+')
    return *this;
  if (ch == '-')
    dt.tzHour = -1;
  else
    dt.tzHour = 1;
  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.tzHour *= static_cast<int8_t>(k);
  if (i >= strLength || j < 2)
    return *this;

  if (dtStr[i++] != '\'')
    return *this;
  j = 0;
  k = 0;
  while (i < strLength && j < 2) {
    ch = dtStr[i];
    k = k * 10 + FXSYS_toDecimalDigit(ch);
    j++;
    if (!std::isdigit(ch))
      break;
    i++;
  }
  dt.tzMinute = static_cast<uint8_t>(k);
  return *this;
}

CFX_ByteString CPDFSDK_DateTime::ToCommonDateTimeString() {
  CFX_ByteString str1;
  str1.Format("%04d-%02u-%02u %02u:%02u:%02u ", dt.year, dt.month, dt.day,
              dt.hour, dt.minute, dt.second);
  if (dt.tzHour < 0)
    str1 += "-";
  else
    str1 += "+";
  CFX_ByteString str2;
  str2.Format("%02d:%02u", std::abs(static_cast<int>(dt.tzHour)), dt.tzMinute);
  return str1 + str2;
}

CFX_ByteString CPDFSDK_DateTime::ToPDFDateTimeString() {
  CFX_ByteString dtStr;
  char tempStr[32];
  memset(tempStr, 0, sizeof(tempStr));
  FXSYS_snprintf(tempStr, sizeof(tempStr) - 1, "D:%04d%02u%02u%02u%02u%02u",
                 dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
  dtStr = CFX_ByteString(tempStr);
  if (dt.tzHour < 0)
    dtStr += CFX_ByteString("-");
  else
    dtStr += CFX_ByteString("+");
  memset(tempStr, 0, sizeof(tempStr));
  FXSYS_snprintf(tempStr, sizeof(tempStr) - 1, "%02d'%02u'",
                 std::abs(static_cast<int>(dt.tzHour)), dt.tzMinute);
  dtStr += CFX_ByteString(tempStr);
  return dtStr;
}

void CPDFSDK_DateTime::ToSystemTime(FX_SYSTEMTIME& st) {
  time_t t = this->ToTime_t();
  struct tm* pTime = localtime(&t);
  if (pTime) {
    st.wYear = static_cast<uint16_t>(pTime->tm_year) + 1900;
    st.wMonth = static_cast<uint16_t>(pTime->tm_mon) + 1;
    st.wDay = static_cast<uint16_t>(pTime->tm_mday);
    st.wDayOfWeek = static_cast<uint16_t>(pTime->tm_wday);
    st.wHour = static_cast<uint16_t>(pTime->tm_hour);
    st.wMinute = static_cast<uint16_t>(pTime->tm_min);
    st.wSecond = static_cast<uint16_t>(pTime->tm_sec);
    st.wMilliseconds = 0;
  }
}

CPDFSDK_DateTime CPDFSDK_DateTime::ToGMT() const {
  CPDFSDK_DateTime new_dt = *this;
  new_dt.AddSeconds(
      -gAfxGetTimeZoneInSeconds(new_dt.dt.tzHour, new_dt.dt.tzMinute));
  new_dt.dt.tzHour = 0;
  new_dt.dt.tzMinute = 0;
  return new_dt;
}

CPDFSDK_DateTime& CPDFSDK_DateTime::AddDays(short days) {
  if (days == 0)
    return *this;

  int16_t y = dt.year;
  uint8_t m = dt.month;
  uint8_t d = dt.day;

  int ldays = days;
  if (ldays > 0) {
    int16_t yy = y;
    if ((static_cast<uint16_t>(m) * 100 + d) > 300)
      yy++;
    int ydays = gAfxGetYearDays(yy);
    int mdays;
    while (ldays >= ydays) {
      y++;
      ldays -= ydays;
      yy++;
      mdays = gAfxGetMonthDays(y, m);
      if (d > mdays) {
        m++;
        d -= mdays;
      }
      ydays = gAfxGetYearDays(yy);
    }
    mdays = gAfxGetMonthDays(y, m) - d + 1;
    while (ldays >= mdays) {
      ldays -= mdays;
      m++;
      d = 1;
      mdays = gAfxGetMonthDays(y, m);
    }
    d += ldays;
  } else {
    ldays *= -1;
    int16_t yy = y;
    if ((static_cast<uint16_t>(m) * 100 + d) < 300)
      yy--;
    int ydays = gAfxGetYearDays(yy);
    while (ldays >= ydays) {
      y--;
      ldays -= ydays;
      yy--;
      int mdays = gAfxGetMonthDays(y, m);
      if (d > mdays) {
        m++;
        d -= mdays;
      }
      ydays = gAfxGetYearDays(yy);
    }
    while (ldays >= d) {
      ldays -= d;
      m--;
      d = gAfxGetMonthDays(y, m);
    }
    d -= ldays;
  }

  dt.year = y;
  dt.month = m;
  dt.day = d;

  return *this;
}

CPDFSDK_DateTime& CPDFSDK_DateTime::AddSeconds(int seconds) {
  if (seconds == 0)
    return *this;

  int n;
  int days;

  n = dt.hour * 3600 + dt.minute * 60 + dt.second + seconds;
  if (n < 0) {
    days = (n - 86399) / 86400;
    n -= days * 86400;
  } else {
    days = n / 86400;
    n %= 86400;
  }
  dt.hour = static_cast<uint8_t>(n / 3600);
  dt.hour %= 24;
  n %= 3600;
  dt.minute = static_cast<uint8_t>(n / 60);
  dt.second = static_cast<uint8_t>(n % 60);
  if (days != 0)
    AddDays(days);

  return *this;
}

CPDFSDK_Annot::CPDFSDK_Annot(CPDFSDK_PageView* pPageView)
    : m_pPageView(pPageView), m_bSelected(FALSE), m_nTabOrder(-1) {}

CPDFSDK_Annot::~CPDFSDK_Annot() {}

#ifdef PDF_ENABLE_XFA

FX_BOOL CPDFSDK_Annot::IsXFAField() {
  return FALSE;
}

CXFA_FFWidget* CPDFSDK_Annot::GetXFAWidget() const {
  return nullptr;
}

#endif  // PDF_ENABLE_XFA

FX_FLOAT CPDFSDK_Annot::GetMinWidth() const {
  return kMinWidth;
}

FX_FLOAT CPDFSDK_Annot::GetMinHeight() const {
  return kMinHeight;
}

int CPDFSDK_Annot::GetLayoutOrder() const {
  return 5;
}

CPDF_Annot* CPDFSDK_Annot::GetPDFAnnot() const {
  return nullptr;
}

CFX_ByteString CPDFSDK_Annot::GetType() const {
  return "";
}

CFX_ByteString CPDFSDK_Annot::GetSubType() const {
  return "";
}

void CPDFSDK_Annot::SetRect(const CFX_FloatRect& rect) {}

CFX_FloatRect CPDFSDK_Annot::GetRect() const {
  return CFX_FloatRect();
}

void CPDFSDK_Annot::Annot_OnDraw(CFX_RenderDevice* pDevice,
                                 CFX_Matrix* pUser2Device,
                                 CPDF_RenderOptions* pOptions) {}

CPDFSDK_BAAnnot::CPDFSDK_BAAnnot(CPDF_Annot* pAnnot,
                                 CPDFSDK_PageView* pPageView)
    : CPDFSDK_Annot(pPageView), m_pAnnot(pAnnot) {}

CPDFSDK_BAAnnot::~CPDFSDK_BAAnnot() {}

CPDF_Annot* CPDFSDK_BAAnnot::GetPDFAnnot() const {
  return m_pAnnot;
}

FX_BOOL CPDFSDK_Annot::IsSelected() {
  return m_bSelected;
}

void CPDFSDK_Annot::SetSelected(FX_BOOL bSelected) {
  m_bSelected = bSelected;
}

int CPDFSDK_Annot::GetTabOrder() {
  return m_nTabOrder;
}

void CPDFSDK_Annot::SetTabOrder(int iTabOrder) {
  m_nTabOrder = iTabOrder;
}

CPDF_Dictionary* CPDFSDK_BAAnnot::GetAnnotDict() const {
  return m_pAnnot->GetAnnotDict();
}

void CPDFSDK_BAAnnot::SetRect(const CFX_FloatRect& rect) {
  ASSERT(rect.right - rect.left >= GetMinWidth());
  ASSERT(rect.top - rect.bottom >= GetMinHeight());

  m_pAnnot->GetAnnotDict()->SetAtRect("Rect", rect);
}

CFX_FloatRect CPDFSDK_BAAnnot::GetRect() const {
  CFX_FloatRect rect;
  m_pAnnot->GetRect(rect);
  return rect;
}

CFX_ByteString CPDFSDK_BAAnnot::GetType() const {
  return m_pAnnot->GetSubType();
}

CFX_ByteString CPDFSDK_BAAnnot::GetSubType() const {
  return "";
}

void CPDFSDK_BAAnnot::DrawAppearance(CFX_RenderDevice* pDevice,
                                     const CFX_Matrix* pUser2Device,
                                     CPDF_Annot::AppearanceMode mode,
                                     const CPDF_RenderOptions* pOptions) {
  m_pAnnot->DrawAppearance(m_pPageView->GetPDFPage(), pDevice, pUser2Device,
                           mode, pOptions);
}

FX_BOOL CPDFSDK_BAAnnot::IsAppearanceValid() {
  return !!m_pAnnot->GetAnnotDict()->GetDictBy("AP");
}

FX_BOOL CPDFSDK_BAAnnot::IsAppearanceValid(CPDF_Annot::AppearanceMode mode) {
  CPDF_Dictionary* pAP = m_pAnnot->GetAnnotDict()->GetDictBy("AP");
  if (!pAP)
    return FALSE;

  // Choose the right sub-ap
  const FX_CHAR* ap_entry = "N";
  if (mode == CPDF_Annot::Down)
    ap_entry = "D";
  else if (mode == CPDF_Annot::Rollover)
    ap_entry = "R";
  if (!pAP->KeyExist(ap_entry))
    ap_entry = "N";

  // Get the AP stream or subdirectory
  CPDF_Object* psub = pAP->GetDirectObjectBy(ap_entry);
  return !!psub;
}

void CPDFSDK_BAAnnot::DrawBorder(CFX_RenderDevice* pDevice,
                                 const CFX_Matrix* pUser2Device,
                                 const CPDF_RenderOptions* pOptions) {
  m_pAnnot->DrawBorder(pDevice, pUser2Device, pOptions);
}

void CPDFSDK_BAAnnot::ClearCachedAP() {
  m_pAnnot->ClearCachedAP();
}

void CPDFSDK_BAAnnot::SetContents(const CFX_WideString& sContents) {
  if (sContents.IsEmpty())
    m_pAnnot->GetAnnotDict()->RemoveAt("Contents");
  else
    m_pAnnot->GetAnnotDict()->SetAtString("Contents",
                                          PDF_EncodeText(sContents));
}

CFX_WideString CPDFSDK_BAAnnot::GetContents() const {
  return m_pAnnot->GetAnnotDict()->GetUnicodeTextBy("Contents");
}

void CPDFSDK_BAAnnot::SetAnnotName(const CFX_WideString& sName) {
  if (sName.IsEmpty())
    m_pAnnot->GetAnnotDict()->RemoveAt("NM");
  else
    m_pAnnot->GetAnnotDict()->SetAtString("NM", PDF_EncodeText(sName));
}

CFX_WideString CPDFSDK_BAAnnot::GetAnnotName() const {
  return m_pAnnot->GetAnnotDict()->GetUnicodeTextBy("NM");
}

void CPDFSDK_BAAnnot::SetModifiedDate(const FX_SYSTEMTIME& st) {
  CPDFSDK_DateTime dt(st);
  CFX_ByteString str = dt.ToPDFDateTimeString();

  if (str.IsEmpty())
    m_pAnnot->GetAnnotDict()->RemoveAt("M");
  else
    m_pAnnot->GetAnnotDict()->SetAtString("M", str);
}

FX_SYSTEMTIME CPDFSDK_BAAnnot::GetModifiedDate() const {
  FX_SYSTEMTIME systime;
  CFX_ByteString str = m_pAnnot->GetAnnotDict()->GetStringBy("M");

  CPDFSDK_DateTime dt(str);
  dt.ToSystemTime(systime);

  return systime;
}

void CPDFSDK_BAAnnot::SetFlags(uint32_t nFlags) {
  m_pAnnot->GetAnnotDict()->SetAtInteger("F", nFlags);
}

uint32_t CPDFSDK_BAAnnot::GetFlags() const {
  return m_pAnnot->GetAnnotDict()->GetIntegerBy("F");
}

void CPDFSDK_BAAnnot::SetAppState(const CFX_ByteString& str) {
  if (str.IsEmpty())
    m_pAnnot->GetAnnotDict()->RemoveAt("AS");
  else
    m_pAnnot->GetAnnotDict()->SetAtString("AS", str);
}

CFX_ByteString CPDFSDK_BAAnnot::GetAppState() const {
  return m_pAnnot->GetAnnotDict()->GetStringBy("AS");
}

void CPDFSDK_BAAnnot::SetStructParent(int key) {
  m_pAnnot->GetAnnotDict()->SetAtInteger("StructParent", key);
}

int CPDFSDK_BAAnnot::GetStructParent() const {
  return m_pAnnot->GetAnnotDict()->GetIntegerBy("StructParent");
}

// border
void CPDFSDK_BAAnnot::SetBorderWidth(int nWidth) {
  CPDF_Array* pBorder = m_pAnnot->GetAnnotDict()->GetArrayBy("Border");

  if (pBorder) {
    pBorder->SetAt(2, new CPDF_Number(nWidth));
  } else {
    CPDF_Dictionary* pBSDict = m_pAnnot->GetAnnotDict()->GetDictBy("BS");

    if (!pBSDict) {
      pBSDict = new CPDF_Dictionary;
      m_pAnnot->GetAnnotDict()->SetAt("BS", pBSDict);
    }

    pBSDict->SetAtInteger("W", nWidth);
  }
}

int CPDFSDK_BAAnnot::GetBorderWidth() const {
  if (CPDF_Array* pBorder = m_pAnnot->GetAnnotDict()->GetArrayBy("Border")) {
    return pBorder->GetIntegerAt(2);
  }
  if (CPDF_Dictionary* pBSDict = m_pAnnot->GetAnnotDict()->GetDictBy("BS")) {
    return pBSDict->GetIntegerBy("W", 1);
  }
  return 1;
}

void CPDFSDK_BAAnnot::SetBorderStyle(BorderStyle nStyle) {
  CPDF_Dictionary* pBSDict = m_pAnnot->GetAnnotDict()->GetDictBy("BS");
  if (!pBSDict) {
    pBSDict = new CPDF_Dictionary;
    m_pAnnot->GetAnnotDict()->SetAt("BS", pBSDict);
  }

  switch (nStyle) {
    case BorderStyle::SOLID:
      pBSDict->SetAtName("S", "S");
      break;
    case BorderStyle::DASH:
      pBSDict->SetAtName("S", "D");
      break;
    case BorderStyle::BEVELED:
      pBSDict->SetAtName("S", "B");
      break;
    case BorderStyle::INSET:
      pBSDict->SetAtName("S", "I");
      break;
    case BorderStyle::UNDERLINE:
      pBSDict->SetAtName("S", "U");
      break;
    default:
      break;
  }
}

BorderStyle CPDFSDK_BAAnnot::GetBorderStyle() const {
  CPDF_Dictionary* pBSDict = m_pAnnot->GetAnnotDict()->GetDictBy("BS");
  if (pBSDict) {
    CFX_ByteString sBorderStyle = pBSDict->GetStringBy("S", "S");
    if (sBorderStyle == "S")
      return BorderStyle::SOLID;
    if (sBorderStyle == "D")
      return BorderStyle::DASH;
    if (sBorderStyle == "B")
      return BorderStyle::BEVELED;
    if (sBorderStyle == "I")
      return BorderStyle::INSET;
    if (sBorderStyle == "U")
      return BorderStyle::UNDERLINE;
  }

  CPDF_Array* pBorder = m_pAnnot->GetAnnotDict()->GetArrayBy("Border");
  if (pBorder) {
    if (pBorder->GetCount() >= 4) {
      CPDF_Array* pDP = pBorder->GetArrayAt(3);
      if (pDP && pDP->GetCount() > 0)
        return BorderStyle::DASH;
    }
  }

  return BorderStyle::SOLID;
}

void CPDFSDK_BAAnnot::SetColor(FX_COLORREF color) {
  CPDF_Array* pArray = new CPDF_Array;
  pArray->AddNumber((FX_FLOAT)FXSYS_GetRValue(color) / 255.0f);
  pArray->AddNumber((FX_FLOAT)FXSYS_GetGValue(color) / 255.0f);
  pArray->AddNumber((FX_FLOAT)FXSYS_GetBValue(color) / 255.0f);
  m_pAnnot->GetAnnotDict()->SetAt("C", pArray);
}

void CPDFSDK_BAAnnot::RemoveColor() {
  m_pAnnot->GetAnnotDict()->RemoveAt("C");
}

FX_BOOL CPDFSDK_BAAnnot::GetColor(FX_COLORREF& color) const {
  if (CPDF_Array* pEntry = m_pAnnot->GetAnnotDict()->GetArrayBy("C")) {
    size_t nCount = pEntry->GetCount();
    if (nCount == 1) {
      FX_FLOAT g = pEntry->GetNumberAt(0) * 255;

      color = FXSYS_RGB((int)g, (int)g, (int)g);

      return TRUE;
    } else if (nCount == 3) {
      FX_FLOAT r = pEntry->GetNumberAt(0) * 255;
      FX_FLOAT g = pEntry->GetNumberAt(1) * 255;
      FX_FLOAT b = pEntry->GetNumberAt(2) * 255;

      color = FXSYS_RGB((int)r, (int)g, (int)b);

      return TRUE;
    } else if (nCount == 4) {
      FX_FLOAT c = pEntry->GetNumberAt(0);
      FX_FLOAT m = pEntry->GetNumberAt(1);
      FX_FLOAT y = pEntry->GetNumberAt(2);
      FX_FLOAT k = pEntry->GetNumberAt(3);

      FX_FLOAT r = 1.0f - std::min(1.0f, c + k);
      FX_FLOAT g = 1.0f - std::min(1.0f, m + k);
      FX_FLOAT b = 1.0f - std::min(1.0f, y + k);

      color = FXSYS_RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));

      return TRUE;
    }
  }

  return FALSE;
}

void CPDFSDK_BAAnnot::WriteAppearance(const CFX_ByteString& sAPType,
                                      const CFX_FloatRect& rcBBox,
                                      const CFX_Matrix& matrix,
                                      const CFX_ByteString& sContents,
                                      const CFX_ByteString& sAPState) {
  CPDF_Dictionary* pAPDict = m_pAnnot->GetAnnotDict()->GetDictBy("AP");

  if (!pAPDict) {
    pAPDict = new CPDF_Dictionary;
    m_pAnnot->GetAnnotDict()->SetAt("AP", pAPDict);
  }

  CPDF_Stream* pStream = nullptr;
  CPDF_Dictionary* pParentDict = nullptr;

  if (sAPState.IsEmpty()) {
    pParentDict = pAPDict;
    pStream = pAPDict->GetStreamBy(sAPType);
  } else {
    CPDF_Dictionary* pAPTypeDict = pAPDict->GetDictBy(sAPType);
    if (!pAPTypeDict) {
      pAPTypeDict = new CPDF_Dictionary;
      pAPDict->SetAt(sAPType, pAPTypeDict);
    }
    pParentDict = pAPTypeDict;
    pStream = pAPTypeDict->GetStreamBy(sAPState);
  }

  if (!pStream) {
    pStream = new CPDF_Stream(nullptr, 0, nullptr);
    CPDF_Document* pDoc = m_pPageView->GetPDFDocument();
    int32_t objnum = pDoc->AddIndirectObject(pStream);
    pParentDict->SetAtReference(sAPType, pDoc, objnum);
  }

  CPDF_Dictionary* pStreamDict = pStream->GetDict();
  if (!pStreamDict) {
    pStreamDict = new CPDF_Dictionary;
    pStreamDict->SetAtName("Type", "XObject");
    pStreamDict->SetAtName("Subtype", "Form");
    pStreamDict->SetAtInteger("FormType", 1);
    pStream->InitStream(nullptr, 0, pStreamDict);
  }

  if (pStreamDict) {
    pStreamDict->SetAtMatrix("Matrix", matrix);
    pStreamDict->SetAtRect("BBox", rcBBox);
  }

  pStream->SetData((uint8_t*)sContents.c_str(), sContents.GetLength(), FALSE,
                   FALSE);
}

FX_BOOL CPDFSDK_BAAnnot::CreateFormFiller() {
  return TRUE;
}
FX_BOOL CPDFSDK_BAAnnot::IsVisible() const {
  uint32_t nFlags = GetFlags();
  return !((nFlags & ANNOTFLAG_INVISIBLE) || (nFlags & ANNOTFLAG_HIDDEN) ||
           (nFlags & ANNOTFLAG_NOVIEW));
}

CPDF_Action CPDFSDK_BAAnnot::GetAction() const {
  return CPDF_Action(m_pAnnot->GetAnnotDict()->GetDictBy("A"));
}

void CPDFSDK_BAAnnot::SetAction(const CPDF_Action& action) {
  ASSERT(action.GetDict());
  if (action.GetDict() != m_pAnnot->GetAnnotDict()->GetDictBy("A")) {
    CPDF_Document* pDoc = m_pPageView->GetPDFDocument();
    CPDF_Dictionary* pDict = action.GetDict();
    if (pDict && pDict->GetObjNum() == 0) {
      pDoc->AddIndirectObject(pDict);
    }
    m_pAnnot->GetAnnotDict()->SetAtReference("A", pDoc, pDict->GetObjNum());
  }
}

void CPDFSDK_BAAnnot::RemoveAction() {
  m_pAnnot->GetAnnotDict()->RemoveAt("A");
}

CPDF_AAction CPDFSDK_BAAnnot::GetAAction() const {
  return CPDF_AAction(m_pAnnot->GetAnnotDict()->GetDictBy("AA"));
}

void CPDFSDK_BAAnnot::SetAAction(const CPDF_AAction& aa) {
  if (aa.GetDict() != m_pAnnot->GetAnnotDict()->GetDictBy("AA"))
    m_pAnnot->GetAnnotDict()->SetAt("AA", aa.GetDict());
}

void CPDFSDK_BAAnnot::RemoveAAction() {
  m_pAnnot->GetAnnotDict()->RemoveAt("AA");
}

CPDF_Action CPDFSDK_BAAnnot::GetAAction(CPDF_AAction::AActionType eAAT) {
  CPDF_AAction AAction = GetAAction();

  if (AAction.ActionExist(eAAT))
    return AAction.GetAction(eAAT);

  if (eAAT == CPDF_AAction::ButtonUp)
    return GetAction();

  return CPDF_Action();
}

void CPDFSDK_BAAnnot::Annot_OnDraw(CFX_RenderDevice* pDevice,
                                   CFX_Matrix* pUser2Device,
                                   CPDF_RenderOptions* pOptions) {
  m_pAnnot->GetAPForm(m_pPageView->GetPDFPage(), CPDF_Annot::Normal);
  m_pAnnot->DrawAppearance(m_pPageView->GetPDFPage(), pDevice, pUser2Device,
                           CPDF_Annot::Normal, nullptr);
}

UnderlyingPageType* CPDFSDK_Annot::GetUnderlyingPage() {
#ifdef PDF_ENABLE_XFA
  return GetPDFXFAPage();
#else   // PDF_ENABLE_XFA
  return GetPDFPage();
#endif  // PDF_ENABLE_XFA
}

CPDF_Page* CPDFSDK_Annot::GetPDFPage() {
  return m_pPageView ? m_pPageView->GetPDFPage() : nullptr;
}

#ifdef PDF_ENABLE_XFA

CPDFXFA_Page* CPDFSDK_Annot::GetPDFXFAPage() {
  return m_pPageView ? m_pPageView->GetPDFXFAPage() : nullptr;
}

#endif  // PDF_ENABLE_XFA
