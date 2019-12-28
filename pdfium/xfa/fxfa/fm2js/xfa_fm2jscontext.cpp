// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/xfa_fm2jscontext.h"

#include <time.h>

#include "core/fxcrt/include/fx_ext.h"
#include "fxjs/include/cfxjse_arguments.h"
#include "fxjs/include/cfxjse_class.h"
#include "fxjs/include/cfxjse_value.h"
#include "xfa/fgas/localization/fgas_locale.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/fm2js/xfa_program.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localevalue.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"

namespace {

const double kFinancialPrecision = 0.00000001;

struct XFA_FMHtmlReserveCode {
  uint32_t m_uCode;
  const FX_WCHAR* m_htmlReserve;
};

struct XFA_FMHtmlHashedReserveCode {
  uint32_t m_uHash;
  uint32_t m_uCode;
};

const XFA_FMHtmlHashedReserveCode reservesForDecode[] = {
    {0x00018b62, /*L"Mu",*/ 924},       {0x00019083, /*L"Nu",*/ 925},
    {0x00019ab9, /*L"Pi",*/ 928},       {0x0001c3c1, /*L"Xi",*/ 926},
    {0x000210ac, /*L"ge",*/ 8805},      {0x000210bb, /*L"gt",*/ 62},
    {0x00022a51, /*L"le",*/ 8804},      {0x00022a60, /*L"lt",*/ 60},
    {0x00022f82, /*L"mu",*/ 956},       {0x00023493, /*L"ne",*/ 8800},
    {0x00023497, /*L"ni",*/ 8715},      {0x000234a3, /*L"nu",*/ 957},
    {0x000239c1, /*L"or",*/ 8744},      {0x00023ed9, /*L"pi",*/ 960},
    {0x000267e1, /*L"xi",*/ 958},       {0x00c41789, /*L"lceil",*/ 8968},
    {0x00eef34f, /*L"thetasym",*/ 977}, {0x012d7ead, /*L"lcirc",*/ 206},
    {0x01637b56, /*L"agrave",*/ 224},   {0x020856da, /*L"crarr",*/ 8629},
    {0x022188c3, /*L"gamma",*/ 947},    {0x033586d3, /*L"nbsp",*/ 160},
    {0x04f4c358, /*L"nsub",*/ 8836},    {0x0581466a, /*L"dagger",*/ 8224},
    {0x06b1f790, /*L"oelig",*/ 339},    {0x06e490d4, /*L"Chi",*/ 935},
    {0x0718c6a1, /*L"ETH",*/ 208},      {0x07196ada, /*L"Eta",*/ 919},
    {0x07f667ca, /*L"Ugrave",*/ 217},   {0x083a8a21, /*L"Phi",*/ 934},
    {0x083ac28c, /*L"Psi",*/ 936},      {0x086f26a9, /*L"Rho",*/ 929},
    {0x089b5b51, /*L"aring",*/ 229},    {0x08a39f4a, /*L"Tau",*/ 932},
    {0x08b6188b, /*L"THORN",*/ 222},    {0x09ce792a, /*L"icirc",*/ 238},
    {0x09f9d61e, /*L"amp",*/ 38},       {0x09f9db33, /*L"and",*/ 8743},
    {0x09f9db36, /*L"ang",*/ 8736},     {0x0a2e3514, /*L"cap",*/ 8745},
    {0x0a2e58f4, /*L"chi",*/ 967},      {0x0a2e9ba8, /*L"cup",*/ 8746},
    {0x0a4897d0, /*L"deg",*/ 176},      {0x0a6332fa, /*L"eta",*/ 951},
    {0x0a633301, /*L"eth",*/ 240},      {0x0acc4d4b, /*L"int",*/ 8747},
    {0x0b1b3d35, /*L"loz",*/ 9674},     {0x0b1b4c8b, /*L"lrm",*/ 8206},
    {0x0b4fd9b1, /*L"not",*/ 172},      {0x0b845241, /*L"phi",*/ 966},
    {0x0b84576f, /*L"piv",*/ 982},      {0x0b848aac, /*L"psi",*/ 968},
    {0x0bb8df5e, /*L"reg",*/ 174},      {0x0bb8eec9, /*L"rho",*/ 961},
    {0x0bb9034b, /*L"rlm",*/ 8207},     {0x0bd33d14, /*L"shy",*/ 173},
    {0x0bd34229, /*L"sim",*/ 8764},     {0x0bd37faa, /*L"sub",*/ 8834},
    {0x0bd37fb5, /*L"sum",*/ 8721},     {0x0bd37fb8, /*L"sup",*/ 8835},
    {0x0bed676a, /*L"tau",*/ 964},      {0x0c07f32e, /*L"uml",*/ 168},
    {0x0c71032c, /*L"yen",*/ 165},      {0x0c7f2889, /*L"szlig",*/ 223},
    {0x0c8badbb, /*L"zwj",*/ 8205},     {0x10ba4dba, /*L"Egrave",*/ 200},
    {0x10f1ea24, /*L"para",*/ 182},     {0x10f1ea37, /*L"part",*/ 8706},
    {0x115b2337, /*L"perp",*/ 8869},    {0x12b10d15, /*L"prod",*/ 8719},
    {0x12b10d21, /*L"prop",*/ 8733},    {0x12dfa9f4, /*L"rfloor",*/ 8971},
    {0x12eb4736, /*L"Agrave",*/ 192},   {0x12fff2b7, /*L"pund",*/ 163},
    {0x13fda9f2, /*L"tilde",*/ 732},    {0x1417fd62, /*L"times",*/ 215},
    {0x154fc726, /*L"ecirc",*/ 234},    {0x165aa451, /*L"sigma",*/ 963},
    {0x1709124a, /*L"Dagger",*/ 8225},  {0x192f78d5, /*L"iexcl",*/ 161},
    {0x1b7ed8d7, /*L"rArr",*/ 8658},    {0x1ec88c68, /*L"rang",*/ 9002},
    {0x1ec8a0f7, /*L"rarr",*/ 8594},    {0x1eda07f3, /*L"atilde",*/ 227},
    {0x1f3182c4, /*L"real",*/ 8476},    {0x1fc34f8b, /*L"yacute",*/ 253},
    {0x20d11522, /*L"acirc",*/ 226},    {0x21933a9b, /*L"rsaquo",*/ 8250},
    {0x21f44907, /*L"uacute",*/ 250},   {0x220cca72, /*L"acute",*/ 180},
    {0x242cded1, /*L"alefsym",*/ 8501}, {0x2655c66a, /*L"delta",*/ 948},
    {0x269e4b4d, /*L"exist",*/ 8707},   {0x273379fa, /*L"micro",*/ 181},
    {0x27a37440, /*L"forall",*/ 8704},  {0x2854e62c, /*L"minus",*/ 8722},
    {0x28636f81, /*L"cedil",*/ 184},    {0x2887357b, /*L"iacute",*/ 237},
    {0x2994d5ff, /*L"frac12",*/ 189},   {0x2994d601, /*L"frac14",*/ 188},
    {0x2994e043, /*L"frac34",*/ 190},   {0x2a1feb41, /*L"lambda",*/ 955},
    {0x2ab215f3, /*L"apos",*/ 39},      {0x2ab82ef7, /*L"eacute",*/ 233},
    {0x2b3592ef, /*L"auml",*/ 228},     {0x2ce92873, /*L"aacute",*/ 225},
    {0x2daff48a, /*L"oslash",*/ 248},   {0x2ef68882, /*L"aelig",*/ 230},
    {0x3061d3d3, /*L"Atilde",*/ 195},   {0x314b1b6b, /*L"Yacute",*/ 221},
    {0x337c14e7, /*L"Uacute",*/ 218},   {0x37676aca, /*L"cent",*/ 162},
    {0x37d0b841, /*L"circ",*/ 710},     {0x386e7947, /*L"cong",*/ 8773},
    {0x386e839b, /*L"copy",*/ 169},     {0x3a0e225a, /*L"Epsilon",*/ 917},
    {0x3ba7b721, /*L"Lambda",*/ 923},   {0x3bd9abe6, /*L"Alpha",*/ 913},
    {0x3c3ffad7, /*L"Eacute",*/ 201},   {0x3cfaf69f, /*L"brvbar",*/ 166},
    {0x3d54a489, /*L"omega",*/ 969},    {0x3e70f453, /*L"Aacute",*/ 193},
    {0x3f37c06a, /*L"Oslash",*/ 216},   {0x40e1b34e, /*L"diams",*/ 9830},
    {0x416596df, /*L"plusmn",*/ 177},   {0x4354ff16, /*L"Ucirc",*/ 219},
    {0x454fce6a, /*L"Upsilon",*/ 933},  {0x4610ad35, /*L"emsp",*/ 8195},
    {0x462afb76, /*L"ensp",*/ 8194},    {0x46e30073, /*L"euml",*/ 235},
    {0x46e31a1b, /*L"euro",*/ 8364},    {0x46f2eada, /*L"lowast",*/ 8727},
    {0x4dca26cf, /*L"Auml",*/ 196},     {0x4e2d6083, /*L"image",*/ 8465},
    {0x4f964ee8, /*L"notin",*/ 8713},   {0x50917a7a, /*L"epsilon",*/ 949},
    {0x52f9a4cd, /*L"Kappa",*/ 922},    {0x5496f410, /*L"Ocirc",*/ 212},
    {0x568cbf34, /*L"zeta",*/ 950},     {0x57badd20, /*L"ntilde",*/ 241},
    {0x58662109, /*L"zwnj",*/ 8204},    {0x5b39870f, /*L"empty",*/ 8709},
    {0x5bd3268a, /*L"upsilon",*/ 965},  {0x5e2bf8a3, /*L"Gamma",*/ 915},
    {0x5f73c13a, /*L"rsquo",*/ 8217},   {0x61f2bc4d, /*L"iota",*/ 953},
    {0x625bbcf3, /*L"isin",*/ 8712},    {0x62906df7, /*L"iuml",*/ 239},
    {0x64a5cb31, /*L"Aring",*/ 197},    {0x66f25c4a, /*L"sbquo",*/ 8218},
    {0x6851ab60, /*L"spades",*/ 9824},  {0x6942a900, /*L"Ntilde",*/ 209},
    {0x69779453, /*L"Euml",*/ 203},     {0x6cda6e23, /*L"current",*/ 164},
    {0x70b5b634, /*L"lsquo",*/ 8216},   {0x715a3706, /*L"Ecirc",*/ 202},
    {0x71e8bf8d, /*L"tdquo",*/ 8221},   {0x72651431, /*L"Sigma",*/ 931},
    {0x7569813b, /*L"iquest",*/ 191},   {0x776a436a, /*L"equiv",*/ 8801},
    {0x79215314, /*L"Zeta",*/ 918},     {0x79b81224, /*L"ograve",*/ 242},
    {0x7c2f8b23, /*L"macr",*/ 175},     {0x7cdb8502, /*L"Acirc",*/ 194},
    {0x8185c62e, /*L"ndash",*/ 8211},   {0x8260364a, /*L"Delta",*/ 916},
    {0x846619ad, /*L"mdash",*/ 8212},   {0x8550fb50, /*L"OElig",*/ 338},
    {0x88eb5b85, /*L"ldquo",*/ 8220},   {0x8b3fde04, /*L"Ograve",*/ 210},
    {0x8bc5794b, /*L"ordf",*/ 170},     {0x8bc57952, /*L"ordm",*/ 186},
    {0x8c14923d, /*L"ouml",*/ 246},     {0x8c5a7cd6, /*L"theta",*/ 952},
    {0x8d61812b, /*L"thorn",*/ 254},    {0x912b95aa, /*L"asymp",*/ 8776},
    {0x947faf81, /*L"middot",*/ 183},   {0x9629202e, /*L"lfloor",*/ 8970},
    {0x972e9ec1, /*L"otilde",*/ 245},   {0x9748f231, /*L"otimes",*/ 8855},
    {0x995f1469, /*L"Omega",*/ 937},    {0x99eb5349, /*L"quot",*/ 34},
    {0x9aeb639e, /*L"hellip",*/ 8230},  {0xa0ae2f86, /*L"Scaron",*/ 352},
    {0xa4dcb0d5, /*L"lsaquo",*/ 8249},  {0xa53dbf41, /*L"oacute",*/ 243},
    {0xa5ae9e7b, /*L"bdquo",*/ 8222},   {0xa602d7ba, /*L"sdot",*/ 8901},
    {0xa61ce86f, /*L"sect",*/ 167},     {0xa6e4c3d7, /*L"sigmaf",*/ 962},
    {0xa7c1c74f, /*L"sube",*/ 8838},    {0xa7c20ee9, /*L"sup1",*/ 185},
    {0xa7c20eea, /*L"sup2",*/ 178},     {0xa7c20eeb, /*L"sup3",*/ 179},
    {0xa7c20f1d, /*L"supe",*/ 8839},    {0xa8b66aa1, /*L"Otilde",*/ 213},
    {0xad958c42, /*L"AElig",*/ 198},    {0xaea9261d, /*L"Ouml",*/ 214},
    {0xb040eafa, /*L"uArr",*/ 8657},    {0xb07c2e1c, /*L"beta",*/ 946},
    {0xb220e92f, /*L"bull",*/ 8226},    {0xb22750c4, /*L"ccedil",*/ 231},
    {0xb38ab31a, /*L"uarr",*/ 8593},    {0xb598b683, /*L"uuml",*/ 252},
    {0xb6c58b21, /*L"Oacute",*/ 211},   {0xb6d2a617, /*L"oline",*/ 8254},
    {0xba9fd989, /*L"dArr",*/ 8659},    {0xbb5ccd41, /*L"lgrave",*/ 204},
    {0xbd39b44c, /*L"weierp",*/ 8472},  {0xbde9a1a9, /*L"darr",*/ 8595},
    {0xc027e329, /*L"permil",*/ 8240},  {0xc2451389, /*L"upsih",*/ 978},
    {0xc3af1ca4, /*L"Ccedil",*/ 199},   {0xcd164249, /*L"fnof",*/ 402},
    {0xcf6c8467, /*L"hearts",*/ 9829},  {0xd1228390, /*L"trade",*/ 8482},
    {0xd1462407, /*L"yuml",*/ 255},     {0xd2cf2253, /*L"oplus",*/ 8853},
    {0xd310c1fc, /*L"Beta",*/ 914},     {0xd59c4d74, /*L"infin",*/ 8734},
    {0xd64d470d, /*L"hArr",*/ 8660},    {0xd67d9c75, /*L"divide",*/ 247},
    {0xd698dd37, /*L"Omicron",*/ 927},  {0xd82d4a63, /*L"Uuml",*/ 220},
    {0xd9970f2d, /*L"harr",*/ 8596},    {0xda91fd99, /*L"clubs",*/ 9827},
    {0xdbe5bdcc, /*L"there4",*/ 8756},  {0xdd7671bd, /*L"prime",*/ 8242},
    {0xdfcf3c06, /*L"alpha",*/ 945},    {0xe0213063, /*L"saron",*/ 353},
    {0xe1911d83, /*L"radic",*/ 8730},   {0xe2e75468, /*L"raquo",*/ 187},
    {0xe6e27a5e, /*L"lacute",*/ 205},   {0xe74a8f36, /*L"ucirc",*/ 251},
    {0xe864ecb6, /*L"Theta",*/ 920},    {0xecddde5e, /*L"nabla",*/ 8711},
    {0xed1c3557, /*L"omicron",*/ 959},  {0xef82228f, /*L"rceil",*/ 8969},
    {0xf1fab491, /*L"lArr",*/ 8656},    {0xf3dab7e7, /*L"Yuml",*/ 376},
    {0xf4294962, /*L"laquo",*/ 171},    {0xf5446822, /*L"lang",*/ 9001},
    {0xf5447cb1, /*L"larr",*/ 8592},    {0xf66e9bea, /*L"ugrave",*/ 249},
    {0xf6b4ce70, /*L"lota",*/ 921},     {0xf6ef34ed, /*L"kappa",*/ 954},
    {0xf72a3a56, /*L"thinsp",*/ 8201},  {0xf752801a, /*L"luml",*/ 207},
    {0xf88c8430, /*L"ocirc",*/ 244},    {0xf9676178, /*L"frasl",*/ 8260},
    {0xfd01885e, /*L"igrave",*/ 236},   {0xff3281da, /*L"egrave",*/ 232},
};

const XFA_FMHtmlReserveCode reservesForEncode[] = {
    {34, L"quot"},     {38, L"amp"},      {39, L"apos"},
    {60, L"lt"},       {62, L"gt"},       {160, L"nbsp"},
    {161, L"iexcl"},   {162, L"cent"},    {163, L"pund"},
    {164, L"current"}, {165, L"yen"},     {166, L"brvbar"},
    {167, L"sect"},    {168, L"uml"},     {169, L"copy"},
    {170, L"ordf"},    {171, L"laquo"},   {172, L"not"},
    {173, L"shy"},     {174, L"reg"},     {175, L"macr"},
    {176, L"deg"},     {177, L"plusmn"},  {178, L"sup2"},
    {179, L"sup3"},    {180, L"acute"},   {181, L"micro"},
    {182, L"para"},    {183, L"middot"},  {184, L"cedil"},
    {185, L"sup1"},    {186, L"ordm"},    {187, L"raquo"},
    {188, L"frac14"},  {189, L"frac12"},  {190, L"frac34"},
    {191, L"iquest"},  {192, L"Agrave"},  {193, L"Aacute"},
    {194, L"Acirc"},   {195, L"Atilde"},  {196, L"Auml"},
    {197, L"Aring"},   {198, L"AElig"},   {199, L"Ccedil"},
    {200, L"Egrave"},  {201, L"Eacute"},  {202, L"Ecirc"},
    {203, L"Euml"},    {204, L"lgrave"},  {205, L"lacute"},
    {206, L"lcirc"},   {207, L"luml"},    {208, L"ETH"},
    {209, L"Ntilde"},  {210, L"Ograve"},  {211, L"Oacute"},
    {212, L"Ocirc"},   {213, L"Otilde"},  {214, L"Ouml"},
    {215, L"times"},   {216, L"Oslash"},  {217, L"Ugrave"},
    {218, L"Uacute"},  {219, L"Ucirc"},   {220, L"Uuml"},
    {221, L"Yacute"},  {222, L"THORN"},   {223, L"szlig"},
    {224, L"agrave"},  {225, L"aacute"},  {226, L"acirc"},
    {227, L"atilde"},  {228, L"auml"},    {229, L"aring"},
    {230, L"aelig"},   {231, L"ccedil"},  {232, L"egrave"},
    {233, L"eacute"},  {234, L"ecirc"},   {235, L"euml"},
    {236, L"igrave"},  {237, L"iacute"},  {238, L"icirc"},
    {239, L"iuml"},    {240, L"eth"},     {241, L"ntilde"},
    {242, L"ograve"},  {243, L"oacute"},  {244, L"ocirc"},
    {245, L"otilde"},  {246, L"ouml"},    {247, L"divide"},
    {248, L"oslash"},  {249, L"ugrave"},  {250, L"uacute"},
    {251, L"ucirc"},   {252, L"uuml"},    {253, L"yacute"},
    {254, L"thorn"},   {255, L"yuml"},    {338, L"OElig"},
    {339, L"oelig"},   {352, L"Scaron"},  {353, L"saron"},
    {376, L"Yuml"},    {402, L"fnof"},    {710, L"circ"},
    {732, L"tilde"},   {913, L"Alpha"},   {914, L"Beta"},
    {915, L"Gamma"},   {916, L"Delta"},   {917, L"Epsilon"},
    {918, L"Zeta"},    {919, L"Eta"},     {920, L"Theta"},
    {921, L"lota"},    {922, L"Kappa"},   {923, L"Lambda"},
    {924, L"Mu"},      {925, L"Nu"},      {926, L"Xi"},
    {927, L"Omicron"}, {928, L"Pi"},      {929, L"Rho"},
    {931, L"Sigma"},   {932, L"Tau"},     {933, L"Upsilon"},
    {934, L"Phi"},     {935, L"Chi"},     {936, L"Psi"},
    {937, L"Omega"},   {945, L"alpha"},   {946, L"beta"},
    {947, L"gamma"},   {948, L"delta"},   {949, L"epsilon"},
    {950, L"zeta"},    {951, L"eta"},     {952, L"theta"},
    {953, L"iota"},    {954, L"kappa"},   {955, L"lambda"},
    {956, L"mu"},      {957, L"nu"},      {958, L"xi"},
    {959, L"omicron"}, {960, L"pi"},      {961, L"rho"},
    {962, L"sigmaf"},  {963, L"sigma"},   {964, L"tau"},
    {965, L"upsilon"}, {966, L"phi"},     {967, L"chi"},
    {968, L"psi"},     {969, L"omega"},   {977, L"thetasym"},
    {978, L"upsih"},   {982, L"piv"},     {8194, L"ensp"},
    {8195, L"emsp"},   {8201, L"thinsp"}, {8204, L"zwnj"},
    {8205, L"zwj"},    {8206, L"lrm"},    {8207, L"rlm"},
    {8211, L"ndash"},  {8212, L"mdash"},  {8216, L"lsquo"},
    {8217, L"rsquo"},  {8218, L"sbquo"},  {8220, L"ldquo"},
    {8221, L"tdquo"},  {8222, L"bdquo"},  {8224, L"dagger"},
    {8225, L"Dagger"}, {8226, L"bull"},   {8230, L"hellip"},
    {8240, L"permil"}, {8242, L"prime"},  {8249, L"lsaquo"},
    {8250, L"rsaquo"}, {8254, L"oline"},  {8260, L"frasl"},
    {8364, L"euro"},   {8465, L"image"},  {8472, L"weierp"},
    {8476, L"real"},   {8482, L"trade"},  {8501, L"alefsym"},
    {8592, L"larr"},   {8593, L"uarr"},   {8594, L"rarr"},
    {8595, L"darr"},   {8596, L"harr"},   {8629, L"crarr"},
    {8656, L"lArr"},   {8657, L"uArr"},   {8658, L"rArr"},
    {8659, L"dArr"},   {8660, L"hArr"},   {8704, L"forall"},
    {8706, L"part"},   {8707, L"exist"},  {8709, L"empty"},
    {8711, L"nabla"},  {8712, L"isin"},   {8713, L"notin"},
    {8715, L"ni"},     {8719, L"prod"},   {8721, L"sum"},
    {8722, L"minus"},  {8727, L"lowast"}, {8730, L"radic"},
    {8733, L"prop"},   {8734, L"infin"},  {8736, L"ang"},
    {8743, L"and"},    {8744, L"or"},     {8745, L"cap"},
    {8746, L"cup"},    {8747, L"int"},    {8756, L"there4"},
    {8764, L"sim"},    {8773, L"cong"},   {8776, L"asymp"},
    {8800, L"ne"},     {8801, L"equiv"},  {8804, L"le"},
    {8805, L"ge"},     {8834, L"sub"},    {8835, L"sup"},
    {8836, L"nsub"},   {8838, L"sube"},   {8839, L"supe"},
    {8853, L"oplus"},  {8855, L"otimes"}, {8869, L"perp"},
    {8901, L"sdot"},   {8968, L"lceil"},  {8969, L"rceil"},
    {8970, L"lfloor"}, {8971, L"rfloor"}, {9001, L"lang"},
    {9002, L"rang"},   {9674, L"loz"},    {9824, L"spades"},
    {9827, L"clubs"},  {9829, L"hearts"}, {9830, L"diams"},
};

const FXJSE_FUNCTION_DESCRIPTOR formcalc_fm2js_functions[] = {
    {"Abs", CXFA_FM2JSContext::Abs},
    {"Avg", CXFA_FM2JSContext::Avg},
    {"Ceil", CXFA_FM2JSContext::Ceil},
    {"Count", CXFA_FM2JSContext::Count},
    {"Floor", CXFA_FM2JSContext::Floor},
    {"Max", CXFA_FM2JSContext::Max},
    {"Min", CXFA_FM2JSContext::Min},
    {"Mod", CXFA_FM2JSContext::Mod},
    {"Round", CXFA_FM2JSContext::Round},
    {"Sum", CXFA_FM2JSContext::Sum},
    {"Date", CXFA_FM2JSContext::Date},
    {"Date2Num", CXFA_FM2JSContext::Date2Num},
    {"DateFmt", CXFA_FM2JSContext::DateFmt},
    {"IsoDate2Num", CXFA_FM2JSContext::IsoDate2Num},
    {"IsoTime2Num", CXFA_FM2JSContext::IsoTime2Num},
    {"LocalDateFmt", CXFA_FM2JSContext::LocalDateFmt},
    {"LocalTimeFmt", CXFA_FM2JSContext::LocalTimeFmt},
    {"Num2Date", CXFA_FM2JSContext::Num2Date},
    {"Num2GMTime", CXFA_FM2JSContext::Num2GMTime},
    {"Num2Time", CXFA_FM2JSContext::Num2Time},
    {"Time", CXFA_FM2JSContext::Time},
    {"Time2Num", CXFA_FM2JSContext::Time2Num},
    {"TimeFmt", CXFA_FM2JSContext::TimeFmt},
    {"Apr", CXFA_FM2JSContext::Apr},
    {"Cterm", CXFA_FM2JSContext::CTerm},
    {"FV", CXFA_FM2JSContext::FV},
    {"Ipmt", CXFA_FM2JSContext::IPmt},
    {"NPV", CXFA_FM2JSContext::NPV},
    {"Pmt", CXFA_FM2JSContext::Pmt},
    {"PPmt", CXFA_FM2JSContext::PPmt},
    {"PV", CXFA_FM2JSContext::PV},
    {"Rate", CXFA_FM2JSContext::Rate},
    {"Term", CXFA_FM2JSContext::Term},
    {"Choose", CXFA_FM2JSContext::Choose},
    {"Exists", CXFA_FM2JSContext::Exists},
    {"HasValue", CXFA_FM2JSContext::HasValue},
    {"Oneof", CXFA_FM2JSContext::Oneof},
    {"Within", CXFA_FM2JSContext::Within},
    {"If", CXFA_FM2JSContext::If},
    {"Eval", CXFA_FM2JSContext::Eval},
    {"Translate", CXFA_FM2JSContext::eval_translation},
    {"Ref", CXFA_FM2JSContext::Ref},
    {"UnitType", CXFA_FM2JSContext::UnitType},
    {"UnitValue", CXFA_FM2JSContext::UnitValue},
    {"At", CXFA_FM2JSContext::At},
    {"Concat", CXFA_FM2JSContext::Concat},
    {"Decode", CXFA_FM2JSContext::Decode},
    {"Encode", CXFA_FM2JSContext::Encode},
    {"Format", CXFA_FM2JSContext::Format},
    {"Left", CXFA_FM2JSContext::Left},
    {"Len", CXFA_FM2JSContext::Len},
    {"Lower", CXFA_FM2JSContext::Lower},
    {"Ltrim", CXFA_FM2JSContext::Ltrim},
    {"Parse", CXFA_FM2JSContext::Parse},
    {"Replace", CXFA_FM2JSContext::Replace},
    {"Right", CXFA_FM2JSContext::Right},
    {"Rtrim", CXFA_FM2JSContext::Rtrim},
    {"Space", CXFA_FM2JSContext::Space},
    {"Str", CXFA_FM2JSContext::Str},
    {"Stuff", CXFA_FM2JSContext::Stuff},
    {"Substr", CXFA_FM2JSContext::Substr},
    {"Uuid", CXFA_FM2JSContext::Uuid},
    {"Upper", CXFA_FM2JSContext::Upper},
    {"WordNum", CXFA_FM2JSContext::WordNum},
    {"Get", CXFA_FM2JSContext::Get},
    {"Post", CXFA_FM2JSContext::Post},
    {"Put", CXFA_FM2JSContext::Put},
    {"positive_operator", CXFA_FM2JSContext::positive_operator},
    {"negative_operator", CXFA_FM2JSContext::negative_operator},
    {"logical_or_operator", CXFA_FM2JSContext::logical_or_operator},
    {"logical_and_operator", CXFA_FM2JSContext::logical_and_operator},
    {"logical_not_operator", CXFA_FM2JSContext::logical_not_operator},
    {"equality_operator", CXFA_FM2JSContext::equality_operator},
    {"notequality_operator", CXFA_FM2JSContext::notequality_operator},
    {"less_operator", CXFA_FM2JSContext::less_operator},
    {"lessequal_operator", CXFA_FM2JSContext::lessequal_operator},
    {"greater_operator", CXFA_FM2JSContext::greater_operator},
    {"greaterequal_operator", CXFA_FM2JSContext::greaterequal_operator},
    {"plus_operator", CXFA_FM2JSContext::plus_operator},
    {"minus_operator", CXFA_FM2JSContext::minus_operator},
    {"multiple_operator", CXFA_FM2JSContext::multiple_operator},
    {"divide_operator", CXFA_FM2JSContext::divide_operator},
    {"assign_value_operator", CXFA_FM2JSContext::assign_value_operator},
    {"dot_accessor", CXFA_FM2JSContext::dot_accessor},
    {"dotdot_accessor", CXFA_FM2JSContext::dotdot_accessor},
    {"concat_fm_object", CXFA_FM2JSContext::concat_fm_object},
    {"is_fm_object", CXFA_FM2JSContext::is_fm_object},
    {"is_fm_array", CXFA_FM2JSContext::is_fm_array},
    {"get_fm_value", CXFA_FM2JSContext::get_fm_value},
    {"get_fm_jsobj", CXFA_FM2JSContext::get_fm_jsobj},
    {"fm_var_filter", CXFA_FM2JSContext::fm_var_filter},
};

const FXJSE_CLASS_DESCRIPTOR formcalc_fm2js_descriptor = {
    "XFA_FM2JS_FormCalcClass",               // name
    nullptr,                                 // constructor
    nullptr,                                 // properties
    formcalc_fm2js_functions,                // methods
    0,                                       // number of properties
    FX_ArraySize(formcalc_fm2js_functions),  // number of methods
    nullptr,                                 // dynamic prop type
    nullptr,                                 // dynamic prop getter
    nullptr,                                 // dynamic prop setter
    nullptr,                                 // dynamic prop deleter
    nullptr,                                 // dynamic prop method call
};

const uint8_t g_sAltTable_Date[] = {
    255, 255, 255, 3,   9,   255, 255, 255, 255, 255, 255,
    255, 2,   255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 1,   255, 255, 255, 255, 255, 255, 255, 255,
};
static_assert(FX_ArraySize(g_sAltTable_Date) == L'a' - L'A' + 1,
              "Invalid g_sAltTable_Date size.");

const uint8_t g_sAltTable_Time[] = {
    14,  255, 255, 3,   9,   255, 255, 15,  255, 255, 255,
    255, 6,   255, 255, 255, 255, 255, 7,   255, 255, 255,
    255, 255, 1,   17,  255, 255, 255, 255, 255, 255, 255,
};
static_assert(FX_ArraySize(g_sAltTable_Time) == L'a' - L'A' + 1,
              "Invalid g_sAltTable_Time size.");

void AlternateDateTimeSymbols(CFX_WideString& wsPattern,
                              const CFX_WideString& wsAltSymbols,
                              const uint8_t* pAltTable) {
  int32_t nLength = wsPattern.GetLength();
  FX_BOOL bInConstRange = FALSE;
  FX_BOOL bEscape = FALSE;
  int32_t i = 0;
  while (i < nLength) {
    FX_WCHAR wc = wsPattern[i];
    if (wc == L'\'') {
      bInConstRange = !bInConstRange;
      if (bEscape) {
        i++;
      } else {
        wsPattern.Delete(i);
        nLength--;
      }
      bEscape = !bEscape;
      continue;
    }
    if (!bInConstRange && wc >= L'A' && wc <= L'a') {
      uint8_t nAlt = pAltTable[wc - L'A'];
      if (nAlt != 255)
        wsPattern.SetAt(i, wsAltSymbols[nAlt]);
    }
    i++;
    bEscape = FALSE;
  }
}

bool PatternStringType(const CFX_ByteStringC& szPattern,
                       uint32_t& patternType) {
  CFX_WideString wsPattern = CFX_WideString::FromUTF8(szPattern);
  if (FX_WSTRC(L"datetime") == wsPattern.Left(8)) {
    patternType = XFA_VT_DATETIME;
    return true;
  }
  if (FX_WSTRC(L"date") == wsPattern.Left(4)) {
    patternType = wsPattern.Find(L"time") > 0 ? XFA_VT_DATETIME : XFA_VT_DATE;
    return true;
  }
  if (FX_WSTRC(L"time") == wsPattern.Left(4)) {
    patternType = XFA_VT_TIME;
    return true;
  }
  if (FX_WSTRC(L"text") == wsPattern.Left(4)) {
    patternType = XFA_VT_TEXT;
    return true;
  }
  if (FX_WSTRC(L"num") == wsPattern.Left(3)) {
    if (FX_WSTRC(L"integer") == wsPattern.Mid(4, 7)) {
      patternType = XFA_VT_INTEGER;
    } else if (FX_WSTRC(L"decimal") == wsPattern.Mid(4, 7)) {
      patternType = XFA_VT_DECIMAL;
    } else if (FX_WSTRC(L"currency") == wsPattern.Mid(4, 8)) {
      patternType = XFA_VT_FLOAT;
    } else if (FX_WSTRC(L"percent") == wsPattern.Mid(4, 7)) {
      patternType = XFA_VT_FLOAT;
    } else {
      patternType = XFA_VT_FLOAT;
    }
    return true;
  }

  patternType = XFA_VT_NULL;
  wsPattern.MakeLower();
  const FX_WCHAR* pData = wsPattern.c_str();
  int32_t iLength = wsPattern.GetLength();
  int32_t iIndex = 0;
  FX_BOOL bSingleQuotation = FALSE;
  FX_WCHAR patternChar;
  while (iIndex < iLength) {
    patternChar = pData[iIndex];
    if (patternChar == 0x27) {
      bSingleQuotation = !bSingleQuotation;
    } else if (!bSingleQuotation &&
               (patternChar == 'y' || patternChar == 'j')) {
      patternType = XFA_VT_DATE;
      iIndex++;
      FX_WCHAR timePatternChar;
      while (iIndex < iLength) {
        timePatternChar = pData[iIndex];
        if (timePatternChar == 0x27) {
          bSingleQuotation = !bSingleQuotation;
        } else if (!bSingleQuotation && timePatternChar == 't') {
          patternType = XFA_VT_DATETIME;
          break;
        }
        iIndex++;
      }
      break;
    } else if (!bSingleQuotation &&
               (patternChar == 'h' || patternChar == 'k')) {
      patternType = XFA_VT_TIME;
      break;
    } else if (!bSingleQuotation &&
               (patternChar == 'a' || patternChar == 'x' ||
                patternChar == 'o' || patternChar == '0')) {
      patternType = XFA_VT_TEXT;
      if (patternChar == 'x' || patternChar == 'o' || patternChar == '0') {
        break;
      }
    } else if (!bSingleQuotation &&
               (patternChar == 'z' || patternChar == 's' ||
                patternChar == 'e' || patternChar == 'v' ||
                patternChar == '8' || patternChar == ',' ||
                patternChar == '.' || patternChar == '$')) {
      patternType = XFA_VT_FLOAT;
      if (patternChar == 'v' || patternChar == '8' || patternChar == '$') {
        break;
      }
    }
    iIndex++;
  }
  if (patternType == XFA_VT_NULL) {
    patternType = XFA_VT_TEXT | XFA_VT_FLOAT;
  }
  return false;
}

CXFA_FM2JSContext* ToJSContext(CFXJSE_Value* pValue, CFXJSE_Class* pClass) {
  return static_cast<CXFA_FM2JSContext*>(pValue->ToHostObject(pClass));
}

bool IsWhitespace(char c) {
  return c == 0x20 || c == 0x09 || c == 0x0B || c == 0x0C || c == 0x0A ||
         c == 0x0D;
}

}  // namespace

// static
void CXFA_FM2JSContext::Abs(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Abs");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE dValue = ValueToDouble(pThis, argOne.get());
  if (dValue < 0)
    dValue = -dValue;

  args.GetReturnValue()->SetDouble(dValue);
}

// static
void CXFA_FM2JSContext::Avg(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1) {
    args.GetReturnValue()->SetNull();
    return;
  }

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  uint32_t uCount = 0;
  FX_DOUBLE dSum = 0.0;
  for (int32_t i = 0; i < argc; i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (!argValue->IsArray()) {
      dSum += ValueToDouble(pThis, argValue.get());
      uCount++;
      continue;
    }

    std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
    argValue->GetObjectProperty("length", lengthValue.get());
    int32_t iLength = lengthValue->ToInteger();

    if (iLength > 2) {
      std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());

      std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          std::unique_ptr<CFXJSE_Value> defaultPropValue(
              new CFXJSE_Value(pIsolate));
          GetObjectDefaultValue(jsObjectValue.get(), defaultPropValue.get());
          if (defaultPropValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, defaultPropValue.get());
          uCount++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          std::unique_ptr<CFXJSE_Value> newPropertyValue(
              new CFXJSE_Value(pIsolate));
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, newPropertyValue.get());
          uCount++;
        }
      }
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dSum / uCount);
}

// static
void CXFA_FM2JSContext::Ceil(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Ceil");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetFloat(
      FXSYS_ceil(ValueToFloat(pThis, argValue.get())));
}

// static
void CXFA_FM2JSContext::Count(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  int32_t iCount = 0;
  for (int32_t i = 0; i < args.GetLength(); i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argValue->GetObjectProperty("length", lengthValue.get());

      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
        return;
      }

      std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      argValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (!newPropertyValue->IsNull())
            iCount++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), newPropertyValue.get());
          iCount += newPropertyValue->IsNull() ? 0 : 1;
        }
      }
    } else if (argValue->IsObject()) {
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (!newPropertyValue->IsNull())
        iCount++;
    } else {
      iCount++;
    }
  }
  args.GetReturnValue()->SetInteger(iCount);
}

// static
void CXFA_FM2JSContext::Floor(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Floor");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetFloat(
      FXSYS_floor(ValueToFloat(pThis, argValue.get())));
}

// static
void CXFA_FM2JSContext::Max(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  uint32_t uCount = 0;
  FX_DOUBLE dMaxValue = 0.0;
  for (int32_t i = 0; i < args.GetLength(); i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
        return;
      }

      std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      argValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          FX_DOUBLE dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          FX_DOUBLE dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
        }
      }
    } else if (argValue->IsObject()) {
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (newPropertyValue->IsNull())
        continue;

      uCount++;
      FX_DOUBLE dValue = ValueToDouble(pThis, newPropertyValue.get());
      dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
    } else {
      uCount++;
      FX_DOUBLE dValue = ValueToDouble(pThis, argValue.get());
      dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dMaxValue);
}

// static
void CXFA_FM2JSContext::Min(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  uint32_t uCount = 0;
  FX_DOUBLE dMinValue = 0.0;
  for (int32_t i = 0; i < args.GetLength(); i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
        return;
      }

      std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      argValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          FX_DOUBLE dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          FX_DOUBLE dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
        }
      }
    } else if (argValue->IsObject()) {
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (newPropertyValue->IsNull())
        continue;

      uCount++;
      FX_DOUBLE dValue = ValueToDouble(pThis, newPropertyValue.get());
      dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
    } else {
      uCount++;
      FX_DOUBLE dValue = ValueToDouble(pThis, argValue.get());
      dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dMinValue);
}

// static
void CXFA_FM2JSContext::Mod(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 2) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Mod");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> argTwo = args.GetValue(1);
  if (argOne->IsNull() || argTwo->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  bool argOneResult;
  FX_DOUBLE dDividend = ExtractDouble(pThis, argOne.get(), &argOneResult);
  bool argTwoResult;
  FX_DOUBLE dDivisor = ExtractDouble(pThis, argTwo.get(), &argTwoResult);
  if (!argOneResult || !argTwoResult) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  if (dDivisor == 0.0) {
    pContext->ThrowException(XFA_IDS_DIVIDE_ZERO);
    return;
  }

  args.GetReturnValue()->SetDouble(dDividend -
                                   dDivisor * (int32_t)(dDividend / dDivisor));
}

// static
void CXFA_FM2JSContext::Round(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Round");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  bool dValueRet;
  FX_DOUBLE dValue = ExtractDouble(pThis, argOne.get(), &dValueRet);
  if (!dValueRet) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  uint8_t uPrecision = 0;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argTwo = args.GetValue(1);
    if (argTwo->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }

    bool dPrecisionRet;
    FX_DOUBLE dPrecision = ExtractDouble(pThis, argTwo.get(), &dPrecisionRet);
    if (!dPrecisionRet) {
      pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
      return;
    }

    uPrecision =
        static_cast<uint8_t>(std::min(std::max(dPrecision, 0.0), 12.0));
  }

  CFX_Decimal decimalValue((FX_FLOAT)dValue, uPrecision);
  CFX_WideString wsValue = decimalValue;
  args.GetReturnValue()->SetString(wsValue.UTF8Encode().AsStringC());
}

// static
void CXFA_FM2JSContext::Sum(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  uint32_t uCount = 0;
  FX_DOUBLE dSum = 0.0;
  for (int32_t i = 0; i < argc; i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
        return;
      }

      std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, jsObjectValue.get());
          uCount++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, newPropertyValue.get());
          uCount++;
        }
      }
    } else if (argValue->IsObject()) {
      std::unique_ptr<CFXJSE_Value> newPropertyValue(
          new CFXJSE_Value(pIsolate));
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (newPropertyValue->IsNull())
        continue;

      dSum += ValueToDouble(pThis, argValue.get());
      uCount++;
    } else {
      dSum += ValueToDouble(pThis, argValue.get());
      uCount++;
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dSum);
}

// static
void CXFA_FM2JSContext::Date(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 0) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Date");
    return;
  }

  time_t currentTime;
  time(&currentTime);
  struct tm* pTmStruct = gmtime(&currentTime);

  CFX_ByteString bufferYear;
  CFX_ByteString bufferMon;
  CFX_ByteString bufferDay;
  bufferYear.Format("%d", pTmStruct->tm_year + 1900);
  bufferMon.Format("%02d", pTmStruct->tm_mon + 1);
  bufferDay.Format("%02d", pTmStruct->tm_mday);

  CFX_ByteString bufferCurrent = bufferYear + bufferMon + bufferDay;
  args.GetReturnValue()->SetInteger(DateString2Num(bufferCurrent.AsStringC()));
}

// static
void CXFA_FM2JSContext::Date2Num(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Date2Num");
    return;
  }

  std::unique_ptr<CFXJSE_Value> dateValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, dateValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString dateString;
  ValueToUTF8String(dateValue.get(), dateString);

  CFX_ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (ValueIsNull(pThis, formatValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(formatValue.get(), formatString);
  }

  CFX_ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (ValueIsNull(pThis, localValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(localValue.get(), localString);
  }

  CFX_ByteString szIsoDateString;
  if (!Local2IsoDate(pThis, dateString.AsStringC(), formatString.AsStringC(),
                     localString.AsStringC(), szIsoDateString)) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  args.GetReturnValue()->SetInteger(
      DateString2Num(szIsoDateString.AsStringC()));
}

// static
void CXFA_FM2JSContext::DateFmt(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Date2Num");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }

    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle < 0 || iStyle > 4)
      iStyle = 0;
  }

  CFX_ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(argLocal.get(), szLocal);
  }

  CFX_ByteString formatStr;
  GetStandardDateFormat(pThis, iStyle, szLocal.AsStringC(), formatStr);
  args.GetReturnValue()->SetString(formatStr.AsStringC());
}

// static
void CXFA_FM2JSContext::IsoDate2Num(CFXJSE_Value* pThis,
                                    const CFX_ByteStringC& szFuncName,
                                    CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"IsoDate2Num");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString szArgString;
  ValueToUTF8String(argOne.get(), szArgString);
  args.GetReturnValue()->SetInteger(DateString2Num(szArgString.AsStringC()));
}

// static
void CXFA_FM2JSContext::IsoTime2Num(CFXJSE_Value* pThis,
                                    const CFX_ByteStringC& szFuncName,
                                    CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD,
                             L"IsoTime2Num");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();

  CFX_ByteString szArgString;
  ValueToUTF8String(argOne.get(), szArgString);
  szArgString = szArgString.Mid(szArgString.Find('T', 0) + 1);
  if (szArgString.IsEmpty()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  CXFA_LocaleValue timeValue(
      XFA_VT_TIME, CFX_WideString::FromUTF8(szArgString.AsStringC()), pMgr);
  if (!timeValue.IsValid()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  CFX_Unitime uniTime = timeValue.GetTime();
  int32_t hour = uniTime.GetHour();
  int32_t min = uniTime.GetMinute();
  int32_t second = uniTime.GetSecond();
  int32_t milSecond = uniTime.GetMillisecond();

  FX_TIMEZONE tzLocale;
  pMgr->GetDefLocale()->GetTimeZone(tzLocale);

  // TODO(dsinclair): See if there is other time conversion code in pdfium and
  //   consolidate.
  int32_t mins = hour * 60 + min;
  mins -= (tzLocale.tzHour * 60);
  while (mins > 1440)
    mins -= 1440;
  while (mins < 0)
    mins += 1440;
  hour = mins / 60;
  min = mins % 60;

  args.GetReturnValue()->SetInteger(hour * 3600000 + min * 60000 +
                                    second * 1000 + milSecond + 1);
}

// static
void CXFA_FM2JSContext::LocalDateFmt(CFXJSE_Value* pThis,
                                     const CFX_ByteStringC& szFuncName,
                                     CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"LocalDateFmt");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle > 4 || iStyle < 0)
      iStyle = 0;
  }

  CFX_ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(argLocal.get(), szLocal);
  }

  CFX_ByteString formatStr;
  GetLocalDateFormat(pThis, iStyle, szLocal.AsStringC(), formatStr, FALSE);
  args.GetReturnValue()->SetString(formatStr.AsStringC());
}

// static
void CXFA_FM2JSContext::LocalTimeFmt(CFXJSE_Value* pThis,
                                     const CFX_ByteStringC& szFuncName,
                                     CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"LocalTimeFmt");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle > 4 || iStyle < 0)
      iStyle = 0;
  }

  CFX_ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(argLocal.get(), szLocal);
  }

  CFX_ByteString formatStr;
  GetLocalTimeFormat(pThis, iStyle, szLocal.AsStringC(), formatStr, FALSE);
  args.GetReturnValue()->SetString(formatStr.AsStringC());
}

// static
void CXFA_FM2JSContext::Num2Date(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Num2Date");
    return;
  }

  std::unique_ptr<CFXJSE_Value> dateValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, dateValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }
  int32_t dDate = (int32_t)ValueToFloat(pThis, dateValue.get());
  if (dDate < 1) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (ValueIsNull(pThis, formatValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(formatValue.get(), formatString);
  }

  CFX_ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (ValueIsNull(pThis, localValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(localValue.get(), localString);
  }

  int32_t iYear = 1900;
  int32_t iMonth = 1;
  int32_t iDay = 1;
  int32_t i = 0;
  while (dDate > 0) {
    if (iMonth == 2) {
      if ((!((iYear + i) % 4) && ((iYear + i) % 100)) || !((iYear + i) % 400)) {
        if (dDate > 29) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 29;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      } else {
        if (dDate > 28) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 28;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      }
    } else if (iMonth < 8) {
      if ((iMonth % 2 == 0)) {
        if (dDate > 30) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 30;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      } else {
        if (dDate > 31) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 31;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      }
    } else {
      if (iMonth % 2 != 0) {
        if (dDate > 30) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 30;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      } else {
        if (dDate > 31) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 31;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      }
    }
  }

  CFX_ByteString szIsoDateString;
  szIsoDateString.Format("%d%02d%02d", iYear + i, iMonth, iDay);
  CFX_ByteString szLocalDateString;
  IsoDate2Local(pThis, szIsoDateString.AsStringC(), formatString.AsStringC(),
                localString.AsStringC(), szLocalDateString);
  args.GetReturnValue()->SetString(szLocalDateString.AsStringC());
}

// static
void CXFA_FM2JSContext::Num2GMTime(CFXJSE_Value* pThis,
                                   const CFX_ByteStringC& szFuncName,
                                   CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Num2GMTime");
    return;
  }

  std::unique_ptr<CFXJSE_Value> timeValue = GetSimpleValue(pThis, args, 0);
  if (timeValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  int32_t iTime = (int32_t)ValueToFloat(pThis, timeValue.get());
  if (FXSYS_abs(iTime) < 1.0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (formatValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(formatValue.get(), formatString);
  }

  CFX_ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (localValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(localValue.get(), localString);
  }

  CFX_ByteString szGMTTimeString;
  Num2AllTime(pThis, iTime, formatString.AsStringC(), localString.AsStringC(),
              TRUE, szGMTTimeString);
  args.GetReturnValue()->SetString(szGMTTimeString.AsStringC());
}

// static
void CXFA_FM2JSContext::Num2Time(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Num2Time");
    return;
  }

  std::unique_ptr<CFXJSE_Value> timeValue = GetSimpleValue(pThis, args, 0);
  if (timeValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  FX_FLOAT fTime = ValueToFloat(pThis, timeValue.get());
  if (FXSYS_fabs(fTime) < 1.0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (formatValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(formatValue.get(), formatString);
  }

  CFX_ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (localValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(localValue.get(), localString);
  }

  CFX_ByteString szLocalTimeString;
  Num2AllTime(pThis, (int32_t)fTime, formatString.AsStringC(),
              localString.AsStringC(), FALSE, szLocalTimeString);
  args.GetReturnValue()->SetString(szLocalTimeString.AsStringC());
}

// static
void CXFA_FM2JSContext::Time(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 0) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Time");
    return;
  }

  time_t now;
  time(&now);

  struct tm* pGmt = gmtime(&now);
  args.GetReturnValue()->SetInteger(
      (pGmt->tm_hour * 3600 + pGmt->tm_min * 60 + pGmt->tm_sec) * 1000);
}

// static
void CXFA_FM2JSContext::Time2Num(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Time2Num");
    return;
  }

  CFX_ByteString timeString;
  std::unique_ptr<CFXJSE_Value> timeValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, timeValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }
  ValueToUTF8String(timeValue.get(), timeString);

  CFX_ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (ValueIsNull(pThis, formatValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(formatValue.get(), formatString);
  }

  CFX_ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (ValueIsNull(pThis, localValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(localValue.get(), localString);
  }

  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  IFX_Locale* pLocale = nullptr;
  if (localString.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);

    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pMgr->GetLocaleByName(
        CFX_WideString::FromUTF8(localString.AsStringC()));
  }

  CFX_WideString wsFormat;
  if (formatString.IsEmpty())
    pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Default, wsFormat);
  else
    wsFormat = CFX_WideString::FromUTF8(formatString.AsStringC());

  wsFormat = L"time{" + wsFormat + L"}";
  CXFA_LocaleValue localeValue(XFA_VT_TIME,
                               CFX_WideString::FromUTF8(timeString.AsStringC()),
                               wsFormat, pLocale, pMgr);
  if (!localeValue.IsValid()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  CFX_Unitime uniTime = localeValue.GetTime();
  int32_t hour = uniTime.GetHour();
  int32_t min = uniTime.GetMinute();
  int32_t second = uniTime.GetSecond();
  int32_t milSecond = uniTime.GetMillisecond();
  int32_t mins = hour * 60 + min;
  CXFA_TimeZoneProvider* pProvider = CXFA_TimeZoneProvider::Get();
  if (pProvider) {
    FX_TIMEZONE tz;
    pProvider->GetTimeZone(tz);
    mins -= (tz.tzHour * 60);
    while (mins > 1440)
      mins -= 1440;
    while (mins < 0)
      mins += 1440;

    hour = mins / 60;
    min = mins % 60;
  }
  args.GetReturnValue()->SetInteger(hour * 3600000 + min * 60000 +
                                    second * 1000 + milSecond + 1);
}

// static
void CXFA_FM2JSContext::TimeFmt(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"TimeFmt");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle > 4 || iStyle < 0)
      iStyle = 0;
  }

  CFX_ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(argLocal.get(), szLocal);
  }

  CFX_ByteString formatStr;
  GetStandardTimeFormat(pThis, iStyle, szLocal.AsStringC(), formatStr);
  args.GetReturnValue()->SetString(formatStr.AsStringC());
}

// static
FX_BOOL CXFA_FM2JSContext::IsIsoDateFormat(const FX_CHAR* pData,
                                           int32_t iLength,
                                           int32_t& iStyle,
                                           int32_t& iYear,
                                           int32_t& iMonth,
                                           int32_t& iDay) {
  iYear = 0;
  iMonth = 1;
  iDay = 1;

  if (iLength < 4)
    return FALSE;

  FX_CHAR strYear[5];
  strYear[4] = '\0';
  for (int32_t i = 0; i < 4; ++i) {
    if (pData[i] > '9' || pData[i] < '0')
      return FALSE;

    strYear[i] = pData[i];
  }
  iYear = FXSYS_atoi(strYear);
  iStyle = 0;
  if (iLength == 4)
    return TRUE;

  iStyle = pData[4] == '-' ? 1 : 0;

  FX_CHAR strTemp[3];
  strTemp[2] = '\0';
  int32_t iPosOff = iStyle == 0 ? 4 : 5;
  if ((pData[iPosOff] > '9' || pData[iPosOff] < '0') ||
      (pData[iPosOff + 1] > '9' || pData[iPosOff + 1] < '0'))
    return FALSE;

  strTemp[0] = pData[iPosOff];
  strTemp[1] = pData[iPosOff + 1];
  iMonth = FXSYS_atoi(strTemp);
  if (iMonth > 12 || iMonth < 1)
    return FALSE;

  if (iStyle == 0) {
    iPosOff += 2;
    if (iLength == 6)
      return TRUE;
  } else {
    iPosOff += 3;
    if (iLength == 7)
      return TRUE;
  }
  if ((pData[iPosOff] > '9' || pData[iPosOff] < '0') ||
      (pData[iPosOff + 1] > '9' || pData[iPosOff + 1] < '0'))
    return FALSE;

  strTemp[0] = pData[iPosOff];
  strTemp[1] = pData[iPosOff + 1];
  iDay = FXSYS_atoi(strTemp);
  if (iPosOff + 2 < iLength)
    return FALSE;

  if ((!(iYear % 4) && (iYear % 100)) || !(iYear % 400)) {
    if (iMonth == 2 && iDay > 29)
      return FALSE;
  } else {
    if (iMonth == 2 && iDay > 28)
      return FALSE;
  }
  if (iMonth != 2) {
    if (iMonth < 8) {
      if (iDay > (iMonth % 2 == 0 ? 30 : 31))
        return FALSE;
    } else if (iDay > (iMonth % 2 == 0 ? 31 : 30)) {
      return FALSE;
    }
  }
  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::IsIsoTimeFormat(const FX_CHAR* pData,
                                           int32_t iLength,
                                           int32_t& iHour,
                                           int32_t& iMinute,
                                           int32_t& iSecond,
                                           int32_t& iMilliSecond,
                                           int32_t& iZoneHour,
                                           int32_t& iZoneMinute) {
  iHour = 0;
  iMinute = 0;
  iSecond = 0;
  iMilliSecond = 0;
  iZoneHour = 0;
  iZoneMinute = 0;
  if (!pData)
    return FALSE;

  FX_CHAR strTemp[3];
  strTemp[2] = '\0';
  int32_t iZone = 0;
  int32_t i = 0;
  while (i < iLength) {
    if ((pData[i] > '9' || pData[i] < '0') && pData[i] != ':') {
      iZone = i;
      break;
    }
    ++i;
  }
  if (i == iLength)
    iZone = iLength;

  int32_t iPos = 0;
  int32_t iIndex = 0;
  while (iIndex < iZone) {
    if (iIndex >= iZone)
      break;

    if (pData[iIndex] > '9' || pData[iIndex] < '0')
      return FALSE;

    strTemp[0] = pData[iIndex];
    if (pData[iIndex + 1] > '9' || pData[iIndex + 1] < '0')
      return FALSE;

    strTemp[1] = pData[iIndex + 1];
    if (FXSYS_atoi(strTemp) > 60)
      return FALSE;

    if (pData[2] == ':') {
      if (iPos == 0) {
        iHour = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 1) {
        iMinute = FXSYS_atoi(strTemp);
        ++iPos;
      } else {
        iSecond = FXSYS_atoi(strTemp);
      }
      iIndex += 3;
    } else {
      if (iPos == 0) {
        iHour = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 1) {
        iMinute = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 2) {
        iSecond = FXSYS_atoi(strTemp);
        ++iPos;
      }
      iIndex += 2;
    }
  }
  if (pData[iIndex] == '.') {
    ++iIndex;
    FX_CHAR strSec[4];
    strSec[3] = '\0';
    if (pData[iIndex] > '9' || pData[iIndex] < '0')
      return FALSE;

    strSec[0] = pData[iIndex];
    if (pData[iIndex + 1] > '9' || pData[iIndex + 1] < '0')
      return FALSE;

    strSec[1] = pData[iIndex + 1];
    if (pData[iIndex + 2] > '9' || pData[iIndex + 2] < '0')
      return FALSE;

    strSec[2] = pData[iIndex + 2];
    iMilliSecond = FXSYS_atoi(strSec);
    if (iMilliSecond > 100) {
      iMilliSecond = 0;
      return FALSE;
    }
    iIndex += 3;
  }
  if (pData[iIndex] == 'z' || pData[iIndex] == 'Z')
    return TRUE;

  int32_t iSign = 1;
  if (pData[iIndex] == '+') {
    ++iIndex;
  } else if (pData[iIndex] == '-') {
    iSign = -1;
    ++iIndex;
  }
  iPos = 0;
  while (iIndex < iLength) {
    if (iIndex >= iLength)
      return FALSE;
    if (pData[iIndex] > '9' || pData[iIndex] < '0')
      return FALSE;

    strTemp[0] = pData[iIndex];
    if (pData[iIndex + 1] > '9' || pData[iIndex + 1] < '0')
      return FALSE;

    strTemp[1] = pData[iIndex + 1];
    if (FXSYS_atoi(strTemp) > 60)
      return FALSE;

    if (pData[2] == ':') {
      if (iPos == 0) {
        iZoneHour = FXSYS_atoi(strTemp);
      } else if (iPos == 1) {
        iZoneMinute = FXSYS_atoi(strTemp);
      }
      iIndex += 3;
    } else {
      if (!iPos) {
        iZoneHour = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 1) {
        iZoneMinute = FXSYS_atoi(strTemp);
        ++iPos;
      }
      iIndex += 2;
    }
  }
  if (iIndex < iLength)
    return FALSE;

  iZoneHour *= iSign;
  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::IsIsoDateTimeFormat(const FX_CHAR* pData,
                                               int32_t iLength,
                                               int32_t& iYear,
                                               int32_t& iMonth,
                                               int32_t& iDay,
                                               int32_t& iHour,
                                               int32_t& iMinute,
                                               int32_t& iSecond,
                                               int32_t& iMillionSecond,
                                               int32_t& iZoneHour,
                                               int32_t& iZoneMinute) {
  iYear = 0;
  iMonth = 0;
  iDay = 0;
  iHour = 0;
  iMinute = 0;
  iSecond = 0;
  if (!pData)
    return FALSE;

  int32_t iIndex = 0;
  while (pData[iIndex] != 'T' && pData[iIndex] != 't') {
    if (iIndex >= iLength)
      return FALSE;
    ++iIndex;
  }
  if (iIndex != 8 && iIndex != 10)
    return FALSE;

  int32_t iStyle = -1;
  if (!IsIsoDateFormat(pData, iIndex, iStyle, iYear, iMonth, iDay))
    return FALSE;
  if (pData[iIndex] != 'T' && pData[iIndex] != 't')
    return TRUE;

  ++iIndex;
  if (((iLength - iIndex > 13) && (iLength - iIndex < 6)) &&
      (iLength - iIndex != 15)) {
    return TRUE;
  }
  if (!IsIsoTimeFormat(pData + iIndex, iLength - iIndex, iHour, iMinute,
                       iSecond, iMillionSecond, iZoneHour, iZoneMinute)) {
    return FALSE;
  }

  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::Local2IsoDate(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szDate,
                                         const CFX_ByteStringC& szFormat,
                                         const CFX_ByteStringC& szLocale,
                                         CFX_ByteString& strIsoDate) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return FALSE;

  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  IFX_Locale* pLocale = nullptr;
  if (szLocale.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);

    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pMgr->GetLocaleByName(CFX_WideString::FromUTF8(szLocale));
  }
  if (!pLocale)
    return FALSE;

  CFX_WideString wsFormat;
  if (szFormat.IsEmpty())
    pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Default, wsFormat);
  else
    wsFormat = CFX_WideString::FromUTF8(szFormat);

  CXFA_LocaleValue widgetValue(XFA_VT_DATE, CFX_WideString::FromUTF8(szDate),
                               wsFormat, pLocale, pMgr);
  CFX_Unitime dt = widgetValue.GetDate();
  strIsoDate.Format("%4d-%02d-%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::Local2IsoTime(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szTime,
                                         const CFX_ByteStringC& szFormat,
                                         const CFX_ByteStringC& szLocale,
                                         CFX_ByteString& strIsoTime) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return FALSE;

  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  IFX_Locale* pLocale = nullptr;
  if (szLocale.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);

    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pMgr->GetLocaleByName(CFX_WideString::FromUTF8(szLocale));
  }
  if (!pLocale)
    return FALSE;

  CFX_WideString wsFormat;
  if (szFormat.IsEmpty())
    pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Default, wsFormat);
  else
    wsFormat = CFX_WideString::FromUTF8(szFormat);

  wsFormat = L"time{" + wsFormat + L"}";
  CXFA_LocaleValue widgetValue(XFA_VT_TIME, CFX_WideString::FromUTF8(szTime),
                               wsFormat, pLocale, pMgr);
  CFX_Unitime utime = widgetValue.GetTime();
  strIsoTime.Format("%02d:%02d:%02d.%03d", utime.GetHour(), utime.GetMinute(),
                    utime.GetSecond(), utime.GetMillisecond());
  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::IsoDate2Local(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szDate,
                                         const CFX_ByteStringC& szFormat,
                                         const CFX_ByteStringC& szLocale,
                                         CFX_ByteString& strLocalDate) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return FALSE;

  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  IFX_Locale* pLocale = nullptr;
  if (szLocale.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);
    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pMgr->GetLocaleByName(CFX_WideString::FromUTF8(szLocale));
  }
  if (!pLocale)
    return FALSE;

  CFX_WideString wsFormat;
  if (szFormat.IsEmpty())
    pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Default, wsFormat);
  else
    wsFormat = CFX_WideString::FromUTF8(szFormat);

  CXFA_LocaleValue widgetValue(XFA_VT_DATE, CFX_WideString::FromUTF8(szDate),
                               pMgr);
  CFX_WideString wsRet;
  widgetValue.FormatPatterns(wsRet, wsFormat, pLocale,
                             XFA_VALUEPICTURE_Display);
  strLocalDate = FX_UTF8Encode(wsRet.c_str(), wsRet.GetLength());
  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::IsoTime2Local(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szTime,
                                         const CFX_ByteStringC& szFormat,
                                         const CFX_ByteStringC& szLocale,
                                         CFX_ByteString& strLocalTime) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return FALSE;

  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  IFX_Locale* pLocale = nullptr;
  if (szLocale.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);
    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pMgr->GetLocaleByName(CFX_WideString::FromUTF8(szLocale));
  }
  if (!pLocale)
    return FALSE;

  CFX_WideString wsFormat;
  if (szFormat.IsEmpty())
    pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Default, wsFormat);
  else
    wsFormat = CFX_WideString::FromUTF8(szFormat);

  wsFormat = L"time{" + wsFormat + L"}";
  CXFA_LocaleValue widgetValue(XFA_VT_TIME, CFX_WideString::FromUTF8(szTime),
                               pMgr);
  CFX_WideString wsRet;
  widgetValue.FormatPatterns(wsRet, wsFormat, pLocale,
                             XFA_VALUEPICTURE_Display);
  strLocalTime = FX_UTF8Encode(wsRet.c_str(), wsRet.GetLength());
  return TRUE;
}

// static
FX_BOOL CXFA_FM2JSContext::GetGMTTime(CFXJSE_Value* pThis,
                                      const CFX_ByteStringC& szTime,
                                      const CFX_ByteStringC& szFormat,
                                      const CFX_ByteStringC& szLocale,
                                      CFX_ByteString& strGMTTime) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return FALSE;

  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  IFX_Locale* pLocale = nullptr;
  if (szLocale.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);
    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pMgr->GetLocaleByName(CFX_WideString::FromUTF8(szLocale));
  }
  if (!pLocale)
    return FALSE;

  CFX_WideString wsFormat;
  if (szFormat.IsEmpty())
    pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Default, wsFormat);
  else
    wsFormat = CFX_WideString::FromUTF8(szFormat);

  wsFormat = L"time{" + wsFormat + L"}";
  CXFA_LocaleValue widgetValue(XFA_VT_TIME, CFX_WideString::FromUTF8(szTime),
                               pMgr);
  CFX_WideString wsRet;
  widgetValue.FormatPatterns(wsRet, wsFormat, pLocale,
                             XFA_VALUEPICTURE_Display);
  strGMTTime = FX_UTF8Encode(wsRet.c_str(), wsRet.GetLength());
  return TRUE;
}

// static
int32_t CXFA_FM2JSContext::DateString2Num(const CFX_ByteStringC& szDateString) {
  int32_t iLength = szDateString.GetLength();
  int32_t iYear = 0;
  int32_t iMonth = 0;
  int32_t iDay = 0;
  if (iLength <= 10) {
    int32_t iStyle = -1;
    if (!IsIsoDateFormat(szDateString.c_str(), iLength, iStyle, iYear, iMonth,
                         iDay)) {
      return 0;
    }
  } else {
    int32_t iHour = 0;
    int32_t iMinute = 0;
    int32_t iSecond = 0;
    int32_t iMilliSecond = 0;
    int32_t iZoneHour = 0;
    int32_t iZoneMinute = 0;
    if (!IsIsoDateTimeFormat(szDateString.c_str(), iLength, iYear, iMonth, iDay,
                             iHour, iMinute, iSecond, iMilliSecond, iZoneHour,
                             iZoneMinute)) {
      return 0;
    }
  }

  FX_FLOAT dDays = 0;
  int32_t i = 1;
  if (iYear < 1900)
    return 0;

  while (iYear - i >= 1900) {
    dDays +=
        ((!((iYear - i) % 4) && ((iYear - i) % 100)) || !((iYear - i) % 400))
            ? 366
            : 365;
    ++i;
  }
  i = 1;
  while (i < iMonth) {
    if (i == 2)
      dDays += ((!(iYear % 4) && (iYear % 100)) || !(iYear % 400)) ? 29 : 28;
    else if (i <= 7)
      dDays += (i % 2 == 0) ? 30 : 31;
    else
      dDays += (i % 2 == 0) ? 31 : 30;

    ++i;
  }
  i = 0;
  while (iDay - i > 0) {
    dDays += 1;
    ++i;
  }
  return (int32_t)dDays;
}

// static
void CXFA_FM2JSContext::GetLocalDateFormat(CFXJSE_Value* pThis,
                                           int32_t iStyle,
                                           const CFX_ByteStringC& szLocalStr,
                                           CFX_ByteString& strFormat,
                                           FX_BOOL bStandard) {
  FX_LOCALEDATETIMESUBCATEGORY strStyle;
  switch (iStyle) {
    case 1:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Short;
      break;
    case 3:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Long;
      break;
    case 4:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Full;
      break;
    case 0:
    case 2:
    default:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Medium;
      break;
  }
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return;

  IFX_Locale* pLocale = nullptr;
  if (szLocalStr.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);

    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pDoc->GetLocalMgr()->GetLocaleByName(
        CFX_WideString::FromUTF8(szLocalStr));
  }
  if (!pLocale)
    return;

  CFX_WideString strRet;
  pLocale->GetDatePattern(strStyle, strRet);
  if (!bStandard) {
    CFX_WideString wsSymbols;
    pLocale->GetDateTimeSymbols(wsSymbols);
    AlternateDateTimeSymbols(strRet, wsSymbols, g_sAltTable_Date);
  }
  strFormat = FX_UTF8Encode(strRet.c_str(), strRet.GetLength());
}

// static
void CXFA_FM2JSContext::GetLocalTimeFormat(CFXJSE_Value* pThis,
                                           int32_t iStyle,
                                           const CFX_ByteStringC& szLocalStr,
                                           CFX_ByteString& strFormat,
                                           FX_BOOL bStandard) {
  FX_LOCALEDATETIMESUBCATEGORY strStyle;
  switch (iStyle) {
    case 1:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Short;
      break;
    case 3:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Long;
      break;
    case 4:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Full;
      break;
    case 0:
    case 2:
    default:
      strStyle = FX_LOCALEDATETIMESUBCATEGORY_Medium;
      break;
  }
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return;

  IFX_Locale* pLocale = nullptr;
  if (szLocalStr.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);

    CXFA_WidgetData widgetData(pThisNode);
    pLocale = widgetData.GetLocal();
  } else {
    pLocale = pDoc->GetLocalMgr()->GetLocaleByName(
        CFX_WideString::FromUTF8(szLocalStr));
  }
  if (!pLocale)
    return;

  CFX_WideString strRet;
  pLocale->GetTimePattern(strStyle, strRet);
  if (!bStandard) {
    CFX_WideString wsSymbols;
    pLocale->GetDateTimeSymbols(wsSymbols);
    AlternateDateTimeSymbols(strRet, wsSymbols, g_sAltTable_Time);
  }
  strFormat = FX_UTF8Encode(strRet.c_str(), strRet.GetLength());
}

// static
void CXFA_FM2JSContext::GetStandardDateFormat(CFXJSE_Value* pThis,
                                              int32_t iStyle,
                                              const CFX_ByteStringC& szLocalStr,
                                              CFX_ByteString& strFormat) {
  GetLocalDateFormat(pThis, iStyle, szLocalStr, strFormat, TRUE);
}

// static
void CXFA_FM2JSContext::GetStandardTimeFormat(CFXJSE_Value* pThis,
                                              int32_t iStyle,
                                              const CFX_ByteStringC& szLocalStr,
                                              CFX_ByteString& strFormat) {
  GetLocalTimeFormat(pThis, iStyle, szLocalStr, strFormat, TRUE);
}

// static
void CXFA_FM2JSContext::Num2AllTime(CFXJSE_Value* pThis,
                                    int32_t iTime,
                                    const CFX_ByteStringC& szFormat,
                                    const CFX_ByteStringC& szLocale,
                                    FX_BOOL bGM,
                                    CFX_ByteString& strTime) {
  int32_t iHour = 0;
  int32_t iMin = 0;
  int32_t iSec = 0;
  iHour = static_cast<int>(iTime) / 3600000;
  iMin = (static_cast<int>(iTime) - iHour * 3600000) / 60000;
  iSec = (static_cast<int>(iTime) - iHour * 3600000 - iMin * 60000) / 1000;

  if (!bGM) {
    int32_t iZoneHour = 0;
    int32_t iZoneMin = 0;
    int32_t iZoneSec = 0;
    GetLocalTimeZone(iZoneHour, iZoneMin, iZoneSec);
    iHour += iZoneHour;
    iMin += iZoneMin;
    iSec += iZoneSec;
  }

  FX_BOOL iRet = FALSE;
  CFX_ByteString strIsoTime;
  strIsoTime.Format("%02d:%02d:%02d", iHour, iMin, iSec);
  if (bGM) {
    iRet =
        GetGMTTime(pThis, strIsoTime.AsStringC(), szFormat, szLocale, strTime);
  } else {
    iRet = IsoTime2Local(pThis, strIsoTime.AsStringC(), szFormat, szLocale,
                         strTime);
  }
  if (!iRet)
    strTime = "";
}

// static
void CXFA_FM2JSContext::GetLocalTimeZone(int32_t& iHour,
                                         int32_t& iMin,
                                         int32_t& iSec) {
  time_t now;
  time(&now);

  struct tm* pGmt = gmtime(&now);
  struct tm* pLocal = localtime(&now);
  iHour = pLocal->tm_hour - pGmt->tm_hour;
  iMin = pLocal->tm_min - pGmt->tm_min;
  iSec = pLocal->tm_sec - pGmt->tm_sec;
}

// static
void CXFA_FM2JSContext::Apr(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Apr");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE nPrincipal = ValueToDouble(pThis, argOne.get());
  FX_DOUBLE nPayment = ValueToDouble(pThis, argTwo.get());
  FX_DOUBLE nPeriods = ValueToDouble(pThis, argThree.get());
  if (nPrincipal <= 0 || nPayment <= 0 || nPeriods <= 0) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  FX_DOUBLE r =
      2 * (nPeriods * nPayment - nPrincipal) / (nPeriods * nPrincipal);
  FX_DOUBLE nTemp = 1;
  for (int32_t i = 0; i < nPeriods; ++i)
    nTemp *= (1 + r);

  FX_DOUBLE nRet = r * nTemp / (nTemp - 1) - nPayment / nPrincipal;
  while (fabs(nRet) > kFinancialPrecision) {
    FX_DOUBLE nDerivative =
        ((nTemp + r * nPeriods * (nTemp / (1 + r))) * (nTemp - 1) -
         (r * nTemp * nPeriods * (nTemp / (1 + r)))) /
        ((nTemp - 1) * (nTemp - 1));
    if (nDerivative == 0) {
      args.GetReturnValue()->SetNull();
      return;
    }

    r = r - nRet / nDerivative;
    nTemp = 1;
    for (int32_t i = 0; i < nPeriods; ++i) {
      nTemp *= (1 + r);
    }
    nRet = r * nTemp / (nTemp - 1) - nPayment / nPrincipal;
  }
  args.GetReturnValue()->SetDouble(r * 12);
}

// static
void CXFA_FM2JSContext::CTerm(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"CTerm");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT nRate = ValueToFloat(pThis, argOne.get());
  FX_FLOAT nFutureValue = ValueToFloat(pThis, argTwo.get());
  FX_FLOAT nInitAmount = ValueToFloat(pThis, argThree.get());
  if ((nRate <= 0) || (nFutureValue <= 0) || (nInitAmount <= 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  args.GetReturnValue()->SetFloat(
      FXSYS_log((FX_FLOAT)(nFutureValue / nInitAmount)) /
      FXSYS_log((FX_FLOAT)(1 + nRate)));
}

// static
void CXFA_FM2JSContext::FV(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"FV");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE nAmount = ValueToDouble(pThis, argOne.get());
  FX_DOUBLE nRate = ValueToDouble(pThis, argTwo.get());
  FX_DOUBLE nPeriod = ValueToDouble(pThis, argThree.get());
  if ((nRate < 0) || (nPeriod <= 0) || (nAmount <= 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  FX_DOUBLE dResult = 0;
  if (nRate) {
    FX_DOUBLE nTemp = 1;
    for (int i = 0; i < nPeriod; ++i) {
      nTemp *= 1 + nRate;
    }
    dResult = nAmount * (nTemp - 1) / nRate;
  } else {
    dResult = nAmount * nPeriod;
  }

  args.GetReturnValue()->SetDouble(dResult);
}

// static
void CXFA_FM2JSContext::IPmt(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 5) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"IPmt");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  std::unique_ptr<CFXJSE_Value> argFour = GetSimpleValue(pThis, args, 3);
  std::unique_ptr<CFXJSE_Value> argFive = GetSimpleValue(pThis, args, 4);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get()) || ValueIsNull(pThis, argFour.get()) ||
      ValueIsNull(pThis, argFive.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT nPrincipalAmount = ValueToFloat(pThis, argOne.get());
  FX_FLOAT nRate = ValueToFloat(pThis, argTwo.get());
  FX_FLOAT nPayment = ValueToFloat(pThis, argThree.get());
  FX_FLOAT nFirstMonth = ValueToFloat(pThis, argFour.get());
  FX_FLOAT nNumberOfMonths = ValueToFloat(pThis, argFive.get());
  if ((nPrincipalAmount <= 0) || (nRate <= 0) || (nPayment <= 0) ||
      (nFirstMonth < 0) || (nNumberOfMonths < 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  FX_FLOAT nRateOfMonth = nRate / 12;
  int32_t iNums = (int32_t)(
      (FXSYS_log10((FX_FLOAT)(nPayment / nPrincipalAmount)) -
       FXSYS_log10((FX_FLOAT)(nPayment / nPrincipalAmount - nRateOfMonth))) /
      FXSYS_log10((FX_FLOAT)(1 + nRateOfMonth)));
  int32_t iEnd = std::min((int32_t)(nFirstMonth + nNumberOfMonths - 1), iNums);

  if (nPayment < nPrincipalAmount * nRateOfMonth) {
    args.GetReturnValue()->SetFloat(0);
    return;
  }

  int32_t i = 0;
  for (i = 0; i < nFirstMonth - 1; ++i)
    nPrincipalAmount -= nPayment - nPrincipalAmount * nRateOfMonth;

  FX_FLOAT nSum = 0;
  for (; i < iEnd; ++i) {
    nSum += nPrincipalAmount * nRateOfMonth;
    nPrincipalAmount -= nPayment - nPrincipalAmount * nRateOfMonth;
  }
  args.GetReturnValue()->SetFloat(nSum);
}

// static
void CXFA_FM2JSContext::NPV(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  int32_t argc = args.GetLength();
  if (argc < 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"NPV");
    return;
  }

  std::vector<std::unique_ptr<CFXJSE_Value>> argValues;
  for (int32_t i = 0; i < argc; i++) {
    argValues.push_back(GetSimpleValue(pThis, args, i));
    if (ValueIsNull(pThis, argValues[i].get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
  }

  FX_DOUBLE nRate = ValueToDouble(pThis, argValues[0].get());
  if (nRate <= 0) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  std::vector<FX_DOUBLE> data(argc - 1);
  for (int32_t i = 1; i < argc; i++)
    data.push_back(ValueToDouble(pThis, argValues[i].get()));

  FX_DOUBLE nSum = 0;
  int32_t iIndex = 0;
  for (int32_t i = 0; i < argc - 1; i++) {
    FX_DOUBLE nTemp = 1;
    for (int32_t j = 0; j <= i; j++)
      nTemp *= 1 + nRate;

    FX_DOUBLE nNum = data[iIndex++];
    nSum += nNum / nTemp;
  }
  args.GetReturnValue()->SetDouble(nSum);
}

// static
void CXFA_FM2JSContext::Pmt(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Pmt");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT nPrincipal = ValueToFloat(pThis, argOne.get());
  FX_FLOAT nRate = ValueToFloat(pThis, argTwo.get());
  FX_FLOAT nPeriods = ValueToFloat(pThis, argThree.get());
  if ((nPrincipal <= 0) || (nRate <= 0) || (nPeriods <= 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  FX_FLOAT nTmp = 1 + nRate;
  FX_FLOAT nSum = nTmp;
  for (int32_t i = 0; i < nPeriods - 1; ++i)
    nSum *= nTmp;

  args.GetReturnValue()->SetFloat((nPrincipal * nRate * nSum) / (nSum - 1));
}

// static
void CXFA_FM2JSContext::PPmt(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 5) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"PPmt");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  std::unique_ptr<CFXJSE_Value> argFour = GetSimpleValue(pThis, args, 3);
  std::unique_ptr<CFXJSE_Value> argFive = GetSimpleValue(pThis, args, 4);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get()) || ValueIsNull(pThis, argFour.get()) ||
      ValueIsNull(pThis, argFive.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT nPrincipalAmount = ValueToFloat(pThis, argOne.get());
  FX_FLOAT nRate = ValueToFloat(pThis, argTwo.get());
  FX_FLOAT nPayment = ValueToFloat(pThis, argThree.get());
  FX_FLOAT nFirstMonth = ValueToFloat(pThis, argFour.get());
  FX_FLOAT nNumberOfMonths = ValueToFloat(pThis, argFive.get());
  if ((nPrincipalAmount <= 0) || (nRate <= 0) || (nPayment <= 0) ||
      (nFirstMonth < 0) || (nNumberOfMonths < 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  FX_FLOAT nRateOfMonth = nRate / 12;
  int32_t iNums = (int32_t)(
      (FXSYS_log10((FX_FLOAT)(nPayment / nPrincipalAmount)) -
       FXSYS_log10((FX_FLOAT)(nPayment / nPrincipalAmount - nRateOfMonth))) /
      FXSYS_log10((FX_FLOAT)(1 + nRateOfMonth)));
  int32_t iEnd = std::min((int32_t)(nFirstMonth + nNumberOfMonths - 1), iNums);
  if (nPayment < nPrincipalAmount * nRateOfMonth) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  int32_t i = 0;
  for (i = 0; i < nFirstMonth - 1; ++i)
    nPrincipalAmount -= nPayment - nPrincipalAmount * nRateOfMonth;

  FX_FLOAT nTemp = 0;
  FX_FLOAT nSum = 0;
  for (; i < iEnd; ++i) {
    nTemp = nPayment - nPrincipalAmount * nRateOfMonth;
    nSum += nTemp;
    nPrincipalAmount -= nTemp;
  }
  args.GetReturnValue()->SetFloat(nSum);
}

// static
void CXFA_FM2JSContext::PV(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"PV");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE nAmount = ValueToDouble(pThis, argOne.get());
  FX_DOUBLE nRate = ValueToDouble(pThis, argTwo.get());
  FX_DOUBLE nPeriod = ValueToDouble(pThis, argThree.get());
  if ((nAmount <= 0) || (nRate < 0) || (nPeriod <= 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  FX_DOUBLE nTemp = 1;
  for (int32_t i = 0; i < nPeriod; ++i)
    nTemp *= 1 + nRate;

  nTemp = 1 / nTemp;
  args.GetReturnValue()->SetDouble(nAmount * ((1 - nTemp) / nRate));
}

// static
void CXFA_FM2JSContext::Rate(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Rate");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT nFuture = ValueToFloat(pThis, argOne.get());
  FX_FLOAT nPresent = ValueToFloat(pThis, argTwo.get());
  FX_FLOAT nTotalNumber = ValueToFloat(pThis, argThree.get());
  if ((nFuture <= 0) || (nPresent < 0) || (nTotalNumber <= 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  args.GetReturnValue()->SetFloat(
      FXSYS_pow((FX_FLOAT)(nFuture / nPresent), (FX_FLOAT)(1 / nTotalNumber)) -
      1);
}

// static
void CXFA_FM2JSContext::Term(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Term");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT nMount = ValueToFloat(pThis, argOne.get());
  FX_FLOAT nRate = ValueToFloat(pThis, argTwo.get());
  FX_FLOAT nFuture = ValueToFloat(pThis, argThree.get());
  if ((nMount <= 0) || (nRate <= 0) || (nFuture <= 0)) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  args.GetReturnValue()->SetFloat(
      FXSYS_log((FX_FLOAT)(nFuture / nMount * nRate) + 1) /
      FXSYS_log((FX_FLOAT)(1 + nRate)));
}

// static
void CXFA_FM2JSContext::Choose(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  int32_t argc = args.GetLength();
  if (argc < 2) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Choose");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  int32_t iIndex = (int32_t)ValueToFloat(pThis, argOne.get());
  if (iIndex < 1) {
    args.GetReturnValue()->SetString("");
    return;
  }

  FX_BOOL bFound = FALSE;
  FX_BOOL bStopCounterFlags = FALSE;
  int32_t iArgIndex = 1;
  int32_t iValueIndex = 0;
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  while (!bFound && !bStopCounterFlags && (iArgIndex < argc)) {
    std::unique_ptr<CFXJSE_Value> argIndexValue = args.GetValue(iArgIndex);
    if (argIndexValue->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argIndexValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength > 3)
        bStopCounterFlags = TRUE;

      iValueIndex += (iLength - 2);
      if (iValueIndex >= iIndex) {
        std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
        std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
        std::unique_ptr<CFXJSE_Value> newPropertyValue(
            new CFXJSE_Value(pIsolate));
        argIndexValue->GetObjectPropertyByIdx(1, propertyValue.get());
        argIndexValue->GetObjectPropertyByIdx(
            (iLength - 1) - (iValueIndex - iIndex), jsObjectValue.get());
        if (propertyValue->IsNull()) {
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
        } else {
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), newPropertyValue.get());
        }
        CFX_ByteString bsChoosed;
        ValueToUTF8String(newPropertyValue.get(), bsChoosed);
        args.GetReturnValue()->SetString(bsChoosed.AsStringC());
        bFound = TRUE;
      }
    } else {
      iValueIndex++;
      if (iValueIndex == iIndex) {
        CFX_ByteString bsChoosed;
        ValueToUTF8String(argIndexValue.get(), bsChoosed);
        args.GetReturnValue()->SetString(bsChoosed.AsStringC());
        bFound = TRUE;
      }
    }
    iArgIndex++;
  }
  if (!bFound)
    args.GetReturnValue()->SetString("");
}

// static
void CXFA_FM2JSContext::Exists(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Exists");
    return;
  }
  args.GetReturnValue()->SetInteger(args.GetValue(0)->IsObject());
}

// static
void CXFA_FM2JSContext::HasValue(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"HasValue");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (!argOne->IsString()) {
    args.GetReturnValue()->SetInteger(argOne->IsNumber() ||
                                      argOne->IsBoolean());
    return;
  }

  CFX_ByteString valueStr = argOne->ToString();
  valueStr.TrimLeft();
  args.GetReturnValue()->SetInteger(!valueStr.IsEmpty());
}

// static
void CXFA_FM2JSContext::Oneof(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  if (args.GetLength() < 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Oneof");
    return;
  }

  FX_BOOL bFlags = FALSE;
  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  CFXJSE_Value** parametersValue = nullptr;
  int32_t iCount = 0;
  unfoldArgs(pThis, args, parametersValue, iCount, 1);
  for (int32_t i = 0; i < iCount; i++) {
    if (simpleValueCompare(pThis, argOne.get(), parametersValue[i])) {
      bFlags = TRUE;
      break;
    }
  }
  for (int32_t i = 0; i < iCount; i++)
    delete parametersValue[i];
  FX_Free(parametersValue);

  args.GetReturnValue()->SetInteger(bFlags);
}

// static
void CXFA_FM2JSContext::Within(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Within");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetUndefined();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argLow = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argHigh = GetSimpleValue(pThis, args, 2);
  if (argOne->IsNumber()) {
    FX_FLOAT oneNumber = ValueToFloat(pThis, argOne.get());
    FX_FLOAT lowNumber = ValueToFloat(pThis, argLow.get());
    FX_FLOAT heightNumber = ValueToFloat(pThis, argHigh.get());
    args.GetReturnValue()->SetInteger((oneNumber >= lowNumber) &&
                                      (oneNumber <= heightNumber));
    return;
  }

  CFX_ByteString oneString;
  CFX_ByteString lowString;
  CFX_ByteString heightString;
  ValueToUTF8String(argOne.get(), oneString);
  ValueToUTF8String(argLow.get(), lowString);
  ValueToUTF8String(argHigh.get(), heightString);
  args.GetReturnValue()->SetInteger(
      (oneString.Compare(lowString.AsStringC()) >= 0) &&
      (oneString.Compare(heightString.AsStringC()) <= 0));
}

// static
void CXFA_FM2JSContext::If(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args) {
  if (args.GetLength() != 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"If");
    return;
  }

  args.GetReturnValue()->Assign(GetSimpleValue(pThis, args, 0)->ToBoolean()
                                    ? GetSimpleValue(pThis, args, 1).get()
                                    : GetSimpleValue(pThis, args, 2).get());
}

// static
void CXFA_FM2JSContext::Eval(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Eval");
    return;
  }

  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> scriptValue = GetSimpleValue(pThis, args, 0);
  CFX_ByteString utf8ScriptString;
  ValueToUTF8String(scriptValue.get(), utf8ScriptString);
  if (utf8ScriptString.IsEmpty()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_WideTextBuf wsJavaScriptBuf;
  CFX_WideString wsError;
  CXFA_FM2JSContext::Translate(
      CFX_WideString::FromUTF8(utf8ScriptString.AsStringC()).AsStringC(),
      wsJavaScriptBuf, wsError);
  std::unique_ptr<CFXJSE_Context> pNewContext(
      CFXJSE_Context::Create(pIsolate, nullptr, nullptr));

  std::unique_ptr<CFXJSE_Value> returnValue(new CFXJSE_Value(pIsolate));
  CFX_WideString javaScript(wsJavaScriptBuf.AsStringC());
  pNewContext->ExecuteScript(
      FX_UTF8Encode(javaScript.c_str(), javaScript.GetLength()).c_str(),
      returnValue.get());

  args.GetReturnValue()->Assign(returnValue.get());
}

// static
void CXFA_FM2JSContext::Ref(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Ref");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (!argOne->IsArray() && !argOne->IsObject() && !argOne->IsBoolean() &&
      !argOne->IsString() && !argOne->IsNull() && !argOne->IsNumber()) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  if (argOne->IsBoolean() || argOne->IsString() || argOne->IsNumber()) {
    args.GetReturnValue()->Assign(argOne.get());
    return;
  }

  CFXJSE_Value* rgValues[3];
  for (int32_t i = 0; i < 3; i++)
    rgValues[i] = new CFXJSE_Value(pIsolate);

  int intVal = 3;
  if (argOne->IsNull()) {
    // TODO(dsinclair): Why is this 4 when the others are all 3?
    intVal = 4;
    rgValues[2]->SetNull();
  } else if (argOne->IsArray()) {
#ifndef NDEBUG
    std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
    argOne->GetObjectProperty("length", lengthValue.get());
    ASSERT(lengthValue->ToInteger() >= 3);
#endif

    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    argOne->GetObjectPropertyByIdx(1, propertyValue.get());
    argOne->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (!propertyValue->IsNull() || jsObjectValue->IsNull()) {
      for (int32_t i = 0; i < 3; i++)
        delete rgValues[i];

      pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
      return;
    }

    rgValues[2]->Assign(jsObjectValue.get());
  } else if (argOne->IsObject()) {
    rgValues[2]->Assign(argOne.get());
  }

  rgValues[0]->SetInteger(intVal);
  rgValues[1]->SetNull();
  args.GetReturnValue()->SetArray(3, rgValues);

  for (int32_t i = 0; i < 3; i++)
    delete rgValues[i];
}

// static
void CXFA_FM2JSContext::UnitType(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"UnitType");
    return;
  }

  std::unique_ptr<CFXJSE_Value> unitspanValue = GetSimpleValue(pThis, args, 0);
  if (unitspanValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString unitspanString;
  ValueToUTF8String(unitspanValue.get(), unitspanString);
  if (unitspanString.IsEmpty()) {
    args.GetReturnValue()->SetString("in");
    return;
  }

  enum XFA_FM2JS_VALUETYPE_ParserStatus {
    VALUETYPE_START,
    VALUETYPE_HAVEINVALIDCHAR,
    VALUETYPE_HAVEDIGIT,
    VALUETYPE_HAVEDIGITWHITE,
    VALUETYPE_ISCM,
    VALUETYPE_ISMM,
    VALUETYPE_ISPT,
    VALUETYPE_ISMP,
    VALUETYPE_ISIN,
  };
  unitspanString.MakeLower();
  CFX_WideString wsTypeString =
      CFX_WideString::FromUTF8(unitspanString.AsStringC());
  const FX_WCHAR* pData = wsTypeString.c_str();
  int32_t u = 0;
  int32_t uLen = wsTypeString.GetLength();
  while (IsWhitespace(pData[u]))
    u++;

  XFA_FM2JS_VALUETYPE_ParserStatus eParserStatus = VALUETYPE_START;
  FX_WCHAR typeChar;
  // TODO(dsinclair): Cleanup this parser, figure out what the various checks
  //    are for.
  while (u < uLen) {
    typeChar = pData[u];
    if (IsWhitespace(typeChar)) {
      if (eParserStatus != VALUETYPE_HAVEDIGIT &&
          eParserStatus != VALUETYPE_HAVEDIGITWHITE) {
        eParserStatus = VALUETYPE_ISIN;
        break;
      }
      eParserStatus = VALUETYPE_HAVEDIGITWHITE;
    } else if ((typeChar >= '0' && typeChar <= '9') || typeChar == '-' ||
               typeChar == '.') {
      if (eParserStatus == VALUETYPE_HAVEDIGITWHITE) {
        eParserStatus = VALUETYPE_ISIN;
        break;
      }
      eParserStatus = VALUETYPE_HAVEDIGIT;
    } else if ((typeChar == 'c' || typeChar == 'p') && (u + 1 < uLen)) {
      FX_WCHAR nextChar = pData[u + 1];
      if ((eParserStatus == VALUETYPE_START ||
           eParserStatus == VALUETYPE_HAVEDIGIT ||
           eParserStatus == VALUETYPE_HAVEDIGITWHITE) &&
          (nextChar > '9' || nextChar < '0') && nextChar != '.' &&
          nextChar != '-') {
        eParserStatus = (typeChar == 'c') ? VALUETYPE_ISCM : VALUETYPE_ISPT;
        break;
      }
      eParserStatus = VALUETYPE_HAVEINVALIDCHAR;
    } else if (typeChar == 'm' && (u + 1 < uLen)) {
      FX_WCHAR nextChar = pData[u + 1];
      if ((eParserStatus == VALUETYPE_START ||
           eParserStatus == VALUETYPE_HAVEDIGIT ||
           eParserStatus == VALUETYPE_HAVEDIGITWHITE) &&
          (nextChar > '9' || nextChar < '0') && nextChar != '.' &&
          nextChar != '-') {
        eParserStatus = VALUETYPE_ISMM;
        if (nextChar == 'p' || ((u + 5 < uLen) && pData[u + 1] == 'i' &&
                                pData[u + 2] == 'l' && pData[u + 3] == 'l' &&
                                pData[u + 4] == 'i' && pData[u + 5] == 'p')) {
          eParserStatus = VALUETYPE_ISMP;
        }
        break;
      }
    } else {
      eParserStatus = VALUETYPE_HAVEINVALIDCHAR;
    }
    u++;
  }
  switch (eParserStatus) {
    case VALUETYPE_ISCM:
      args.GetReturnValue()->SetString("cm");
      break;
    case VALUETYPE_ISMM:
      args.GetReturnValue()->SetString("mm");
      break;
    case VALUETYPE_ISPT:
      args.GetReturnValue()->SetString("pt");
      break;
    case VALUETYPE_ISMP:
      args.GetReturnValue()->SetString("mp");
      break;
    default:
      args.GetReturnValue()->SetString("in");
      break;
  }
}

// static
void CXFA_FM2JSContext::UnitValue(CFXJSE_Value* pThis,
                                  const CFX_ByteStringC& szFuncName,
                                  CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"UnitValue");
    return;
  }

  std::unique_ptr<CFXJSE_Value> unitspanValue = GetSimpleValue(pThis, args, 0);
  if (unitspanValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString unitspanString;
  ValueToUTF8String(unitspanValue.get(), unitspanString);
  const FX_CHAR* pData = unitspanString.c_str();
  if (!pData) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  int32_t u = 0;
  while (IsWhitespace(pData[u]))
    ++u;

  while (u < unitspanString.GetLength()) {
    if ((pData[u] > '9' || pData[u] < '0') && pData[u] != '.' &&
        pData[u] != '-') {
      break;
    }
    ++u;
  }

  FX_CHAR* pTemp = nullptr;
  FX_DOUBLE dFirstNumber = strtod(pData, &pTemp);
  while (IsWhitespace(pData[u]))
    ++u;

  int32_t uLen = unitspanString.GetLength();
  CFX_ByteString strFirstUnit;
  while (u < uLen) {
    if (pData[u] == ' ')
      break;

    strFirstUnit += pData[u];
    ++u;
  }
  strFirstUnit.MakeLower();

  CFX_ByteString strUnit;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> unitValue = GetSimpleValue(pThis, args, 1);
    CFX_ByteString unitTempString;
    ValueToUTF8String(unitValue.get(), unitTempString);
    const FX_CHAR* pChar = unitTempString.c_str();
    int32_t uVal = 0;
    while (IsWhitespace(pChar[uVal]))
      ++uVal;

    while (uVal < unitTempString.GetLength()) {
      if ((pChar[uVal] > '9' || pChar[uVal] < '0') && pChar[uVal] != '.') {
        break;
      }
      ++uVal;
    }
    while (IsWhitespace(pChar[uVal]))
      ++uVal;

    int32_t uValLen = unitTempString.GetLength();
    while (uVal < uValLen) {
      if (pChar[uVal] == ' ')
        break;

      strUnit += pChar[uVal];
      ++uVal;
    }
    strUnit.MakeLower();
  } else {
    strUnit = strFirstUnit;
  }

  FX_DOUBLE dResult = 0;
  if (strFirstUnit == "in" || strFirstUnit == "inches") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber * 25.4;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber * 2.54;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 72;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber / 72000;
    else
      dResult = dFirstNumber;
  } else if (strFirstUnit == "mm" || strFirstUnit == "millimeters") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber / 10;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 25.4 / 72;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber / 25.4 / 72000;
    else
      dResult = dFirstNumber / 25.4;
  } else if (strFirstUnit == "cm" || strFirstUnit == "centimeters") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber * 10;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 2.54 / 72;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber / 2.54 / 72000;
    else
      dResult = dFirstNumber / 2.54;
  } else if (strFirstUnit == "pt" || strFirstUnit == "points") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber / 72 * 25.4;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber / 72 * 2.54;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber * 1000;
    else
      dResult = dFirstNumber / 72;
  } else if (strFirstUnit == "mp" || strFirstUnit == "millipoints") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber / 72000 * 25.4;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber / 72000 * 2.54;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 1000;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber;
    else
      dResult = dFirstNumber / 72000;
  }
  args.GetReturnValue()->SetDouble(dResult);
}

// static
void CXFA_FM2JSContext::At(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"At");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString stringTwo;
  ValueToUTF8String(argTwo.get(), stringTwo);
  if (stringTwo.IsEmpty()) {
    args.GetReturnValue()->SetInteger(1);
    return;
  }

  CFX_ByteString stringOne;
  ValueToUTF8String(argOne.get(), stringOne);
  FX_STRSIZE iPosition = stringOne.Find(stringTwo.AsStringC());
  args.GetReturnValue()->SetInteger(iPosition + 1);
}

// static
void CXFA_FM2JSContext::Concat(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Concat");
    return;
  }

  CFX_ByteString resultString;
  FX_BOOL bAllNull = TRUE;
  for (int32_t i = 0; i < argc; i++) {
    std::unique_ptr<CFXJSE_Value> value = GetSimpleValue(pThis, args, i);
    if (ValueIsNull(pThis, value.get()))
      continue;

    bAllNull = FALSE;

    CFX_ByteString valueStr;
    ValueToUTF8String(value.get(), valueStr);
    resultString += valueStr;
  }

  if (bAllNull) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetString(resultString.AsStringC());
}

// static
void CXFA_FM2JSContext::Decode(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Decode");
    return;
  }

  if (argc == 1) {
    std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
    if (ValueIsNull(pThis, argOne.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }

    CFX_ByteString toDecodeString;
    ValueToUTF8String(argOne.get(), toDecodeString);
    CFX_ByteTextBuf resultBuf;
    DecodeURL(toDecodeString.AsStringC(), resultBuf);
    args.GetReturnValue()->SetString(resultBuf.AsStringC());
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString toDecodeString;
  ValueToUTF8String(argOne.get(), toDecodeString);

  CFX_ByteString identifyString;
  ValueToUTF8String(argTwo.get(), identifyString);

  CFX_ByteTextBuf resultBuf;
  if (identifyString.EqualNoCase("html"))
    DecodeHTML(toDecodeString.AsStringC(), resultBuf);
  else if (identifyString.EqualNoCase("xml"))
    DecodeXML(toDecodeString.AsStringC(), resultBuf);
  else
    DecodeURL(toDecodeString.AsStringC(), resultBuf);

  args.GetReturnValue()->SetString(resultBuf.AsStringC());
}

// static
void CXFA_FM2JSContext::DecodeURL(const CFX_ByteStringC& szURLString,
                                  CFX_ByteTextBuf& szResultString) {
  CFX_WideString wsURLString = CFX_WideString::FromUTF8(szURLString);
  const FX_WCHAR* pData = wsURLString.c_str();
  int32_t i = 0;
  CFX_WideTextBuf wsResultBuf;
  while (i < wsURLString.GetLength()) {
    FX_WCHAR ch = pData[i];
    if ('%' != ch) {
      wsResultBuf.AppendChar(ch);
      ++i;
      continue;
    }

    FX_WCHAR chTemp = 0;
    int32_t iCount = 0;
    while (iCount < 2) {
      ++i;
      ch = pData[i];
      if (ch <= '9' && ch >= '0') {
        // TODO(dsinclair): Premultiply and add rather then scale.
        chTemp += (ch - '0') * (!iCount ? 16 : 1);
      } else if (ch <= 'F' && ch >= 'A') {
        chTemp += (ch - 'A' + 10) * (!iCount ? 16 : 1);
      } else if (ch <= 'f' && ch >= 'a') {
        chTemp += (ch - 'a' + 10) * (!iCount ? 16 : 1);
      } else {
        wsResultBuf.Clear();
        return;
      }
      ++iCount;
    }
    wsResultBuf.AppendChar(chTemp);
    ++i;
  }
  wsResultBuf.AppendChar(0);

  szResultString.Clear();
  szResultString << FX_UTF8Encode(wsResultBuf.GetBuffer(),
                                  wsResultBuf.GetLength())
                        .AsStringC();
}

// static
void CXFA_FM2JSContext::DecodeHTML(const CFX_ByteStringC& szHTMLString,
                                   CFX_ByteTextBuf& szResultString) {
  CFX_WideString wsHTMLString = CFX_WideString::FromUTF8(szHTMLString);
  FX_WCHAR strString[9];
  int32_t iStrIndex = 0;
  int32_t iLen = wsHTMLString.GetLength();
  int32_t i = 0;
  int32_t iCode = 0;
  const FX_WCHAR* pData = wsHTMLString.c_str();
  CFX_WideTextBuf wsResultBuf;
  while (i < iLen) {
    FX_WCHAR ch = pData[i];
    if (ch != '&') {
      wsResultBuf.AppendChar(ch);
      ++i;
      continue;
    }

    ++i;
    ch = pData[i];
    if (ch == '#') {
      ++i;
      ch = pData[i];
      if (ch != 'x' && ch != 'X') {
        wsResultBuf.Clear();
        return;
      }

      ++i;
      ch = pData[i];
      if ((ch >= '0' && ch <= '9') || (ch <= 'f' && ch >= 'a') ||
          (ch <= 'F' && ch >= 'A')) {
        while (ch != ';' && i < iLen) {
          if (ch >= '0' && ch <= '9') {
            iCode += ch - '0';
          } else if (ch <= 'f' && ch >= 'a') {
            iCode += ch - 'a' + 10;
          } else if (ch <= 'F' && ch >= 'A') {
            iCode += ch - 'A' + 10;
          } else {
            wsResultBuf.Clear();
            return;
          }
          ++i;
          // TODO(dsinclair): Postmultiply seems wrong, start at zero
          //   and pre-multiply then can remove the post divide.
          iCode *= 16;
          ch = pData[i];
        }
        iCode /= 16;
      }
    } else {
      while (ch != ';' && i < iLen) {
        strString[iStrIndex++] = ch;
        ++i;
        ch = pData[i];
      }
      strString[iStrIndex] = 0;
    }
    uint32_t iData = 0;
    if (HTMLSTR2Code(strString, iData)) {
      wsResultBuf.AppendChar((FX_WCHAR)iData);
    } else {
      wsResultBuf.AppendChar(iCode);
    }
    iStrIndex = 0;
    strString[iStrIndex] = 0;
    ++i;
  }
  wsResultBuf.AppendChar(0);

  szResultString.Clear();
  szResultString << FX_UTF8Encode(wsResultBuf.GetBuffer(),
                                  wsResultBuf.GetLength())
                        .AsStringC();
}

// static
void CXFA_FM2JSContext::DecodeXML(const CFX_ByteStringC& szXMLString,
                                  CFX_ByteTextBuf& szResultString) {
  CFX_WideString wsXMLString = CFX_WideString::FromUTF8(szXMLString);
  FX_WCHAR strString[9];
  int32_t iStrIndex = 0;
  int32_t iLen = wsXMLString.GetLength();
  int32_t i = 0;
  int32_t iCode = 0;
  FX_WCHAR ch = 0;
  const FX_WCHAR* pData = wsXMLString.c_str();
  CFX_WideTextBuf wsXMLBuf;
  while (i < iLen) {
    ch = pData[i];
    if (ch != '&') {
      wsXMLBuf.AppendChar(ch);
      ++i;
      continue;
    }

    // TODO(dsinclair): This is very similar to DecodeHTML, can they be
    //   combined?
    ++i;
    ch = pData[i];
    if (ch == '#') {
      ++i;
      ch = pData[i];
      if (ch != 'x' && ch != 'X') {
        wsXMLBuf.Clear();
        return;
      }

      ++i;
      ch = pData[i];
      if ((ch >= '0' && ch <= '9') || (ch <= 'f' && ch >= 'a') ||
          (ch <= 'F' && ch >= 'A')) {
        while (ch != ';') {
          if (ch >= '0' && ch <= '9') {
            iCode += ch - '0';
          } else if (ch <= 'f' && ch >= 'a') {
            iCode += ch - 'a' + 10;
          } else if (ch <= 'F' && ch >= 'A') {
            iCode += ch - 'A' + 10;
          } else {
            wsXMLBuf.Clear();
            return;
          }
          ++i;
          iCode *= 16;
          ch = pData[i];
        }
        iCode /= 16;
      }
    } else {
      while (ch != ';' && i < iLen) {
        strString[iStrIndex++] = ch;
        ++i;
        ch = pData[i];
      }
      strString[iStrIndex] = 0;
    }

    const FX_WCHAR* const strName[] = {L"quot", L"amp", L"apos", L"lt", L"gt"};
    int32_t iIndex = 0;
    while (iIndex < 5) {
      if (FXSYS_memcmp(strString, strName[iIndex],
                       FXSYS_wcslen(strName[iIndex])) == 0) {
        break;
      }
      ++iIndex;
    }
    switch (iIndex) {
      case 0:
        wsXMLBuf.AppendChar('"');
        break;
      case 1:
        wsXMLBuf.AppendChar('&');
        break;
      case 2:
        wsXMLBuf.AppendChar('\'');
        break;
      case 3:
        wsXMLBuf.AppendChar('<');
        break;
      case 4:
        wsXMLBuf.AppendChar('>');
        break;
      default:
        wsXMLBuf.AppendChar(iCode);
        break;
    }
    iStrIndex = 0;
    strString[iStrIndex] = 0;
    ++i;
    iCode = 0;
  }
  wsXMLBuf.AppendChar(0);

  szResultString.Clear();
  szResultString << FX_UTF8Encode(wsXMLBuf.GetBuffer(), wsXMLBuf.GetLength())
                        .AsStringC();
}

// static
void CXFA_FM2JSContext::Encode(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Encode");
    return;
  }

  if (argc == 1) {
    std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
    if (ValueIsNull(pThis, argOne.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }

    CFX_ByteString toEncodeString;
    ValueToUTF8String(argOne.get(), toEncodeString);
    CFX_ByteTextBuf resultBuf;
    EncodeURL(toEncodeString.AsStringC(), resultBuf);
    args.GetReturnValue()->SetString(resultBuf.AsStringC());
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString toEncodeString;
  ValueToUTF8String(argOne.get(), toEncodeString);
  CFX_ByteString identifyString;
  ValueToUTF8String(argTwo.get(), identifyString);
  CFX_ByteTextBuf resultBuf;
  if (identifyString.EqualNoCase("html"))
    EncodeHTML(toEncodeString.AsStringC(), resultBuf);
  else if (identifyString.EqualNoCase("xml"))
    EncodeXML(toEncodeString.AsStringC(), resultBuf);
  else
    EncodeURL(toEncodeString.AsStringC(), resultBuf);

  args.GetReturnValue()->SetString(resultBuf.AsStringC());
}

// static
void CXFA_FM2JSContext::EncodeURL(const CFX_ByteStringC& szURLString,
                                  CFX_ByteTextBuf& szResultBuf) {
  CFX_WideString wsURLString = CFX_WideString::FromUTF8(szURLString);
  CFX_WideTextBuf wsResultBuf;
  FX_WCHAR strEncode[4];
  strEncode[0] = '%';
  strEncode[3] = 0;
  FX_WCHAR strUnsafe[] = {' ', '<',  '>', '"', '#', '%', '{', '}',
                          '|', '\\', '^', '~', '[', ']', '`'};
  FX_WCHAR strReserved[] = {';', '/', '?', ':', '@', '=', '&'};
  FX_WCHAR strSpecial[] = {'$', '-', '+', '!', '*', '\'', '(', ')', ','};
  const FX_WCHAR* strCode = L"0123456789abcdef";
  for (int32_t u = 0; u < wsURLString.GetLength(); ++u) {
    FX_WCHAR ch = wsURLString.GetAt(u);
    int32_t i = 0;
    int32_t iCount = FX_ArraySize(strUnsafe);
    while (i < iCount) {
      if (ch == strUnsafe[i]) {
        int32_t iIndex = ch / 16;
        strEncode[1] = strCode[iIndex];
        strEncode[2] = strCode[ch - iIndex * 16];
        wsResultBuf << FX_WSTRC(strEncode);
        break;
      }
      ++i;
    }
    if (i < iCount)
      continue;

    i = 0;
    iCount = FX_ArraySize(strReserved);
    while (i < iCount) {
      if (ch == strReserved[i]) {
        int32_t iIndex = ch / 16;
        strEncode[1] = strCode[iIndex];
        strEncode[2] = strCode[ch - iIndex * 16];
        wsResultBuf << FX_WSTRC(strEncode);
        break;
      }
      ++i;
    }
    if (i < iCount)
      continue;

    i = 0;
    iCount = FX_ArraySize(strSpecial);
    while (i < iCount) {
      if (ch == strSpecial[i]) {
        wsResultBuf.AppendChar(ch);
        break;
      }
      ++i;
    }
    if (i < iCount)
      continue;

    if ((ch >= 0x80 && ch <= 0xff) || ch <= 0x1f || ch == 0x7f) {
      int32_t iIndex = ch / 16;
      strEncode[1] = strCode[iIndex];
      strEncode[2] = strCode[ch - iIndex * 16];
      wsResultBuf << FX_WSTRC(strEncode);
    } else if (ch >= 0x20 && ch <= 0x7e) {
      wsResultBuf.AppendChar(ch);
    } else {
      const FX_WCHAR iRadix = 16;
      CFX_WideString strTmp;
      while (ch >= iRadix) {
        FX_WCHAR tmp = strCode[ch % iRadix];
        ch /= iRadix;
        strTmp += tmp;
      }
      strTmp += strCode[ch];
      int32_t iLen = strTmp.GetLength();
      if (iLen < 2)
        break;

      int32_t iIndex = 0;
      if (iLen % 2 != 0) {
        strEncode[1] = '0';
        strEncode[2] = strTmp.GetAt(iLen - 1);
        iIndex = iLen - 2;
      } else {
        strEncode[1] = strTmp.GetAt(iLen - 1);
        strEncode[2] = strTmp.GetAt(iLen - 2);
        iIndex = iLen - 3;
      }
      wsResultBuf << FX_WSTRC(strEncode);
      while (iIndex > 0) {
        strEncode[1] = strTmp.GetAt(iIndex);
        strEncode[2] = strTmp.GetAt(iIndex - 1);
        iIndex -= 2;
        wsResultBuf << FX_WSTRC(strEncode);
      }
    }
  }
  wsResultBuf.AppendChar(0);
  szResultBuf.Clear();

  szResultBuf << FX_UTF8Encode(wsResultBuf.GetBuffer(), wsResultBuf.GetLength())
                     .AsStringC();
}

// static
void CXFA_FM2JSContext::EncodeHTML(const CFX_ByteStringC& szHTMLString,
                                   CFX_ByteTextBuf& szResultBuf) {
  CFX_ByteString str = szHTMLString.c_str();
  CFX_WideString wsHTMLString = CFX_WideString::FromUTF8(str.AsStringC());
  const FX_WCHAR* strCode = L"0123456789abcdef";
  FX_WCHAR strEncode[9];
  strEncode[0] = '&';
  strEncode[1] = '#';
  strEncode[2] = 'x';
  strEncode[5] = ';';
  strEncode[6] = 0;
  strEncode[7] = ';';
  strEncode[8] = 0;
  CFX_WideTextBuf wsResultBuf;
  int32_t iLen = wsHTMLString.GetLength();
  int32_t i = 0;
  const FX_WCHAR* pData = wsHTMLString.c_str();
  while (i < iLen) {
    uint32_t ch = pData[i];
    CFX_WideString htmlReserve;
    if (HTMLCode2STR(ch, htmlReserve)) {
      wsResultBuf.AppendChar(L'&');
      wsResultBuf << htmlReserve;
      wsResultBuf.AppendChar(L';');
    } else if (ch >= 32 && ch <= 126) {
      wsResultBuf.AppendChar((FX_WCHAR)ch);
    } else if (ch < 256) {
      int32_t iIndex = ch / 16;
      strEncode[3] = strCode[iIndex];
      strEncode[4] = strCode[ch - iIndex * 16];
      strEncode[5] = ';';
      strEncode[6] = 0;
      wsResultBuf << FX_WSTRC(strEncode);
    } else {
      int32_t iBigByte = ch / 256;
      int32_t iLittleByte = ch % 256;
      strEncode[3] = strCode[iBigByte / 16];
      strEncode[4] = strCode[iBigByte % 16];
      strEncode[5] = strCode[iLittleByte / 16];
      strEncode[6] = strCode[iLittleByte % 16];
      wsResultBuf << FX_WSTRC(strEncode);
    }
    ++i;
  }
  wsResultBuf.AppendChar(0);
  szResultBuf.Clear();

  szResultBuf << FX_UTF8Encode(wsResultBuf.GetBuffer(), wsResultBuf.GetLength())
                     .AsStringC();
}

// static
void CXFA_FM2JSContext::EncodeXML(const CFX_ByteStringC& szXMLString,
                                  CFX_ByteTextBuf& szResultBuf) {
  CFX_WideString wsXMLString = CFX_WideString::FromUTF8(szXMLString);
  CFX_WideTextBuf wsResultBuf;
  FX_WCHAR strEncode[9];
  strEncode[0] = '&';
  strEncode[1] = '#';
  strEncode[2] = 'x';
  strEncode[5] = ';';
  strEncode[6] = 0;
  strEncode[7] = ';';
  strEncode[8] = 0;
  const FX_WCHAR* strCode = L"0123456789abcdef";
  const FX_WCHAR* pData = wsXMLString.c_str();
  for (int32_t u = 0; u < wsXMLString.GetLength(); ++u) {
    FX_WCHAR ch = pData[u];
    switch (ch) {
      case '"':
        wsResultBuf.AppendChar('&');
        wsResultBuf << CFX_WideStringC(L"quot");
        wsResultBuf.AppendChar(';');
        break;
      case '&':
        wsResultBuf.AppendChar('&');
        wsResultBuf << CFX_WideStringC(L"amp");
        wsResultBuf.AppendChar(';');
        break;
      case '\'':
        wsResultBuf.AppendChar('&');
        wsResultBuf << CFX_WideStringC(L"apos");
        wsResultBuf.AppendChar(';');
        break;
      case '<':
        wsResultBuf.AppendChar('&');
        wsResultBuf << CFX_WideStringC(L"lt");
        wsResultBuf.AppendChar(';');
        break;
      case '>':
        wsResultBuf.AppendChar('&');
        wsResultBuf << CFX_WideStringC(L"gt");
        wsResultBuf.AppendChar(';');
        break;
      default: {
        if (ch >= 32 && ch <= 126) {
          wsResultBuf.AppendChar(ch);
        } else if (ch < 256) {
          int32_t iIndex = ch / 16;
          strEncode[3] = strCode[iIndex];
          strEncode[4] = strCode[ch - iIndex * 16];
          strEncode[5] = ';';
          strEncode[6] = 0;
          wsResultBuf << FX_WSTRC(strEncode);
        } else {
          int32_t iBigByte = ch / 256;
          int32_t iLittleByte = ch % 256;
          strEncode[3] = strCode[iBigByte / 16];
          strEncode[4] = strCode[iBigByte % 16];
          strEncode[5] = strCode[iLittleByte / 16];
          strEncode[6] = strCode[iLittleByte % 16];
          wsResultBuf << FX_WSTRC(strEncode);
        }
        break;
      }
    }
  }
  wsResultBuf.AppendChar(0);
  szResultBuf.Clear();

  szResultBuf << FX_UTF8Encode(wsResultBuf.GetBuffer(), wsResultBuf.GetLength())
                     .AsStringC();
}

// static
FX_BOOL CXFA_FM2JSContext::HTMLSTR2Code(const CFX_WideStringC& pData,
                                        uint32_t& iCode) {
  uint32_t uHash = FX_HashCode_GetW(pData, false);
  int32_t iStart = 0;
  int32_t iEnd = FX_ArraySize(reservesForDecode) - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    XFA_FMHtmlHashedReserveCode htmlhashedreservecode = reservesForDecode[iMid];
    if (uHash == htmlhashedreservecode.m_uHash) {
      iCode = htmlhashedreservecode.m_uCode;
      return TRUE;
    }

    if (uHash < htmlhashedreservecode.m_uHash)
      iEnd = iMid - 1;
    else
      iStart = iMid + 1;
  } while (iStart <= iEnd);
  return FALSE;
}

// static
FX_BOOL CXFA_FM2JSContext::HTMLCode2STR(uint32_t iCode,
                                        CFX_WideString& wsHTMLReserve) {
  int32_t iStart = 0;
  int32_t iEnd = FX_ArraySize(reservesForEncode) - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    XFA_FMHtmlReserveCode htmlreservecode = reservesForEncode[iMid];
    if (iCode == htmlreservecode.m_uCode) {
      wsHTMLReserve = htmlreservecode.m_htmlReserve;
      return TRUE;
    }

    if (iCode < htmlreservecode.m_uCode)
      iEnd = iMid - 1;
    else
      iStart = iMid + 1;
  } while (iStart <= iEnd);
  return FALSE;
}

// static
void CXFA_FM2JSContext::Format(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() < 2) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Format");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  CFX_ByteString szPattern;
  ValueToUTF8String(argOne.get(), szPattern);

  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  CFX_ByteString szValue;
  ValueToUTF8String(argTwo.get(), szValue);

  CXFA_Document* pDoc = pContext->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
  ASSERT(pThisNode);

  CXFA_WidgetData widgetData(pThisNode);
  IFX_Locale* pLocale = widgetData.GetLocal();
  uint32_t patternType;
  CFX_WideString wsPattern = CFX_WideString::FromUTF8(szPattern.AsStringC());
  CFX_WideString wsValue = CFX_WideString::FromUTF8(szValue.AsStringC());
  if (!PatternStringType(szPattern.AsStringC(), patternType)) {
    switch (patternType) {
      case XFA_VT_DATETIME: {
        FX_STRSIZE iTChar = wsPattern.Find(L'T');
        CFX_WideString wsDatePattern(L"date{");
        wsDatePattern += wsPattern.Left(iTChar) + L"} ";

        CFX_WideString wsTimePattern(L"time{");
        wsTimePattern += wsPattern.Mid(iTChar + 1) + L"}";
        wsPattern = wsDatePattern + wsTimePattern;
      } break;
      case XFA_VT_DATE: {
        wsPattern = L"date{" + wsPattern + L"}";
      } break;
      case XFA_VT_TIME: {
        wsPattern = L"time{" + wsPattern + L"}";
      } break;
      case XFA_VT_TEXT: {
        wsPattern = L"text{" + wsPattern + L"}";
      } break;
      case XFA_VT_FLOAT: {
        wsPattern = L"num{" + wsPattern + L"}";
      } break;
      default: {
        CFX_WideString wsTestPattern;
        wsTestPattern = L"num{" + wsPattern + L"}";
        CXFA_LocaleValue tempLocaleValue(XFA_VT_FLOAT, wsValue, wsTestPattern,
                                         pLocale, pMgr);
        if (tempLocaleValue.IsValid()) {
          wsPattern = wsTestPattern;
          patternType = XFA_VT_FLOAT;
        } else {
          wsTestPattern = L"text{" + wsPattern + L"}";
          wsPattern = wsTestPattern;
          patternType = XFA_VT_TEXT;
        }
      } break;
    }
  }
  CXFA_LocaleValue localeValue(patternType, wsValue, wsPattern, pLocale, pMgr);
  CFX_WideString wsRet;
  if (!localeValue.FormatPatterns(wsRet, wsPattern, pLocale,
                                  XFA_VALUEPICTURE_Display)) {
    args.GetReturnValue()->SetString("");
    return;
  }

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(wsRet.c_str(), wsRet.GetLength()).AsStringC());
}

// static
void CXFA_FM2JSContext::Left(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Left");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if ((ValueIsNull(pThis, argOne.get())) ||
      (ValueIsNull(pThis, argTwo.get()))) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString sourceString;
  ValueToUTF8String(argOne.get(), sourceString);
  int32_t count = std::max(0, ValueToInteger(pThis, argTwo.get()));
  args.GetReturnValue()->SetString(sourceString.Left(count).AsStringC());
}

// static
void CXFA_FM2JSContext::Len(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Len");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString sourceString;
  ValueToUTF8String(argOne.get(), sourceString);
  args.GetReturnValue()->SetInteger(sourceString.GetLength());
}

// static
void CXFA_FM2JSContext::Lower(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Lower");
    return;
  }

  CFX_ByteString argString;
  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ValueToUTF8String(argOne.get(), argString);
  CFX_WideTextBuf lowStringBuf;
  CFX_WideString wsArgString = CFX_WideString::FromUTF8(argString.AsStringC());
  const FX_WCHAR* pData = wsArgString.c_str();
  int32_t i = 0;
  while (i < argString.GetLength()) {
    int32_t ch = pData[i];
    if ((ch >= 0x41 && ch <= 0x5A) || (ch >= 0xC0 && ch <= 0xDE))
      ch += 32;
    else if (ch == 0x100 || ch == 0x102 || ch == 0x104)
      ch += 1;

    lowStringBuf.AppendChar(ch);
    ++i;
  }
  lowStringBuf.AppendChar(0);

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(lowStringBuf.GetBuffer(), lowStringBuf.GetLength())
          .AsStringC());
}

// static
void CXFA_FM2JSContext::Ltrim(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Ltrim");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString sourceString;
  ValueToUTF8String(argOne.get(), sourceString);
  sourceString.TrimLeft();
  args.GetReturnValue()->SetString(sourceString.AsStringC());
}

// static
void CXFA_FM2JSContext::Parse(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 2) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Parse");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString szPattern;
  ValueToUTF8String(argOne.get(), szPattern);
  CFX_ByteString szValue;
  ValueToUTF8String(argTwo.get(), szValue);

  CXFA_Document* pDoc = pContext->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocalMgr();
  CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
  ASSERT(pThisNode);

  CXFA_WidgetData widgetData(pThisNode);
  IFX_Locale* pLocale = widgetData.GetLocal();
  CFX_WideString wsPattern = CFX_WideString::FromUTF8(szPattern.AsStringC());
  CFX_WideString wsValue = CFX_WideString::FromUTF8(szValue.AsStringC());
  uint32_t patternType;
  if (PatternStringType(szPattern.AsStringC(), patternType)) {
    CXFA_LocaleValue localeValue(patternType, wsValue, wsPattern, pLocale,
                                 pMgr);
    if (!localeValue.IsValid()) {
      args.GetReturnValue()->SetString("");
      return;
    }
    args.GetReturnValue()->SetString(
        FX_UTF8Encode(localeValue.GetValue()).AsStringC());
    return;
  }

  switch (patternType) {
    case XFA_VT_DATETIME: {
      FX_STRSIZE iTChar = wsPattern.Find(L'T');
      CFX_WideString wsDatePattern(L"date{" + wsPattern.Left(iTChar) + L"} ");
      CFX_WideString wsTimePattern(L"time{" + wsPattern.Mid(iTChar + 1) + L"}");
      wsPattern = wsDatePattern + wsTimePattern;
      CXFA_LocaleValue localeValue(patternType, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          FX_UTF8Encode(localeValue.GetValue()).AsStringC());
      return;
    }
    case XFA_VT_DATE: {
      wsPattern = L"date{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(patternType, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          FX_UTF8Encode(localeValue.GetValue()).AsStringC());
      return;
    }
    case XFA_VT_TIME: {
      wsPattern = L"time{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(patternType, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          FX_UTF8Encode(localeValue.GetValue()).AsStringC());
      return;
    }
    case XFA_VT_TEXT: {
      wsPattern = L"text{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(XFA_VT_TEXT, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          FX_UTF8Encode(localeValue.GetValue()).AsStringC());
      return;
    }
    case XFA_VT_FLOAT: {
      wsPattern = L"num{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(XFA_VT_FLOAT, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetDouble(localeValue.GetDoubleNum());
      return;
    }
    default: {
      CFX_WideString wsTestPattern;
      wsTestPattern = L"num{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(XFA_VT_FLOAT, wsValue, wsTestPattern,
                                   pLocale, pMgr);
      if (localeValue.IsValid()) {
        args.GetReturnValue()->SetDouble(localeValue.GetDoubleNum());
        return;
      }

      wsTestPattern = L"text{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue2(XFA_VT_TEXT, wsValue, wsTestPattern,
                                    pLocale, pMgr);
      if (!localeValue2.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          FX_UTF8Encode(localeValue2.GetValue()).AsStringC());
      return;
    }
  }
}

// static
void CXFA_FM2JSContext::Replace(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 2 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Replace");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  CFX_ByteString oneString;
  CFX_ByteString twoString;
  if (!ValueIsNull(pThis, argOne.get()) && !ValueIsNull(pThis, argTwo.get())) {
    ValueToUTF8String(argOne.get(), oneString);
    ValueToUTF8String(argTwo.get(), twoString);
  }

  CFX_ByteString threeString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
    ValueToUTF8String(argThree.get(), threeString);
  }

  int32_t iFindLen = twoString.GetLength();
  CFX_ByteTextBuf resultString;
  int32_t iFindIndex = 0;
  for (int32_t u = 0; u < oneString.GetLength(); ++u) {
    uint8_t ch = oneString.GetAt(u);
    if (ch != twoString.GetAt(iFindIndex)) {
      resultString.AppendChar(ch);
      continue;
    }

    int32_t iTemp = u + 1;
    ++iFindIndex;
    while (iFindIndex < iFindLen) {
      uint8_t chTemp = oneString.GetAt(iTemp);
      if (chTemp != twoString.GetAt(iFindIndex)) {
        iFindIndex = 0;
        break;
      }

      ++iTemp;
      ++iFindIndex;
    }
    if (iFindIndex == iFindLen) {
      resultString << threeString.AsStringC();
      u += iFindLen - 1;
      iFindIndex = 0;
    } else {
      resultString.AppendChar(ch);
    }
  }
  resultString.AppendChar(0);
  args.GetReturnValue()->SetString(resultString.AsStringC());
}

// static
void CXFA_FM2JSContext::Right(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Right");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if ((ValueIsNull(pThis, argOne.get())) ||
      (ValueIsNull(pThis, argTwo.get()))) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString sourceString;
  ValueToUTF8String(argOne.get(), sourceString);
  int32_t count = std::max(0, ValueToInteger(pThis, argTwo.get()));
  args.GetReturnValue()->SetString(sourceString.Right(count).AsStringC());
}

// static
void CXFA_FM2JSContext::Rtrim(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Rtrim");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString sourceString;
  ValueToUTF8String(argOne.get(), sourceString);
  sourceString.TrimRight();
  args.GetReturnValue()->SetString(sourceString.AsStringC());
}

// static
void CXFA_FM2JSContext::Space(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Space");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  int32_t count = std::max(0, ValueToInteger(pThis, argOne.get()));
  CFX_ByteTextBuf spaceString;
  int32_t index = 0;
  while (index < count) {
    spaceString.AppendByte(' ');
    index++;
  }
  spaceString.AppendByte(0);
  args.GetReturnValue()->SetString(spaceString.AsStringC());
}

// static
void CXFA_FM2JSContext::Str(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Str");
    return;
  }

  std::unique_ptr<CFXJSE_Value> numberValue = GetSimpleValue(pThis, args, 0);
  if (numberValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  FX_FLOAT fNumber = ValueToFloat(pThis, numberValue.get());

  int32_t iWidth = 10;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> widthValue = GetSimpleValue(pThis, args, 1);
    iWidth = static_cast<int32_t>(ValueToFloat(pThis, widthValue.get()));
  }

  int32_t iPrecision = 0;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> precisionValue =
        GetSimpleValue(pThis, args, 2);
    iPrecision = std::max(
        0, static_cast<int32_t>(ValueToFloat(pThis, precisionValue.get())));
  }

  CFX_ByteString numberString;
  CFX_ByteString formatStr = "%";
  if (iPrecision) {
    formatStr += ".";
    formatStr += CFX_ByteString::FormatInteger(iPrecision);
  }
  formatStr += "f";
  numberString.Format(formatStr.c_str(), fNumber);

  const FX_CHAR* pData = numberString.c_str();
  int32_t iLength = numberString.GetLength();
  int32_t u = 0;
  while (u < iLength) {
    if (pData[u] == '.')
      break;

    ++u;
  }

  CFX_ByteTextBuf resultBuf;
  if (u > iWidth || (iPrecision + u) >= iWidth) {
    int32_t i = 0;
    while (i < iWidth) {
      resultBuf.AppendChar('*');
      ++i;
    }
    resultBuf.AppendChar(0);
    args.GetReturnValue()->SetString(resultBuf.AsStringC());
    return;
  }

  if (u == iLength) {
    if (iLength > iWidth) {
      int32_t i = 0;
      while (i < iWidth) {
        resultBuf.AppendChar('*');
        ++i;
      }
    } else {
      int32_t i = 0;
      while (i < iWidth - iLength) {
        resultBuf.AppendChar(' ');
        ++i;
      }
      resultBuf << pData;
    }
    args.GetReturnValue()->SetString(resultBuf.AsStringC());
    return;
  }

  int32_t iLeavingSpace = iWidth - u - iPrecision;
  if (iPrecision != 0)
    iLeavingSpace--;

  int32_t i = 0;
  while (i < iLeavingSpace) {
    resultBuf.AppendChar(' ');
    ++i;
  }
  i = 0;
  while (i < u) {
    resultBuf.AppendChar(pData[i]);
    ++i;
  }
  if (iPrecision != 0)
    resultBuf.AppendChar('.');

  u++;
  i = 0;
  while (u < iLength) {
    if (i >= iPrecision)
      break;

    resultBuf.AppendChar(pData[u]);
    ++i;
    ++u;
  }
  while (i < iPrecision) {
    resultBuf.AppendChar('0');
    ++i;
  }
  resultBuf.AppendChar(0);
  args.GetReturnValue()->SetString(resultBuf.AsStringC());
}

// static
void CXFA_FM2JSContext::Stuff(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 3 || argc > 4) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Stuff");
    return;
  }

  CFX_ByteString sourceString;
  CFX_ByteString insertString;
  int32_t iLength = 0;
  int32_t iStart = 0;
  int32_t iDelete = 0;
  std::unique_ptr<CFXJSE_Value> sourceValue = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> startValue = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> deleteValue = GetSimpleValue(pThis, args, 2);
  if (!sourceValue->IsNull() && !startValue->IsNull() &&
      !deleteValue->IsNull()) {
    ValueToUTF8String(sourceValue.get(), sourceString);
    iLength = sourceString.GetLength();
    iStart = std::min(iLength, std::max(1, static_cast<int32_t>(ValueToFloat(
                                               pThis, startValue.get()))));
    iDelete = std::max(
        0, static_cast<int32_t>(ValueToFloat(pThis, deleteValue.get())));
  }

  if (argc > 3) {
    std::unique_ptr<CFXJSE_Value> insertValue = GetSimpleValue(pThis, args, 3);
    ValueToUTF8String(insertValue.get(), insertString);
  }

  iStart -= 1;
  CFX_ByteTextBuf resultString;
  int32_t i = 0;
  while (i < iStart) {
    resultString.AppendChar(sourceString.GetAt(i));
    ++i;
  }
  resultString << insertString.AsStringC();
  i = iStart + iDelete;
  while (i < iLength) {
    resultString.AppendChar(sourceString.GetAt(i));
    ++i;
  }
  resultString.AppendChar(0);
  args.GetReturnValue()->SetString(resultString.AsStringC());
}

// static
void CXFA_FM2JSContext::Substr(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Substr");
    return;
  }

  std::unique_ptr<CFXJSE_Value> stringValue = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> startValue = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> endValue = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, stringValue.get()) ||
      (ValueIsNull(pThis, startValue.get())) ||
      (ValueIsNull(pThis, endValue.get()))) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString szSourceStr;
  int32_t iStart = 0;
  int32_t iCount = 0;
  ValueToUTF8String(stringValue.get(), szSourceStr);
  int32_t iLength = szSourceStr.GetLength();
  if (iLength == 0) {
    args.GetReturnValue()->SetString("");
    return;
  }

  iStart = std::min(
      iLength,
      std::max(1, static_cast<int32_t>(ValueToFloat(pThis, startValue.get()))));
  iCount =
      std::max(0, static_cast<int32_t>(ValueToFloat(pThis, endValue.get())));

  iStart -= 1;
  args.GetReturnValue()->SetString(szSourceStr.Mid(iStart, iCount).AsStringC());
}

// static
void CXFA_FM2JSContext::Uuid(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 0 || argc > 1) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Uuid");
    return;
  }

  int32_t iNum = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
    iNum = static_cast<int32_t>(ValueToFloat(pThis, argOne.get()));
  }
  FX_GUID guid;
  FX_GUID_CreateV4(&guid);

  CFX_ByteString bsUId;
  FX_GUID_ToString(&guid, bsUId, iNum);
  args.GetReturnValue()->SetString(bsUId.AsStringC());
}

// static
void CXFA_FM2JSContext::Upper(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Upper");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_ByteString argString;
  ValueToUTF8String(argOne.get(), argString);

  CFX_WideTextBuf upperStringBuf;
  CFX_WideString wsArgString = CFX_WideString::FromUTF8(argString.AsStringC());
  const FX_WCHAR* pData = wsArgString.c_str();
  int32_t i = 0;
  while (i < wsArgString.GetLength()) {
    int32_t ch = pData[i];
    if ((ch >= 0x61 && ch <= 0x7A) || (ch >= 0xE0 && ch <= 0xFE))
      ch -= 32;
    else if (ch == 0x101 || ch == 0x103 || ch == 0x105)
      ch -= 1;

    upperStringBuf.AppendChar(ch);
    ++i;
  }
  upperStringBuf.AppendChar(0);

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(upperStringBuf.GetBuffer(), upperStringBuf.GetLength())
          .AsStringC());
}

// static
void CXFA_FM2JSContext::WordNum(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToJSContext(pThis, nullptr)
        ->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"WordNum");
    return;
  }

  std::unique_ptr<CFXJSE_Value> numberValue = GetSimpleValue(pThis, args, 0);
  if (numberValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  FX_FLOAT fNumber = ValueToFloat(pThis, numberValue.get());

  int32_t iIdentifier = 0;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> identifierValue =
        GetSimpleValue(pThis, args, 1);
    if (identifierValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iIdentifier =
        static_cast<int32_t>(ValueToFloat(pThis, identifierValue.get()));
  }

  CFX_ByteString localeString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localeValue = GetSimpleValue(pThis, args, 2);
    if (localeValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    ValueToUTF8String(localeValue.get(), localeString);
  }

  if (fNumber < 0.0f || fNumber > 922337203685477550.0f) {
    args.GetReturnValue()->SetString("*");
    return;
  }

  CFX_ByteString numberString;
  numberString.Format("%.2f", fNumber);

  CFX_ByteTextBuf resultBuf;
  WordUS(numberString.AsStringC(), iIdentifier, resultBuf);
  args.GetReturnValue()->SetString(resultBuf.AsStringC());
}

// static
void CXFA_FM2JSContext::TrillionUS(const CFX_ByteStringC& szData,
                                   CFX_ByteTextBuf& strBuf) {
  CFX_ByteStringC pUnits[] = {"zero", "one", "two",   "three", "four",
                              "five", "six", "seven", "eight", "nine"};
  CFX_ByteStringC pCapUnits[] = {"Zero", "One", "Two",   "Three", "Four",
                                 "Five", "Six", "Seven", "Eight", "Nine"};
  CFX_ByteStringC pTens[] = {"Ten",      "Eleven",  "Twelve",  "Thirteen",
                             "Fourteen", "Fifteen", "Sixteen", "Seventeen",
                             "Eighteen", "Nineteen"};
  CFX_ByteStringC pLastTens[] = {"Twenty", "Thirty",  "Forty",  "Fifty",
                                 "Sixty",  "Seventy", "Eighty", "Ninety"};
  CFX_ByteStringC pComm[] = {" Hundred ", " Thousand ", " Million ",
                             " Billion ", "Trillion"};
  const FX_CHAR* pData = szData.c_str();
  int32_t iLength = szData.GetLength();
  int32_t iComm = 0;
  if (iLength > 12)
    iComm = 4;
  else if (iLength > 9)
    iComm = 3;
  else if (iLength > 6)
    iComm = 2;
  else if (iLength > 3)
    iComm = 1;

  int32_t iFirstCount = iLength % 3;
  if (iFirstCount == 0)
    iFirstCount = 3;

  int32_t iIndex = 0;
  if (iFirstCount == 3) {
    if (pData[iIndex] != '0') {
      strBuf << pCapUnits[pData[iIndex] - '0'];
      strBuf << pComm[0];
    }
    if (pData[iIndex + 1] == '0') {
      strBuf << pCapUnits[pData[iIndex + 2] - '0'];
    } else {
      if (pData[iIndex + 1] > '1') {
        strBuf << pLastTens[pData[iIndex + 1] - '2'];
        strBuf << "-";
        strBuf << pUnits[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '1') {
        strBuf << pTens[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '0') {
        strBuf << pCapUnits[pData[iIndex + 2] - '0'];
      }
    }
    iIndex += 3;
  } else if (iFirstCount == 2) {
    if (pData[iIndex] == '0') {
      strBuf << pCapUnits[pData[iIndex + 1] - '0'];
    } else {
      if (pData[iIndex] > '1') {
        strBuf << pLastTens[pData[iIndex] - '2'];
        strBuf << "-";
        strBuf << pUnits[pData[iIndex + 1] - '0'];
      } else if (pData[iIndex] == '1') {
        strBuf << pTens[pData[iIndex + 1] - '0'];
      } else if (pData[iIndex] == '0') {
        strBuf << pCapUnits[pData[iIndex + 1] - '0'];
      }
    }
    iIndex += 2;
  } else if (iFirstCount == 1) {
    strBuf << pCapUnits[pData[iIndex] - '0'];
    iIndex += 1;
  }
  if (iLength > 3 && iFirstCount > 0) {
    strBuf << pComm[iComm];
    --iComm;
  }
  while (iIndex < iLength) {
    if (pData[iIndex] != '0') {
      strBuf << pCapUnits[pData[iIndex] - '0'];
      strBuf << pComm[0];
    }
    if (pData[iIndex + 1] == '0') {
      strBuf << pCapUnits[pData[iIndex + 2] - '0'];
    } else {
      if (pData[iIndex + 1] > '1') {
        strBuf << pLastTens[pData[iIndex + 1] - '2'];
        strBuf << "-";
        strBuf << pUnits[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '1') {
        strBuf << pTens[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '0') {
        strBuf << pCapUnits[pData[iIndex + 2] - '0'];
      }
    }
    if (iIndex < iLength - 3) {
      strBuf << pComm[iComm];
      --iComm;
    }
    iIndex += 3;
  }
}

// static
void CXFA_FM2JSContext::WordUS(const CFX_ByteStringC& szData,
                               int32_t iStyle,
                               CFX_ByteTextBuf& strBuf) {
  const FX_CHAR* pData = szData.c_str();
  int32_t iLength = szData.GetLength();
  if (iStyle < 0 || iStyle > 2) {
    return;
  }

  int32_t iIndex = 0;
  while (iIndex < iLength) {
    if (pData[iIndex] == '.')
      break;
    ++iIndex;
  }
  int32_t iInteger = iIndex;
  iIndex = 0;
  while (iIndex < iInteger) {
    int32_t iCount = (iInteger - iIndex) % 12;
    if (!iCount && iInteger - iIndex > 0)
      iCount = 12;

    TrillionUS(CFX_ByteStringC(pData + iIndex, iCount), strBuf);
    iIndex += iCount;
    if (iIndex < iInteger)
      strBuf << " Trillion ";
  }

  if (iStyle > 0)
    strBuf << " Dollars";

  if (iStyle > 1 && iInteger < iLength) {
    strBuf << " And ";
    iIndex = iInteger + 1;
    while (iIndex < iLength) {
      int32_t iCount = (iLength - iIndex) % 12;
      if (!iCount && iLength - iIndex > 0)
        iCount = 12;

      TrillionUS(CFX_ByteStringC(pData + iIndex, iCount), strBuf);
      iIndex += iCount;
      if (iIndex < iLength)
        strBuf << " Trillion ";
    }
    strBuf << " Cents";
  }
}

// static
void CXFA_FM2JSContext::Get(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Get");
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  if (!pDoc)
    return;

  IXFA_AppProvider* pAppProvider = pDoc->GetNotify()->GetAppProvider();
  if (!pAppProvider)
    return;

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  CFX_ByteString urlString;
  ValueToUTF8String(argOne.get(), urlString);
  IFX_FileRead* pFile = pAppProvider->DownloadURL(
      CFX_WideString::FromUTF8(urlString.AsStringC()));
  if (!pFile)
    return;

  int32_t size = pFile->GetSize();
  std::unique_ptr<uint8_t> pData(FX_Alloc(uint8_t, size));
  pFile->ReadBlock(pData.get(), size);
  args.GetReturnValue()->SetString(CFX_ByteStringC(pData.get(), size));
  pFile->Release();
}

// static
void CXFA_FM2JSContext::Post(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  int32_t argc = args.GetLength();
  if (argc < 2 || argc > 5) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Post");
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  if (!pDoc)
    return;

  IXFA_AppProvider* pAppProvider = pDoc->GetNotify()->GetAppProvider();
  if (!pAppProvider)
    return;

  CFX_ByteString bsURL;
  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ValueToUTF8String(argOne.get(), bsURL);

  CFX_ByteString bsData;
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  ValueToUTF8String(argTwo.get(), bsData);

  CFX_ByteString bsContentType;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
    ValueToUTF8String(argThree.get(), bsContentType);
  }

  CFX_ByteString bsEncode;
  if (argc > 3) {
    std::unique_ptr<CFXJSE_Value> argFour = GetSimpleValue(pThis, args, 3);
    ValueToUTF8String(argFour.get(), bsEncode);
  }

  CFX_ByteString bsHeader;
  if (argc > 4) {
    std::unique_ptr<CFXJSE_Value> argFive = GetSimpleValue(pThis, args, 4);
    ValueToUTF8String(argFive.get(), bsHeader);
  }

  CFX_WideString decodedResponse;
  if (!pAppProvider->PostRequestURL(
          CFX_WideString::FromUTF8(bsURL.AsStringC()),
          CFX_WideString::FromUTF8(bsData.AsStringC()),
          CFX_WideString::FromUTF8(bsContentType.AsStringC()),
          CFX_WideString::FromUTF8(bsEncode.AsStringC()),
          CFX_WideString::FromUTF8(bsHeader.AsStringC()), decodedResponse)) {
    pContext->ThrowException(XFA_IDS_SERVER_DENY);
    return;
  }

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(decodedResponse.c_str(), decodedResponse.GetLength())
          .AsStringC());
}

// static
void CXFA_FM2JSContext::Put(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  int32_t argc = args.GetLength();
  if (argc < 2 || argc > 3) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Put");
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  if (!pDoc)
    return;

  IXFA_AppProvider* pAppProvider = pDoc->GetNotify()->GetAppProvider();
  if (!pAppProvider)
    return;

  CFX_ByteString bsURL;
  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ValueToUTF8String(argOne.get(), bsURL);

  CFX_ByteString bsData;
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  ValueToUTF8String(argTwo.get(), bsData);

  CFX_ByteString bsEncode;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
    ValueToUTF8String(argThree.get(), bsEncode);
  }

  if (!pAppProvider->PutRequestURL(
          CFX_WideString::FromUTF8(bsURL.AsStringC()),
          CFX_WideString::FromUTF8(bsData.AsStringC()),
          CFX_WideString::FromUTF8(bsEncode.AsStringC()))) {
    pContext->ThrowException(XFA_IDS_SERVER_DENY);
    return;
  }

  args.GetReturnValue()->SetString("");
}

// static
void CXFA_FM2JSContext::assign_value_operator(CFXJSE_Value* pThis,
                                              const CFX_ByteStringC& szFuncName,
                                              CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 2) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> lValue = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> rValue = GetSimpleValue(pThis, args, 1);
  if (lValue->IsArray()) {
    v8::Isolate* pIsolate = pContext->GetScriptRuntime();
    std::unique_ptr<CFXJSE_Value> leftLengthValue(new CFXJSE_Value(pIsolate));
    lValue->GetObjectProperty("length", leftLengthValue.get());
    int32_t iLeftLength = leftLengthValue->ToInteger();
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    lValue->GetObjectPropertyByIdx(1, propertyValue.get());
    if (propertyValue->IsNull()) {
      for (int32_t i = 2; i < iLeftLength; i++) {
        lValue->GetObjectPropertyByIdx(i, jsObjectValue.get());
        if (!SetObjectDefaultValue(jsObjectValue.get(), rValue.get())) {
          pContext->ThrowException(XFA_IDS_NOT_DEFAUL_VALUE);
          return;
        }
      }
    } else {
      for (int32_t i = 2; i < iLeftLength; i++) {
        lValue->GetObjectPropertyByIdx(i, jsObjectValue.get());
        jsObjectValue->SetObjectProperty(propertyValue->ToString().AsStringC(),
                                         rValue.get());
      }
    }
  } else if (lValue->IsObject()) {
    if (!SetObjectDefaultValue(lValue.get(), rValue.get())) {
      pContext->ThrowException(XFA_IDS_NOT_DEFAUL_VALUE);
      return;
    }
  }
  args.GetReturnValue()->Assign(rValue.get());
}

// static
void CXFA_FM2JSContext::logical_or_operator(CFXJSE_Value* pThis,
                                            const CFX_ByteStringC& szFuncName,
                                            CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT first = ValueToFloat(pThis, argFirst.get());
  FX_FLOAT second = ValueToFloat(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first || second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::logical_and_operator(CFXJSE_Value* pThis,
                                             const CFX_ByteStringC& szFuncName,
                                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_FLOAT first = ValueToFloat(pThis, argFirst.get());
  FX_FLOAT second = ValueToFloat(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first && second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::equality_operator(CFXJSE_Value* pThis,
                                          const CFX_ByteStringC& szFuncName,
                                          CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  if (fm_ref_equal(pThis, args)) {
    args.GetReturnValue()->SetInteger(1);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 1 : 0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(argFirst->ToString() ==
                                      argSecond->ToString());
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first == second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::notequality_operator(CFXJSE_Value* pThis,
                                             const CFX_ByteStringC& szFuncName,
                                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  if (fm_ref_equal(pThis, args)) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 0 : 1);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(argFirst->ToString() !=
                                      argSecond->ToString());
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger(first != second);
}

// static
FX_BOOL CXFA_FM2JSContext::fm_ref_equal(CFXJSE_Value* pThis,
                                        CFXJSE_Arguments& args) {
  std::unique_ptr<CFXJSE_Value> argFirst = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> argSecond = args.GetValue(1);
  if (!argFirst->IsArray() || !argSecond->IsArray())
    return FALSE;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> firstFlagValue(new CFXJSE_Value(pIsolate));
  std::unique_ptr<CFXJSE_Value> secondFlagValue(new CFXJSE_Value(pIsolate));
  argFirst->GetObjectPropertyByIdx(0, firstFlagValue.get());
  argSecond->GetObjectPropertyByIdx(0, secondFlagValue.get());
  if (firstFlagValue->ToInteger() != 3 || secondFlagValue->ToInteger() != 3)
    return FALSE;

  std::unique_ptr<CFXJSE_Value> firstJSObject(new CFXJSE_Value(pIsolate));
  std::unique_ptr<CFXJSE_Value> secondJSObject(new CFXJSE_Value(pIsolate));
  argFirst->GetObjectPropertyByIdx(2, firstJSObject.get());
  argSecond->GetObjectPropertyByIdx(2, secondJSObject.get());
  if (firstJSObject->IsNull() || secondJSObject->IsNull())
    return FALSE;

  return (firstJSObject->ToHostObject(nullptr) ==
          secondJSObject->ToHostObject(nullptr));
}

// static
void CXFA_FM2JSContext::less_operator(CFXJSE_Value* pThis,
                                      const CFX_ByteStringC& szFuncName,
                                      CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(
        argFirst->ToString().Compare(argSecond->ToString().AsStringC()) == -1);
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first < second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::lessequal_operator(CFXJSE_Value* pThis,
                                           const CFX_ByteStringC& szFuncName,
                                           CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 1 : 0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(
        argFirst->ToString().Compare(argSecond->ToString().AsStringC()) != 1);
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first <= second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::greater_operator(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szFuncName,
                                         CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(
        argFirst->ToString().Compare(argSecond->ToString().AsStringC()) == 1);
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first > second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::greaterequal_operator(CFXJSE_Value* pThis,
                                              const CFX_ByteStringC& szFuncName,
                                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 1 : 0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(
        argFirst->ToString().Compare(argSecond->ToString().AsStringC()) != -1);
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first >= second) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::plus_operator(CFXJSE_Value* pThis,
                                      const CFX_ByteStringC& szFuncName,
                                      CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> argSecond = args.GetValue(1);
  if (ValueIsNull(pThis, argFirst.get()) &&
      ValueIsNull(pThis, argSecond.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetDouble(first + second);
}

// static
void CXFA_FM2JSContext::minus_operator(CFXJSE_Value* pThis,
                                       const CFX_ByteStringC& szFuncName,
                                       CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetDouble(first - second);
}

// static
void CXFA_FM2JSContext::multiple_operator(CFXJSE_Value* pThis,
                                          const CFX_ByteStringC& szFuncName,
                                          CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetDouble(first * second);
}

// static
void CXFA_FM2JSContext::divide_operator(CFXJSE_Value* pThis,
                                        const CFX_ByteStringC& szFuncName,
                                        CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 2) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE second = ValueToDouble(pThis, argSecond.get());
  if (second == 0.0) {
    pContext->ThrowException(XFA_IDS_DIVIDE_ZERO);
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argFirst.get());
  args.GetReturnValue()->SetDouble(first / second);
}

// static
void CXFA_FM2JSContext::positive_operator(CFXJSE_Value* pThis,
                                          const CFX_ByteStringC& szFuncName,
                                          CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  args.GetReturnValue()->SetDouble(0.0 + ValueToDouble(pThis, argOne.get()));
}

// static
void CXFA_FM2JSContext::negative_operator(CFXJSE_Value* pThis,
                                          const CFX_ByteStringC& szFuncName,
                                          CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  args.GetReturnValue()->SetDouble(0.0 - ValueToDouble(pThis, argOne.get()));
}

// static
void CXFA_FM2JSContext::logical_not_operator(CFXJSE_Value* pThis,
                                             const CFX_ByteStringC& szFuncName,
                                             CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  FX_DOUBLE first = ValueToDouble(pThis, argOne.get());
  args.GetReturnValue()->SetInteger((first == 0.0) ? 1 : 0);
}

// static
void CXFA_FM2JSContext::dot_accessor(CFXJSE_Value* pThis,
                                     const CFX_ByteStringC& szFuncName,
                                     CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  int32_t argc = args.GetLength();
  if (argc < 4 || argc > 5) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  bool bIsStar = true;
  int32_t iIndexValue = 0;
  if (argc > 4) {
    bIsStar = false;
    iIndexValue = ValueToInteger(pThis, args.GetValue(4).get());
  }

  CFX_ByteString szName = args.GetUTF8String(2);
  CFX_ByteString szSomExp;
  GenerateSomExpression(szName.AsStringC(), args.GetInt32(3), iIndexValue,
                        bIsStar, szSomExp);

  std::unique_ptr<CFXJSE_Value> argAccessor = args.GetValue(0);
  if (argAccessor->IsArray()) {
    std::unique_ptr<CFXJSE_Value> pLengthValue(new CFXJSE_Value(pIsolate));
    argAccessor->GetObjectProperty("length", pLengthValue.get());
    int32_t iLength = pLengthValue->ToInteger();
    CFXJSE_Value*** hResolveValues = FX_Alloc(CFXJSE_Value**, iLength - 2);
    int32_t* iSizes = FX_Alloc(int32_t, iLength - 2);
    for (int32_t i = 0; i < (iLength - 2); i++)
      iSizes[i] = 0;

    std::unique_ptr<CFXJSE_Value> hJSObjValue(new CFXJSE_Value(pIsolate));
    FX_BOOL bAttribute = FALSE;
    int32_t iCounter = 0;
    for (int32_t i = 2; i < iLength; i++) {
      argAccessor->GetObjectPropertyByIdx(i, hJSObjValue.get());

      XFA_RESOLVENODE_RS resoveNodeRS;
      if (ResolveObjects(pThis, hJSObjValue.get(), szSomExp.AsStringC(),
                         resoveNodeRS, TRUE, szName.IsEmpty()) > 0) {
        ParseResolveResult(pThis, resoveNodeRS, hJSObjValue.get(),
                           hResolveValues[i - 2], iSizes[i - 2], bAttribute);
        iCounter += iSizes[i - 2];
      }
    }
    if (iCounter < 1) {
      CFX_WideString wsPropertyName =
          CFX_WideString::FromUTF8(szName.AsStringC());
      CFX_WideString wsSomExpression =
          CFX_WideString::FromUTF8(szSomExp.AsStringC());
      pContext->ThrowException(XFA_IDS_ACCESS_PROPERTY_IN_NOT_OBJECT,
                               wsPropertyName.c_str(), wsSomExpression.c_str());
      return;
    }

    CFXJSE_Value** rgValues = FX_Alloc(CFXJSE_Value*, iCounter + 2);
    for (int32_t i = 0; i < (iCounter + 2); i++)
      rgValues[i] = new CFXJSE_Value(pIsolate);

    rgValues[0]->SetInteger(1);
    if (bAttribute)
      rgValues[1]->SetString(szName.AsStringC());
    else
      rgValues[1]->SetNull();

    int32_t iIndex = 2;
    for (int32_t i = 0; i < iLength - 2; i++) {
      for (int32_t j = 0; j < iSizes[i]; j++) {
        rgValues[iIndex]->Assign(hResolveValues[i][j]);
        iIndex++;
      }
    }
    args.GetReturnValue()->SetArray(iCounter + 2, rgValues);

    for (int32_t i = 0; i < (iCounter + 2); i++)
      delete rgValues[i];
    FX_Free(rgValues);

    for (int32_t i = 0; i < iLength - 2; i++) {
      for (int32_t j = 0; j < iSizes[i]; j++)
        delete hResolveValues[i][j];

      if (iSizes[i] > 0)
        FX_Free(hResolveValues[i]);
    }
    FX_Free(hResolveValues);
    FX_Free(iSizes);

    return;
  }

  XFA_RESOLVENODE_RS resoveNodeRS;
  int32_t iRet = 0;
  CFX_ByteString bsAccessorName = args.GetUTF8String(1);
  if (argAccessor->IsObject() ||
      (argAccessor->IsNull() && bsAccessorName.IsEmpty())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringC(),
                          resoveNodeRS, TRUE, szName.IsEmpty());
  } else if (!argAccessor->IsObject() && !bsAccessorName.IsEmpty() &&
             GetObjectByName(pThis, argAccessor.get(),
                             bsAccessorName.AsStringC())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringC(),
                          resoveNodeRS, TRUE, szName.IsEmpty());
  }
  if (iRet < 1) {
    CFX_WideString wsPropertyName =
        CFX_WideString::FromUTF8(szName.AsStringC());
    CFX_WideString wsSomExpression =
        CFX_WideString::FromUTF8(szSomExp.AsStringC());
    pContext->ThrowException(XFA_IDS_ACCESS_PROPERTY_IN_NOT_OBJECT,
                             wsPropertyName.c_str(), wsSomExpression.c_str());
    return;
  }

  CFXJSE_Value** hResolveValues;
  int32_t iSize = 0;
  FX_BOOL bAttribute = FALSE;
  ParseResolveResult(pThis, resoveNodeRS, argAccessor.get(), hResolveValues,
                     iSize, bAttribute);
  CFXJSE_Value** rgValues = FX_Alloc(CFXJSE_Value*, iSize + 2);
  for (int32_t i = 0; i < (iSize + 2); i++)
    rgValues[i] = new CFXJSE_Value(pIsolate);

  rgValues[0]->SetInteger(1);
  if (bAttribute)
    rgValues[1]->SetString(szName.AsStringC());
  else
    rgValues[1]->SetNull();

  for (int32_t i = 0; i < iSize; i++)
    rgValues[i + 2]->Assign(hResolveValues[i]);

  args.GetReturnValue()->SetArray(iSize + 2, rgValues);
  for (int32_t i = 0; i < (iSize + 2); i++)
    delete rgValues[i];
  FX_Free(rgValues);

  for (int32_t i = 0; i < iSize; i++)
    delete hResolveValues[i];
  FX_Free(hResolveValues);
}

// static
void CXFA_FM2JSContext::dotdot_accessor(CFXJSE_Value* pThis,
                                        const CFX_ByteStringC& szFuncName,
                                        CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  int32_t argc = args.GetLength();
  if (argc < 4 || argc > 5) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  bool bIsStar = true;
  int32_t iIndexValue = 0;
  if (argc > 4) {
    bIsStar = false;
    iIndexValue = ValueToInteger(pThis, args.GetValue(4).get());
  }

  CFX_ByteString szSomExp;
  CFX_ByteString szName = args.GetUTF8String(2);
  GenerateSomExpression(szName.AsStringC(), args.GetInt32(3), iIndexValue,
                        bIsStar, szSomExp);

  std::unique_ptr<CFXJSE_Value> argAccessor = args.GetValue(0);
  if (argAccessor->IsArray()) {
    std::unique_ptr<CFXJSE_Value> pLengthValue(new CFXJSE_Value(pIsolate));
    argAccessor->GetObjectProperty("length", pLengthValue.get());
    int32_t iLength = pLengthValue->ToInteger();
    int32_t iCounter = 0;

    CFXJSE_Value*** hResolveValues = FX_Alloc(CFXJSE_Value**, iLength - 2);
    int32_t* iSizes = FX_Alloc(int32_t, iLength - 2);
    std::unique_ptr<CFXJSE_Value> hJSObjValue(new CFXJSE_Value(pIsolate));
    FX_BOOL bAttribute = FALSE;
    for (int32_t i = 2; i < iLength; i++) {
      argAccessor->GetObjectPropertyByIdx(i, hJSObjValue.get());
      XFA_RESOLVENODE_RS resoveNodeRS;
      if (ResolveObjects(pThis, hJSObjValue.get(), szSomExp.AsStringC(),
                         resoveNodeRS, FALSE) > 0) {
        ParseResolveResult(pThis, resoveNodeRS, hJSObjValue.get(),
                           hResolveValues[i - 2], iSizes[i - 2], bAttribute);
        iCounter += iSizes[i - 2];
      }
    }
    if (iCounter < 1) {
      CFX_WideString wsPropertyName =
          CFX_WideString::FromUTF8(szName.AsStringC());
      CFX_WideString wsSomExpression =
          CFX_WideString::FromUTF8(szSomExp.AsStringC());
      pContext->ThrowException(XFA_IDS_ACCESS_PROPERTY_IN_NOT_OBJECT,
                               wsPropertyName.c_str(), wsSomExpression.c_str());
      return;
    }

    CFXJSE_Value** rgValues = FX_Alloc(CFXJSE_Value*, iCounter + 2);
    for (int32_t i = 0; i < (iCounter + 2); i++)
      rgValues[i] = new CFXJSE_Value(pIsolate);

    rgValues[0]->SetInteger(1);
    if (bAttribute)
      rgValues[1]->SetString(szName.AsStringC());
    else
      rgValues[1]->SetNull();

    int32_t iIndex = 2;
    for (int32_t i = 0; i < iLength - 2; i++) {
      for (int32_t j = 0; j < iSizes[i]; j++) {
        rgValues[iIndex]->Assign(hResolveValues[i][j]);
        iIndex++;
      }
    }
    args.GetReturnValue()->SetArray(iCounter + 2, rgValues);
    for (int32_t i = 0; i < (iCounter + 2); i++)
      delete rgValues[i];
    FX_Free(rgValues);

    for (int32_t i = 0; i < iLength - 2; i++) {
      for (int32_t j = 0; j < iSizes[i]; j++)
        delete hResolveValues[i][j];
      FX_Free(hResolveValues[i]);
    }
    FX_Free(hResolveValues);
    FX_Free(iSizes);
    return;
  }

  XFA_RESOLVENODE_RS resoveNodeRS;
  int32_t iRet = 0;
  CFX_ByteString bsAccessorName = args.GetUTF8String(1);
  if (argAccessor->IsObject() ||
      (argAccessor->IsNull() && bsAccessorName.IsEmpty())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringC(),
                          resoveNodeRS, FALSE);
  } else if (!argAccessor->IsObject() && !bsAccessorName.IsEmpty() &&
             GetObjectByName(pThis, argAccessor.get(),
                             bsAccessorName.AsStringC())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringC(),
                          resoveNodeRS, FALSE);
  }
  if (iRet < 1) {
    CFX_WideString wsPropertyName =
        CFX_WideString::FromUTF8(szName.AsStringC());
    CFX_WideString wsSomExpression =
        CFX_WideString::FromUTF8(szSomExp.AsStringC());
    pContext->ThrowException(XFA_IDS_ACCESS_PROPERTY_IN_NOT_OBJECT,
                             wsPropertyName.c_str(), wsSomExpression.c_str());
    return;
  }

  CFXJSE_Value** hResolveValues;
  int32_t iSize = 0;
  FX_BOOL bAttribute = FALSE;
  ParseResolveResult(pThis, resoveNodeRS, argAccessor.get(), hResolveValues,
                     iSize, bAttribute);
  CFXJSE_Value** rgValues = FX_Alloc(CFXJSE_Value*, iSize + 2);
  for (int32_t i = 0; i < (iSize + 2); i++)
    rgValues[i] = new CFXJSE_Value(pIsolate);

  rgValues[0]->SetInteger(1);
  if (bAttribute)
    rgValues[1]->SetString(szName.AsStringC());
  else
    rgValues[1]->SetNull();

  for (int32_t i = 0; i < iSize; i++)
    rgValues[i + 2]->Assign(hResolveValues[i]);

  args.GetReturnValue()->SetArray(iSize + 2, rgValues);

  for (int32_t i = 0; i < (iSize + 2); i++)
    delete rgValues[i];
  FX_Free(rgValues);

  for (int32_t i = 0; i < iSize; i++)
    delete hResolveValues[i];
  FX_Free(hResolveValues);
}

// static
void CXFA_FM2JSContext::eval_translation(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szFuncName,
                                         CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"Eval");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  CFX_ByteString argString;
  ValueToUTF8String(argOne.get(), argString);
  if (argString.IsEmpty()) {
    pContext->ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    return;
  }

  CFX_WideString scriptString = CFX_WideString::FromUTF8(argString.AsStringC());
  CFX_WideTextBuf wsJavaScriptBuf;
  CFX_WideString wsError;
  CXFA_FM2JSContext::Translate(scriptString.AsStringC(), wsJavaScriptBuf,
                               wsError);
  if (!wsError.IsEmpty()) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  CFX_WideString javaScript = wsJavaScriptBuf.MakeString();
  args.GetReturnValue()->SetString(
      FX_UTF8Encode(javaScript.c_str(), javaScript.GetLength()).AsStringC());
}

// static
void CXFA_FM2JSContext::is_fm_object(CFXJSE_Value* pThis,
                                     const CFX_ByteStringC& szFuncName,
                                     CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    args.GetReturnValue()->SetBoolean(FALSE);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  args.GetReturnValue()->SetBoolean(argOne->IsObject());
}

// static
void CXFA_FM2JSContext::is_fm_array(CFXJSE_Value* pThis,
                                    const CFX_ByteStringC& szFuncName,
                                    CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    args.GetReturnValue()->SetBoolean(FALSE);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  args.GetReturnValue()->SetBoolean(argOne->IsArray());
}

// static
void CXFA_FM2JSContext::get_fm_value(CFXJSE_Value* pThis,
                                     const CFX_ByteStringC& szFuncName,
                                     CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (argOne->IsArray()) {
    v8::Isolate* pIsolate = pContext->GetScriptRuntime();
    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    argOne->GetObjectPropertyByIdx(1, propertyValue.get());
    argOne->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), args.GetReturnValue());
      return;
    }

    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                     args.GetReturnValue());
    return;
  }

  if (argOne->IsObject()) {
    GetObjectDefaultValue(argOne.get(), args.GetReturnValue());
    return;
  }

  args.GetReturnValue()->Assign(argOne.get());
}

// static
void CXFA_FM2JSContext::get_fm_jsobj(CFXJSE_Value* pThis,
                                     const CFX_ByteStringC& szFuncName,
                                     CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToJSContext(pThis, nullptr)->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (!argOne->IsArray()) {
    args.GetReturnValue()->Assign(argOne.get());
    return;
  }

#ifndef NDEBUG
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
  argOne->GetObjectProperty("length", lengthValue.get());
  ASSERT(lengthValue->ToInteger() >= 3);
#endif

  argOne->GetObjectPropertyByIdx(2, args.GetReturnValue());
}

// static
void CXFA_FM2JSContext::fm_var_filter(CFXJSE_Value* pThis,
                                      const CFX_ByteStringC& szFuncName,
                                      CFXJSE_Arguments& args) {
  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  if (args.GetLength() != 1) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }

  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (!argOne->IsArray()) {
    std::unique_ptr<CFXJSE_Value> simpleValue = GetSimpleValue(pThis, args, 0);
    args.GetReturnValue()->Assign(simpleValue.get());
    return;
  }

#ifndef NDEBUG
  std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
  argOne->GetObjectProperty("length", lengthValue.get());
  ASSERT(lengthValue->ToInteger() >= 3);
#endif

  std::unique_ptr<CFXJSE_Value> flagsValue(new CFXJSE_Value(pIsolate));
  argOne->GetObjectPropertyByIdx(0, flagsValue.get());
  int32_t iFlags = flagsValue->ToInteger();
  if (iFlags != 3 && iFlags != 4) {
    std::unique_ptr<CFXJSE_Value> simpleValue = GetSimpleValue(pThis, args, 0);
    args.GetReturnValue()->Assign(simpleValue.get());
    return;
  }

  if (iFlags == 4) {
    CFXJSE_Value* rgValues[3];
    for (int32_t i = 0; i < 3; i++)
      rgValues[i] = new CFXJSE_Value(pIsolate);

    rgValues[0]->SetInteger(3);
    rgValues[1]->SetNull();
    rgValues[2]->SetNull();
    args.GetReturnValue()->SetArray(3, rgValues);
    for (int32_t i = 0; i < 3; i++)
      delete rgValues[i];

    return;
  }

  std::unique_ptr<CFXJSE_Value> objectValue(new CFXJSE_Value(pIsolate));
  argOne->GetObjectPropertyByIdx(2, objectValue.get());
  if (objectValue->IsNull()) {
    pContext->ThrowException(XFA_IDS_COMPILER_ERROR);
    return;
  }
  args.GetReturnValue()->Assign(argOne.get());
}

// static
void CXFA_FM2JSContext::concat_fm_object(CFXJSE_Value* pThis,
                                         const CFX_ByteStringC& szFuncName,
                                         CFXJSE_Arguments& args) {
  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  uint32_t iLength = 0;
  int32_t argc = args.GetLength();
  std::vector<std::unique_ptr<CFXJSE_Value>> argValues;
  for (int32_t i = 0; i < argc; i++) {
    argValues.push_back(args.GetValue(i));
    if (argValues[i]->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argValues[i]->GetObjectProperty("length", lengthValue.get());
      int32_t length = lengthValue->ToInteger();
      iLength = iLength + ((length > 2) ? (length - 2) : 0);
    }
    iLength += 1;
  }

  CFXJSE_Value** returnValues = FX_Alloc(CFXJSE_Value*, iLength);
  for (int32_t i = 0; i < (int32_t)iLength; i++)
    returnValues[i] = new CFXJSE_Value(pIsolate);

  int32_t index = 0;
  for (int32_t i = 0; i < argc; i++) {
    if (argValues[i]->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argValues[i]->GetObjectProperty("length", lengthValue.get());
      int32_t length = lengthValue->ToInteger();
      for (int32_t j = 2; j < length; j++) {
        argValues[i]->GetObjectPropertyByIdx(j, returnValues[index]);
        index++;
      }
    }
    returnValues[index]->Assign(argValues[i].get());
    index++;
  }
  args.GetReturnValue()->SetArray(iLength, returnValues);
  for (int32_t i = 0; i < (int32_t)iLength; i++)
    delete returnValues[i];

  FX_Free(returnValues);
}

// static
std::unique_ptr<CFXJSE_Value> CXFA_FM2JSContext::GetSimpleValue(
    CFXJSE_Value* pThis,
    CFXJSE_Arguments& args,
    uint32_t index) {
  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  ASSERT(index < (uint32_t)args.GetLength());

  std::unique_ptr<CFXJSE_Value> argIndex = args.GetValue(index);
  if (!argIndex->IsArray() && !argIndex->IsObject())
    return argIndex;

  if (argIndex->IsArray()) {
    std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
    argIndex->GetObjectProperty("length", lengthValue.get());
    int32_t iLength = lengthValue->ToInteger();
    std::unique_ptr<CFXJSE_Value> simpleValue(new CFXJSE_Value(pIsolate));
    if (iLength < 3) {
      simpleValue.get()->SetUndefined();
      return simpleValue;
    }

    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    argIndex->GetObjectPropertyByIdx(1, propertyValue.get());
    argIndex->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), simpleValue.get());
      return simpleValue;
    }

    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                     simpleValue.get());
    return simpleValue;
  }

  std::unique_ptr<CFXJSE_Value> defaultValue(new CFXJSE_Value(pIsolate));
  GetObjectDefaultValue(argIndex.get(), defaultValue.get());
  return defaultValue;
}

// static
FX_BOOL CXFA_FM2JSContext::ValueIsNull(CFXJSE_Value* pThis, CFXJSE_Value* arg) {
  if (!arg || arg->IsNull())
    return TRUE;

  if (!arg->IsArray() && !arg->IsObject())
    return FALSE;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  if (arg->IsArray()) {
    int32_t iLength = hvalue_get_array_length(pThis, arg);
    if (iLength < 3)
      return TRUE;

    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    arg->GetObjectPropertyByIdx(1, propertyValue.get());
    arg->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      std::unique_ptr<CFXJSE_Value> defaultValue(new CFXJSE_Value(pIsolate));
      GetObjectDefaultValue(jsObjectValue.get(), defaultValue.get());
      return defaultValue->IsNull();
    }

    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                     newPropertyValue.get());
    return newPropertyValue->IsNull();
  }

  std::unique_ptr<CFXJSE_Value> defaultValue(new CFXJSE_Value(pIsolate));
  GetObjectDefaultValue(arg, defaultValue.get());
  return defaultValue->IsNull();
}

// static
int32_t CXFA_FM2JSContext::hvalue_get_array_length(CFXJSE_Value* pThis,
                                                   CFXJSE_Value* arg) {
  if (!arg || !arg->IsArray())
    return 0;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
  arg->GetObjectProperty("length", lengthValue.get());
  return lengthValue->ToInteger();
}

// static
FX_BOOL CXFA_FM2JSContext::simpleValueCompare(CFXJSE_Value* pThis,
                                              CFXJSE_Value* firstValue,
                                              CFXJSE_Value* secondValue) {
  if (!firstValue)
    return FALSE;

  if (firstValue->IsString()) {
    CFX_ByteString firstString, secondString;
    ValueToUTF8String(firstValue, firstString);
    ValueToUTF8String(secondValue, secondString);
    return firstString == secondString;
  }
  if (firstValue->IsNumber()) {
    FX_FLOAT first = ValueToFloat(pThis, firstValue);
    FX_FLOAT second = ValueToFloat(pThis, secondValue);
    return (first == second);
  }
  if (firstValue->IsBoolean())
    return (firstValue->ToBoolean() == secondValue->ToBoolean());

  return firstValue->IsNull() && secondValue && secondValue->IsNull();
}

// static
void CXFA_FM2JSContext::unfoldArgs(CFXJSE_Value* pThis,
                                   CFXJSE_Arguments& args,
                                   CFXJSE_Value**& resultValues,
                                   int32_t& iCount,
                                   int32_t iStart) {
  iCount = 0;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  int32_t argc = args.GetLength();
  std::vector<std::unique_ptr<CFXJSE_Value>> argsValue;
  for (int32_t i = 0; i < argc - iStart; i++) {
    argsValue.push_back(args.GetValue(i + iStart));
    if (argsValue[i]->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argsValue[i]->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      iCount += ((iLength > 2) ? (iLength - 2) : 0);
    } else {
      iCount += 1;
    }
  }
  resultValues = FX_Alloc(CFXJSE_Value*, iCount);
  for (int32_t i = 0; i < iCount; i++)
    resultValues[i] = new CFXJSE_Value(pIsolate);

  int32_t index = 0;
  for (int32_t i = 0; i < argc - iStart; i++) {
    if (argsValue[i]->IsArray()) {
      std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
      argsValue[i]->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength < 3)
        continue;

      std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
      std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
      argsValue[i]->GetObjectPropertyByIdx(1, propertyValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argsValue[i]->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), resultValues[index]);
          index++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argsValue[i]->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringC(), resultValues[index]);
          index++;
        }
      }
    } else if (argsValue[i]->IsObject()) {
      GetObjectDefaultValue(argsValue[i].get(), resultValues[index]);
      index++;
    } else {
      resultValues[index]->Assign(argsValue[i].get());
      index++;
    }
  }
}

// static
void CXFA_FM2JSContext::GetObjectDefaultValue(CFXJSE_Value* pValue,
                                              CFXJSE_Value* pDefaultValue) {
  CXFA_Node* pNode = ToNode(CXFA_ScriptContext::ToObject(pValue, nullptr));
  if (!pNode) {
    pDefaultValue->SetNull();
    return;
  }
  pNode->Script_Som_DefaultValue(pDefaultValue, FALSE, (XFA_ATTRIBUTE)-1);
}

// static
FX_BOOL CXFA_FM2JSContext::SetObjectDefaultValue(CFXJSE_Value* pValue,
                                                 CFXJSE_Value* hNewValue) {
  CXFA_Node* pNode = ToNode(CXFA_ScriptContext::ToObject(pValue, nullptr));
  if (!pNode)
    return FALSE;

  pNode->Script_Som_DefaultValue(hNewValue, TRUE, (XFA_ATTRIBUTE)-1);
  return TRUE;
}

// static
void CXFA_FM2JSContext::GenerateSomExpression(const CFX_ByteStringC& szName,
                                              int32_t iIndexFlags,
                                              int32_t iIndexValue,
                                              bool bIsStar,
                                              CFX_ByteString& szSomExp) {
  if (bIsStar) {
    szSomExp = szName + "[*]";
    return;
  }
  if (iIndexFlags == 0) {
    szSomExp = szName;
    return;
  }
  if (iIndexFlags == 1 || iIndexValue == 0) {
    szSomExp = szName + "[" +
               CFX_ByteString::FormatInteger(iIndexValue, FXFORMAT_SIGNED) +
               "]";
  } else if (iIndexFlags == 2) {
    szSomExp = (iIndexValue < 0) ? (szName + "[-") : (szName + "[+");
    iIndexValue = (iIndexValue < 0) ? (0 - iIndexValue) : iIndexValue;
    szSomExp += CFX_ByteString::FormatInteger(iIndexValue);
    szSomExp += "]";
  } else {
    szSomExp = (iIndexValue < 0) ? (szName + "[") : (szName + "[-");
    iIndexValue = (iIndexValue < 0) ? (0 - iIndexValue) : iIndexValue;
    szSomExp += CFX_ByteString::FormatInteger(iIndexValue);
    szSomExp += "]";
  }
}

// static
FX_BOOL CXFA_FM2JSContext::GetObjectByName(
    CFXJSE_Value* pThis,
    CFXJSE_Value* accessorValue,
    const CFX_ByteStringC& szAccessorName) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return FALSE;

  CXFA_ScriptContext* pScriptContext = pDoc->GetScriptContext();
  XFA_RESOLVENODE_RS resoveNodeRS;
  uint32_t dwFlags = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
                     XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent;
  int32_t iRet = pScriptContext->ResolveObjects(
      pScriptContext->GetThisObject(),
      CFX_WideString::FromUTF8(szAccessorName).AsStringC(), resoveNodeRS,
      dwFlags);
  if (iRet >= 1 && resoveNodeRS.dwFlags == XFA_RESOVENODE_RSTYPE_Nodes) {
    accessorValue->Assign(
        pScriptContext->GetJSValueFromMap(resoveNodeRS.nodes.GetAt(0)));
    return TRUE;
  }
  return FALSE;
}

// static
int32_t CXFA_FM2JSContext::ResolveObjects(CFXJSE_Value* pThis,
                                          CFXJSE_Value* pRefValue,
                                          const CFX_ByteStringC& bsSomExp,
                                          XFA_RESOLVENODE_RS& resoveNodeRS,
                                          FX_BOOL bdotAccessor,
                                          FX_BOOL bHasNoResolveName) {
  CXFA_Document* pDoc = ToJSContext(pThis, nullptr)->GetDocument();
  if (!pDoc)
    return -1;

  CFX_WideString wsSomExpression = CFX_WideString::FromUTF8(bsSomExp);
  CXFA_ScriptContext* pScriptContext = pDoc->GetScriptContext();
  CXFA_Object* pNode = nullptr;
  uint32_t dFlags = 0UL;
  if (bdotAccessor) {
    if (pRefValue && pRefValue->IsNull()) {
      pNode = pScriptContext->GetThisObject();
      dFlags = XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent;
    } else {
      pNode = CXFA_ScriptContext::ToObject(pRefValue, nullptr);
      ASSERT(pNode);
      if (bHasNoResolveName) {
        CFX_WideString wsName;
        if (CXFA_Node* pXFANode = pNode->AsNode())
          pXFANode->GetAttribute(XFA_ATTRIBUTE_Name, wsName, FALSE);
        if (wsName.IsEmpty())
          wsName = FX_WSTRC(L"#") + pNode->GetClassName();

        wsSomExpression = wsName + wsSomExpression;
        dFlags = XFA_RESOLVENODE_Siblings;
      } else {
        dFlags = (bsSomExp == "*")
                     ? (XFA_RESOLVENODE_Children)
                     : (XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                        XFA_RESOLVENODE_Properties);
      }
    }
  } else {
    pNode = CXFA_ScriptContext::ToObject(pRefValue, nullptr);
    dFlags = XFA_RESOLVENODE_AnyChild;
  }
  return pScriptContext->ResolveObjects(pNode, wsSomExpression.AsStringC(),
                                        resoveNodeRS, dFlags);
}

// static
void CXFA_FM2JSContext::ParseResolveResult(
    CFXJSE_Value* pThis,
    const XFA_RESOLVENODE_RS& resoveNodeRS,
    CFXJSE_Value* pParentValue,
    CFXJSE_Value**& resultValues,
    int32_t& iSize,
    FX_BOOL& bAttribute) {
  iSize = 0;
  resultValues = nullptr;

  CXFA_FM2JSContext* pContext = ToJSContext(pThis, nullptr);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  if (resoveNodeRS.dwFlags == XFA_RESOVENODE_RSTYPE_Nodes) {
    bAttribute = FALSE;
    iSize = resoveNodeRS.nodes.GetSize();
    resultValues = FX_Alloc(CFXJSE_Value*, iSize);
    for (int32_t i = 0; i < iSize; i++) {
      resultValues[i] = new CFXJSE_Value(pIsolate);
      resultValues[i]->Assign(
          pContext->GetDocument()->GetScriptContext()->GetJSValueFromMap(
              resoveNodeRS.nodes.GetAt(i)));
    }
    return;
  }

  CXFA_ValueArray objectProperties(pIsolate);
  int32_t iRet = resoveNodeRS.GetAttributeResult(objectProperties);
  bAttribute = (iRet == 0);
  if (!bAttribute) {
    iSize = iRet;
    resultValues = FX_Alloc(CFXJSE_Value*, iSize);
    for (int32_t i = 0; i < iSize; i++) {
      resultValues[i] = new CFXJSE_Value(pIsolate);
      resultValues[i]->Assign(objectProperties[i]);
    }
    return;
  }

  if (!pParentValue || !pParentValue->IsObject())
    return;

  iSize = 1;
  resultValues = FX_Alloc(CFXJSE_Value*, 1);
  resultValues[0] = new CFXJSE_Value(pIsolate);
  resultValues[0]->Assign(pParentValue);
}

// static
int32_t CXFA_FM2JSContext::ValueToInteger(CFXJSE_Value* pThis,
                                          CFXJSE_Value* pValue) {
  if (!pValue)
    return 0;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  if (pValue->IsArray()) {
    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    pValue->GetObjectPropertyByIdx(1, propertyValue.get());
    pValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
      return ValueToInteger(pThis, newPropertyValue.get());
    }

    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                     newPropertyValue.get());
    return ValueToInteger(pThis, newPropertyValue.get());
  }
  if (pValue->IsObject()) {
    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    GetObjectDefaultValue(pValue, newPropertyValue.get());
    return ValueToInteger(pThis, newPropertyValue.get());
  }
  if (pValue->IsString())
    return FXSYS_atoi(pValue->ToString().c_str());
  return pValue->ToInteger();
}

// static
FX_FLOAT CXFA_FM2JSContext::ValueToFloat(CFXJSE_Value* pThis,
                                         CFXJSE_Value* arg) {
  if (!arg)
    return 0.0f;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  if (arg->IsArray()) {
    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    arg->GetObjectPropertyByIdx(1, propertyValue.get());
    arg->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
      return ValueToFloat(pThis, newPropertyValue.get());
    }
    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                     newPropertyValue.get());
    return ValueToFloat(pThis, newPropertyValue.get());
  }
  if (arg->IsObject()) {
    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    GetObjectDefaultValue(arg, newPropertyValue.get());
    return ValueToFloat(pThis, newPropertyValue.get());
  }
  if (arg->IsString())
    return (FX_FLOAT)XFA_ByteStringToDouble(arg->ToString().AsStringC());
  if (arg->IsUndefined())
    return 0;

  return arg->ToFloat();
}

// static
FX_DOUBLE CXFA_FM2JSContext::ValueToDouble(CFXJSE_Value* pThis,
                                           CFXJSE_Value* arg) {
  if (!arg)
    return 0;

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  if (arg->IsArray()) {
    std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    arg->GetObjectPropertyByIdx(1, propertyValue.get());
    arg->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
      return ValueToDouble(pThis, newPropertyValue.get());
    }
    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                     newPropertyValue.get());
    return ValueToDouble(pThis, newPropertyValue.get());
  }
  if (arg->IsObject()) {
    std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
    GetObjectDefaultValue(arg, newPropertyValue.get());
    return ValueToDouble(pThis, newPropertyValue.get());
  }
  if (arg->IsString())
    return XFA_ByteStringToDouble(arg->ToString().AsStringC());
  if (arg->IsUndefined())
    return 0;
  return arg->ToDouble();
}

// static.
double CXFA_FM2JSContext::ExtractDouble(CFXJSE_Value* pThis,
                                        CFXJSE_Value* src,
                                        bool* ret) {
  ASSERT(ret);
  *ret = true;

  if (!src)
    return 0;

  if (!src->IsArray())
    return ValueToDouble(pThis, src);

  v8::Isolate* pIsolate = ToJSContext(pThis, nullptr)->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> lengthValue(new CFXJSE_Value(pIsolate));
  src->GetObjectProperty("length", lengthValue.get());
  int32_t iLength = lengthValue->ToInteger();
  if (iLength <= 2) {
    *ret = false;
    return 0.0;
  }

  std::unique_ptr<CFXJSE_Value> propertyValue(new CFXJSE_Value(pIsolate));
  std::unique_ptr<CFXJSE_Value> jsObjectValue(new CFXJSE_Value(pIsolate));
  src->GetObjectPropertyByIdx(1, propertyValue.get());
  src->GetObjectPropertyByIdx(2, jsObjectValue.get());
  if (propertyValue->IsNull())
    return ValueToDouble(pThis, jsObjectValue.get());

  std::unique_ptr<CFXJSE_Value> newPropertyValue(new CFXJSE_Value(pIsolate));
  jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringC(),
                                   newPropertyValue.get());
  return ValueToDouble(pThis, newPropertyValue.get());
}

// static
void CXFA_FM2JSContext::ValueToUTF8String(CFXJSE_Value* arg,
                                          CFX_ByteString& szOutputString) {
  if (!arg)
    return;

  if (arg->IsNull() || arg->IsUndefined())
    szOutputString = "";
  else if (arg->IsBoolean())
    szOutputString = arg->ToBoolean() ? "1" : "0";
  else
    szOutputString = arg->ToString();
}

// static.
int32_t CXFA_FM2JSContext::Translate(const CFX_WideStringC& wsFormcalc,
                                     CFX_WideTextBuf& wsJavascript,
                                     CFX_WideString& wsError) {
  if (wsFormcalc.IsEmpty()) {
    wsJavascript.Clear();
    wsError.clear();
    return 0;
  }
  int32_t status = 0;
  CXFA_FMProgram program;
  status = program.Init(wsFormcalc);
  if (status) {
    wsError = program.GetError().message;
    return status;
  }
  status = program.ParseProgram();
  if (status) {
    wsError = program.GetError().message;
    return status;
  }
  program.TranslateProgram(wsJavascript);
  return 0;
}

CXFA_FM2JSContext::CXFA_FM2JSContext(v8::Isolate* pScriptIsolate,
                                     CFXJSE_Context* pScriptContext,
                                     CXFA_Document* pDoc)
    : m_pIsolate(pScriptIsolate),
      m_pFMClass(
          CFXJSE_Class::Create(pScriptContext, &formcalc_fm2js_descriptor)),
      m_pValue(new CFXJSE_Value(pScriptIsolate)),
      m_pDocument(pDoc) {
  m_pValue.get()->SetNull();
  m_pValue.get()->SetObject(this, m_pFMClass);
}

CXFA_FM2JSContext::~CXFA_FM2JSContext() {}

void CXFA_FM2JSContext::GlobalPropertyGetter(CFXJSE_Value* pValue) {
  pValue->Assign(m_pValue.get());
}

void CXFA_FM2JSContext::ThrowException(int32_t iStringID, ...) {
  IXFA_AppProvider* pAppProvider = m_pDocument->GetNotify()->GetAppProvider();
  ASSERT(pAppProvider);
  CFX_WideString wsFormat;
  pAppProvider->LoadString(iStringID, wsFormat);
  CFX_WideString wsMessage;
  va_list arg_ptr;
  va_start(arg_ptr, iStringID);
  wsMessage.FormatV(wsFormat.c_str(), arg_ptr);
  va_end(arg_ptr);
  FXJSE_ThrowMessage(
      FX_UTF8Encode(wsMessage.c_str(), wsMessage.GetLength()).AsStringC());
}
