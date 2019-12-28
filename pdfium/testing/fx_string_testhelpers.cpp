// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/fx_string_testhelpers.h"

#include <iomanip>
#include <ios>

namespace {

template <typename T>
std::ostream& output_string(std::ostream& out, const T& str) {
  out << std::hex << std::setfill('0') << '"';
  // This function is used for FX strings whose length is defined as int.
  for (int i = 0; i < str.GetLength(); ++i) {
    unsigned int c = str.GetAt(i);
    if (c >= 0x20 && c < 0x7F) {
      out << static_cast<char>(c);
    } else if (sizeof(typename T::CharType) == 1) {
      out << "\\x" << std::setw(2) << c << std::setw(0);
    } else if (c < 0x10000) {
      out << "\\u" << std::setw(4) << c << std::setw(0);
    } else {
      out << "<invalid>";
    }
  }
  out << '"' << std::dec << std::setfill(' ');
  return out;
}

}  // namespace

std::ostream& operator<<(std::ostream& out, const CFX_ByteStringC& str) {
  return output_string(out, str);
}

std::ostream& operator<<(std::ostream& out, const CFX_ByteString& str) {
  return output_string(out, str);
}

std::ostream& operator<<(std::ostream& out, const CFX_WideStringC& str) {
  return output_string(out, str);
}

std::ostream& operator<<(std::ostream& out, const CFX_WideString& str) {
  return output_string(out, str);
}
