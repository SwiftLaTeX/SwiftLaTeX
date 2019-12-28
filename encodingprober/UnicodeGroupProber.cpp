/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*  -*- C++ -*-
*  Copyright (C) 2008 <wkai@gmail.com>
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

#include "UnicodeGroupProber.h"

#include "ctype_test_p.h"

#include <QtAlgorithms>
#include <math.h>

namespace qencodingprober {
UnicodeGroupProber::UnicodeGroupProber(void)
{
  mCodingSM[0] = new nsCodingStateMachine(&UTF8SMModel);
  mCodingSM[1] = new nsCodingStateMachine(&UCS2LESMModel);
  mCodingSM[2] = new nsCodingStateMachine(&UCS2BESMModel);
  mActiveSM = NUM_OF_UNICODE_CHARSETS;
  mState = eDetecting;
  mDetectedCharset = "UTF-8";
}

UnicodeGroupProber::~UnicodeGroupProber(void)
{
  for (unsigned int i = 0; i < NUM_OF_UNICODE_CHARSETS; i++)
    delete mCodingSM[i];
}

void UnicodeGroupProber::Reset(void)
{
  mState = eDetecting;
  for (unsigned int i = 0; i < NUM_OF_UNICODE_CHARSETS; i++)
    mCodingSM[i]->Reset();
  mActiveSM = NUM_OF_UNICODE_CHARSETS;
  mDetectedCharset = "UTF-8";
}

nsProbingState UnicodeGroupProber::HandleData(const char* aBuf, unsigned int aLen)
{
  nsSMState codingState;
  int j;
  uint i, weight_BOM, counts[5];
  static bool disableUTF16LE = false;
  static bool disableUTF16BE = false;
  double weight_zero;
  
  if (mActiveSM <= 0) {
      mState = eNotMe;
      return mState;
  }
  
  if (! (disableUTF16LE || disableUTF16BE)) {
    if (aLen%2 != 0) {
            disableUTF16LE = true;
            disableUTF16BE = true;
    }      
    weight_BOM = (uint)(sqrt((double)aLen) + aLen/10.0);
    for (uint i = 0; i < 5; i++) 
        qCount(aBuf, aBuf+aLen, char(i), counts[i]);
    weight_zero = (2.0*(counts[0] + counts[1] + counts[2] + counts[3] + counts[4]) + weight_BOM)/aLen;
    if (weight_zero < log(1.4142)) {
        disableUTF16LE = true;
        disableUTF16BE = true;
    }
    if (4 >= aBuf[1] && aBuf[1] >= 0 && isprint(aBuf[0]))
        disableUTF16BE = true;
    else 
        disableUTF16LE = true;
    if (disableUTF16BE)
      mActiveSM--;
    if (disableUTF16LE) {
      nsCodingStateMachine* t;
      t = mCodingSM[1];
      mCodingSM[1] = mCodingSM[2];
      mCodingSM[2] = t;
      mActiveSM--;
    }
  }
  
  for (i = 0; i < aLen; ++i) {
    for (j = mActiveSM-1; j>= 0; --j)
    {
      //byte is feed to all active state machine 
      codingState = mCodingSM[j]->NextState(aBuf[i]);
      if (codingState == eError)
      {
        //got negative answer for this state machine, make it inactive
        mActiveSM--;
        if (mActiveSM == 0)
        {
          mState = eNotMe;
          return mState;
        }
        else if (j != (int)mActiveSM)
        {
          nsCodingStateMachine* t;
          t = mCodingSM[mActiveSM];
          mCodingSM[mActiveSM] = mCodingSM[j];
          mCodingSM[j] = t;
        }
      }
      else if (codingState == eItsMe)
      {
        mState = eFoundIt;
        mDetectedCharset = mCodingSM[j]->GetCodingStateMachine();
        return mState;
      } else if (mState == eDetecting)
          mDetectedCharset = mCodingSM[j]->GetCodingStateMachine();;
    }
  }
  return mState;
}

float UnicodeGroupProber::GetConfidence()
{
  if (mState == eFoundIt)
    return 0.99f;
  else
    return 0.0f;
}

#ifdef DEBUG_PROBE
void UnicodeGroupProber::DumpStatus()
{
    GetConfidence();
    for (uint i = 0; i < mActiveSM; i++)
    {
        kDebug(180) << "Unicode group" << mCodingSM[i]->DumpCurrentState() << mCodingSM[i]->GetCodingStateMachine() ;
    }
}
#endif

}


