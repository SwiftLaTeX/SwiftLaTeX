//========================================================================
//
// XFAForm.h
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#ifndef XFAFORM_H
#define XFAFORM_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Form.h"

class GHash;
class XFATableInfo;
class ZxDoc;
class ZxElement;
class ZxAttr;

//------------------------------------------------------------------------

enum XFAHorizAlign {
  xfaHAlignLeft,
  xfaHAlignCenter,
  xfaHAlignRight
};

enum XFAVertAlign {
  xfaVAlignTop,
  xfaVAlignBottom,
  xfaVAlignMiddle
};

//------------------------------------------------------------------------

class XFAForm: public Form {
public:

  static XFAForm *load(PDFDoc *docA, Catalog *catalog,
		       Object *acroFormObj, Object *xfaObj);

  virtual ~XFAForm();

  virtual const char *getType() { return "XFA"; }

  virtual void draw(int pageNum, Gfx *gfx, GBool printing);

  virtual int getNumFields();
  virtual FormField *getField(int idx);

private:

  XFAForm(PDFDoc *docA, int nPagesA, ZxDoc *xmlA,
	  Object *resourceDictA, GBool fullXFAA);
  void scanNode(ZxElement *elem,
		GString *parentName, GString *parentFullName,
		GBool inPageSet, XFATableInfo *tableInfo,
		GHash *nameCount, GHash *nameIdx,
		GHash *fullNameCount, GHash *fullNameIdx,
		GString *exclGroupName, Catalog *catalog);
  void scanNames(ZxElement *elem, GHash *nameCount);
  void scanFullNames(ZxElement *elem, GHash *fullNameCount);
  void scanField(ZxElement *elem, GString *name, GString *fullName,
		 GString *exclGroupName, GBool inPageSet,
		 XFATableInfo *tableInfo, int colSpan,
		 Catalog *catalog);
  void scanNonField(ZxElement *elem, GString *name, GString *fullName,
		    GBool inPageSet,
		    XFATableInfo *tableInfo, int colSpan,
		    GHash *nameCount, GHash *nameIdx,
		    GHash *fullNameCount, GHash *fullNameIdx,
		    Catalog *catalog);
  GString *getNodeName(ZxElement *elem);
  GString *getNodeFullName(ZxElement *elem);
  GBool nodeIsBindGlobal(ZxElement *elem);

  ZxDoc *xml;
  GList *fields;		// [XFAFormField]
  Object resourceDict;
  GBool fullXFA;		// true for "Full XFA", false for
				//   "XFA Foreground"
  int nPages;			// number of pages in PDF file
  double *pageOffsetX,		// x,y offset for each page
         *pageOffsetY;
  int pageSetNPages;		// number of pages in pageSet element
  int curPageNum;		// current page number - used by scanFields()
  double curXOffset,		// current x,y offset - used by scanFields()
         curYOffset;

  friend class XFAFormField;
};

//------------------------------------------------------------------------

class XFAFormField: public FormField {
public:

  XFAFormField(XFAForm *xfaFormA, ZxElement *xmlA,
	       GString *nameA, GString *fullNameA, GString *exclGroupNameA,
	       int pageNumA, double xOffsetA, double yOffsetA,
	       double columnWidthA, double rowHeightA);

  virtual ~XFAFormField();

  virtual int getPageNum();
  virtual const char *getType();
  virtual Unicode *getName(int *length);
  virtual Unicode *getValue(int *length);
  virtual void getBBox(double *llx, double *lly, double *urx, double *ury);
  virtual void getFont(Ref *fontID, double *fontSize);
  virtual void getColor(double *red, double *green, double *blue);

  virtual Object *getResources(Object *res);

private:

  Unicode *utf8ToUnicode(GString *s, int *length);
  void draw(int pageNumA, Gfx *gfx, GBool printing, GfxFontDict *fontDict);
  void getRectangle(double *xfaX, double *xfaY,
		    double *xfaW, double *xfaH,
		    double *pdfX, double *pdfY,
		    double *pdfW, double *pdfH,
		    int *pdfRot);
  void drawTextEdit(GfxFontDict *fontDict,
		    double w, double h, int rot,
		    GString *appearBuf);
  void drawCheckButton(GfxFontDict *fontDict,
		       double w, double h, int rot,
		       GString *appearBuf);
  void drawBarCode(GfxFontDict *fontDict,
		   double w, double h, int rot,
		   GString *appearBuf);
  static double getMeasurement(ZxAttr *attr, double defaultVal);
  static double getMeasurement(GString *s, int begin);
  GString *getFieldValue(const char *valueChildType);
  ZxElement *findFieldInDatasets(ZxElement *elem, char *partName);
  ZxElement *findFieldInFormElem(ZxElement *elem, char *partName);
  void transform(int rot, double w, double h,
		 double *wNew, double *hNew, GString *appearBuf);
  void drawText(GString *text, GBool multiLine, int combCells,
		GString *fontName, GBool bold,
		GBool italic, double fontSize,
		XFAHorizAlign hAlign, XFAVertAlign vAlign,
		double x, double y, double w, double h,
		GBool whiteBackground,
		GfxFontDict *fontDict, GString *appearBuf);
  GfxFont *findFont(GfxFontDict *fontDict, GString *fontName,
		    GBool bold, GBool italic);
  Ref findFontName(GString *name, GBool bold, GBool italic);
  void getNextLine(GString *text, int start,
		   GfxFont *font, double fontSize, double wMax,
		   int *end, double *width, int *next);
  GString *pictureFormatDateTime(GString *value, GString *picture);
  GString *pictureFormatNumber(GString *value, GString *picture);
  GString *pictureFormatText(GString *value, GString *picture);
  GBool isValidInt(GString *s, int start, int len);
  int convertInt(GString *s, int start, int len);

  XFAForm *xfaForm;
  ZxElement *xml;
  GString *name;
  GString *fullName;
  GString *exclGroupName;
  int pageNum;
  double xOffset, yOffset;
  double columnWidth;		// column width, if this field is in a
				//   table; otherwise zero
  double rowHeight;		// row height, if this field is in a
				//   table; otherwise zero

  friend class XFAForm;
  friend class XFATableInfo;
};

#endif
