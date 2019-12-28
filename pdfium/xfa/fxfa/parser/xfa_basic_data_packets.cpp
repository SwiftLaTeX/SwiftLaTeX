// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_basic_data.h"

#include "xfa/fxfa/include/fxfa_basic.h"

const XFA_PACKETINFO g_XFAPacketData[] = {
    {0x0, nullptr, XFA_XDPPACKET_USER, nullptr,
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTMANY},
    {0x811929d, L"sourceSet", XFA_XDPPACKET_SourceSet,
     L"http://www.xfa.org/schema/xfa-source-set/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0xb843dba, L"pdf", XFA_XDPPACKET_Pdf, L"http://ns.adobe.com/xdp/pdf/",
     XFA_XDPPACKET_FLAGS_COMPLETEMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0xc56afbf, L"xdc", XFA_XDPPACKET_Xdc, L"http://www.xfa.org/schema/xdc/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0xc56afcc, L"xdp", XFA_XDPPACKET_XDP, L"http://ns.adobe.com/xdp/",
     XFA_XDPPACKET_FLAGS_COMPLETEMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0x132a8fbc, L"xmpmeta", XFA_XDPPACKET_Xmpmeta,
     L"http://ns.adobe.com/xmpmeta/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTMANY},
    {0x48d004a8, L"xfdf", XFA_XDPPACKET_Xfdf, L"http://ns.adobe.com/xfdf/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0x4e1e39b6, L"config", XFA_XDPPACKET_Config,
     L"http://www.xfa.org/schema/xci/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0x5473b6dc, L"localeSet", XFA_XDPPACKET_LocaleSet,
     L"http://www.xfa.org/schema/xfa-locale-set/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0x6038580a, L"stylesheet", XFA_XDPPACKET_Stylesheet,
     L"http://www.w3.org/1999/XSL/Transform",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTMANY},
    {0x803550fc, L"template", XFA_XDPPACKET_Template,
     L"http://www.xfa.org/schema/xfa-template/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0x8b036f32, L"signature", XFA_XDPPACKET_Signature,
     L"http://www.w3.org/2000/09/xmldsig#",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0x99b95079, L"datasets", XFA_XDPPACKET_Datasets,
     L"http://www.xfa.org/schema/xfa-data/",
     XFA_XDPPACKET_FLAGS_PREFIXMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0xcd309ff4, L"form", XFA_XDPPACKET_Form,
     L"http://www.xfa.org/schema/xfa-form/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
    {0xe14c801c, L"connectionSet", XFA_XDPPACKET_ConnectionSet,
     L"http://www.xfa.org/schema/xfa-connection-set/",
     XFA_XDPPACKET_FLAGS_NOMATCH | XFA_XDPPACKET_FLAGS_SUPPORTONE},
};
const int32_t g_iXFAPacketCount =
    sizeof(g_XFAPacketData) / sizeof(XFA_PACKETINFO);
