//========================================================================
//
// AcroForm.h
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#ifndef ACROFORM_H
#define ACROFORM_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Form.h"

class TextString;
class GfxFont;
class GfxFontDict;

//------------------------------------------------------------------------

class AcroForm: public Form {
public:

  static AcroForm *load(PDFDoc *docA, Catalog *catalog, Object *acroFormObjA);

  virtual ~AcroForm();

  virtual const char *getType() { return "AcroForm"; }

  virtual void draw(int pageNum, Gfx *gfx, GBool printing);

  virtual int getNumFields();
  virtual FormField *getField(int idx);

private:

  AcroForm(PDFDoc *docA, Object *acroFormObjA);
  void buildAnnotPageList(Catalog *catalog);
  int lookupAnnotPage(Object *annotRef);
  void scanField(Object *fieldRef);

  Object acroFormObj;
  GBool needAppearances;
  GList *annotPages;		// [AcroFormAnnotPage]
  GList *fields;		// [AcroFormField]

  friend class AcroFormField;
};

//------------------------------------------------------------------------

enum AcroFormFieldType {
  acroFormFieldPushbutton,
  acroFormFieldRadioButton,
  acroFormFieldCheckbox,
  acroFormFieldFileSelect,
  acroFormFieldMultilineText,
  acroFormFieldText,
  acroFormFieldComboBox,
  acroFormFieldListBox,
  acroFormFieldSignature
};

class AcroFormField: public FormField {
public:

  static AcroFormField *load(AcroForm *acroFormA, Object *fieldRefA);

  virtual ~AcroFormField();

  virtual int getPageNum();
  virtual const char *getType();
  virtual Unicode *getName(int *length);
  virtual Unicode *getValue(int *length);
  virtual void getBBox(double *llx, double *lly, double *urx, double *ury);
  virtual void getFont(Ref *fontID, double *fontSize);
  virtual void getColor(double *red, double *green, double *blue);
  int getMaxLen();

  virtual Object *getResources(Object *res);

  AcroFormFieldType getAcroFormFieldType() { return type; }
  Object *getFieldRef(Object *ref);
  Object *getValueObj(Object *val);
  Object *getParentRef(Object *parent);
  GBool getTypeFromParent() { return typeFromParent; }

private:

  AcroFormField(AcroForm *acroFormA, Object *fieldRefA, Object *fieldObjA,
		AcroFormFieldType typeA, TextString *nameA,
		Guint flagsA, GBool typeFromParentA);
  Ref findFontName(char *fontTag);
  void draw(int pageNum, Gfx *gfx, GBool printing);
  void drawAnnot(int pageNum, Gfx *gfx, GBool printing,
		 Object *annotRef, Object *annotObj);
  void drawExistingAppearance(Gfx *gfx, Dict *annot,
			      double xMin, double yMin,
			      double xMax, double yMax);
  void drawNewAppearance(Gfx *gfx, Dict *annot,
			 double xMin, double yMin,
			 double xMax, double yMax);
  void setColor(Array *a, GBool fill, int adjust, GString *appearBuf);
  void drawText(GString *text, GString *da, GfxFontDict *fontDict,
		GBool multiline, int comb, int quadding,
		GBool txField, GBool forceZapfDingbats, int rot,
		double xMin, double yMin, double xMax, double yMax,
		double border, GString *appearBuf);
  void drawListBox(GString **text, GBool *selection,
		   int nOptions, int topIdx,
		   GString *da, GfxFontDict *fontDict,
		   GBool quadding, double xMin, double yMin,
		   double xMax, double yMax, double border,
		   GString *appearBuf);
  void getNextLine(GString *text, int start,
		   GfxFont *font, double fontSize, double wMax,
		   int *end, double *width, int *next);
  void drawCircle(double cx, double cy, double r, const char *cmd,
		  GString *appearBuf);
  void drawCircleTopLeft(double cx, double cy, double r,
			 GString *appearBuf);
  void drawCircleBottomRight(double cx, double cy, double r,
			     GString *appearBuf);
  GList *tokenize(GString *s);
  Object *getAnnotObj(Object *annotObj);
  Object *getAnnotResources(Dict *annot, Object *res);
  void buildDefaultResourceDict(Object *dr);
  Object *fieldLookup(const char *key, Object *obj);
  Object *fieldLookup(Dict *dict, const char *key, Object *obj);

  AcroForm *acroForm;
  Object fieldRef;
  Object fieldObj;
  AcroFormFieldType type;
  TextString *name;
  Guint flags;
  GBool typeFromParent;

  friend class AcroForm;
};

#endif
