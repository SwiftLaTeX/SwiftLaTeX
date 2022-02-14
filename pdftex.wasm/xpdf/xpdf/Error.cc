//========================================================================
//
// Error.cc
//
// Copyright 1996-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include "gmempp.h"
#include "GString.h"
#include "GlobalParams.h"
#include "Error.h"

const char *errorCategoryNames[] = {
  "Syntax Warning",
  "Syntax Error",
  "Config Error",
  "Command Line Error",
  "I/O Error",
  "Permission Error",
  "Unimplemented Feature",
  "Internal Error"
};

static void (*errorCbk)(void *data, ErrorCategory category,
			int pos, char *msg) = NULL;
static void *errorCbkData = NULL;

void setErrorCallback(void (*cbk)(void *data, ErrorCategory category,
				  int pos, char *msg),
		      void *data) {
  errorCbk = cbk;
  errorCbkData = data;
}

void *getErrorCallbackData() {
  return errorCbkData;
}

void CDECL error(ErrorCategory category, GFileOffset pos,
		 const char *msg, ...) {
  va_list args;
  GString *s, *sanitized;
  char c;
  int i;

  // NB: this can be called before the globalParams object is created
  if (!errorCbk && globalParams && globalParams->getErrQuiet()) {
    return;
  }
  va_start(args, msg);
  s = GString::formatv(msg, args);
  va_end(args);

  // remove non-printable characters, just in case they might cause
  // problems for the terminal program
  sanitized = new GString();
  for (i = 0; i < s->getLength(); ++i) {
    c = s->getChar(i);
    if (c >= 0x20 && c <= 0x7e) {
      sanitized->append(c);
    } else {
      sanitized->appendf("<{0:02x}>", c & 0xff);
    }
  }

  if (errorCbk) {
    (*errorCbk)(errorCbkData, category, (int)pos, sanitized->getCString());
  } else {
    fflush(stdout);
    if (pos >= 0) {
      fprintf(stderr, "%s (%d): %s\n",
	      errorCategoryNames[category], (int)pos, sanitized->getCString());
    } else {
      fprintf(stderr, "%s: %s\n",
	      errorCategoryNames[category], sanitized->getCString());
    }
    fflush(stderr);
  }

  delete s;
  delete sanitized;
}
