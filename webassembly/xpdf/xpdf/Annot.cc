//========================================================================
//
// Annot.cc
//
// Copyright 2000-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <math.h>
#include "gmem.h"
#include "gmempp.h"
#include "GList.h"
#include "Error.h"
#include "Object.h"
#include "Catalog.h"
#include "Gfx.h"
#include "GfxFont.h"
#include "Lexer.h"
#include "PDFDoc.h"
#include "OptionalContent.h"
#include "Form.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "Annot.h"

// the MSVC math.h doesn't define this
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//------------------------------------------------------------------------

#define annotFlagHidden    0x0002
#define annotFlagPrint     0x0004
#define annotFlagNoView    0x0020

// distance of Bezier control point from center for circle approximation
// = (4 * (sqrt(2) - 1) / 3) * r
#define bezierCircle 0.55228475

#define lineEndSize1    6
#define lineEndSize2   10
#define lineArrowAngle (M_PI / 6)

//------------------------------------------------------------------------
// AnnotBorderStyle
//------------------------------------------------------------------------

AnnotBorderStyle::AnnotBorderStyle(AnnotBorderType typeA, double widthA,
				   double *dashA, int dashLengthA,
				   double *colorA, int nColorCompsA) {
  type = typeA;
  width = widthA;
  dash = dashA;
  dashLength = dashLengthA;
  color[0] = colorA[0];
  color[1] = colorA[1];
  color[2] = colorA[2];
  color[3] = colorA[3];
  nColorComps = nColorCompsA;
}

AnnotBorderStyle::~AnnotBorderStyle() {
  if (dash) {
    gfree(dash);
  }
}

//------------------------------------------------------------------------
// Annot
//------------------------------------------------------------------------

Annot::Annot(PDFDoc *docA, Dict *dict, Ref *refA) {
  Object apObj, asObj, obj1, obj2, obj3;
  AnnotBorderType borderType;
  double borderWidth;
  double *borderDash;
  int borderDashLength;
  double borderColor[4];
  int nBorderColorComps;
  double t;
  int i;

  ok = gTrue;
  doc = docA;
  xref = doc->getXRef();
  ref = *refA;
  type = NULL;
  appearanceState = NULL;
  appearBuf = NULL;
  borderStyle = NULL;

  //----- parse the type

  if (dict->lookup("Subtype", &obj1)->isName()) {
    type = new GString(obj1.getName());
  }
  obj1.free();

  //----- parse the rectangle

  if (dict->lookup("Rect", &obj1)->isArray() &&
      obj1.arrayGetLength() == 4) {
    xMin = yMin = xMax = yMax = 0;
    if (obj1.arrayGet(0, &obj2)->isNum()) {
      xMin = obj2.getNum();
    }
    obj2.free();
    if (obj1.arrayGet(1, &obj2)->isNum()) {
      yMin = obj2.getNum();
    }
    obj2.free();
    if (obj1.arrayGet(2, &obj2)->isNum()) {
      xMax = obj2.getNum();
    }
    obj2.free();
    if (obj1.arrayGet(3, &obj2)->isNum()) {
      yMax = obj2.getNum();
    }
    obj2.free();
    if (xMin > xMax) {
      t = xMin; xMin = xMax; xMax = t;
    }
    if (yMin > yMax) {
      t = yMin; yMin = yMax; yMax = t;
    }
  } else {
    error(errSyntaxError, -1, "Bad bounding box for annotation");
    ok = gFalse;
  }
  obj1.free();

  //----- parse the flags

  if (dict->lookup("F", &obj1)->isInt()) {
    flags = obj1.getInt();
  } else {
    flags = 0;
  }
  obj1.free();

  //----- parse the border style

  borderType = annotBorderSolid;
  borderWidth = 1;
  borderDash = NULL;
  borderDashLength = 0;
  nBorderColorComps = 3;
  borderColor[0] = 0;
  borderColor[1] = 0;
  borderColor[2] = 1;
  borderColor[3] = 0;
  if (dict->lookup("BS", &obj1)->isDict()) {
    if (obj1.dictLookup("S", &obj2)->isName()) {
      if (obj2.isName("S")) {
	borderType = annotBorderSolid;
      } else if (obj2.isName("D")) {
	borderType = annotBorderDashed;
      } else if (obj2.isName("B")) {
	borderType = annotBorderBeveled;
      } else if (obj2.isName("I")) {
	borderType = annotBorderInset;
      } else if (obj2.isName("U")) {
	borderType = annotBorderUnderlined;
      }
    }
    obj2.free();
    if (obj1.dictLookup("W", &obj2)->isNum()) {
      borderWidth = obj2.getNum();
    }
    obj2.free();
    if (obj1.dictLookup("D", &obj2)->isArray()) {
      borderDashLength = obj2.arrayGetLength();
      borderDash = (double *)gmallocn(borderDashLength, sizeof(double));
      for (i = 0; i < borderDashLength; ++i) {
	if (obj2.arrayGet(i, &obj3)->isNum()) {
	  borderDash[i] = obj3.getNum();
	} else {
	  borderDash[i] = 1;
	}
	obj3.free();
      }
    }
    obj2.free();
  } else {
    obj1.free();
    if (dict->lookup("Border", &obj1)->isArray()) {
      if (obj1.arrayGetLength() >= 3) {
	if (obj1.arrayGet(2, &obj2)->isNum()) {
	  borderWidth = obj2.getNum();
	}
	obj2.free();
	if (obj1.arrayGetLength() >= 4) {
	  if (obj1.arrayGet(3, &obj2)->isArray()) {
	    borderType = annotBorderDashed;
	    borderDashLength = obj2.arrayGetLength();
	    borderDash = (double *)gmallocn(borderDashLength, sizeof(double));
	    for (i = 0; i < borderDashLength; ++i) {
	      if (obj2.arrayGet(i, &obj3)->isNum()) {
		borderDash[i] = obj3.getNum();
	      } else {
		borderDash[i] = 1;
	      }
	      obj3.free();
	    }
	  } else {
	    // Adobe draws no border at all if the last element is of
	    // the wrong type.
	    borderWidth = 0;
	  }
	  obj2.free();
	}
      } else {
	// an empty Border array also means "no border"
	borderWidth = 0;
      }
    }
  }
  obj1.free();
  if (dict->lookup("C", &obj1)->isArray() &&
      (obj1.arrayGetLength() == 1 ||
       obj1.arrayGetLength() == 3 ||
       obj1.arrayGetLength() == 4)) {
    nBorderColorComps = obj1.arrayGetLength();
    for (i = 0; i < nBorderColorComps; ++i) {
      if (obj1.arrayGet(i, &obj2)->isNum()) {
	borderColor[i] = obj2.getNum();
      } else {
	borderColor[i] = 0;
      }
      obj2.free();
    }
  }
  obj1.free();
  borderStyle = new AnnotBorderStyle(borderType, borderWidth,
				     borderDash, borderDashLength,
				     borderColor, nBorderColorComps);

  //----- get the appearance state

  dict->lookup("AP", &apObj);
  dict->lookup("AS", &asObj);
  if (asObj.isName()) {
    appearanceState = new GString(asObj.getName());
  } else if (apObj.isDict()) {
    apObj.dictLookup("N", &obj1);
    if (obj1.isDict() && obj1.dictGetLength() == 1) {
      appearanceState = new GString(obj1.dictGetKey(0));
    }
    obj1.free();
  }
  if (!appearanceState) {
    appearanceState = new GString("Off");
  }
  asObj.free();

  //----- get the annotation appearance

  if (apObj.isDict()) {
    apObj.dictLookup("N", &obj1);
    apObj.dictLookupNF("N", &obj2);
    if (obj1.isDict()) {
      if (obj1.dictLookupNF(appearanceState->getCString(), &obj3)->isRef()) {
	obj3.copy(&appearance);
      }
      obj3.free();
    } else if (obj2.isRef()) {
      obj2.copy(&appearance);
    }
    obj1.free();
    obj2.free();
  }
  apObj.free();

  //----- get the optional content entry

  dict->lookupNF("OC", &ocObj);
}

Annot::~Annot() {
  if (type) {
    delete type;
  }
  if (appearanceState) {
    delete appearanceState;
  }
  appearance.free();
  if (appearBuf) {
    delete appearBuf;
  }
  if (borderStyle) {
    delete borderStyle;
  }
  ocObj.free();
}

void Annot::generateAnnotAppearance() {
  Object obj;

  appearance.fetch(doc->getXRef(), &obj);
  if (!obj.isStream()) {
    if (type) {
      if (!type->cmp("Line")) {
	generateLineAppearance();
      } else if (!type->cmp("PolyLine")) {
	generatePolyLineAppearance();
      } else if (!type->cmp("Polygon")) {
	generatePolygonAppearance();
      } else if (!type->cmp("FreeText")) {
	generateFreeTextAppearance();
      }
    }
  }
  obj.free();
}

//~ this doesn't draw the caption
void Annot::generateLineAppearance() {
  Object annotObj, gfxStateDict, appearDict, obj1, obj2;
  MemStream *appearStream;
  double x1, y1, x2, y2, dx, dy, len, w;
  double lx1, ly1, lx2, ly2;
  double tx1, ty1, tx2, ty2;
  double ax1, ay1, ax2, ay2;
  double bx1, by1, bx2, by2;
  double leaderLen, leaderExtLen, leaderOffLen;
  AnnotLineEndType lineEnd1, lineEnd2;
  GBool fill;

  if (!getObject(&annotObj)->isDict()) {
    annotObj.free();
    return;
  }

  appearBuf = new GString();

  //----- check for transparency
  if (annotObj.dictLookup("CA", &obj1)->isNum()) {
    gfxStateDict.initDict(doc->getXRef());
    gfxStateDict.dictAdd(copyString("ca"), obj1.copy(&obj2));
    appearBuf->append("/GS1 gs\n");
  }
  obj1.free();

  //----- set line style, colors
  setLineStyle(borderStyle, &w);
  setStrokeColor(borderStyle->getColor(), borderStyle->getNumColorComps());
  fill = gFalse;
  if (annotObj.dictLookup("IC", &obj1)->isArray()) {
    if (setFillColor(&obj1)) {
      fill = gTrue;
    }
  }
  obj1.free();

  //----- get line properties
  if (annotObj.dictLookup("L", &obj1)->isArray() &&
      obj1.arrayGetLength() == 4) {
    if (obj1.arrayGet(0, &obj2)->isNum()) {
      x1 = obj2.getNum();
    } else {
      obj2.free();
      obj1.free();
      goto err1;
    }
    obj2.free();
    if (obj1.arrayGet(1, &obj2)->isNum()) {
      y1 = obj2.getNum();
    } else {
      obj2.free();
      obj1.free();
      goto err1;
    }
    obj2.free();
    if (obj1.arrayGet(2, &obj2)->isNum()) {
      x2 = obj2.getNum();
    } else {
      obj2.free();
      obj1.free();
      goto err1;
    }
    obj2.free();
    if (obj1.arrayGet(3, &obj2)->isNum()) {
      y2 = obj2.getNum();
    } else {
      obj2.free();
      obj1.free();
      goto err1;
    }
    obj2.free();
  } else {
    obj1.free();
    goto err1;
  }
  obj1.free();
  lineEnd1 = lineEnd2 = annotLineEndNone;
  if (annotObj.dictLookup("LE", &obj1)->isArray() &&
      obj1.arrayGetLength() == 2) {
    lineEnd1 = parseLineEndType(obj1.arrayGet(0, &obj2));
    obj2.free();
    lineEnd2 = parseLineEndType(obj1.arrayGet(1, &obj2));
    obj2.free();
  }
  obj1.free();
  if (annotObj.dictLookup("LL", &obj1)->isNum()) {
    leaderLen = obj1.getNum();
  } else {
    leaderLen = 0;
  }
  obj1.free();
  if (annotObj.dictLookup("LLE", &obj1)->isNum()) {
    leaderExtLen = obj1.getNum();
  } else {
    leaderExtLen = 0;
  }
  obj1.free();
  if (annotObj.dictLookup("LLO", &obj1)->isNum()) {
    leaderOffLen = obj1.getNum();
  } else {
    leaderOffLen = 0;
  }
  obj1.free();

  //----- compute positions
  x1 -= xMin;
  y1 -= yMin;
  x2 -= xMin;
  y2 -= yMin;
  dx = x2 - x1;
  dy = y2 - y1;
  len = sqrt(dx*dx + dy*dy);
  if (len > 0) {
    dx /= len;
    dy /= len;
  }
  if (leaderLen != 0) {
    ax1 = x1 + leaderOffLen * dy;
    ay1 = y1 - leaderOffLen * dx;
    lx1 = ax1 + leaderLen * dy;
    ly1 = ay1 - leaderLen * dx;
    bx1 = lx1 + leaderExtLen * dy;
    by1 = ly1 - leaderExtLen * dx;
    ax2 = x2 + leaderOffLen * dy;
    ay2 = y2 - leaderOffLen * dx;
    lx2 = ax2 + leaderLen * dy;
    ly2 = ay2 - leaderLen * dx;
    bx2 = lx2 + leaderExtLen * dy;
    by2 = ly2 - leaderExtLen * dx;
  } else {
    lx1 = x1;
    ly1 = y1;
    lx2 = x2;
    ly2 = y2;
    ax1 = ay1 = ax2 = ay2 = 0; // make gcc happy
    bx1 = by1 = bx2 = by2 = 0;
  }
  adjustLineEndpoint(lineEnd1, lx1, ly1, dx, dy, w, &tx1, &ty1);
  adjustLineEndpoint(lineEnd2, lx2, ly2, -dx, -dy, w, &tx2, &ty2);

  //----- draw leaders
  if (leaderLen != 0) {
    appearBuf->appendf("{0:.4f} {1:.4f} m {2:.4f} {3:.4f} l\n",
		       ax1, ay1, bx1, by1);
    appearBuf->appendf("{0:.4f} {1:.4f} m {2:.4f} {3:.4f} l\n",
		       ax2, ay2 , bx2, by2);
  }

  //----- draw the line
  appearBuf->appendf("{0:.4f} {1:.4f} m {2:.4f} {3:.4f} l\n",
		     tx1, ty1, tx2, ty2);
  appearBuf->append("S\n");

  //----- draw the arrows
  if (borderStyle->getType() == annotBorderDashed) {
    appearBuf->append("[] 0 d\n");
  }
  drawLineArrow(lineEnd1, lx1, ly1, dx, dy, w, fill);
  drawLineArrow(lineEnd2, lx2, ly2, -dx, -dy, w, fill);

  //----- build the appearance stream dictionary
  appearDict.initDict(doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(xMax - xMin));
  obj1.arrayAdd(obj2.initReal(yMax - yMin));
  appearDict.dictAdd(copyString("BBox"), &obj1);
  if (gfxStateDict.isDict()) {
    obj1.initDict(doc->getXRef());
    obj2.initDict(doc->getXRef());
    obj2.dictAdd(copyString("GS1"), &gfxStateDict);
    obj1.dictAdd(copyString("ExtGState"), &obj2);
    appearDict.dictAdd(copyString("Resources"), &obj1);
  }

  //----- build the appearance stream
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.free();
  appearance.initStream(appearStream);

 err1:
  annotObj.free();
}

//~ this doesn't handle line ends (arrows)
void Annot::generatePolyLineAppearance() {
  Object annotObj, gfxStateDict, appearDict, obj1, obj2;
  MemStream *appearStream;
  double x1, y1, w;
  int i;

  if (!getObject(&annotObj)->isDict()) {
    annotObj.free();
    return;
  }

  appearBuf = new GString();

  //----- check for transparency
  if (annotObj.dictLookup("CA", &obj1)->isNum()) {
    gfxStateDict.initDict(doc->getXRef());
    gfxStateDict.dictAdd(copyString("ca"), obj1.copy(&obj2));
    appearBuf->append("/GS1 gs\n");
  }
  obj1.free();

  //----- set line style, colors
  setLineStyle(borderStyle, &w);
  setStrokeColor(borderStyle->getColor(), borderStyle->getNumColorComps());
  // fill = gFalse;
  // if (annotObj.dictLookup("IC", &obj1)->isArray()) {
  //   if (setFillColor(&obj1)) {
  //     fill = gTrue;
  //   }
  // }
  // obj1.free();

  //----- draw line
  if (!annotObj.dictLookup("Vertices", &obj1)->isArray()) {
    obj1.free();
    goto err1;
  }
  for (i = 0; i+1 < obj1.arrayGetLength(); i += 2) {
    if (!obj1.arrayGet(i, &obj2)->isNum()) {
      obj2.free();
      obj1.free();
      goto err1;
    }
    x1 = obj2.getNum();
    obj2.free();
    if (!obj1.arrayGet(i+1, &obj2)->isNum()) {
      obj2.free();
      obj1.free();
      goto err1;
    }
    y1 = obj2.getNum();
    obj2.free();
    x1 -= xMin;
    y1 -= yMin;
    if (i == 0) {
      appearBuf->appendf("{0:.4f} {1:.4f} m\n", x1, y1);
    } else {
      appearBuf->appendf("{0:.4f} {1:.4f} l\n", x1, y1);
    }
  }
  appearBuf->append("S\n");
  obj1.free();

  //----- build the appearance stream dictionary
  appearDict.initDict(doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(xMax - xMin));
  obj1.arrayAdd(obj2.initReal(yMax - yMin));
  appearDict.dictAdd(copyString("BBox"), &obj1);
  if (gfxStateDict.isDict()) {
    obj1.initDict(doc->getXRef());
    obj2.initDict(doc->getXRef());
    obj2.dictAdd(copyString("GS1"), &gfxStateDict);
    obj1.dictAdd(copyString("ExtGState"), &obj2);
    appearDict.dictAdd(copyString("Resources"), &obj1);
  }

  //----- build the appearance stream
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.free();
  appearance.initStream(appearStream);

 err1:
  annotObj.free();
}

void Annot::generatePolygonAppearance() {
  Object annotObj, gfxStateDict, appearDict, obj1, obj2;
  MemStream *appearStream;
  double x1, y1;
  int i;

  if (!getObject(&annotObj)->isDict()) {
    annotObj.free();
    return;
  }

  appearBuf = new GString();

  //----- check for transparency
  if (annotObj.dictLookup("CA", &obj1)->isNum()) {
    gfxStateDict.initDict(doc->getXRef());
    gfxStateDict.dictAdd(copyString("ca"), obj1.copy(&obj2));
    appearBuf->append("/GS1 gs\n");
  }
  obj1.free();

  //----- set fill color
  if (!annotObj.dictLookup("IC", &obj1)->isArray()  ||
      !setFillColor(&obj1)) {
    obj1.free();
    goto err1;
  }
  obj1.free();

  //----- fill polygon
  if (!annotObj.dictLookup("Vertices", &obj1)->isArray()) {
    obj1.free();
    goto err1;
  }
  for (i = 0; i+1 < obj1.arrayGetLength(); i += 2) {
    if (!obj1.arrayGet(i, &obj2)->isNum()) {
      obj2.free();
      obj1.free();
      goto err1;
    }
    x1 = obj2.getNum();
    obj2.free();
    if (!obj1.arrayGet(i+1, &obj2)->isNum()) {
      obj2.free();
      obj1.free();
      goto err1;
    }
    y1 = obj2.getNum();
    obj2.free();
    x1 -= xMin;
    y1 -= yMin;
    if (i == 0) {
      appearBuf->appendf("{0:.4f} {1:.4f} m\n", x1, y1);
    } else {
      appearBuf->appendf("{0:.4f} {1:.4f} l\n", x1, y1);
    }
  }
  appearBuf->append("f\n");
  obj1.free();

  //----- build the appearance stream dictionary
  appearDict.initDict(doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(xMax - xMin));
  obj1.arrayAdd(obj2.initReal(yMax - yMin));
  appearDict.dictAdd(copyString("BBox"), &obj1);
  if (gfxStateDict.isDict()) {
    obj1.initDict(doc->getXRef());
    obj2.initDict(doc->getXRef());
    obj2.dictAdd(copyString("GS1"), &gfxStateDict);
    obj1.dictAdd(copyString("ExtGState"), &obj2);
    appearDict.dictAdd(copyString("Resources"), &obj1);
  }

  //----- build the appearance stream
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.free();
  appearance.initStream(appearStream);

 err1:
  annotObj.free();
}

//~ this doesn't handle rich text
//~ this doesn't handle the callout
//~ this doesn't handle the RD field
void Annot::generateFreeTextAppearance() {
  Object annotObj, gfxStateDict, appearDict, obj1, obj2;
  Object resources, gsResources, fontResources, defaultFont;
  GString *text, *da;
  double lineWidth;
  int quadding, rot;
  MemStream *appearStream;

  if (!getObject(&annotObj)->isDict()) {
    annotObj.free();
    return;
  }

  appearBuf = new GString();

  //----- check for transparency
  if (annotObj.dictLookup("CA", &obj1)->isNum()) {
    gfxStateDict.initDict(doc->getXRef());
    gfxStateDict.dictAdd(copyString("ca"), obj1.copy(&obj2));
    appearBuf->append("/GS1 gs\n");
  }
  obj1.free();

  //----- draw the text
  if (annotObj.dictLookup("Contents", &obj1)->isString()) {
    text = obj1.getString()->copy();
  } else {
    text = new GString();
  }
  obj1.free();
  if (annotObj.dictLookup("Q", &obj1)->isInt()) {
    quadding = obj1.getInt();
  } else {
    quadding = 0;
  }
  obj1.free();
  if (annotObj.dictLookup("DA", &obj1)->isString()) {
    da = obj1.getString()->copy();
  } else {
    da = new GString();
  }
  obj1.free();
  // the "Rotate" field is not defined in the PDF spec, but Acrobat
  // looks at it
  if (annotObj.dictLookup("Rotate", &obj1)->isInt()) {
    rot = obj1.getInt();
  } else {
    rot = 0;
  }
  obj1.free();
  drawText(text, da, quadding, 0, rot);
  delete text;
  delete da;

  //----- draw the border
  if (borderStyle->getWidth() != 0) {
    setLineStyle(borderStyle, &lineWidth);
    appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} re s\n",
		       0.5 * lineWidth, 0.5 * lineWidth,
		       xMax - xMin - lineWidth, yMax - yMin - lineWidth);
  }

  //----- build the appearance stream dictionary
  appearDict.initDict(doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(xMax - xMin));
  obj1.arrayAdd(obj2.initReal(yMax - yMin));
  appearDict.dictAdd(copyString("BBox"), &obj1);
  resources.initDict(doc->getXRef());
  defaultFont.initDict(doc->getXRef());
  defaultFont.dictAdd(copyString("Type"), obj1.initName("Font"));
  defaultFont.dictAdd(copyString("Subtype"), obj1.initName("Type1"));
  defaultFont.dictAdd(copyString("BaseFont"), obj1.initName("Helvetica"));
  defaultFont.dictAdd(copyString("Encoding"), obj1.initName("WinAnsiEncoding"));
  fontResources.initDict(doc->getXRef());
  fontResources.dictAdd(copyString("xpdf_default_font"), &defaultFont);
  resources.dictAdd(copyString("Font"), &fontResources);
  if (gfxStateDict.isDict()) {
    gsResources.initDict(doc->getXRef());
    gsResources.dictAdd(copyString("GS1"), &gfxStateDict);
    resources.dictAdd(copyString("ExtGState"), &gsResources);
  }
  appearDict.dictAdd(copyString("Resources"), &resources);

  //----- build the appearance stream
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.free();
  appearance.initStream(appearStream);

  annotObj.free();
}

void Annot::setLineStyle(AnnotBorderStyle *bs, double *lineWidth) {
  double *dash;
  double w;
  int dashLength, i;

  if ((w = borderStyle->getWidth()) <= 0) {
    w = 0.1;
  }
  *lineWidth = w;
  appearBuf->appendf("{0:.4f} w\n", w);
  // this treats beveled/inset/underline as solid
  if (borderStyle->getType() == annotBorderDashed) {
    borderStyle->getDash(&dash, &dashLength);
    appearBuf->append("[");
    for (i = 0; i < dashLength; ++i) {
      appearBuf->appendf(" {0:.4f}", dash[i]);
    }
    appearBuf->append("] 0 d\n");
  }
  appearBuf->append("0 j\n0 J\n");
}

void Annot::setStrokeColor(double *color, int nComps) {
  switch (nComps) {
  case 0:
    appearBuf->append("0 G\n");
    break;
  case 1:
    appearBuf->appendf("{0:.2f} G\n", color[0]);
    break;
  case 3:
    appearBuf->appendf("{0:.2f} {1:.2f} {2:.2f} RG\n",
		       color[0], color[1], color[2]);
    break;
  case 4:
    appearBuf->appendf("{0:.2f} {1:.2f} {2:.2f} {3:.2f} K\n",
		       color[0], color[1], color[2], color[3]);
    break;
  }
}

GBool Annot::setFillColor(Object *colorObj) {
  Object obj;
  double color[4];
  int i;

  if (!colorObj->isArray()) {
    return gFalse;
  }
  for (i = 0; i < colorObj->arrayGetLength() && i < 4; ++i) {
    if (colorObj->arrayGet(i, &obj)->isNum()) {
      color[i] = obj.getNum();
    } else {
      color[i] = 0;
    }
    obj.free();
  }
  switch (colorObj->arrayGetLength()) {
  case 1:
    appearBuf->appendf("{0:.2f} g\n", color[0]);
    return gTrue;
  case 3:
    appearBuf->appendf("{0:.2f} {1:.2f} {2:.2f} rg\n",
		       color[0], color[1], color[2]);
    return gTrue;
  case 4:
    appearBuf->appendf("{0:.2f} {1:.2f} {2:.2f} {3:.3f} k\n",
		       color[0], color[1],
		       color[2], color[3]);
    return gTrue;
  }
  return gFalse;
}

AnnotLineEndType Annot::parseLineEndType(Object *obj) {
  if (obj->isName("None")) {
    return annotLineEndNone;
  } else if (obj->isName("Square")) {
    return annotLineEndSquare;
  } else if (obj->isName("Circle")) {
    return annotLineEndCircle;
  } else if (obj->isName("Diamond")) {
    return annotLineEndDiamond;
  } else if (obj->isName("OpenArrow")) {
    return annotLineEndOpenArrow;
  } else if (obj->isName("ClosedArrow")) {
    return annotLineEndClosedArrow;
  } else if (obj->isName("Butt")) {
    return annotLineEndButt;
  } else if (obj->isName("ROpenArrow")) {
    return annotLineEndROpenArrow;
  } else if (obj->isName("RClosedArrow")) {
    return annotLineEndRClosedArrow;
  } else if (obj->isName("Slash")) {
    return annotLineEndSlash;
  } else {
    return annotLineEndNone;
  }
}

void Annot::adjustLineEndpoint(AnnotLineEndType lineEnd,
			       double x, double y, double dx, double dy,
			       double w, double *tx, double *ty) {
  switch (lineEnd) {
  case annotLineEndNone:
    w = 0;
    break;
  case annotLineEndSquare:
    w *= lineEndSize1;
    break;
  case annotLineEndCircle:
    w *= lineEndSize1;
    break;
  case annotLineEndDiamond:
    w *= lineEndSize1;
    break;
  case annotLineEndOpenArrow:
    w = 0;
    break;
  case annotLineEndClosedArrow:
    w *= lineEndSize2 * cos(lineArrowAngle);
    break;
  case annotLineEndButt:
    w = 0;
    break;
  case annotLineEndROpenArrow:
    w *= lineEndSize2 * cos(lineArrowAngle);
    break;
  case annotLineEndRClosedArrow:
    w *= lineEndSize2 * cos(lineArrowAngle);
    break;
  case annotLineEndSlash:
    w = 0;
    break;
  }
  *tx = x + w * dx;
  *ty = y + w * dy;
}

void Annot::drawLineArrow(AnnotLineEndType lineEnd,
			  double x, double y, double dx, double dy,
			  double w, GBool fill) {
  switch (lineEnd) {
  case annotLineEndNone:
    break;
  case annotLineEndSquare:
    w *= lineEndSize1;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + w*dx + 0.5*w*dy,
		       y + w*dy - 0.5*w*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + 0.5*w*dy,
		       y - 0.5*w*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x - 0.5*w*dy,
		       y + 0.5*w*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + w*dx - 0.5*w*dy,
		       y + w*dy + 0.5*w*dx);
    appearBuf->append(fill ? "b\n" : "s\n");
    break;
  case annotLineEndCircle:
    w *= lineEndSize1;
    drawCircle(x + 0.5*w*dx, y + 0.5*w*dy, 0.5*w, fill ? "b" : "s");
    break;
  case annotLineEndDiamond:
    w *= lineEndSize1;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n", x, y);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + 0.5*w*dx - 0.5*w*dy,
		       y + 0.5*w*dy + 0.5*w*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + w*dx,
		       y + w*dy);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + 0.5*w*dx + 0.5*w*dy,
		       y + 0.5*w*dy - 0.5*w*dx);
    appearBuf->append(fill ? "b\n" : "s\n");
    break;
  case annotLineEndOpenArrow:
    w *= lineEndSize2;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + w*cos(lineArrowAngle)*dx + w*sin(lineArrowAngle)*dy,
		       y + w*cos(lineArrowAngle)*dy - w*sin(lineArrowAngle)*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n", x, y);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + w*cos(lineArrowAngle)*dx - w*sin(lineArrowAngle)*dy,
		       y + w*cos(lineArrowAngle)*dy + w*sin(lineArrowAngle)*dx);
    appearBuf->append("S\n");
    break;
  case annotLineEndClosedArrow:
    w *= lineEndSize2;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + w*cos(lineArrowAngle)*dx + w*sin(lineArrowAngle)*dy,
		       y + w*cos(lineArrowAngle)*dy - w*sin(lineArrowAngle)*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n", x, y);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + w*cos(lineArrowAngle)*dx - w*sin(lineArrowAngle)*dy,
		       y + w*cos(lineArrowAngle)*dy + w*sin(lineArrowAngle)*dx);
    appearBuf->append(fill ? "b\n" : "s\n");
    break;
  case annotLineEndButt:
    w *= lineEndSize1;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + 0.5*w*dy,
		       y - 0.5*w*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x - 0.5*w*dy,
		       y + 0.5*w*dx);
    appearBuf->append("S\n");
    break;
  case annotLineEndROpenArrow:
    w *= lineEndSize2;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + w*sin(lineArrowAngle)*dy,
		       y - w*sin(lineArrowAngle)*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + w*cos(lineArrowAngle)*dx,
		       y + w*cos(lineArrowAngle)*dy);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x - w*sin(lineArrowAngle)*dy,
		       y + w*sin(lineArrowAngle)*dx);
    appearBuf->append("S\n");
    break;
  case annotLineEndRClosedArrow:
    w *= lineEndSize2;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + w*sin(lineArrowAngle)*dy,
		       y - w*sin(lineArrowAngle)*dx);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x + w*cos(lineArrowAngle)*dx,
		       y + w*cos(lineArrowAngle)*dy);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x - w*sin(lineArrowAngle)*dy,
		       y + w*sin(lineArrowAngle)*dx);
    appearBuf->append(fill ? "b\n" : "s\n");
    break;
  case annotLineEndSlash:
    w *= lineEndSize1;
    appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		       x + 0.5*w*cos(lineArrowAngle)*dy
		         - 0.5*w*sin(lineArrowAngle)*dx,
		       y - 0.5*w*cos(lineArrowAngle)*dx
		         - 0.5*w*sin(lineArrowAngle)*dy);
    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
		       x - 0.5*w*cos(lineArrowAngle)*dy
		         + 0.5*w*sin(lineArrowAngle)*dx,
		       y + 0.5*w*cos(lineArrowAngle)*dx
		         + 0.5*w*sin(lineArrowAngle)*dy);
    appearBuf->append("S\n");
    break;
  }
}

// Draw an (approximate) circle of radius <r> centered at (<cx>, <cy>).
// <cmd> is used to draw the circle ("f", "s", or "b").
void Annot::drawCircle(double cx, double cy, double r, const char *cmd) {
  appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		     cx + r, cy);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx + r, cy + bezierCircle * r,
		     cx + bezierCircle * r, cy + r,
		     cx, cy + r);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx - bezierCircle * r, cy + r,
		     cx - r, cy + bezierCircle * r,
		     cx - r, cy);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx - r, cy - bezierCircle * r,
		     cx - bezierCircle * r, cy - r,
		     cx, cy - r);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx + bezierCircle * r, cy - r,
		     cx + r, cy - bezierCircle * r,
		     cx + r, cy);
  appearBuf->appendf("{0:s}\n", cmd);
}

// Draw the top-left half of an (approximate) circle of radius <r>
// centered at (<cx>, <cy>).
void Annot::drawCircleTopLeft(double cx, double cy, double r) {
  double r2;

  r2 = r / sqrt(2.0);
  appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		     cx + r2, cy + r2);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx + (1 - bezierCircle) * r2,
		     cy + (1 + bezierCircle) * r2,
		     cx - (1 - bezierCircle) * r2,
		     cy + (1 + bezierCircle) * r2,
		     cx - r2,
		     cy + r2);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx - (1 + bezierCircle) * r2,
		     cy + (1 - bezierCircle) * r2,
		     cx - (1 + bezierCircle) * r2,
		     cy - (1 - bezierCircle) * r2,
		     cx - r2,
		     cy - r2);
  appearBuf->append("S\n");
}

// Draw the bottom-right half of an (approximate) circle of radius <r>
// centered at (<cx>, <cy>).
void Annot::drawCircleBottomRight(double cx, double cy, double r) {
  double r2;

  r2 = r / sqrt(2.0);
  appearBuf->appendf("{0:.4f} {1:.4f} m\n",
		     cx - r2, cy - r2);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx - (1 - bezierCircle) * r2,
		     cy - (1 + bezierCircle) * r2,
		     cx + (1 - bezierCircle) * r2,
		     cy - (1 + bezierCircle) * r2,
		     cx + r2,
		     cy - r2);
  appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} {4:.4f} {5:.4f} c\n",
		     cx + (1 + bezierCircle) * r2,
		     cy - (1 - bezierCircle) * r2,
		     cx + (1 + bezierCircle) * r2,
		     cy + (1 - bezierCircle) * r2,
		     cx + r2,
		     cy + r2);
  appearBuf->append("S\n");
}

void Annot::drawText(GString *text, GString *da, int quadding, double margin,
		     int rot) {
  GString *text2, *tok;
  GList *daToks;
  const char *charName;
  double dx, dy, fontSize, fontSize2, x, y, w;
  Gushort charWidth;
  int tfPos, tmPos, i, j, c;

  // check for a Unicode string
  //~ this currently drops all non-Latin1 characters
  if (text->getLength() >= 2 &&
      text->getChar(0) == '\xfe' && text->getChar(1) == '\xff') {
    text2 = new GString();
    for (i = 2; i+1 < text->getLength(); i += 2) {
      c = ((text->getChar(i) & 0xff) << 8) + (text->getChar(i+1) & 0xff);
      if (c <= 0xff) {
	text2->append((char)c);
      } else {
	text2->append('?');
      }
    }
  } else {
    text2 = text;
  }

  // parse the default appearance string
  tfPos = tmPos = -1;
  if (da) {
    daToks = new GList();
    i = 0;
    while (i < da->getLength()) {
      while (i < da->getLength() && Lexer::isSpace(da->getChar(i))) {
	++i;
      }
      if (i < da->getLength()) {
	for (j = i + 1;
	     j < da->getLength() && !Lexer::isSpace(da->getChar(j));
	     ++j) ;
	daToks->append(new GString(da, i, j - i));
	i = j;
      }
    }
    for (i = 2; i < daToks->getLength(); ++i) {
      if (i >= 2 && !((GString *)daToks->get(i))->cmp("Tf")) {
	tfPos = i - 2;
      } else if (i >= 6 && !((GString *)daToks->get(i))->cmp("Tm")) {
	tmPos = i - 6;
      }
    }
  } else {
    daToks = NULL;
  }

  // get the font and font size
  fontSize = 0;
  if (tfPos >= 0) {
    //~ where do we look up the font?
    tok = (GString *)daToks->get(tfPos);
    tok->clear();
    tok->append("/xpdf_default_font");
    tok = (GString *)daToks->get(tfPos + 1);
    fontSize = atof(tok->getCString());
  } else {
    error(errSyntaxError, -1,
	  "Missing 'Tf' operator in annotation's DA string");
    daToks->append(new GString("/xpdf_default_font"));
    daToks->append(new GString("10"));
    daToks->append(new GString("Tf"));
  }

  // setup
  appearBuf->append("q\n");
  if (rot == 90) {
    appearBuf->appendf("0 1 -1 0 {0:.4f} 0 cm\n", xMax - xMin);
    dx = yMax - yMin;
    dy = xMax - xMin;
  } else if (rot == 180) {
    appearBuf->appendf("-1 0 0 -1 {0:.4f} {1:.4f} cm\n",
		       xMax - xMin, yMax - yMin);
    dx = xMax - yMax;
    dy = yMax - yMin;
  } else if (rot == 270) {
    appearBuf->appendf("0 -1 1 0 0 {0:.4f} cm\n", yMax - yMin);
    dx = yMax - yMin;
    dy = xMax - xMin;
  } else { // assume rot == 0
    dx = xMax - xMin;
    dy = yMax - yMin;
  }
  appearBuf->append("BT\n");

  // compute string width
  //~ this assumes we're substituting Helvetica/WinAnsiEncoding for everything
  w = 0;
  for (i = 0; i < text2->getLength(); ++i) {
    charName = winAnsiEncoding[text->getChar(i) & 0xff];
    if (charName && builtinFonts[4].widths->getWidth(charName, &charWidth)) {
      w += charWidth;
    } else {
      w += 0.5;
    }
  }

  // compute font autosize
  if (fontSize == 0) {
    fontSize = dy - 2 * margin;
    fontSize2 = (dx - 2 * margin) / w;
    if (fontSize2 < fontSize) {
      fontSize = fontSize2;
    }
    fontSize = floor(fontSize);
    if (tfPos >= 0) {
      tok = (GString *)daToks->get(tfPos + 1);
      tok->clear();
      tok->appendf("{0:.4f}", fontSize);
    }
  }

  // compute text start position
  w *= fontSize;
  switch (quadding) {
  case 0:
  default:
    x = margin + 2;
    break;
  case 1:
    x = (dx - w) / 2;
    break;
  case 2:
    x = dx - margin - 2 - w;
    break;
  }
  y = 0.5 * dy - 0.4 * fontSize;

  // set the font matrix
  if (tmPos >= 0) {
    tok = (GString *)daToks->get(tmPos + 4);
    tok->clear();
    tok->appendf("{0:.4f}", x);
    tok = (GString *)daToks->get(tmPos + 5);
    tok->clear();
    tok->appendf("{0:.4f}", y);
  }
  
  // write the DA string
  if (daToks) {
    for (i = 0; i < daToks->getLength(); ++i) {
      appearBuf->append((GString *)daToks->get(i))->append(' ');
    }
  }

  // write the font matrix (if not part of the DA string)
  if (tmPos < 0) {
    appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n", x, y);
  }

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

  // cleanup
  appearBuf->append("ET\n");
  appearBuf->append("Q\n");

  if (daToks) {
    deleteGList(daToks, GString);
  }
  if (text2 != text) {
    delete text2;
  }
}

void Annot::draw(Gfx *gfx, GBool printing) {
  GBool oc, isLink;

  // check the flags
  if ((flags & annotFlagHidden) ||
      (printing && !(flags & annotFlagPrint)) ||
      (!printing && (flags & annotFlagNoView))) {
    return;
  }

  // check the optional content entry
  if (doc->getOptionalContent()->evalOCObject(&ocObj, &oc) && !oc) {
    return;
  }

  // draw the appearance stream
  isLink = type && !type->cmp("Link");
  gfx->drawAnnot(&appearance, isLink ? borderStyle : (AnnotBorderStyle *)NULL,
		 xMin, yMin, xMax, yMax);
}

Object *Annot::getObject(Object *obj) {
  if (ref.num >= 0) {
    xref->fetch(ref.num, ref.gen, obj);
  } else {
    obj->initNull();
  }
  return obj;
}

//------------------------------------------------------------------------
// Annots
//------------------------------------------------------------------------

Annots::Annots(PDFDoc *docA, Object *annotsObj) {
  Annot *annot;
  Object obj1, obj2;
  Ref ref;
  GBool drawWidgetAnnots;
  int size;
  int i;

  doc = docA;
  annots = NULL;
  size = 0;
  nAnnots = 0;

  if (annotsObj->isArray()) {
    // Kludge: some PDF files define an empty AcroForm, but still
    // include Widget-type annotations -- in that case, we want to
    // draw the widgets (since the form code won't).  This really
    // ought to look for Widget-type annotations that are not included
    // in any form field.
    drawWidgetAnnots = !doc->getCatalog()->getForm() ||
                       doc->getCatalog()->getForm()->getNumFields() == 0;
    for (i = 0; i < annotsObj->arrayGetLength(); ++i) {
      if (annotsObj->arrayGetNF(i, &obj1)->isRef()) {
	ref = obj1.getRef();
	obj1.free();
	annotsObj->arrayGet(i, &obj1);
      } else {
	ref.num = ref.gen = -1;
      }
      if (obj1.isDict()) {
	if (drawWidgetAnnots ||
	    !obj1.dictLookup("Subtype", &obj2)->isName("Widget")) {
	  annot = new Annot(doc, obj1.getDict(), &ref);
	  if (annot->isOk()) {
	    if (nAnnots >= size) {
	      size += 16;
	      annots = (Annot **)greallocn(annots, size, sizeof(Annot *));
	    }
	    annots[nAnnots++] = annot;
	  } else {
	    delete annot;
	  }
	}
	obj2.free();
      }
      obj1.free();
    }
  }
}

Annots::~Annots() {
  int i;

  for (i = 0; i < nAnnots; ++i) {
    delete annots[i];
  }
  gfree(annots);
}

Annot *Annots::find(double x, double y) {
  int i;

  for (i = nAnnots - 1; i >= 0; --i) {
    if (annots[i]->inRect(x, y)) {
      return annots[i];
    }
  }
  return NULL;
}

int Annots::findIdx(double x, double y) {
  int i;

  for (i = nAnnots - 1; i >= 0; --i) {
    if (annots[i]->inRect(x, y)) {
      return i;
    }
  }
  return -1;
}

void Annots::generateAnnotAppearances() {
  int i;

  for (i = 0; i < nAnnots; ++i) {
    annots[i]->generateAnnotAppearance();
  }
}

Annot *Annots::findAnnot(Ref *ref) {
  int i;

  for (i = 0; i < nAnnots; ++i) {
    if (annots[i]->match(ref)) {
      return annots[i];
    }
  }
  return NULL;
}
