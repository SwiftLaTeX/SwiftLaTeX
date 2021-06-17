#ifndef W2CEMUHEADER
#define W2CEMUHEADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
typedef int integer;
typedef int boolean;
typedef long longinteger;
typedef const char* constcstring;
typedef char* cstring;
typedef int cinttype;
typedef signed char schar;
typedef char* string;
typedef const char* const_string;
typedef FILE* text;
typedef double real;
typedef double glueratio;
typedef long long longword;
#define false 0
#define true 1

#ifdef __cplusplus
extern "C" {
#endif
	#include "xmemory.h"
	#include "kpseemu.h"
#ifdef __cplusplus
}
#endif

#ifndef PRIdPTR
#define PRIdPTR "ld"
#endif
#ifndef PRIxPTR
#define PRIxPTR "lx"
#endif
#define LONGINTEGER_TYPE long
#define LONGINTEGER_PRI "l"

#endif
