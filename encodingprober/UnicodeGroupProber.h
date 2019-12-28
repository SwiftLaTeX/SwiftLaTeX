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

#ifndef UNICODEGROUPPROBER_H
#define UNICODEGROUPPROBER_H

#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"

#define NUM_OF_UNICODE_CHARSETS   3
namespace qencodingprober {
class  UnicodeGroupProber: public nsCharSetProber {
public:
  UnicodeGroupProber(void);
  virtual ~UnicodeGroupProber(void);
  nsProbingState HandleData(const char* aBuf, unsigned int aLen);
  const char* GetCharSetName() {return mDetectedCharset;};
  nsProbingState GetState(void) {return mState;};
  void      Reset(void);
  float     GetConfidence();
  void      SetOpion() {};
#ifdef DEBUG_PROBE
  void DumpStatus();
#endif

protected:
  void      GetDistribution(unsigned int aCharLen, const char* aStr);
  
  nsCodingStateMachine* mCodingSM[NUM_OF_UNICODE_CHARSETS] ;
  unsigned int    mActiveSM;
  nsProbingState mState;
  const char *  mDetectedCharset;
};
}
#endif /* UNICODEGROUPPROBER_H */

