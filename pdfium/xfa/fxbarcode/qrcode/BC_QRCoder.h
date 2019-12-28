// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODER_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODER_H_

class CBC_QRCoderErrorCorrectionLevel;
class CBC_QRCoderMode;
class CBC_CommonByteMatrix;

class CBC_QRCoder {
 private:
  CBC_QRCoderMode* m_mode;
  CBC_QRCoderErrorCorrectionLevel* m_ecLevel;
  int32_t m_version;
  int32_t m_matrixWidth;
  int32_t m_maskPattern;
  int32_t m_numTotalBytes;
  int32_t m_numDataBytes;
  int32_t m_numECBytes;
  int32_t m_numRSBlocks;
  CBC_CommonByteMatrix* m_matrix;

 public:
  static const int32_t NUM_MASK_PATTERNS;
  CBC_QRCoder();
  virtual ~CBC_QRCoder();
  CBC_QRCoderMode* GetMode();
  CBC_QRCoderErrorCorrectionLevel* GetECLevel();
  int32_t GetVersion();
  int32_t GetMatrixWidth();
  int32_t GetMaskPattern();
  int32_t GetNumTotalBytes();
  int32_t GetNumDataBytes();
  int32_t GetNumECBytes();
  int32_t GetNumRSBlocks();
  CBC_CommonByteMatrix* GetMatrix();
  int32_t At(int32_t x, int32_t y, int32_t& e);
  FX_BOOL IsValid();

  void SetMode(CBC_QRCoderMode* value);
  void SetECLevel(CBC_QRCoderErrorCorrectionLevel* ecLevel);
  void SetVersion(int32_t version);
  void SetMatrixWidth(int32_t width);
  void SetMaskPattern(int32_t pattern);
  void SetNumDataBytes(int32_t bytes);
  void SetNumTotalBytes(int32_t value);
  void SetNumECBytes(int32_t value);
  void SetNumRSBlocks(int32_t block);
  void SetMatrix(CBC_CommonByteMatrix* value);
  static FX_BOOL IsValidMaskPattern(int32_t maskPattern);
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODER_H_
