// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_stream.h"

FX_BOOL IFX_FileWrite::WriteBlock(const void* pData, size_t size) {
  return WriteBlock(pData, GetSize(), size);
}

FX_BOOL IFX_FileRead::IsEOF() {
  return FALSE;
}

FX_FILESIZE IFX_FileRead::GetPosition() {
  return 0;
}

size_t IFX_FileRead::ReadBlock(void* buffer, size_t size) {
  return 0;
}

FX_BOOL IFX_FileStream::WriteBlock(const void* buffer, size_t size) {
  return WriteBlock(buffer, GetSize(), size);
}
