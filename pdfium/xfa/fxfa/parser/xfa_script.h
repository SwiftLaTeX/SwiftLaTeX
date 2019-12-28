// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_SCRIPT_H_
#define XFA_FXFA_PARSER_XFA_SCRIPT_H_

#include "fxjs/include/cfxjse_value.h"
#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/parser/cxfa_valuearray.h"

#define XFA_RESOLVENODE_Children 0x0001
#define XFA_RESOLVENODE_Attributes 0x0004
#define XFA_RESOLVENODE_Properties 0x0008
#define XFA_RESOLVENODE_Siblings 0x0020
#define XFA_RESOLVENODE_Parent 0x0040
#define XFA_RESOLVENODE_AnyChild 0x0080
#define XFA_RESOLVENODE_ALL 0x0100
#define XFA_RESOLVENODE_CreateNode 0x0400
#define XFA_RESOLVENODE_Bind 0x0800
#define XFA_RESOLVENODE_BindNew 0x1000

enum XFA_SCRIPTLANGTYPE {
  XFA_SCRIPTLANGTYPE_Formcalc = XFA_SCRIPTTYPE_Formcalc,
  XFA_SCRIPTLANGTYPE_Javascript = XFA_SCRIPTTYPE_Javascript,
  XFA_SCRIPTLANGTYPE_Unkown = XFA_SCRIPTTYPE_Unkown,
};

enum XFA_RESOVENODE_RSTYPE {
  XFA_RESOVENODE_RSTYPE_Nodes,
  XFA_RESOVENODE_RSTYPE_Attribute,
  XFA_RESOLVENODE_RSTYPE_CreateNodeOne,
  XFA_RESOLVENODE_RSTYPE_CreateNodeAll,
  XFA_RESOLVENODE_RSTYPE_CreateNodeMidAll,
  XFA_RESOVENODE_RSTYPE_ExistNodes,
};

struct XFA_RESOLVENODE_RS {
  XFA_RESOLVENODE_RS();
  ~XFA_RESOLVENODE_RS();

  int32_t GetAttributeResult(CXFA_ValueArray& valueArray) const {
    if (pScriptAttribute && pScriptAttribute->eValueType == XFA_SCRIPT_Object) {
      v8::Isolate* pIsolate = valueArray.m_pIsolate;
      for (int32_t i = 0; i < nodes.GetSize(); i++) {
        std::unique_ptr<CFXJSE_Value> pValue(new CFXJSE_Value(pIsolate));
        (nodes[i]->*(pScriptAttribute->lpfnCallback))(
            pValue.get(), FALSE, (XFA_ATTRIBUTE)pScriptAttribute->eAttribute);
        valueArray.Add(pValue.release());
      }
    }
    return valueArray.GetSize();
  }

  CXFA_ObjArray nodes;
  XFA_RESOVENODE_RSTYPE dwFlags;
  const XFA_SCRIPTATTRIBUTEINFO* pScriptAttribute;
};

inline XFA_RESOLVENODE_RS::XFA_RESOLVENODE_RS()
    : dwFlags(XFA_RESOVENODE_RSTYPE_Nodes), pScriptAttribute(nullptr) {}

inline XFA_RESOLVENODE_RS::~XFA_RESOLVENODE_RS() {
  nodes.RemoveAll();
}
#endif  // XFA_FXFA_PARSER_XFA_SCRIPT_H_
