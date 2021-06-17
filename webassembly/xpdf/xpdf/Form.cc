//========================================================================
//
// Form.cc
//
// Copyright 2012 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmempp.h"
#include "GlobalParams.h"
#include "Error.h"
#include "Object.h"
#include "PDFDoc.h"
#include "AcroForm.h"
#include "XFAForm.h"
#include "Form.h"

//------------------------------------------------------------------------
// Form
//------------------------------------------------------------------------

Form *Form::load(PDFDoc *docA, Catalog *catalog, Object *acroFormObj) {
  Form *form;
  Object xfaObj;

  if (acroFormObj->isDict()) {
    //~ temporary: create an XFAForm only for XFAF, not for dynamic XFA
    acroFormObj->dictLookup("XFA", &xfaObj);
    if (globalParams->getEnableXFA() &&
	!xfaObj.isNull() &&
	!catalog->getNeedsRendering()) {
      form = XFAForm::load(docA, catalog, acroFormObj, &xfaObj);
      xfaObj.free();
      return form;
    }
    xfaObj.free();
  }

  // if acroFormObj is a null object, this will still create an
  // AcroForm if there are unattached Widget-type annots
  return AcroForm::load(docA, catalog, acroFormObj);
}

Form::Form(PDFDoc *docA) {
  doc = docA;
}

Form::~Form() {
}

FormField *Form::findField(int pg, double x, double y) {
  FormField *field;
  double llx, lly, urx, ury;
  int i;

  for (i = 0; i < getNumFields(); ++i) {
    field = getField(i);
    if (field->getPageNum() == pg) {
      field->getBBox(&llx, &lly, &urx, &ury);
      if (llx <= x && x <= urx && lly <= y && y <= ury) {
	return field;
      }
    }
  }
  return NULL;
}

int Form::findFieldIdx(int pg, double x, double y) {
  FormField *field;
  double llx, lly, urx, ury;
  int i;

  for (i = 0; i < getNumFields(); ++i) {
    field = getField(i);
    if (field->getPageNum() == pg) {
      field->getBBox(&llx, &lly, &urx, &ury);
      if (llx <= x && x <= urx && lly <= y && y <= ury) {
	return i;
      }
    }
  }
  return -1;
}

//------------------------------------------------------------------------
// FormField
//------------------------------------------------------------------------

FormField::FormField() {
}

FormField::~FormField() {
}
