// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/codec/codec_int.h"
#include "core/fxcodec/include/fx_codec.h"

namespace {

const uint8_t OneLeadPos[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
const uint8_t ZeroLeadPos[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,
};

int FindBit(const uint8_t* data_buf, int max_pos, int start_pos, int bit) {
  if (start_pos >= max_pos) {
    return max_pos;
  }
  const uint8_t* leading_pos = bit ? OneLeadPos : ZeroLeadPos;
  if (start_pos % 8) {
    uint8_t data = data_buf[start_pos / 8];
    if (bit) {
      data &= 0xff >> (start_pos % 8);
    } else {
      data |= 0xff << (8 - start_pos % 8);
    }
    if (leading_pos[data] < 8) {
      return start_pos / 8 * 8 + leading_pos[data];
    }
    start_pos += 7;
  }
  uint8_t skip = bit ? 0x00 : 0xff;
  int byte_pos = start_pos / 8;
  int max_byte = (max_pos + 7) / 8;
  while (byte_pos < max_byte) {
    if (data_buf[byte_pos] != skip) {
      break;
    }
    byte_pos++;
  }
  if (byte_pos == max_byte) {
    return max_pos;
  }
  int pos = leading_pos[data_buf[byte_pos]] + byte_pos * 8;
  if (pos > max_pos) {
    pos = max_pos;
  }
  return pos;
}

void FaxG4FindB1B2(const uint8_t* ref_buf,
                   int columns,
                   int a0,
                   bool a0color,
                   int& b1,
                   int& b2) {
  uint8_t first_bit =
      (a0 < 0) ? 1 : ((ref_buf[a0 / 8] & (1 << (7 - a0 % 8))) != 0);
  b1 = FindBit(ref_buf, columns, a0 + 1, !first_bit);
  if (b1 >= columns) {
    b1 = b2 = columns;
    return;
  }
  if (first_bit == !a0color) {
    b1 = FindBit(ref_buf, columns, b1 + 1, first_bit);
    first_bit = !first_bit;
  }
  if (b1 >= columns) {
    b1 = b2 = columns;
    return;
  }
  b2 = FindBit(ref_buf, columns, b1 + 1, first_bit);
}

void FaxFillBits(uint8_t* dest_buf, int columns, int startpos, int endpos) {
  if (startpos < 0) {
    startpos = 0;
  }
  if (endpos < 0) {
    endpos = 0;
  }
  if (endpos >= columns) {
    endpos = columns;
  }
  if (startpos >= endpos) {
    return;
  }
  int first_byte = startpos / 8;
  int last_byte = (endpos - 1) / 8;
  if (first_byte == last_byte) {
    for (int i = startpos % 8; i <= (endpos - 1) % 8; i++) {
      dest_buf[first_byte] -= 1 << (7 - i);
    }
    return;
  }
  int i;
  for (i = startpos % 8; i < 8; i++) {
    dest_buf[first_byte] -= 1 << (7 - i);
  }
  for (i = 0; i <= (endpos - 1) % 8; i++) {
    dest_buf[last_byte] -= 1 << (7 - i);
  }
  if (last_byte > first_byte + 1) {
    FXSYS_memset(dest_buf + first_byte + 1, 0, last_byte - first_byte - 1);
  }
}

#define NEXTBIT                                  \
  src_buf[bitpos / 8] & (1 << (7 - bitpos % 8)); \
  bitpos++;

const uint8_t FaxBlackRunIns[] = {
    0,          2,          0x02,       3,          0,          0x03,
    2,          0,          2,          0x02,       1,          0,
    0x03,       4,          0,          2,          0x02,       6,
    0,          0x03,       5,          0,          1,          0x03,
    7,          0,          2,          0x04,       9,          0,
    0x05,       8,          0,          3,          0x04,       10,
    0,          0x05,       11,         0,          0x07,       12,
    0,          2,          0x04,       13,         0,          0x07,
    14,         0,          1,          0x18,       15,         0,
    5,          0x08,       18,         0,          0x0f,       64,
    0,          0x17,       16,         0,          0x18,       17,
    0,          0x37,       0,          0,          10,         0x08,
    0x00,       0x07,       0x0c,       0x40,       0x07,       0x0d,
    0x80,       0x07,       0x17,       24,         0,          0x18,
    25,         0,          0x28,       23,         0,          0x37,
    22,         0,          0x67,       19,         0,          0x68,
    20,         0,          0x6c,       21,         0,          54,
    0x12,       1984 % 256, 1984 / 256, 0x13,       2048 % 256, 2048 / 256,
    0x14,       2112 % 256, 2112 / 256, 0x15,       2176 % 256, 2176 / 256,
    0x16,       2240 % 256, 2240 / 256, 0x17,       2304 % 256, 2304 / 256,
    0x1c,       2368 % 256, 2368 / 256, 0x1d,       2432 % 256, 2432 / 256,
    0x1e,       2496 % 256, 2496 / 256, 0x1f,       2560 % 256, 2560 / 256,
    0x24,       52,         0,          0x27,       55,         0,
    0x28,       56,         0,          0x2b,       59,         0,
    0x2c,       60,         0,          0x33,       320 % 256,  320 / 256,
    0x34,       384 % 256,  384 / 256,  0x35,       448 % 256,  448 / 256,
    0x37,       53,         0,          0x38,       54,         0,
    0x52,       50,         0,          0x53,       51,         0,
    0x54,       44,         0,          0x55,       45,         0,
    0x56,       46,         0,          0x57,       47,         0,
    0x58,       57,         0,          0x59,       58,         0,
    0x5a,       61,         0,          0x5b,       256 % 256,  256 / 256,
    0x64,       48,         0,          0x65,       49,         0,
    0x66,       62,         0,          0x67,       63,         0,
    0x68,       30,         0,          0x69,       31,         0,
    0x6a,       32,         0,          0x6b,       33,         0,
    0x6c,       40,         0,          0x6d,       41,         0,
    0xc8,       128,        0,          0xc9,       192,        0,
    0xca,       26,         0,          0xcb,       27,         0,
    0xcc,       28,         0,          0xcd,       29,         0,
    0xd2,       34,         0,          0xd3,       35,         0,
    0xd4,       36,         0,          0xd5,       37,         0,
    0xd6,       38,         0,          0xd7,       39,         0,
    0xda,       42,         0,          0xdb,       43,         0,
    20,         0x4a,       640 % 256,  640 / 256,  0x4b,       704 % 256,
    704 / 256,  0x4c,       768 % 256,  768 / 256,  0x4d,       832 % 256,
    832 / 256,  0x52,       1280 % 256, 1280 / 256, 0x53,       1344 % 256,
    1344 / 256, 0x54,       1408 % 256, 1408 / 256, 0x55,       1472 % 256,
    1472 / 256, 0x5a,       1536 % 256, 1536 / 256, 0x5b,       1600 % 256,
    1600 / 256, 0x64,       1664 % 256, 1664 / 256, 0x65,       1728 % 256,
    1728 / 256, 0x6c,       512 % 256,  512 / 256,  0x6d,       576 % 256,
    576 / 256,  0x72,       896 % 256,  896 / 256,  0x73,       960 % 256,
    960 / 256,  0x74,       1024 % 256, 1024 / 256, 0x75,       1088 % 256,
    1088 / 256, 0x76,       1152 % 256, 1152 / 256, 0x77,       1216 % 256,
    1216 / 256, 0xff};

const uint8_t FaxWhiteRunIns[] = {
    0,          0,          0,          6,          0x07,       2,
    0,          0x08,       3,          0,          0x0B,       4,
    0,          0x0C,       5,          0,          0x0E,       6,
    0,          0x0F,       7,          0,          6,          0x07,
    10,         0,          0x08,       11,         0,          0x12,
    128,        0,          0x13,       8,          0,          0x14,
    9,          0,          0x1b,       64,         0,          9,
    0x03,       13,         0,          0x07,       1,          0,
    0x08,       12,         0,          0x17,       192,        0,
    0x18,       1664 % 256, 1664 / 256, 0x2a,       16,         0,
    0x2B,       17,         0,          0x34,       14,         0,
    0x35,       15,         0,          12,         0x03,       22,
    0,          0x04,       23,         0,          0x08,       20,
    0,          0x0c,       19,         0,          0x13,       26,
    0,          0x17,       21,         0,          0x18,       28,
    0,          0x24,       27,         0,          0x27,       18,
    0,          0x28,       24,         0,          0x2B,       25,
    0,          0x37,       256 % 256,  256 / 256,  42,         0x02,
    29,         0,          0x03,       30,         0,          0x04,
    45,         0,          0x05,       46,         0,          0x0a,
    47,         0,          0x0b,       48,         0,          0x12,
    33,         0,          0x13,       34,         0,          0x14,
    35,         0,          0x15,       36,         0,          0x16,
    37,         0,          0x17,       38,         0,          0x1a,
    31,         0,          0x1b,       32,         0,          0x24,
    53,         0,          0x25,       54,         0,          0x28,
    39,         0,          0x29,       40,         0,          0x2a,
    41,         0,          0x2b,       42,         0,          0x2c,
    43,         0,          0x2d,       44,         0,          0x32,
    61,         0,          0x33,       62,         0,          0x34,
    63,         0,          0x35,       0,          0,          0x36,
    320 % 256,  320 / 256,  0x37,       384 % 256,  384 / 256,  0x4a,
    59,         0,          0x4b,       60,         0,          0x52,
    49,         0,          0x53,       50,         0,          0x54,
    51,         0,          0x55,       52,         0,          0x58,
    55,         0,          0x59,       56,         0,          0x5a,
    57,         0,          0x5b,       58,         0,          0x64,
    448 % 256,  448 / 256,  0x65,       512 % 256,  512 / 256,  0x67,
    640 % 256,  640 / 256,  0x68,       576 % 256,  576 / 256,  16,
    0x98,       1472 % 256, 1472 / 256, 0x99,       1536 % 256, 1536 / 256,
    0x9a,       1600 % 256, 1600 / 256, 0x9b,       1728 % 256, 1728 / 256,
    0xcc,       704 % 256,  704 / 256,  0xcd,       768 % 256,  768 / 256,
    0xd2,       832 % 256,  832 / 256,  0xd3,       896 % 256,  896 / 256,
    0xd4,       960 % 256,  960 / 256,  0xd5,       1024 % 256, 1024 / 256,
    0xd6,       1088 % 256, 1088 / 256, 0xd7,       1152 % 256, 1152 / 256,
    0xd8,       1216 % 256, 1216 / 256, 0xd9,       1280 % 256, 1280 / 256,
    0xda,       1344 % 256, 1344 / 256, 0xdb,       1408 % 256, 1408 / 256,
    0,          3,          0x08,       1792 % 256, 1792 / 256, 0x0c,
    1856 % 256, 1856 / 256, 0x0d,       1920 % 256, 1920 / 256, 10,
    0x12,       1984 % 256, 1984 / 256, 0x13,       2048 % 256, 2048 / 256,
    0x14,       2112 % 256, 2112 / 256, 0x15,       2176 % 256, 2176 / 256,
    0x16,       2240 % 256, 2240 / 256, 0x17,       2304 % 256, 2304 / 256,
    0x1c,       2368 % 256, 2368 / 256, 0x1d,       2432 % 256, 2432 / 256,
    0x1e,       2496 % 256, 2496 / 256, 0x1f,       2560 % 256, 2560 / 256,
    0xff,
};

int FaxGetRun(const uint8_t* ins_array,
              const uint8_t* src_buf,
              int& bitpos,
              int bitsize) {
  uint32_t code = 0;
  int ins_off = 0;
  while (1) {
    uint8_t ins = ins_array[ins_off++];
    if (ins == 0xff) {
      return -1;
    }
    if (bitpos >= bitsize) {
      return -1;
    }
    code <<= 1;
    if (src_buf[bitpos / 8] & (1 << (7 - bitpos % 8))) {
      code++;
    }
    bitpos++;
    int next_off = ins_off + ins * 3;
    for (; ins_off < next_off; ins_off += 3) {
      if (ins_array[ins_off] == code) {
        return ins_array[ins_off + 1] + ins_array[ins_off + 2] * 256;
      }
    }
  }
}

FX_BOOL FaxG4GetRow(const uint8_t* src_buf,
                    int bitsize,
                    int& bitpos,
                    uint8_t* dest_buf,
                    const uint8_t* ref_buf,
                    int columns) {
  int a0 = -1;
  bool a0color = true;
  while (1) {
    if (bitpos >= bitsize) {
      return FALSE;
    }
    int a1, a2, b1, b2;
    FaxG4FindB1B2(ref_buf, columns, a0, a0color, b1, b2);
    FX_BOOL bit = NEXTBIT;
    int v_delta = 0;
    if (bit) {
    } else {
      if (bitpos >= bitsize) {
        return FALSE;
      }
      FX_BOOL bit1 = NEXTBIT;
      if (bitpos >= bitsize) {
        return FALSE;
      }
      FX_BOOL bit2 = NEXTBIT;
      if (bit1 && bit2) {
        v_delta = 1;
      } else if (bit1) {
        v_delta = -1;
      } else if (bit2) {
        int run_len1 = 0;
        while (1) {
          int run = FaxGetRun(a0color ? FaxWhiteRunIns : FaxBlackRunIns,
                              src_buf, bitpos, bitsize);
          run_len1 += run;
          if (run < 64) {
            break;
          }
        }
        if (a0 < 0) {
          run_len1++;
        }
        a1 = a0 + run_len1;
        if (!a0color) {
          FaxFillBits(dest_buf, columns, a0, a1);
        }
        int run_len2 = 0;
        while (1) {
          int run = FaxGetRun(a0color ? FaxBlackRunIns : FaxWhiteRunIns,
                              src_buf, bitpos, bitsize);
          run_len2 += run;
          if (run < 64) {
            break;
          }
        }
        a2 = a1 + run_len2;
        if (a0color) {
          FaxFillBits(dest_buf, columns, a1, a2);
        }
        a0 = a2;
        if (a0 < columns) {
          continue;
        }
        return TRUE;
      } else {
        if (bitpos >= bitsize) {
          return FALSE;
        }
        bit = NEXTBIT;
        if (bit) {
          if (!a0color) {
            FaxFillBits(dest_buf, columns, a0, b2);
          }
          if (b2 >= columns) {
            return TRUE;
          }
          a0 = b2;
          continue;
        } else {
          if (bitpos >= bitsize) {
            return FALSE;
          }
          FX_BOOL next_bit1 = NEXTBIT;
          if (bitpos >= bitsize) {
            return FALSE;
          }
          FX_BOOL next_bit2 = NEXTBIT;
          if (next_bit1 && next_bit2) {
            v_delta = 2;
          } else if (next_bit1) {
            v_delta = -2;
          } else if (next_bit2) {
            if (bitpos >= bitsize) {
              return FALSE;
            }
            bit = NEXTBIT;
            if (bit) {
              v_delta = 3;
            } else {
              v_delta = -3;
            }
          } else {
            if (bitpos >= bitsize) {
              return FALSE;
            }
            bit = NEXTBIT;
            if (bit) {
              bitpos += 3;
              continue;
            } else {
              bitpos += 5;
              return TRUE;
            }
          }
        }
      }
    }
    a1 = b1 + v_delta;
    if (!a0color) {
      FaxFillBits(dest_buf, columns, a0, a1);
    }
    if (a1 >= columns) {
      return TRUE;
    }
    a0 = a1;
    a0color = !a0color;
  }
}

FX_BOOL FaxSkipEOL(const uint8_t* src_buf, int bitsize, int& bitpos) {
  int startbit = bitpos;
  while (bitpos < bitsize) {
    int bit = NEXTBIT;
    if (bit) {
      if (bitpos - startbit <= 11) {
        bitpos = startbit;
      }
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL FaxGet1DLine(const uint8_t* src_buf,
                     int bitsize,
                     int& bitpos,
                     uint8_t* dest_buf,
                     int columns) {
  bool color = true;
  int startpos = 0;
  while (1) {
    if (bitpos >= bitsize) {
      return FALSE;
    }
    int run_len = 0;
    while (1) {
      int run = FaxGetRun(color ? FaxWhiteRunIns : FaxBlackRunIns, src_buf,
                          bitpos, bitsize);
      if (run < 0) {
        while (bitpos < bitsize) {
          int bit = NEXTBIT;
          if (bit) {
            return TRUE;
          }
        }
        return FALSE;
      }
      run_len += run;
      if (run < 64) {
        break;
      }
    }
    if (!color) {
      FaxFillBits(dest_buf, columns, startpos, startpos + run_len);
    }
    startpos += run_len;
    if (startpos >= columns) {
      break;
    }
    color = !color;
  }
  return TRUE;
}

}  // namespace

class CCodec_FaxDecoder : public CCodec_ScanlineDecoder {
 public:
  CCodec_FaxDecoder(const uint8_t* src_buf,
                    uint32_t src_size,
                    int width,
                    int height,
                    int K,
                    FX_BOOL EndOfLine,
                    FX_BOOL EncodedByteAlign,
                    FX_BOOL BlackIs1,
                    int Columns,
                    int Rows);
  ~CCodec_FaxDecoder() override;


  // CCodec_ScanlineDecoder
  FX_BOOL v_Rewind() override;
  uint8_t* v_GetNextLine() override;
  uint32_t GetSrcOffset() override;

  int m_Encoding, m_bEndOfLine, m_bByteAlign, m_bBlack;
  int bitpos;
  const uint8_t* m_pSrcBuf;
  uint32_t m_SrcSize;
  uint8_t* m_pScanlineBuf;
  uint8_t* m_pRefBuf;
};

CCodec_FaxDecoder::CCodec_FaxDecoder(const uint8_t* src_buf,
                                     uint32_t src_size,
                                     int width,
                                     int height,
                                     int K,
                                     FX_BOOL EndOfLine,
                                     FX_BOOL EncodedByteAlign,
                                     FX_BOOL BlackIs1,
                                     int Columns,
                                     int Rows) {
  m_Encoding = K;
  m_bEndOfLine = EndOfLine;
  m_bByteAlign = EncodedByteAlign;
  m_bBlack = BlackIs1;
  m_OrigWidth = Columns;
  m_OrigHeight = Rows;
  if (m_OrigWidth == 0)
    m_OrigWidth = width;
  if (m_OrigHeight == 0)
    m_OrigHeight = height;
  // Should not overflow. Checked by FPDFAPI_CreateFaxDecoder.
  m_Pitch = (static_cast<uint32_t>(m_OrigWidth) + 31) / 32 * 4;
  m_OutputWidth = m_OrigWidth;
  m_OutputHeight = m_OrigHeight;
  m_pScanlineBuf = FX_Alloc(uint8_t, m_Pitch);
  m_pRefBuf = FX_Alloc(uint8_t, m_Pitch);
  m_pSrcBuf = src_buf;
  m_SrcSize = src_size;
  m_nComps = 1;
  m_bpc = 1;
}

CCodec_FaxDecoder::~CCodec_FaxDecoder() {
  FX_Free(m_pScanlineBuf);
  FX_Free(m_pRefBuf);
}

FX_BOOL CCodec_FaxDecoder::v_Rewind() {
  FXSYS_memset(m_pRefBuf, 0xff, m_Pitch);
  bitpos = 0;
  return TRUE;
}
uint8_t* CCodec_FaxDecoder::v_GetNextLine() {
  int bitsize = m_SrcSize * 8;
  FaxSkipEOL(m_pSrcBuf, bitsize, bitpos);
  if (bitpos >= bitsize) {
    return nullptr;
  }
  FXSYS_memset(m_pScanlineBuf, 0xff, m_Pitch);
  if (m_Encoding < 0) {
    FaxG4GetRow(m_pSrcBuf, bitsize, bitpos, m_pScanlineBuf, m_pRefBuf,
                m_OrigWidth);
    FXSYS_memcpy(m_pRefBuf, m_pScanlineBuf, m_Pitch);
  } else if (m_Encoding == 0) {
    FaxGet1DLine(m_pSrcBuf, bitsize, bitpos, m_pScanlineBuf, m_OrigWidth);
  } else {
    FX_BOOL bNext1D = m_pSrcBuf[bitpos / 8] & (1 << (7 - bitpos % 8));
    bitpos++;
    if (bNext1D) {
      FaxGet1DLine(m_pSrcBuf, bitsize, bitpos, m_pScanlineBuf, m_OrigWidth);
    } else {
      FaxG4GetRow(m_pSrcBuf, bitsize, bitpos, m_pScanlineBuf, m_pRefBuf,
                  m_OrigWidth);
    }
    FXSYS_memcpy(m_pRefBuf, m_pScanlineBuf, m_Pitch);
  }
  if (m_bEndOfLine) {
    FaxSkipEOL(m_pSrcBuf, bitsize, bitpos);
  }
  if (m_bByteAlign && bitpos < bitsize) {
    int bitpos0 = bitpos;
    int bitpos1 = (bitpos + 7) / 8 * 8;
    while (m_bByteAlign && bitpos0 < bitpos1) {
      int bit = m_pSrcBuf[bitpos0 / 8] & (1 << (7 - bitpos0 % 8));
      if (bit != 0) {
        m_bByteAlign = FALSE;
      } else {
        bitpos0++;
      }
    }
    if (m_bByteAlign) {
      bitpos = bitpos1;
    }
  }
  if (m_bBlack) {
    for (uint32_t i = 0; i < m_Pitch; i++) {
      m_pScanlineBuf[i] = ~m_pScanlineBuf[i];
    }
  }
  return m_pScanlineBuf;
}
uint32_t CCodec_FaxDecoder::GetSrcOffset() {
  uint32_t ret = (bitpos + 7) / 8;
  if (ret > m_SrcSize) {
    ret = m_SrcSize;
  }
  return ret;
}

void FaxG4Decode(const uint8_t* src_buf,
                 uint32_t src_size,
                 int* pbitpos,
                 uint8_t* dest_buf,
                 int width,
                 int height,
                 int pitch) {
  if (pitch == 0) {
    pitch = (width + 7) / 8;
  }
  uint8_t* ref_buf = FX_Alloc(uint8_t, pitch);
  FXSYS_memset(ref_buf, 0xff, pitch);
  int bitpos = *pbitpos;
  for (int iRow = 0; iRow < height; iRow++) {
    uint8_t* line_buf = dest_buf + iRow * pitch;
    FXSYS_memset(line_buf, 0xff, pitch);
    FaxG4GetRow(src_buf, src_size << 3, bitpos, line_buf, ref_buf, width);
    FXSYS_memcpy(ref_buf, line_buf, pitch);
  }
  FX_Free(ref_buf);
  *pbitpos = bitpos;
}

CCodec_ScanlineDecoder* CCodec_FaxModule::CreateDecoder(
    const uint8_t* src_buf,
    uint32_t src_size,
    int width,
    int height,
    int K,
    FX_BOOL EndOfLine,
    FX_BOOL EncodedByteAlign,
    FX_BOOL BlackIs1,
    int Columns,
    int Rows) {
  return new CCodec_FaxDecoder(src_buf, src_size, width, height, K, EndOfLine,
                               EncodedByteAlign, BlackIs1, Columns, Rows);
}
