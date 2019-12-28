// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODEWRITER_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODEWRITER_H_

#include "xfa/fxbarcode/BC_TwoDimWriter.h"

class CBC_TwoDimWriter;
class CBC_QRCodeWriter : public CBC_TwoDimWriter {
 public:
  CBC_QRCodeWriter();
  ~CBC_QRCodeWriter() override;

  uint8_t* Encode(const CFX_WideString& contents,
                  int32_t ecLevel,
                  int32_t& outWidth,
                  int32_t& outHeight,
                  int32_t& e);
  uint8_t* Encode(const CFX_ByteString& contents,
                  BCFORMAT format,
                  int32_t& outWidth,
                  int32_t& outHeight,
                  int32_t hints,
                  int32_t& e);
  uint8_t* Encode(const CFX_ByteString& contents,
                  BCFORMAT format,
                  int32_t& outWidth,
                  int32_t& outHeight,
                  int32_t& e);
  FX_BOOL SetVersion(int32_t version);

  // CBC_TwoDimWriter
  FX_BOOL SetErrorCorrectionLevel(int32_t level) override;

  static void ReleaseAll();

 private:
  int32_t m_iVersion;
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODEWRITER_H_
