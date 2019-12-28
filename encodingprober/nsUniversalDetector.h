/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*  -*- C++ -*-
*  Copyright (C) 1998 <developer@mozilla.org>
*  Copyright (C) 2008 <zealot.kai@gmail.com>
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

#ifndef nsUniversalDetector_h__
#define nsUniversalDetector_h__

#include "nsCharSetProber.h"

#define NUM_OF_CHARSET_PROBERS  3

namespace qencodingprober {
typedef enum {
  ePureAscii = 0,
  eEscAscii  = 1,
  eHighbyte  = 2
} nsInputState;

class  nsUniversalDetector: public nsCharSetProber {
public:
    nsUniversalDetector();
    virtual ~nsUniversalDetector();
    nsProbingState HandleData(const char* aBuf, unsigned int aLen);
    const char* GetCharSetName();
    void      Reset(void);
    float     GetConfidence(void);
    nsProbingState GetState();
    void      SetOpion() {};   

protected:
   nsInputState  mInputState;
   bool  mDone;
   bool  mInTag;
   bool  mStart;
   bool  mGotData;
   char    mLastChar;
   const char *  mDetectedCharset;
   int mBestGuess;

   nsCharSetProber  *mCharSetProbers[NUM_OF_CHARSET_PROBERS];
   nsCharSetProber  *mEscCharSetProber;
};
}

#endif

