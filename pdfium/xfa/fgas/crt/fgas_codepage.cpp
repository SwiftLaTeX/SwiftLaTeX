// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/crt/fgas_language.h"

namespace {

struct FX_STR2CPHASH {
  uint32_t uHash;
  uint16_t uCodePage;
};

struct FX_CHARSET_MAP {
  uint16_t charset;
  uint16_t codepage;
};

struct FX_LANG2CPMAP {
  uint16_t wLanguage;
  uint16_t wCodepage;
};

const FX_CHARSET_MAP g_FXCharset2CodePageTable[] = {
    {0, 1252},   {1, 0},      {2, 42},     {77, 10000}, {78, 10001},
    {79, 10003}, {80, 10008}, {81, 10002}, {83, 10005}, {84, 10004},
    {85, 10006}, {86, 10081}, {87, 10021}, {88, 10029}, {89, 10007},
    {128, 932},  {129, 949},  {130, 1361}, {134, 936},  {136, 950},
    {161, 1253}, {162, 1254}, {163, 1258}, {177, 1255}, {178, 1256},
    {186, 1257}, {204, 1251}, {222, 874},  {238, 1250}, {254, 437},
    {255, 850},
};

const FX_CHARSET_MAP g_FXCodepage2CharsetTable[] = {
    {1, 0},      {2, 42},     {254, 437},  {255, 850},  {222, 874},
    {128, 932},  {134, 936},  {129, 949},  {136, 950},  {238, 1250},
    {204, 1251}, {0, 1252},   {161, 1253}, {162, 1254}, {177, 1255},
    {178, 1256}, {186, 1257}, {163, 1258}, {130, 1361}, {77, 10000},
    {78, 10001}, {79, 10003}, {80, 10008}, {81, 10002}, {83, 10005},
    {84, 10004}, {85, 10006}, {86, 10081}, {87, 10021}, {88, 10029},
    {89, 10007},
};

const FX_LANG2CPMAP g_FXLang2CodepageTable[] = {
    {FX_LANG_Arabic_SaudiArabia, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Bulgarian_Bulgaria, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Catalan_Catalan, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Chinese_Taiwan, FX_CODEPAGE_ChineseTraditional},
    {FX_LANG_CzechRepublic, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Danish_Denmark, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_German_Germany, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Greek_Greece, FX_CODEPAGE_MSWin_Greek},
    {FX_LANG_English_UnitedStates, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_TraditionalSort, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Finnish_Finland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_French_France, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Hebrew_Israel, FX_CODEPAGE_MSWin_Hebrew},
    {FX_LANG_Hungarian_Hungary, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Icelandic_Iceland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Italian_Italy, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Japanese_Japan, FX_CODEPAGE_ShiftJIS},
    {FX_LANG_Korean_Korea, FX_CODEPAGE_Korean},
    {FX_LANG_Dutch_Netherlands, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Norwegian_Bokmal, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Polish_Poland, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Portuguese_Brazil, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Romanian_Romania, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Russian_Russia, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Croatian_Croatia, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Slovak_Slovakia, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Albanian_Albania, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Swedish_Sweden, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Thai_Thailand, FX_CODEPAGE_MSDOS_Thai},
    {FX_LANG_Turkish_Turkey, FX_CODEPAGE_MSWin_Turkish},
    {FX_LANG_Urdu_Pakistan, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Indonesian_Indonesia, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Ukrainian_Ukraine, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Belarusian_Belarus, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Slovenian_Slovenia, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Estonian_Estonia, FX_CODEPAGE_MSWin_Baltic},
    {FX_LANG_Latvian_Latvia, FX_CODEPAGE_MSWin_Baltic},
    {FX_LANG_Lithuanian_Lithuania, FX_CODEPAGE_MSWin_Baltic},
    {FX_LANG_Persian, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Vietnamese_Vietnam, FX_CODEPAGE_MSWin_Vietnamese},
    {FX_LANG_Armenian_Armenia, FX_CODEPAGE_DefANSI},
    {FX_LANG_Azerbaijan_Latin, FX_CODEPAGE_MSWin_Turkish},
    {FX_LANG_Basque_Basque, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Macedonian, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Afrikaans_SouthAfrica, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Georgian_Georgia, FX_CODEPAGE_DefANSI},
    {FX_LANG_Faroese_FaroeIslands, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Hindi_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Malay_Malaysia, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Kazakh_Kazakhstan, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Kyrgyz_Kyrgyzstan, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Kiswahili_Kenya, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Uzbek_LatinUzbekistan, FX_CODEPAGE_MSWin_Turkish},
    {FX_LANG_Tatar_Russia, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Punjabi_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Gujarati_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Tamil_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Telugu_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Kannada_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Marathi_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_SanskritIndia, FX_CODEPAGE_DefANSI},
    {FX_LANG_Mongolian_CyrillicMongolia, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Galician_Galician, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Konkani_India, FX_CODEPAGE_DefANSI},
    {FX_LANG_Syriac_Syria, FX_CODEPAGE_DefANSI},
    {FX_LANG_Divehi_Maldives, FX_CODEPAGE_DefANSI},
    {FX_LANG_Arabic_Iraq, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Chinese_PRC, FX_CODEPAGE_ChineseSimplified},
    {FX_LANG_German_Switzerland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_English_UnitedKingdom, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Mexico, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_French_Belgium, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Italian_Switzerland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Dutch_Belgium, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Norwegian_Nynorsk, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Portuguese_Portugal, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_SerbianLatin_Serbia, FX_CODEPAGE_MSWin_EasternEuropean},
    {FX_LANG_Swedish_Finland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Azerbaijan_Cyrillic, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Malay_BruneiDarussalam, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Uzbek_CyrillicUzbekistan, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Arabic_Egypt, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Chinese_HongKong, FX_CODEPAGE_ChineseTraditional},
    {FX_LANG_German_Austria, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_English_Australia, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_InternationalSort, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_French_Canada, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_SerbianCyrillic_Serbia, FX_CODEPAGE_MSWin_Cyrillic},
    {FX_LANG_Arabic_Libya, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Chinese_Singapore, FX_CODEPAGE_ChineseSimplified},
    {FX_LANG_German_Luxembourg, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_English_Canada, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Guatemala, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_French_Switzerland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Algeria, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Chinese_Macao, FX_CODEPAGE_ChineseTraditional},
    {FX_LANG_German_Liechtenstein, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_English_NewZealand, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_CostaRica, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_French_Luxembourg, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Morocco, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_Ireland, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Panama, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_French_Monaco, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Tunisia, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_SouthAfrica, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_DominicanRepublic, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Oman, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_Jamaica, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Venezuela, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Yemen, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_Caribbean, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Colombia, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Syria, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_Belize, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Peru, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Jordan, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_TrinidadTobago, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Argentina, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Lebanon, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_Zimbabwe, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Ecuador, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Kuwait, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_English_Philippines, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Chile, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_UAE, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Spanish_Uruguay, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Bahrain, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Spanish_Paraguay, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Arabic_Qatar, FX_CODEPAGE_MSWin_Arabic},
    {FX_LANG_Spanish_Bolivia, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_ElSalvador, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Honduras, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_Nicaragua, FX_CODEPAGE_MSWin_WesternEuropean},
    {FX_LANG_Spanish_PuertoRico, FX_CODEPAGE_MSWin_WesternEuropean},
};

const FX_STR2CPHASH g_FXCPHashTable[] = {
    {0xd45, 0x6faf},      {0xd46, 0x6fb0},      {0xd47, 0x6fb1},
    {0xd48, 0x6fb2},      {0xd49, 0x4e6},       {0xd4d, 0x6fbd},
    {0xe9e, 0x4e4},       {0xc998, 0x1b5},      {0x18ef0, 0x3a8},
    {0x19f85, 0x5182},    {0x2e2335, 0x3b6},    {0x325153, 0x5182},
    {0x145bded, 0x2716},  {0x3c9a5f2, 0xc6f3},  {0x4c45f2d, 0x3a4},
    {0x4c45f4e, 0xc431},  {0x58caf51, 0x4e4},   {0x5a5cd7d, 0x3a8},
    {0x5a6c6a7, 0x4e4},   {0x5a6ca0b, 0x1b5},   {0x5a6cd68, 0x307},
    {0x5a6d8d3, 0x4e4},   {0x5a6d948, 0x354},   {0x5a6d96b, 0x362},
    {0x5a6d984, 0x366},   {0x5a90e35, 0x1b5},   {0x5e0cf00, 0x6fb5},
    {0x609c324, 0x551},   {0x617d97f, 0x5182},  {0x6a6fd91, 0xfde8},
    {0x6a6fd92, 0xfde9},  {0x6b102de, 0xcadc},  {0x6b10f48, 0x4e89},
    {0x1020805f, 0x4e4},  {0x10f0524c, 0x6fb5}, {0x11d558fe, 0x6fb0},
    {0x13898d19, 0xc42d}, {0x13898d3a, 0xc431}, {0x138a319e, 0x6fb1},
    {0x14679c09, 0x96c6}, {0x153f0a3d, 0x6fb2}, {0x1576eeb3, 0x4e20},
    {0x169a0ce6, 0xc6f9}, {0x16f3e2dc, 0x6fb3}, {0x18a8bb7a, 0x6fb4},
    {0x1a5d9419, 0x6fb5}, {0x1a847b48, 0x3a8},  {0x1b762419, 0xcec8},
    {0x1b9d7847, 0x475},  {0x1c126cb9, 0x6fb6}, {0x1ccdbc7d, 0x4f42},
    {0x1d330f5f, 0x2714}, {0x1dc74559, 0x4e6},  {0x1edd80da, 0x4e4},
    {0x23e4b03d, 0xfde8}, {0x24f28a16, 0x4f3d}, {0x286e7a32, 0x2715},
    {0x2c7c18ed, 0x3a8},  {0x2e2103b7, 0x2713}, {0x304bf479, 0x6fb4},
    {0x304bf47d, 0x6fb5}, {0x309bb869, 0xfde8}, {0x309bb86a, 0xfde9},
    {0x33664357, 0x3b6},  {0x352d6b49, 0x3a4},  {0x36f5661c, 0x1b5},
    {0x392e8f48, 0xcadc}, {0x3dc7c64c, 0x47c},  {0x3ed2e8e1, 0x4e4},
    {0x3f0c2fea, 0xcaed}, {0x3f0fef8f, 0xc6f2}, {0x3f5e130f, 0x5182},
    {0x47174d1f, 0x3a8},  {0x49686b7b, 0x6fb4}, {0x4b80b0d9, 0x3a4},
    {0x4dcda97a, 0x4e4},  {0x4dcda9b6, 0x4e4},  {0x4e881e6a, 0x5221},
    {0x4ffdf5a1, 0x36a},  {0x4ffdf5a5, 0x6fbd}, {0x5241ce16, 0x4e8b},
    {0x546bab9d, 0x4e4},  {0x54a3d64e, 0x6fb6}, {0x562179bd, 0x5161},
    {0x57c1df15, 0xc6f7}, {0x61ff6e62, 0x4f36}, {0x6359c7d8, 0x4f35},
    {0x63f3c335, 0x3a8},  {0x645a0f78, 0x477},  {0x691ac2fd, 0x275f},
    {0x6dc2eab0, 0x2d0},  {0x6dc2eeef, 0x35e},  {0x6dc2ef10, 0x36a},
    {0x7103138a, 0x47d},  {0x710dfbd0, 0xc6f5}, {0x7319f6cb, 0x36a},
    {0x745096ad, 0x3a8},  {0x74866229, 0x4e8c}, {0x77185fa5, 0x3a8},
    {0x7953f002, 0x6faf}, {0x7953f003, 0x6fb0}, {0x7953f004, 0x6fb1},
    {0x7953f005, 0x6fb2}, {0x7953f006, 0x6fb7}, {0x7953f00a, 0x6fbd},
    {0x7c577571, 0x2761}, {0x7e8c8ff1, 0x479},  {0x8031f47f, 0x3b5},
    {0x8031f481, 0x3b5},  {0x80c4a710, 0x5187}, {0x857c7e14, 0xfde8},
    {0x857c7e15, 0xfde9}, {0x86b59c90, 0x4e4},  {0x86b59c91, 0x6fb0},
    {0x86b59c92, 0x6fb1}, {0x86b59c93, 0x6fb2}, {0x86b59c94, 0x6fb3},
    {0x86b59c95, 0x6fb4}, {0x86b59c96, 0x6fb5}, {0x86b59c97, 0x4e7},
    {0x86b59c98, 0x4e6},  {0x8b4b24ec, 0x5190}, {0x8face362, 0x4e4},
    {0x8ff9ec2a, 0xfde9}, {0x919d3989, 0xcadc}, {0x9967e5ad, 0x4e22},
    {0x99f8b933, 0x6fbd}, {0x9bd2a380, 0x4fc7}, {0x9befad23, 0x4f38},
    {0x9c7ac649, 0x4f3c}, {0xa02468db, 0xdeae}, {0xa02468ec, 0xdeab},
    {0xa024692a, 0xdeaa}, {0xa0246997, 0xdeb2}, {0xa02469ff, 0xdeb0},
    {0xa0246a3d, 0xdeb1}, {0xa0246a8c, 0xdeaf}, {0xa0246a9a, 0xdeb3},
    {0xa0246b16, 0xdeac}, {0xa0246b1a, 0xdead}, {0xa071addc, 0x4b1},
    {0xa38b62dc, 0x474},  {0xa4c09fed, 0x3a8},  {0xa51e86e5, 0x4e7},
    {0xa67ab13e, 0x3a4},  {0xa7414244, 0x51a9}, {0xa9ddbead, 0xc6fb},
    {0xab24ffab, 0x4e8a}, {0xabef8ac4, 0x2710}, {0xabfa20ac, 0x6fb4},
    {0xad36895e, 0x4e2},  {0xad36895f, 0x4e3},  {0xaf310e90, 0x402},
    {0xaf31166f, 0x4e8},  {0xaf7277a5, 0x3b6},  {0xafc0d8b3, 0x96c6},
    {0xb0fd5dba, 0xcae0}, {0xb0fd5e95, 0xcadc}, {0xb1052893, 0x7149},
    {0xb1e98745, 0x36a},  {0xb277e91c, 0x5166}, {0xb2f7eac5, 0xcae0},
    {0xb2f7eba0, 0xcadc}, {0xb2f7ebc1, 0x3b5},  {0xb53fa77d, 0x3a8},
    {0xb6391138, 0x6fb5}, {0xb7358b7f, 0x6fb6}, {0xb8c42b40, 0x4e4},
    {0xb8c42ea4, 0x1b5},  {0xb8c439e7, 0x2e1},  {0xb8c43a61, 0x307},
    {0xb8c43d6c, 0x4e4},  {0xb8c43ddf, 0x352},  {0xb8c43de1, 0x354},
    {0xb8c43de6, 0x359},  {0xb8c43dff, 0x35d},  {0xb8c43e04, 0x362},
    {0xb8c43e07, 0x365},  {0xbcd29a7f, 0x3a8},  {0xbce34e78, 0x5182},
    {0xbce34e7b, 0x556a}, {0xbce81504, 0x3b5},  {0xbd8a4c95, 0x272d},
    {0xbdd89dad, 0x4e4},  {0xbdd89dae, 0x6fb0}, {0xbdd89daf, 0x6fb1},
    {0xbdd89db0, 0x6fb2}, {0xbdd89db1, 0x4e6},  {0xbdd89db5, 0x6fbd},
    {0xc1756e9f, 0x36b},  {0xc7482444, 0x47a},  {0xc9281c18, 0x4e4},
    {0xc9ef95df, 0x47b},  {0xccc9db0d, 0x4e4},  {0xccc9db0e, 0x6fb0},
    {0xcd73425f, 0x3b6},  {0xce38b40b, 0x4b0},  {0xce99e549, 0x25},
    {0xcf598740, 0x4e7},  {0xcf6d6f78, 0x4e4},  {0xcf758df6, 0x3a4},
    {0xd1266e51, 0x6fb5}, {0xd2910213, 0x2718}, {0xd29196bb, 0x2712},
    {0xd3eb2fc2, 0x476},  {0xd442dc2c, 0x4fc4}, {0xd9da4da4, 0x2711},
    {0xdbad2f42, 0x4e4},  {0xdbad2f43, 0x6fb0}, {0xdbad2f44, 0x6fb1},
    {0xdbad2f45, 0x6fb2}, {0xdbad2f46, 0x6fb3}, {0xdbad2f47, 0x6fb4},
    {0xdbad2f48, 0x6fb5}, {0xdbad2f49, 0x6fb6}, {0xdbad2f4a, 0x4e6},
    {0xdc438033, 0x4f31}, {0xdccb439b, 0x477},  {0xdccdc626, 0x3b5},
    {0xdd80a595, 0x4e4},  {0xdd80a596, 0x6fb0}, {0xdd80a59e, 0x6fb1},
    {0xdd80a5b4, 0x6fb2}, {0xdd80a5d9, 0x6fb5}, {0xdd80a5da, 0x6fb4},
    {0xdd80a5fa, 0x6fb6}, {0xdd80a615, 0x6fb3}, {0xdd80a619, 0x4e6},
    {0xdd80a61a, 0x3b5},  {0xdd80c0f8, 0x4e9f}, {0xdf7e46ff, 0x4fc8},
    {0xdf8680fd, 0x556a}, {0xdfb0bd6e, 0xc42d}, {0xdff05486, 0x2c4},
    {0xe3323399, 0x3a4},  {0xe60412dd, 0x3b5},  {0xeee47add, 0x4b0},
    {0xf021a186, 0x4e2},  {0xf021a187, 0x4e3},  {0xf021a188, 0x4e4},
    {0xf021a189, 0x4e5},  {0xf021a18a, 0x4e6},  {0xf021a18b, 0x4e7},
    {0xf021a18c, 0x4e8},  {0xf021a18d, 0x4e9},  {0xf021a18e, 0x4ea},
    {0xf0700456, 0x6fb3}, {0xf274f175, 0x3b5},  {0xf2a9730b, 0x3a8},
    {0xf3d463c2, 0x3a4},  {0xf52a70a3, 0xc42e}, {0xf5693147, 0x6fb3},
    {0xf637e157, 0x478},  {0xfc213f3a, 0x2717}, {0xff654d14, 0x3b5},
};

uint16_t GetCodePageFromStringA(const FX_CHAR* pStr, int32_t iLength) {
  ASSERT(pStr);
  if (iLength < 0) {
    iLength = FXSYS_strlen(pStr);
  }
  if (iLength == 0) {
    return 0xFFFF;
  }
  uint32_t uHash = FX_HashCode_GetA(CFX_ByteStringC(pStr, iLength), true);
  int32_t iStart = 0;
  int32_t iEnd = sizeof(g_FXCPHashTable) / sizeof(FX_STR2CPHASH) - 1;
  ASSERT(iEnd >= 0);
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    const FX_STR2CPHASH& cp = g_FXCPHashTable[iMid];
    if (uHash == cp.uHash) {
      return (uint16_t)cp.uCodePage;
    } else if (uHash < cp.uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return 0xFFFF;
}

}  // namespace

uint16_t FX_GetCodePageFromCharset(uint8_t charset) {
  int32_t iEnd = sizeof(g_FXCharset2CodePageTable) / sizeof(FX_CHARSET_MAP) - 1;
  ASSERT(iEnd >= 0);
  int32_t iStart = 0, iMid;
  do {
    iMid = (iStart + iEnd) / 2;
    const FX_CHARSET_MAP& cp = g_FXCharset2CodePageTable[iMid];
    if (charset == cp.charset) {
      return cp.codepage;
    } else if (charset < cp.charset) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return 0xFFFF;
}

uint16_t FX_GetCharsetFromCodePage(uint16_t codepage) {
  int32_t iEnd = sizeof(g_FXCodepage2CharsetTable) / sizeof(FX_CHARSET_MAP) - 1;
  ASSERT(iEnd >= 0);
  int32_t iStart = 0, iMid;
  do {
    iMid = (iStart + iEnd) / 2;
    const FX_CHARSET_MAP& cp = g_FXCodepage2CharsetTable[iMid];
    if (codepage == cp.codepage) {
      return cp.charset;
    } else if (codepage < cp.codepage) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return 0xFFFF;
}

uint16_t FX_GetDefCodePageByLanguage(uint16_t wLanguage) {
  int32_t iEnd = sizeof(g_FXLang2CodepageTable) / sizeof(FX_LANG2CPMAP) - 1;
  ASSERT(iEnd >= 0);
  int32_t iStart = 0, iMid;
  do {
    iMid = (iStart + iEnd) / 2;
    const FX_LANG2CPMAP& cp = g_FXLang2CodepageTable[iMid];
    if (wLanguage == cp.wLanguage) {
      return cp.wCodepage;
    } else if (wLanguage < cp.wLanguage) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return 0xFFFF;
}

uint16_t FX_GetCodePageFromStringW(const FX_WCHAR* pStr, int32_t iLength) {
  if (iLength < 0) {
    iLength = FXSYS_wcslen(pStr);
  }
  if (iLength == 0) {
    return 0xFFFF;
  }
  CFX_ByteString csStr;
  FX_CHAR* pBuf = csStr.GetBuffer(iLength + 1);
  for (int32_t i = 0; i < iLength; ++i) {
    *pBuf++ = (FX_CHAR)*pStr++;
  }
  csStr.ReleaseBuffer(iLength);
  return GetCodePageFromStringA(csStr.c_str(), iLength);
}

void FX_SwapByteOrder(FX_WCHAR* pStr, int32_t iLength) {
  ASSERT(pStr);
  if (iLength < 0) {
    iLength = FXSYS_wcslen(pStr);
  }
  uint16_t wch;
  if (sizeof(FX_WCHAR) > 2) {
    while (iLength-- > 0) {
      wch = (uint16_t)*pStr;
      wch = (wch >> 8) | (wch << 8);
      wch &= 0x00FF;
      *pStr++ = wch;
    }
  } else {
    while (iLength-- > 0) {
      wch = (uint16_t)*pStr;
      wch = (wch >> 8) | (wch << 8);
      *pStr++ = wch;
    }
  }
}

void FX_UTF16ToWChar(void* pBuffer, int32_t iLength) {
  ASSERT(pBuffer && iLength > 0);
  if (sizeof(FX_WCHAR) == 2) {
    return;
  }
  uint16_t* pSrc = (uint16_t*)pBuffer;
  FX_WCHAR* pDst = (FX_WCHAR*)pBuffer;
  while (--iLength >= 0) {
    pDst[iLength] = (FX_WCHAR)pSrc[iLength];
  }
}

void FX_WCharToUTF16(void* pBuffer, int32_t iLength) {
  ASSERT(pBuffer && iLength > 0);
  if (sizeof(FX_WCHAR) == 2) {
    return;
  }
  const FX_WCHAR* pSrc = (const FX_WCHAR*)pBuffer;
  uint16_t* pDst = (uint16_t*)pBuffer;
  while (--iLength >= 0) {
    *pDst++ = (uint16_t)*pSrc++;
  }
}

int32_t FX_DecodeString(uint16_t wCodePage,
                        const FX_CHAR* pSrc,
                        int32_t* pSrcLen,
                        FX_WCHAR* pDst,
                        int32_t* pDstLen,
                        FX_BOOL bErrBreak) {
  if (wCodePage == FX_CODEPAGE_UTF8) {
    return FX_UTF8Decode(pSrc, pSrcLen, pDst, pDstLen);
  }
  return -1;
}
int32_t FX_UTF8Decode(const FX_CHAR* pSrc,
                      int32_t* pSrcLen,
                      FX_WCHAR* pDst,
                      int32_t* pDstLen) {
  if (!pSrcLen || !pDstLen) {
    return -1;
  }
  int32_t iSrcLen = *pSrcLen;
  if (iSrcLen < 1) {
    *pSrcLen = *pDstLen = 0;
    return 1;
  }
  int32_t iDstLen = *pDstLen;
  FX_BOOL bValidDst = (pDst && iDstLen > 0);
  uint32_t dwCode = 0;
  int32_t iPending = 0;
  int32_t iSrcNum = 0, iDstNum = 0;
  int32_t k = 0;
  int32_t iIndex = 0;
  k = 1;
  while (iIndex < iSrcLen) {
    uint8_t byte = (uint8_t) * (pSrc + iIndex);
    if (byte < 0x80) {
      iPending = 0;
      k = 1;
      iDstNum++;
      iSrcNum += k;
      if (bValidDst) {
        *pDst++ = byte;
        if (iDstNum >= iDstLen) {
          break;
        }
      }
    } else if (byte < 0xc0) {
      if (iPending < 1) {
        break;
      }
      iPending--;
      dwCode |= (byte & 0x3f) << (iPending * 6);
      if (iPending == 0) {
        iDstNum++;
        iSrcNum += k;
        if (bValidDst) {
          *pDst++ = dwCode;
          if (iDstNum >= iDstLen) {
            break;
          }
        }
      }
    } else if (byte < 0xe0) {
      iPending = 1;
      k = 2;
      dwCode = (byte & 0x1f) << 6;
    } else if (byte < 0xf0) {
      iPending = 2;
      k = 3;
      dwCode = (byte & 0x0f) << 12;
    } else if (byte < 0xf8) {
      iPending = 3;
      k = 4;
      dwCode = (byte & 0x07) << 18;
    } else if (byte < 0xfc) {
      iPending = 4;
      k = 5;
      dwCode = (byte & 0x03) << 24;
    } else if (byte < 0xfe) {
      iPending = 5;
      k = 6;
      dwCode = (byte & 0x01) << 30;
    } else {
      break;
    }
    iIndex++;
  }
  *pSrcLen = iSrcNum;
  *pDstLen = iDstNum;
  return 1;
}
