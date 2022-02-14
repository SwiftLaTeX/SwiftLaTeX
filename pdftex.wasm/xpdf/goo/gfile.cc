//========================================================================
//
// gfile.cc
//
// Miscellaneous file and directory name manipulation.
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef _WIN32
#  undef WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <time.h>
#  include <direct.h>
#else
#  if !defined(ACORN)
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <fcntl.h>
#  endif
#  include <time.h>
#  include <limits.h>
#  include <string.h>
#  if !defined(VMS) && !defined(ACORN)
#    include <pwd.h>
#  endif
#  if defined(VMS) && (__DECCXX_VER < 50200000)
#    include <unixlib.h>
#  endif
#endif // _WIN32
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "gfile.h"

// Some systems don't define this, so just make it something reasonably
// large.
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

//------------------------------------------------------------------------

GString *getHomeDir() {
#ifdef VMS
  //---------- VMS ----------
  return new GString("SYS$LOGIN:");

#elif defined(__EMX__) || defined(_WIN32)
  //---------- OS/2+EMX and Win32 ----------
  char *s;
  GString *ret;

  if ((s = getenv("HOME")))
    ret = new GString(s);
  else
    ret = new GString(".");
  return ret;

#elif defined(ACORN)
  //---------- RISCOS ----------
  return new GString("@");

#else
  //---------- Unix ----------
  char *s;
  struct passwd *pw;
  GString *ret;

  if ((s = getenv("HOME"))) {
    ret = new GString(s);
  } else {
    if ((s = getenv("USER")))
      pw = getpwnam(s);
    else
      pw = getpwuid(getuid());
    if (pw)
      ret = new GString(pw->pw_dir);
    else
      ret = new GString(".");
  }
  return ret;
#endif
}

GString *getCurrentDir() {
  char buf[PATH_MAX+1];

#if defined(__EMX__)
  if (_getcwd2(buf, sizeof(buf)))
#elif defined(_WIN32)
  if (GetCurrentDirectoryA(sizeof(buf), buf))
#elif defined(ACORN)
  if (strcpy(buf, "@"))
#else
  if (getcwd(buf, sizeof(buf)))
#endif
    return new GString(buf);
  return new GString();
}

GString *appendToPath(GString *path, const char *fileName) {
#if defined(VMS)
  //---------- VMS ----------
  //~ this should handle everything necessary for file
  //~ requesters, but it's certainly not complete
  char *p0, *p1, *p2;
  char *q1;

  p0 = path->getCString();
  p1 = p0 + path->getLength() - 1;
  if (!strcmp(fileName, "-")) {
    if (*p1 == ']') {
      for (p2 = p1; p2 > p0 && *p2 != '.' && *p2 != '['; --p2) ;
      if (*p2 == '[')
	++p2;
      path->del(p2 - p0, p1 - p2);
    } else if (*p1 == ':') {
      path->append("[-]");
    } else {
      path->clear();
      path->append("[-]");
    }
  } else if ((q1 = strrchr(fileName, '.')) && !strncmp(q1, ".DIR;", 5)) {
    if (*p1 == ']') {
      path->insert(p1 - p0, '.');
      path->insert(p1 - p0 + 1, fileName, q1 - fileName);
    } else if (*p1 == ':') {
      path->append('[');
      path->append(']');
      path->append(fileName, q1 - fileName);
    } else {
      path->clear();
      path->append(fileName, q1 - fileName);
    }
  } else {
    if (*p1 != ']' && *p1 != ':')
      path->clear();
    path->append(fileName);
  }
  return path;

#elif defined(_WIN32)
  //---------- Win32 ----------
  GString *tmp;
  char buf[256];
  char *fp;

  tmp = new GString(path);
  tmp->append('/');
  tmp->append(fileName);
  GetFullPathNameA(tmp->getCString(), sizeof(buf), buf, &fp);
  delete tmp;
  path->clear();
  path->append(buf);
  return path;

#elif defined(ACORN)
  //---------- RISCOS ----------
  char *p;
  int i;

  path->append(".");
  i = path->getLength();
  path->append(fileName);
  for (p = path->getCString() + i; *p; ++p) {
    if (*p == '/') {
      *p = '.';
    } else if (*p == '.') {
      *p = '/';
    }
  }
  return path;

#elif defined(__EMX__)
  //---------- OS/2+EMX ----------
  int i;

  // appending "." does nothing
  if (!strcmp(fileName, "."))
    return path;

  // appending ".." goes up one directory
  if (!strcmp(fileName, "..")) {
    for (i = path->getLength() - 2; i >= 0; --i) {
      if (path->getChar(i) == '/' || path->getChar(i) == '\\' ||
	  path->getChar(i) == ':')
	break;
    }
    if (i <= 0) {
      if (path->getChar(0) == '/' || path->getChar(0) == '\\') {
	path->del(1, path->getLength() - 1);
      } else if (path->getLength() >= 2 && path->getChar(1) == ':') {
	path->del(2, path->getLength() - 2);
      } else {
	path->clear();
	path->append("..");
      }
    } else {
      if (path->getChar(i-1) == ':')
	++i;
      path->del(i, path->getLength() - i);
    }
    return path;
  }

  // otherwise, append "/" and new path component
  if (path->getLength() > 0 &&
      path->getChar(path->getLength() - 1) != '/' &&
      path->getChar(path->getLength() - 1) != '\\')
    path->append('/');
  path->append(fileName);
  return path;

#else
  //---------- Unix ----------
  int i;

  // appending "." does nothing
  if (!strcmp(fileName, "."))
    return path;

  // appending ".." goes up one directory
  if (!strcmp(fileName, "..")) {
    for (i = path->getLength() - 2; i >= 0; --i) {
      if (path->getChar(i) == '/')
	break;
    }
    if (i <= 0) {
      if (path->getChar(0) == '/') {
	path->del(1, path->getLength() - 1);
      } else {
	path->clear();
	path->append("..");
      }
    } else {
      path->del(i, path->getLength() - i);
    }
    return path;
  }

  // otherwise, append "/" and new path component
  if (path->getLength() > 0 &&
      path->getChar(path->getLength() - 1) != '/')
    path->append('/');
  path->append(fileName);
  return path;
#endif
}

GString *grabPath(char *fileName) {
#ifdef VMS
  //---------- VMS ----------
  char *p;

  if ((p = strrchr(fileName, ']')))
    return new GString(fileName, p + 1 - fileName);
  if ((p = strrchr(fileName, ':')))
    return new GString(fileName, p + 1 - fileName);
  return new GString();

#elif defined(__EMX__) || defined(_WIN32)
  //---------- OS/2+EMX and Win32 ----------
  char *p;

  if ((p = strrchr(fileName, '/')))
    return new GString(fileName, (int)(p - fileName));
  if ((p = strrchr(fileName, '\\')))
    return new GString(fileName, (int)(p - fileName));
  if ((p = strrchr(fileName, ':')))
    return new GString(fileName, (int)(p + 1 - fileName));
  return new GString();

#elif defined(ACORN)
  //---------- RISCOS ----------
  char *p;

  if ((p = strrchr(fileName, '.')))
    return new GString(fileName, p - fileName);
  return new GString();

#else
  //---------- Unix ----------
  char *p;

  if ((p = strrchr(fileName, '/')))
    return new GString(fileName, (int)(p - fileName));
  return new GString();
#endif
}

GBool isAbsolutePath(char *path) {
#ifdef VMS
  //---------- VMS ----------
  return strchr(path, ':') ||
	 (path[0] == '[' && path[1] != '.' && path[1] != '-');

#elif defined(__EMX__) || defined(_WIN32)
  //---------- OS/2+EMX and Win32 ----------
  return path[0] == '/' || path[0] == '\\' || path[1] == ':';

#elif defined(ACORN)
  //---------- RISCOS ----------
  return path[0] == '$';

#else
  //---------- Unix ----------
  return path[0] == '/';
#endif
}

GString *makePathAbsolute(GString *path) {
#ifdef VMS
  //---------- VMS ----------
  char buf[PATH_MAX+1];

  if (!isAbsolutePath(path->getCString())) {
    if (getcwd(buf, sizeof(buf))) {
      path->insert(0, buf);
    }
  }
  return path;

#elif defined(_WIN32)
  //---------- Win32 ----------
  char buf[_MAX_PATH];
  char *fp;

  buf[0] = '\0';
  if (!GetFullPathNameA(path->getCString(), _MAX_PATH, buf, &fp)) {
    path->clear();
    return path;
  }
  path->clear();
  path->append(buf);
  return path;

#elif defined(ACORN)
  //---------- RISCOS ----------
  path->insert(0, '@');
  return path;

#else
  //---------- Unix and OS/2+EMX ----------
  struct passwd *pw;
  char buf[PATH_MAX+1];
  GString *s;
  char *p1, *p2;
  int n;

  if (path->getChar(0) == '~') {
    if (path->getChar(1) == '/' ||
#ifdef __EMX__
	path->getChar(1) == '\\' ||
#endif
	path->getLength() == 1) {
      path->del(0, 1);
      s = getHomeDir();
      path->insert(0, s);
      delete s;
    } else {
      p1 = path->getCString() + 1;
#ifdef __EMX__
      for (p2 = p1; *p2 && *p2 != '/' && *p2 != '\\'; ++p2) ;
#else
      for (p2 = p1; *p2 && *p2 != '/'; ++p2) ;
#endif
      if ((n = (int)(p2 - p1)) > PATH_MAX)
	n = PATH_MAX;
      strncpy(buf, p1, n);
      buf[n] = '\0';
      if ((pw = getpwnam(buf))) {
	path->del(0, (int)(p2 - p1 + 1));
	path->insert(0, pw->pw_dir);
      }
    }
  } else if (!isAbsolutePath(path->getCString())) {
    if (getcwd(buf, sizeof(buf))) {
#ifndef __EMX__
      path->insert(0, '/');
#endif
      path->insert(0, buf);
    }
  }
  return path;
#endif
}

time_t getModTime(char *fileName) {
#ifdef _WIN32
  //~ should implement this, but it's (currently) only used in xpdf
  return 0;
#else
  struct stat statBuf;

  if (stat(fileName, &statBuf)) {
    return 0;
  }
  return statBuf.st_mtime;
#endif
}

GBool openTempFile(GString **name, FILE **f,
		   const char *mode, const char *ext) {
#if defined(_WIN32)
  //---------- Win32 ----------
  char tempPath[MAX_PATH + 1];
  GString *s, *s2;
  FILE *f2;
  DWORD n;
  int t, i;

  // this has the standard race condition problem, but I haven't found
  // a better way to generate temp file names with extensions on
  // Windows
  n = GetTempPathA(sizeof(tempPath), tempPath);
  if (n > 0 && n <= sizeof(tempPath)) {
    s = new GString(tempPath);
    if (tempPath[n-1] != '\\') {
      s->append('\\');
    }
  } else {
    s = new GString(".\\");
  }
  s->appendf("xpdf_{0:d}_{1:d}_",
	     (int)GetCurrentProcessId(), (int)GetCurrentThreadId());
  t = (int)time(NULL);
  for (i = 0; i < 1000; ++i) {
    s2 = GString::format("{0:t}{1:d}", s, t + i);
    if (ext) {
      s2->append(ext);
    }
    if (!(f2 = fopen(s2->getCString(), "r"))) {
      if (!(f2 = fopen(s2->getCString(), mode))) {
	delete s2;
	delete s;
	return gFalse;
      }
      *name = s2;
      *f = f2;
      delete s;
      return gTrue;
    }
    fclose(f2);
    delete s2;
  }
  delete s;
  return gFalse;
#elif defined(VMS) || defined(__EMX__) || defined(ACORN)
  //---------- non-Unix ----------
  char *s;

  // There is a security hole here: an attacker can create a symlink
  // with this file name after the tmpnam call and before the fopen
  // call.  I will happily accept fixes to this function for non-Unix
  // OSs.
  if (!(s = tmpnam(NULL))) {
    return gFalse;
  }
  *name = new GString(s);
  if (ext) {
    (*name)->append(ext);
  }
  if (!(*f = fopen((*name)->getCString(), mode))) {
    delete (*name);
    *name = NULL;
    return gFalse;
  }
  return gTrue;
#else
  //---------- Unix ----------
  char *s;
  int fd;

  if (ext) {
#if HAVE_MKSTEMPS
    if ((s = getenv("TMPDIR"))) {
      *name = new GString(s);
    } else {
      *name = new GString("/tmp");
    }
    (*name)->append("/XXXXXX")->append(ext);
    fd = mkstemps((*name)->getCString(), (int)strlen(ext));
#else
    if (!(s = tmpnam(NULL))) {
      return gFalse;
    }
    *name = new GString(s);
    (*name)->append(ext);
    fd = open((*name)->getCString(), O_WRONLY | O_CREAT | O_EXCL, 0600);
#endif
  } else {
#if HAVE_MKSTEMP
    if ((s = getenv("TMPDIR"))) {
      *name = new GString(s);
    } else {
      *name = new GString("/tmp");
    }
    (*name)->append("/XXXXXX");
    fd = mkstemp((*name)->getCString());
#else // HAVE_MKSTEMP
    if (!(s = tmpnam(NULL))) {
      return gFalse;
    }
    *name = new GString(s);
    fd = open((*name)->getCString(), O_WRONLY | O_CREAT | O_EXCL, 0600);
#endif // HAVE_MKSTEMP
  }
  if (fd < 0 || !(*f = fdopen(fd, mode))) {
    delete *name;
    *name = NULL;
    return gFalse;
  }
  return gTrue;
#endif
}

GBool createDir(char *path, int mode) {
#ifdef _WIN32
  return !mkdir(path);
#else
  return !mkdir(path, mode);
#endif
}

GBool executeCommand(char *cmd) {
#ifdef VMS
  return system(cmd) ? gTrue : gFalse;
#else
  return system(cmd) ? gFalse : gTrue;
#endif
}

#ifdef _WIN32
GString *fileNameToUTF8(char *path) {
  GString *s;
  char *p;

  s = new GString();
  for (p = path; *p; ++p) {
    if (*p & 0x80) {
      s->append((char)(0xc0 | ((*p >> 6) & 0x03)));
      s->append((char)(0x80 | (*p & 0x3f)));
    } else {
      s->append(*p);
    }
  }
  return s;
}

GString *fileNameToUTF8(wchar_t *path) {
  GString *s;
  wchar_t *p;

  s = new GString();
  for (p = path; *p; ++p) {
    if (*p < 0x80) {
      s->append((char)*p);
    } else if (*p < 0x800) {
      s->append((char)(0xc0 | ((*p >> 6) & 0x1f)));
      s->append((char)(0x80 | (*p & 0x3f)));
    } else {
      s->append((char)(0xe0 | ((*p >> 12) & 0x0f)));
      s->append((char)(0x80 | ((*p >> 6) & 0x3f)));
      s->append((char)(0x80 | (*p & 0x3f)));
    }
  }
  return s;
}
#endif

FILE *openFile(const char *path, const char *mode) {
#if defined(_WIN32)
  OSVERSIONINFO version;
  wchar_t wPath[_MAX_PATH + 1];
  char nPath[_MAX_PATH + 1];
  wchar_t wMode[8];
  const char *p;
  int i;

  // NB: _wfopen is only available in NT
  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    for (p = path, i = 0; *p && i < _MAX_PATH; ++i) {
      if ((p[0] & 0xe0) == 0xc0 &&
	  p[1] && (p[1] & 0xc0) == 0x80) {
	wPath[i] = (wchar_t)(((p[0] & 0x1f) << 6) |
			     (p[1] & 0x3f));
	p += 2;
      } else if ((p[0] & 0xf0) == 0xe0 &&
		 p[1] && (p[1] & 0xc0) == 0x80 &&
		 p[2] && (p[2] & 0xc0) == 0x80) {
	wPath[i] = (wchar_t)(((p[0] & 0x0f) << 12) |
			     ((p[1] & 0x3f) << 6) |
			     (p[2] & 0x3f));
	p += 3;
      } else {
	wPath[i] = (wchar_t)(p[0] & 0xff);
	p += 1;
      }
    }
    wPath[i] = (wchar_t)0;
    for (i = 0; mode[i] && i < sizeof(wMode)/sizeof(wchar_t) - 1; ++i) {
      wMode[i] = (wchar_t)(mode[i] & 0xff);
    }
    wMode[i] = (wchar_t)0;
    return _wfopen(wPath, wMode);
  } else {
    for (p = path, i = 0; *p && i < _MAX_PATH; ++i) {
      if ((p[0] & 0xe0) == 0xc0 &&
	  p[1] && (p[1] & 0xc0) == 0x80) {
	nPath[i] = (char)(((p[0] & 0x1f) << 6) |
			  (p[1] & 0x3f));
	p += 2;
      } else if ((p[0] & 0xf0) == 0xe0 &&
		 p[1] && (p[1] & 0xc0) == 0x80 &&
		 p[2] && (p[2] & 0xc0) == 0x80) {
	nPath[i] = (char)(((p[1] & 0x3f) << 6) |
			  (p[2] & 0x3f));
	p += 3;
      } else {
	nPath[i] = p[0];
	p += 1;
      }
    }
    nPath[i] = '\0';
    return fopen(nPath, mode);
  }
#elif defined(VMS)
  return fopen(path, mode, "ctx=stm");
#else
  return fopen(path, mode);
#endif
}

char *getLine(char *buf, int size, FILE *f) {
  int c, i;

  i = 0;
  while (i < size - 1) {
    if ((c = fgetc(f)) == EOF) {
      break;
    }
    buf[i++] = (char)c;
    if (c == '\x0a') {
      break;
    }
    if (c == '\x0d') {
      c = fgetc(f);
      if (c == '\x0a' && i < size - 1) {
	buf[i++] = (char)c;
      } else if (c != EOF) {
	ungetc(c, f);
      }
      break;
    }
  }
  buf[i] = '\0';
  if (i == 0) {
    return NULL;
  }
  return buf;
}

int gfseek(FILE *f, GFileOffset offset, int whence) {
#if HAVE_FSEEKO
  return fseeko(f, offset, whence);
#elif HAVE_FSEEK64
  return fseek64(f, offset, whence);
#elif HAVE_FSEEKI64
  return _fseeki64(f, offset, whence);
#else
  return fseek(f, offset, whence);
#endif
}

GFileOffset gftell(FILE *f) {
#if HAVE_FSEEKO
  return ftello(f);
#elif HAVE_FSEEK64
  return ftell64(f);
#elif HAVE_FSEEKI64
  return _ftelli64(f);
#else
  return ftell(f);
#endif
}

void fixCommandLine(int *argc, char **argv[]) {
#ifdef _WIN32
  int argcw;
  wchar_t **argvw;
  GString *arg;
  int i;

  argvw = CommandLineToArgvW(GetCommandLineW(), &argcw);
  if (!argvw || argcw < 0) {
    return;
  }

  *argc = argcw;

  *argv = (char **)gmallocn(argcw + 1, sizeof(char *));
  for (i = 0; i < argcw; ++i) {
    arg = fileNameToUTF8(argvw[i]);
    (*argv)[i] = copyString(arg->getCString());
    delete arg;
  }
  (*argv)[argcw] = NULL;

  LocalFree(argvw);
#endif
}
