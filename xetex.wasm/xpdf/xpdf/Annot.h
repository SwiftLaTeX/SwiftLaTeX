//========================================================================
//
// Annot.h
//
// Copyright 2000-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef ANNOT_H
#define ANNOT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

class XRef;
class Catalog;
class Gfx;
class GfxFontDict;
class PDFDoc;

//------------------------------------------------------------------------
// AnnotBorderStyle
//------------------------------------------------------------------------

enum AnnotBorderType {
  annotBorderSolid,
  annotBorderDashed,
  annotBorderBeveled,
  annotBorderInset,
  annotBorderUnderlined
};

class AnnotBorderStyle {
public:

  AnnotBorderStyle(AnnotBorderType typeA, double widthA,
		   double *dashA, int dashLengthA,
		   double *colorA, int nColorCompsA);
  ~AnnotBorderStyle();

  AnnotBorderType getType() { return type; }
  double getWidth() { return width; }
  void getDash(double **dashA, int *dashLengthA)
    { *dashA = dash; *dashLengthA = dashLength; }
  int getNumColorComps() { return nColorComps; }
  double *getColor() { return color; }

private:

  AnnotBorderType type;
  double width;
  double *dash;
  int dashLength;
  double color[4];
  int nColorComps;
};

//------------------------------------------------------------------------

enum AnnotLineEndType {
  annotLineEndNone,
  annotLineEndSquare,
  annotLineEndCircle,
  annotLineEndDiamond,
  annotLineEndOpenArrow,
  annotLineEndClosedArrow,
  annotLineEndButt,
  annotLineEndROpenArrow,
  annotLineEndRClosedArrow,
  annotLineEndSlash
};

//------------------------------------------------------------------------
// Annot
//------------------------------------------------------------------------

class Annot {
public:

  Annot(PDFDoc *docA, Dict *dict, Ref *refA);
  ~Annot();
  GBool isOk() { return ok; }

  void draw(Gfx *gfx, GBool printing);

  GString *getType() { return type; }
  double getXMin() { return xMin; }
  double getYMin() { return yMin; }
  double getXMax() { return xMax; }
  double getYMax() { return yMax; }
  Object *getObject(Object *obj);

  // Check if point is inside the annotation rectangle.
  GBool inRect(double x, double y)
    { return xMin <= x && x <= xMax && yMin <= y && y <= yMax; }

  // Get appearance object.
  Object *getAppearance(Object *obj) { return appearance.fetch(xref, obj); }

  AnnotBorderStyle *getBorderStyle() { return borderStyle; }

  GBool match(Ref *refA)
    { return ref.num == refA->num && ref.gen == refA->gen; }

  void generateAnnotAppearance();

private:
 
  void generateLineAppearance();
  void generatePolyLineAppearance();
  void generatePolygonAppearance();
  void generateFreeTextAppearance();
  void setLineStyle(AnnotBorderStyle *bs, double *lineWidth);
  void setStrokeColor(double *color, int nComps);
  GBool setFillColor(Object *colorObj);
  AnnotLineEndType parseLineEndType(Object *obj);
  void adjustLineEndpoint(AnnotLineEndType lineEnd,
			  double x, double y, double dx, double dy,
			  double w, double *tx, double *ty);
  void drawLineArrow(AnnotLineEndType lineEnd,
		     double x, double y, double dx, double dy,
		     double w, GBool fill);
  void drawCircle(double cx, double cy, double r, const char *cmd);
  void drawCircleTopLeft(double cx, double cy, double r);
  void drawCircleBottomRight(double cx, double cy, double r);
  void drawText(GString *text, GString *da, int quadding, double margin,
		int rot);

  PDFDoc *doc;
  XRef *xref;			// the xref table for this PDF file
  Ref ref;			// object ref identifying this annotation
  GString *type;		// annotation type
  GString *appearanceState;	// appearance state name
  Object appearance;		// a reference to the Form XObject stream
				//   for the normal appearance
  GString *appearBuf;
  double xMin, yMin,		// annotation rectangle
         xMax, yMax;
  Guint flags;
  AnnotBorderStyle *borderStyle;
  Object ocObj;			// optional content entry
  GBool ok;
};

//------------------------------------------------------------------------
// Annots
//------------------------------------------------------------------------

class Annots {
public:

  // Build a list of Annot objects.
  Annots(PDFDoc *docA, Object *annotsObj);

  ~Annots();

  // Iterate through list of annotations.
  int getNumAnnots() { return nAnnots; }
  Annot *getAnnot(int i) { return annots[i]; }

  // If point <x>,<y> is in an annotation, return the associated
  // annotation; else return NULL.
  Annot *find(double x, double y);
  int findIdx(double x, double y);

  // Generate an appearance stream for any non-form-field annotation
  // that is missing it.
  void generateAnnotAppearances();

private:

  void scanFieldAppearances(Dict *node, Ref *ref, Dict *parent,
			    Dict *acroForm);

  Annot *findAnnot(Ref *ref);

  PDFDoc *doc;
  Annot **annots;
  int nAnnots;
};

#endif
