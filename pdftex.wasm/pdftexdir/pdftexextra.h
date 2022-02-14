/*
Copyright 1996-2016 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* pdftexextra.h: banner etc. for pdfTeX.

   This is included by pdfTeX, from pdftexextra.c.
*/

#include <pdftexdir/pdftex_version.h> /* for PDFTEX_VERSION */
#include <pdftexdir/etex_version.h> /* for ETEX_VERSION */

#define BANNER "This is pdfTeX, Version 3.14159265-" ETEX_VERSION "-" PDFTEX_VERSION
#define COPYRIGHT_HOLDER "Han The Thanh (pdfTeX) et al"
#define AUTHOR NULL
#define PROGRAM_HELP PDFTEXHELP
#define BUG_ADDRESS "pdftex@tug.org"
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "pdfinitex"
#define VIR_PROGRAM "pdfvirtex"

