/* kpsewhich -- standalone path lookup and variable expansion for Kpathsea.
   Ideas from Thomas Esser, Pierre MacKay, and many others.

   Copyright 1995-2016 Karl Berry & Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/expand.h>
#include <kpathsea/getopt.h>
#include <kpathsea/line.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/proginit.h>
#include <kpathsea/str-list.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-glyph.h>
#include <kpathsea/variable.h>
#include <kpathsea/version.h>
#include <string.h>
#include <Python.h>


kpathsea kpse = NULL;


/* Base resolution. (-D, -dpi) */
unsigned dpi = 600;



/* Search the disk as well as ls-R?  (-must-exist, -mktex) */
boolean must_exist = false;


/* The file type and path for lookups.  (-format, -path) */




/* Define one-word abbreviations for those format types which
   can otherwise only be specified by strings containing spaces.  */

typedef struct
{
  const_string abbr;
  kpse_file_format_type format;
} format_abbr_type;

static format_abbr_type format_abbrs[]
  = { { "bitmapfont", kpse_any_glyph_format },
      { "mpsupport", kpse_mpsupport_format },
      { "doc", kpse_texdoc_format },
      { "source", kpse_texsource_format },
      { "trofffont", kpse_troff_font_format },
      { "dvipsconfig", kpse_dvips_config_format },
      { "web2c", kpse_web2c_format },
      { "othertext", kpse_program_text_format },
      { "otherbin", kpse_program_binary_format },
      { "miscfont", kpse_miscfonts_format },
      { "cmap", kpse_cmap_format },
      { "pdftexconfig", kpse_pdftex_config_format },
      { NULL, kpse_last_format } };

/* The function to look up STR in the abbr table above.
   This is called only on a user-specified format string.
   Return `kpse_last_format' if no match.  */

static kpse_file_format_type
format_abbr (const_string str)
{
  kpse_file_format_type ret = kpse_last_format;
  unsigned a = 0;

  while (format_abbrs[a].abbr != NULL) {
    if (STREQ (str, format_abbrs[a].abbr)) {
      ret = format_abbrs[a].format;
      break;
    }
    a++;
  }

  return ret;
}



/* Return the <number> substring in `<name>.<number><stuff>', if S has
   that form.  If it doesn't, return 0.  */

static unsigned
find_dpi (string s)
{
  unsigned dpi_number = 0;
  const_string extension = find_suffix (s);

  if (extension != NULL)
    sscanf (extension, "%u", &dpi_number);

  return dpi_number;
}



/* Return true if FTRY (the candidate suffix) matches NAME.  If
   IS_FILENAME is true, the check is simply that FTRY is a suffix of
   NAME.  If false (that is, NAME is a format), then FTRY and NAME must
   be entirely equal.  */

static boolean
try_suffix (boolean is_filename, string name, unsigned name_len,
            const_string ftry)
{
  unsigned try_len;

  if (!ftry || ! *ftry) {
    return false;
  }

  try_len = strlen (ftry);
  if (try_len > name_len) {
    /* Candidate is longer than what we're looking for.  */
    return false;
  }
  if (!is_filename && try_len < name_len) {
    /* We're doing format names, not file names, and candidate is
       shorter than what we're looking for.  E.g., do not find `lua'
       when looking for `clua'.  */
    return false;
  }

  if (FILESTRCASEEQ (name + name_len - try_len, ftry)) {
    return true;
  }

  return false;
}



/* Use the file type from -format if that was previously determined
   (i.e., the user_format global variable), else guess dynamically from
   NAME.  Return kpse_last_format if undeterminable.  This function is
   also used to parse the -format string, a case we distinguish via
   is_filename being false.

   A few filenames have been hard-coded for format types that
   differ from what would be inferred from their extensions. */

static kpse_file_format_type
find_format (kpathsea kpse, string name, boolean is_filename)
{
  kpse_file_format_type ret = kpse_last_format;

  if (FILESTRCASEEQ (name, "config.ps")) {
    ret = kpse_dvips_config_format;
  } else if (FILESTRCASEEQ (name, "fmtutil.cnf")) {
    ret = kpse_web2c_format;
  } else if (FILESTRCASEEQ (name, "glyphlist.txt")) {
    ret = kpse_fontmap_format;
  } else if (FILESTRCASEEQ (name, "mktex.cnf")) {
    ret = kpse_web2c_format;
  } else if (FILESTRCASEEQ (name, "pdfglyphlist.txt")) {
    ret = kpse_fontmap_format;
  } else if (FILESTRCASEEQ (name, "pdftex.cfg")) {
    ret = kpse_pdftex_config_format;
  } else if (FILESTRCASEEQ (name, "texglyphlist.txt")) {
    ret = kpse_fontmap_format;
  } else if (FILESTRCASEEQ (name, "texmf.cnf")) {
    ret = kpse_cnf_format;
  } else if (FILESTRCASEEQ (name, "updmap.cfg")) {
    ret = kpse_web2c_format;
  } else if (FILESTRCASEEQ (name, "XDvi")) {
    ret = kpse_program_text_format;
  } else {
    if (!is_filename) {
      /* Look for kpsewhich-specific format abbreviations.  */
      ret = format_abbr (name);
    }

    if (ret == kpse_last_format) {
      int f = 0;  /* kpse_file_format_type */
      unsigned name_len = strlen (name);

      while (f != kpse_last_format) {
        const_string *ext;
        const_string ftry;
        boolean found = false;

        if (!kpse->format_info[f].type)
          kpathsea_init_format (kpse, (kpse_file_format_type) f);

/* Just to abbreviate this lengthy call.  */
#define TRY_SUFFIX(ftry) try_suffix (is_filename, name, name_len, (ftry))

        if (!is_filename) {
          /* Allow the long name, but only in the format options.  We don't
             want a filename confused with a format name.  */
          ftry = kpse->format_info[f].type;
          found = TRY_SUFFIX (ftry);
        }
        for (ext = kpse->format_info[f].suffix; !found && ext && *ext; ext++) {
          found = TRY_SUFFIX (*ext);
          //printf("Trying1 %s %d\n", *ext, found);
        }
        for (ext=kpse->format_info[f].alt_suffix; !found && ext && *ext;ext++){
          found = TRY_SUFFIX (*ext);
          //printf("Trying2 %s %d\n", *ext, found);

        }

        if (found)
          break;
        f++;
      }
      ret = f;
    }
  }
  return ret;
}






/* Look up a single filename NAME.  Return 0 if success, 1 if failure.  */

static string
lookup (kpathsea kpse, string name)
{

  string ret = NULL;



    /* No user-specified search path, check user format or guess from NAME.  */
    kpse_file_format_type fmt = find_format (kpse, name, true);

    switch (fmt) {
      case kpse_pk_format:
      case kpse_gf_format:
      case kpse_any_glyph_format:
        {
          kpse_glyph_file_type glyph_ret;
          string temp = remove_suffix (name);

          /* Try to extract the resolution from the name.  */
          unsigned local_dpi = find_dpi (name);
          if (!local_dpi)
            local_dpi = dpi;
          ret = kpathsea_find_glyph (kpse, temp,
                                     local_dpi, fmt, &glyph_ret);
          if (temp != name)
            free (temp);
        }
        break;

      case kpse_last_format:
        /* If the suffix isn't recognized, assume it's a tex file. */
        fmt = kpse_tex_format;
        /* fall through */

      default:
          ret = kpathsea_find_file (kpse, name, fmt, must_exist);

    }


  return ret;
}







static PyObject *py_kpse_find_file(PyObject *self, PyObject *args)
{
  char *filename;
  char *completefilename;
  PyObject *returnvalue;
  if (PyArg_ParseTuple(args, "s", &filename)) {
    completefilename = lookup(kpse, filename);
    returnvalue = Py_BuildValue("s", completefilename);
    if(completefilename != NULL)
      free(completefilename);
    return returnvalue;
  }
  return NULL;
}

/* exported methods */

static PyMethodDef pykpathsea_methods[] = {
  {"find_file", (PyCFunction) py_kpse_find_file,  METH_VARARGS, NULL},
  {NULL, NULL}
};

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "pykpathsea",
        NULL,
        -1,
        pykpathsea_methods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC
PyInit_pykpathsea(void)
{
  PyObject *module = PyModule_Create(&moduledef);
  if (module == NULL)
    return NULL;

  kpse = kpathsea_new();

  kpathsea_set_program_name (kpse, "xelatex", "xelatex");

  kpathsea_set_program_enabled (kpse, kpse_pk_format, true, kpse_src_cmdline - 1);

  kpathsea_set_program_enabled (kpse, kpse_tfm_format, true, kpse_src_cmdline - 1);

  kpathsea_xputenv (kpse, "engine", "xetex");

  kpathsea_init_prog (kpse, uppercasify (kpse->program_name), dpi, NULL, NULL);

  return module;
}