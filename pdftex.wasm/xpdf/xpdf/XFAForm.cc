//========================================================================
//
// XFAForm.cc
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "GHash.h"
#include "Error.h"
#include "Object.h"
#include "PDFDoc.h"
#include "Gfx.h"
#include "GfxFont.h"
#include "Zoox.h"
#include "PDF417Barcode.h"
#include "UTF8.h"
#include "XFAForm.h"

#ifdef _WIN32
#  undef strcasecmp
#  undef strncasecmp
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#endif

//------------------------------------------------------------------------

// 5 bars + 5 spaces -- each can be wide (1) or narrow (0)
// (there are always exactly 3 wide elements;
// the last space is always narrow)
static Guchar code3Of9Data[128][10] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x00
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x10
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 1, 0, 0, 0, 1, 0, 0, 0 }, // ' ' = 0x20
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 1, 0, 1, 0, 0, 0, 0 }, // '$' = 0x24
  { 0, 0, 0, 1, 0, 1, 0, 1, 0, 0 }, // '%' = 0x25
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 0 }, // '*' = 0x2a
  { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0 }, // '+' = 0x2b
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0 }, // '-' = 0x2d
  { 1, 1, 0, 0, 0, 0, 1, 0, 0, 0 }, // '.' = 0x2e
  { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 }, // '/' = 0x2f
  { 0, 0, 0, 1, 1, 0, 1, 0, 0, 0 }, // '0' = 0x30
  { 1, 0, 0, 1, 0, 0, 0, 0, 1, 0 }, // '1'
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 0 }, // '2'
  { 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // '3'
  { 0, 0, 0, 1, 1, 0, 0, 0, 1, 0 }, // '4'
  { 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 }, // '5'
  { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 }, // '6'
  { 0, 0, 0, 1, 0, 0, 1, 0, 1, 0 }, // '7'
  { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0 }, // '8'
  { 0, 0, 1, 1, 0, 0, 1, 0, 0, 0 }, // '9'
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x40
  { 1, 0, 0, 0, 0, 1, 0, 0, 1, 0 }, // 'A' = 0x41
  { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 }, // 'B'
  { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0 }, // 'C'
  { 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 }, // 'D'
  { 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 }, // 'E'
  { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0 }, // 'F'
  { 0, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 'G'
  { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 }, // 'H'
  { 0, 0, 1, 0, 0, 1, 1, 0, 0, 0 }, // 'I'
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0 }, // 'J'
  { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 }, // 'K'
  { 0, 0, 1, 0, 0, 0, 0, 1, 1, 0 }, // 'L'
  { 1, 0, 1, 0, 0, 0, 0, 1, 0, 0 }, // 'M'
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 0 }, // 'N'
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0 }, // 'O'
  { 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 }, // 'P' = 0x50
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0 }, // 'Q'
  { 1, 0, 0, 0, 0, 0, 1, 1, 0, 0 }, // 'R'
  { 0, 0, 1, 0, 0, 0, 1, 1, 0, 0 }, // 'S'
  { 0, 0, 0, 0, 1, 0, 1, 1, 0, 0 }, // 'T'
  { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0 }, // 'U'
  { 0, 1, 1, 0, 0, 0, 0, 0, 1, 0 }, // 'V'
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, // 'W'
  { 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 }, // 'X'
  { 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 }, // 'Y'
  { 0, 1, 1, 0, 1, 0, 0, 0, 0, 0 }, // 'Z'
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x60
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0x70
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

// 3 bars + 3 spaces -- each can be 1, 2, 3, or 4 units wide
static Guchar code128Data[107][6] = {
  { 2, 1, 2, 2, 2, 2 },
  { 2, 2, 2, 1, 2, 2 },
  { 2, 2, 2, 2, 2, 1 },
  { 1, 2, 1, 2, 2, 3 },
  { 1, 2, 1, 3, 2, 2 },
  { 1, 3, 1, 2, 2, 2 },
  { 1, 2, 2, 2, 1, 3 },
  { 1, 2, 2, 3, 1, 2 },
  { 1, 3, 2, 2, 1, 2 },
  { 2, 2, 1, 2, 1, 3 },
  { 2, 2, 1, 3, 1, 2 },
  { 2, 3, 1, 2, 1, 2 },
  { 1, 1, 2, 2, 3, 2 },
  { 1, 2, 2, 1, 3, 2 },
  { 1, 2, 2, 2, 3, 1 },
  { 1, 1, 3, 2, 2, 2 },
  { 1, 2, 3, 1, 2, 2 },
  { 1, 2, 3, 2, 2, 1 },
  { 2, 2, 3, 2, 1, 1 },
  { 2, 2, 1, 1, 3, 2 },
  { 2, 2, 1, 2, 3, 1 },
  { 2, 1, 3, 2, 1, 2 },
  { 2, 2, 3, 1, 1, 2 },
  { 3, 1, 2, 1, 3, 1 },
  { 3, 1, 1, 2, 2, 2 },
  { 3, 2, 1, 1, 2, 2 },
  { 3, 2, 1, 2, 2, 1 },
  { 3, 1, 2, 2, 1, 2 },
  { 3, 2, 2, 1, 1, 2 },
  { 3, 2, 2, 2, 1, 1 },
  { 2, 1, 2, 1, 2, 3 },
  { 2, 1, 2, 3, 2, 1 },
  { 2, 3, 2, 1, 2, 1 },
  { 1, 1, 1, 3, 2, 3 },
  { 1, 3, 1, 1, 2, 3 },
  { 1, 3, 1, 3, 2, 1 },
  { 1, 1, 2, 3, 1, 3 },
  { 1, 3, 2, 1, 1, 3 },
  { 1, 3, 2, 3, 1, 1 },
  { 2, 1, 1, 3, 1, 3 },
  { 2, 3, 1, 1, 1, 3 },
  { 2, 3, 1, 3, 1, 1 },
  { 1, 1, 2, 1, 3, 3 },
  { 1, 1, 2, 3, 3, 1 },
  { 1, 3, 2, 1, 3, 1 },
  { 1, 1, 3, 1, 2, 3 },
  { 1, 1, 3, 3, 2, 1 },
  { 1, 3, 3, 1, 2, 1 },
  { 3, 1, 3, 1, 2, 1 },
  { 2, 1, 1, 3, 3, 1 },
  { 2, 3, 1, 1, 3, 1 },
  { 2, 1, 3, 1, 1, 3 },
  { 2, 1, 3, 3, 1, 1 },
  { 2, 1, 3, 1, 3, 1 },
  { 3, 1, 1, 1, 2, 3 },
  { 3, 1, 1, 3, 2, 1 },
  { 3, 3, 1, 1, 2, 1 },
  { 3, 1, 2, 1, 1, 3 },
  { 3, 1, 2, 3, 1, 1 },
  { 3, 3, 2, 1, 1, 1 },
  { 3, 1, 4, 1, 1, 1 },
  { 2, 2, 1, 4, 1, 1 },
  { 4, 3, 1, 1, 1, 1 },
  { 1, 1, 1, 2, 2, 4 },
  { 1, 1, 1, 4, 2, 2 },
  { 1, 2, 1, 1, 2, 4 },
  { 1, 2, 1, 4, 2, 1 },
  { 1, 4, 1, 1, 2, 2 },
  { 1, 4, 1, 2, 2, 1 },
  { 1, 1, 2, 2, 1, 4 },
  { 1, 1, 2, 4, 1, 2 },
  { 1, 2, 2, 1, 1, 4 },
  { 1, 2, 2, 4, 1, 1 },
  { 1, 4, 2, 1, 1, 2 },
  { 1, 4, 2, 2, 1, 1 },
  { 2, 4, 1, 2, 1, 1 },
  { 2, 2, 1, 1, 1, 4 },
  { 4, 1, 3, 1, 1, 1 },
  { 2, 4, 1, 1, 1, 2 },
  { 1, 3, 4, 1, 1, 1 },
  { 1, 1, 1, 2, 4, 2 },
  { 1, 2, 1, 1, 4, 2 },
  { 1, 2, 1, 2, 4, 1 },
  { 1, 1, 4, 2, 1, 2 },
  { 1, 2, 4, 1, 1, 2 },
  { 1, 2, 4, 2, 1, 1 },
  { 4, 1, 1, 2, 1, 2 },
  { 4, 2, 1, 1, 1, 2 },
  { 4, 2, 1, 2, 1, 1 },
  { 2, 1, 2, 1, 4, 1 },
  { 2, 1, 4, 1, 2, 1 },
  { 4, 1, 2, 1, 2, 1 },
  { 1, 1, 1, 1, 4, 3 },
  { 1, 1, 1, 3, 4, 1 },
  { 1, 3, 1, 1, 4, 1 },
  { 1, 1, 4, 1, 1, 3 },
  { 1, 1, 4, 3, 1, 1 },
  { 4, 1, 1, 1, 1, 3 },
  { 4, 1, 1, 3, 1, 1 },
  { 1, 1, 3, 1, 4, 1 },
  { 1, 1, 4, 1, 3, 1 },
  { 3, 1, 1, 1, 4, 1 },
  { 4, 1, 1, 1, 3, 1 },
  { 2, 1, 1, 4, 1, 2 }, // start code A
  { 2, 1, 1, 2, 1, 4 }, // start code B
  { 2, 1, 1, 2, 3, 2 }, // start code C
  { 2, 3, 3, 1, 1, 1 }  // stop code (without final bar)
};

//------------------------------------------------------------------------

class XFATableInfo {
public:

  XFATableInfo(ZxAttr *columnWidthsAttr);
  ~XFATableInfo();
  void computeRowHeight(ZxElement *rowSubform);

  int nColumns;			// number of columns
  double *columnRight;		// right edge (x coord) of each column
  int rowIdx;			// current row index
  int columnIdx;		// current column index
  double rowTop;		// top edge (y coord) of current row
  double rowHeight;		// height of current row (max cell height
				//   so far)
};

XFATableInfo::XFATableInfo(ZxAttr *columnWidthsAttr) {
  GString *s;
  double w;
  int i, columnRightSize;

  nColumns = 0;
  columnRight = NULL;
  columnRightSize = 0;
  if (columnWidthsAttr) {
    s = columnWidthsAttr->getValue();
    i = 0;
    while (1) {
      for (;
	   i < s->getLength() &&
	     (s->getChar(i) == ' ' || s->getChar(i) == '\t' ||
	      s->getChar(i) == '\r' || s->getChar(i) == '\n');
	   ++i) ;
      if (i == s->getLength()) {
	break;
      }
      w = XFAFormField::getMeasurement(s, i);
      if (nColumns == columnRightSize) {
	columnRightSize = columnRightSize ? 2 * columnRightSize : 8;
	columnRight = (double *)greallocn(columnRight, columnRightSize,
					  sizeof(double));
      }
      columnRight[nColumns] = (nColumns > 0 ? columnRight[nColumns - 1] : 0) + w;
      ++nColumns;
      for (++i;
	   i < s->getLength() &&
	     !(s->getChar(i) == ' ' || s->getChar(i) == '\t' ||
	       s->getChar(i) == '\r' || s->getChar(i) == '\n');
	   ++i) ;
    }
  }
  rowIdx = -1;
  columnIdx = 0;
  rowTop = rowHeight = 0;
}

XFATableInfo::~XFATableInfo() {
  gfree(columnRight);
}

void XFATableInfo::computeRowHeight(ZxElement *rowSubform) {
  ZxNode *child;
  ZxAttr *attr;
  double h;

  rowHeight = 0;
  for (child = rowSubform->getFirstChild();
       child;
       child = child->getNextChild()) {
    if (child->isElement("field") || child->isElement("draw")) {
      if (!(attr = ((ZxElement *)child)->findAttr("h"))) {
	attr = ((ZxElement *)child)->findAttr("minH");
      }
      h = XFAFormField::getMeasurement(attr,  0);
      if (h > rowHeight) {
	rowHeight = h;
      }
    }
  }
}

//------------------------------------------------------------------------
// XFAForm
//------------------------------------------------------------------------

XFAForm *XFAForm::load(PDFDoc *docA, Catalog *catalog,
		       Object *acroFormObj, Object *xfaObj) {
  XFAForm *xfaForm;
  XFAFormField *field;
  ZxDoc *xmlA;
  ZxElement *tmpl;
  Object catDict, resourceDictA, obj1;
  GString *data;
  GBool fullXFAA;
  GString *name, *fullName;
  GHash *nameCount, *nameIdx, *fullNameCount, *fullNameIdx;
  char buf[4096];
  int n, i;

  if (catalog->getNumPages() == 0) {
    return NULL;
  }

  docA->getXRef()->getCatalog(&catDict);
  catDict.dictLookup("NeedsRendering", &obj1);
  fullXFAA = obj1.isBool() && obj1.getBool();
  obj1.free();
  catDict.free();

  if (xfaObj->isStream()) {
    data = new GString();
    xfaObj->streamReset();
    while ((n = xfaObj->getStream()->getBlock(buf, sizeof(buf))) > 0) {
      data->append(buf, n);
    }
  } else if (xfaObj->isArray()) {
    data = new GString();
    for (i = 1; i < xfaObj->arrayGetLength(); i += 2) {
      if (!xfaObj->arrayGet(i, &obj1)->isStream()) {
	error(errSyntaxError, -1, "XFA array element is wrong type");
	obj1.free();
	delete data;
	return NULL;
      }
      obj1.streamReset();
      while ((n = obj1.getStream()->getBlock(buf, sizeof(buf))) > 0) {
	data->append(buf, n);
      }
      obj1.free();
    }
  } else {
    error(errSyntaxError, -1, "XFA object is wrong type");
    return NULL;
  }

  xmlA = ZxDoc::loadMem(data->getCString(), data->getLength());
  delete data;
  if (!xmlA) {
    error(errSyntaxError, -1, "Invalid XML in XFA form");
    return NULL;
  }

  if (acroFormObj->isDict()) {
    acroFormObj->dictLookup("DR", &resourceDictA);
  }

  xfaForm = new XFAForm(docA, catalog->getNumPages(),
			xmlA, &resourceDictA, fullXFAA);

  resourceDictA.free();

  if (xfaForm->xml->getRoot()) {
    if ((tmpl = xfaForm->xml->getRoot()->findFirstChildElement("template"))) {
      xfaForm->curPageNum = 0;
      xfaForm->curXOffset = xfaForm->curYOffset = 0;
      name = new GString();
      fullName = new GString();
      nameCount = new GHash();
      nameIdx = new GHash();
      fullNameCount = new GHash();
      fullNameIdx = new GHash();
      xfaForm->scanNode(tmpl, name, fullName, gFalse, NULL,
			nameCount, nameIdx, fullNameCount, fullNameIdx,
			NULL, catalog);
      delete nameCount;
      delete nameIdx;
      delete fullNameCount;
      delete fullNameIdx;
      delete name;
      delete fullName;

      // apply pageOffsetX/Y (the pageSet/pageArea/contentArea offset)
      // to all fields
      if (xfaForm->pageSetNPages >= 1 &&
	  xfaForm->pageSetNPages < xfaForm->nPages) {
	for (i = xfaForm->pageSetNPages + 1; i <= xfaForm->nPages; ++i) {
	  xfaForm->pageOffsetX[i - 1] =
	      xfaForm->pageOffsetX[xfaForm->pageSetNPages - 1];
	  xfaForm->pageOffsetY[i - 1] =
	      xfaForm->pageOffsetY[xfaForm->pageSetNPages - 1];
	}
      }
      for (i = 0; i < xfaForm->fields->getLength(); ++i) {
	field = (XFAFormField *)xfaForm->fields->get(i);
	if (field->pageNum >= 1 && field->pageNum <= xfaForm->nPages) {
	  field->xOffset += xfaForm->pageOffsetX[field->pageNum - 1];
	  field->yOffset += xfaForm->pageOffsetY[field->pageNum - 1];
	}
      }
    }
  }

  return xfaForm;
}

XFAForm::XFAForm(PDFDoc *docA, int nPagesA, ZxDoc *xmlA,
		 Object *resourceDictA, GBool fullXFAA):
  Form(docA) {
  int pg;

  xml = xmlA;
  fields = new GList();
  resourceDictA->copy(&resourceDict);
  fullXFA = fullXFAA;
  nPages = nPagesA;
  pageSetNPages = 0;
  pageOffsetX = (double *)gmallocn(nPages, sizeof(double));
  pageOffsetY = (double *)gmallocn(nPages, sizeof(double));
  for (pg = 0; pg < nPages; ++pg) {
    pageOffsetX[pg] = pageOffsetY[pg] = 0;
  }
}

XFAForm::~XFAForm() {
  delete xml;
  deleteGList(fields, XFAFormField);
  resourceDict.free();
  gfree(pageOffsetX);
  gfree(pageOffsetY);
}

// Scan <elem>.  Constructs the node's name and full name.  If <elem>
// is a field, creates an XFAFormField; else scans <elem>'s children.
void XFAForm::scanNode(ZxElement *elem,
		       GString *parentName, GString *parentFullName,
		       GBool inPageSet, XFATableInfo *tableInfo,
		       GHash *nameCount, GHash *nameIdx,
		       GHash *fullNameCount, GHash *fullNameIdx,
		       GString *exclGroupName, Catalog *catalog) {
  ZxAttr *attr;
  GString *name, *fullName, *namePart, *fullNamePart;
  GHash *childNameCount, *childNameIdx, *childFullNameCount, *childFullNameIdx;
  int colSpan, i;

  if (elem->isElement("template")) {
    name = new GString("form");
    fullName = new GString("form");
    childNameCount = new GHash();
    scanNames(elem, childNameCount);
    childNameIdx = new GHash();
    childFullNameCount = new GHash();
    scanFullNames(elem, childFullNameCount);
    childFullNameIdx = new GHash();
  } else {
    if ((namePart = getNodeName(elem))) {
      name = GString::format("{0:t}.{1:t}", parentName, namePart);
      if (nameCount->lookupInt(namePart) > 1) {
	i = nameIdx->lookupInt(namePart);
	name->appendf("[{0:d}]", i);
	nameIdx->replace(namePart, i + 1);
      }
      childNameCount = new GHash();
      scanNames(elem, childNameCount);
      childNameIdx = new GHash();
    } else {
      name = parentName->copy();
      childNameCount = nameCount;
      childNameIdx = nameIdx;
    }
    if ((fullNamePart = getNodeFullName(elem))) {
      fullName = GString::format("{0:t}.{1:t}", parentFullName, fullNamePart);
      if (fullNameCount->lookupInt(fullNamePart) > 1) {
	i = fullNameIdx->lookupInt(fullNamePart);
	fullName->appendf("[{0:d}]", i);
	fullNameIdx->replace(fullNamePart, i + 1);
      }
      childFullNameCount = new GHash();
      scanFullNames(elem, childFullNameCount);
      childFullNameIdx = new GHash();
    } else {
      fullName = parentFullName->copy();
      childFullNameCount = fullNameCount;
      childFullNameIdx = fullNameIdx;
    }
  }

  if (tableInfo && (elem->isElement("field") || elem->isElement("draw"))) {
    if ((attr = elem->findAttr("colSpan"))) {
      colSpan = atoi(attr->getValue()->getCString());
    } else {
      colSpan = 1;
    }
  } else {
    colSpan = 0;
  }

  if (elem->isElement("field")) {
    scanField(elem, name, fullName, exclGroupName,
	      inPageSet, tableInfo, colSpan, catalog);
  } else {
    scanNonField(elem, name, fullName, inPageSet, tableInfo, colSpan,
		 childNameCount, childNameIdx,
		 childFullNameCount, childFullNameIdx,
		 catalog);
  }

  if (tableInfo) {
    tableInfo->columnIdx += colSpan;
  }

  delete name;
  delete fullName;
  if (childNameCount != nameCount) {
    delete childNameCount;
  }
  if (childNameIdx != nameIdx) {
    delete childNameIdx;
  }
  if (childFullNameCount != fullNameCount) {
    delete childFullNameCount;
  }
  if (childFullNameIdx != fullNameIdx) {
    delete childFullNameIdx;
  }
}

// Traverse all children of <elem>, incrementing nameCount[name] for
// each named child.  Traversal stops at each named child.
void XFAForm::scanNames(ZxElement *elem, GHash *nameCount) {
  ZxNode *node;
  ZxElement *child;
  GString *namePart;

  for (node = elem->getFirstChild(); node; node = node->getNextChild()) {
    if (node->isElement()) {
      child = (ZxElement *)node;
      if ((namePart = getNodeName(child))) {
	if (nodeIsBindGlobal(child)) {
	  nameCount->replace(namePart, 1);
	} else {
	  nameCount->replace(namePart, nameCount->lookupInt(namePart) + 1);
	}
      } else {
	scanNames(child, nameCount);
      }
    }
  }
}

// Traverse all children of <elem>, incrementing fullNameCount[name]
// for each full-named child.  Traversal stops at each full-named
// child.
void XFAForm::scanFullNames(ZxElement *elem, GHash *fullNameCount) {
  ZxNode *node;
  ZxElement *child;
  GString *fullNamePart;

  for (node = elem->getFirstChild(); node; node = node->getNextChild()) {
    if (node->isElement()) {
      child = (ZxElement *)node;
      if ((fullNamePart = getNodeFullName(child))) {
	if (nodeIsBindGlobal(child)) {
	  fullNameCount->replace(fullNamePart, 1);
	} else {
	  fullNameCount->replace(fullNamePart,
				 fullNameCount->lookupInt(fullNamePart) + 1);
	}
      } else {
	scanFullNames(child, fullNameCount);
      }
    }
  }
}

void XFAForm::scanField(ZxElement *elem, GString *name, GString *fullName,
			GString *exclGroupName, GBool inPageSet,
			XFATableInfo *tableInfo, int colSpan,
			Catalog *catalog) {
  double xSubOffset, ySubOffset, columnWidth, rowHeight;

  if (curPageNum == 0) {
    curPageNum = 1;
  }

  xSubOffset = ySubOffset = 0;
  columnWidth = rowHeight = 0;
  if (tableInfo) {
    if (tableInfo->columnIdx > 0 &&
	tableInfo->columnIdx <= tableInfo->nColumns) {
      xSubOffset = tableInfo->columnRight[tableInfo->columnIdx - 1];
    }
    if (tableInfo->columnIdx + colSpan <= tableInfo->nColumns) {
      columnWidth = tableInfo->columnRight[tableInfo->columnIdx + colSpan - 1]
	            - xSubOffset;
    }
    ySubOffset = tableInfo->rowTop;
    rowHeight = tableInfo->rowHeight;
    curXOffset += xSubOffset;
    curYOffset += ySubOffset;
  }

  fields->append(new XFAFormField(this, elem, name->copy(), fullName->copy(),
				  exclGroupName ? exclGroupName->copy()
				                : (GString *)NULL,
				  curPageNum, curXOffset, curYOffset,
				  columnWidth, rowHeight));

  if (tableInfo) {
    curXOffset -= xSubOffset;
    curYOffset -= ySubOffset;
  }
}

void XFAForm::scanNonField(ZxElement *elem, GString *name, GString *fullName,
			   GBool inPageSet,
			   XFATableInfo *tableInfo, int colSpan,
			   GHash *nameCount, GHash *nameIdx,
			   GHash *fullNameCount, GHash *fullNameIdx,
			   Catalog *catalog) {
  XFATableInfo *newTableInfo;
  ZxElement *brk, *contentArea;
  ZxNode *child;
  ZxAttr *attr;
  PDFRectangle *box;
  GString *exclGroupName;
  double xSubOffset, ySubOffset;
  int savedPageNum;

  newTableInfo = tableInfo;

  if (elem->isElement("subform")) {

    // update page number
    if (((brk = elem->findFirstChildElement("breakBefore")) &&
	 (attr = brk->findAttr("targetType")) &&
	 !attr->getValue()->cmp("pageArea")) ||
	((brk = elem->findFirstChildElement("break")) &&
	 (attr = brk->findAttr("before")) &&
	 !attr->getValue()->cmp("pageArea")) ||
	(curPageNum < nPages &&
	 (attr = elem->findAttr("w")) &&
	 (box = catalog->getPage(curPageNum + 1)->getMediaBox()) &&
	 XFAFormField::getMeasurement(attr, 0) == box->x2 - box->x1 &&
	 (attr = elem->findAttr("h")) &&
	 XFAFormField::getMeasurement(attr, 0) == box->y2 - box->y1)) {
      if (curPageNum < nPages) {
	++curPageNum;
      }
    }

    // update tableInfo
    if ((attr = elem->findAttr("layout"))) {
      if (!attr->getValue()->cmp("table")) {
	newTableInfo = new XFATableInfo(elem->findAttr("columnWidths"));
	newTableInfo->rowIdx = -1;
	newTableInfo->columnIdx = 0;
      } else if (tableInfo && !attr->getValue()->cmp("row")) {
	++tableInfo->rowIdx;
	tableInfo->columnIdx = 0;
	tableInfo->rowTop += tableInfo->rowHeight;
	tableInfo->computeRowHeight(elem);
      }
    }

    // update position
    xSubOffset = XFAFormField::getMeasurement(elem->findAttr("x"), 0);
    ySubOffset = XFAFormField::getMeasurement(elem->findAttr("y"), 0);
    curXOffset += xSubOffset;
    curYOffset += ySubOffset;

  } else if (elem->isElement("area") ||
	     elem->isElement("exclGroup")) {
    xSubOffset = XFAFormField::getMeasurement(elem->findAttr("x"), 0);
    ySubOffset = XFAFormField::getMeasurement(elem->findAttr("y"), 0);
    curXOffset += xSubOffset;
    curYOffset += ySubOffset;

  } else {
    xSubOffset = ySubOffset = 0;
  }

  savedPageNum = curPageNum;
  if (elem->isElement("pageSet")) {
    inPageSet = gTrue;
    curPageNum = 0;

  } else if (elem->isElement("pageArea")) {
    if (inPageSet) {
      if (curPageNum < nPages) {
	++curPageNum;
      }
      if ((contentArea = elem->findFirstChildElement("contentArea"))) {
	pageOffsetX[curPageNum - 1] =
	    XFAFormField::getMeasurement(contentArea->findAttr("x"), 0);
	pageOffsetY[curPageNum - 1] =
	    XFAFormField::getMeasurement(contentArea->findAttr("y"), 0);
	// looks like the contentArea offset (pageOffsetX/Y) should
	// not be added to fields defined inside the pageArea
	// element (?)
	xSubOffset -= pageOffsetX[curPageNum - 1];
	ySubOffset -= pageOffsetY[curPageNum - 1];
	curXOffset -= pageOffsetX[curPageNum - 1];
	curYOffset -= pageOffsetY[curPageNum - 1];
      }
    }
  }

  if (elem->isElement("exclGroup")) {
    exclGroupName = name;
  } else {
    exclGroupName = NULL;
  }

  for (child = elem->getFirstChild(); child; child = child->getNextChild()) {
    if (child->isElement()) {
      scanNode((ZxElement *)child, name, fullName, inPageSet,
	       newTableInfo, nameCount, nameIdx, fullNameCount, fullNameIdx,
	       exclGroupName, catalog);
    }
  }

  curXOffset -= xSubOffset;
  curYOffset -= ySubOffset;

  if (newTableInfo != tableInfo) {
    delete newTableInfo;
  }

  if (elem->isElement("pageSet")) {
    pageSetNPages = curPageNum;
    curPageNum = savedPageNum;
    inPageSet = gFalse;
  }
}

GString *XFAForm::getNodeName(ZxElement *elem) {
  ZxElement *bindElem;
  ZxAttr *attr;

  if (!(elem->getType()->cmp("field") &&
	(bindElem = elem->findFirstChildElement("bind")) &&
	(attr = bindElem->findAttr("match")) &&
	!attr->getValue()->cmp("none")) &&
      !elem->isElement("area") &&
      (attr = elem->findAttr("name"))) {
    return attr->getValue();
  }
  return NULL;
}

GString *XFAForm::getNodeFullName(ZxElement *elem) {
  ZxAttr *attr;

  if (!elem->isElement("area") &&
      (attr = elem->findAttr("name"))) {
    return attr->getValue();
  }
  return NULL;
}

GBool XFAForm::nodeIsBindGlobal(ZxElement *elem) {
  ZxElement *bindElem;
  ZxAttr *attr;

  return (bindElem = elem->findFirstChildElement("bind")) &&
         (attr = bindElem->findAttr("match")) &&
         !attr->getValue()->cmp("global");
}

void XFAForm::draw(int pageNum, Gfx *gfx, GBool printing) {
  GfxFontDict *fontDict;
  Object obj1;
  int i;

  // build the font dictionary
  fontDict = NULL;
  if (resourceDict.isDict()) {
    if (resourceDict.dictLookup("Font", &obj1)->isDict()) {
      fontDict = new GfxFontDict(doc->getXRef(), NULL, obj1.getDict());
    }
    obj1.free();
  }

  for (i = 0; i < fields->getLength(); ++i) {
    ((XFAFormField *)fields->get(i))->draw(pageNum, gfx, printing, fontDict);
  }

  delete fontDict;
}

int XFAForm::getNumFields() {
  return fields->getLength();
}

FormField *XFAForm::getField(int idx) {
  return (XFAFormField *)fields->get(idx);
}

//------------------------------------------------------------------------
// XFAFormField
//------------------------------------------------------------------------

XFAFormField::XFAFormField(XFAForm *xfaFormA, ZxElement *xmlA,
			   GString *nameA, GString *fullNameA,
			   GString *exclGroupNameA,
			   int pageNumA, double xOffsetA, double yOffsetA,
			   double columnWidthA, double rowHeightA) {
  xfaForm = xfaFormA;
  xml = xmlA;
  name = nameA;
  fullName = fullNameA;
  exclGroupName = exclGroupNameA;
  pageNum = pageNumA;
  xOffset = xOffsetA;
  yOffset = yOffsetA;
  columnWidth = columnWidthA;
  rowHeight = rowHeightA;
}

XFAFormField::~XFAFormField() {
  delete name;
  delete fullName;
  if (exclGroupName) {
    delete exclGroupName;
  }
}

int XFAFormField::getPageNum() {
  return pageNum;
}

const char *XFAFormField::getType() {
  ZxElement *uiElem;
  ZxNode *node;

  if ((uiElem = xml->findFirstChildElement("ui"))) {
    for (node = uiElem->getFirstChild(); node; node = node->getNextChild()) {
      if (node->isElement("textEdit")) {
	return "Text";
      } else if (node->isElement("numericEdit")) {
	return "Numeric";
      } else if (node->isElement("dateTimeEdit")) {
	return "DateTime";
      } else if (node->isElement("choiceList")) {
	return "ChoiceList";
      } else if (node->isElement("checkButton")) {
	return "CheckButton";
      } else if (node->isElement("barcode")) {
	return "BarCode";
      }
      //~ other field types go here
    }
  }
  return NULL;
}

Unicode *XFAFormField::getName(int *length) {
  //~ assumes name is UTF-8
  return utf8ToUnicode(name, length);
}

Unicode *XFAFormField::getValue(int *length) {
  ZxElement *uiElem;
  ZxNode *node;
  GString *s;

  //~ assumes value is UTF-8
  s = NULL;
  if ((uiElem = xml->findFirstChildElement("ui"))) {
    for (node = uiElem->getFirstChild(); node; node = node->getNextChild()) {
      if (node->isElement("textEdit")) {
	s = getFieldValue("text");
	break;
      } else if (node->isElement("numericEdit")) {
	//~ not sure if this is correct
	s = getFieldValue("text");
	break;
      } else if (node->isElement("dateTimeEdit")) {
	s = getFieldValue("text");
	break;
      } else if (node->isElement("checkButton")) {
	if (!(s = getFieldValue("integer"))) {
	  s = getFieldValue("text");
	}
	break;
      } else if (node->isElement("barcode")) {
	s = getFieldValue("text");
	break;
      }
      //~ other field types go here
    }
  }
  if (!s) {
    return NULL;
  }
  return utf8ToUnicode(s, length);
}

Unicode *XFAFormField::utf8ToUnicode(GString *s, int *length) {
  Unicode *u;
  int n, size, c0, c1, c2, c3, c4, c5, i;

  n = size = 0;
  u = NULL;
  i = 0;
  while (i < s->getLength()) {
    if (n == size) {
      size = size ? size * 2 : 16;
      u = (Unicode *)greallocn(u, size, sizeof(Unicode));
    }
    c0 = s->getChar(i++) & 0xff;
    if (c0 <= 0x7f) {
      u[n++] = c0;
    } else if (c0 <= 0xdf && i < n) {
      c1 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x1f) << 6) | (c1 & 0x3f);
    } else if (c0 <= 0xef && i+1 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x0f) << 12) | ((c1 & 0x3f) << 6) | (c2 & 0x3f);
    } else if (c0 <= 0xf7 && i+2 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      c3 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x07) << 18) | ((c1 & 0x3f) << 12) | ((c2 & 0x3f) << 6)
	       | (c3 & 0x3f);
    } else if (c0 <= 0xfb && i+3 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      c3 = s->getChar(i++) & 0xff;
      c4 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x03) << 24) | ((c1 & 0x3f) << 18) | ((c2 & 0x3f) << 12)
	       | ((c3 & 0x3f) << 6) | (c4 & 0x3f);
    } else if (c0 <= 0xfd && i+4 < n) {
      c1 = s->getChar(i++) & 0xff;
      c2 = s->getChar(i++) & 0xff;
      c3 = s->getChar(i++) & 0xff;
      c4 = s->getChar(i++) & 0xff;
      c5 = s->getChar(i++) & 0xff;
      u[n++] = ((c0 & 0x01) << 30) | ((c1 & 0x3f) << 24) | ((c2 & 0x3f) << 18)
	       | ((c3 & 0x3f) << 12) | ((c4 & 0x3f) << 6) | (c5 & 0x3f);
    } else {
      u[n++] = '?';
    }
  }
  *length = n;
  return u;
}

void XFAFormField::getBBox(double *llx, double *lly,
			   double *urx, double *ury) {
  double xfaX, xfaY, xfaW, xfaH, pdfX, pdfY, pdfW, pdfH;
  int pdfRot;

  getRectangle(&xfaX, &xfaY, &xfaW, &xfaH,
	       &pdfX, &pdfY, &pdfW, &pdfH, &pdfRot);
  *llx = pdfX;
  *lly = pdfY;
  *urx = pdfX + pdfW;
  *ury = pdfY + pdfH;
}

void XFAFormField::getFont(Ref *fontID, double *fontSize) {
  ZxElement *fontElem;
  ZxAttr *attr;
  GBool bold, italic;

  fontID->num = fontID->gen = -1;
  *fontSize = 0;
  if ((fontElem = xml->findFirstChildElement("font"))) {
    bold = italic = gFalse;
    if ((attr = fontElem->findAttr("weight"))) {
      if (!attr->getValue()->cmp("bold")) {
	bold = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("posture"))) {
      if (!attr->getValue()->cmp("italic")) {
	italic = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("typeface"))) {
      *fontID = findFontName(attr->getValue(), bold, italic);
    }
    if ((attr = fontElem->findAttr("size"))) {
      *fontSize = getMeasurement(attr, 0);
    }
  }
}

void XFAFormField::getColor(double *red, double *green, double *blue) {
  ZxElement *fontElem, *fillElem, *colorElem;
  ZxAttr *attr;
  int r, g, b;

  *red = *green = *blue = 0;
  if ((fontElem = xml->findFirstChildElement("font"))) {
    if ((fillElem = fontElem->findFirstChildElement("fill"))) {
      if ((colorElem = fillElem->findFirstChildElement("color"))) {
	if ((attr = colorElem->findAttr("value"))) {
	  if (sscanf(attr->getValue()->getCString(), "%d,%d,%d",
		     &r, &g, &b) == 3) {
	    *red = r / 255.0;
	    *green = g / 255.0;
	    *blue = b / 255.0;
	  }
	}
      }
    }
  }
}

void XFAFormField::draw(int pageNumA, Gfx *gfx, GBool printing,
			GfxFontDict *fontDict) {
#ifndef PDF_PARSER_ONLY
  ZxElement *uiElem;
  ZxNode *node;
  ZxAttr *attr;
  GString *appearBuf;
  MemStream *appearStream;
  Object appearDict, appearance, resourceDict, resourceSubdict;
  Object fontResources, fontResource, defaultFont;
  Object obj1, obj2;
  char *resType, *fontName;
  double mat[6];
  double xfaX, xfaY, xfaW, xfaH, pdfX, pdfY, pdfW, pdfH;
  int rot3, i;

  if (pageNumA != pageNum) {
    return;
  }

  // check the 'presence' attribute
  if ((attr = xml->findAttr("presence"))) {
    if (!attr->getValue()->cmp("hidden") ||
	!attr->getValue()->cmp("invisible")) {
      return;
    }
  }

  getRectangle(&xfaX, &xfaY, &xfaW, &xfaH, &pdfX, &pdfY, &pdfW, &pdfH, &rot3);

  // generate transform matrix
  switch (rot3) {
  case 0:
  default:
    mat[0] = 1;  mat[1] = 0;
    mat[2] = 0;  mat[3] = 1;
    mat[4] = 0;  mat[5] = 0;
    break;
  case 90:
    mat[0] =  0;  mat[1] = 1;
    mat[2] = -1;  mat[3] = 0;
    mat[4] =  xfaH;  mat[5] = 0;
    break;
  case 180:
    mat[0] = -1;  mat[1] =  0;
    mat[2] =  0;  mat[3] = -1;
    mat[4] =  xfaW;  mat[5] =  xfaH;
    break;
  case 270:
    mat[0] = 0;  mat[1] = -1;
    mat[2] = 1;  mat[3] =  0;
    mat[4] = 0;  mat[5] =  xfaW;
    break;
  }

  // get the appearance stream data
  appearBuf = new GString();
#if 0 //~ for debugging
  double debugPad = 0;
  appearBuf->appendf("q 1 1 0 rg {0:.4f} {1:.4f} {2:.4f} {3:.4f} re f Q\n",
		     debugPad, debugPad, xfaW - 2*debugPad, xfaH - 2*debugPad);
#endif
  if ((uiElem = xml->findFirstChildElement("ui"))) {
    for (node = uiElem->getFirstChild(); node; node = node->getNextChild()) {
      if (node->isElement("textEdit") ||
	  node->isElement("numericEdit")) {
	drawTextEdit(fontDict, xfaW, xfaH, rot3, appearBuf);
	break;
      } else if (node->isElement("dateTimeEdit")) {
	drawTextEdit(fontDict, xfaW, xfaH, rot3, appearBuf);
	break;
      } else if (node->isElement("choiceList")) {
	drawTextEdit(fontDict, xfaW, xfaH, rot3, appearBuf);
	break;
      } else if (node->isElement("checkButton")) {
	drawCheckButton(fontDict, xfaW, xfaH, rot3, appearBuf);
	break;
      } else if (node->isElement("barcode")) {
	drawBarCode(fontDict, xfaW, xfaH, rot3, appearBuf);
	break;
      }
      //~ other field types go here
    }
  } else {
    drawTextEdit(fontDict, xfaW, xfaH, rot3, appearBuf);
  }

  // create the appearance stream
  appearDict.initDict(xfaForm->doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(xfaForm->doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(xfaW));
  obj1.arrayAdd(obj2.initReal(xfaH));
  appearDict.dictAdd(copyString("BBox"), &obj1);
  obj1.initArray(xfaForm->doc->getXRef());
  obj1.arrayAdd(obj2.initReal(mat[0]));
  obj1.arrayAdd(obj2.initReal(mat[1]));
  obj1.arrayAdd(obj2.initReal(mat[2]));
  obj1.arrayAdd(obj2.initReal(mat[3]));
  obj1.arrayAdd(obj2.initReal(mat[4]));
  obj1.arrayAdd(obj2.initReal(mat[5]));
  appearDict.dictAdd(copyString("Matrix"), &obj1);
  // NB: we need to deep-copy the resource dict and font resource
  // subdict, because the font subdict is modified, and multiple
  // threads can be sharing these objects.
  resourceDict.initDict(xfaForm->doc->getXRef());
  if (xfaForm->resourceDict.isDict()) {
    for (i = 0; i < xfaForm->resourceDict.dictGetLength(); ++i) {
      resType = xfaForm->resourceDict.dictGetKey(i);
      if (strcmp(resType, "Font")) {
	xfaForm->resourceDict.dictGetVal(i, &resourceSubdict);
	resourceDict.dictAdd(copyString(resType), &resourceSubdict);
      }
    }
  }
  fontResources.initDict(xfaForm->doc->getXRef());
  if (xfaForm->resourceDict.isDict() &&
      xfaForm->resourceDict.dictLookup("Font", &resourceSubdict)->isDict()) {
    for (i = 0; i < resourceSubdict.dictGetLength(); ++i) {
      fontName = resourceSubdict.dictGetKey(i);
      resourceSubdict.dictGetVal(i, &fontResource);
      fontResources.dictAdd(copyString(fontName), &fontResource);
    }
    resourceSubdict.free();
  }
  defaultFont.initDict(xfaForm->doc->getXRef());
  defaultFont.dictAdd(copyString("Type"), obj1.initName("Font"));
  defaultFont.dictAdd(copyString("Subtype"), obj1.initName("Type1"));
  defaultFont.dictAdd(copyString("BaseFont"), obj1.initName("Helvetica"));
  defaultFont.dictAdd(copyString("Encoding"), obj1.initName("WinAnsiEncoding"));
  fontResources.dictAdd(copyString("xpdf_default_font"), &defaultFont);
  resourceDict.dictAdd(copyString("Font"), &fontResources);
  appearDict.dictAdd(copyString("Resources"), &resourceDict);
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.initStream(appearStream);
  gfx->drawAnnot(&appearance, NULL, pdfX, pdfY, pdfX + pdfW, pdfY + pdfH);
  appearance.free();
  delete appearBuf;
#endif
}

void XFAFormField::getRectangle(double *xfaX, double *xfaY,
				double *xfaW, double *xfaH,
				double *pdfX, double *pdfY,
				double *pdfW, double *pdfH,
				int *pdfRot) {
  Page *page;
  PDFRectangle *pageRect;
  ZxElement *captionElem, *paraElem, *marginElem;
  ZxAttr *attr;
  double x2, y2, w2, h2, t;
  double anchorX, anchorY;
  int pageRot, rot;

  page = xfaForm->doc->getCatalog()->getPage(pageNum);
  pageRect = page->getMediaBox();
  pageRot = page->getRotate();

  anchorX = 0;
  anchorY = 0;
  if ((attr = xml->findAttr("anchorType"))) {
    if (!attr->getValue()->cmp("topLeft")) {
      anchorX = 0;
      anchorY = 0;
    } else if (!attr->getValue()->cmp("topCenter")) {
      anchorX = 0.5;
      anchorY = 0;
    } else if (!attr->getValue()->cmp("topRight")) {
      anchorX = 1;
      anchorY = 0;
    } else if (!attr->getValue()->cmp("middleLeft")) {
      anchorX = 0;
      anchorY = 0.5;
    } else if (!attr->getValue()->cmp("middleCenter")) {
      anchorX = 0.5;
      anchorY = 0.5;
    } else if (!attr->getValue()->cmp("middleRight")) {
      anchorX = 1;
      anchorY = 0.5;
    } else if (!attr->getValue()->cmp("bottomLeft")) {
      anchorX = 0;
      anchorY = 1;
    } else if (!attr->getValue()->cmp("bottomCenter")) {
      anchorX = 0.5;
      anchorY = 1;
    } else if (!attr->getValue()->cmp("bottomRight")) {
      anchorX = 1;
      anchorY = 1;
    }
  }
  *xfaX = getMeasurement(xml->findAttr("x"), 0) + xOffset;
  *xfaY = getMeasurement(xml->findAttr("y"), 0) + yOffset;
  if (!(attr = xml->findAttr("w"))) {
    attr = xml->findAttr("minW");
  }
  *xfaW = getMeasurement(attr, 0);
  if (*xfaW < columnWidth) {
    *xfaW = columnWidth;
  }
  if (!(attr = xml->findAttr("h"))) {
    attr = xml->findAttr("minH");
  }
  *xfaH = getMeasurement(attr, 0);
  if (*xfaH < rowHeight) {
    *xfaH = rowHeight;
  }
  if ((attr = xml->findAttr("rotate"))) {
    rot = atoi(attr->getValue()->getCString());
    if ((rot %= 360) < 0) {
      rot += 360;
    }
  } else {
    rot = 0;
  }

  // look for <caption reserve="...."> -- add the caption width/height
  // (plus a margin allowance)
  if ((captionElem = xml->findFirstChildElement("caption"))) {
    if ((attr = captionElem->findAttr("reserve"))) {
      t = getMeasurement(attr, 0);
      if ((attr = captionElem->findAttr("placement"))) {
	if (!attr->getValue()->cmp("left")) {
	  *xfaX += t + 1.5;
	  *xfaW -= t + 1.5;
	} else if (!attr->getValue()->cmp("right")) {
	  *xfaW -= t + 1.5;
	} else if (!attr->getValue()->cmp("top")) {
	  *xfaY += t;
	  *xfaH -= t;
	} else if (!attr->getValue()->cmp("bottom")) {
	  *xfaH -= t;
	}
      } else {
	// default is placement="left"
	*xfaX += t + 1.5;
	*xfaW -= t + 1.5;
      }
    }
  }

  // look for <margin>
  if ((marginElem = xml->findFirstChildElement("margin"))) {
    if ((attr = marginElem->findAttr("leftInset"))) {
      t = getMeasurement(attr, 0);
      *xfaX += t;
      *xfaW -= t;
    }
    if ((attr = marginElem->findAttr("rightInset"))) {
      t = getMeasurement(attr, 0);
      *xfaW -= t;
    }
    if ((attr = marginElem->findAttr("topInset"))) {
      t = getMeasurement(attr, 0);
      *xfaY += t;
      *xfaH -= t;
    }
    if ((attr = marginElem->findAttr("bottomInset"))) {
      t = getMeasurement(attr, 0);
      *xfaH -= t;
    }
  }

  // look for <para> -- add the margins
  if ((paraElem = xml->findFirstChildElement("para"))) {
    if ((attr = paraElem->findAttr("marginLeft"))) {
      t = getMeasurement(attr, 0);
      *xfaX += t;
      *xfaW -= t;
    }
    if ((attr = paraElem->findAttr("marginRight"))) {
      t = getMeasurement(attr, 0);
      *xfaW -= t;
    }
    if ((attr = paraElem->findAttr("spaceAbove"))) {
      t = getMeasurement(attr, 0);
      *xfaY += t;
      *xfaH -= t;
    }
    if ((attr = paraElem->findAttr("spaceBelow"))) {
      t = getMeasurement(attr, 0);
      *xfaH -= t;
    }
  }

  // get annot rect (UL corner, width, height) in XFA coords
  // notes:
  // - XFA coordinates are top-left origin, after page rotation
  // - XFA coordinates are dependent on choice of anchor point
  //   and field rotation
  switch (rot) {
  case 0:
  default:
    x2 = *xfaX - anchorX * *xfaW;
    y2 = *xfaY - anchorY * *xfaH;
    w2 = *xfaW;
    h2 = *xfaH;
    break;
  case 90:
    x2 = *xfaX - anchorY * *xfaH;
    y2 = *xfaY - (1 - anchorX) * *xfaW;
    w2 = *xfaH;
    h2 = *xfaW;
    break;
  case 180:
    x2 = *xfaX - (1 - anchorX) * *xfaW;
    y2 = *xfaY - (1 - anchorY) * *xfaH;
    w2 = *xfaW;
    h2 = *xfaH;
    break;
  case 270:
    x2 = *xfaX - (1 - anchorY) * *xfaH;
    y2 = *xfaY - anchorX * *xfaW;
    w2 = *xfaH;
    h2 = *xfaW;
    break;
  }

  // convert annot rect to PDF coords (LL corner, width, height),
  // taking page rotation into account
  switch (pageRot) {
  case 0:
  default:
    *pdfX = pageRect->x1 + x2;
    *pdfY = pageRect->y2 - (y2 + h2);
    *pdfW = w2;
    *pdfH = h2;
    break;
  case 90:
    *pdfX = pageRect->x1 + y2;
    *pdfY = pageRect->y1 + x2;
    *pdfW = h2;
    *pdfH = w2;
    break;
  case 180:
    *pdfX = pageRect->x2 - (x2 + w2);
    *pdfY = pageRect->y1 + y2;
    *pdfW = w2;
    *pdfH = h2;
    break;
  case 270:
    *pdfX = pageRect->x2 - (y2 + h2);
    *pdfY = pageRect->y1 + (x2 + w2);
    *pdfW = h2;
    *pdfH = w2;
    break;
  }
  *pdfRot = (rot + pageRot) % 360;
}

void XFAFormField::drawTextEdit(GfxFontDict *fontDict,
				double w, double h, int rot,
				GString *appearBuf) {
  ZxElement *formatElem, *pictureElem;
  ZxNode *pictureChildElem;
  ZxElement *valueElem, *textElem, *uiElem, *textEditElem, *combElem;
  ZxElement *fontElem, *paraElem;
  ZxAttr *attr;
  GString *picture, *value, *formattedValue, *fontName;
  double fontSize;
  int maxChars, combCells;
  GBool multiLine, bold, italic;
  XFAHorizAlign hAlign;
  XFAVertAlign vAlign;

  if (!(value = getFieldValue("text"))) {
    return;
  }

  uiElem = xml->findFirstChildElement("ui");

  //--- picture formatting
  if (uiElem &&
      (formatElem = xml->findFirstChildElement("format")) &&
      (pictureElem = formatElem->findFirstChildElement("picture")) &&
      (pictureChildElem = pictureElem->getFirstChild()) &&
      pictureChildElem->isCharData()) {
    picture = ((ZxCharData *)pictureChildElem)->getData();
    if (uiElem->findFirstChildElement("dateTimeEdit")) {
      formattedValue = pictureFormatDateTime(value, picture);
    } else if (uiElem->findFirstChildElement("numericEdit")) {
      formattedValue = pictureFormatNumber(value, picture);
    } else if (uiElem->findFirstChildElement("textEdit")) {
      formattedValue = pictureFormatText(value, picture);
    } else {
      formattedValue = value->copy();
    }
  } else {
    formattedValue = value->copy();
  }

  maxChars = 0;
  if ((valueElem = xml->findFirstChildElement("value")) &&
      (textElem = valueElem->findFirstChildElement("text")) &&
      (attr = textElem->findAttr("maxChars"))) {
    maxChars = atoi(attr->getValue()->getCString());
  }

  multiLine = gFalse;
  combCells = 0;
  if (uiElem &&
      (textEditElem = uiElem->findFirstChildElement("textEdit"))) {
    if ((attr = textEditElem->findAttr("multiLine")) &&
	!attr->getValue()->cmp("1")) {
      multiLine = gTrue;
    }
    if ((combElem = textEditElem->findFirstChildElement("comb"))) {
      if ((attr = combElem->findAttr("numberOfCells"))) {
	combCells = atoi(attr->getValue()->getCString());
      } else {
	combCells = maxChars;
      }
    }
  }
	
  fontName = NULL;
  fontSize = 10;
  bold = gFalse;
  italic = gFalse;
  if ((fontElem = xml->findFirstChildElement("font"))) {
    if ((attr = fontElem->findAttr("typeface"))) {
      fontName = attr->getValue()->copy();
    }
    if ((attr = fontElem->findAttr("weight"))) {
      if (!attr->getValue()->cmp("bold")) {
	bold = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("posture"))) {
      if (!attr->getValue()->cmp("italic")) {
	italic = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("size"))) {
      fontSize = getMeasurement(attr, fontSize);
    }
  }
  if (!fontName) {
    fontName = new GString("Courier");
  }

  hAlign = xfaHAlignLeft;
  vAlign = xfaVAlignTop;
  if ((paraElem = xml->findFirstChildElement("para"))) {
    if ((attr = paraElem->findAttr("hAlign"))) {
      if (!attr->getValue()->cmp("left")) {
	hAlign = xfaHAlignLeft;
      } else if (!attr->getValue()->cmp("center")) {
	hAlign = xfaHAlignCenter;
      } else if (!attr->getValue()->cmp("right")) {
	hAlign = xfaHAlignRight;
      }
      //~ other hAlign values (justify, justifyAll, radix) are
      //~   currently unsupported
    }
    if ((attr = paraElem->findAttr("vAlign"))) {
      if (!attr->getValue()->cmp("top")) {
	vAlign = xfaVAlignTop;
      } else if (!attr->getValue()->cmp("bottom")) {
	vAlign = xfaVAlignBottom;
      } else if (!attr->getValue()->cmp("middle")) {
	vAlign = xfaVAlignMiddle;
      }
    }
  }

  drawText(formattedValue, multiLine, combCells,
	   fontName, bold, italic, fontSize,
	   hAlign, vAlign, 0, 0, w, h, gFalse, fontDict, appearBuf);
  delete fontName;
  delete formattedValue;
}

void XFAFormField::drawCheckButton(GfxFontDict *fontDict,
				   double w, double h, int rot,
				   GString *appearBuf) {
  ZxElement *items;
  ZxNode *firstItem, *firstItemVal;
  const char *itemType;
  const char *onValue;
  GString *value;
  double x, y;

  // get the "on" value for this check button
  itemType = "integer";
  onValue = "1";
  if ((items = xml->findFirstChildElement("items"))) {
    if ((firstItem = items->getFirstChild())) {
      if (firstItem->isElement("text") && firstItem->getFirstChild()) {
	itemType = "text";
	if ((firstItemVal = firstItem->getFirstChild())->isCharData()) {
	  onValue = ((ZxCharData *)firstItemVal)->getData()->getCString();
	}
      } else if (firstItem->isElement("integer") &&
		 firstItem->getFirstChild()) {
	itemType = "integer";
	if ((firstItemVal = firstItem->getFirstChild())->isCharData()) {
	  onValue = ((ZxCharData *)firstItemVal)->getData()->getCString();
	}
      }
    }
  }

  // compare its value to the "on" value
  if (!(value = getFieldValue(itemType))) {
    return;
  }
  if (value->cmp(onValue)) {
    return;
  }

  // adjust shape to be square
  x = y = 0;
  if (w > h) {
    x += 0.5 * (w - h);
    w = h;
  } else {
    y += 0.5 * (h - w);
    h = w;
  }

  // margins
  x += 1.5;
  w -= 3;
  y += 1.5;
  h -= 3;

  //~ this should look at the "shape" and "mark" attributes

  appearBuf->appendf("0.5 w {0:.4f} {1:.4f} m {2:.4f} {3:.4f} l {0:.4f} {3:.4f} m {2:.4f} {1:.4f} l S\n",
		     x, y, x + w, y + h);
}

void XFAFormField::drawBarCode(GfxFontDict *fontDict,
			       double w, double h, int rot,
			       GString *appearBuf) {
#ifndef PDF_PARSER_ONLY
  ZxElement *uiElem, *barcodeElem, *fontElem;
  ZxAttr *attr;
  GString *value, *value2, *barcodeType, *textLocation, *fontName, *s1, *s2;
  XFAVertAlign textAlign;
  double wideNarrowRatio, moduleWidth, moduleHeight, fontSize;
  double yText, wText, yBarcode, hBarcode, wNarrow, xx;
  GBool doText;
  int dataLength, errorCorrectionLevel, checksum;
  GBool bold, italic;
  char *p;
  int i, j, c;

  //--- get field value
  if (!(value = getFieldValue("text"))) {
    return;
  }

  //--- get font
  fontName = NULL;
  fontSize = 0.2 * h;
  bold = gFalse;
  italic = gFalse;
  if ((fontElem = xml->findFirstChildElement("font"))) {
    if ((attr = fontElem->findAttr("typeface"))) {
      fontName = attr->getValue()->copy();
    }
    if ((attr = fontElem->findAttr("weight"))) {
      if (!attr->getValue()->cmp("bold")) {
	bold = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("posture"))) {
      if (!attr->getValue()->cmp("italic")) {
	italic = gTrue;
      }
    }
    if ((attr = fontElem->findAttr("size"))) {
      fontSize = getMeasurement(attr, fontSize);
    }
  }
  if (!fontName) {
    fontName = new GString("Courier");
  }

  //--- get field attributes
  barcodeType = NULL;
  wideNarrowRatio = 3;
  moduleWidth = 0;
  moduleHeight = 0;
  dataLength = 0;
  errorCorrectionLevel = 0;
  textLocation = NULL;
  if ((uiElem = xml->findFirstChildElement("ui")) &&
      (barcodeElem = uiElem->findFirstChildElement("barcode"))) {
    if ((attr = barcodeElem->findAttr("type"))) {
      barcodeType = attr->getValue();
    }
    if ((attr = barcodeElem->findAttr("wideNarrowRatio"))) {
      s1 = attr->getValue();
      if ((p = strchr(s1->getCString(), ':'))) {
	s2 = new GString(s1, 0, (int)(p - s1->getCString()));
	wideNarrowRatio = atof(p + 1);
	if (wideNarrowRatio == 0) {
	  wideNarrowRatio = 1;
	}
	wideNarrowRatio = atof(s2->getCString()) / wideNarrowRatio;
	delete s2;
      } else {
	wideNarrowRatio = atof(s1->getCString());
      }
    }
    if ((attr = barcodeElem->findAttr("moduleWidth"))) {
      moduleWidth = getMeasurement(attr, (0.25 / 25.4) * 72.0); // 0.25mm
    }
    if ((attr = barcodeElem->findAttr("moduleHeight"))) {
      moduleHeight = getMeasurement(attr, (0.5 / 25.4) * 72.0); // 0.5mm
    }
    if ((attr = barcodeElem->findAttr("dataLength"))) {
      dataLength = atoi(attr->getValue()->getCString());
    }
    if ((attr = barcodeElem->findAttr("errorCorrectionLevel"))) {
      errorCorrectionLevel = atoi(attr->getValue()->getCString());
    }
    if ((attr = barcodeElem->findAttr("textLocation"))) {
      textLocation = attr->getValue();
    }
  }
  if (!barcodeType) {
    error(errSyntaxError, -1, "Missing 'type' attribute in XFA barcode field");
    delete fontName;
    return;
  }

  //--- compute the embedded text type position
  doText = gTrue;
  yText = yBarcode = hBarcode = 0;
  if (textLocation && !textLocation->cmp("above")) {
    textAlign = xfaVAlignTop;
    yText = h;
    yBarcode = 0;
    hBarcode = h - fontSize;
  } else if (textLocation && !textLocation->cmp("belowEmbedded")) {
    textAlign = xfaVAlignBottom;
    yText = 0;
    yBarcode = 0;
    hBarcode = h;
  } else if (textLocation && !textLocation->cmp("aboveEmbedded")) {
    textAlign = xfaVAlignTop;
    yText = h;
    yBarcode = 0;
    hBarcode = h;
  } else if (textLocation && !textLocation->cmp("none")) {
    textAlign = xfaVAlignBottom; // make gcc happy
    doText = gFalse;
  } else { // default is "below"
    textAlign = xfaVAlignBottom;
    yText = 0;
    yBarcode = fontSize;
    hBarcode = h - fontSize;
  }
  wText = w;

  //--- remove extraneous start/stop chars
  value2 = value->copy();
  if (!barcodeType->cmp("code3Of9")) {
    if (value2->getLength() >= 1 && value2->getChar(0) == '*') {
      value2->del(0);
    }
    if (value2->getLength() >= 1 &&
	value2->getChar(value2->getLength() - 1) == '*') {
      value2->del(value2->getLength() - 1);
    }
  }

  //--- draw the bar code
  if (!barcodeType->cmp("code3Of9")) {
    if (!dataLength) {
      error(errSyntaxError, -1,
	    "Missing 'dataLength' attribute in XFA barcode field");
      goto err;
    }
    appearBuf->append("0 g\n");
    wNarrow = w / ((7 + 3 * wideNarrowRatio) * (dataLength + 2));
    xx = 0;
    for (i = -1; i <= value2->getLength(); ++i) {
      if (i < 0 || i >= value2->getLength()) {
	c = '*';
      } else {
	c = value2->getChar(i) & 0x7f;
      }
      for (j = 0; j < 10; j += 2) {
	appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} re f\n",
			   xx, yBarcode,
			   (code3Of9Data[c][j] ? wideNarrowRatio : 1) * wNarrow,
			   hBarcode);
	xx += ((code3Of9Data[c][j] ? wideNarrowRatio : 1) +
	       (code3Of9Data[c][j+1] ? wideNarrowRatio : 1)) * wNarrow;
      }
    }
    // center the text on the drawn barcode (not the max length barcode)
    wText = (value2->getLength() + 2) * (7 + 3 * wideNarrowRatio) * wNarrow;
  } else if (!barcodeType->cmp("code128B")) {
    if (!dataLength) {
      error(errSyntaxError, -1,
	    "Missing 'dataLength' attribute in XFA barcode field");
      goto err;
    }
    appearBuf->append("0 g\n");
    wNarrow = w / (11 * (dataLength + 3) + 2);
    xx = 0;
    checksum = 0;
    for (i = -1; i <= value2->getLength() + 1; ++i) {
      if (i == -1) {
	// start code B
	c = 104;
	checksum += c;
      } else if (i == value2->getLength()) {
	// checksum
	c = checksum % 103;
      } else if (i == value2->getLength() + 1) {
	// stop code
	c = 106;
      } else {
	c = value2->getChar(i) & 0xff;
	if (c >= 32 && c <= 127) {
	  c -= 32;
	} else {
	  c = 0;
	}	  
	checksum += (i + 1) * c;
      }
      for (j = 0; j < 6; j += 2) {
	appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} re f\n",
			   xx, yBarcode,
			   code128Data[c][j] * wNarrow, hBarcode);
	xx += (code128Data[c][j] + code128Data[c][j+1]) * wNarrow;
      }
    }
    // final bar of the stop code
    appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} re f\n",
		       xx, yBarcode, 2 * wNarrow, hBarcode);
    // center the text on the drawn barcode (not the max length barcode)
    wText = (11 * (value2->getLength() + 3) + 2) * wNarrow;
  } else if (!barcodeType->cmp("pdf417")) {
    drawPDF417Barcode(w, h, moduleWidth, moduleHeight, errorCorrectionLevel,
		      value2, appearBuf);
    doText = gFalse;
  } else {
    error(errSyntaxError, -1,
	  "Unimplemented barcode type '{0:t}' in XFA barcode field",
	  barcodeType);
  }
  //~ add other barcode types here

  //--- draw the embedded text
  if (doText) {
    appearBuf->append("0 g\n");
    drawText(value2, gFalse, 0,
	     fontName, bold, italic, fontSize,
	     xfaHAlignCenter, textAlign, 0, yText, wText, h, gTrue,
	     fontDict, appearBuf);
  }

 err:
  delete fontName;
  delete value2;
#endif
}

Object *XFAFormField::getResources(Object *res) {
  return xfaForm->resourceDict.copy(res);
}

double XFAFormField::getMeasurement(ZxAttr *attr, double defaultVal) {
  GString *s;

  if (!attr) {
    return defaultVal;
  }
  s = attr->getValue();
  return getMeasurement(s, 0);
}

double XFAFormField::getMeasurement(GString *s, int begin) {
  double val, mul;
  GBool neg;
  int i;

  i = begin;
  neg = gFalse;
  if (i < s->getLength() && s->getChar(i) == '+') {
    ++i;
  } else if (i < s->getLength() && s->getChar(i) == '-') {
    neg = gTrue;
    ++i;
  }
  val = 0;
  while (i < s->getLength() && s->getChar(i) >= '0' && s->getChar(i) <= '9') {
    val = val * 10 + s->getChar(i) - '0';
    ++i;
  }
  if (i < s->getLength() && s->getChar(i) == '.') {
    ++i;
    mul = 0.1;
    while (i < s->getLength() && s->getChar(i) >= '0' && s->getChar(i) <= '9') {
      val += mul * (s->getChar(i) - '0');
      mul *= 0.1;
      ++i;
    }
  }
  if (neg) {
    val = -val;
  }
  if (i+1 < s->getLength()) {
    if (s->getChar(i) == 'i' && s->getChar(i+1) == 'n') {
      val *= 72;
    } else if (s->getChar(i) == 'p' && s->getChar(i+1) == 't') {
      // no change
    } else if (s->getChar(i) == 'c' && s->getChar(i+1) == 'm') {
      val *= 72 / 2.54;
    } else if (s->getChar(i) == 'm' && s->getChar(i+1) == 'm') {
      val *= 72 / 25.4;
    } else {
      // default to inches
      val *= 72;
    }
  } else {
    // default to inches
    val *= 72;
  }
  return val;
}

GString *XFAFormField::getFieldValue(const char *valueChildType) {
  ZxElement *valueElem, *datasets, *data, *formElem, *elem;
  char *p;

  // check the <datasets> packet
  p = name->getCString();
  if (xfaForm->xml->getRoot() && !strncmp(p, "form.",  5)) {
    if ((datasets =
	 xfaForm->xml->getRoot()->findFirstChildElement("xfa:datasets")) &&
	(data = datasets->findFirstChildElement("xfa:data"))) {
      elem = findFieldInDatasets(data, p + 5);
      if (elem &&
	  elem->getFirstChild() &&
	  elem->getFirstChild()->isCharData() &&
	  ((ZxCharData *)elem->getFirstChild())->getData()->getLength() > 0) {
	return ((ZxCharData *)elem->getFirstChild())->getData();
      }
    }
  }
  if (exclGroupName) {
    p = exclGroupName->getCString();
    if (xfaForm->xml->getRoot() && !strncmp(p, "form.",  5)) {
      if ((datasets =
	   xfaForm->xml->getRoot()->findFirstChildElement("xfa:datasets")) &&
	  (data = datasets->findFirstChildElement("xfa:data"))) {
	elem = findFieldInDatasets(data, p + 5);
	if (elem &&
	    elem->getFirstChild() &&
	    elem->getFirstChild()->isCharData() &&
	    ((ZxCharData *)elem->getFirstChild())->getData()->getLength() > 0) {
	  return ((ZxCharData *)elem->getFirstChild())->getData();
	}
      }
    }
  }

  // check the <form> element
  p = fullName->getCString();
  if (xfaForm->xml->getRoot() && !strncmp(p, "form.",  5)) {
    if ((formElem = xfaForm->xml->getRoot()->findFirstChildElement("form"))) {
      elem = findFieldInFormElem(formElem, p + 5);
      if (elem &&
	  (valueElem = elem->findFirstChildElement("value")) &&
	  (elem = valueElem->findFirstChildElement(valueChildType))) {
	if (elem->getFirstChild() &&
	    elem->getFirstChild()->isCharData() &&
	    ((ZxCharData *)elem->getFirstChild())
	    ->getData()->getLength() > 0) {
	  return ((ZxCharData *)elem->getFirstChild())->getData();
	}
      }
    }
  }

  // check the <value> element within the field
  if ((valueElem = xml->findFirstChildElement("value")) &&
      (elem = valueElem->findFirstChildElement(valueChildType))) {
    if (elem->getFirstChild() &&
	elem->getFirstChild()->isCharData() &&
	((ZxCharData *)elem->getFirstChild())->getData()->getLength() > 0) {
      return ((ZxCharData *)elem->getFirstChild())->getData();
    }
  }

  return NULL;
}

ZxElement *XFAFormField::findFieldInDatasets(ZxElement *elem, char *partName) {
  ZxNode *node;
  ZxElement *result;
  GString *nodeName;
  char *next;
  int curIdx, idx, n;

  curIdx = 0;
  for (node = elem->getFirstChild(); node; node = node->getNextChild()) {
    if (node->isElement()) {
      nodeName = ((ZxElement *)node)->getType();
      n = nodeName->getLength();
      if (!strncmp(partName, nodeName->getCString(), n)) { 
	if (partName[n] == '[') {
	  idx = atoi(partName + n + 1);
	  if (idx == curIdx) {
	    for (++n; partName[n] && partName[n-1] != ']'; ++n) ;
	  } else {
	    ++curIdx;
	    continue;
	  }
	}
	if (!partName[n]) {
	  return (ZxElement *)node;
	} else if (partName[n] == '.') {
	  if ((result = findFieldInDatasets((ZxElement *)node,
					    partName + n + 1))) {
	    return result;
	  }
	  break;
	}
      }
    }
  }

  // search for an "ancestor match"
  if ((next = strchr(partName, '.'))) {
    return findFieldInDatasets(elem, next + 1);
  }

  return NULL;
}

ZxElement *XFAFormField::findFieldInFormElem(ZxElement *elem, char *partName) {
  ZxElement *elem2;
  ZxNode *node;
  ZxAttr *attr;
  GString *nodeName;
  int curIdx, idx, n;

  curIdx = 0;
  for (node = elem->getFirstChild(); node; node = node->getNextChild()) {
    if ((node->isElement("subform") || node->isElement("field")) &&
	((attr = ((ZxElement *)node)->findAttr("name")))) {
      nodeName = attr->getValue();
      n = nodeName->getLength();
      if (!strncmp(partName, nodeName->getCString(), n)) { 
	if (partName[n] == '[') {
	  idx = atoi(partName + n + 1);
	  if (idx == curIdx) {
	    for (++n; partName[n] && partName[n-1] != ']'; ++n) ;
	  } else {
	    ++curIdx;
	    continue;
	  }
	}
	if (!partName[n]) {
	  return (ZxElement *)node;
	} else if (partName[n] == '.') {
	  return findFieldInFormElem((ZxElement *)node, partName + n + 1);
	}
      }
    } else if (node->isElement("subform")) {
      if ((elem2 = findFieldInFormElem((ZxElement *)node, partName))) {
	return elem2;
      }
    }
  }
  return NULL;
}

void XFAFormField::transform(int rot, double w, double h,
			     double *wNew, double *hNew, GString *appearBuf) {
  switch (rot) {
  case 0:
  default:
    appearBuf->appendf("1 0 0 1 0 {0:.4f} cm\n", -h);
    break;
  case 90:
    appearBuf->appendf("0 1 -1 0 {0:.4f} 0 cm\n", w);
    *wNew = h;
    *hNew = w;
    break;
  case 180:
    appearBuf->appendf("-1 0 0 -1 {0:.4f} {1:.4f} cm\n", w, h);
    *wNew = w;
    *hNew = h;
    break;
  case 270:
    appearBuf->appendf("0 -1 1 0 0 {0:.4f} cm\n", h);
    *wNew = h;
    *hNew = w;
    break;
  }
}

void XFAFormField::drawText(GString *text, GBool multiLine, int combCells,
			    GString *fontName, GBool bold,
			    GBool italic, double fontSize,
			    XFAHorizAlign hAlign, XFAVertAlign vAlign,
			    double x, double y, double w, double h,
			    GBool whiteBackground,
			    GfxFontDict *fontDict, GString *appearBuf) {
  GString *text2;
  GfxFont *font;
  const char *fontTag;
  GString *s;
  Unicode u;
  double yTop, xx, yy, tw, charWidth, lineHeight;
  double ascent, descent, rectX, rectY, rectW, rectH, blkH;
  int nLines, line, i, j, k, c, rectI;

  // convert UTF-8 to Latin1
  //~ this currently drops all non-Latin1 characters
  text2 = new GString();
  i = 0;
  while (getUTF8(text, &i, &u)) {
    if (u <= 0xff) {
      text2->append((char)u);
    } else {
      text2->append('?');
    }
  }

  // find the font
  if ((font = findFont(fontDict, fontName, bold, italic))) {
    fontTag = font->getTag()->getCString();
    ascent = font->getAscent() * fontSize;
    descent = font->getDescent() * fontSize;
  } else {
    error(errSyntaxError, -1, "Couldn't find a font for '{0:t}', {1:s}, {2:s} used in XFA field",
	  fontName, bold ? "bold" : "non-bold",
	  italic ? "italic" : "non-italic");
    fontTag = "xpdf_default_font";
    ascent = 0.718 * fontSize;
    descent = -0.207 * fontSize;
  }

  // setup
  rectW = rectH = 0;
  rectI = appearBuf->getLength();
  appearBuf->append("BT\n");
  appearBuf->appendf("/{0:s} {1:.2f} Tf\n", fontTag, fontSize);

  // multi-line text
  if (multiLine) {

    // compute line height
    lineHeight = 1.2 * fontSize;

    // handle bottom/middle alignment
    if (vAlign == xfaVAlignBottom || vAlign == xfaVAlignMiddle) {
      nLines = 0;
      i = 0;
      while (i < text2->getLength()) {
	getNextLine(text2, i, font, fontSize, w, &j, &tw, &k);
	++nLines;
	i = k;
      }
      blkH = nLines * lineHeight - (lineHeight - fontSize);
      if (vAlign == xfaVAlignBottom) {
	yTop = y + blkH;
      } else {
	yTop = y + 0.5 * (h + blkH);
      }
      if (yTop > y + h) {
	yTop = y + h;
      }
    } else {
      yTop = y + h;
    }

    // write a series of lines of text
    line = 0;
    i = 0;
    while (i < text2->getLength()) {

      getNextLine(text2, i, font, fontSize, w, &j, &tw, &k);
      if (tw > rectW) {
	rectW = tw;
      }

      // compute text start position
      switch (hAlign) {
      case xfaHAlignLeft:
      default:
	xx = x;
	break;
      case xfaHAlignCenter:
	xx = x + 0.5 * (w - tw);
	break;
      case xfaHAlignRight:
	xx = x + w - tw;
	break;
      }
      yy = yTop - line * lineHeight - ascent;

      // draw the line
      appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n", xx, yy);
      appearBuf->append('(');
      for (; i < j; ++i) {
	c = text2->getChar(i) & 0xff;
	if (c == '(' || c == ')' || c == '\\') {
	  appearBuf->append('\\');
	  appearBuf->append((char)c);
	} else if (c < 0x20 || c >= 0x80) {
	  appearBuf->appendf("\\{0:03o}", c);
	} else {
	  appearBuf->append((char)c);
	}
      }
      appearBuf->append(") Tj\n");

      // next line
      i = k;
      ++line;
    }
    rectH = line * lineHeight;
    rectY = y + h - rectH;
 
  // comb formatting
  } else if (combCells > 0) {

    // compute comb spacing
    tw = w / combCells;

    // compute text start position
    switch (hAlign) {
    case xfaHAlignLeft:
    default:
      xx = x;
      break;
    case xfaHAlignCenter:
      xx = x + (int)(0.5 * (combCells - text2->getLength())) * tw;
      break;
    case xfaHAlignRight:
      xx = x + w - text2->getLength() * tw;
      break;
    }
    rectW = text2->getLength() * tw;
    switch (vAlign) {
    case xfaVAlignTop:
    default:
      yy = y + h - ascent;
      break;
    case xfaVAlignMiddle:
      yy = y + 0.5 * (h - (ascent + descent));
      break;
    case xfaVAlignBottom:
      yy = y - descent;
      break;
    }
    rectY = yy + descent;
    rectH = ascent - descent;

    // write the text string
    for (i = 0; i < text2->getLength(); ++i) {
      c = text2->getChar(i) & 0xff;
      if (font && !font->isCIDFont()) {
	charWidth = fontSize * ((Gfx8BitFont *)font)->getWidth((Guchar)c);
	appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n",
			   xx + i * tw + 0.5 * (tw - charWidth), yy);
      } else {
	appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n",
			   xx + i * tw, yy);
      }
      appearBuf->append('(');
      if (c == '(' || c == ')' || c == '\\') {
	appearBuf->append('\\');
	appearBuf->append((char)c);
      } else if (c < 0x20 || c >= 0x80) {
	appearBuf->appendf("{0:.4f} 0 Td\n", w);
      } else {
	appearBuf->append((char)c);
      }
      appearBuf->append(") Tj\n");
    }

  // regular (non-comb) formatting
  } else {

    // compute string width
    if (font && !font->isCIDFont()) {
      tw = 0;
      for (i = 0; i < text2->getLength(); ++i) {
	tw += ((Gfx8BitFont *)font)->getWidth(text2->getChar(i));
      }
    } else {
      // otherwise, make a crude estimate
      tw = text2->getLength() * 0.5;
    }
    tw *= fontSize;
    rectW = tw;

    // compute text start position
    switch (hAlign) {
    case xfaHAlignLeft:
    default:
      xx = x;
      break;
    case xfaHAlignCenter:
      xx = x + 0.5 * (w - tw);
      break;
    case xfaHAlignRight:
      xx = x + w - tw;
      break;
    }
    switch (vAlign) {
    case xfaVAlignTop:
    default:
      yy = y + h - ascent;
      break;
    case xfaVAlignMiddle:
      yy = y + 0.5 * (h - (ascent + descent));
      break;
    case xfaVAlignBottom:
      yy = y - descent;
      break;
    }
    rectY = yy + descent;
    rectH = ascent - descent;
    appearBuf->appendf("{0:.4f} {1:.4f} Td\n", xx, yy);

    // write the text string
    appearBuf->append('(');
    for (i = 0; i < text2->getLength(); ++i) {
      c = text2->getChar(i) & 0xff;
      if (c == '(' || c == ')' || c == '\\') {
	appearBuf->append('\\');
	appearBuf->append((char)c);
      } else if (c < 0x20 || c >= 0x80) {
	appearBuf->appendf("\\{0:03o}", c);
      } else {
	appearBuf->append((char)c);
      }
    }
    appearBuf->append(") Tj\n");
  }

  // cleanup
  appearBuf->append("ET\n");

  // draw a white rectangle behind the text
  if (whiteBackground) {
    switch (hAlign) {
    case xfaHAlignLeft:
    default:
      rectX = x;
      break;
    case xfaHAlignCenter:
      rectX = x + 0.5 * (w - rectW);
      break;
    case xfaHAlignRight:
      rectX = x + w - rectW;
      break;
    }
    rectX -= 0.25 * fontSize;
    rectW += 0.5 * fontSize;
    rectY -= 0.1 * fontSize;
    rectH += 0.2 * fontSize;
    s = GString::format("q 1 g {0:.4f} {1:.4f} {2:.4f} {3:.4f} re f Q\n",
			rectX, rectY, rectW, rectH);
    appearBuf->insert(rectI, s);
    delete s;
  }

  delete text2;
}

// Searches <fontDict> for a font matching(<fontName>, <bold>,
// <italic>).
GfxFont *XFAFormField::findFont(GfxFontDict *fontDict, GString *fontName,
				GBool bold, GBool italic) {
  GString *reqName, *testName;
  GfxFont *font;
  GBool foundName, foundBold, foundItalic;
  char *p;
  char c;
  int i, j;

  if (!fontDict) {
    return NULL;
  }

  reqName = new GString();
  for (i = 0; i < fontName->getLength(); ++i) {
    c = fontName->getChar(i);
    if (c != ' ') {
      reqName->append(c);
    }
  }

  for (i = 0; i < fontDict->getNumFonts(); ++i) {
    font = fontDict->getFont(i);
    if (!font || !font->getName()) {
      continue;
    }
    testName = new GString();
    for (j = 0; j < font->getName()->getLength(); ++j) {
      c = font->getName()->getChar(j);
      if (c != ' ') {
	testName->append(c);
      }
    }
    foundName = foundBold = foundItalic = gFalse;
    for (p = testName->getCString(); *p; ++p) {
      if (!strncasecmp(p, reqName->getCString(), reqName->getLength())) {
	foundName = gTrue;
      }
      if (!strncasecmp(p, "bold", 4)) {
	foundBold = gTrue;
      }
      if (!strncasecmp(p, "italic", 6) || !strncasecmp(p, "oblique", 7)) {
	foundItalic = gTrue;
      }
    }
    delete testName;
    if (foundName && foundBold == bold && foundItalic == italic) {
      delete reqName;
      return font;
    }
  }

  delete reqName;
  return NULL;
}

// Searches the font resource dict for a font matching(<name>, <bold>,
// <italic>), and returns the font object ID.  This does not require a
// GfxFontDict, and it does not do any parsing of font objects beyond
// looking at BaseFont.
Ref XFAFormField::findFontName(GString *name, GBool bold, GBool italic) {
  Object fontDictObj, fontObj, baseFontObj, fontRef;
  Ref fontID;
  GString *reqName, *testName;
  GBool foundName, foundBold, foundItalic;
  char *p;
  char c;
  int i;

  fontID.num = fontID.gen = -1;

  // remove space chars from the requested name
  reqName = new GString();
  for (i = 0; i < name->getLength(); ++i) {
    c = name->getChar(i);
    if (c != ' ') {
      reqName->append(c);
    }
  }

  if (xfaForm->resourceDict.isDict()) {
    if (xfaForm->resourceDict.dictLookup("Font", &fontDictObj)->isDict()) {
      for (i = 0; i < fontDictObj.dictGetLength() && fontID.num < 0; ++i) {
	fontDictObj.dictGetVal(i, &fontObj);
	if (fontObj.dictLookup("BaseFont", &baseFontObj)->isName()) {

	  // remove space chars from the font name
	  testName = new GString();
	  for (p = baseFontObj.getName(); *p; ++p) {
	    if (*p != ' ') {
	      testName->append(p);
	    }
	  }

	  // compare the names
	  foundName = foundBold = foundItalic = gFalse;
	  for (p = testName->getCString(); *p; ++p) {
	    if (!strncasecmp(p, reqName->getCString(), reqName->getLength())) {
	      foundName = gTrue;
	    }
	    if (!strncasecmp(p, "bold", 4)) {
	      foundBold = gTrue;
	    }
	    if (!strncasecmp(p, "italic", 6) || !strncasecmp(p, "oblique", 7)) {
	      foundItalic = gTrue;
	    }
	  }
	  delete testName;
	  if (foundName && foundBold == bold && foundItalic == italic) {
	    if (fontObj.dictGetValNF(i, &fontRef)) {
	      fontID = fontRef.getRef();
	    }
	    fontRef.free();
	  }
	}

	baseFontObj.free();
	fontObj.free();
      }
    }
    fontDictObj.free();
  }
  delete reqName;

  return fontID;
}

// Figure out how much text will fit on the next line.  Returns:
// *end = one past the last character to be included
// *width = width of the characters start .. end-1
// *next = index of first character on the following line
void XFAFormField::getNextLine(GString *text, int start,
			       GfxFont *font, double fontSize, double wMax,
			       int *end, double *width, int *next) {
  double w, dw;
  int j, k, c;

  // figure out how much text will fit on the line
  //~ what does Adobe do with tabs?
  w = 0;
  for (j = start; j < text->getLength() && w <= wMax; ++j) {
    c = text->getChar(j) & 0xff;
    if (c == 0x0a || c == 0x0d) {
      break;
    }
    if (font && !font->isCIDFont()) {
      dw = ((Gfx8BitFont *)font)->getWidth((Guchar)c) * fontSize;
    } else {
      // otherwise, make a crude estimate
      dw = 0.5 * fontSize;
    }
    w += dw;
  }
  if (w > wMax) {
    for (k = j; k > start && text->getChar(k-1) != ' '; --k) ;
    for (; k > start && text->getChar(k-1) == ' '; --k) ;
    if (k > start) {
      j = k;
    }
    if (j == start) {
      // handle the pathological case where the first character is
      // too wide to fit on the line all by itself
      j = start + 1;
    }
  }
  *end = j;

  // compute the width
  w = 0;
  for (k = start; k < j; ++k) {
    if (font && !font->isCIDFont()) {
      dw = ((Gfx8BitFont *)font)->getWidth(text->getChar(k)) * fontSize;
    } else {
      // otherwise, make a crude estimate
      dw = 0.5 * fontSize;
    }
    w += dw;
  }
  *width = w;

  // next line
  while (j < text->getLength() && text->getChar(j) == ' ') {
    ++j;
  }
  if (j < text->getLength() && text->getChar(j) == 0x0d) {
    ++j;
  }
  if (j < text->getLength() && text->getChar(j) == 0x0a) {
    ++j;
  }
  *next = j;
}

//------------------------------------------------------------------------
// 'picture' formatting
//------------------------------------------------------------------------

class XFAPictureNode {
public:
  virtual ~XFAPictureNode() {}
  virtual GBool isLiteral() { return gFalse; }
  virtual GBool isSign() { return gFalse; }
  virtual GBool isDigit() { return gFalse; }
  virtual GBool isDecPt() { return gFalse; }
  virtual GBool isSeparator() { return gFalse; }
  virtual GBool isYear() { return gFalse; }
  virtual GBool isMonth() { return gFalse; }
  virtual GBool isDay() { return gFalse; }
  virtual GBool isHour() { return gFalse; }
  virtual GBool isMinute() { return gFalse; }
  virtual GBool isSecond() { return gFalse; }
  virtual GBool isChar() { return gFalse; }
};

class XFAPictureLiteral: public XFAPictureNode {
public:
  XFAPictureLiteral(GString *sA) { s = sA; }
  virtual ~XFAPictureLiteral() { delete s; }
  virtual GBool isLiteral() { return gTrue; }
  GString *s;
};

class XFAPictureSign: public XFAPictureNode {
public:
  XFAPictureSign(char cA) { c = cA; }
  virtual GBool isSign() { return gTrue; }
  char c;
};

class XFAPictureDigit: public XFAPictureNode {
public:
  XFAPictureDigit(char cA) { c = cA; pos = 0; }
  virtual GBool isDigit() { return gTrue; }
  char c;
  int pos;
};

class XFAPictureDecPt: public XFAPictureNode {
public:
  XFAPictureDecPt() { }
  virtual GBool isDecPt() { return gTrue; }
};

class XFAPictureSeparator: public XFAPictureNode {
public:
  XFAPictureSeparator() { }
  virtual GBool isSeparator() { return gTrue; }
};

class XFAPictureYear: public XFAPictureNode {
public:
  XFAPictureYear(int nDigitsA) { nDigits = nDigitsA; }
  virtual GBool isYear() { return gTrue; }
  int nDigits;
};

class XFAPictureMonth: public XFAPictureNode {
public:
  XFAPictureMonth(int nDigitsA) { nDigits = nDigitsA; }
  virtual GBool isMonth() { return gTrue; }
  int nDigits;
};

class XFAPictureDay: public XFAPictureNode {
public:
  XFAPictureDay(int nDigitsA) { nDigits = nDigitsA; }
  virtual GBool isDay() { return gTrue; }
  int nDigits;
};

class XFAPictureHour: public XFAPictureNode {
public:
  XFAPictureHour(GBool is24HourA, int nDigitsA)
    { is24Hour = is24HourA; nDigits = nDigitsA; }
  virtual GBool isHour() { return gTrue; }
  GBool is24Hour;
  int nDigits;
};

class XFAPictureMinute: public XFAPictureNode {
public:
  XFAPictureMinute(int nDigitsA) { nDigits = nDigitsA; }
  virtual GBool isMinute() { return gTrue; }
  int nDigits;
};

class XFAPictureSecond: public XFAPictureNode {
public:
  XFAPictureSecond(int nDigitsA) { nDigits = nDigitsA; }
  virtual GBool isSecond() { return gTrue; }
  int nDigits;
};

class XFAPictureChar: public XFAPictureNode {
public:
  XFAPictureChar() {}
  virtual GBool isChar() { return gTrue; }
};

GString *XFAFormField::pictureFormatDateTime(GString *value, GString *picture) {
  GList *pic;
  XFAPictureNode *node;
  GString *ret, *s;
  char c;
  int year, month, day, hour, min, sec;
  int len, picStart, picEnd, u, n, i, j;

  len = value->getLength();
  if (len == 0) {
    return value->copy();
  }

  //--- parse the value

  // expected format is yyyy(-mm(-dd)?)?Thh(:mm(:ss)?)?
  // where:
  // - the '-'s and ':'s are optional
  // - the 'T' is literal
  // - we're ignoring optional time zone info at the end
  // (if the value is not in this canonical format, we just punt and
  // return the value string)
  //~ another option would be to parse the value following the
  //~   <ui><picture> element
  year = month = day = hour = min = sec = 0;
  i = 0;
  if (!(i + 4 <= len && isValidInt(value, i, 4))) {
    return value->copy();
  }
  year = convertInt(value, i, 4);
  i += 4;
  if (i < len && value->getChar(i) == '-') {
    ++i;
  }
  if (i + 2 <= len && isValidInt(value, i, 2)) {
    month = convertInt(value, i, 2);
    i += 2;
    if (i < len && value->getChar(i) == '-') {
      ++i;
    }
    if (i + 2 <= len && isValidInt(value, i, 2)) {
      day = convertInt(value, i, 2);
      i += 2;
    }
  }
  if (i < len) {
    if (value->getChar(i) != 'T') {
      return value->copy();
    }
    ++i;
    if (!(i + 2 <= len && isValidInt(value, i, 2))) {
      return value->copy();
    }
    hour = convertInt(value, i, 2);
    i += 2;
    if (i < len && value->getChar(i) == ':') {
      ++i;
    }
    if (i + 2 <= len && isValidInt(value, i, 2)) {
      min = convertInt(value, i, 2);
      i += 2;
      if (i < len && value->getChar(i) == ':') {
	++i;
      }
      if (i + 2 <= len && isValidInt(value, i, 2)) {
	sec = convertInt(value, i, 2);
	i += 2;
      }
    }
  }
  if (i < len) {
    return value->copy();
  }

  //--- skip the category and locale in the picture

  picStart = 0;
  picEnd = picture->getLength();
  for (i = 0; i < picture->getLength(); ++i) {
    c = picture->getChar(i);
    if (c == '{') {
      picStart = i + 1;
      for (picEnd = picStart;
	   picEnd < picture->getLength() && picture->getChar(picEnd) != '}';
	   ++picEnd) ;
      break;
    } else if (!((c >= 'a' && c <= 'z') ||
		 (c >= 'A' && c <= 'Z') ||
		 c == '(' ||
		 c == ')')) {
      break;
    }
  }

  //--- parse the picture

  pic = new GList();
  i = picStart;
  while (i < picEnd) {
    c = picture->getChar(i);
    ++i;
    if (c == '\'') {
      s = new GString();
      while (i < picEnd) {
	c = picture->getChar(i);
	if (c == '\'') {
	  ++i;
	  if (i < picEnd && picture->getChar(i) == '\'') {
	    s->append('\'');
	    ++i;
	  } else {
	    break;
	  }
	} else if (c == '\\') {
	  ++i;
	  if (i == picEnd) {
	    break;
	  }
	  c = picture->getChar(i);
	  ++i;
	  if (c == 'u' && i+4 <= picEnd) {
	    u = 0;
	    for (j = 0; j < 4; ++j, ++i) {
	      c = picture->getChar(i);
	      u <<= 4;
	      if (c >= '0' && c <= '9') {
		u += c - '0';
	      } else if (c >= 'a' && c <= 'f') {
		u += c - 'a' + 10;
	      } else if (c >= 'A' && c <= 'F') {
		u += c - 'A' + 10;
	      }
	    }
	    //~ this should convert to UTF-8 (?)
	    if (u <= 0xff) {
	      s->append((char)u);
	    }
	  } else {
	    s->append(c);
	  }
	} else {
	  s->append(c);
	}
      }
      pic->append(new XFAPictureLiteral(s));
    } else if (c == ',' || c == '-' || c == ':' ||
	       c == '/' || c == '.' || c == ' ') {
      s = new GString();
      s->append(c);
      pic->append(new XFAPictureLiteral(s));
    } else if (c == 'Y') {
      for (n = 1; n < 4 && i < picEnd && picture->getChar(i) == 'Y'; ++n, ++i) ;
      pic->append(new XFAPictureYear(n));
    } else if (c == 'M') {
      for (n = 1; n < 2 && i < picEnd && picture->getChar(i) == 'M'; ++n, ++i) ;
      pic->append(new XFAPictureMonth(n));
    } else if (c == 'D') {
      for (n = 1; n < 2 && i < picEnd && picture->getChar(i) == 'D'; ++n, ++i) ;
      pic->append(new XFAPictureDay(n));
    } else if (c == 'h') {
      for (n = 1; n < 2 && i < picEnd && picture->getChar(i) == 'h'; ++n, ++i) ;
      pic->append(new XFAPictureHour(gFalse, n));
    } else if (c == 'H') {
      for (n = 1; n < 2 && i < picEnd && picture->getChar(i) == 'H'; ++n, ++i) ;
      pic->append(new XFAPictureHour(gTrue, n));
    } else if (c == 'M') {
      for (n = 1; n < 2 && i < picEnd && picture->getChar(i) == 'M'; ++n, ++i) ;
      pic->append(new XFAPictureMinute(n));
    } else if (c == 'S') {
      for (n = 1; n < 2 && i < picEnd && picture->getChar(i) == 'S'; ++n, ++i) ;
      pic->append(new XFAPictureSecond(n));
    }
  }

  //--- generate formatted text

  ret = new GString();
  for (i = 0; i < pic->getLength(); ++i) {
    node = (XFAPictureNode *)pic->get(i);
    if (node->isLiteral()) {
      ret->append(((XFAPictureLiteral *)node)->s);
    } else if (node->isYear()) {
      if (((XFAPictureYear *)node)->nDigits == 2) {
	if (year >= 1930 && year < 2030) {
	  ret->appendf("{0:02d}", year % 100);
	} else {
	  ret->append("??");
	}
      } else {
	ret->appendf("{0:04d}", year);
      }
    } else if (node->isMonth()) {
      if (((XFAPictureMonth *)node)->nDigits == 1) {
	ret->appendf("{0:d}", month);
      } else {
	ret->appendf("{0:02d}", month);
      }
    } else if (node->isDay()) {
      if (((XFAPictureDay *)node)->nDigits == 1) {
	ret->appendf("{0:d}", day);
      } else {
	ret->appendf("{0:02d}", day);
      }
    } else if (node->isHour()) {
      if (((XFAPictureHour *)node)->is24Hour) {
	n = hour;
      } else {
	n = hour % 12;
	if (n == 0) {
	  n = 12;
	}
      }
      if (((XFAPictureHour *)node)->nDigits == 1) {
	ret->appendf("{0:d}", n);
      } else {
	ret->appendf("{0:02d}", n);
      }
    } else if (node->isMinute()) {
      if (((XFAPictureMinute *)node)->nDigits == 1) {
	ret->appendf("{0:d}", min);
      } else {
	ret->appendf("{0:02d}", min);
      }
    } else if (node->isSecond()) {
      if (((XFAPictureSecond *)node)->nDigits == 1) {
	ret->appendf("{0:d}", sec);
      } else {
	ret->appendf("{0:02d}", sec);
      }
    }
  }
  deleteGList(pic, XFAPictureNode);

  return ret;
}

GString *XFAFormField::pictureFormatNumber(GString *value, GString *picture) {
  GList *pic;
  XFAPictureNode *node;
  GString *ret, *s;
  GBool neg, haveDigits;
  char c;
  int start, decPt, trailingZero, len;
  int picStart, picEnd, u, pos, i, j;

  len = value->getLength();
  if (len == 0) {
    return value->copy();
  }

  //--- parse the value

  // -nnnn.nnnn0000
  //  ^   ^    ^   ^
  //  |   |    |   +-- len
  //  |   |    +------ trailingZero
  //  |   +----------- decPt
  //  +--------------- start
  start = 0;
  neg = gFalse;
  if (value->getChar(start) == '-') {
    neg = gTrue;
    ++start;
  } else if (value->getChar(start) == '+') {
    ++start;
  }
  for (decPt = start; decPt < len && value->getChar(decPt) != '.'; ++decPt) ;
  for (trailingZero = len;
       trailingZero > decPt && value->getChar(trailingZero - 1) == '0';
       --trailingZero) ;

  //--- skip the category and locale in the picture

  picStart = 0;
  picEnd = picture->getLength();
  for (i = 0; i < picture->getLength(); ++i) {
    c = picture->getChar(i);
    if (c == '{') {
      picStart = i + 1;
      for (picEnd = picStart;
	   picEnd < picture->getLength() && picture->getChar(picEnd) != '}';
	   ++picEnd) ;
      break;
    } else if (!((c >= 'a' && c <= 'z') ||
		 (c >= 'A' && c <= 'Z') ||
		 c == '(' ||
		 c == ')')) {
      break;
    }
  }

  //--- parse the picture

  pic = new GList();
  i = picStart;
  while (i < picEnd) {
    c = picture->getChar(i);
    ++i;
    if (c == '\'') {
      s = new GString();
      while (i < picEnd) {
	c = picture->getChar(i);
	if (c == '\'') {
	  ++i;
	  if (i < picEnd && picture->getChar(i) == '\'') {
	    s->append('\'');
	    ++i;
	  } else {
	    break;
	  }
	} else if (c == '\\') {
	  ++i;
	  if (i == picEnd) {
	    break;
	  }
	  c = picture->getChar(i);
	  ++i;
	  if (c == 'u' && i+4 <= picEnd) {
	    u = 0;
	    for (j = 0; j < 4; ++j, ++i) {
	      c = picture->getChar(i);
	      u <<= 4;
	      if (c >= '0' && c <= '9') {
		u += c - '0';
	      } else if (c >= 'a' && c <= 'F') {
		u += c - 'a' + 10;
	      } else if (c >= 'A' && c <= 'F') {
		u += c - 'A' + 10;
	      }
	    }
	    //~ this should convert to UTF-8 (?)
	    if (u <= 0xff) {
	      s->append((char)u);
	    }
	  } else {
	    s->append(c);
	  }
	} else {
	  s->append(c);
	  ++i;
	}
      }
      pic->append(new XFAPictureLiteral(s));
    } else if (c == '-' || c == ':' || c == '/' || c == ' ') {
      s = new GString();
      s->append(c);
      pic->append(new XFAPictureLiteral(s));
    } else if (c == 's' || c == 'S') {
      pic->append(new XFAPictureSign(c));
    } else if (c == 'Z' || c == 'z' || c == '8' || c == '9') {
      pic->append(new XFAPictureDigit(c));
    } else if (c == '.') {
      pic->append(new XFAPictureDecPt());
    } else if (c == ',') {
      pic->append(new XFAPictureSeparator());
    }
  }
  for (i = 0; i < pic->getLength(); ++i) {
    node = (XFAPictureNode *)pic->get(i);
    if (node->isDecPt()) {
      break;
    }
  }
  pos = 0;
  for (j = i - 1; j >= 0; --j) {
    node = (XFAPictureNode *)pic->get(j);
    if (node->isDigit()) {
      ((XFAPictureDigit *)node)->pos = pos;
      ++pos;
    }
  }
  pos = -1;
  for (j = i + 1; j < pic->getLength(); ++j) {
    node = (XFAPictureNode *)pic->get(j);
    if (node->isDigit()) {
      ((XFAPictureDigit *)node)->pos = pos;
      --pos;
    }
  }

  //--- generate formatted text

  ret = new GString();
  haveDigits = gFalse;
  for (i = 0; i < pic->getLength(); ++i) {
    node = (XFAPictureNode *)pic->get(i);
    if (node->isLiteral()) {
      ret->append(((XFAPictureLiteral *)node)->s);
    } else if (node->isSign()) {
      if (((XFAPictureSign *)node)->c == 'S') {
	ret->append(neg ? '-' : ' ');
      } else {
	if (neg) {
	  ret->append('-');
	}
      }
    } else if (node->isDigit()) {
      pos = ((XFAPictureDigit *)node)->pos;
      c = ((XFAPictureDigit *)node)->c;
      if (pos >= 0 && pos < decPt - start) {
	ret->append(value->getChar(decPt - 1 - pos));
	haveDigits = gTrue;
      } else if (pos < 0 && -pos <= trailingZero - decPt - 1) {
	ret->append(value->getChar(decPt - pos));
	haveDigits = gTrue;
      } else if (c == '8' &&
		 pos < 0 &&
		 -pos <= len - decPt - 1) {
	ret->append('0');
	haveDigits = gTrue;
      } else if (c == '9') {
	ret->append('0');
	haveDigits = gTrue;
      } else if (c == 'Z' && pos >= 0) {
	ret->append(' ');
      }
    } else if (node->isDecPt()) {
      if (!(i+1 < pic->getLength() &&
	    ((XFAPictureNode *)pic->get(i+1))->isDigit() &&
	    ((XFAPictureDigit *)pic->get(i+1))->c == 'z') ||
	  trailingZero > decPt + 1) {
	ret->append('.');
      }
    } else if (node->isSeparator()) {
      if (haveDigits) {
	ret->append(',');
      }
    }
  }
  deleteGList(pic, XFAPictureNode);

  return ret;
}

GString *XFAFormField::pictureFormatText(GString *value, GString *picture) {
  GList *pic;
  XFAPictureNode *node;
  GString *ret, *s;
  char c;
  int len, picStart, picEnd, u, i, j;

  len = value->getLength();
  if (len == 0) {
    return value->copy();
  }

  //--- skip the category and locale in the picture

  picStart = 0;
  picEnd = picture->getLength();
  for (i = 0; i < picture->getLength(); ++i) {
    c = picture->getChar(i);
    if (c == '{') {
      picStart = i + 1;
      for (picEnd = picStart;
	   picEnd < picture->getLength() && picture->getChar(picEnd) != '}';
	   ++picEnd) ;
      break;
    } else if (!((c >= 'a' && c <= 'z') ||
		 (c >= 'A' && c <= 'Z') ||
		 c == '(' ||
		 c == ')')) {
      break;
    }
  }

  //--- parse the picture

  pic = new GList();
  i = picStart;
  while (i < picEnd) {
    c = picture->getChar(i);
    ++i;
    if (c == '\'') {
      s = new GString();
      while (i < picEnd) {
	c = picture->getChar(i);
	if (c == '\'') {
	  ++i;
	  if (i < picEnd && picture->getChar(i) == '\'') {
	    s->append('\'');
	    ++i;
	  } else {
	    break;
	  }
	} else if (c == '\\') {
	  ++i;
	  if (i == picEnd) {
	    break;
	  }
	  c = picture->getChar(i);
	  ++i;
	  if (c == 'u' && i+4 <= picEnd) {
	    u = 0;
	    for (j = 0; j < 4; ++j, ++i) {
	      c = picture->getChar(i);
	      u <<= 4;
	      if (c >= '0' && c <= '9') {
		u += c - '0';
	      } else if (c >= 'a' && c <= 'F') {
		u += c - 'a' + 10;
	      } else if (c >= 'A' && c <= 'F') {
		u += c - 'A' + 10;
	      }
	    }
	    //~ this should convert to UTF-8 (?)
	    if (u <= 0xff) {
	      s->append((char)u);
	    }
	  } else {
	    s->append(c);
	  }
	} else {
	  s->append(c);
	  ++i;
	}
      }
      pic->append(new XFAPictureLiteral(s));
    } else if (c == ',' || c == '-' || c == ':' ||
	       c == '/' || c == '.' || c == ' ') {
      s = new GString();
      s->append(c);
      pic->append(new XFAPictureLiteral(s));
    } else if (c == 'A' || c == 'X' || c == 'O' || c == '0' || c == '9') {
      pic->append(new XFAPictureChar());
    }
  }

  //--- generate formatted text

  ret = new GString();
  j = 0;
  for (i = 0; i < pic->getLength(); ++i) {
    node = (XFAPictureNode *)pic->get(i);
    if (node->isLiteral()) {
      ret->append(((XFAPictureLiteral *)node)->s);
    } else if (node->isChar()) {
      // if there are more chars in the picture than in the value,
      // Adobe renders the value as-is, without picture formatting
      if (j >= value->getLength()) {
	delete ret;
	ret = value->copy();
	break;
      }
      ret->append(value->getChar(j));
      ++j;
    }
  }
  deleteGList(pic, XFAPictureNode);

  return ret;
}

GBool XFAFormField::isValidInt(GString *s, int start, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    if (!(start + i < s->getLength() &&
	  s->getChar(start + i) >= '0' &&
	  s->getChar(start + i) <= '9')) {
      return gFalse;
    }
  }
  return gTrue;
}

int XFAFormField::convertInt(GString *s, int start, int len) {
  char c;
  int x, i;

  x = 0;
  for (i = 0; i < len && start + i < s->getLength(); ++i) {
    c = s->getChar(start + i);
    if (c < '0' || c > '9') {
      break;
    }
    x = x * 10 + (c - '0');
  }
  return x;
}
