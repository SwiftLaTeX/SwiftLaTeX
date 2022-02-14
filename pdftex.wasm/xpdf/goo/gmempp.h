//========================================================================
//
// gmempp.h
//
// Redefine the new and delete operators to call the debugging malloc.
//
// This is a hack: some libraries (Qt) allocate global objects and
// never free them, so this file #define's new to have an argument, so
// that we can differentiate our new calls from external library new
// calls.  Calls from external libraries will not be counted when
// un-freed memory is listed.  There's no guarantee that this will
// work in all cases: if some external library also defines a new
// operator that takes the same extra argument, things will break
// horribly.
//
// This entire .h file is wrapped in '#ifdef DEBUG_MEM'.  I.e., if you
// don't define DEBUG_MEM, this hack goes away and can't cause any
// problems.  Do not define DEBUG_MEM in production code.
//
// To use this .h file, it must be #included *after* all system
// includes.  Calls to new in any modules that do not include this .h
// file will not be counted as un-freed memory.
//
//========================================================================

#ifndef GMEMPP_H
#define GMEMPP_H

#ifdef DEBUG_MEM

#include <stdlib.h>

extern void *operator new(size_t size, int dummy);
extern void *operator new[](size_t size, int dummy);

// These have to be defined (and declared) to avoid a compiler warning
// with Visual Studio.
extern void operator delete(void *p, int dummy);
extern void operator delete[](void *p, int dummy);

// This transforms 'new Foo(...)' into 'new (1) Foo(...)', which
// forces a call to the operator new variant with the 'int dummy' arg.
#define debug_new new (1)
#define new debug_new

#endif // DEBUG_MEM

#endif // GMEMPP_H
