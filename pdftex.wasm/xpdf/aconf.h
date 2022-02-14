/*
 * aconf.h
 *
 * This file is modified by cmake.
 *
 * Copyright 2002-2015 Glyph & Cog, LLC
 */

#ifndef ACONF_H
#define ACONF_H

#include <aconf2.h>

/*
 * Use A4 paper size instead of Letter for PostScript output.
 */
/* #undef A4_PAPER */

/*
 * Do not allow text selection.
 */
/* #undef NO_TEXT_SELECT */

/*
 * Include support for OPI comments.
 */
#define OPI_SUPPORT 0

#define DISABLE_OUTLINE 1 
#define PDF_PARSER_ONLY 1
/*
 * Enable multithreading support.
 */
#define MULTITHREADED 0

/*
 * Enable C++ exceptions.
 */
#define USE_EXCEPTIONS 0

/*
 * Use fixed point (instead of floating point) arithmetic.
 */
#define USE_FIXEDPOINT 0

/*
 * Enable support for CMYK output.
 */
#define SPLASH_CMYK 0

/*
 * Enable support for DeviceN output.
 */
#define SPLASH_DEVICEN 0

/*
 * Enable support for highlighted regions.
 */
/* #undef HIGHLIGHTED_REGIONS */

/*
 * Full path for the system-wide xpdfrc file.
 */
/* #undef SYSTEM_XPDFRC */

/*
 * Various include files and functions.
 */
#define HAVE_MKSTEMP 1
#define HAVE_MKSTEMPS 1
#define HAVE_POPEN 0
#define HAVE_STD_SORT 1
#define HAVE_FSEEKO 1
#define HAVE_FSEEK64 0
#define HAVE_FSEEKI64 0
#define _FILE_OFFSET_BITS 64
#define _LARGE_FILES 1
#define _LARGEFILE_SOURCE 1

/*
 * This is defined if using FreeType 2.
 */
#define HAVE_FREETYPE_H 0

/*
 * This is defined if using D-Type 4.
 */
#define HAVE_DTYPE4_H 0

/*
 * This is defined if using libpaper.
 */
#define HAVE_PAPER_H 0

/*
 * Defined if the Splash library is avaiable.
 */
#define HAVE_SPLASH 0

/*
 * Defined if using lcms2.
 */
#define HAVE_LCMS 0

/*
 * Defined for evaluation mode.
 */
#define EVAL_MODE 0

/*
 * Defined when building the closed source XpdfReader binary.
 */
#define BUILDING_XPDFREADER 0

#endif
