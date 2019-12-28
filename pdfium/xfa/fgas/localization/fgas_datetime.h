// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LOCALIZATION_FGAS_DATETIME_H_
#define XFA_FGAS_LOCALIZATION_FGAS_DATETIME_H_

#include "core/fxcrt/include/fx_system.h"

class CFX_Unitime;
class CFX_DateTime;

typedef int64_t FX_UNITIME;
enum FX_WEEKDAY {
  FX_Sunday = 0,
  FX_Monday,
  FX_Tuesday,
  FX_Wednesday,
  FX_Thursday,
  FX_Friday,
  FX_Saturday,
};

FX_BOOL FX_IsLeapYear(int32_t iYear);
int32_t FX_DaysInYear(int32_t iYear);
uint8_t FX_DaysInMonth(int32_t iYear, uint8_t iMonth);

class CFX_Unitime {
 public:
  CFX_Unitime() { m_iUnitime = 0; }
  CFX_Unitime(FX_UNITIME iUnitime) { m_iUnitime = iUnitime; }
  CFX_Unitime(const CFX_Unitime& unitime) { m_iUnitime = unitime.m_iUnitime; }
  operator FX_UNITIME*() { return &m_iUnitime; }
  operator FX_UNITIME const*() const { return &m_iUnitime; }
  operator FX_UNITIME&() { return m_iUnitime; }
  operator const FX_UNITIME&() const { return m_iUnitime; }
  CFX_Unitime& operator=(const CFX_Unitime& t) {
    m_iUnitime = t.m_iUnitime;
    return *this;
  }
  CFX_Unitime& operator=(FX_UNITIME t) {
    m_iUnitime = t;
    return *this;
  }
  CFX_Unitime& operator+=(const CFX_Unitime& t) {
    m_iUnitime += t.m_iUnitime;
    return *this;
  }
  CFX_Unitime& operator+=(FX_UNITIME t) {
    m_iUnitime += t;
    return *this;
  }
  CFX_Unitime& operator-=(const CFX_Unitime& t) {
    m_iUnitime -= t.m_iUnitime;
    return *this;
  }
  CFX_Unitime& operator-=(FX_UNITIME t) {
    m_iUnitime -= t;
    return *this;
  }
  void Now();
  void SetGMTime();
  void Set(int32_t year,
           uint8_t month,
           uint8_t day,
           uint8_t hour = 0,
           uint8_t minute = 0,
           uint8_t second = 0,
           uint16_t millisecond = 0);
  void Set(FX_UNITIME t);
  int32_t GetYear() const;
  uint8_t GetMonth() const;
  uint8_t GetDay() const;
  FX_WEEKDAY GetDayOfWeek() const;
  uint16_t GetDayOfYear() const;
  int64_t GetDayOfAD() const;
  uint8_t GetHour() const;
  uint8_t GetMinute() const;
  uint8_t GetSecond() const;
  uint16_t GetMillisecond() const;
  FX_BOOL AddYears(int32_t iYears);
  FX_BOOL AddMonths(int32_t iMonths);
  FX_BOOL AddDays(int32_t iDays);
  FX_BOOL AddHours(int32_t iHours);
  FX_BOOL AddMinutes(int32_t iMinutes);
  FX_BOOL AddSeconds(int32_t iSeconds);
  FX_BOOL AddMilliseconds(int32_t iMilliseconds);
  friend CFX_Unitime operator+(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return CFX_Unitime(t1.m_iUnitime + t2.m_iUnitime);
  }
  friend CFX_Unitime operator+(const CFX_Unitime& t1, FX_UNITIME t2) {
    return CFX_Unitime(t1.m_iUnitime + t2);
  }
  friend CFX_Unitime operator+(FX_UNITIME t1, const CFX_Unitime& t2) {
    return CFX_Unitime(t1 + t2.m_iUnitime);
  }
  friend CFX_Unitime operator-(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return CFX_Unitime(t1.m_iUnitime + t2.m_iUnitime);
  }
  friend CFX_Unitime operator-(const CFX_Unitime& t1, FX_UNITIME t2) {
    return CFX_Unitime(t1.m_iUnitime + t2);
  }
  friend CFX_Unitime operator-(FX_UNITIME t1, const CFX_Unitime& t2) {
    return CFX_Unitime(t1 + t2.m_iUnitime);
  }
  friend bool operator==(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return t1.m_iUnitime == t2.m_iUnitime;
  }
  friend bool operator==(const CFX_Unitime& t1, FX_UNITIME t2) {
    return t1.m_iUnitime == t2;
  }
  friend bool operator==(FX_UNITIME t1, const CFX_Unitime& t2) {
    return t1 == t2.m_iUnitime;
  }
  friend bool operator!=(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return t1.m_iUnitime != t2.m_iUnitime;
  }
  friend bool operator!=(const CFX_Unitime& t1, FX_UNITIME t2) {
    return t1.m_iUnitime != t2;
  }
  friend bool operator!=(FX_UNITIME t1, const CFX_Unitime& t2) {
    return t1 != t2.m_iUnitime;
  }
  friend bool operator>(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return t1.m_iUnitime > t2.m_iUnitime;
  }
  friend bool operator>(const CFX_Unitime& t1, FX_UNITIME t2) {
    return t1.m_iUnitime > t2;
  }
  friend bool operator>(FX_UNITIME t1, const CFX_Unitime& t2) {
    return t1 > t2.m_iUnitime;
  }
  friend bool operator>=(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return t1.m_iUnitime >= t2.m_iUnitime;
  }
  friend bool operator>=(const CFX_Unitime& t1, FX_UNITIME t2) {
    return t1.m_iUnitime >= t2;
  }
  friend bool operator>=(FX_UNITIME t1, const CFX_Unitime& t2) {
    return t1 >= t2.m_iUnitime;
  }
  friend bool operator<(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return t1.m_iUnitime < t2.m_iUnitime;
  }
  friend bool operator<(const CFX_Unitime& t1, FX_UNITIME t2) {
    return t1.m_iUnitime < t2;
  }
  friend bool operator<(FX_UNITIME t1, const CFX_Unitime& t2) {
    return t1 < t2.m_iUnitime;
  }
  friend bool operator<=(const CFX_Unitime& t1, const CFX_Unitime& t2) {
    return t1.m_iUnitime <= t2.m_iUnitime;
  }
  friend bool operator<=(const CFX_Unitime& t1, FX_UNITIME t2) {
    return t1.m_iUnitime <= t2;
  }
  friend bool operator<=(FX_UNITIME t1, const CFX_Unitime& t2) {
    return t1 <= t2.m_iUnitime;
  }

 private:
  FX_UNITIME m_iUnitime;
};

#if _FX_OS_ != _FX_ANDROID_
#pragma pack(push, 1)
#endif
struct FX_DATE {
  int32_t year;
  uint8_t month;
  uint8_t day;
};

struct FX_TIME {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t millisecond;
};

struct FX_TIMEZONE {
  int8_t tzHour;
  uint8_t tzMinute;
};

struct FX_DATETIME {
  union {
    struct {
      int32_t year;
      uint8_t month;
      uint8_t day;
    } sDate;
    FX_DATE aDate;
  } Date;
  union {
    struct {
      uint8_t hour;
      uint8_t minute;
      uint8_t second;
      uint16_t millisecond;
    } sTime;
    FX_TIME aTime;
  } Time;
};

#if _FX_OS_ != _FX_ANDROID_
#pragma pack(pop)
#endif

class CFX_DateTime {
 public:
  CFX_DateTime() {}
  CFX_DateTime(const FX_DATETIME& dt) { m_DateTime = dt; }
  CFX_DateTime(const CFX_DateTime& dt) { m_DateTime = dt.m_DateTime; }
  virtual ~CFX_DateTime() {}
  operator FX_DATETIME*() { return &m_DateTime; }
  operator const FX_DATETIME*() const { return &m_DateTime; }
  operator FX_DATETIME&() { return m_DateTime; }
  operator const FX_DATETIME&() const { return m_DateTime; }
  CFX_DateTime& operator=(const CFX_DateTime& dt) {
    m_DateTime = dt.m_DateTime;
    return *this;
  }
  CFX_DateTime& operator=(const FX_DATETIME& dt) {
    m_DateTime = dt;
    return *this;
  }
  CFX_DateTime& operator+=(const CFX_DateTime& dt) {
    FromUnitime(ToUnitime() + dt.ToUnitime());
    return *this;
  }
  CFX_DateTime& operator+=(const FX_DATETIME& dt) {
    FromUnitime(ToUnitime() + ((const CFX_DateTime&)dt).ToUnitime());
    return *this;
  }
  CFX_DateTime& operator-=(const CFX_DateTime& dt) {
    FromUnitime(ToUnitime() - dt.ToUnitime());
    return *this;
  }
  CFX_DateTime& operator-=(const FX_DATETIME& dt) {
    FromUnitime(ToUnitime() - ((const CFX_DateTime&)dt).ToUnitime());
    return *this;
  }
  virtual FX_BOOL Set(int32_t year,
                      uint8_t month,
                      uint8_t day,
                      uint8_t hour = 0,
                      uint8_t minute = 0,
                      uint8_t second = 0,
                      uint16_t millisecond = 0);
  virtual FX_BOOL FromUnitime(FX_UNITIME t);
  virtual FX_UNITIME ToUnitime() const;
  virtual int32_t GetYear() const;
  virtual uint8_t GetMonth() const;
  virtual uint8_t GetDay() const;
  virtual FX_WEEKDAY GetDayOfWeek() const;
  virtual uint16_t GetDayOfYear() const;
  virtual int64_t GetDayOfAD() const;
  virtual uint8_t GetHour() const;
  virtual uint8_t GetMinute() const;
  virtual uint8_t GetSecond() const;
  virtual uint16_t GetMillisecond() const;
  virtual FX_BOOL AddYears(int32_t iYears);
  virtual FX_BOOL AddMonths(int32_t iMonths);
  virtual FX_BOOL AddDays(int32_t iDays);
  virtual FX_BOOL AddHours(int32_t iHours);
  virtual FX_BOOL AddMinutes(int32_t iMinutes);
  virtual FX_BOOL AddSeconds(int32_t iSeconds);
  virtual FX_BOOL AddMilliseconds(int32_t iMilliseconds);
  friend CFX_DateTime operator+(const CFX_DateTime& dt1,
                                const CFX_DateTime& dt2) {
    CFX_DateTime dt;
    dt.FromUnitime(dt1.ToUnitime() + dt2.ToUnitime());
    return dt;
  }
  friend CFX_DateTime operator+(const CFX_DateTime& dt1,
                                const FX_DATETIME& dt2) {
    CFX_DateTime dt;
    dt.FromUnitime(dt1.ToUnitime() + ((const CFX_DateTime&)dt2).ToUnitime());
    return dt;
  }
  friend CFX_DateTime operator+(const FX_DATETIME& dt1,
                                const CFX_DateTime& dt2) {
    CFX_DateTime dt;
    dt.FromUnitime(((const CFX_DateTime&)dt1).ToUnitime() + dt2.ToUnitime());
    return dt;
  }
  friend CFX_DateTime operator-(const CFX_DateTime& dt1,
                                const CFX_DateTime& dt2) {
    CFX_DateTime dt;
    dt.FromUnitime(dt1.ToUnitime() - dt2.ToUnitime());
    return dt;
  }
  friend CFX_DateTime operator-(const CFX_DateTime& dt1,
                                const FX_DATETIME& dt2) {
    CFX_DateTime dt;
    dt.FromUnitime(dt1.ToUnitime() - ((const CFX_DateTime&)dt2).ToUnitime());
    return dt;
  }
  friend CFX_DateTime operator-(const FX_DATETIME& dt1,
                                const CFX_DateTime& dt2) {
    CFX_DateTime dt;
    dt.FromUnitime(((const CFX_DateTime&)dt1).ToUnitime() - dt2.ToUnitime());
    return dt;
  }
  friend FX_BOOL operator==(const CFX_DateTime& dt1, const CFX_DateTime& dt2) {
    return FXSYS_memcmp(static_cast<const FX_DATETIME*>(dt1),
                        static_cast<const FX_DATETIME*>(dt2),
                        sizeof(FX_DATETIME)) == 0;
  }
  friend FX_BOOL operator==(const CFX_DateTime& dt1, const FX_DATETIME& dt2) {
    return FXSYS_memcmp(static_cast<const FX_DATETIME*>(dt1), &dt2,
                        sizeof(FX_DATETIME)) == 0;
  }
  friend FX_BOOL operator==(const FX_DATETIME& dt1, const CFX_DateTime& dt2) {
    return FXSYS_memcmp(&dt1, static_cast<const FX_DATETIME*>(dt2),
                        sizeof(FX_DATETIME)) == 0;
  }
  friend FX_BOOL operator!=(const CFX_DateTime& dt1, const CFX_DateTime& dt2) {
    return FXSYS_memcmp(static_cast<const FX_DATETIME*>(dt1),
                        static_cast<const FX_DATETIME*>(dt2),
                        sizeof(FX_DATETIME)) != 0;
  }
  friend FX_BOOL operator!=(const CFX_DateTime& dt1, const FX_DATETIME& dt2) {
    return FXSYS_memcmp(static_cast<const FX_DATETIME*>(dt1), &dt2,
                        sizeof(FX_DATETIME)) != 0;
  }
  friend FX_BOOL operator!=(const FX_DATETIME& dt1, const CFX_DateTime& dt2) {
    return FXSYS_memcmp(&dt1, static_cast<const FX_DATETIME*>(dt2),
                        sizeof(FX_DATETIME)) != 0;
  }
  friend FX_BOOL operator>(const CFX_DateTime& dt1, const CFX_DateTime& dt2) {
    return dt1.ToUnitime() > dt2.ToUnitime();
  }
  friend FX_BOOL operator>(const CFX_DateTime& dt1, const FX_DATETIME& dt2) {
    return dt1.ToUnitime() > ((const CFX_DateTime&)dt2).ToUnitime();
  }
  friend FX_BOOL operator>(const FX_DATETIME& dt1, const CFX_DateTime& dt2) {
    return ((const CFX_DateTime&)dt1).ToUnitime() > dt2.ToUnitime();
  }
  friend FX_BOOL operator>=(const CFX_DateTime& dt1, const CFX_DateTime& dt2) {
    return dt1.ToUnitime() >= dt2.ToUnitime();
  }
  friend FX_BOOL operator>=(const CFX_DateTime& dt1, const FX_DATETIME& dt2) {
    return dt1.ToUnitime() >= ((const CFX_DateTime&)dt2).ToUnitime();
  }
  friend FX_BOOL operator>=(const FX_DATETIME& dt1, const CFX_DateTime& dt2) {
    return ((const CFX_DateTime&)dt1).ToUnitime() >= dt2.ToUnitime();
  }
  friend FX_BOOL operator<(const CFX_DateTime& dt1, const CFX_DateTime& dt2) {
    return dt1.ToUnitime() < dt2.ToUnitime();
  }
  friend FX_BOOL operator<(const CFX_DateTime& dt1, const FX_DATETIME& dt2) {
    return dt1.ToUnitime() < ((const CFX_DateTime&)dt2).ToUnitime();
  }
  friend FX_BOOL operator<(const FX_DATETIME& dt1, const CFX_DateTime& dt2) {
    return ((const CFX_DateTime&)dt1).ToUnitime() < dt2.ToUnitime();
  }
  friend FX_BOOL operator<=(const CFX_DateTime& dt1, const CFX_DateTime& dt2) {
    return dt1.ToUnitime() <= dt2.ToUnitime();
  }
  friend FX_BOOL operator<=(const CFX_DateTime& dt1, const FX_DATETIME& dt2) {
    return dt1.ToUnitime() <= ((const CFX_DateTime&)dt2).ToUnitime();
  }
  friend FX_BOOL operator<=(const FX_DATETIME& dt1, const CFX_DateTime& dt2) {
    return ((const CFX_DateTime&)dt1).ToUnitime() <= dt2.ToUnitime();
  }

 private:
  FX_DATETIME m_DateTime;
};

#endif  // XFA_FGAS_LOCALIZATION_FGAS_DATETIME_H_
