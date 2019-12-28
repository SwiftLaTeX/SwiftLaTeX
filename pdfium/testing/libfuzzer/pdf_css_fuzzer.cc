// Copyright 2016 The PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fde/css/fde_css.h"
#include "xfa/fde/css/fde_csssyntax.h"
#include "xfa/fgas/crt/fgas_stream.h"
#include "xfa/fxfa/parser/xfa_utils.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  CFDE_CSSSyntaxParser parser;

  CFX_WideString input = CFX_WideString::FromUTF8(
      CFX_ByteStringC(data, static_cast<FX_STRSIZE>(size)));
  std::unique_ptr<IFX_Stream, ReleaseDeleter<IFX_Stream>> stream(
      XFA_CreateWideTextRead(input));
  if (!stream)
    return 0;

  parser.Init(stream.get(), 1024);

  FDE_CSSSYNTAXSTATUS status = parser.DoSyntaxParse();
  while (status != FDE_CSSSYNTAXSTATUS_Error &&
         status != FDE_CSSSYNTAXSTATUS_EOS)
    status = parser.DoSyntaxParse();

  return 0;
}
