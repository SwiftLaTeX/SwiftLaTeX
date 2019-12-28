// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_CRT_FGAS_CODEPAGE_H_
#define XFA_FGAS_CRT_FGAS_CODEPAGE_H_

#include "core/fxcrt/include/fx_basic.h"

#define FX_CODEPAGE_DefANSI 0
#define FX_CODEPAGE_DefOEM 1
#define FX_CODEPAGE_DefMAC 2
#define FX_CODEPAGE_Thread 3
#define FX_CODEPAGE_Symbol 42
#define FX_CODEPAGE_MSDOS_US 437
#define FX_CODEPAGE_Arabic_ASMO708 708
#define FX_CODEPAGE_Arabic_ASMO449Plus 709
#define FX_CODEPAGE_Arabic_Transparent 710
#define FX_CODEPAGE_Arabic_NafithaEnhanced 711
#define FX_CODEPAGE_Arabic_TransparentASMO 720
#define FX_CODEPAGE_MSDOS_Greek1 737
#define FX_CODEPAGE_MSDOS_Baltic 775
#define FX_CODEPAGE_MSWin31_WesternEuropean 819
#define FX_CODEPAGE_MSDOS_WesternEuropean 850
#define FX_CODEPAGE_MSDOS_EasternEuropean 852
#define FX_CODEPAGE_MSDOS_Latin3 853
#define FX_CODEPAGE_MSDOS_Cyrillic 855
#define FX_CODEPAGE_MSDOS_Turkish 857
#define FX_CODEPAGE_MSDOS_Latin1Euro 858
#define FX_CODEPAGE_MSDOS_Portuguese 860
#define FX_CODEPAGE_MSDOS_Icelandic 861
#define FX_CODEPAGE_MSDOS_Hebrew 862
#define FX_CODEPAGE_MSDOS_FrenchCanadian 863
#define FX_CODEPAGE_MSDOS_Arabic 864
#define FX_CODEPAGE_MSDOS_Norwegian 865
#define FX_CODEPAGE_MSDOS_Russian 866
#define FX_CODEPAGE_MSDOS_Greek2 869
#define FX_CODEPAGE_MSDOS_Thai 874
#define FX_CODEPAGE_MSDOS_KamenickyCS 895
#define FX_CODEPAGE_ShiftJIS 932
#define FX_CODEPAGE_ChineseSimplified 936
#define FX_CODEPAGE_Korean 949
#define FX_CODEPAGE_ChineseTraditional 950
#define FX_CODEPAGE_UTF16LE 1200
#define FX_CODEPAGE_UTF16BE 1201
#define FX_CODEPAGE_MSWin_EasternEuropean 1250
#define FX_CODEPAGE_MSWin_Cyrillic 1251
#define FX_CODEPAGE_MSWin_WesternEuropean 1252
#define FX_CODEPAGE_MSWin_Greek 1253
#define FX_CODEPAGE_MSWin_Turkish 1254
#define FX_CODEPAGE_MSWin_Hebrew 1255
#define FX_CODEPAGE_MSWin_Arabic 1256
#define FX_CODEPAGE_MSWin_Baltic 1257
#define FX_CODEPAGE_MSWin_Vietnamese 1258
#define FX_CODEPAGE_Johab 1361
#define FX_CODEPAGE_MAC_Roman 10000
#define FX_CODEPAGE_MAC_ShiftJIS 10001
#define FX_CODEPAGE_MAC_ChineseTraditional 10002
#define FX_CODEPAGE_MAC_Korean 10003
#define FX_CODEPAGE_MAC_Arabic 10004
#define FX_CODEPAGE_MAC_Hebrew 10005
#define FX_CODEPAGE_MAC_Greek 10006
#define FX_CODEPAGE_MAC_Cyrillic 10007
#define FX_CODEPAGE_MAC_ChineseSimplified 10008
#define FX_CODEPAGE_MAC_Thai 10021
#define FX_CODEPAGE_MAC_EasternEuropean 10029
#define FX_CODEPAGE_MAC_Turkish 10081
#define FX_CODEPAGE_UTF32LE 12000
#define FX_CODEPAGE_UTF32BE 12001
#define FX_CODEPAGE_ISO8859_1 28591
#define FX_CODEPAGE_ISO8859_2 28592
#define FX_CODEPAGE_ISO8859_3 28593
#define FX_CODEPAGE_ISO8859_4 28594
#define FX_CODEPAGE_ISO8859_5 28595
#define FX_CODEPAGE_ISO8859_6 28596
#define FX_CODEPAGE_ISO8859_7 28597
#define FX_CODEPAGE_ISO8859_8 28598
#define FX_CODEPAGE_ISO8859_9 28599
#define FX_CODEPAGE_ISO8859_10 28600
#define FX_CODEPAGE_ISO8859_11 28601
#define FX_CODEPAGE_ISO8859_12 28602
#define FX_CODEPAGE_ISO8859_13 28603
#define FX_CODEPAGE_ISO8859_14 28604
#define FX_CODEPAGE_ISO8859_15 28605
#define FX_CODEPAGE_ISO8859_16 28606
#define FX_CODEPAGE_ISCII_Devanagari 57002
#define FX_CODEPAGE_ISCII_Bengali 57003
#define FX_CODEPAGE_ISCII_Tamil 57004
#define FX_CODEPAGE_ISCII_Telugu 57005
#define FX_CODEPAGE_ISCII_Assamese 57006
#define FX_CODEPAGE_ISCII_Oriya 57007
#define FX_CODEPAGE_ISCII_Kannada 57008
#define FX_CODEPAGE_ISCII_Malayalam 57009
#define FX_CODEPAGE_ISCII_Gujarati 57010
#define FX_CODEPAGE_ISCII_Punjabi 57011
#define FX_CODEPAGE_UTF7 65000
#define FX_CODEPAGE_UTF8 65001
#define FX_CHARSET_ANSI 0
#define FX_CHARSET_Default 1
#define FX_CHARSET_Symbol 2
#define FX_CHARSET_MAC_Roman 77
#define FX_CHARSET_MAC_ShiftJIS 78
#define FX_CHARSET_MAC_Korean 79
#define FX_CHARSET_MAC_ChineseSimplified 80
#define FX_CHARSET_MAC_ChineseTriditional 81
#define FX_CHARSET_MAC_Johab 82
#define FX_CHARSET_MAC_Hebrew 83
#define FX_CHARSET_MAC_Arabic 84
#define FX_CHARSET_MAC_Greek 85
#define FX_CHARSET_MAC_Turkish 86
#define FX_CHARSET_MAC_Thai 87
#define FX_CHARSET_MAC_EasternEuropean 88
#define FX_CHARSET_MAC_Cyrillic 89
#define FX_CHARSET_ShiftJIS 128
#define FX_CHARSET_Korean 129
#define FX_CHARSET_Johab 130
#define FX_CHARSET_ChineseSimplified 134
#define FX_CHARSET_ChineseTriditional 136
#define FX_CHARSET_MSWin_Greek 161
#define FX_CHARSET_MSWin_Turkish 162
#define FX_CHARSET_MSWin_Vietnamese 163
#define FX_CHARSET_MSWin_Hebrew 177
#define FX_CHARSET_MSWin_Arabic 178
#define FX_CHARSET_ArabicTraditional 179
#define FX_CHARSET_ArabicUser 180
#define FX_CHARSET_HebrewUser 181
#define FX_CHARSET_MSWin_Baltic 186
#define FX_CHARSET_MSWin_Cyrillic 204
#define FX_CHARSET_Thai 222
#define FX_CHARSET_MSWin_EasterEuropean 238
#define FX_CHARSET_US 254
#define FX_CHARSET_OEM 255

uint16_t FX_GetCodePageFromCharset(uint8_t charset);
uint16_t FX_GetCharsetFromCodePage(uint16_t codepage);
uint16_t FX_GetCodePageFromStringW(const FX_WCHAR* pStr, int32_t iLength);
uint16_t FX_GetDefCodePageByLanguage(uint16_t wLanguage);
void FX_SwapByteOrder(FX_WCHAR* pStr, int32_t iLength);

void FX_UTF16ToWChar(void* pBuffer, int32_t iLength);
void FX_WCharToUTF16(void* pBuffer, int32_t iLength);
int32_t FX_DecodeString(uint16_t wCodePage,
                        const FX_CHAR* pSrc,
                        int32_t* pSrcLen,
                        FX_WCHAR* pDst,
                        int32_t* pDstLen,
                        FX_BOOL bErrBreak = FALSE);
int32_t FX_UTF8Decode(const FX_CHAR* pSrc,
                      int32_t* pSrcLen,
                      FX_WCHAR* pDst,
                      int32_t* pDstLen);

#endif  // XFA_FGAS_CRT_FGAS_CODEPAGE_H_
