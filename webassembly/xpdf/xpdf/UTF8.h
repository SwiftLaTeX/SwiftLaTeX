//========================================================================
//
// UTF8.h
//
// Copyright 2001-2017 Glyph & Cog, LLC
//
//========================================================================

#ifndef UTF8_H
#define UTF8_H

#include <aconf.h>

#include "gtypes.h"
#include "GString.h"
#include "CharTypes.h"

// Convert [u] to UTF-8 in [buf].  Returns the number of bytes written
// to [buf].  If [u] requires more then [bufSize] bytes in UTF-8,
// writes nothing and returns 0.
extern int mapUTF8(Unicode u, char *buf, int bufSize);

// Convert [u] to UCS-2BE in [buf].  Returns the number of bytes
// written to [buf].  If [u] requires more then [bufSize] bytes in
// UCS-2, writes nothing and returns 0.
extern int mapUCS2(Unicode u, char *buf, int bufSize);

// Parse one UTF-8 character from [s], starting at *[i].  Writes the
// character to *[u], updates *[i] to point to the next available byte
// in [s], and returns true.  At end of string: writes nothing to *[u]
// or *[i] and returns false.  For an invalid UTF-8 character: sets
// *[u] to the next byte, advances *[i] by one (to avoid infinite
// loops), and returns true.
extern GBool getUTF8(GString *s, int *i, Unicode *u);

// Same as getUTF8, but for UTF-16BE.
extern GBool getUTF16BE(GString *s, int *i, Unicode *u);

// Same as getUTF8, but for UTF-16LE.
extern GBool getUTF16LE(GString *s, int *i, Unicode *u);

#endif
