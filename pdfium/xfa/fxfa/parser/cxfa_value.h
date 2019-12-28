// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_VALUE_H_
#define XFA_FXFA_PARSER_CXFA_VALUE_H_

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/parser/cxfa_arc.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_exdata.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_line.h"
#include "xfa/fxfa/parser/cxfa_rectangle.h"
#include "xfa/fxfa/parser/cxfa_text.h"

class CXFA_Node;

class CXFA_Value : public CXFA_Data {
 public:
  explicit CXFA_Value(CXFA_Node* pNode) : CXFA_Data(pNode) {}

  XFA_Element GetChildValueClassID();
  FX_BOOL GetChildValueContent(CFX_WideString& wsContent);
  CXFA_Arc GetArc();
  CXFA_Line GetLine();
  CXFA_Rectangle GetRectangle();
  CXFA_Text GetText();
  CXFA_ExData GetExData();
  CXFA_Image GetImage();
};

#endif  // XFA_FXFA_PARSER_CXFA_VALUE_H_
