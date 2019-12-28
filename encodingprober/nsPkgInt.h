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

#ifndef nsPkgInt_h__
#define nsPkgInt_h__

namespace qencodingprober {
typedef enum {
  eIdxSft4bits  = 3,
  eIdxSft8bits  = 2,
  eIdxSft16bits = 1
} nsIdxSft; 

typedef enum {
  eSftMsk4bits  = 7,
  eSftMsk8bits  = 3,
  eSftMsk16bits = 1
} nsSftMsk; 

typedef enum {
  eBitSft4bits  = 2,
  eBitSft8bits  = 3,
  eBitSft16bits = 4
} nsBitSft; 

typedef enum {
  eUnitMsk4bits  = 0x0000000FL,
  eUnitMsk8bits  = 0x000000FFL,
  eUnitMsk16bits = 0x0000FFFFL
} nsUnitMsk; 

typedef struct nsPkgInt {
  nsIdxSft  idxsft;
  nsSftMsk  sftmsk;
  nsBitSft  bitsft;
  nsUnitMsk unitmsk;
  unsigned int  *data;
} nsPkgInt;
}

#define PCK16BITS(a,b)            ((unsigned int)(((b) << 16) | (a)))

#define PCK8BITS(a,b,c,d)         PCK16BITS( ((unsigned int)(((b) << 8) | (a))),  \
                                             ((unsigned int)(((d) << 8) | (c))))

#define PCK4BITS(a,b,c,d,e,f,g,h) PCK8BITS(  ((unsigned int)(((b) << 4) | (a))), \
                                             ((unsigned int)(((d) << 4) | (c))), \
                                             ((unsigned int)(((f) << 4) | (e))), \
                                             ((unsigned int)(((h) << 4) | (g))) )

#define GETFROMPCK(i, c) \
 (((((c).data)[(i)>>(c).idxsft])>>(((i)&(c).sftmsk)<<(c).bitsft))&(c).unitmsk)

#endif /* nsPkgInt_h__ */

