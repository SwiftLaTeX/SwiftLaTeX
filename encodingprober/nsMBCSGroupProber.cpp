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

#include "nsMBCSGroupProber.h"

#include <stdio.h>
#include <stdlib.h>

namespace qencodingprober {
#ifdef DEBUG_PROBE
static const char* const ProberName[] =
{
  "Unicode",
  "SJIS",
  "EUCJP",
  "GB18030",
  "EUCKR",
  "Big5",
  "EUCTW",
};

#endif

nsMBCSGroupProber::nsMBCSGroupProber()
{
  mProbers[0] = new UnicodeGroupProber();
  mProbers[1] = new nsSJISProber();
  mProbers[2] = new nsEUCJPProber();
  mProbers[3] = new nsGB18030Prober();
  mProbers[4] = new nsEUCKRProber();
  mProbers[5] = new nsBig5Prober();
  mProbers[6] = new nsEUCTWProber();
  Reset();
}

nsMBCSGroupProber::~nsMBCSGroupProber()
{
  for (unsigned int i = 0; i < NUM_OF_PROBERS; i++)
  {
    delete mProbers[i];
  }
}

const char* nsMBCSGroupProber::GetCharSetName()
{
  if (mBestGuess == -1)
  {
    GetConfidence();
    if (mBestGuess == -1)
      mBestGuess = 0;
  }
  return mProbers[mBestGuess]->GetCharSetName();
}

void  nsMBCSGroupProber::Reset(void)
{
  mActiveNum = 0;
  for (unsigned int i = 0; i < NUM_OF_PROBERS; i++)
  {
    if (mProbers[i])
    {
      mProbers[i]->Reset();
      mIsActive[i] = true;
      ++mActiveNum;
    }
    else
      mIsActive[i] = false;
  }
  mBestGuess = -1;
  mState = eDetecting;
}

nsProbingState nsMBCSGroupProber::HandleData(const char* aBuf, unsigned int aLen)
{
  nsProbingState st;
  unsigned int i;

  //do filtering to reduce load to probers
  char *highbyteBuf;
  char *hptr;
  bool keepNext = true;   //assume previous is not ascii, it will do no harm except add some noise
  hptr = highbyteBuf = (char*)malloc(aLen);
  if (!hptr)
      return mState;
  for (i = 0; i < aLen; ++i)
  {
    if (aBuf[i] & 0x80)
    {
      *hptr++ = aBuf[i];
      keepNext = true;
    }
    else
    {
      //if previous is highbyte, keep this even it is a ASCII
      if (keepNext)
      {
          *hptr++ = aBuf[i];
          keepNext = false;
      }
    }
  }

  for (i = 0; i < NUM_OF_PROBERS; ++i)
  {
     if (!mIsActive[i])
       continue;
     st = mProbers[i]->HandleData(highbyteBuf, hptr - highbyteBuf);
     if (st == eFoundIt)
     {
       mBestGuess = i;
       mState = eFoundIt;
       break;
     }
     else if (st == eNotMe)
     {
       mIsActive[i] = false;
       mActiveNum--;
       if (mActiveNum <= 0)
       {
         mState = eNotMe;
         break;
       }
     }
  }

  free(highbyteBuf);

  return mState;
}

float nsMBCSGroupProber::GetConfidence(void)
{
  unsigned int i;
  float bestConf = 0.0, cf;

  switch (mState)
  {
  case eFoundIt:
    return (float)0.99;
  case eNotMe:
    return (float)0.01;
  default:
    for (i = 0; i < NUM_OF_PROBERS; ++i)
    {
      if (!mIsActive[i])
        continue;
      cf = mProbers[i]->GetConfidence();
      if (bestConf < cf)
      {
        bestConf = cf;
        mBestGuess = i;
      }
    }
  }
  return bestConf;
}

#ifdef DEBUG_PROBE
void nsMBCSGroupProber::DumpStatus()
{
  unsigned int i;
  float cf;
  
  GetConfidence();
  for (i = 0; i < NUM_OF_PROBERS; i++)
  {
    if (!mIsActive[i])
      printf("  MBCS inactive: [%s] (confidence is too low).\r\n", ProberName[i]);
    else
    {
      cf = mProbers[i]->GetConfidence();
      printf("  MBCS %1.3f: [%s]\r\n", cf, ProberName[i]);
    }
  }
}
#endif
}


