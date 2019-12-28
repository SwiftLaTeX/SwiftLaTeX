// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODERENCODER_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODERENCODER_H_

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_string.h"

class CBC_QRCoder;
class CBC_QRCoderErrorCorrectionLevel;
class CBC_QRCoderMode;
class CBC_QRCoderBitVector;
class CBC_CommonByteArray;
class CBC_CommonByteMatrix;
class Make_Pair;

class CBC_QRCoderEncoder {
 private:
  static const int32_t m_alphaNumbericTable[96];

 public:
  CBC_QRCoderEncoder();
  virtual ~CBC_QRCoderEncoder();

  static void Encode(const CFX_ByteString& content,
                     CBC_QRCoderErrorCorrectionLevel* ecLevel,
                     CBC_QRCoder* qrCode,
                     int32_t& e,
                     int32_t versionSpecify = 0);
  static void Encode(const CFX_WideString& content,
                     CBC_QRCoderErrorCorrectionLevel* ecLevel,
                     CBC_QRCoder* qrCode,
                     int32_t& e);
  static void EncodeWithSpecifyVersion(const CFX_ByteString& content,
                                       CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                       CBC_QRCoder* qrCode,
                                       int32_t versionSpecify,
                                       int32_t& e);
  static void EncodeWithAutoVersion(const CFX_ByteString& content,
                                    CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                    CBC_QRCoder* qrCode,
                                    int32_t& e);
  static CBC_QRCoderMode* ChooseMode(const CFX_ByteString& content,
                                     CFX_ByteString encoding);
  static int32_t GetAlphaNumericCode(int32_t code);
  static void AppendECI(CBC_QRCoderBitVector* bits);
  static void AppendBytes(const CFX_ByteString& content,
                          CBC_QRCoderMode* mode,
                          CBC_QRCoderBitVector* bits,
                          CFX_ByteString encoding,
                          int32_t& e);
  static void AppendNumericBytes(const CFX_ByteString& content,
                                 CBC_QRCoderBitVector* bits,
                                 int32_t& e);
  static void AppendAlphaNumericBytes(const CFX_ByteString& content,
                                      CBC_QRCoderBitVector* bits,
                                      int32_t& e);
  static void Append8BitBytes(const CFX_ByteString& content,
                              CBC_QRCoderBitVector* bits,
                              CFX_ByteString encoding,
                              int32_t& e);
  static void Append8BitBytes(CFX_ByteArray& bytes,
                              CBC_QRCoderBitVector* bits,
                              int32_t& e);
  static void AppendKanjiBytes(const CFX_ByteString& content,
                               CBC_QRCoderBitVector* bits,
                               int32_t& e);
  static void AppendGBKBytes(const CFX_ByteString& content,
                             CBC_QRCoderBitVector* bits,
                             int32_t& e);
  static void InitQRCode(int32_t numInputBytes,
                         int32_t versionNumber,
                         CBC_QRCoderErrorCorrectionLevel* ecLevel,
                         CBC_QRCoderMode* mode,
                         CBC_QRCoder* qrCode,
                         int32_t& e);
  static void InitQRCode(int32_t numInputBytes,
                         CBC_QRCoderErrorCorrectionLevel* ecLevel,
                         CBC_QRCoderMode* mode,
                         CBC_QRCoder* qrCode,
                         int32_t& e);
  static void AppendModeInfo(CBC_QRCoderMode* mode,
                             CBC_QRCoderBitVector* bits,
                             int32_t& e);
  static void AppendLengthInfo(int32_t numLetters,
                               int32_t version,
                               CBC_QRCoderMode* mode,
                               CBC_QRCoderBitVector* bits,
                               int32_t& e);

  static void InterleaveWithECBytes(CBC_QRCoderBitVector* bits,
                                    int32_t numTotalBytes,
                                    int32_t numDataBytes,
                                    int32_t numRSBlocks,
                                    CBC_QRCoderBitVector* result,
                                    int32_t& e);
  static void GetNumDataBytesAndNumECBytesForBlockID(
      int32_t numTotalBytes,
      int32_t numDataBytes,
      int32_t numRSBlocks,
      int32_t blockID,
      int32_t& numDataBytesInBlock,
      int32_t& numECBytesInBlocks);
  static CBC_CommonByteArray* GenerateECBytes(CBC_CommonByteArray* dataBytes,
                                              int32_t numEcBytesInBlock,
                                              int32_t& e);
  static int32_t ChooseMaskPattern(CBC_QRCoderBitVector* bits,
                                   CBC_QRCoderErrorCorrectionLevel* ecLevel,
                                   int32_t version,
                                   CBC_CommonByteMatrix* matrix,
                                   int32_t& e);
  static int32_t CalculateMaskPenalty(CBC_CommonByteMatrix* matrix);
  static void TerminateBits(int32_t numDataBytes,
                            CBC_QRCoderBitVector* bits,
                            int32_t& e);
  static int32_t GetSpanByVersion(CBC_QRCoderMode* modeFirst,
                                  CBC_QRCoderMode* modeSecond,
                                  int32_t versionNum,
                                  int32_t& e);
  static void MergeString(CFX_ArrayTemplate<Make_Pair*>* result,
                          int32_t versionNum,
                          int32_t& e);
  static void SplitString(const CFX_ByteString& content,
                          CFX_ArrayTemplate<Make_Pair*>* result);
  static void AppendDataModeLenghInfo(
      const CFX_ArrayTemplate<Make_Pair*>& splitResult,
      CBC_QRCoderBitVector& headerAndDataBits,
      CBC_QRCoderMode* tempMode,
      CBC_QRCoder* qrCode,
      CFX_ByteString& encoding,
      int32_t& e);
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODERENCODER_H_
