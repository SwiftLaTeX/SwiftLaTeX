//========================================================================
//
// Form.h
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#ifndef FORM_H
#define FORM_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"

class Gfx;
class FormField;

//------------------------------------------------------------------------

class Form {
public:

  static Form *load(PDFDoc *docA, Catalog *catalog, Object *acroFormObj);

  virtual ~Form();

  virtual const char *getType() = 0;

  virtual void draw(int pageNum, Gfx *gfx, GBool printing) = 0;

  virtual int getNumFields() = 0;
  virtual FormField *getField(int idx) = 0;

  FormField *findField(int pg, double x, double y);
  int findFieldIdx(int pg, double x, double y);

protected:

  Form(PDFDoc *docA);

  PDFDoc *doc;
};

//------------------------------------------------------------------------

class FormField {
public:

  FormField();
  virtual ~FormField();

  virtual int getPageNum() = 0;
  virtual const char *getType() = 0;

  // Return the field name.  This never returns NULL.
  virtual Unicode *getName(int *length) = 0;

  // Return the field value.  This returns NULL if the field does not
  // have a value.
  virtual Unicode *getValue(int *length) = 0;

  virtual void getBBox(double *llx, double *lly, double *urx, double *ury) = 0;
  virtual void getFont(Ref *fontID, double *fontSize) = 0;
  virtual void getColor(double *red, double *green, double *blue) = 0;

  // Return the resource dictionaries used to draw this field.  The
  // returned object must be either a dictionary or an array of
  // dictonaries.
  virtual Object *getResources(Object *res) = 0;
};

#endif
