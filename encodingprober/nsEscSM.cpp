/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*  -*- C++ -*-
*  Copyright (C) 1998 <developer@mozilla.org>
*
*
*  Permission is hereby granted, free of charge, to any person obtaining
*  a copy of this software and associated documentation files (the
*  "Software"), to deal in the Software without restriction, including
*  without limitation the rights to use, copy, modify, merge, publish,
*  distribute, sublicense, and/or sell copies of the Software, and to
*  permit persons to whom the Software is furnished to do so, subject to
*  the following conditions:
*
*  The above copyright notice and this permission notice shall be included 
*  in all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
*  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
*  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
*  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "nsCodingStateMachine.h"

namespace qencodingprober {
static unsigned int HZ_cls[ 256 / 8 ] = {
PCK4BITS(1,0,0,0,0,0,0,0),  // 00 - 07 
PCK4BITS(0,0,0,0,0,0,0,0),  // 08 - 0f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 10 - 17 
PCK4BITS(0,0,0,1,0,0,0,0),  // 18 - 1f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 20 - 27 
PCK4BITS(0,0,0,0,0,0,0,0),  // 28 - 2f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 30 - 37 
PCK4BITS(0,0,0,0,0,0,0,0),  // 38 - 3f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 40 - 47 
PCK4BITS(0,0,0,0,0,0,0,0),  // 48 - 4f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 50 - 57 
PCK4BITS(0,0,0,0,0,0,0,0),  // 58 - 5f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 60 - 67 
PCK4BITS(0,0,0,0,0,0,0,0),  // 68 - 6f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 70 - 77 
PCK4BITS(0,0,0,4,0,5,2,0),  // 78 - 7f 
PCK4BITS(1,1,1,1,1,1,1,1),  // 80 - 87 
PCK4BITS(1,1,1,1,1,1,1,1),  // 88 - 8f 
PCK4BITS(1,1,1,1,1,1,1,1),  // 90 - 97 
PCK4BITS(1,1,1,1,1,1,1,1),  // 98 - 9f 
PCK4BITS(1,1,1,1,1,1,1,1),  // a0 - a7 
PCK4BITS(1,1,1,1,1,1,1,1),  // a8 - af 
PCK4BITS(1,1,1,1,1,1,1,1),  // b0 - b7 
PCK4BITS(1,1,1,1,1,1,1,1),  // b8 - bf 
PCK4BITS(1,1,1,1,1,1,1,1),  // c0 - c7 
PCK4BITS(1,1,1,1,1,1,1,1),  // c8 - cf 
PCK4BITS(1,1,1,1,1,1,1,1),  // d0 - d7 
PCK4BITS(1,1,1,1,1,1,1,1),  // d8 - df 
PCK4BITS(1,1,1,1,1,1,1,1),  // e0 - e7 
PCK4BITS(1,1,1,1,1,1,1,1),  // e8 - ef 
PCK4BITS(1,1,1,1,1,1,1,1),  // f0 - f7 
PCK4BITS(1,1,1,1,1,1,1,1)   // f8 - ff 
};


static unsigned int HZ_st [ 6] = {
PCK4BITS(eStart,eError,     3,eStart,eStart,eStart,eError,eError),//00-07 
PCK4BITS(eError,eError,eError,eError,eItsMe,eItsMe,eItsMe,eItsMe),//08-0f 
PCK4BITS(eItsMe,eItsMe,eError,eError,eStart,eStart,     4,eError),//10-17 
PCK4BITS(     5,eError,     6,eError,     5,     5,     4,eError),//18-1f 
PCK4BITS(     4,eError,     4,     4,     4,eError,     4,eError),//20-27 
PCK4BITS(     4,eItsMe,eStart,eStart,eStart,eStart,eStart,eStart) //28-2f 
};

static const unsigned int HZCharLenTable[] = {0, 0, 0, 0, 0, 0};

SMModel HZSMModel = {
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, HZ_cls },
   6,
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, HZ_st },
  HZCharLenTable,
  "HZ-GB-2312",
};


static unsigned int ISO2022CN_cls [ 256 / 8 ] = {
PCK4BITS(2,0,0,0,0,0,0,0),  // 00 - 07 
PCK4BITS(0,0,0,0,0,0,0,0),  // 08 - 0f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 10 - 17 
PCK4BITS(0,0,0,1,0,0,0,0),  // 18 - 1f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 20 - 27 
PCK4BITS(0,3,0,0,0,0,0,0),  // 28 - 2f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 30 - 37 
PCK4BITS(0,0,0,0,0,0,0,0),  // 38 - 3f 
PCK4BITS(0,0,0,4,0,0,0,0),  // 40 - 47 
PCK4BITS(0,0,0,0,0,0,0,0),  // 48 - 4f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 50 - 57 
PCK4BITS(0,0,0,0,0,0,0,0),  // 58 - 5f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 60 - 67 
PCK4BITS(0,0,0,0,0,0,0,0),  // 68 - 6f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 70 - 77 
PCK4BITS(0,0,0,0,0,0,0,0),  // 78 - 7f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 80 - 87 
PCK4BITS(2,2,2,2,2,2,2,2),  // 88 - 8f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 90 - 97 
PCK4BITS(2,2,2,2,2,2,2,2),  // 98 - 9f 
PCK4BITS(2,2,2,2,2,2,2,2),  // a0 - a7 
PCK4BITS(2,2,2,2,2,2,2,2),  // a8 - af 
PCK4BITS(2,2,2,2,2,2,2,2),  // b0 - b7 
PCK4BITS(2,2,2,2,2,2,2,2),  // b8 - bf 
PCK4BITS(2,2,2,2,2,2,2,2),  // c0 - c7 
PCK4BITS(2,2,2,2,2,2,2,2),  // c8 - cf 
PCK4BITS(2,2,2,2,2,2,2,2),  // d0 - d7 
PCK4BITS(2,2,2,2,2,2,2,2),  // d8 - df 
PCK4BITS(2,2,2,2,2,2,2,2),  // e0 - e7 
PCK4BITS(2,2,2,2,2,2,2,2),  // e8 - ef 
PCK4BITS(2,2,2,2,2,2,2,2),  // f0 - f7 
PCK4BITS(2,2,2,2,2,2,2,2)   // f8 - ff 
};


static unsigned int ISO2022CN_st [ 8] = {
PCK4BITS(eStart,     3,eError,eStart,eStart,eStart,eStart,eStart),//00-07 
PCK4BITS(eStart,eError,eError,eError,eError,eError,eError,eError),//08-0f 
PCK4BITS(eError,eError,eItsMe,eItsMe,eItsMe,eItsMe,eItsMe,eItsMe),//10-17 
PCK4BITS(eItsMe,eItsMe,eItsMe,eError,eError,eError,     4,eError),//18-1f 
PCK4BITS(eError,eError,eError,eItsMe,eError,eError,eError,eError),//20-27 
PCK4BITS(     5,     6,eError,eError,eError,eError,eError,eError),//28-2f 
PCK4BITS(eError,eError,eError,eItsMe,eError,eError,eError,eError),//30-37 
PCK4BITS(eError,eError,eError,eError,eError,eItsMe,eError,eStart) //38-3f 
};

static const unsigned int ISO2022CNCharLenTable[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

SMModel ISO2022CNSMModel = {
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, ISO2022CN_cls },
  9,
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, ISO2022CN_st },
  ISO2022CNCharLenTable,
  "ISO-2022-CN",
};

static unsigned int ISO2022JP_cls [ 256 / 8 ] = {
PCK4BITS(2,0,0,0,0,0,0,0),  // 00 - 07 
PCK4BITS(0,0,0,0,0,0,2,2),  // 08 - 0f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 10 - 17 
PCK4BITS(0,0,0,1,0,0,0,0),  // 18 - 1f 
PCK4BITS(0,0,0,0,7,0,0,0),  // 20 - 27 
PCK4BITS(3,0,0,0,0,0,0,0),  // 28 - 2f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 30 - 37 
PCK4BITS(0,0,0,0,0,0,0,0),  // 38 - 3f 
PCK4BITS(6,0,4,0,8,0,0,0),  // 40 - 47 
PCK4BITS(0,9,5,0,0,0,0,0),  // 48 - 4f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 50 - 57 
PCK4BITS(0,0,0,0,0,0,0,0),  // 58 - 5f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 60 - 67 
PCK4BITS(0,0,0,0,0,0,0,0),  // 68 - 6f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 70 - 77 
PCK4BITS(0,0,0,0,0,0,0,0),  // 78 - 7f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 80 - 87 
PCK4BITS(2,2,2,2,2,2,2,2),  // 88 - 8f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 90 - 97 
PCK4BITS(2,2,2,2,2,2,2,2),  // 98 - 9f 
PCK4BITS(2,2,2,2,2,2,2,2),  // a0 - a7 
PCK4BITS(2,2,2,2,2,2,2,2),  // a8 - af 
PCK4BITS(2,2,2,2,2,2,2,2),  // b0 - b7 
PCK4BITS(2,2,2,2,2,2,2,2),  // b8 - bf 
PCK4BITS(2,2,2,2,2,2,2,2),  // c0 - c7 
PCK4BITS(2,2,2,2,2,2,2,2),  // c8 - cf 
PCK4BITS(2,2,2,2,2,2,2,2),  // d0 - d7 
PCK4BITS(2,2,2,2,2,2,2,2),  // d8 - df 
PCK4BITS(2,2,2,2,2,2,2,2),  // e0 - e7 
PCK4BITS(2,2,2,2,2,2,2,2),  // e8 - ef 
PCK4BITS(2,2,2,2,2,2,2,2),  // f0 - f7 
PCK4BITS(2,2,2,2,2,2,2,2)   // f8 - ff 
};


static unsigned int ISO2022JP_st [ 9] = {
PCK4BITS(eStart,     3,eError,eStart,eStart,eStart,eStart,eStart),//00-07 
PCK4BITS(eStart,eStart,eError,eError,eError,eError,eError,eError),//08-0f 
PCK4BITS(eError,eError,eError,eError,eItsMe,eItsMe,eItsMe,eItsMe),//10-17 
PCK4BITS(eItsMe,eItsMe,eItsMe,eItsMe,eItsMe,eItsMe,eError,eError),//18-1f 
PCK4BITS(eError,     5,eError,eError,eError,     4,eError,eError),//20-27 
PCK4BITS(eError,eError,eError,     6,eItsMe,eError,eItsMe,eError),//28-2f 
PCK4BITS(eError,eError,eError,eError,eError,eError,eItsMe,eItsMe),//30-37 
PCK4BITS(eError,eError,eError,eItsMe,eError,eError,eError,eError),//38-3f 
PCK4BITS(eError,eError,eError,eError,eItsMe,eError,eStart,eStart) //40-47 
};

static const unsigned int ISO2022JPCharLenTable[] = {0, 0, 0, 0, 0, 0, 0, 0};

SMModel ISO2022JPSMModel = {
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, ISO2022JP_cls },
  10,
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, ISO2022JP_st },
  ISO2022JPCharLenTable,
  "ISO-2022-JP",
};

static unsigned int ISO2022KR_cls [ 256 / 8 ] = {
PCK4BITS(2,0,0,0,0,0,0,0),  // 00 - 07 
PCK4BITS(0,0,0,0,0,0,0,0),  // 08 - 0f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 10 - 17 
PCK4BITS(0,0,0,1,0,0,0,0),  // 18 - 1f 
PCK4BITS(0,0,0,0,3,0,0,0),  // 20 - 27 
PCK4BITS(0,4,0,0,0,0,0,0),  // 28 - 2f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 30 - 37 
PCK4BITS(0,0,0,0,0,0,0,0),  // 38 - 3f 
PCK4BITS(0,0,0,5,0,0,0,0),  // 40 - 47 
PCK4BITS(0,0,0,0,0,0,0,0),  // 48 - 4f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 50 - 57 
PCK4BITS(0,0,0,0,0,0,0,0),  // 58 - 5f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 60 - 67 
PCK4BITS(0,0,0,0,0,0,0,0),  // 68 - 6f 
PCK4BITS(0,0,0,0,0,0,0,0),  // 70 - 77 
PCK4BITS(0,0,0,0,0,0,0,0),  // 78 - 7f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 80 - 87 
PCK4BITS(2,2,2,2,2,2,2,2),  // 88 - 8f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 90 - 97 
PCK4BITS(2,2,2,2,2,2,2,2),  // 98 - 9f 
PCK4BITS(2,2,2,2,2,2,2,2),  // a0 - a7 
PCK4BITS(2,2,2,2,2,2,2,2),  // a8 - af 
PCK4BITS(2,2,2,2,2,2,2,2),  // b0 - b7 
PCK4BITS(2,2,2,2,2,2,2,2),  // b8 - bf 
PCK4BITS(2,2,2,2,2,2,2,2),  // c0 - c7 
PCK4BITS(2,2,2,2,2,2,2,2),  // c8 - cf 
PCK4BITS(2,2,2,2,2,2,2,2),  // d0 - d7 
PCK4BITS(2,2,2,2,2,2,2,2),  // d8 - df 
PCK4BITS(2,2,2,2,2,2,2,2),  // e0 - e7 
PCK4BITS(2,2,2,2,2,2,2,2),  // e8 - ef 
PCK4BITS(2,2,2,2,2,2,2,2),  // f0 - f7 
PCK4BITS(2,2,2,2,2,2,2,2)   // f8 - ff 
};


static unsigned int ISO2022KR_st [ 5] = {
PCK4BITS(eStart,     3,eError,eStart,eStart,eStart,eError,eError),//00-07 
PCK4BITS(eError,eError,eError,eError,eItsMe,eItsMe,eItsMe,eItsMe),//08-0f 
PCK4BITS(eItsMe,eItsMe,eError,eError,eError,     4,eError,eError),//10-17 
PCK4BITS(eError,eError,eError,eError,     5,eError,eError,eError),//18-1f 
PCK4BITS(eError,eError,eError,eItsMe,eStart,eStart,eStart,eStart) //20-27 
};

static const unsigned int ISO2022KRCharLenTable[] = {0, 0, 0, 0, 0, 0};

SMModel ISO2022KRSMModel = {
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, ISO2022KR_cls },
   6,
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, ISO2022KR_st },
  ISO2022KRCharLenTable,
  "ISO-2022-KR",
};
}


