//========================================================================
//
// HTMLGen.cc
//
// Copyright 2010 Glyph & Cog, LLC
//
//========================================================================

//~ to do:
//~ - fonts
//~   - underlined? (underlines are present in the background image)
//~   - include the original font name in the CSS entry (before the
//~     generic serif/sans-serif/monospace name)
//~ - check that htmlDir exists and is a directory
//~ - links:
//~   - links to pages
//~   - links to named destinations
//~   - links to URLs
//~ - rotated text should go in the background image
//~ - metadata
//~ - PDF outline

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <png.h>
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "SplashBitmap.h"
#include "PDFDoc.h"
#include "GfxFont.h"
#include "TextOutputDev.h"
#include "SplashOutputDev.h"
#include "ErrorCodes.h"
#include "WebFont.h"
#include "HTMLGen.h"

#ifdef _WIN32
#  define strcasecmp stricmp
#  define strncasecmp strnicmp
#endif

//------------------------------------------------------------------------

struct FontStyleTagInfo {
  const char *tag;
  int tagLen;
  GBool bold;
  GBool italic;
};

// NB: these are compared, in order, against the tail of the font
// name, so "BoldItalic" must come before "Italic", etc.
static FontStyleTagInfo fontStyleTags[] = {
  {"Roman",                    5, gFalse, gFalse},
  {"Regular",                  7, gFalse, gFalse},
  {"Condensed",                9, gFalse, gFalse},
  {"CondensedBold",           13, gTrue,  gFalse},
  {"CondensedLight",          14, gFalse, gFalse},
  {"SemiBold",                 8, gTrue,  gFalse},
  {"BoldItalicMT",            12, gTrue,  gTrue},
  {"BoldItalic",              10, gTrue,  gTrue},
  {"Bold_Italic",             11, gTrue,  gTrue},
  {"BoldOblique",             11, gTrue,  gTrue},
  {"Bold_Oblique",            12, gTrue,  gTrue},
  {"BoldMT",                   6, gTrue,  gFalse},
  {"Bold",                     4, gTrue,  gFalse},
  {"ItalicMT",                 8, gFalse, gTrue},
  {"Italic",                   6, gFalse, gTrue},
  {"Oblique",                  7, gFalse, gTrue},
  {"Light",                    5, gFalse, gFalse},
  {NULL,                       0, gFalse, gFalse}
};

struct StandardFontInfo {
  const char *name;
  GBool fixedWidth;
  GBool serif;
};

static StandardFontInfo standardFonts[] = {
  {"Arial",                    gFalse, gFalse},
  {"Courier",                  gTrue,  gFalse},
  {"Futura",                   gFalse, gFalse},
  {"Helvetica",                gFalse, gFalse},
  {"Minion",                   gFalse, gTrue},
  {"NewCenturySchlbk",         gFalse, gTrue},
  {"Times",                    gFalse, gTrue},
  {"TimesNew",                 gFalse, gTrue},
  {"Times_New",                gFalse, gTrue},
  {"Verdana",                  gFalse, gFalse},
  {"LucidaSans",               gFalse, gFalse},
  {NULL,                       gFalse, gFalse}
};

struct SubstFontInfo {
  double mWidth;
};

// index: {fixed:8, serif:4, sans-serif:0} + bold*2 + italic
static SubstFontInfo substFonts[16] = {
  {0.833},
  {0.833},
  {0.889},
  {0.889},
  {0.788},
  {0.722},
  {0.833},
  {0.778},
  {0.600},
  {0.600},
  {0.600},
  {0.600}
};

// Map Unicode indexes from the private use area, following the Adobe
// Glyph list.
#define privateUnicodeMapStart 0xf6f9
#define privateUnicodeMapEnd   0xf7ff
static int
privateUnicodeMap[privateUnicodeMapEnd - privateUnicodeMapStart + 1] = {
  0x0141, 0x0152, 0,      0,      0x0160, 0,      0x017d,         // f6f9
  0,      0,      0,      0,      0,      0,      0,      0,      // f700
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0,      0,      0,      0,      0,      0,      0,      // f710
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0x0021, 0,      0,      0x0024, 0,      0x0026, 0,      // f720
  0,      0,      0,      0,      0,      0,      0,      0,
  0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, // f730
  0x0038, 0x0039, 0,      0,      0,      0,      0,      0x003f,
  0,      0,      0,      0,      0,      0,      0,      0,      // f740
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0,      0,      0,      0,      0,      0,      0,      // f750
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, // f760
  0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
  0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, // f770
  0x0058, 0x0059, 0x005a, 0,      0,      0,      0,      0,
  0,      0,      0,      0,      0,      0,      0,      0,      // f780
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0,      0,      0,      0,      0,      0,      0,      // f790
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0x00a1, 0x00a2, 0,      0,      0,      0,      0,      // f7a0
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0,      0,      0,      0,      0,      0,      0,      // f7b0
  0,      0,      0,      0,      0,      0,      0,      0x00bf,
  0,      0,      0,      0,      0,      0,      0,      0,      // f7c0
  0,      0,      0,      0,      0,      0,      0,      0,
  0,      0,      0,      0,      0,      0,      0,      0,      // f7d0
  0,      0,      0,      0,      0,      0,      0,      0,
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // f7e0
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
  0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0,      // f7f0
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x0178
};

enum VerticalAlignment {
  vertAlignBaseline,
  vertAlignSub,
  vertAlignSuper,
  vertAlignTop
};

static const char *vertAlignNames[] = {
  "baseline",
  "sub",
  "super",
  "top"
};

//------------------------------------------------------------------------

class HTMLGenFontDefn {
public:

  HTMLGenFontDefn(Ref fontIDA, GString *fontFaceA, GString *fontSpecA,
		  double scaleA)
    : fontID(fontIDA), fontFace(fontFaceA), fontSpec(fontSpecA)
    , scale(scaleA), used(gFalse) {}
  ~HTMLGenFontDefn() { delete fontFace; delete fontSpec; }
  GBool match(Ref fontIDA)
    { return fontIDA.num == fontID.num && fontIDA.gen == fontID.gen; }

  Ref fontID;
  GString *fontFace;		// NULL for substituted fonts
  GString *fontSpec;
  double scale;
  GBool used;			// set when used (per page)
};

//------------------------------------------------------------------------


//------------------------------------------------------------------------

HTMLGen::HTMLGen(double backgroundResolutionA) {
  TextOutputControl textOutControl;
  SplashColor paperColor;

  ok = gTrue;

  backgroundResolution = backgroundResolutionA;
  zoom = 1.0;
  drawInvisibleText = gTrue;
  allTextInvisible = gFalse;
  extractFontFiles = gFalse;

  // set up the TextOutputDev
  textOutControl.mode = textOutReadingOrder;
  textOutControl.html = gTrue;
  textOut = new TextOutputDev(NULL, &textOutControl, gFalse);
  if (!textOut->isOk()) {
    ok = gFalse;
  }

  // set up the SplashOutputDev
  paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
  splashOut = new SplashOutputDev(splashModeRGB8, 1, gFalse, paperColor);

  fontDefns = NULL;
}

HTMLGen::~HTMLGen() {
  delete textOut;
  delete splashOut;
  if (fontDefns) {
    deleteGList(fontDefns, HTMLGenFontDefn);
  }
}

void HTMLGen::startDoc(PDFDoc *docA) {
  doc = docA;
  splashOut->startDoc(doc->getXRef());

  if (fontDefns) {
    deleteGList(fontDefns, HTMLGenFontDefn);
  }
  fontDefns = new GList();
  nextFontFaceIdx = 0;
}

static inline int pr(int (*writeFunc)(void *stream, const char *data, int size),
		     void *stream, const char *data) {
  return writeFunc(stream, data, (int)strlen(data));
}

static int pf(int (*writeFunc)(void *stream, const char *data, int size),
	      void *stream, const char *fmt, ...) {
  va_list args;
  GString *s;
  int ret;

  va_start(args, fmt);
  s = GString::formatv(fmt, args);
  va_end(args);
  ret = writeFunc(stream, s->getCString(), s->getLength());
  delete s;
  return ret;
}

struct PNGWriteInfo {
  int (*writePNG)(void *stream, const char *data, int size);
  void *pngStream;
};

static void pngWriteFunc(png_structp png, png_bytep data, png_size_t size) {
  PNGWriteInfo *info;

  info = (PNGWriteInfo *)png_get_progressive_ptr(png);
  info->writePNG(info->pngStream, (char *)data, (int)size);
}

int HTMLGen::convertPage(
		 int pg, const char *pngURL, const char *htmlDir,
		 int (*writeHTML)(void *stream, const char *data, int size),
		 void *htmlStream,
		 int (*writePNG)(void *stream, const char *data, int size),
		 void *pngStream) {
  png_structp png;
  png_infop pngInfo;
  PNGWriteInfo writeInfo;
  SplashBitmap *bitmap;
  Guchar *p;
  double pageW, pageH;
  TextPage *text;
  GList *cols, *pars, *lines, *words;
  TextFontInfo *font;
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  HTMLGenFontDefn *fontDefn;
  GString *s;
  double base;
  int primaryDir, spanDir;
  int colIdx, parIdx, lineIdx, firstWordIdx, lastWordIdx;
  int y, i;

  // generate the background bitmap
  splashOut->setSkipText(!allTextInvisible, gFalse);
  doc->displayPage(splashOut, pg, backgroundResolution, backgroundResolution,
		   0, gFalse, gTrue, gFalse);
  bitmap = splashOut->getBitmap();
  if (!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				      NULL, NULL, NULL)) ||
      !(pngInfo = png_create_info_struct(png))) {
    return errFileIO;
  }
  if (setjmp(png_jmpbuf(png))) {
    return errFileIO;
  }
  writeInfo.writePNG = writePNG;
  writeInfo.pngStream = pngStream;
  png_set_write_fn(png, &writeInfo, pngWriteFunc, NULL);
  png_set_IHDR(png, pngInfo, bitmap->getWidth(), bitmap->getHeight(),
	       8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, pngInfo);
  p = bitmap->getDataPtr();
  for (y = 0; y < bitmap->getHeight(); ++y) {
    png_write_row(png, (png_bytep)p);
    p += bitmap->getRowSize();
  }
  png_write_end(png, pngInfo);
  png_destroy_write_struct(&png, &pngInfo);

  // page size
  if (doc->getPageRotate(pg) == 90 || doc->getPageRotate(pg) == 270) {
    pageW = doc->getPageCropHeight(pg);
    pageH = doc->getPageCropWidth(pg);
  } else {
    pageW = doc->getPageCropWidth(pg);
    pageH = doc->getPageCropHeight(pg);
  }

  // get the PDF text
  doc->displayPage(textOut, pg, 72, 72, 0, gFalse, gTrue, gFalse);
  doc->processLinks(textOut, pg);
  text = textOut->takeText();
  primaryDir = text->primaryDirectionIsLR() ? 1 : -1;

  // HTML header
  pr(writeHTML, htmlStream, "<html>\n");
  pr(writeHTML, htmlStream, "<head>\n");
  pr(writeHTML, htmlStream, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
  pr(writeHTML, htmlStream, "<style type=\"text/css\">\n");
  pr(writeHTML, htmlStream, ".txt { white-space:nowrap; }\n");
  fonts = text->getFonts();
  fontScales = (double *)gmallocn(fonts->getLength(), sizeof(double));
  for (i = 0; i < fontDefns->getLength(); ++i) {
    fontDefn = (HTMLGenFontDefn *)fontDefns->get(i);
    fontDefn->used = gFalse;
  }
  for (i = 0; i < fonts->getLength(); ++i) {
    font = (TextFontInfo *)fonts->get(i);
    fontDefn = getFontDefn(font, htmlDir);
    if (!fontDefn->used && fontDefn->fontFace) {
      pr(writeHTML, htmlStream, fontDefn->fontFace->getCString());
    }
    pf(writeHTML, htmlStream, "#f{0:d} {{ {1:t} }}\n", i, fontDefn->fontSpec);
    fontScales[i] = fontDefn->scale;
    fontDefn->used = gTrue;
  }
  pr(writeHTML, htmlStream, "</style>\n");
  pr(writeHTML, htmlStream, "</head>\n");
  if (primaryDir >= 0) {
    pr(writeHTML, htmlStream, "<body>\n");
  } else {
    pr(writeHTML, htmlStream, "<body dir=\"rtl\">\n");
  }
  if (primaryDir >= 0) {
    pf(writeHTML, htmlStream, "<img id=\"background\" style=\"position:absolute; left:0px; top:0px;\" width=\"{0:d}\" height=\"{1:d}\" src=\"{2:s}\">\n",
       (int)(pageW * zoom), (int)(pageH * zoom), pngURL);
  } else {
    pf(writeHTML, htmlStream, "<img id=\"background\" style=\"position:absolute; right:0px; top:0px;\" width=\"{0:d}\" height=\"{1:d}\" src=\"{2:s}\">\n",
       (int)(pageW * zoom), (int)(pageH * zoom), pngURL);
  }

  // generate the HTML text
  cols = text->makeColumns();
  for (colIdx = 0; colIdx < cols->getLength(); ++colIdx) {
    col = (TextColumn *)cols->get(colIdx);
    pars = col->getParagraphs();
    for (parIdx = 0; parIdx < pars->getLength(); ++parIdx) {
      par = (TextParagraph *)pars->get(parIdx);
      lines = par->getLines();
      for (lineIdx = 0; lineIdx < lines->getLength(); ++lineIdx) {
	line = (TextLine *)lines->get(lineIdx);
	if (line->getRotation() != 0) {
	  continue;
	}
	words = line->getWords();
	if (lineIdx == 0 && par->hasDropCap() && words->getLength() >= 2) {
	  base = ((TextWord *)words->get(1))->getBaseline();
	} else {
	  base = line->getBaseline();
	}
	s = new GString();
	for (firstWordIdx = (primaryDir >= 0) ? 0 : words->getLength() - 1;
	     (primaryDir >= 0) ? firstWordIdx < words->getLength()
	                       : firstWordIdx >= 0;
	     firstWordIdx = lastWordIdx + primaryDir) {
	  lastWordIdx = findDirSpan(words, firstWordIdx,
				    primaryDir, &spanDir);
	  appendSpans(words, firstWordIdx, lastWordIdx,
		      primaryDir, spanDir,
		      base, lineIdx == 0 && par->hasDropCap(),
		      s);
	}
	if (primaryDir >= 0) {
	  pf(writeHTML, htmlStream, "<div class=\"txt\" style=\"position:absolute; left:{0:d}px; top:{1:d}px;\">{2:t}</div>\n",
	     (int)(line->getXMin() * zoom),
	     (int)(line->getYMin() * zoom), s);
	} else {
	  pf(writeHTML, htmlStream, "<div class=\"txt\" style=\"position:absolute; right:{0:d}px; top:{1:d}px;\">{2:t}</div>\n",
	     (int)((pageW - line->getXMax()) * zoom),
	     (int)(line->getYMin() * zoom), s);
	}
	delete s;
      }
    }
  }
  gfree(fontScales);
  delete text;
  deleteGList(cols, TextColumn);

  // HTML trailer
  pr(writeHTML, htmlStream, "</body>\n");
  pr(writeHTML, htmlStream, "</html>\n");

  return errNone;
}

// Find a sequence of words, starting at <firstWordIdx>, that have the
// same writing direction.  Returns the index of the last word, and
// sets *<spanDir> to the span direction.
int HTMLGen::findDirSpan(GList *words, int firstWordIdx, int primaryDir,
			 int *spanDir) {
  int dir0, dir1, nextWordIdx;

  dir0 = ((TextWord *)words->get(firstWordIdx))->getDirection();
  for (nextWordIdx = firstWordIdx + primaryDir;
       (primaryDir >= 0) ? nextWordIdx < words->getLength()
	                 : nextWordIdx >= 0;
       nextWordIdx += primaryDir) {
    dir1 = ((TextWord *)words->get(nextWordIdx))->getDirection();
    if (dir0 == 0) {
      dir0 = dir1;
    } else if (dir1 != 0 && dir1 != dir0) {
      break;
    }
  }

  if (dir0 == 0) {
    *spanDir = primaryDir;
  } else {
    *spanDir = dir0;
  }

  return nextWordIdx - primaryDir;
}

// Create HTML spans for words <firstWordIdx> .. <lastWordIdx>, and
// append them to <s>.
void HTMLGen::appendSpans(GList *words, int firstWordIdx, int lastWordIdx,
			  int primaryDir, int spanDir,
			  double base, GBool dropCapLine, GString *s) {
  TextWord *word0, *word1;
  VerticalAlignment vertAlign0, vertAlign1;
  const char *dirTag;
  Unicode u;
  GBool invisible, sp;
  double r0, g0, b0, r1, g1, b1;
  double base1;
  int wordIdx, t, i;

  if (spanDir != primaryDir) {
    t = firstWordIdx;
    firstWordIdx = lastWordIdx;
    lastWordIdx = t;
  }

  word0 = NULL;
  vertAlign0 = vertAlignBaseline; // make gcc happy
  r0 = g0 = b0 = 0; // make gcc happy
  for (wordIdx = firstWordIdx;
       (spanDir >= 0) ? wordIdx <= lastWordIdx : wordIdx >= lastWordIdx;
       wordIdx += spanDir) {
    word1 = (TextWord *)words->get(wordIdx);
    invisible = allTextInvisible || word1->isInvisible();
    if (!drawInvisibleText && invisible) {
      continue;
    }
    word1->getColor(&r1, &g1, &b1);
    base1 = word1->getBaseline();
    if (dropCapLine) {
      //~ this will fail if there are subscripts or superscripts in
      //~   the first line of a paragraph with a drop cap
      vertAlign1 = vertAlignTop;
    } else if (base1 - base < -1) {
      vertAlign1 = vertAlignSuper;
    } else if (base1 - base > 1) {
      vertAlign1 = vertAlignSub;
    } else {
      vertAlign1 = vertAlignBaseline;
    }
    if (!word0 ||
	word1->getFontInfo() != word0->getFontInfo() ||
	word1->getFontSize() != word0->getFontSize() ||
	word1->isInvisible() != word0->isInvisible() ||
	vertAlign1 != vertAlign0 ||
	r1 != r0 || g1 != g0 || b1 != b0) {
      if (word0) {
	s->append("</span>");
      }
      for (i = 0; i < fonts->getLength(); ++i) {
	if (word1->getFontInfo() == (TextFontInfo *)fonts->get(i)) {
	  break;
	}
      }
      // we force spans to be LTR or RTL; this is a kludge, but it's
      // far easier than implementing the full Unicode bidi algorithm
      if (spanDir == primaryDir) {
	dirTag = "";
      } else if (spanDir < 0) {
	dirTag = " dir=\"rtl\"";
      } else {
	dirTag = " dir=\"ltr\"";
      }
      s->appendf("<span id=\"f{0:d}\"{1:s} style=\"font-size:{2:d}px;vertical-align:{3:s};{4:s}color:rgba({5:d},{6:d},{7:d},{8:d});\">",
		 i,
		 dirTag,
		 (int)(fontScales[i] * word1->getFontSize() * zoom),
		 vertAlignNames[vertAlign1],
		 (dropCapLine && wordIdx == 0) ? "line-height:75%;" : "",
		 (int)(r1 * 255), (int)(g1 * 255), (int)(b1 * 255),
		 invisible ? 0 : 1);
    }

    // add a space before the word, if needed
    // -- this only happens with the first word in a reverse section
    if (spanDir != primaryDir && wordIdx == firstWordIdx) {
      if (spanDir >= 0) {
	if (wordIdx > 0) {
	  sp = ((TextWord *)words->get(wordIdx - 1))->getSpaceAfter();
	} else {
	  sp = gFalse;
	}
      } else {
	sp = word1->getSpaceAfter();
      }
      if (sp) {
	s->append(' ');
      }
    }

    for (i = (spanDir >= 0) ? 0 : word1->getLength() - 1;
	 (spanDir >= 0) ? i < word1->getLength() : i >= 0;
	 i += spanDir) {
      u = word1->getChar(i);
      if (u >= privateUnicodeMapStart &&
	  u <= privateUnicodeMapEnd &&
	  privateUnicodeMap[u - privateUnicodeMapStart]) {
	u = privateUnicodeMap[u - privateUnicodeMapStart];
      }
      appendUTF8(u, s);
    }

    // add a space after the word, if needed
    // -- there is never a space after the last word in a reverse
    //    section (this will be handled as a space after the last word
    //    in the previous primary-direction section)
    if (spanDir != primaryDir && wordIdx == lastWordIdx) {
      sp = gFalse;
    } else if (spanDir >= 0) {
      sp = word1->getSpaceAfter();
    } else {
      if (wordIdx > 0) {
	sp = ((TextWord *)words->get(wordIdx - 1))->getSpaceAfter();
      } else {
	sp = gFalse;
      }
    }
    if (sp) {
      s->append(' ');
    }

    word0 = word1;
    vertAlign0 = vertAlign1;
    r0 = r1;
    g0 = g1;
    b0 = b1;
  }
  s->append("</span>");
}

void HTMLGen::appendUTF8(Unicode u, GString *s) {
  if (u <= 0x7f) {
    if (u == '&') {
      s->append("&amp;");
    } else if (u == '<') {
      s->append("&lt;");
    } else if (u == '>') {
      s->append("&gt;");
    } else {
      s->append((char)u);
    }
  } else if (u <= 0x7ff) {
    s->append((char)(0xc0 + (u >> 6)));
    s->append((char)(0x80 + (u & 0x3f)));
  } else if (u <= 0xffff) {
    s->append((char)(0xe0 + (u >> 12)));
    s->append((char)(0x80 + ((u >> 6) & 0x3f)));
    s->append((char)(0x80 + (u & 0x3f)));
  } else if (u <= 0x1fffff) {
    s->append((char)(0xf0 + (u >> 18)));
    s->append((char)(0x80 + ((u >> 12) & 0x3f)));
    s->append((char)(0x80 + ((u >> 6) & 0x3f)));
    s->append((char)(0x80 + (u & 0x3f)));
  } else if (u <= 0x3ffffff) {
    s->append((char)(0xf8 + (u >> 24)));
    s->append((char)(0x80 + ((u >> 18) & 0x3f)));
    s->append((char)(0x80 + ((u >> 12) & 0x3f)));
    s->append((char)(0x80 + ((u >> 6) & 0x3f)));
    s->append((char)(0x80 + (u & 0x3f)));
  } else if (u <= 0x7fffffff) {
    s->append((char)(0xfc + (u >> 30)));
    s->append((char)(0x80 + ((u >> 24) & 0x3f)));
    s->append((char)(0x80 + ((u >> 18) & 0x3f)));
    s->append((char)(0x80 + ((u >> 12) & 0x3f)));
    s->append((char)(0x80 + ((u >> 6) & 0x3f)));
    s->append((char)(0x80 + (u & 0x3f)));
  }
}

HTMLGenFontDefn *HTMLGen::getFontDefn(TextFontInfo *font,
				      const char *htmlDir) {
  Ref id;
  HTMLGenFontDefn *fontDefn;
  int i;

  // check the existing font defns
  id = font->getFontID();
  if (id.num >= 0) {
    for (i = 0; i < fontDefns->getLength(); ++i) {
      fontDefn = (HTMLGenFontDefn *)fontDefns->get(i);
      if (fontDefn->match(id)) {
	return fontDefn;
      }
    }
  }

  // try to extract a font file
  if (!extractFontFiles ||
      !(fontDefn = getFontFile(font, htmlDir))) {

    // get a substitute font
    fontDefn = getSubstituteFont(font);
  }

  fontDefns->append(fontDefn);
  return fontDefn;
}

HTMLGenFontDefn *HTMLGen::getFontFile(TextFontInfo *font,
				      const char *htmlDir) {
  Ref id;
  HTMLGenFontDefn *fontDefn;
  Object fontObj;
  GfxFont *gfxFont;
  WebFont *webFont;
  GString *fontFile, *fontPath, *fontFace, *fontSpec;
  const char *family, *weight, *style;
  double scale;

  id = font->getFontID();
  if (id.num < 0) {
    return NULL;
  }

  doc->getXRef()->fetch(id.num, id.gen, &fontObj);
  if (!fontObj.isDict()) {
    fontObj.free();
    return NULL;
  }

  gfxFont = GfxFont::makeFont(doc->getXRef(), "F", id, fontObj.getDict());
  webFont = new WebFont(gfxFont, doc->getXRef());
  fontDefn = NULL;

  if (webFont->canWriteTTF()) {
    fontFile = GString::format("{0:d}.ttf", nextFontFaceIdx);
    fontPath = GString::format("{0:s}/{1:t}", htmlDir, fontFile);
    if (webFont->writeTTF(fontPath->getCString())) {
      fontFace = GString::format("@font-face {{ font-family: ff{0:d}; src: url(\"{1:t}\"); }}\n",
				 nextFontFaceIdx, fontFile);
      getFontDetails(font, &family, &weight, &style, &scale);
      fontSpec = GString::format("font-family:ff{0:d},{1:s}; font-weight:{2:s}; font-style:{3:s};",
				 nextFontFaceIdx, family, weight, style);
      ++nextFontFaceIdx;
      fontDefn = new HTMLGenFontDefn(id, fontFace, fontSpec, 1.0);
    }
    delete fontPath;
    delete fontFile;

  } else if (webFont->canWriteOTF()) {
    fontFile = GString::format("{0:d}.otf", nextFontFaceIdx);
    fontPath = GString::format("{0:s}/{1:t}", htmlDir, fontFile);
    if (webFont->writeOTF(fontPath->getCString())) {
      fontFace = GString::format("@font-face {{ font-family: ff{0:d}; src: url(\"{1:t}\"); }}\n",
				  nextFontFaceIdx, fontFile);
      getFontDetails(font, &family, &weight, &style, &scale);
      fontSpec = GString::format("font-family:ff{0:d},{1:s}; font-weight:{2:s}; font-style:{3:s};",
				 nextFontFaceIdx, family, weight, style);
      fontDefn = new HTMLGenFontDefn(id, fontFace, fontSpec, 1.0);
    }
    delete fontPath;
    delete fontFile;
  }

  delete webFont;
  delete gfxFont;
  fontObj.free();

  return fontDefn;
}

HTMLGenFontDefn *HTMLGen::getSubstituteFont(TextFontInfo *font) {
  const char *family, *weight, *style;
  double scale;
  GString *fontSpec;

  getFontDetails(font, &family, &weight, &style, &scale);
  fontSpec = GString::format("font-family:{0:s}; font-weight:{1:s}; font-style:{2:s};",
			     family, weight, style);
  return new HTMLGenFontDefn(font->getFontID(), NULL, fontSpec, scale);
}

void HTMLGen::getFontDetails(TextFontInfo *font, const char **family,
			     const char **weight, const char **style,
			     double *scale) {
  GString *fontName;
  char *fontName2;
  FontStyleTagInfo *fst;
  StandardFontInfo *sf;
  GBool fixedWidth, serif, bold, italic;
  double s;
  int n, i;

  // get the font name, remove any subset tag
  fontName = font->getFontName();
  if (fontName) {
    fontName2 = fontName->getCString();
    n = fontName->getLength();
    for (i = 0; i < n && i < 7; ++i) {
      if (fontName2[i] < 'A' || fontName2[i] > 'Z') {
	break;
      }
    }
    if (i == 6 && n > 7 && fontName2[6] == '+') {
      fontName2 += 7;
      n -= 7;
    }
  } else {
    fontName2 = NULL;
    n = 0;
  }

  // get the style info from the font descriptor flags
  fixedWidth = font->isFixedWidth();
  serif = font->isSerif();
  bold = font->isBold();
  italic = font->isItalic();

  if (fontName2) {

    // look for a style tag at the end of the font name -- this
    // overrides the font descriptor bold/italic flags
    for (fst = fontStyleTags; fst->tag; ++fst) {
      if (n > fst->tagLen &&
	  !strcasecmp(fontName2 + n - fst->tagLen, fst->tag)) {
	bold = fst->bold;
	italic = fst->italic;
	n -= fst->tagLen;
	if (n > 1 && (fontName2[n-1] == '-' ||
		      fontName2[n-1] == ',' ||
		      fontName2[n-1] == '.' ||
		      fontName2[n-1] == '_')) {
	  --n;
	}
	break;
      }
    }

    // look for a known font name -- this overrides the font descriptor
    // fixedWidth/serif flags
    for (sf = standardFonts; sf->name; ++sf) {
      if (!strncasecmp(fontName2, sf->name, n)) {
	fixedWidth = sf->fixedWidth;
	serif = sf->serif;
	break;
      }
    }
  }

  // compute the scaling factor
  *scale = 1;
  if ((s = font->getMWidth())) {
    i = (fixedWidth ? 8 : serif ? 4 : 0) + (bold ? 2 : 0) + (italic ? 1 : 0);
    if (s < substFonts[i].mWidth) {
      *scale = s / substFonts[i].mWidth;
    }
  }

  *family = fixedWidth ? "monospace" : serif ? "serif" : "sans-serif";
  *weight = bold ? "bold" : "normal";
  *style = italic ? "italic" : "normal";
}
