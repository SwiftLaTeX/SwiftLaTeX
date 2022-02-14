//========================================================================
//
// gfile.h
//
// Miscellaneous file and directory name manipulation.
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef GFILE_H
#define GFILE_H

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#if defined(_WIN32)
#  include <sys/stat.h>
#  ifdef FPTEX
#    include <win32lib.h>
#  else
#    include <windows.h>
#  endif
#elif defined(ACORN)
#elif defined(ANDROID)
#else
#  include <unistd.h>
#  include <sys/types.h>
#endif
#include "gtypes.h"

class GString;

//------------------------------------------------------------------------

// Get home directory path.
extern GString *getHomeDir();

// Get current directory.
extern GString *getCurrentDir();

// Append a file name to a path string.  <path> may be an empty
// string, denoting the current directory).  Returns <path>.
extern GString *appendToPath(GString *path, const char *fileName);

// Grab the path from the front of the file name.  If there is no
// directory component in <fileName>, returns an empty string.
extern GString *grabPath(char *fileName);

// Is this an absolute path or file name?
extern GBool isAbsolutePath(char *path);

// Make this path absolute by prepending current directory (if path is
// relative) or prepending user's directory (if path starts with '~').
extern GString *makePathAbsolute(GString *path);

// Get the modification time for <fileName>.  Returns 0 if there is an
// error.
extern time_t getModTime(char *fileName);

// Create a temporary file and open it for writing.  If <ext> is not
// NULL, it will be used as the file name extension.  Returns both the
// name and the file pointer.  For security reasons, all writing
// should be done to the returned file pointer; the file may be
// reopened later for reading, but not for writing.  The <mode> string
// should be "w" or "wb".  Returns true on success.
extern GBool openTempFile(GString **name, FILE **f,
			  const char *mode, const char *ext);

// Create a directory.  Returns true on success.
extern GBool createDir(char *path, int mode);

// Execute <command>.  Returns true on success.
extern GBool executeCommand(char *cmd);

#ifdef _WIN32
// Convert a file name from Latin-1 to UTF-8.
extern GString *fileNameToUTF8(char *path);

// Convert a file name from UCS-2 to UTF-8.
extern GString *fileNameToUTF8(wchar_t *path);
#endif

// Open a file.  On Windows, this converts the path from UTF-8 to
// UCS-2 and calls _wfopen (if available).  On other OSes, this simply
// calls fopen.
extern FILE *openFile(const char *path, const char *mode);

// Just like fgets, but handles Unix, Mac, and/or DOS end-of-line
// conventions.
extern char *getLine(char *buf, int size, FILE *f);

// Type used by gfseek/gftell for file offsets.  This will be 64 bits
// on systems that support it.
#if HAVE_FSEEKO
typedef off_t GFileOffset;
#define GFILEOFFSET_MAX 0x7fffffffffffffffLL
#elif HAVE_FSEEK64
typedef long long GFileOffset;
#define GFILEOFFSET_MAX 0x7fffffffffffffffLL
#elif HAVE_FSEEKI64
typedef __int64 GFileOffset;
#define GFILEOFFSET_MAX 0x7fffffffffffffffLL
#else
typedef long GFileOffset;
#define GFILEOFFSET_MAX LONG_MAX
#endif

// Like fseek, but uses a 64-bit file offset if available.
extern int gfseek(FILE *f, GFileOffset offset, int whence);

// Like ftell, but returns a 64-bit file offset if available.
extern GFileOffset gftell(FILE *f);

// On Windows, this gets the Unicode command line and converts it to
// UTF-8.  On other systems, this is a nop.
extern void fixCommandLine(int *argc, char **argv[]);

#endif
