// Copyright 2016 The PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstddef>
#include <cstdint>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/fm2js/xfa_program.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size > std::numeric_limits<FX_STRSIZE>::max())
    return 0;

  CFX_WideString input = CFX_WideString::FromUTF8(
      CFX_ByteStringC(data, static_cast<FX_STRSIZE>(size)));
  CXFA_FMProgram program;
  if (program.Init(input.AsStringC()) || program.ParseProgram())
    return 0;

  CFX_WideTextBuf js;
  program.TranslateProgram(js);
  return 0;
}
