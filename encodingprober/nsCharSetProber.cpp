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

#include "nsCharSetProber.h"

#include <stdlib.h>

namespace qencodingprober {
//This filter applies to all scripts which do not use English characters
bool nsCharSetProber::FilterWithoutEnglishLetters(const char* aBuf, unsigned int aLen, char** newBuf, unsigned int& newLen)
{
  char *newptr;
  char *prevPtr, *curPtr;
  
  bool meetMSB = false;   
  newptr = *newBuf = (char*)malloc(aLen);
  if (!newptr)
    return false;

  for (curPtr = prevPtr = (char*)aBuf; curPtr < aBuf+aLen; ++curPtr)
  {
    if (*curPtr & 0x80)
    {
      meetMSB = true;
    }
    else if (*curPtr < 'A' || (*curPtr > 'Z' && *curPtr < 'a') || *curPtr > 'z') 
    {
      //current char is a symbol, most likely a punctuation. we treat it as segment delimiter
      if (meetMSB && curPtr > prevPtr) 
      //this segment contains more than single symbol, and it has upper ASCII, we need to keep it
      {
        while (prevPtr < curPtr) *newptr++ = *prevPtr++;  
        prevPtr++;
        *newptr++ = ' ';
        meetMSB = false;
      }
      else //ignore current segment. (either because it is just a symbol or just an English word)
        prevPtr = curPtr+1;
    }
  }
  if (meetMSB && curPtr > prevPtr) 
    while (prevPtr < curPtr) *newptr++ = *prevPtr++;  

  newLen = newptr - *newBuf;

  return true;
}

//This filter applies to all scripts which contain both English characters and upper ASCII characters.
bool nsCharSetProber::FilterWithEnglishLetters(const char* aBuf, unsigned int aLen, char** newBuf, unsigned int& newLen)
{
  //do filtering to reduce load to probers
  char *newptr;
  char *prevPtr, *curPtr;
  bool isInTag = false;

  newptr = *newBuf = (char*)malloc(aLen);
  if (!newptr)
    return false;

  for (curPtr = prevPtr = (char*)aBuf; curPtr < aBuf+aLen; ++curPtr)
  {
    if (*curPtr == '>')
      isInTag = false;
    else if (*curPtr == '<')
      isInTag = true;

    if (!(*curPtr & 0x80) &&
        (*curPtr < 'A' || (*curPtr > 'Z' && *curPtr < 'a') || *curPtr > 'z') )
    {
      if (curPtr > prevPtr && !isInTag) // Current segment contains more than just a symbol 
                                        // and it is not inside a tag, keep it.
      {
        while (prevPtr < curPtr) *newptr++ = *prevPtr++;  
        prevPtr++;
        *newptr++ = ' ';
      }
      else
        prevPtr = curPtr+1;
    }
  }

  // If the current segment contains more than just a symbol 
  // and it is not inside a tag then keep it.
  if (!isInTag)
    while (prevPtr < curPtr)
      *newptr++ = *prevPtr++;  

  newLen = newptr - *newBuf;

  return true;
}
}


