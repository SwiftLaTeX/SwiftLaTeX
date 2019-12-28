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

#ifndef nsCharSetProber_h__
#define nsCharSetProber_h__

#include "qencodingprober.h"



namespace qencodingprober {
typedef enum {
  eDetecting = 0,   //We are still detecting, no sure answer yet, but caller can ask for confidence.
  eFoundIt = 1,     //That's a positive answer
  eNotMe = 2        //Negative answer
} nsProbingState;

#define SHORTCUT_THRESHOLD      (float)0.95

class  nsCharSetProber {
public:
  virtual ~nsCharSetProber() {};
  virtual const char* GetCharSetName() = 0;
  virtual nsProbingState HandleData(const char* aBuf, unsigned int aLen) = 0;
  virtual nsProbingState GetState(void) = 0;
  virtual void      Reset(void)  = 0;
  virtual float     GetConfidence(void) = 0;
  virtual void      SetOpion() = 0;

#ifdef DEBUG_PROBE
  virtual void  DumpStatus() {};
#endif

  // Helper functions used in the Latin1 and Group probers.
  // both functions Allocate a new buffer for newBuf. This buffer should be 
  // freed by the caller using PR_FREEIF.
  // Both functions return false in case of memory allocation failure.
  static bool FilterWithoutEnglishLetters(const char* aBuf, unsigned int aLen, char** newBuf, unsigned int& newLen);
  static bool FilterWithEnglishLetters(const char* aBuf, unsigned int aLen, char** newBuf, unsigned int& newLen);

};
}
#endif /* nsCharSetProber_h__ */
