// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTING_FX_STRING_TESTHELPERS_H_
#define TESTING_FX_STRING_TESTHELPERS_H_

#include <ostream>

#include "core/fxcrt/include/fx_string.h"

// Output stream operator so GTEST macros work with FX strings.
std::ostream& operator<<(std::ostream& out, const CFX_ByteStringC& str);
std::ostream& operator<<(std::ostream& out, const CFX_ByteString& str);
std::ostream& operator<<(std::ostream& out, const CFX_WideStringC& str);
std::ostream& operator<<(std::ostream& out, const CFX_WideString& str);

#endif  // TESTING_FX_STRING_TESTHELPERS_H_
