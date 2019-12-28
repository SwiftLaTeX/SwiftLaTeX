// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2008 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "xfa/fxbarcode/qrcode/BC_QRCoderEncoder.h"

#include <algorithm>
#include <memory>

#include "xfa/fxbarcode/BC_UtilCodingConvert.h"
#include "xfa/fxbarcode/common/BC_CommonByteArray.h"
#include "xfa/fxbarcode/common/BC_CommonByteMatrix.h"
#include "xfa/fxbarcode/common/reedsolomon/BC_ReedSolomon.h"
#include "xfa/fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoder.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderBitVector.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderBlockPair.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderECBlocks.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMaskUtil.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMatrixUtil.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMode.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderVersion.h"

const int32_t CBC_QRCoderEncoder::m_alphaNumbericTable[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  44, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1};

CBC_QRCoderEncoder::CBC_QRCoderEncoder() {}
CBC_QRCoderEncoder::~CBC_QRCoderEncoder() {}
class Make_Pair {
 public:
  CBC_QRCoderMode* m_mode;
  CFX_ByteString m_string;

 private:
  Make_Pair(const Make_Pair& mode_string) {}
  Make_Pair& operator=(Make_Pair& mode_string) {
    if (this == &mode_string) {
      return *this;
    }
    m_mode = mode_string.m_mode;
    m_string = mode_string.m_string;
    return *this;
  }

 public:
  Make_Pair(CBC_QRCoderMode* mode, const CFX_ByteString& str)
      : m_mode(mode), m_string(str) {}
  ~Make_Pair() {}
};
void CBC_QRCoderEncoder::Encode(const CFX_ByteString& content,
                                CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                CBC_QRCoder* qrCode,
                                int32_t& e,
                                int32_t versionSpecify) {
  if (versionSpecify == 0) {
    EncodeWithAutoVersion(content, ecLevel, qrCode, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e)
  } else if (versionSpecify > 0 && versionSpecify <= 40) {
    EncodeWithSpecifyVersion(content, ecLevel, qrCode, versionSpecify, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  } else {
    e = BCExceptionVersionMust1_40;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::AppendECI(CBC_QRCoderBitVector* bits) {}
void CBC_QRCoderEncoder::AppendDataModeLenghInfo(
    const CFX_ArrayTemplate<Make_Pair*>& splitResult,
    CBC_QRCoderBitVector& headerAndDataBits,
    CBC_QRCoderMode* tempMode,
    CBC_QRCoder* qrCode,
    CFX_ByteString& encoding,
    int32_t& e) {
  for (int32_t i = 0; i < splitResult.GetSize(); i++) {
    tempMode = splitResult[i]->m_mode;
    if (tempMode == CBC_QRCoderMode::sGBK) {
      AppendModeInfo(tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendLengthInfo(splitResult[i]->m_string.GetLength(),
                       qrCode->GetVersion(), tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendBytes(splitResult[i]->m_string, tempMode, &headerAndDataBits,
                  encoding, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    } else if (tempMode == CBC_QRCoderMode::sBYTE) {
      CFX_ByteArray bytes;
      CBC_UtilCodingConvert::LocaleToUtf8(splitResult[i]->m_string, bytes);
      AppendModeInfo(tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendLengthInfo(bytes.GetSize(), qrCode->GetVersion(), tempMode,
                       &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      Append8BitBytes(bytes, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    } else if (tempMode == CBC_QRCoderMode::sALPHANUMERIC) {
      AppendModeInfo(tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendLengthInfo(splitResult[i]->m_string.GetLength(),
                       qrCode->GetVersion(), tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendBytes(splitResult[i]->m_string, tempMode, &headerAndDataBits,
                  encoding, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    } else if (tempMode == CBC_QRCoderMode::sNUMERIC) {
      AppendModeInfo(tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendLengthInfo(splitResult[i]->m_string.GetLength(),
                       qrCode->GetVersion(), tempMode, &headerAndDataBits, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      AppendBytes(splitResult[i]->m_string, tempMode, &headerAndDataBits,
                  encoding, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    } else {
      e = BCExceptionUnknown;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
  }
}
void CBC_QRCoderEncoder::SplitString(const CFX_ByteString& content,
                                     CFX_ArrayTemplate<Make_Pair*>* result) {
  int32_t index = 0, flag = 0;
  while (
      (((uint8_t)content[index] >= 0xA1 && (uint8_t)content[index] <= 0xAA) ||
       ((uint8_t)content[index] >= 0xB0 && (uint8_t)content[index] <= 0xFA)) &&
      (index < content.GetLength())) {
    index += 2;
  }
  if (index != flag) {
    result->Add(
        new Make_Pair(CBC_QRCoderMode::sGBK, content.Mid(flag, index - flag)));
  }
  flag = index;
  if (index >= content.GetLength()) {
    return;
  }
  while (
      GetAlphaNumericCode((uint8_t)content[index]) == -1 &&
      !(((uint8_t)content[index] >= 0xA1 && (uint8_t)content[index] <= 0xAA) ||
        ((uint8_t)content[index] >= 0xB0 && (uint8_t)content[index] <= 0xFA)) &&
      (index < content.GetLength())) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
    if (IsDBCSLeadByte((uint8_t)content[index]))
#else
    if ((uint8_t)content[index] > 127)
#endif
    {
      index += 2;
    } else {
      index++;
    }
  }
  if (index != flag) {
    result->Add(
        new Make_Pair(CBC_QRCoderMode::sBYTE, content.Mid(flag, index - flag)));
  }
  flag = index;
  if (index >= content.GetLength()) {
    return;
  }
  while (FXSYS_Isdigit((uint8_t)content[index]) &&
         (index < content.GetLength())) {
    index++;
  }
  if (index != flag) {
    result->Add(new Make_Pair(CBC_QRCoderMode::sNUMERIC,
                              content.Mid(flag, index - flag)));
  }
  flag = index;
  if (index >= content.GetLength()) {
    return;
  }
  while (GetAlphaNumericCode((uint8_t)content[index]) != -1 &&
         (index < content.GetLength())) {
    index++;
  }
  if (index != flag) {
    result->Add(new Make_Pair(CBC_QRCoderMode::sALPHANUMERIC,
                              content.Mid(flag, index - flag)));
  }
  flag = index;
  if (index < content.GetLength())
    SplitString(content.Mid(index, content.GetLength() - index), result);
}

int32_t CBC_QRCoderEncoder::GetSpanByVersion(CBC_QRCoderMode* modeFirst,
                                             CBC_QRCoderMode* modeSecond,
                                             int32_t versionNum,
                                             int32_t& e) {
  if (versionNum == 0) {
    return 0;
  }
  if ((modeFirst == CBC_QRCoderMode::sALPHANUMERIC) &&
      (modeSecond == CBC_QRCoderMode::sBYTE)) {
    if (versionNum >= 1 && versionNum <= 9) {
      return 11;
    } else if (versionNum >= 10 && versionNum <= 26) {
      return 15;
    } else if (versionNum >= 27 && versionNum <= 40) {
      return 16;
    } else {
      e = BCExceptionNoSuchVersion;
      BC_EXCEPTION_CHECK_ReturnValue(e, 0);
    }
  } else if ((modeSecond == CBC_QRCoderMode::sALPHANUMERIC) &&
             (modeFirst == CBC_QRCoderMode::sNUMERIC)) {
    if (versionNum >= 1 && versionNum <= 9) {
      return 13;
    } else if (versionNum >= 10 && versionNum <= 26) {
      return 15;
    } else if (versionNum >= 27 && versionNum <= 40) {
      return 17;
    } else {
      e = BCExceptionNoSuchVersion;
      BC_EXCEPTION_CHECK_ReturnValue(e, 0);
    }
  } else if ((modeSecond == CBC_QRCoderMode::sBYTE) &&
             (modeFirst == CBC_QRCoderMode::sNUMERIC)) {
    if (versionNum >= 1 && versionNum <= 9) {
      return 6;
    } else if (versionNum >= 10 && versionNum <= 26) {
      return 8;
    } else if (versionNum >= 27 && versionNum <= 40) {
      return 9;
    } else {
      e = BCExceptionNoSuchVersion;
      BC_EXCEPTION_CHECK_ReturnValue(e, 0);
    }
  }
  return -1;
}

void CBC_QRCoderEncoder::MergeString(CFX_ArrayTemplate<Make_Pair*>* result,
                                     int32_t versionNum,
                                     int32_t& e) {
  Make_Pair* first = nullptr;
  Make_Pair* second = nullptr;
  size_t mergeNum = 0;
  int32_t i;
  for (i = 0; ((i < result->GetSize()) && (i + 1 < result->GetSize())); i++) {
    first = (*result)[i];
    second = (*result)[i + 1];
    if (first->m_mode == CBC_QRCoderMode::sALPHANUMERIC) {
      int32_t tmp = GetSpanByVersion(CBC_QRCoderMode::sALPHANUMERIC,
                                     CBC_QRCoderMode::sBYTE, versionNum, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      if ((second->m_mode == CBC_QRCoderMode::sBYTE) &&
          (first->m_string.GetLength() < tmp)) {
        CFX_ByteString str = first->m_string + second->m_string;
        second->m_string = str;
        delete first;
        result->RemoveAt(i);
        i--;
        mergeNum++;
      }
    } else if (first->m_mode == CBC_QRCoderMode::sBYTE) {
      if (second->m_mode == CBC_QRCoderMode::sBYTE) {
        first->m_string += second->m_string;
        delete second;
        result->RemoveAt(i + 1);
        i--;
        mergeNum++;
      }
    } else if (first->m_mode == CBC_QRCoderMode::sNUMERIC) {
      int32_t tmp = GetSpanByVersion(CBC_QRCoderMode::sNUMERIC,
                                     CBC_QRCoderMode::sBYTE, versionNum, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      if ((second->m_mode == CBC_QRCoderMode::sBYTE) &&
          (first->m_string.GetLength() < tmp)) {
        CFX_ByteString str = first->m_string + second->m_string;
        second->m_string = str;
        delete first;
        result->RemoveAt(i);
        i--;
        mergeNum++;
      }
      tmp = GetSpanByVersion(CBC_QRCoderMode::sNUMERIC,
                             CBC_QRCoderMode::sALPHANUMERIC, versionNum, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      if ((second->m_mode == CBC_QRCoderMode::sALPHANUMERIC) &&
          (first->m_string.GetLength() < tmp)) {
        CFX_ByteString str = first->m_string + second->m_string;
        second->m_string = str;
        delete first;
        result->RemoveAt(i);
        i--;
        mergeNum++;
      }
    }
  }
  if (mergeNum == 0) {
    return;
  }
  MergeString(result, versionNum, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderEncoder::InitQRCode(int32_t numInputBytes,
                                    int32_t versionNumber,
                                    CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                    CBC_QRCoderMode* mode,
                                    CBC_QRCoder* qrCode,
                                    int32_t& e) {
  qrCode->SetECLevel(ecLevel);
  qrCode->SetMode(mode);
  CBC_QRCoderVersion* version =
      CBC_QRCoderVersion::GetVersionForNumber(versionNumber, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t numBytes = version->GetTotalCodeWords();
  CBC_QRCoderECBlocks* ecBlocks = version->GetECBlocksForLevel(ecLevel);
  int32_t numEcBytes = ecBlocks->GetTotalECCodeWords();
  int32_t numRSBlocks = ecBlocks->GetNumBlocks();
  int32_t numDataBytes = numBytes - numEcBytes;
  if (numDataBytes >= numInputBytes + 3) {
    qrCode->SetVersion(versionNumber);
    qrCode->SetNumTotalBytes(numBytes);
    qrCode->SetNumDataBytes(numDataBytes);
    qrCode->SetNumRSBlocks(numRSBlocks);
    qrCode->SetNumECBytes(numEcBytes);
    qrCode->SetMatrixWidth(version->GetDimensionForVersion());
    return;
  }
  e = BCExceptionCannotFindBlockInfo;
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderEncoder::EncodeWithSpecifyVersion(
    const CFX_ByteString& content,
    CBC_QRCoderErrorCorrectionLevel* ecLevel,
    CBC_QRCoder* qrCode,
    int32_t versionSpecify,
    int32_t& e) {
  CFX_ByteString encoding = "utf8";
  CBC_QRCoderMode* mode = CBC_QRCoderMode::sBYTE;
  CFX_ArrayTemplate<Make_Pair*> splitResult;
  CBC_QRCoderBitVector dataBits;
  dataBits.Init();
  SplitString(content, &splitResult);
  MergeString(&splitResult, versionSpecify, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e) CBC_QRCoderMode* tempMode = nullptr;
  for (int32_t i = 0; i < splitResult.GetSize(); i++) {
    AppendBytes(splitResult[i]->m_string, splitResult[i]->m_mode, &dataBits,
                encoding, e);
    if (e != BCExceptionNO) {
      for (int32_t y = 0; y < splitResult.GetSize(); y++)
        delete splitResult[y];
      return;
    }
  }
  int32_t numInputBytes = dataBits.sizeInBytes();
  CBC_QRCoderBitVector headerAndDataBits;
  headerAndDataBits.Init();
  InitQRCode(numInputBytes, versionSpecify, ecLevel, mode, qrCode, e);
  if (e != BCExceptionNO) {
    for (int32_t k = 0; k < splitResult.GetSize(); k++)
      delete splitResult[k];
    return;
  }
  AppendDataModeLenghInfo(splitResult, headerAndDataBits, tempMode, qrCode,
                          encoding, e);
  if (e != BCExceptionNO) {
    for (int32_t k = 0; k < splitResult.GetSize(); k++)
      delete splitResult[k];
    return;
  }
  numInputBytes = headerAndDataBits.sizeInBytes();
  TerminateBits(qrCode->GetNumDataBytes(), &headerAndDataBits, e);
  for (int32_t j = 0; j < splitResult.GetSize(); j++) {
    delete splitResult[j];
  }
  if (e != BCExceptionNO)
    return;

  CBC_QRCoderBitVector finalBits;
  finalBits.Init();
  InterleaveWithECBytes(&headerAndDataBits, qrCode->GetNumTotalBytes(),
                        qrCode->GetNumDataBytes(), qrCode->GetNumRSBlocks(),
                        &finalBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  std::unique_ptr<CBC_CommonByteMatrix> matrix(new CBC_CommonByteMatrix(
      qrCode->GetMatrixWidth(), qrCode->GetMatrixWidth()));
  matrix->Init();
  int32_t maskPattern = ChooseMaskPattern(
      &finalBits, qrCode->GetECLevel(), qrCode->GetVersion(), matrix.get(), e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  qrCode->SetMaskPattern(maskPattern);
  CBC_QRCoderMatrixUtil::BuildMatrix(&finalBits, qrCode->GetECLevel(),
                                     qrCode->GetVersion(),
                                     qrCode->GetMaskPattern(), matrix.get(), e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  qrCode->SetMatrix(matrix.release());
  if (!qrCode->IsValid()) {
    e = BCExceptionInvalidQRCode;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::EncodeWithAutoVersion(
    const CFX_ByteString& content,
    CBC_QRCoderErrorCorrectionLevel* ecLevel,
    CBC_QRCoder* qrCode,
    int32_t& e) {
  CFX_ByteString encoding = "utf8";
  CBC_QRCoderMode* mode = CBC_QRCoderMode::sBYTE;
  CFX_ArrayTemplate<Make_Pair*> splitResult;
  CBC_QRCoderBitVector dataBits;
  dataBits.Init();
  SplitString(content, &splitResult);
  MergeString(&splitResult, 8, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  CBC_QRCoderMode* tempMode = nullptr;
  for (int32_t i = 0; i < splitResult.GetSize(); i++) {
    AppendBytes(splitResult[i]->m_string, splitResult[i]->m_mode, &dataBits,
                encoding, e);
    if (e != BCExceptionNO) {
      for (int32_t l = 0; l < splitResult.GetSize(); l++)
        delete splitResult[l];
      return;
    }
  }
  int32_t numInputBytes = dataBits.sizeInBytes();
  InitQRCode(numInputBytes, ecLevel, mode, qrCode, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e) CBC_QRCoderBitVector headerAndDataBits;
  headerAndDataBits.Init();
  tempMode = nullptr;
  int32_t versionNum = qrCode->GetVersion();
sign:
  AppendDataModeLenghInfo(splitResult, headerAndDataBits, tempMode, qrCode,
                          encoding, e);
  if (e != BCExceptionNO) {
    goto catchException;
  }
  numInputBytes = headerAndDataBits.sizeInBytes();
  TerminateBits(qrCode->GetNumDataBytes(), &headerAndDataBits, e);
  if (e != BCExceptionNO) {
    goto catchException;
  }
catchException:
  if (e != BCExceptionNO) {
    int32_t e1 = BCExceptionNO;
    InitQRCode(numInputBytes, ecLevel, mode, qrCode, e1);
    if (e1 != BCExceptionNO) {
      e = e1;
      return;
    }
    versionNum++;
    if (versionNum <= 40) {
      headerAndDataBits.Clear();
      e = BCExceptionNO;
      goto sign;
    } else {
      for (int32_t j = 0; j < splitResult.GetSize(); j++)
        delete splitResult[j];
      return;
    }
  }
  for (int32_t k = 0; k < splitResult.GetSize(); k++)
    delete splitResult[k];

  CBC_QRCoderBitVector finalBits;
  finalBits.Init();
  InterleaveWithECBytes(&headerAndDataBits, qrCode->GetNumTotalBytes(),
                        qrCode->GetNumDataBytes(), qrCode->GetNumRSBlocks(),
                        &finalBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  std::unique_ptr<CBC_CommonByteMatrix> matrix(new CBC_CommonByteMatrix(
      qrCode->GetMatrixWidth(), qrCode->GetMatrixWidth()));
  matrix->Init();
  int32_t maskPattern = ChooseMaskPattern(
      &finalBits, qrCode->GetECLevel(), qrCode->GetVersion(), matrix.get(), e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  qrCode->SetMaskPattern(maskPattern);
  CBC_QRCoderMatrixUtil::BuildMatrix(&finalBits, qrCode->GetECLevel(),
                                     qrCode->GetVersion(),
                                     qrCode->GetMaskPattern(), matrix.get(), e);
  BC_EXCEPTION_CHECK_ReturnVoid(e) qrCode->SetMatrix(matrix.release());
  if (!qrCode->IsValid()) {
    e = BCExceptionInvalidQRCode;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::Encode(const CFX_WideString& content,
                                CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                CBC_QRCoder* qrCode,
                                int32_t& e) {
  CFX_ByteString encoding = "utf8";
  CFX_ByteString utf8Data;
  CBC_UtilCodingConvert::UnicodeToUTF8(content, utf8Data);
  CBC_QRCoderMode* mode = ChooseMode(utf8Data, encoding);
  CBC_QRCoderBitVector dataBits;
  dataBits.Init();
  AppendBytes(utf8Data, mode, &dataBits, encoding, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t numInputBytes = dataBits.sizeInBytes();
  InitQRCode(numInputBytes, ecLevel, mode, qrCode, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  CBC_QRCoderBitVector headerAndDataBits;
  headerAndDataBits.Init();
  AppendModeInfo(mode, &headerAndDataBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t numLetters = mode == CBC_QRCoderMode::sBYTE ? dataBits.sizeInBytes()
                                                      : content.GetLength();
  AppendLengthInfo(numLetters, qrCode->GetVersion(), mode, &headerAndDataBits,
                   e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  headerAndDataBits.AppendBitVector(&dataBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e)
      TerminateBits(qrCode->GetNumDataBytes(), &headerAndDataBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  CBC_QRCoderBitVector finalBits;
  finalBits.Init();
  InterleaveWithECBytes(&headerAndDataBits, qrCode->GetNumTotalBytes(),
                        qrCode->GetNumDataBytes(), qrCode->GetNumRSBlocks(),
                        &finalBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  std::unique_ptr<CBC_CommonByteMatrix> matrix(new CBC_CommonByteMatrix(
      qrCode->GetMatrixWidth(), qrCode->GetMatrixWidth()));
  matrix->Init();
  int32_t maskPattern = ChooseMaskPattern(
      &finalBits, qrCode->GetECLevel(), qrCode->GetVersion(), matrix.get(), e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  qrCode->SetMaskPattern(maskPattern);
  CBC_QRCoderMatrixUtil::BuildMatrix(&finalBits, qrCode->GetECLevel(),
                                     qrCode->GetVersion(),
                                     qrCode->GetMaskPattern(), matrix.get(), e);
  BC_EXCEPTION_CHECK_ReturnVoid(e) qrCode->SetMatrix(matrix.release());
  if (!qrCode->IsValid()) {
    e = BCExceptionInvalidQRCode;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::TerminateBits(int32_t numDataBytes,
                                       CBC_QRCoderBitVector* bits,
                                       int32_t& e) {
  int32_t capacity = numDataBytes << 3;
  if (bits->Size() > capacity) {
    e = BCExceptionDataTooMany;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t i = 0; i < 4 && bits->Size() < capacity; ++i) {
    bits->AppendBit(0, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t numBitsInLastByte = bits->Size() % 8;
  if (numBitsInLastByte > 0) {
    int32_t numPaddingBits = 8 - numBitsInLastByte;
    for (int32_t j = 0; j < numPaddingBits; ++j) {
      bits->AppendBit(0, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e)
    }
  }
  if (bits->Size() % 8 != 0) {
    e = BCExceptionDigitLengthMustBe8;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t numPaddingBytes = numDataBytes - bits->sizeInBytes();
  for (int32_t k = 0; k < numPaddingBytes; ++k) {
    if (k % 2 == 0) {
      bits->AppendBits(0xec, 8, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    } else {
      bits->AppendBits(0x11, 8, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
  }
  if (bits->Size() != capacity) {
    e = BCExceptionBitsNotEqualCacity;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
int32_t CBC_QRCoderEncoder::ChooseMaskPattern(
    CBC_QRCoderBitVector* bits,
    CBC_QRCoderErrorCorrectionLevel* ecLevel,
    int32_t version,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  int32_t minPenalty = 65535;
  int32_t bestMaskPattern = -1;
  for (int32_t maskPattern = 0; maskPattern < CBC_QRCoder::NUM_MASK_PATTERNS;
       maskPattern++) {
    CBC_QRCoderMatrixUtil::BuildMatrix(bits, ecLevel, version, maskPattern,
                                       matrix, e);
    BC_EXCEPTION_CHECK_ReturnValue(e, 0);
    int32_t penalty = CalculateMaskPenalty(matrix);
    if (penalty < minPenalty) {
      minPenalty = penalty;
      bestMaskPattern = maskPattern;
    }
  }
  return bestMaskPattern;
}
int32_t CBC_QRCoderEncoder::CalculateMaskPenalty(CBC_CommonByteMatrix* matrix) {
  int32_t penalty = 0;
  penalty += CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule1(matrix);
  penalty += CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule2(matrix);
  penalty += CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule3(matrix);
  penalty += CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule4(matrix);
  return penalty;
}
CBC_QRCoderMode* CBC_QRCoderEncoder::ChooseMode(const CFX_ByteString& content,
                                                CFX_ByteString encoding) {
  if (encoding.Compare("SHIFT_JIS") == 0) {
    return CBC_QRCoderMode::sKANJI;
  }
  FX_BOOL hasNumeric = FALSE;
  FX_BOOL hasAlphaNumeric = FALSE;
  for (int32_t i = 0; i < content.GetLength(); i++) {
    if (isdigit((uint8_t)content[i])) {
      hasNumeric = TRUE;
    } else if (GetAlphaNumericCode((uint8_t)content[i]) != -1) {
      hasAlphaNumeric = TRUE;
    } else {
      return CBC_QRCoderMode::sBYTE;
    }
  }
  if (hasAlphaNumeric) {
    return CBC_QRCoderMode::sALPHANUMERIC;
  } else if (hasNumeric) {
    return CBC_QRCoderMode::sNUMERIC;
  }
  return CBC_QRCoderMode::sBYTE;
}
int32_t CBC_QRCoderEncoder::GetAlphaNumericCode(int32_t code) {
  if (code < 96 && code >= 0) {
    return m_alphaNumbericTable[code];
  }
  return -1;
}
void CBC_QRCoderEncoder::AppendBytes(const CFX_ByteString& content,
                                     CBC_QRCoderMode* mode,
                                     CBC_QRCoderBitVector* bits,
                                     CFX_ByteString encoding,
                                     int32_t& e) {
  if (mode == CBC_QRCoderMode::sNUMERIC) {
    AppendNumericBytes(content, bits, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  } else if (mode == CBC_QRCoderMode::sALPHANUMERIC) {
    AppendAlphaNumericBytes(content, bits, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  } else if (mode == CBC_QRCoderMode::sBYTE) {
    Append8BitBytes(content, bits, encoding, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  } else if (mode == CBC_QRCoderMode::sKANJI) {
    AppendKanjiBytes(content, bits, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  } else if (mode == CBC_QRCoderMode::sGBK) {
    AppendGBKBytes(content, bits, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  } else {
    e = BCExceptionUnsupportedMode;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::AppendNumericBytes(const CFX_ByteString& content,
                                            CBC_QRCoderBitVector* bits,
                                            int32_t& e) {
  int32_t length = content.GetLength();
  int32_t i = 0;
  while (i < length) {
    int32_t num1 = content[i] - '0';
    if (i + 2 < length) {
      int32_t num2 = content[i + 1] - '0';
      int32_t num3 = content[i + 2] - '0';
      bits->AppendBits(num1 * 100 + num2 * 10 + num3, 10, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e) i += 3;
    } else if (i + 1 < length) {
      int32_t num2 = content[i + 1] - '0';
      bits->AppendBits(num1 * 10 + num2, 7, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e) i += 2;
    } else {
      bits->AppendBits(num1, 4, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      i++;
    }
  }
}
void CBC_QRCoderEncoder::AppendAlphaNumericBytes(const CFX_ByteString& content,
                                                 CBC_QRCoderBitVector* bits,
                                                 int32_t& e) {
  int32_t length = content.GetLength();
  int32_t i = 0;
  while (i < length) {
    int32_t code1 = GetAlphaNumericCode(content[i]);
    if (code1 == -1) {
      e = BCExceptionInvalidateCharacter;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
    if (i + 1 < length) {
      int32_t code2 = GetAlphaNumericCode(content[i + 1]);
      if (code2 == -1) {
        e = BCExceptionInvalidateCharacter;
        BC_EXCEPTION_CHECK_ReturnVoid(e);
      }
      bits->AppendBits(code1 * 45 + code2, 11, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      i += 2;
    } else {
      bits->AppendBits(code1, 6, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e) i++;
    }
  }
}
void CBC_QRCoderEncoder::AppendGBKBytes(const CFX_ByteString& content,
                                        CBC_QRCoderBitVector* bits,
                                        int32_t& e) {
  int32_t length = content.GetLength();
  uint32_t value = 0;
  for (int32_t i = 0; i < length; i += 2) {
    value = (uint32_t)((uint8_t)content[i] << 8 | (uint8_t)content[i + 1]);
    if (value <= 0xAAFE && value >= 0xA1A1) {
      value -= 0xA1A1;
    } else if (value <= 0xFAFE && value >= 0xB0A1) {
      value -= 0xA6A1;
    } else {
      e = BCExceptionInvalidateCharacter;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
    value = (uint32_t)((value >> 8) * 0x60) + (uint32_t)(value & 0xff);
    bits->AppendBits(value, 13, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::Append8BitBytes(const CFX_ByteString& content,
                                         CBC_QRCoderBitVector* bits,
                                         CFX_ByteString encoding,
                                         int32_t& e) {
  for (int32_t i = 0; i < content.GetLength(); i++) {
    bits->AppendBits(content[i], 8, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::Append8BitBytes(CFX_ByteArray& bytes,
                                         CBC_QRCoderBitVector* bits,
                                         int32_t& e) {
  for (int32_t i = 0; i < bytes.GetSize(); i++) {
    bits->AppendBits(bytes[i], 8, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::AppendKanjiBytes(const CFX_ByteString& content,
                                          CBC_QRCoderBitVector* bits,
                                          int32_t& e) {
  CFX_ByteArray bytes;
  uint32_t value = 0;
  for (int32_t i = 0; i < bytes.GetSize(); i += 2) {
    value = (uint32_t)((uint8_t)(content[i] << 8) | (uint8_t)content[i + 1]);
    if (value <= 0x9ffc && value >= 0x8140) {
      value -= 0x8140;
    } else if (value <= 0xebbf && value >= 0xe040) {
      value -= 0xc140;
    } else {
      e = BCExceptionInvalidateCharacter;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
    value = (uint32_t)((value >> 8) * 0xc0) + (uint32_t)(value & 0xff);
    bits->AppendBits(value, 13, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::InitQRCode(int32_t numInputBytes,
                                    CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                    CBC_QRCoderMode* mode,
                                    CBC_QRCoder* qrCode,
                                    int32_t& e) {
  qrCode->SetECLevel(ecLevel);
  qrCode->SetMode(mode);
  for (int32_t versionNum = 1; versionNum <= 40; versionNum++) {
    CBC_QRCoderVersion* version =
        CBC_QRCoderVersion::GetVersionForNumber(versionNum, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
    int32_t numBytes = version->GetTotalCodeWords();
    CBC_QRCoderECBlocks* ecBlocks = version->GetECBlocksForLevel(ecLevel);
    int32_t numEcBytes = ecBlocks->GetTotalECCodeWords();
    int32_t numRSBlocks = ecBlocks->GetNumBlocks();
    int32_t numDataBytes = numBytes - numEcBytes;
    if (numDataBytes >= numInputBytes + 3) {
      qrCode->SetVersion(versionNum);
      qrCode->SetNumTotalBytes(numBytes);
      qrCode->SetNumDataBytes(numDataBytes);
      qrCode->SetNumRSBlocks(numRSBlocks);
      qrCode->SetNumECBytes(numEcBytes);
      qrCode->SetMatrixWidth(version->GetDimensionForVersion());
      return;
    }
  }
  e = BCExceptionCannotFindBlockInfo;
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderEncoder::AppendModeInfo(CBC_QRCoderMode* mode,
                                        CBC_QRCoderBitVector* bits,
                                        int32_t& e) {
  bits->AppendBits(mode->GetBits(), 4, e);
  if (mode == CBC_QRCoderMode::sGBK) {
    bits->AppendBits(1, 4, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::AppendLengthInfo(int32_t numLetters,
                                          int32_t version,
                                          CBC_QRCoderMode* mode,
                                          CBC_QRCoderBitVector* bits,
                                          int32_t& e) {
  CBC_QRCoderVersion* qcv = CBC_QRCoderVersion::GetVersionForNumber(version, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t numBits = mode->GetCharacterCountBits(qcv, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  if (numBits > ((1 << numBits) - 1)) {
    return;
  }
  if (mode == CBC_QRCoderMode::sGBK) {
    bits->AppendBits(numLetters / 2, numBits, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  bits->AppendBits(numLetters, numBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderEncoder::InterleaveWithECBytes(CBC_QRCoderBitVector* bits,
                                               int32_t numTotalBytes,
                                               int32_t numDataBytes,
                                               int32_t numRSBlocks,
                                               CBC_QRCoderBitVector* result,
                                               int32_t& e) {
  if (bits->sizeInBytes() != numDataBytes) {
    e = BCExceptionBitsBytesNotMatch;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t dataBytesOffset = 0;
  int32_t maxNumDataBytes = 0;
  int32_t maxNumEcBytes = 0;
  CFX_ArrayTemplate<CBC_QRCoderBlockPair*> blocks;
  int32_t i;
  for (i = 0; i < numRSBlocks; i++) {
    int32_t numDataBytesInBlock;
    int32_t numEcBytesInBlosk;
    GetNumDataBytesAndNumECBytesForBlockID(numTotalBytes, numDataBytes,
                                           numRSBlocks, i, numDataBytesInBlock,
                                           numEcBytesInBlosk);
    CBC_CommonByteArray* dataBytes = new CBC_CommonByteArray;
    dataBytes->Set(bits->GetArray(), dataBytesOffset, numDataBytesInBlock);
    CBC_CommonByteArray* ecBytes =
        GenerateECBytes(dataBytes, numEcBytesInBlosk, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
    blocks.Add(new CBC_QRCoderBlockPair(dataBytes, ecBytes));
    maxNumDataBytes = std::max(maxNumDataBytes, dataBytes->Size());
    maxNumEcBytes = std::max(maxNumEcBytes, ecBytes->Size());
    dataBytesOffset += numDataBytesInBlock;
  }
  if (numDataBytes != dataBytesOffset) {
    e = BCExceptionBytesNotMatchOffset;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t x = 0; x < maxNumDataBytes; x++) {
    for (int32_t j = 0; j < blocks.GetSize(); j++) {
      CBC_CommonByteArray* dataBytes = blocks[j]->GetDataBytes();
      if (x < dataBytes->Size()) {
        result->AppendBits(dataBytes->At(x), 8, e);
        BC_EXCEPTION_CHECK_ReturnVoid(e);
      }
    }
  }
  for (int32_t y = 0; y < maxNumEcBytes; y++) {
    for (int32_t l = 0; l < blocks.GetSize(); l++) {
      CBC_CommonByteArray* ecBytes = blocks[l]->GetErrorCorrectionBytes();
      if (y < ecBytes->Size()) {
        result->AppendBits(ecBytes->At(y), 8, e);
        BC_EXCEPTION_CHECK_ReturnVoid(e);
      }
    }
  }
  for (int32_t k = 0; k < blocks.GetSize(); k++) {
    delete blocks[k];
  }
  if (numTotalBytes != result->sizeInBytes()) {
    e = BCExceptionSizeInBytesDiffer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderEncoder::GetNumDataBytesAndNumECBytesForBlockID(
    int32_t numTotalBytes,
    int32_t numDataBytes,
    int32_t numRSBlocks,
    int32_t blockID,
    int32_t& numDataBytesInBlock,
    int32_t& numECBytesInBlock) {
  if (blockID >= numRSBlocks) {
    return;
  }
  int32_t numRsBlocksInGroup2 = numTotalBytes % numRSBlocks;
  int32_t numRsBlocksInGroup1 = numRSBlocks - numRsBlocksInGroup2;
  int32_t numTotalBytesInGroup1 = numTotalBytes / numRSBlocks;
  int32_t numTotalBytesInGroup2 = numTotalBytesInGroup1 + 1;
  int32_t numDataBytesInGroup1 = numDataBytes / numRSBlocks;
  int32_t numDataBytesInGroup2 = numDataBytesInGroup1 + 1;
  int32_t numEcBytesInGroup1 = numTotalBytesInGroup1 - numDataBytesInGroup1;
  int32_t numEcBytesInGroup2 = numTotalBytesInGroup2 - numDataBytesInGroup2;
  if (blockID < numRsBlocksInGroup1) {
    numDataBytesInBlock = numDataBytesInGroup1;
    numECBytesInBlock = numEcBytesInGroup1;
  } else {
    numDataBytesInBlock = numDataBytesInGroup2;
    numECBytesInBlock = numEcBytesInGroup2;
  }
}
CBC_CommonByteArray* CBC_QRCoderEncoder::GenerateECBytes(
    CBC_CommonByteArray* dataBytes,
    int32_t numEcBytesInBlock,
    int32_t& e) {
  int32_t numDataBytes = dataBytes->Size();
  CFX_Int32Array toEncode;
  toEncode.SetSize(numDataBytes + numEcBytesInBlock);
  for (int32_t i = 0; i < numDataBytes; i++) {
    toEncode[i] = (dataBytes->At(i));
  }
  CBC_ReedSolomonEncoder encode(CBC_ReedSolomonGF256::QRCodeFild);
  encode.Init();
  encode.Encode(&toEncode, numEcBytesInBlock, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  CBC_CommonByteArray* ecBytes = new CBC_CommonByteArray(numEcBytesInBlock);
  for (int32_t j = 0; j < numEcBytesInBlock; j++) {
    ecBytes->Set(j, toEncode[numDataBytes + j]);
  }
  return ecBytes;
}
