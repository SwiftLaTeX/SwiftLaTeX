/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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

#ifndef CharDistribution_h__
#define CharDistribution_h__

#define ENOUGH_DATA_THRESHOLD 256
 
namespace qencodingprober {
class CharDistributionAnalysis
{
public:
  CharDistributionAnalysis() {Reset();};
  virtual ~CharDistributionAnalysis() {};

  //feed a block of data and do distribution analysis
  void HandleData(const char* /* aBuf */, unsigned int /* aLen */) {};
  
  //Feed a character with known length
  void HandleOneChar(const char* aStr, unsigned int aCharLen)
  {
    int order;

    //we only care about 2-bytes character in our distribution analysis
    order = (aCharLen == 2) ? GetOrder(aStr) : -1;

    if (order >= 0)
    {
      mTotalChars++;
      //order is valid
      if ((unsigned int)order < mTableSize)
      {
        if (512 > mCharToFreqOrder[order])
          mFreqChars++;
      }
    }
  };

  //return confidence base on existing data
  float GetConfidence();

  //Reset analyser, clear any state 
  void      Reset(void) 
  {
    mDone = false;
    mTotalChars = 0;
    mFreqChars = 0;
  };

  //This function is for future extension. Caller can use this function to control
  //analyser's behavior
  void      SetOpion(){};

  //It is not necessary to receive all data to draw conclusion. For charset detection,
  // certain amount of data is enough
  bool GotEnoughData() {return mTotalChars > ENOUGH_DATA_THRESHOLD;};

protected:
  //we do not handle character base on its original encoding string, but 
  //convert this encoding string to a number, here called order.
  //This allow multiple encoding of a language to share one frequency table 
  virtual int GetOrder(const char* /* str */) {return -1;};
  
  //If this flag is set to true, detection is done and conclusion has been made
  bool   mDone;

  //The number of characters whose frequency order is less than 512
  unsigned int mFreqChars;

  //Total character encounted.
  unsigned int mTotalChars;

  //Mapping table to get frequency order from char order (get from GetOrder())
  const short  *mCharToFreqOrder;

  //Size of above table
  unsigned int mTableSize;

  //This is a constant value varies from language to language, it is used in 
  //calculating confidence. See my paper for further detail.
  float    mTypicalDistributionRatio;
};


class  EUCTWDistributionAnalysis: public CharDistributionAnalysis
{
public:
  EUCTWDistributionAnalysis();
protected:

  //for euc-TW encoding, we are interested 
  //  first  byte range: 0xc4 -- 0xfe
  //  second byte range: 0xa1 -- 0xfe
  //no validation needed here. State machine has done that
  int GetOrder(const char* str) 
  { if ((unsigned char)*str >= (unsigned char)0xc4)  
      return 94*((unsigned char)str[0]-(unsigned char)0xc4) + (unsigned char)str[1] - (unsigned char)0xa1;
    else
      return -1;
  };
};


class  EUCKRDistributionAnalysis : public CharDistributionAnalysis
{
public:
  EUCKRDistributionAnalysis();
protected:
  //for euc-KR encoding, we are interested 
  //  first  byte range: 0xb0 -- 0xfe
  //  second byte range: 0xa1 -- 0xfe
  //no validation needed here. State machine has done that
  int GetOrder(const char* str) 
  { if ((unsigned char)*str >= (unsigned char)0xb0)  
      return 94*((unsigned char)str[0]-(unsigned char)0xb0) + (unsigned char)str[1] - (unsigned char)0xa1;
    else
      return -1;
  };
};

class  GB2312DistributionAnalysis : public CharDistributionAnalysis
{
public:
  GB2312DistributionAnalysis();
protected:
  //for GB2312 encoding, we are interested 
  //  first  byte range: 0xb0 -- 0xfe
  //  second byte range: 0xa1 -- 0xfe
  //no validation needed here. State machine has done that
  int GetOrder(const char* str) 
  { if ((unsigned char)*str >= (unsigned char)0xb0 && (unsigned char)str[1] >= (unsigned char)0xa1)  
      return 94*((unsigned char)str[0]-(unsigned char)0xb0) + (unsigned char)str[1] - (unsigned char)0xa1;
    else
      return -1;
  };
};


class  Big5DistributionAnalysis : public CharDistributionAnalysis
{
public:
  Big5DistributionAnalysis();
protected:
  //for big5 encoding, we are interested 
  //  first  byte range: 0xa4 -- 0xfe
  //  second byte range: 0x40 -- 0x7e , 0xa1 -- 0xfe
  //no validation needed here. State machine has done that
  int GetOrder(const char* str) 
  { if ((unsigned char)*str >= (unsigned char)0xa4)  
      if ((unsigned char)str[1] >= (unsigned char)0xa1)
        return 157*((unsigned char)str[0]-(unsigned char)0xa4) + (unsigned char)str[1] - (unsigned char)0xa1 +63;
      else
        return 157*((unsigned char)str[0]-(unsigned char)0xa4) + (unsigned char)str[1] - (unsigned char)0x40;
    else
      return -1;
  };
};

class SJISDistributionAnalysis : public CharDistributionAnalysis
{
public:
  SJISDistributionAnalysis();
protected:
  //for sjis encoding, we are interested 
  //  first  byte range: 0x81 -- 0x9f , 0xe0 -- 0xfe
  //  second byte range: 0x40 -- 0x7e,  0x81 -- oxfe
  //no validation needed here. State machine has done that
  int GetOrder(const char* str) 
  { 
    int order;
    if ((unsigned char)*str >= (unsigned char)0x81 && (unsigned char)*str <= (unsigned char)0x9f)  
      order = 188 * ((unsigned char)str[0]-(unsigned char)0x81);
    else if ((unsigned char)*str >= (unsigned char)0xe0 && (unsigned char)*str <= (unsigned char)0xef)  
      order = 188 * ((unsigned char)str[0]-(unsigned char)0xe0 + 31);
    else
      return -1;
    order += (unsigned char)*(str+1) - 0x40;
    if ((unsigned char)str[1] > (unsigned char)0x7f)
      order--;
    return order;
  };
};

class  EUCJPDistributionAnalysis : public CharDistributionAnalysis
{
public:
  EUCJPDistributionAnalysis();
protected:
  //for euc-JP encoding, we are interested 
  //  first  byte range: 0xa0 -- 0xfe
  //  second byte range: 0xa1 -- 0xfe
  //no validation needed here. State machine has done that
  int GetOrder(const char* str) 
  { if ((unsigned char)*str >= (unsigned char)0xa0)  
      return 94*((unsigned char)str[0]-(unsigned char)0xa1) + (unsigned char)str[1] - (unsigned char)0xa1;
    else
      return -1;
  };
};
}
#endif //CharDistribution_h__

