/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*  -*- C++ -*-
*  Copyright (C) 1998 <developer@mozilla.org>
*  Copyright (C) 2008 <wkai@gmail.com>
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

#include "nsUniversalDetector.h"

#include "nsMBCSGroupProber.h"
#include "nsSBCSGroupProber.h"
#include "nsEscCharsetProber.h"
#include "nsLatin1Prober.h"

namespace qencodingprober {
nsUniversalDetector::nsUniversalDetector()
{
  mDone = false;
  mBestGuess = -1;   //illegal value as signal
  mInTag = false;
  mEscCharSetProber = 0;

  mStart = true;
  mDetectedCharset = 0;
  mGotData = false;
  mInputState = ePureAscii;
  mLastChar = '\0';

  unsigned int i;
  for (i = 0; i < NUM_OF_CHARSET_PROBERS; i++)
    mCharSetProbers[i] = 0;
}

nsUniversalDetector::~nsUniversalDetector()
{
  for (int i = 0; i < NUM_OF_CHARSET_PROBERS; i++)
    delete mCharSetProbers[i];
  delete mEscCharSetProber;
}

void
nsUniversalDetector::Reset()
{
  mDone = false;
  mBestGuess = -1;   //illegal value as signal
  mInTag = false;

  mStart = true;
  mDetectedCharset = 0;
  mGotData = false;
  mInputState = ePureAscii;
  mLastChar = '\0';

  if (mEscCharSetProber)
    mEscCharSetProber->Reset();

  unsigned int i;
  for (i = 0; i < NUM_OF_CHARSET_PROBERS; i++)
    if (mCharSetProbers[i])
      mCharSetProbers[i]->Reset();
}

//---------------------------------------------------------------------
#define SHORTCUT_THRESHOLD      (float)0.95
#define MINIMUM_THRESHOLD      (float)0.20

nsProbingState nsUniversalDetector::HandleData(const char* aBuf, unsigned int aLen)
{
  if(mDone) 
    return eFoundIt;

  if (aLen > 0)
    mGotData = true;

  unsigned int i;
  for (i = 0; i < aLen; i++)
  {
    //other than 0xa0, if every othe character is ascii, the page is ascii
    if (aBuf[i] & '\x80' && aBuf[i] != '\xA0')  //Since many Ascii only page contains NBSP 
    {
      //we got a non-ascii byte (high-byte)
      if (mInputState != eHighbyte)
      {
        //adjust state
        mInputState = eHighbyte;

        //kill mEscCharSetProber if it is active
        delete mEscCharSetProber;
        mEscCharSetProber = 0;

        //start multibyte and singlebyte charset prober
        if (0 == mCharSetProbers[0])
          mCharSetProbers[0] = new nsMBCSGroupProber;
        if (0 == mCharSetProbers[1])
          mCharSetProbers[1] = new nsSBCSGroupProber;
        if (0 == mCharSetProbers[2])
          mCharSetProbers[2] = new nsLatin1Prober; 
      }
    }
    else
    {
      //ok, just pure ascii so far
      if ( ePureAscii == mInputState &&
        (aBuf[i] == '\033' || (aBuf[i] == '{' && mLastChar == '~')) )
      {
        //found escape character or HZ "~{"
        mInputState = eEscAscii;
      }
          
      mLastChar = aBuf[i];
    }
  }

  nsProbingState st = eDetecting;
  switch (mInputState)
  {
  case eEscAscii:
    if (0 == mEscCharSetProber) {
      mEscCharSetProber = new nsEscCharSetProber;
    }
    st = mEscCharSetProber->HandleData(aBuf, aLen);
    if (st == eFoundIt)
    {
      mDone = true;
      mDetectedCharset = mEscCharSetProber->GetCharSetName();
    }
    break;
  case eHighbyte:
    for (i = 0; i < NUM_OF_CHARSET_PROBERS; ++i)
    {
      st = mCharSetProbers[i]->HandleData(aBuf, aLen);
      if (st == eFoundIt) 
      {
        mDone = true;
        mDetectedCharset = mCharSetProbers[i]->GetCharSetName();
      } 
    }
    break;

  default:  //pure ascii
    mDetectedCharset = "UTF-8";
  }
  return st;
}


//---------------------------------------------------------------------
const char* nsUniversalDetector::GetCharSetName()
{
  if (mDetectedCharset)
    return mDetectedCharset;
  switch (mInputState)
  {
  case eHighbyte:
    {
      float proberConfidence;
      float maxProberConfidence = (float)0.0;
      int maxProber = 0;

      for (int i = 0; i < NUM_OF_CHARSET_PROBERS; i++)
      {
        proberConfidence = mCharSetProbers[i]->GetConfidence();
        if (proberConfidence > maxProberConfidence)
        {
          maxProberConfidence = proberConfidence;
          maxProber = i;
        }
      }
      //do not report anything because we are not confident of it, that's in fact a negative answer
      if (maxProberConfidence > MINIMUM_THRESHOLD)
        return mCharSetProbers[maxProber]->GetCharSetName();
    }
  case eEscAscii:
    break;
  default:           // pure ascii
      ;
  }
  return "UTF-8";

}

//---------------------------------------------------------------------
float nsUniversalDetector::GetConfidence()
{
  if (!mGotData)
  {
    // we haven't got any data yet, return immediately
    // caller program sometimes call DataEnd before anything has been sent to detector
    return MINIMUM_THRESHOLD;
  }
  if (mDetectedCharset)
    return 0.99f;
  switch (mInputState)
  {
  case eHighbyte:
    {
      float proberConfidence;
      float maxProberConfidence = (float)0.0;
      int maxProber = 0;

      for (int i = 0; i < NUM_OF_CHARSET_PROBERS; i++)
      {
        proberConfidence = mCharSetProbers[i]->GetConfidence();
        if (proberConfidence > maxProberConfidence)
        {
          maxProberConfidence = proberConfidence;
          maxProber = i;
        }
      }
      //do not report anything because we are not confident of it, that's in fact a negative answer
      if (maxProberConfidence > MINIMUM_THRESHOLD)
        return mCharSetProbers[maxProber]->GetConfidence();
    }
  case eEscAscii:
    break;
  default:           // pure ascii
      ;
  }
  return MINIMUM_THRESHOLD;
}

nsProbingState nsUniversalDetector::GetState()
{
    if (mDone)
        return eFoundIt;
    else
        return eDetecting;
}
}


