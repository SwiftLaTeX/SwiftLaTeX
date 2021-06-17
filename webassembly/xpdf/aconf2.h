/*
 * aconf2.h
 *
 * This gets included by aconf.h, and contains miscellaneous global
 * settings not directly controlled by autoconf.  This is a separate
 * file because otherwise the configure script will munge any
 * #define/#undef constructs.
 *
 * Copyright 2002-2003 Glyph & Cog, LLC
 */

#ifndef ACONF2_H
#define ACONF2_H

/*
 * This controls the use of the interface/implementation pragmas.
 */
#if defined(__GNUC__) && !defined(__clang__)
#define USE_GCC_PRAGMAS
#endif
/* There is a bug in the version of gcc which ships with MacOS X 10.2 */
#if defined(__APPLE__) && defined(__MACH__)
#  include <AvailabilityMacros.h>
#endif
#ifdef MAC_OS_X_VERSION_MAX_ALLOWED
#  if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2
#    undef USE_GCC_PRAGMAS
#  endif
#endif

/*
 * Speed up Windows compilation.  This will only work for the command
 * line tools.
 */
/*
 *#ifdef _WIN32
 *#  define WIN32_LEAN_AND_MEAN
 *#endif
 */

#endif
