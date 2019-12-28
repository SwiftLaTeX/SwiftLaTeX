// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_XML_FDE_XML_H_
#define XFA_FDE_XML_FDE_XML_H_

#include "core/fxcrt/include/fx_system.h"

enum class FDE_XmlSyntaxResult {
  None,
  InstructionOpen,
  InstructionClose,
  ElementOpen,
  ElementBreak,
  ElementClose,
  TargetName,
  TagName,
  AttriName,
  AttriValue,
  Text,
  CData,
  TargetData,
  Error,
  EndOfString
};

enum FDE_XMLNODETYPE {
  FDE_XMLNODE_Unknown = 0,
  FDE_XMLNODE_Instruction,
  FDE_XMLNODE_Element,
  FDE_XMLNODE_Text,
  FDE_XMLNODE_CharData,
};

struct FDE_XMLNODE {
  int32_t iNodeNum;
  FDE_XMLNODETYPE eNodeType;
};

FX_BOOL FDE_IsXMLValidChar(FX_WCHAR ch);

#endif  // XFA_FDE_XML_FDE_XML_H_
