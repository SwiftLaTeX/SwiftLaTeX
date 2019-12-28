// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_BASIC_DATA_H_
#define XFA_FXFA_PARSER_XFA_BASIC_DATA_H_

#include "xfa/fxfa/include/fxfa_basic.h"
#include "xfa/fxfa/parser/xfa_basic_imp.h"

extern const XFA_PACKETINFO g_XFAPacketData[];
extern const int32_t g_iXFAPacketCount;

extern const XFA_ATTRIBUTEENUMINFO g_XFAEnumData[];
extern const int32_t g_iXFAEnumCount;

extern const XFA_ATTRIBUTEINFO g_XFAAttributeData[];
extern const int32_t g_iXFAAttributeCount;

extern const XFA_NOTSUREATTRIBUTE g_XFANotsureAttributes[];
extern const int32_t g_iXFANotsureCount;

extern const XFA_ELEMENTINFO g_XFAElementData[];
extern const int32_t g_iXFAElementCount;

extern const XFA_ELEMENTHIERARCHY g_XFAElementChildrenIndex[];
extern const uint16_t g_XFAElementChildrenData[];

extern const XFA_ELEMENTHIERARCHY g_XFAElementAttributeIndex[];
extern const uint8_t g_XFAElementAttributeData[];

extern const XFA_ELEMENTHIERARCHY g_XFAElementPropertyIndex[];
extern const XFA_PROPERTY g_XFAElementPropertyData[];

extern const XFA_SCRIPTHIERARCHY g_XFAScriptIndex[];

extern const XFA_NOTSUREATTRIBUTE g_XFANotsureAttributes[];
extern const int32_t g_iXFANotsureCount;

extern const XFA_METHODINFO g_SomMethodData[];
extern const int32_t g_iSomMethodCount;

extern const XFA_SCRIPTATTRIBUTEINFO g_SomAttributeData[];
extern const int32_t g_iSomAttributeCount;

#endif  // XFA_FXFA_PARSER_XFA_BASIC_DATA_H_
