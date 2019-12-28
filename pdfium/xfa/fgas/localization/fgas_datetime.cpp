// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fgas/localization/fgas_datetime.h"

#if _FX_OS_ == _FX_LINUX_DESKTOP_ || _FX_OS_ == _FX_ANDROID_ || \
    _FX_OS_ == _FX_MACOSX_ || _FX_OS_ == _FX_IOS_
#include <sys/time.h>
#include <time.h>
#endif

const uint8_t g_FXDaysPerMonth[12] = {31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31};
const uint8_t g_FXDaysPerLeapMonth[12] = {31, 29, 31, 30, 31, 30,
                                          31, 31, 30, 31, 30, 31};
const int32_t g_FXDaysBeforeMonth[12] = {0,   31,  59,  90,  120, 151,
                                         181, 212, 243, 273, 304, 334};
const int32_t g_FXDaysBeforeLeapMonth[12] = {0,   31,  60,  91,  121, 152,
                                             182, 213, 244, 274, 305, 335};
const int32_t g_FXDaysPerYear = 365;
const int32_t g_FXDaysPerLeapYear = 366;
const int32_t g_FXDaysPer4Years = 1461;
const int32_t g_FXDaysPer100Years = 36524;
const int32_t g_FXDaysPer400Years = 146097;
const int64_t g_FXMillisecondsPerSecond = 1000;
const int64_t g_FXMillisecondsPerMinute = 60000;
const int64_t g_FXMillisecondsPerHour = 3600000;
const int64_t g_FXMillisecondsPerDay = 86400000;
FX_BOOL FX_IsLeapYear(int32_t iYear) {
  ASSERT(iYear != 0);
  return ((iYear % 4) == 0 && (iYear % 100) != 0) || (iYear % 400) == 0;
}
int32_t FX_DaysInYear(int32_t iYear) {
  ASSERT(iYear != 0);
  return FX_IsLeapYear(iYear) ? g_FXDaysPerLeapYear : g_FXDaysPerYear;
}
uint8_t FX_DaysInMonth(int32_t iYear, uint8_t iMonth) {
  ASSERT(iYear != 0);
  ASSERT(iMonth >= 1 && iMonth <= 12);
  const uint8_t* p =
      FX_IsLeapYear(iYear) ? g_FXDaysPerLeapMonth : g_FXDaysPerMonth;
  return p[iMonth - 1];
}
static int32_t FX_DaysBeforeMonthInYear(int32_t iYear, uint8_t iMonth) {
  ASSERT(iYear != 0);
  ASSERT(iMonth >= 1 && iMonth <= 12);
  const int32_t* p =
      FX_IsLeapYear(iYear) ? g_FXDaysBeforeLeapMonth : g_FXDaysBeforeMonth;
  return p[iMonth - 1];
}
static int64_t FX_DateToDays(int32_t iYear,
                             uint8_t iMonth,
                             uint8_t iDay,
                             FX_BOOL bIncludeThisDay = FALSE) {
  ASSERT(iYear != 0);
  ASSERT(iMonth >= 1 && iMonth <= 12);
  ASSERT(iDay >= 1 && iDay <= FX_DaysInMonth(iYear, iMonth));
  int64_t iDays = FX_DaysBeforeMonthInYear(iYear, iMonth);
  iDays += iDay;
  if (!bIncludeThisDay) {
    iDays--;
  }
  if (iYear > 0) {
    iYear--;
  } else {
    iDays -= FX_DaysInYear(iYear);
    iYear++;
  }
  return iDays + (int64_t)iYear * 365 + iYear / 4 - iYear / 100 + iYear / 400;
}
static void FX_DaysToDate(int64_t iDays,
                          int32_t& iYear,
                          uint8_t& iMonth,
                          uint8_t& iDay) {
  FX_BOOL bBC = iDays < 0;
  if (bBC) {
    iDays = -iDays;
  }
  iYear = 1;
  iMonth = 1;
  iDay = 1;
  if (iDays >= g_FXDaysPer400Years) {
    iYear += (int32_t)(iDays / g_FXDaysPer400Years * 400);
    iDays %= g_FXDaysPer400Years;
  }
  if (iDays >= g_FXDaysPer100Years) {
    if (iDays == g_FXDaysPer100Years * 4) {
      iYear += 300;
      iDays -= g_FXDaysPer100Years * 3;
    } else {
      iYear += (int32_t)(iDays / g_FXDaysPer100Years * 100);
      iDays %= g_FXDaysPer100Years;
    }
  }
  if (iDays >= g_FXDaysPer4Years) {
    iYear += (int32_t)(iDays / g_FXDaysPer4Years * 4);
    iDays %= g_FXDaysPer4Years;
  }
  while (TRUE) {
    int32_t iYearDays = FX_DaysInYear(iYear);
    if (iDays < iYearDays) {
      if (bBC) {
        iYear = -iYear;
        iDays = iYearDays - iDays;
      }
      break;
    }
    iYear++;
    iDays -= iYearDays;
  }
  while (TRUE) {
    int32_t iMonthDays = FX_DaysInMonth(iYear, iMonth);
    if (iDays < iMonthDays) {
      break;
    }
    iMonth++;
    iDays -= iMonthDays;
  }
  iDay += (uint8_t)iDays;
}

struct FXUT_SYSTEMTIME {
  uint16_t wYear;
  uint16_t wMonth;
  uint16_t wDayOfWeek;
  uint16_t wDay;
  uint16_t wHour;
  uint16_t wMinute;
  uint16_t wSecond;
  uint16_t wMilliseconds;
};

void CFX_Unitime::Now() {
  FXUT_SYSTEMTIME utLocal;
#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN32_MOBILE_ || \
    _FX_OS_ == _FX_WIN64_
  ::GetLocalTime((LPSYSTEMTIME)&utLocal);
#elif _FX_OS_ != _FX_EMBEDDED_
#if 1
  timeval curTime;
  gettimeofday(&curTime, nullptr);
#else
  struct timespec curTime;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &curTime);
#endif
  struct tm st;
  localtime_r(&curTime.tv_sec, &st);
  utLocal.wYear = st.tm_year + 1900;
  utLocal.wMonth = st.tm_mon + 1;
  utLocal.wDayOfWeek = st.tm_wday;
  utLocal.wDay = st.tm_mday;
  utLocal.wHour = st.tm_hour;
  utLocal.wMinute = st.tm_min;
  utLocal.wSecond = st.tm_sec;
  utLocal.wMilliseconds = curTime.tv_usec / 1000;
#endif
  Set(utLocal.wYear, (uint8_t)utLocal.wMonth, (uint8_t)utLocal.wDay,
      (uint8_t)utLocal.wHour, (uint8_t)utLocal.wMinute,
      (uint8_t)utLocal.wSecond, (uint16_t)utLocal.wMilliseconds);
}
void CFX_Unitime::SetGMTime() {
  FXUT_SYSTEMTIME utLocal;
#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN32_MOBILE_ || \
    _FX_OS_ == _FX_WIN64_
  ::GetSystemTime((LPSYSTEMTIME)&utLocal);
#elif _FX_OS_ != _FX_EMBEDDED_
#if 1
  timeval curTime;
  gettimeofday(&curTime, nullptr);
#else
  struct timespec curTime;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &curTime);
#endif
  struct tm st;
  gmtime_r(&curTime.tv_sec, &st);
  utLocal.wYear = st.tm_year + 1900;
  utLocal.wMonth = st.tm_mon + 1;
  utLocal.wDayOfWeek = st.tm_wday;
  utLocal.wDay = st.tm_mday;
  utLocal.wHour = st.tm_hour;
  utLocal.wMinute = st.tm_min;
  utLocal.wSecond = st.tm_sec;
  utLocal.wMilliseconds = curTime.tv_usec / 1000;
#endif
  Set(utLocal.wYear, (uint8_t)utLocal.wMonth, (uint8_t)utLocal.wDay,
      (uint8_t)utLocal.wHour, (uint8_t)utLocal.wMinute,
      (uint8_t)utLocal.wSecond, (uint16_t)utLocal.wMilliseconds);
}
void CFX_Unitime::Set(int32_t year,
                      uint8_t month,
                      uint8_t day,
                      uint8_t hour,
                      uint8_t minute,
                      uint8_t second,
                      uint16_t millisecond) {
  ASSERT(hour <= 23);
  ASSERT(minute <= 59);
  ASSERT(second <= 59);
  ASSERT(millisecond <= 999);
  m_iUnitime = (int64_t)hour * g_FXMillisecondsPerHour +
               (int64_t)minute * g_FXMillisecondsPerMinute +
               (int64_t)second * g_FXMillisecondsPerSecond + millisecond;
  if (year > 0) {
    m_iUnitime =
        m_iUnitime +
        FX_DateToDays(year, month, day, FALSE) * g_FXMillisecondsPerDay;
  }
}
void CFX_Unitime::Set(FX_UNITIME t) {
  m_iUnitime = t;
}
int32_t CFX_Unitime::GetYear() const {
  int32_t iYear;
  uint8_t iMonth, iDay;
  FX_DaysToDate(GetDayOfAD(), iYear, iMonth, iDay);
  return iYear;
}
uint8_t CFX_Unitime::GetMonth() const {
  int32_t iYear;
  uint8_t iMonth, iDay;
  FX_DaysToDate(GetDayOfAD(), iYear, iMonth, iDay);
  return iMonth;
}
uint8_t CFX_Unitime::GetDay() const {
  int32_t iYear;
  uint8_t iMonth, iDay;
  FX_DaysToDate(GetDayOfAD(), iYear, iMonth, iDay);
  return iDay;
}
FX_WEEKDAY CFX_Unitime::GetDayOfWeek() const {
  int32_t v = (int32_t)((m_iUnitime / g_FXMillisecondsPerDay + 1) % 7);
  if (v < 0) {
    v += 7;
  }
  return (FX_WEEKDAY)v;
}
uint16_t CFX_Unitime::GetDayOfYear() const {
  int32_t iYear;
  uint8_t iMonth, iDay;
  FX_DaysToDate(GetDayOfAD(), iYear, iMonth, iDay);
  return FX_DaysBeforeMonthInYear(iYear, iMonth) + iDay;
}
int64_t CFX_Unitime::GetDayOfAD() const {
  FX_BOOL bBC = m_iUnitime < 0;
  int64_t iDays = m_iUnitime / g_FXMillisecondsPerDay;
  iDays += bBC ? -1 : 0;
  if (bBC && (m_iUnitime % g_FXMillisecondsPerDay) == 0) {
    iDays++;
  }
  return iDays;
}
uint8_t CFX_Unitime::GetHour() const {
  int32_t v = (int32_t)(m_iUnitime % g_FXMillisecondsPerDay);
  if (v < 0) {
    v += g_FXMillisecondsPerDay;
  }
  return (uint8_t)(v / g_FXMillisecondsPerHour);
}
uint8_t CFX_Unitime::GetMinute() const {
  int32_t v = (int32_t)(m_iUnitime % g_FXMillisecondsPerHour);
  if (v < 0) {
    v += g_FXMillisecondsPerHour;
  }
  return (uint8_t)(v / g_FXMillisecondsPerMinute);
}
uint8_t CFX_Unitime::GetSecond() const {
  int32_t v = (int32_t)(m_iUnitime % g_FXMillisecondsPerMinute);
  if (v < 0) {
    v += g_FXMillisecondsPerMinute;
  }
  return (uint8_t)(v / g_FXMillisecondsPerSecond);
}
uint16_t CFX_Unitime::GetMillisecond() const {
  int32_t v = (int32_t)(m_iUnitime % g_FXMillisecondsPerSecond);
  if (v < 0) {
    v += g_FXMillisecondsPerSecond;
  }
  return (uint16_t)v;
}
FX_BOOL CFX_Unitime::AddYears(int32_t iYears) {
  FX_UNITIME ut = m_iUnitime;
  if (ut < 0) {
    ut = -ut;
  }
  FX_UNITIME r = ut % g_FXMillisecondsPerDay;
  int32_t iYear;
  uint8_t iMonth, iDay;
  FX_DaysToDate(GetDayOfAD(), iYear, iMonth, iDay);
  iYear += iYears;
  if (iYear == 0) {
    iYear = iYears > 0 ? 1 : -1;
  }
  m_iUnitime =
      FX_DateToDays(iYear, iMonth, iDay, FALSE) * g_FXMillisecondsPerDay;
  m_iUnitime += (iYear < 0) ? -r : r;
  return TRUE;
}
FX_BOOL CFX_Unitime::AddMonths(int32_t iMonths) {
  FX_BOOL b = iMonths > 0;
  FX_UNITIME ut = m_iUnitime;
  if (ut < 0) {
    ut = -ut;
  }
  FX_UNITIME r = ut % g_FXMillisecondsPerDay;
  int32_t iYear;
  uint8_t iMonth, iDay;
  FX_DaysToDate(GetDayOfAD(), iYear, iMonth, iDay);
  iMonths += iMonth;
  while (iMonths < 1) {
    iYear--, iMonths += 12;
  }
  while (iMonths > 12) {
    iYear++, iMonths -= 12;
  }
  if (iYear == 0) {
    iYear = b ? 1 : -1;
  }
  m_iUnitime = FX_DateToDays(iYear, (uint8_t)iMonths, iDay, FALSE) *
               g_FXMillisecondsPerDay;
  m_iUnitime += (iYear < 0) ? -r : r;
  return TRUE;
}
FX_BOOL CFX_Unitime::AddDays(int32_t iDays) {
  m_iUnitime += (int64_t)iDays * g_FXMillisecondsPerDay;
  return TRUE;
}
FX_BOOL CFX_Unitime::AddHours(int32_t iHours) {
  m_iUnitime += (int64_t)iHours * g_FXMillisecondsPerHour;
  return TRUE;
}
FX_BOOL CFX_Unitime::AddMinutes(int32_t iMinutes) {
  m_iUnitime += (int64_t)iMinutes * g_FXMillisecondsPerMinute;
  return TRUE;
}
FX_BOOL CFX_Unitime::AddSeconds(int32_t iSeconds) {
  m_iUnitime += ((int64_t)iSeconds) * g_FXMillisecondsPerSecond;
  return TRUE;
}
FX_BOOL CFX_Unitime::AddMilliseconds(int32_t iMilliseconds) {
  m_iUnitime += iMilliseconds;
  return TRUE;
}
FX_BOOL CFX_DateTime::Set(int32_t year,
                          uint8_t month,
                          uint8_t day,
                          uint8_t hour,
                          uint8_t minute,
                          uint8_t second,
                          uint16_t millisecond) {
  ASSERT(year != 0);
  ASSERT(month >= 1 && month <= 12);
  ASSERT(day >= 1 && day <= FX_DaysInMonth(year, month));
  ASSERT(hour <= 23);
  ASSERT(minute <= 59);
  ASSERT(second <= 59);
  ASSERT(millisecond <= 999);
  m_DateTime.Date.sDate.year = year;
  m_DateTime.Date.sDate.month = month;
  m_DateTime.Date.sDate.day = day;
  m_DateTime.Time.sTime.hour = hour;
  m_DateTime.Time.sTime.minute = minute;
  m_DateTime.Time.sTime.second = second;
  m_DateTime.Time.sTime.millisecond = millisecond;
  return TRUE;
}
FX_BOOL CFX_DateTime::FromUnitime(FX_UNITIME t) {
  CFX_Unitime ut(t);
  FX_DaysToDate(ut.GetDayOfAD(), m_DateTime.Date.sDate.year,
                m_DateTime.Date.sDate.month, m_DateTime.Date.sDate.day);
  m_DateTime.Date.sDate.day = ut.GetHour();
  m_DateTime.Time.sTime.minute = ut.GetMinute();
  m_DateTime.Time.sTime.second = ut.GetSecond();
  m_DateTime.Time.sTime.millisecond = ut.GetMillisecond();
  return TRUE;
}
FX_UNITIME CFX_DateTime::ToUnitime() const {
  FX_UNITIME v =
      (int64_t)m_DateTime.Date.sDate.day * g_FXMillisecondsPerHour +
      (int64_t)m_DateTime.Time.sTime.minute * g_FXMillisecondsPerMinute +
      (int64_t)m_DateTime.Time.sTime.second * g_FXMillisecondsPerSecond +
      m_DateTime.Time.sTime.millisecond;
  v += FX_DateToDays(m_DateTime.Date.sDate.year, m_DateTime.Date.sDate.month,
                     m_DateTime.Date.sDate.day, FALSE) *
       g_FXMillisecondsPerDay;
  return v;
}
int32_t CFX_DateTime::GetYear() const {
  return m_DateTime.Date.sDate.year;
}
uint8_t CFX_DateTime::GetMonth() const {
  return m_DateTime.Date.sDate.month;
}
uint8_t CFX_DateTime::GetDay() const {
  return m_DateTime.Date.sDate.day;
}
FX_WEEKDAY CFX_DateTime::GetDayOfWeek() const {
  int32_t v = (int32_t)(FX_DateToDays(m_DateTime.Date.sDate.year,
                                      m_DateTime.Date.sDate.month,
                                      m_DateTime.Date.sDate.day, TRUE) %
                        7);
  if (v < 0) {
    v += 7;
  }
  return (FX_WEEKDAY)v;
}
uint16_t CFX_DateTime::GetDayOfYear() const {
  return FX_DaysBeforeMonthInYear(m_DateTime.Date.sDate.year,
                                  m_DateTime.Date.sDate.month) +
         m_DateTime.Date.sDate.day;
}
int64_t CFX_DateTime::GetDayOfAD() const {
  return FX_DateToDays(m_DateTime.Date.sDate.year, m_DateTime.Date.sDate.month,
                       m_DateTime.Date.sDate.day, TRUE);
}
uint8_t CFX_DateTime::GetHour() const {
  return m_DateTime.Date.sDate.day;
}
uint8_t CFX_DateTime::GetMinute() const {
  return m_DateTime.Time.sTime.minute;
}
uint8_t CFX_DateTime::GetSecond() const {
  return m_DateTime.Time.sTime.second;
}
uint16_t CFX_DateTime::GetMillisecond() const {
  return m_DateTime.Time.sTime.millisecond;
}
FX_BOOL CFX_DateTime::AddYears(int32_t iYears) {
  if (iYears == 0) {
    return FALSE;
  }
  int32_t v = m_DateTime.Date.sDate.year + iYears;
  if (v >= 0 && m_DateTime.Date.sDate.year < 0) {
    v++;
  } else if (v <= 0 && m_DateTime.Date.sDate.year > 0) {
    v--;
  }
  m_DateTime.Date.sDate.year = v;
  return TRUE;
}
FX_BOOL CFX_DateTime::AddMonths(int32_t iMonths) {
  if (iMonths == 0) {
    return FALSE;
  }
  FX_BOOL b = iMonths > 0;
  iMonths += m_DateTime.Date.sDate.month;
  while (iMonths < 1) {
    m_DateTime.Date.sDate.year--;
    if (m_DateTime.Date.sDate.year == 0) {
      m_DateTime.Date.sDate.year = -1;
    }
    iMonths += 12;
  }
  while (iMonths > 12) {
    m_DateTime.Date.sDate.year++;
    if (m_DateTime.Date.sDate.year == 0) {
      m_DateTime.Date.sDate.year = 1;
    }
    iMonths -= 12;
  }
  if (m_DateTime.Date.sDate.year == 0) {
    m_DateTime.Date.sDate.year = b ? 1 : -1;
  }
  m_DateTime.Date.sDate.month = (uint8_t)iMonths;
  return TRUE;
}
FX_BOOL CFX_DateTime::AddDays(int32_t iDays) {
  if (iDays == 0) {
    return FALSE;
  }
  int64_t v1 =
      FX_DateToDays(m_DateTime.Date.sDate.year, m_DateTime.Date.sDate.month,
                    m_DateTime.Date.sDate.day, TRUE);
  int64_t v2 = v1 + iDays;
  if (v2 <= 0 && v1 > 0) {
    v2--;
  } else if (v2 >= 0 && v1 < 0) {
    v2++;
  }
  FX_DaysToDate(v2, m_DateTime.Date.sDate.year, m_DateTime.Date.sDate.month,
                m_DateTime.Date.sDate.day);
  return TRUE;
}
FX_BOOL CFX_DateTime::AddHours(int32_t iHours) {
  if (iHours == 0) {
    return FALSE;
  }
  iHours += m_DateTime.Date.sDate.day;
  int32_t iDays = iHours / 24;
  iHours %= 24;
  if (iHours < 0) {
    iDays--, iHours += 24;
  }
  m_DateTime.Date.sDate.day = (uint8_t)iHours;
  if (iDays != 0) {
    AddDays(iDays);
  }
  return TRUE;
}
FX_BOOL CFX_DateTime::AddMinutes(int32_t iMinutes) {
  if (iMinutes == 0) {
    return FALSE;
  }
  iMinutes += m_DateTime.Time.sTime.minute;
  int32_t iHours = iMinutes / 60;
  iMinutes %= 60;
  if (iMinutes < 0) {
    iHours--, iMinutes += 60;
  }
  m_DateTime.Time.sTime.minute = (uint8_t)iMinutes;
  if (iHours != 0) {
    AddHours(iHours);
  }
  return TRUE;
}
FX_BOOL CFX_DateTime::AddSeconds(int32_t iSeconds) {
  if (iSeconds == 0) {
    return FALSE;
  }
  iSeconds += m_DateTime.Time.sTime.second;
  int32_t iMinutes = iSeconds / 60;
  iSeconds %= 60;
  if (iSeconds < 0) {
    iMinutes--, iSeconds += 60;
  }
  m_DateTime.Time.sTime.second = (uint8_t)iSeconds;
  if (iMinutes != 0) {
    AddMinutes(iMinutes);
  }
  return TRUE;
}
FX_BOOL CFX_DateTime::AddMilliseconds(int32_t iMilliseconds) {
  if (iMilliseconds == 0) {
    return FALSE;
  }
  iMilliseconds += m_DateTime.Time.sTime.millisecond;
  int32_t iSeconds = (int32_t)(iMilliseconds / g_FXMillisecondsPerSecond);
  iMilliseconds %= g_FXMillisecondsPerSecond;
  if (iMilliseconds < 0) {
    iSeconds--, iMilliseconds += g_FXMillisecondsPerSecond;
  }
  m_DateTime.Time.sTime.millisecond = (uint16_t)iMilliseconds;
  if (iSeconds != 0) {
    AddSeconds(iSeconds);
  }
  return TRUE;
}
