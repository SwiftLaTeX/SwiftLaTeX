#define EXTERN /* Instantiate data from pdftexd.h here.  */

#include <xetexd.h>

#include <errno.h>
#include <md5.h>
#include <setjmp.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "bibtex.h"
 
int ac;
char **av;

const char *ptexbanner = " (XeTeX WebAssembly 0.3.0)";
const char *DEFAULT_FMT_NAME = " swiftlatexxetex.fmt";
const char *DEFAULT_DUMP_NAME = "swiftlatexxetex";
string versionstring = " (XeTeX WebAssembly 0.3.0)";
#define MAXMAINFILENAME 512
char bootstrapcmd[MAXMAINFILENAME] = {0};
int exit_code;
jmp_buf jmpenv;

void topenin(void) {

  buffer[first] = 0;
  char *ptr = bootstrapcmd;
  int k = first;
  while (*ptr) {
    buffer[k++] = *(ptr++);
  }
  buffer[k++] = ' ';
  buffer[k] = 0;
  bootstrapcmd[0] = 0;
  for (last = first; buffer[last]; ++last) {
    
  }

  // if (optind < ac)
  // {
  //  int k = first;
  //  char *ptr = av[optind];
  //  while (*ptr) {
  //          buffer[k++] = *(ptr++);
  //  }
  //   buffer[k++] = ' ';
  //   buffer[k] = 0;
  //  ac = 0;
  //  for (last = first; buffer[last]; ++last);
  // }
}

void uexit(int code) {
  exit_code = code;
  longjmp(jmpenv, 1);
}

#ifndef WEBASSEMBLY_BUILD
int main(int argc, char **argv) {

  haltonerrorp = 1;
  ac = argc;
  av = argv;
  // Parse Argument
  int c;
  while ((c = getopt(ac, av, "io:")) != -1)
    switch (c) {
    case 'i':
      iniversion = 1;
      strncpy(bootstrapcmd, "*xelatex.ini", MAXMAINFILENAME);
      break;
    case 'o':
      output_directory = optarg;
      break;
  }
  
  if (iniversion != 1) {
    for (int index = optind; index < argc; index++) {
      strncpy(bootstrapcmd, argv[index], MAXMAINFILENAME);
      bootstrapcmd[MAXMAINFILENAME - 1] = 0;
      break;
    }
  }

  if (strlen(bootstrapcmd) == 0) {
    fprintf(stderr, "Usage: swiftlatex foo.tex\n");
    return -1;
  }

  dumpname = DEFAULT_DUMP_NAME;
  int fmtstrlen = strlen(DEFAULT_FMT_NAME);
  TEXformatdefault = xmalloc(fmtstrlen + 2);
  memcpy(TEXformatdefault, DEFAULT_FMT_NAME, fmtstrlen);
  formatdefaultlength = strlen(TEXformatdefault + 1);
  interactionoption = 1;
  filelineerrorstylep = 0;
  parsefirstlinep = 0;
  // Go
  if (setjmp(jmpenv) == 0)
    mainbody();

  return EXIT_SUCCESS;
}
#else

char main_entry_file[MAXMAINFILENAME];

int _compile() {
  haltonerrorp = 1;
  dumpname = DEFAULT_DUMP_NAME;
  int fmtstrlen = strlen(DEFAULT_FMT_NAME);
  TEXformatdefault = xmalloc(fmtstrlen + 2);
  memcpy(TEXformatdefault, DEFAULT_FMT_NAME, fmtstrlen);
  formatdefaultlength = strlen(TEXformatdefault + 1);
  interactionoption = 1;
  filelineerrorstylep = 0;
  parsefirstlinep = 0;
  // Go
  if (setjmp(jmpenv) == 0)
    mainbody();

  // printf("Compile stop with %d\n", exit_code);
  return exit_code;
}

int compileLaTeX() {
    nopdfoutput = 1;
    if (strlen(main_entry_file) == 0) {
      return -1;
    }
    strncpy(bootstrapcmd, main_entry_file, MAXMAINFILENAME);
    bootstrapcmd[MAXMAINFILENAME - 1] = 0;
    return _compile();
}

int compileFormat() {
    iniversion = 1;
    strncpy(bootstrapcmd, "*xelatex.ini", MAXMAINFILENAME);
    return _compile();
}

int compileBibtex() {
    char main_aux_file[MAXMAINFILENAME];
    strncpy(main_aux_file, main_entry_file, MAXMAINFILENAME);
    main_aux_file[MAXMAINFILENAME - 1] = 0;
    unsigned int s_len = strlen(main_aux_file);
    if (s_len < 3) {
      return -1;
    }
    main_aux_file[s_len - 1] = 'x';
    main_aux_file[s_len - 2] = 'u';
    main_aux_file[s_len - 3] = 'a';
    return bibtex_main(main_aux_file);
}

int setMainEntry(const char *p) {
    strncpy(main_entry_file, p, MAXMAINFILENAME);
    main_entry_file[MAXMAINFILENAME - 1] = 0;
    // fprintf(stderr,"setting main entry from c %s\n", main_entry_file);
    return 0;
}

int main(int argc, char **argv) {
    printf("SwiftLaTeX Engine Loaded\n");
#ifdef WEBASSEMBLY_BUILD_LOCAL
    compileFormat();
#endif
}

#endif
