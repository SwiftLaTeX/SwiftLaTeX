//========================================================================
//
// AcroForm.cc
//
// Copyright 2012 Glyph & Cog, LLC
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
#include "GString.h"
#include "GList.h"
#include "Error.h"
#include "Object.h"
#include "PDFDoc.h"
#include "TextString.h"
#include "Gfx.h"
#include "GfxFont.h"
#include "OptionalContent.h"
#include "Annot.h"
#include "Lexer.h"
#include "AcroForm.h"

//------------------------------------------------------------------------

#define acroFormFlagReadOnly           (1 <<  0)  // all
#define acroFormFlagRequired           (1 <<  1)  // all
#define acroFormFlagNoExport           (1 <<  2)  // all
#define acroFormFlagMultiline          (1 << 12)  // text
#define acroFormFlagPassword           (1 << 13)  // text
#define acroFormFlagNoToggleToOff      (1 << 14)  // button
#define acroFormFlagRadio              (1 << 15)  // button
#define acroFormFlagPushbutton         (1 << 16)  // button
#define acroFormFlagCombo              (1 << 17)  // choice
#define acroFormFlagEdit               (1 << 18)  // choice
#define acroFormFlagSort               (1 << 19)  // choice
#define acroFormFlagFileSelect         (1 << 20)  // text
#define acroFormFlagMultiSelect        (1 << 21)  // choice
#define acroFormFlagDoNotSpellCheck    (1 << 22)  // text, choice
#define acroFormFlagDoNotScroll        (1 << 23)  // text
#define acroFormFlagComb               (1 << 24)  // text
#define acroFormFlagRadiosInUnison     (1 << 25)  // button
#define acroFormFlagRichText           (1 << 25)  // text
#define acroFormFlagCommitOnSelChange  (1 << 26)  // choice

#define acroFormQuadLeft   0
#define acroFormQuadCenter 1
#define acroFormQuadRight  2

#define annotFlagHidden    0x0002
#define annotFlagPrint     0x0004
#define annotFlagNoView    0x0020

// distance of Bezier control point from center for circle approximation
// = (4 * (sqrt(2) - 1) / 3) * r
#define bezierCircle 0.55228475

//------------------------------------------------------------------------

// map an annotation ref to a page number
class AcroFormAnnotPage {
public:
  AcroFormAnnotPage(int annotNumA, int annotGenA, int pageNumA)
    { annotNum = annotNumA; annotGen = annotGenA; pageNum = pageNumA; }
  int annotNum;
  int annotGen;
  int pageNum;
};

//------------------------------------------------------------------------
// AcroForm
//------------------------------------------------------------------------

AcroForm *AcroForm::load(PDFDoc *docA, Catalog *catalog, Object *acroFormObjA) {
  Object acroFormObj2;
  AcroForm *acroForm;
  AcroFormField *field;
  Object fieldsObj, annotsObj, annotRef, annotObj, obj1, obj2;
  int pageNum, i, j;

  // this is the normal case: acroFormObj is a dictionary, as expected
  if (acroFormObjA->isDict()) {
    acroForm = new AcroForm(docA, acroFormObjA);

    if (acroFormObjA->dictLookup("NeedAppearances", &obj1)->isBool()) {
      acroForm->needAppearances = obj1.getBool();
    }
    obj1.free();

    acroForm->buildAnnotPageList(catalog);

    if (!acroFormObjA->dictLookup("Fields", &obj1)->isArray()) {
      if (!obj1.isNull()) {
	error(errSyntaxError, -1, "AcroForm Fields entry is wrong type");
      }
      obj1.free();
      delete acroForm;
      return NULL;
    }
    for (i = 0; i < obj1.arrayGetLength(); ++i) {
      obj1.arrayGetNF(i, &obj2);
      acroForm->scanField(&obj2);
      obj2.free();
    }
    obj1.free();

    // scan the annotations, looking for Widget-type annots that are
    // not attached to the AcroForm object
    for (pageNum = 1; pageNum <= catalog->getNumPages(); ++pageNum) {
      if (catalog->getPage(pageNum)->getAnnots(&annotsObj)->isArray()) {
	for (i = 0; i < annotsObj.arrayGetLength(); ++i) {
	  if (annotsObj.arrayGetNF(i, &annotRef)->isRef()) {
	    for (j = 0; j < acroForm->fields->getLength(); ++j) {
	      field = (AcroFormField *)acroForm->fields->get(j);
	      if (field->fieldRef.isRef()) {
		if (field->fieldRef.getRefNum() == annotRef.getRefNum() &&
		    field->fieldRef.getRefGen() == annotRef.getRefGen()) {
		  break;
		}
	      }
	    }
	    if (j == acroForm->fields->getLength()) {
	      annotRef.fetch(acroForm->doc->getXRef(), &annotObj);
	      if (annotObj.isDict()) {
		if (annotObj.dictLookup("Subtype", &obj1)->isName("Widget")) {
		  acroForm->scanField(&annotRef);
		}
		obj1.free();
	      }
	      annotObj.free();
	    }
	  }
	  annotRef.free();
	}
      }
      annotsObj.free();
    }

  // if acroFormObjA is a null object, but there are Widget-type
  // annots, we still create an AcroForm
  } else {
    // create an empty dict for acroFormObj
    acroFormObj2.initDict(docA->getXRef());
    acroForm = new AcroForm(docA, &acroFormObj2);
    acroFormObj2.free();

    acroForm->buildAnnotPageList(catalog);

    // scan the annotations, looking for any Widget-type annots
    for (pageNum = 1; pageNum <= catalog->getNumPages(); ++pageNum) {
      if (catalog->getPage(pageNum)->getAnnots(&annotsObj)->isArray()) {
	for (i = 0; i < annotsObj.arrayGetLength(); ++i) {
	  if (annotsObj.arrayGetNF(i, &annotRef)->isRef()) {
	    annotRef.fetch(acroForm->doc->getXRef(), &annotObj);
	    if (annotObj.isDict()) {
	      if (annotObj.dictLookup("Subtype", &obj1)->isName("Widget")) {
		acroForm->scanField(&annotRef);
	      }
	      obj1.free();
	    }
	    annotObj.free();
	  }
	  annotRef.free();
	}
      }
      annotsObj.free();
    }

    if (acroForm->fields->getLength() == 0) {
      delete acroForm;
      acroForm = NULL;
    }
  }

  return acroForm;
}

AcroForm::AcroForm(PDFDoc *docA, Object *acroFormObjA): Form(docA) {
  acroFormObjA->copy(&acroFormObj);
  needAppearances = gFalse;
  annotPages = new GList();
  fields = new GList();
}

AcroForm::~AcroForm() {
  acroFormObj.free();
  deleteGList(annotPages, AcroFormAnnotPage);
  deleteGList(fields, AcroFormField);
}

void AcroForm::buildAnnotPageList(Catalog *catalog) {
  Object annotsObj, annotObj;
  int pageNum, i;

  for (pageNum = 1; pageNum <= catalog->getNumPages(); ++pageNum) {
    if (catalog->getPage(pageNum)->getAnnots(&annotsObj)->isArray()) {
      for (i = 0; i < annotsObj.arrayGetLength(); ++i) {
	if (annotsObj.arrayGetNF(i, &annotObj)->isRef()) {
	  annotPages->append(new AcroFormAnnotPage(annotObj.getRefNum(),
						   annotObj.getRefGen(),
						   pageNum));
	}
	annotObj.free();
      }
    }
    annotsObj.free();
  }
  //~ sort the list
}

int AcroForm::lookupAnnotPage(Object *annotRef) {
  AcroFormAnnotPage *annotPage;
  int num, gen, i;

  if (!annotRef->isRef()) {
    return 0;
  }
  num = annotRef->getRefNum();
  gen = annotRef->getRefGen();
  //~ use bin search
  for (i = 0; i < annotPages->getLength(); ++i) {
    annotPage = (AcroFormAnnotPage *)annotPages->get(i);
    if (annotPage->annotNum == num && annotPage->annotGen == gen) {
      return annotPage->pageNum;
    }
  }
  return 0;
}

void AcroForm::scanField(Object *fieldRef) {
  AcroFormField *field;
  Object fieldObj, kidsObj, kidRef, kidObj, subtypeObj;
  GBool isTerminal;
  int i;

  fieldRef->fetch(doc->getXRef(), &fieldObj);
  if (!fieldObj.isDict()) {
    error(errSyntaxError, -1, "AcroForm field object is wrong type");
    fieldObj.free();
    return;
  }

  // if this field has a Kids array, and all of the kids have a Parent
  // reference (i.e., they're all form fields, not widget
  // annotations), then this is a non-terminal field, and we need to
  // scan the kids
  isTerminal = gTrue;
  if (fieldObj.dictLookup("Kids", &kidsObj)->isArray()) {
    isTerminal = gFalse;
    for (i = 0; !isTerminal && i < kidsObj.arrayGetLength(); ++i) {
      kidsObj.arrayGet(i, &kidObj);
      if (kidObj.isDict()) {
	if (kidObj.dictLookup("Parent", &subtypeObj)->isNull()) {
	  isTerminal = gTrue;
	}
	subtypeObj.free();
      }
      kidObj.free();
    }
    if (!isTerminal) {
      for (i = 0; !isTerminal && i < kidsObj.arrayGetLength(); ++i) {
	kidsObj.arrayGetNF(i, &kidRef);
	scanField(&kidRef);
	kidRef.free();
      }
    }
  }
  kidsObj.free();

  if (isTerminal) {
    if ((field = AcroFormField::load(this, fieldRef))) {
      fields->append(field);
    }
  }

  fieldObj.free();
}

void AcroForm::draw(int pageNum, Gfx *gfx, GBool printing) {
  int i;

  for (i = 0; i < fields->getLength(); ++i) {
    ((AcroFormField *)fields->get(i))->draw(pageNum, gfx, printing);
  }
}

int AcroForm::getNumFields() {
  return fields->getLength();
}

FormField *AcroForm::getField(int idx) {
  return (AcroFormField *)fields->get(idx);
}

//------------------------------------------------------------------------
// AcroFormField
//------------------------------------------------------------------------

AcroFormField *AcroFormField::load(AcroForm *acroFormA, Object *fieldRefA) {
  GString *typeStr;
  TextString *nameA;
  Guint flagsA;
  GBool haveFlags, typeFromParentA;
  Object fieldObjA, parentObj, parentObj2, obj1, obj2;
  AcroFormFieldType typeA;
  AcroFormField *field;

  fieldRefA->fetch(acroFormA->doc->getXRef(), &fieldObjA);

  //----- get field info

  if (fieldObjA.dictLookup("T", &obj1)->isString()) {
    nameA = new TextString(obj1.getString());
  } else {
    nameA = new TextString();
  }
  obj1.free();

  if (fieldObjA.dictLookup("FT", &obj1)->isName()) {
    typeStr = new GString(obj1.getName());
    typeFromParentA = gFalse;
  } else {
    typeStr = NULL;
    typeFromParentA = gTrue;
  }
  obj1.free();

  if (fieldObjA.dictLookup("Ff", &obj1)->isInt()) {
    flagsA = (Guint)obj1.getInt();
    haveFlags = gTrue;
  } else {
    flagsA = 0;
    haveFlags = gFalse;
  }
  obj1.free();

  //----- get info from parent non-terminal fields

  fieldObjA.dictLookup("Parent", &parentObj);
  while (parentObj.isDict()) {

    if (parentObj.dictLookup("T", &obj1)->isString()) {
      if (nameA->getLength()) {
	nameA->insert(0, (Unicode)'.');
      }
      nameA->insert(0, obj1.getString());
    }
    obj1.free();

    if (!typeStr) {
      if (parentObj.dictLookup("FT", &obj1)->isName()) {
	typeStr = new GString(obj1.getName());
      }
      obj1.free();
    }

    if (!haveFlags) {
      if (parentObj.dictLookup("Ff", &obj1)->isInt()) {
	flagsA = (Guint)obj1.getInt();
	haveFlags = gTrue;
      }
      obj1.free();
    }

    parentObj.dictLookup("Parent", &parentObj2);
    parentObj.free();
    parentObj = parentObj2;
  }
  parentObj.free();

  if (!typeStr) {
    error(errSyntaxError, -1, "Missing type in AcroForm field");
    goto err1;
  }

  //----- check for a radio button

  // this is a kludge: if we see a Btn-type field with kids, and the
  // Ff entry is missing, assume the kids are radio buttons
  if (typeFromParentA && !typeStr->cmp("Btn") && !haveFlags) {
    flagsA = acroFormFlagRadio;
  }

  //----- determine field type

  if (!typeStr->cmp("Btn")) {
    if (flagsA & acroFormFlagPushbutton) {
      typeA = acroFormFieldPushbutton;
    } else if (flagsA & acroFormFlagRadio) {
      typeA = acroFormFieldRadioButton;
    } else {
      typeA = acroFormFieldCheckbox;
    }
  } else if (!typeStr->cmp("Tx")) {
    if (flagsA & acroFormFlagFileSelect) {
      typeA = acroFormFieldFileSelect;
    } else if (flagsA & acroFormFlagMultiline) {
      typeA = acroFormFieldMultilineText;
    } else {
      typeA = acroFormFieldText;
    }
  } else if (!typeStr->cmp("Ch")) {
    if (flagsA & acroFormFlagCombo) {
      typeA = acroFormFieldComboBox;
    } else {
      typeA = acroFormFieldListBox;
    }
  } else if (!typeStr->cmp("Sig")) {
    typeA = acroFormFieldSignature;
  } else {
    error(errSyntaxError, -1, "Invalid type in AcroForm field");
    goto err1;
  }
  delete typeStr;

  field = new AcroFormField(acroFormA, fieldRefA, &fieldObjA,
			    typeA, nameA, flagsA, typeFromParentA);
  fieldObjA.free();
  return field;

 err1:
  delete typeStr;
  delete nameA;
  fieldObjA.free();
  return NULL;
}

AcroFormField::AcroFormField(AcroForm *acroFormA,
			     Object *fieldRefA, Object *fieldObjA,
			     AcroFormFieldType typeA, TextString *nameA,
			     Guint flagsA, GBool typeFromParentA) {
  acroForm = acroFormA;
  fieldRefA->copy(&fieldRef);
  fieldObjA->copy(&fieldObj);
  type = typeA;
  name = nameA;
  flags = flagsA;
  typeFromParent = typeFromParentA;
}

AcroFormField::~AcroFormField() {
  fieldRef.free();
  fieldObj.free();
  delete name;
}

int AcroFormField::getPageNum() {
  Object kidsObj, annotRef;
  int pageNum;

  pageNum = 0;
  if (fieldObj.dictLookup("Kids", &kidsObj)->isArray()) {
    if (kidsObj.arrayGetLength() > 0) {
      kidsObj.arrayGetNF(0, &annotRef);
      pageNum = acroForm->lookupAnnotPage(&annotRef);
      annotRef.free();
    }
  } else {
    pageNum = acroForm->lookupAnnotPage(&fieldRef);
  }
  kidsObj.free();
  return pageNum;
}

const char *AcroFormField::getType() {
  switch (type) {
  case acroFormFieldPushbutton:    return "PushButton";
  case acroFormFieldRadioButton:   return "RadioButton";
  case acroFormFieldCheckbox:      return "Checkbox";
  case acroFormFieldFileSelect:    return "FileSelect";
  case acroFormFieldMultilineText: return "MultilineText";
  case acroFormFieldText:          return "Text";
  case acroFormFieldComboBox:      return "ComboBox";
  case acroFormFieldListBox:       return "ListBox";
  case acroFormFieldSignature:     return "Signature";
  default:                         return NULL;
  }
}

Unicode *AcroFormField::getName(int *length) {
  Unicode *u, *ret;
  int n;

  u = name->getUnicode();
  n = name->getLength();
  ret = (Unicode *)gmallocn(n, sizeof(Unicode));
  memcpy(ret, u, n * sizeof(Unicode));
  *length = n;
  return ret;
}

Unicode *AcroFormField::getValue(int *length) {
  Object obj1, obj2;
  Unicode *u;
  char *s;
  TextString *ts;
  GString *gs;
  int n, i;

  u = NULL;
  *length = 0;

  fieldLookup("V", &obj1);
  if (obj1.isName()) {
    s = obj1.getName();
    n = (int)strlen(s);
    u = (Unicode *)gmallocn(n, sizeof(Unicode));
    for (i = 0; i < n; ++i) {
      u[i] = s[i] & 0xff;
    }
    *length = n;
  } else if (obj1.isString()) {
    ts = new TextString(obj1.getString());
    n = ts->getLength();
    u = (Unicode *)gmallocn(n, sizeof(Unicode));
    memcpy(u, ts->getUnicode(), n * sizeof(Unicode));
    *length = n;
    delete ts;
  } else if (obj1.isDict()) {
    obj1.dictLookup("Contents", &obj2);
    if (obj2.isString()) {
      gs = obj2.getString();
      n = gs->getLength();
      u = (Unicode *)gmallocn(n, sizeof(Unicode));
      for (i = 0; i < n; ++i) {
	u[i] = gs->getChar(i) & 0xff;
      }
      *length = n;
    }
    obj2.free();
  }
  obj1.free();

  return u;
}

void AcroFormField::getBBox(double *llx, double *lly,
			    double *urx, double *ury) {
  Object annotObj, rectObj, numObj;
  double t;

  *llx = *lly = *urx = *ury = 0;

  if (getAnnotObj(&annotObj)->isDict()) {
    annotObj.dictLookup("Rect", &rectObj);
    if (rectObj.isArray() && rectObj.arrayGetLength() == 4) {
      rectObj.arrayGet(0, &numObj);
      if (numObj.isNum()) {
	*llx = numObj.getNum();
      }
      numObj.free();
      rectObj.arrayGet(1, &numObj);
      if (numObj.isNum()) {
	*lly = numObj.getNum();
      }
      numObj.free();
      rectObj.arrayGet(2, &numObj);
      if (numObj.isNum()) {
	*urx = numObj.getNum();
      }
      numObj.free();
      rectObj.arrayGet(3, &numObj);
      if (numObj.isNum()) {
	*ury = numObj.getNum();
      }
      numObj.free();
    }
    rectObj.free();
  }
  annotObj.free();

  if (*llx > *urx) {
    t = *llx;  *llx = *urx;  *urx = t;
  }
  if (*lly > *ury) {
    t = *lly;  *lly = *ury;  *ury = t;
  }
}

void AcroFormField::getFont(Ref *fontID, double *fontSize) {
  Object daObj;
  GList *daToks;
  char *fontTag;
  double tfSize, m2, m3;
  int tfPos, tmPos, i;

  fontID->num = fontID->gen = -1;
  *fontSize = 0;

  if (fieldLookup("DA", &daObj)->isString()) {

    // parse the default appearance string
    daToks = tokenize(daObj.getString());
    tfPos = tmPos = -1;
    for (i = 2; i < daToks->getLength(); ++i) {
      if (!((GString *)daToks->get(i))->cmp("Tf")) {
	tfPos = i - 2;
      } else if (i >= 6 && !((GString *)daToks->get(i))->cmp("Tm")) {
	tmPos = i - 6;
      }
    }

    // handle the Tf operator
    if (tfPos >= 0) {
      fontTag = ((GString *)daToks->get(tfPos))->getCString();
      if (*fontTag == '/') {
	++fontTag;
      }
      *fontID = findFontName(fontTag);
      tfSize = atof(((GString *)daToks->get(tfPos + 1))->getCString());
    } else {
      tfSize = 1;
    }

    // handle the Tm operator
    if (tmPos >= 0) {
      // transformed font size = sqrt(m[2]^2 + m[3]^2) * size
      m2 = atof(((GString *)daToks->get(tfPos + 2))->getCString());
      m3 = atof(((GString *)daToks->get(tfPos + 3))->getCString());
      tfSize *= sqrt(m2*m2 + m3*m3);
    }
    *fontSize = tfSize;

    deleteGList(daToks, GString);
  }

  daObj.free();
}

Ref AcroFormField::findFontName(char *fontTag) {
  Object drObj, fontDictObj, fontObj, baseFontObj;
  Ref fontID;
  GBool found;

  fontID.num = fontID.gen = -1;
  found = gFalse;

  if (fieldObj.dictLookup("DR", &drObj)->isDict()) {
    if (drObj.dictLookup("Font", &fontDictObj)->isDict()) {
      if (fontDictObj.dictLookupNF(fontTag, &fontObj)->isRef()) {
	fontID = fontObj.getRef();
	found = gTrue;
      }
      fontObj.free();
    }
    fontDictObj.free();
  }
  drObj.free();
  if (found) {
    return fontID;
  }

  if (acroForm->acroFormObj.dictLookup("DR", &drObj)->isDict()) {
    if (drObj.dictLookup("Font", &fontDictObj)->isDict()) {
      if (fontDictObj.dictLookupNF(fontTag, &fontObj)->isRef()) {
	fontID = fontObj.getRef();
      }
      fontObj.free();
    }
    fontDictObj.free();
  }
  drObj.free();
  return fontID;
}

void AcroFormField::getColor(double *red, double *green, double *blue) {
  Object daObj;
  GList *daToks;
  int i;

  *red = *green = *blue = 0;

  if (fieldLookup("DA", &daObj)->isString()) {

    // parse the default appearance string
    daToks = tokenize(daObj.getString());
    for (i = 1; i < daToks->getLength(); ++i) {

      // handle the g operator
      if (!((GString *)daToks->get(i))->cmp("g")) {
	*red = *green = *blue =
	    atof(((GString *)daToks->get(i - 1))->getCString());
	break;

      // handle the rg operator
      } else if (i >= 3 && !((GString *)daToks->get(i))->cmp("rg")) {
	*red = atof(((GString *)daToks->get(i - 3))->getCString());
	*green = atof(((GString *)daToks->get(i - 2))->getCString());
	*blue = atof(((GString *)daToks->get(i - 1))->getCString());
	break;
      }
    }

    deleteGList(daToks, GString);
  }

  daObj.free();
}

int AcroFormField::getMaxLen() {
  Object obj;
  int len;

  if (fieldLookup("MaxLen", &obj)->isInt()) {
    len = obj.getInt();
  } else {
    len = -1;
  }
  obj.free();
  return len;
}

void AcroFormField::draw(int pageNum, Gfx *gfx, GBool printing) {
  Object kidsObj, annotRef, annotObj;
  int i;

  // find the annotation object(s)
  if (fieldObj.dictLookup("Kids", &kidsObj)->isArray()) {
    for (i = 0; i < kidsObj.arrayGetLength(); ++i) {
      kidsObj.arrayGetNF(i, &annotRef);
      annotRef.fetch(acroForm->doc->getXRef(), &annotObj);
      drawAnnot(pageNum, gfx, printing, &annotRef, &annotObj);
      annotObj.free();
      annotRef.free();
    }
  } else {
    drawAnnot(pageNum, gfx, printing, &fieldRef, &fieldObj);
  }
  kidsObj.free();
}

void AcroFormField::drawAnnot(int pageNum, Gfx *gfx, GBool printing,
			      Object *annotRef, Object *annotObj) {
  Object obj1, obj2;
  double xMin, yMin, xMax, yMax, t;
  int annotFlags;
  GBool oc, render;

  if (!annotObj->isDict()) {
    return;
  }

  //----- get the page number

  // the "P" (page) field in annotations is optional, so we can't
  // depend on it here
  if (acroForm->lookupAnnotPage(annotRef) != pageNum) {
    return;
  }

  //----- check annotation flags

  if (annotObj->dictLookup("F", &obj1)->isInt()) {
    annotFlags = obj1.getInt();
  } else {
    annotFlags = 0;
  }
  obj1.free();
  if ((annotFlags & annotFlagHidden) ||
      (printing && !(annotFlags & annotFlagPrint)) ||
      (!printing && (annotFlags & annotFlagNoView))) {
    return;
  }

  //----- check the optional content entry

  annotObj->dictLookupNF("OC", &obj1);
  if (acroForm->doc->getOptionalContent()->evalOCObject(&obj1, &oc) && !oc) {
    obj1.free();
    return;
  }
  obj1.free();

  //----- get the bounding box

  if (annotObj->dictLookup("Rect", &obj1)->isArray() &&
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
    obj1.free();
    return;
  }
  obj1.free();

  //----- draw it

  render = gFalse;
  if (acroForm->needAppearances) {
    render = gTrue;
  } else {
    if (!annotObj->dictLookup("AP", &obj1)->isDict()) {
      render = gTrue;
    }
    obj1.free();
  }
  if (render) {
    drawNewAppearance(gfx, annotObj->getDict(),
		      xMin, yMin, xMax, yMax);
  } else {
    drawExistingAppearance(gfx, annotObj->getDict(),
			   xMin, yMin, xMax, yMax);
  }
}

// Draw the existing appearance stream for a single annotation
// attached to this field.
void AcroFormField::drawExistingAppearance(Gfx *gfx, Dict *annot,
					   double xMin, double yMin,
					   double xMax, double yMax) {
  Object apObj, asObj, appearance, obj1;

  //----- get the appearance stream

  if (annot->lookup("AP", &apObj)->isDict()) {
    apObj.dictLookup("N", &obj1);
    if (obj1.isDict()) {
      if (annot->lookup("AS", &asObj)->isName()) {
	obj1.dictLookupNF(asObj.getName(), &appearance);
      } else if (obj1.dictGetLength() == 1) {
	obj1.dictGetValNF(0, &appearance);
      } else {
	obj1.dictLookupNF("Off", &appearance);
      }
      asObj.free();
    } else {
      apObj.dictLookupNF("N", &appearance);
    }
    obj1.free();
  }
  apObj.free();

  //----- draw it

  if (!appearance.isNone()) {
    gfx->drawAnnot(&appearance, NULL, xMin, yMin, xMax, yMax);
    appearance.free();
  }
}

// Regenerate the appearance for this field, and draw it.
void AcroFormField::drawNewAppearance(Gfx *gfx, Dict *annot,
				      double xMin, double yMin,
				      double xMax, double yMax) {
  GString *appearBuf;
  Object appearance, mkObj, ftObj, appearDict, drObj, apObj, asObj;
  Object resources, fontResources, defaultFont, gfxStateDict;
  Object obj1, obj2, obj3, obj4;
  Dict *mkDict;
  MemStream *appearStream;
  GfxFontDict *fontDict;
  GBool hasCaption;
  double dx, dy, r;
  GString *val, *caption, *da;
  GString **text;
  GBool done;
  GBool *selection;
  AnnotBorderType borderType;
  double borderWidth;
  double *borderDash;
  GString *appearanceState;
  int borderDashLength, rot, quadding, comb, nOptions, topIdx, i, j;

  appearBuf = new GString();

  // get the appearance characteristics (MK) dictionary
  if (annot->lookup("MK", &mkObj)->isDict()) {
    mkDict = mkObj.getDict();
  } else {
    mkDict = NULL;
  }

  // draw the background
  if (mkDict) {
    if (mkDict->lookup("BG", &obj1)->isArray() &&
	obj1.arrayGetLength() > 0) {
      setColor(obj1.getArray(), gTrue, 0, appearBuf);
      appearBuf->appendf("0 0 {0:.4f} {1:.4f} re f\n",
			 xMax - xMin, yMax - yMin);
    }
    obj1.free();
  }

  // get the field type
  fieldLookup("FT", &ftObj);

  // draw the border
  borderType = annotBorderSolid;
  borderWidth = 1;
  borderDash = NULL;
  borderDashLength = 0;
  if (annot->lookup("BS", &obj1)->isDict()) {
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
    if (annot->lookup("Border", &obj1)->isArray()) {
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
      }
    }
  }
  obj1.free();
  if (mkDict) {
    if (borderWidth > 0) {
      mkDict->lookup("BC", &obj1);
      if (!(obj1.isArray() && obj1.arrayGetLength() > 0)) {
	obj1.free();
	mkDict->lookup("BG", &obj1);
      }
      if (obj1.isArray() && obj1.arrayGetLength() > 0) {
	dx = xMax - xMin;
	dy = yMax - yMin;

	// radio buttons with no caption have a round border
	hasCaption = mkDict->lookup("CA", &obj2)->isString();
	obj2.free();
	if (ftObj.isName("Btn") && (flags & acroFormFlagRadio) && !hasCaption) {
	  r = 0.5 * (dx < dy ? dx : dy);
	  switch (borderType) {
	  case annotBorderDashed:
	    appearBuf->append("[");
	    for (i = 0; i < borderDashLength; ++i) {
	      appearBuf->appendf(" {0:.4f}", borderDash[i]);
	    }
	    appearBuf->append("] 0 d\n");
	    // fall through to the solid case
	  case annotBorderSolid:
	  case annotBorderUnderlined:
	    appearBuf->appendf("{0:.4f} w\n", borderWidth);
	    setColor(obj1.getArray(), gFalse, 0, appearBuf);
	    drawCircle(0.5 * dx, 0.5 * dy, r - 0.5 * borderWidth, "s",
		       appearBuf);
	    break;
	  case annotBorderBeveled:
	  case annotBorderInset:
	    appearBuf->appendf("{0:.4f} w\n", 0.5 * borderWidth);
	    setColor(obj1.getArray(), gFalse, 0, appearBuf);
	    drawCircle(0.5 * dx, 0.5 * dy, r - 0.25 * borderWidth, "s",
		       appearBuf);
	    setColor(obj1.getArray(), gFalse,
		     borderType == annotBorderBeveled ? 1 : -1,
		     appearBuf);
	    drawCircleTopLeft(0.5 * dx, 0.5 * dy, r - 0.75 * borderWidth,
			      appearBuf);
	    setColor(obj1.getArray(), gFalse,
		     borderType == annotBorderBeveled ? -1 : 1,
		     appearBuf);
	    drawCircleBottomRight(0.5 * dx, 0.5 * dy, r - 0.75 * borderWidth,
				  appearBuf);
	    break;
	  }

	} else {
	  switch (borderType) {
	  case annotBorderDashed:
	    appearBuf->append("[");
	    for (i = 0; i < borderDashLength; ++i) {
	      appearBuf->appendf(" {0:.4f}", borderDash[i]);
	    }
	    appearBuf->append("] 0 d\n");
	    // fall through to the solid case
	  case annotBorderSolid:
	    appearBuf->appendf("{0:.4f} w\n", borderWidth);
	    setColor(obj1.getArray(), gFalse, 0, appearBuf);
	    appearBuf->appendf("{0:.4f} {0:.4f} {1:.4f} {2:.4f} re s\n",
			       0.5 * borderWidth,
			       dx - borderWidth, dy - borderWidth);
	    break;
	  case annotBorderBeveled:
	  case annotBorderInset:
	    setColor(obj1.getArray(), gTrue,
		     borderType == annotBorderBeveled ? 1 : -1,
		     appearBuf);
	    appearBuf->append("0 0 m\n");
	    appearBuf->appendf("0 {0:.4f} l\n", dy);
	    appearBuf->appendf("{0:.4f} {1:.4f} l\n", dx, dy);
	    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
			       dx - borderWidth, dy - borderWidth);
	    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
			       borderWidth, dy - borderWidth);
	    appearBuf->appendf("{0:.4f} {0:.4f} l\n", borderWidth);
	    appearBuf->append("f\n");
	    setColor(obj1.getArray(), gTrue,
		     borderType == annotBorderBeveled ? -1 : 1,
		     appearBuf);
	    appearBuf->append("0 0 m\n");
	    appearBuf->appendf("{0:.4f} 0 l\n", dx);
	    appearBuf->appendf("{0:.4f} {1:.4f} l\n", dx, dy);
	    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
			       dx - borderWidth, dy - borderWidth);
	    appearBuf->appendf("{0:.4f} {1:.4f} l\n",
			       dx - borderWidth, borderWidth);
	    appearBuf->appendf("{0:.4f} {0:.4f} l\n", borderWidth);
	    appearBuf->append("f\n");
	    break;
	  case annotBorderUnderlined:
	    appearBuf->appendf("{0:.4f} w\n", borderWidth);
	    setColor(obj1.getArray(), gFalse, 0, appearBuf);
	    appearBuf->appendf("0 0 m {0:.4f} 0 l s\n", dx);
	    break;
	  }

	  // clip to the inside of the border
	  appearBuf->appendf("{0:.4f} {0:.4f} {1:.4f} {2:.4f} re W n\n",
			     borderWidth,
			     dx - 2 * borderWidth, dy - 2 * borderWidth);
	}
      }
      obj1.free();
    }
  }
  gfree(borderDash);

  // get the resource dictionary
  buildDefaultResourceDict(&drObj);

  // build the font dictionary
  if (drObj.isDict() && drObj.dictLookup("Font", &obj1)->isDict()) {
    fontDict = new GfxFontDict(acroForm->doc->getXRef(), NULL, obj1.getDict());
  } else {
    fontDict = NULL;
  }
  obj1.free();

  // get the default appearance string
  if (fieldLookup("DA", &obj1)->isString()) {
    da = obj1.getString()->copy();
  } else {
    da = NULL;
  }
  obj1.free();

  // get the rotation value
  rot = 0;
  if (mkDict) {
    if (mkDict->lookup("R", &obj1)->isInt()) {
      rot = obj1.getInt();
    }
    obj1.free();
  }

  // get the appearance state
  annot->lookup("AP", &apObj);
  annot->lookup("AS", &asObj);
  appearanceState = NULL;
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
  apObj.free();

  // draw the field contents
  if (ftObj.isName("Btn")) {
    caption = NULL;
    if (mkDict) {
      if (mkDict->lookup("CA", &obj1)->isString()) {
	caption = obj1.getString()->copy();
      }
      obj1.free();
    }
    // radio button
    if (flags & acroFormFlagRadio) {
      //~ Acrobat doesn't draw a caption if there is no AP dict (?)
      if (fieldLookup("V", &obj1)
	    ->isName(appearanceState->getCString())) {
	if (caption) {
	  drawText(caption, da, fontDict, gFalse, 0, acroFormQuadCenter,
		   gFalse, gTrue, rot, xMin, yMin, xMax, yMax, borderWidth,
		   appearBuf);
	} else {
	  if (mkDict) {
	    if (mkDict->lookup("BC", &obj2)->isArray() &&
		obj2.arrayGetLength() > 0) {
	      dx = xMax - xMin;
	      dy = yMax - yMin;
	      setColor(obj2.getArray(), gTrue, 0, appearBuf);
	      drawCircle(0.5 * dx, 0.5 * dy, 0.2 * (dx < dy ? dx : dy), "f",
			 appearBuf);
	    }
	    obj2.free();
	  }
	}
      }
      obj1.free();
    // pushbutton
    } else if (flags & acroFormFlagPushbutton) {
      if (caption) {
	drawText(caption, da, fontDict, gFalse, 0, acroFormQuadCenter,
		 gFalse, gFalse, rot, xMin, yMin, xMax, yMax, borderWidth,
		 appearBuf);
      }
    // checkbox
    } else {
      fieldLookup("V", &obj1);
      if (obj1.isName() && !(obj1.isName("Off") ||
			     obj1.isName("No") ||
			     obj1.isName(""))) {
	if (!caption) {
	  caption = new GString("3"); // ZapfDingbats checkmark
	}
	drawText(caption, da, fontDict, gFalse, 0, acroFormQuadCenter,
		 gFalse, gTrue, rot, xMin, yMin, xMax, yMax, borderWidth,
		 appearBuf);
      }
      obj1.free();
    }
    if (caption) {
      delete caption;
    }
  } else if (ftObj.isName("Tx")) {
    //~ value strings can be Unicode
    fieldLookup("V", &obj1);
    if (obj1.isString()) {
      if (fieldLookup("Q", &obj2)->isInt()) {
	quadding = obj2.getInt();
      } else {
	quadding = acroFormQuadLeft;
      }
      obj2.free();
      comb = 0;
      if (flags & acroFormFlagComb) {
	if (fieldLookup("MaxLen", &obj2)->isInt()) {
	  comb = obj2.getInt();
	}
	obj2.free();
      }
      drawText(obj1.getString(), da, fontDict,
	       flags & acroFormFlagMultiline, comb, quadding,
	       gTrue, gFalse, rot, xMin, yMin, xMax, yMax, borderWidth,
	       appearBuf);
    }
    obj1.free();
  } else if (ftObj.isName("Ch")) {
    //~ value/option strings can be Unicode
    if (fieldLookup("Q", &obj1)->isInt()) {
      quadding = obj1.getInt();
    } else {
      quadding = acroFormQuadLeft;
    }
    obj1.free();
    // combo box
    if (flags & acroFormFlagCombo) {
      if (fieldLookup("V", &obj1)->isString()) {
	val = obj1.getString()->copy();
	if (fieldObj.dictLookup("Opt", &obj2)->isArray()) {
	  for (i = 0, done = false; i < obj2.arrayGetLength() && !done; ++i) {
	    obj2.arrayGet(i, &obj3);
	    if (obj3.isArray() && obj3.arrayGetLength() == 2) {
	      if (obj3.arrayGet(0, &obj4)->isString() &&
		  obj4.getString()->cmp(val) == 0) {
		obj4.free();
		if (obj3.arrayGet(1, &obj4)->isString()) {
		  delete val;
		  val = obj4.getString()->copy();
		}
		done = gTrue;
	      }
	      obj4.free();
	    }
	    obj3.free();
	  }
	}
	obj2.free();
	drawText(val, da, fontDict,
		 gFalse, 0, quadding, gTrue, gFalse, rot,
		 xMin, yMin, xMax, yMax, borderWidth, appearBuf);
	delete val;
	//~ Acrobat draws a popup icon on the right side
      }
      obj1.free();
    // list box
    } else {
      if (fieldObj.dictLookup("Opt", &obj1)->isArray()) {
	nOptions = obj1.arrayGetLength();
	// get the option text
	text = (GString **)gmallocn(nOptions, sizeof(GString *));
	for (i = 0; i < nOptions; ++i) {
	  text[i] = NULL;
	  obj1.arrayGet(i, &obj2);
	  if (obj2.isString()) {
	    text[i] = obj2.getString()->copy();
	  } else if (obj2.isArray() && obj2.arrayGetLength() == 2) {
	    if (obj2.arrayGet(1, &obj3)->isString()) {
	      text[i] = obj3.getString()->copy();
	    }
	    obj3.free();
	  }
	  obj2.free();
	  if (!text[i]) {
	    text[i] = new GString();
	  }
	}
	// get the selected option(s)
	selection = (GBool *)gmallocn(nOptions, sizeof(GBool));
	//~ need to use the I field in addition to the V field
	fieldLookup("V", &obj2);
	for (i = 0; i < nOptions; ++i) {
	  selection[i] = gFalse;
	  if (obj2.isString()) {
	    if (!obj2.getString()->cmp(text[i])) {
	      selection[i] = gTrue;
	    }
	  } else if (obj2.isArray()) {
	    for (j = 0; j < obj2.arrayGetLength(); ++j) {
	      if (obj2.arrayGet(j, &obj3)->isString() &&
		  !obj3.getString()->cmp(text[i])) {
		selection[i] = gTrue;
	      }
	      obj3.free();
	    }
	  }
	}
	obj2.free();
	// get the top index
	if (fieldObj.dictLookup("TI", &obj2)->isInt()) {
	  topIdx = obj2.getInt();
	} else {
	  topIdx = 0;
	}
	obj2.free();
	// draw the text
	drawListBox(text, selection, nOptions, topIdx, da, fontDict, quadding,
		    xMin, yMin, xMax, yMax, borderWidth, appearBuf);
	for (i = 0; i < nOptions; ++i) {
	  delete text[i];
	}
	gfree(text);
	gfree(selection);
      }
      obj1.free();
    }
  } else if (ftObj.isName("Sig")) {
    //~ check to see if background is already drawn
    gfxStateDict.initDict(acroForm->doc->getXRef());
    obj1.initReal(0.5);
    gfxStateDict.dictAdd(copyString("ca"), &obj1);
    appearBuf->append("/GS1 gs\n");
    appearBuf->appendf("0.7 0.7 1 rg 0 0 {0:.2f} {1:.2f} re f\n",
		       xMax - xMin, yMax - yMin);
    caption = new GString("SIGN HERE");
    if (da) {
      delete da;
    }
    da = new GString("/Helv 10 Tf 1 0 0 rg");
    drawText(caption, da, fontDict,
	     gFalse, 0, acroFormQuadLeft, gFalse, gFalse, rot,
	     xMin, yMin, xMax, yMax, borderWidth, appearBuf);
    delete caption;
  } else {
    error(errSyntaxError, -1, "Unknown field type");
  }

  delete appearanceState;
  if (da) {
    delete da;
  }

  // build the appearance stream dictionary
  appearDict.initDict(acroForm->doc->getXRef());
  appearDict.dictAdd(copyString("Length"),
		     obj1.initInt(appearBuf->getLength()));
  appearDict.dictAdd(copyString("Subtype"), obj1.initName("Form"));
  obj1.initArray(acroForm->doc->getXRef());
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(0));
  obj1.arrayAdd(obj2.initReal(xMax - xMin));
  obj1.arrayAdd(obj2.initReal(yMax - yMin));
  appearDict.dictAdd(copyString("BBox"), &obj1);

  // set the resource dictionary; add a default font
  if (drObj.isDict()) {
    drObj.copy(&resources);
  } else {
    resources.initDict(acroForm->doc->getXRef());
  }
  drObj.free();
  fontResources.initDict(acroForm->doc->getXRef());
  if (resources.dictLookup("Font", &obj1)->isDict()) {
    for (i = 0; i < obj1.dictGetLength(); ++i) {
      obj1.dictGetValNF(i, &obj2);
      fontResources.dictAdd(copyString(obj1.dictGetKey(i)), &obj2);
    }
  }
  obj1.free();
  defaultFont.initDict(acroForm->doc->getXRef());
  defaultFont.dictAdd(copyString("Type"), obj1.initName("Font"));
  defaultFont.dictAdd(copyString("Subtype"), obj1.initName("Type1"));
  defaultFont.dictAdd(copyString("BaseFont"), obj1.initName("Helvetica"));
  defaultFont.dictAdd(copyString("Encoding"), obj1.initName("WinAnsiEncoding"));
  fontResources.dictAdd(copyString("xpdf_default_font"), &defaultFont);
  resources.dictAdd(copyString("Font"), &fontResources);
  if (gfxStateDict.isDict()) {
    obj1.initDict(acroForm->doc->getXRef());
    obj1.dictAdd(copyString("GS1"), &gfxStateDict);
    resources.dictAdd(copyString("ExtGState"), &obj1);
  }
  appearDict.dictAdd(copyString("Resources"), &resources);

  // build the appearance stream
  appearStream = new MemStream(appearBuf->getCString(), 0,
			       appearBuf->getLength(), &appearDict);
  appearance.initStream(appearStream);

  // draw it
  gfx->drawAnnot(&appearance, NULL, xMin, yMin, xMax, yMax);

  appearance.free();
  delete appearBuf;
  appearBuf = NULL;
  if (fontDict) {
    delete fontDict;
  }
  ftObj.free();
  mkObj.free();
}

// Set the current fill or stroke color, based on <a> (which should
// have 1, 3, or 4 elements).  If <adjust> is +1, color is brightened;
// if <adjust> is -1, color is darkened; otherwise color is not
// modified.
void AcroFormField::setColor(Array *a, GBool fill, int adjust,
			     GString *appearBuf) {
  Object obj1;
  double color[4];
  int nComps, i;

  nComps = a->getLength();
  if (nComps > 4) {
    nComps = 4;
  }
  for (i = 0; i < nComps && i < 4; ++i) {
    if (a->get(i, &obj1)->isNum()) {
      color[i] = obj1.getNum();
    } else {
      color[i] = 0;
    }
    obj1.free();
  }
  if (nComps == 4) {
    adjust = -adjust;
  }
  if (adjust > 0) {
    for (i = 0; i < nComps; ++i) {
      color[i] = 0.5 * color[i] + 0.5;
    }
  } else if (adjust < 0) {
    for (i = 0; i < nComps; ++i) {
      color[i] = 0.5 * color[i];
    }
  }
  if (nComps == 4) {
    appearBuf->appendf("{0:.2f} {1:.2f} {2:.2f} {3:.2f} {4:c}\n",
		       color[0], color[1], color[2], color[3],
		       fill ? 'k' : 'K');
  } else if (nComps == 3) {
    appearBuf->appendf("{0:.2f} {1:.2f} {2:.2f} {3:s}\n",
		       color[0], color[1], color[2],
		       fill ? "rg" : "RG");
  } else {
    appearBuf->appendf("{0:.2f} {1:c}\n",
		       color[0],
		       fill ? 'g' : 'G');
  }
}

// Draw the variable text or caption for a field.
void AcroFormField::drawText(GString *text, GString *da, GfxFontDict *fontDict,
			     GBool multiline, int comb, int quadding,
			     GBool txField, GBool forceZapfDingbats, int rot,
			     double xMin, double yMin, double xMax, double yMax,
			     double border, GString *appearBuf) {
  GString *text2;
  GList *daToks;
  GString *tok;
  GfxFont *font;
  double dx, dy;
  double fontSize, fontSize2, x, xPrev, y, w, w2, wMax;
  int tfPos, tmPos, i, j, k, c;

  //~ if there is no MK entry, this should use the existing content stream,
  //~ and only replace the marked content portion of it
  //~ (this is only relevant for Tx fields)

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
  if (text2->getLength() == 0) {
    if (text2 != text) {
      delete text2;
    }
    return;
  }

  // parse the default appearance string
  tfPos = tmPos = -1;
  if (da) {
    daToks = tokenize(da);
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

  // force ZapfDingbats
  //~ this should create the font if needed (?)
  if (forceZapfDingbats) {
    if (tfPos >= 0) {
      tok = (GString *)daToks->get(tfPos);
      if (tok->cmp("/ZaDb")) {
	tok->clear();
	tok->append("/ZaDb");
      }
    }
  }

  // get the font and font size
  font = NULL;
  fontSize = 0;
  if (tfPos >= 0) {
    tok = (GString *)daToks->get(tfPos);
    if (tok->getLength() >= 1 && tok->getChar(0) == '/') {
      if (!fontDict || !(font = fontDict->lookup(tok->getCString() + 1))) {
	error(errSyntaxError, -1, "Unknown font in field's DA string");
	tok->clear();
	tok->append("/xpdf_default_font");
      }
    } else {
      error(errSyntaxError, -1,
	    "Invalid font name in 'Tf' operator in field's DA string");
    }
    tok = (GString *)daToks->get(tfPos + 1);
    fontSize = atof(tok->getCString());
  } else {
    error(errSyntaxError, -1, "Missing 'Tf' operator in field's DA string");
    fontSize = 0;
    if (!daToks) {
      daToks = new GList();
    }
    tfPos = daToks->getLength();
    daToks->append(new GString("/xpdf_default_font"));
    daToks->append(new GString("10"));
    daToks->append(new GString("Tf"));
  }

  // setup
  if (txField) {
    appearBuf->append("/Tx BMC\n");
  }
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

  // multi-line text
  if (multiline) {
    // note: the comb flag is ignored in multiline mode

    wMax = dx - 2 * border - 4;

    // compute font autosize
    if (fontSize == 0) {
      for (fontSize = 10; fontSize > 1; --fontSize) {
	y = dy - 3;
	w2 = 0;
	i = 0;
	while (i < text2->getLength()) {
	  getNextLine(text2, i, font, fontSize, wMax, &j, &w, &k);
	  if (w > w2) {
	    w2 = w;
	  }
	  i = k;
	  y -= fontSize;
	}
	// approximate the descender for the last line
	if (y >= 0.33 * fontSize && w <= wMax) {
	  break;
	}
      }
      if (tfPos >= 0) {
	tok = (GString *)daToks->get(tfPos + 1);
	tok->clear();
	tok->appendf("{0:.2f}", fontSize);
      }
    }

    // starting y coordinate
    // (note: each line of text starts with a Td operator that moves
    // down a line)
    if (dy > fontSize + 6) {
      y = dy - 3;
    } else {
      y = 0.5 * dy - 0.4 * fontSize + fontSize;
    }

    // set the font matrix
    if (tmPos >= 0) {
      tok = (GString *)daToks->get(tmPos + 4);
      tok->clear();
      tok->append('0');
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
      appearBuf->appendf("1 0 0 1 0 {0:.4f} Tm\n", y);
    }

    // write a series of lines of text
    i = 0;
    xPrev = 0;
    while (i < text2->getLength()) {

      getNextLine(text2, i, font, fontSize, wMax, &j, &w, &k);

      // compute text start position
      switch (quadding) {
      case acroFormQuadLeft:
      default:
	x = border + 2;
	break;
      case acroFormQuadCenter:
	x = (dx - w) / 2;
	break;
      case acroFormQuadRight:
	x = dx - border - 2 - w;
	break;
      }

      // draw the line
      appearBuf->appendf("{0:.4f} {1:.4f} Td\n", x - xPrev, -fontSize);
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
      xPrev = x;
    }

  // single-line text
  } else {
    //~ replace newlines with spaces? - what does Acrobat do?

    // comb formatting
    if (comb > 0) {

      // compute comb spacing
      w = (dx - 2 * border) / comb;

      // compute font autosize
      if (fontSize == 0) {
	fontSize = dy - 2 * border;
	if (w < fontSize) {
	  fontSize = w;
	}
	fontSize = floor(fontSize);
	if (fontSize > 10) {
	  fontSize = 10;
	}
	if (tfPos >= 0) {
	  tok = (GString *)daToks->get(tfPos + 1);
	  tok->clear();
	  tok->appendf("{0:.4f}", fontSize);
	}
      }

      // compute text start position
      switch (quadding) {
      case acroFormQuadLeft:
      default:
	x = border + 2;
	break;
      case acroFormQuadCenter:
	x = border + 2 + 0.5 * (comb - text2->getLength()) * w;
	break;
      case acroFormQuadRight:
	x = border + 2 + (comb - text2->getLength()) * w;
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
      //~ this should center (instead of left-justify) each character within
      //~     its comb cell
      for (i = 0; i < text2->getLength(); ++i) {
	if (i > 0) {
	  appearBuf->appendf("{0:.4f} 0 Td\n", w);
	}
	appearBuf->append('(');
	c = text2->getChar(i) & 0xff;
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
	w = 0;
	for (i = 0; i < text2->getLength(); ++i) {
	  w += ((Gfx8BitFont *)font)->getWidth(text2->getChar(i));
	}
      } else {
	// otherwise, make a crude estimate
	w = text2->getLength() * 0.5;
      }

      // compute font autosize
      if (fontSize == 0) {
	fontSize = dy - 2 * border;
	fontSize2 = (dx - 4 - 2 * border) / w;
	if (fontSize2 < fontSize) {
	  fontSize = fontSize2;
	}
	fontSize = floor(fontSize);
	if (fontSize > 10) {
	  fontSize = 10;
	}
	if (tfPos >= 0) {
	  tok = (GString *)daToks->get(tfPos + 1);
	  tok->clear();
	  tok->appendf("{0:.4f}", fontSize);
	}
      }

      // compute text start position
      w *= fontSize;
      switch (quadding) {
      case acroFormQuadLeft:
      default:
	x = border + 2;
	break;
      case acroFormQuadCenter:
	x = (dx - w) / 2;
	break;
      case acroFormQuadRight:
	x = dx - border - 2 - w;
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
    }
  }

  // cleanup
  appearBuf->append("ET\n");
  appearBuf->append("Q\n");
  if (txField) {
    appearBuf->append("EMC\n");
  }

  if (daToks) {
    deleteGList(daToks, GString);
  }
  if (text2 != text) {
    delete text2;
  }
}

// Draw the variable text or caption for a field.
void AcroFormField::drawListBox(GString **text, GBool *selection,
				int nOptions, int topIdx,
				GString *da, GfxFontDict *fontDict,
				GBool quadding, double xMin, double yMin,
				double xMax, double yMax, double border,
				GString *appearBuf) {
  GList *daToks;
  GString *tok;
  GfxFont *font;
  double fontSize, fontSize2, x, y, w, wMax;
  int tfPos, tmPos, i, j, c;

  //~ if there is no MK entry, this should use the existing content stream,
  //~ and only replace the marked content portion of it
  //~ (this is only relevant for Tx fields)

  // parse the default appearance string
  tfPos = tmPos = -1;
  if (da) {
    daToks = tokenize(da);
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
  font = NULL;
  fontSize = 0;
  if (tfPos >= 0) {
    tok = (GString *)daToks->get(tfPos);
    if (tok->getLength() >= 1 && tok->getChar(0) == '/') {
      if (!fontDict || !(font = fontDict->lookup(tok->getCString() + 1))) {
	error(errSyntaxError, -1, "Unknown font in field's DA string");
	tok->clear();
	tok->append("/xpdf_default_font");
      }
    } else {
      error(errSyntaxError, -1,
	    "Invalid font name in 'Tf' operator in field's DA string");
    }
    tok = (GString *)daToks->get(tfPos + 1);
    fontSize = atof(tok->getCString());
  } else {
    error(errSyntaxError, -1, "Missing 'Tf' operator in field's DA string");
  }

  // compute font autosize
  if (fontSize == 0) {
    wMax = 0;
    for (i = 0; i < nOptions; ++i) {
      if (font && !font->isCIDFont()) {
	w = 0;
	for (j = 0; j < text[i]->getLength(); ++j) {
	  w += ((Gfx8BitFont *)font)->getWidth(text[i]->getChar(j));
	}
      } else {
	// otherwise, make a crude estimate
	w = text[i]->getLength() * 0.5;
      }
      if (w > wMax) {
	wMax = w;
      }
    }
    fontSize = yMax - yMin - 2 * border;
    fontSize2 = (xMax - xMin - 4 - 2 * border) / wMax;
    if (fontSize2 < fontSize) {
      fontSize = fontSize2;
    }
    fontSize = floor(fontSize);
    if (fontSize > 10) {
      fontSize = 10;
    }
    if (tfPos >= 0) {
      tok = (GString *)daToks->get(tfPos + 1);
      tok->clear();
      tok->appendf("{0:.4f}", fontSize);
    }
  }

  // draw the text
  y = yMax - yMin - 1.1 * fontSize;
  for (i = topIdx; i < nOptions; ++i) {

    // setup
    appearBuf->append("q\n");

    // draw the background if selected
    if (selection[i]) {
      appearBuf->append("0 g f\n");
      appearBuf->appendf("{0:.4f} {1:.4f} {2:.4f} {3:.4f} re f\n",
	      border,
	      y - 0.2 * fontSize,
	      xMax - xMin - 2 * border,
	      1.1 * fontSize);
    }

    // setup
    appearBuf->append("BT\n");

    // compute string width
    if (font && !font->isCIDFont()) {
      w = 0;
      for (j = 0; j < text[i]->getLength(); ++j) {
	w += ((Gfx8BitFont *)font)->getWidth(text[i]->getChar(j));
      }
    } else {
      // otherwise, make a crude estimate
      w = text[i]->getLength() * 0.5;
    }

    // compute text start position
    w *= fontSize;
    switch (quadding) {
    case acroFormQuadLeft:
    default:
      x = border + 2;
      break;
    case acroFormQuadCenter:
      x = (xMax - xMin - w) / 2;
      break;
    case acroFormQuadRight:
      x = xMax - xMin - border - 2 - w;
      break;
    }

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
      for (j = 0; j < daToks->getLength(); ++j) {
	appearBuf->append((GString *)daToks->get(j))->append(' ');
      }
    }

    // write the font matrix (if not part of the DA string)
    if (tmPos < 0) {
      appearBuf->appendf("1 0 0 1 {0:.4f} {1:.4f} Tm\n", x, y);
    }

    // change the text color if selected
    if (selection[i]) {
      appearBuf->append("1 g\n");
    }

    // write the text string
    appearBuf->append('(');
    for (j = 0; j < text[i]->getLength(); ++j) {
      c = text[i]->getChar(j) & 0xff;
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

    // next line
    y -= 1.1 * fontSize;
  }

  if (daToks) {
    deleteGList(daToks, GString);
  }
}

// Figure out how much text will fit on the next line.  Returns:
// *end = one past the last character to be included
// *width = width of the characters start .. end-1
// *next = index of first character on the following line
void AcroFormField::getNextLine(GString *text, int start,
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

// Draw an (approximate) circle of radius <r> centered at (<cx>, <cy>).
// <cmd> is used to draw the circle ("f", "s", or "b").
void AcroFormField::drawCircle(double cx, double cy, double r,
			       const char *cmd, GString *appearBuf) {
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
void AcroFormField::drawCircleTopLeft(double cx, double cy, double r,
				      GString *appearBuf) {
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
void AcroFormField::drawCircleBottomRight(double cx, double cy, double r,
					  GString *appearBuf) {
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

GList *AcroFormField::tokenize(GString *s) {
  GList *toks;
  int i, j;

  toks = new GList();
  i = 0;
  while (i < s->getLength()) {
    while (i < s->getLength() && Lexer::isSpace(s->getChar(i))) {
      ++i;
    }
    if (i < s->getLength()) {
      for (j = i + 1;
	   j < s->getLength() && !Lexer::isSpace(s->getChar(j));
	   ++j) ;
      toks->append(new GString(s, i, j - i));
      i = j;
    }
  }
  return toks;
}

Object *AcroFormField::getResources(Object *res) {
  Object kidsObj, annotObj, obj1;
  int i;

  if (acroForm->needAppearances) {
    fieldLookup("DR", res);
  } else {
    res->initArray(acroForm->doc->getXRef());
    // find the annotation object(s)
    if (fieldObj.dictLookup("Kids", &kidsObj)->isArray()) {
      for (i = 0; i < kidsObj.arrayGetLength(); ++i) {
	kidsObj.arrayGet(i, &annotObj);
	if (annotObj.isDict()) {
	  if (getAnnotResources(annotObj.getDict(), &obj1)->isDict()) {
	    res->arrayAdd(&obj1);
	  } else {
	    obj1.free();
	  }
	}
	annotObj.free();
      }
    } else {
      if (getAnnotResources(fieldObj.getDict(), &obj1)->isDict()) {
	res->arrayAdd(&obj1);
      } else {
	obj1.free();
      }
    }
    kidsObj.free();
  }

  return res;
}

Object *AcroFormField::getFieldRef(Object *ref) {
  return fieldRef.copy(ref);
}

Object *AcroFormField::getValueObj(Object *val) {
  return fieldLookup("V", val);
}

Object *AcroFormField::getParentRef(Object *parent) {
  return fieldObj.dictLookupNF("Parent", parent);
}

// Get the first annotation object associated with this field.
Object *AcroFormField::getAnnotObj(Object *annotObj) {
  Object kidsObj;

  if (fieldObj.dictLookup("Kids", &kidsObj)->isArray()) {
    if (kidsObj.arrayGetLength() > 0) {
      kidsObj.arrayGet(0, annotObj);
    } else {
      annotObj->initNull();
    }
  } else {
    fieldObj.copy(annotObj);
  }
  kidsObj.free();
  return annotObj;
}

Object *AcroFormField::getAnnotResources(Dict *annot, Object *res) {
  Object apObj, asObj, appearance, obj1;

  // get the appearance stream
  if (annot->lookup("AP", &apObj)->isDict()) {
    apObj.dictLookup("N", &obj1);
    if (obj1.isDict()) {
      if (annot->lookup("AS", &asObj)->isName()) {
	obj1.dictLookup(asObj.getName(), &appearance);
      } else if (obj1.dictGetLength() == 1) {
	obj1.dictGetVal(0, &appearance);
      } else {
	obj1.dictLookup("Off", &appearance);
      }
      asObj.free();
    } else {
      obj1.copy(&appearance);
    }
    obj1.free();
  }
  apObj.free();

  if (appearance.isStream()) {
    appearance.streamGetDict()->lookup("Resources", res);
  } else {
    res->initNull();
  }
  appearance.free();

  return res;
}

// Merge the field and AcroForm DR objects.
void AcroFormField::buildDefaultResourceDict(Object *dr) {
  Object formDR, fieldDR, resDict, newResDict, resObj;
  char *resType, *resName;
  int i, j;

  // NB: we need to deep-copy the dictionaries here, because multiple
  // threads can be sharing these objects.

  dr->initDict(acroForm->doc->getXRef());

  acroForm->acroFormObj.dictLookup("DR", &formDR);
  if (formDR.isDict()) {
    for (i = 0; i < formDR.dictGetLength(); ++i) {
      resType = formDR.dictGetKey(i);
      formDR.dictGetVal(i, &resDict);
      if (resDict.isDict()) {
	newResDict.initDict(acroForm->doc->getXRef());
	dr->dictAdd(copyString(resType), &newResDict);
	for (j = 0; j < resDict.dictGetLength(); ++j) {
	  resName = resDict.dictGetKey(j);
	  resDict.dictGetValNF(j, &resObj);
	  newResDict.dictAdd(copyString(resName), &resObj);
	}
      }
      resDict.free();
    }
  }
  formDR.free();

  fieldObj.dictLookup("DR", &fieldDR);
  if (fieldDR.isDict()) {
    for (i = 0; i < fieldDR.dictGetLength(); ++i) {
      resType = fieldDR.dictGetKey(i);
      fieldDR.dictGetVal(i, &resDict);
      if (resDict.isDict()) {
	dr->dictLookup(resType, &newResDict);
	if (!newResDict.isDict()) {
	  newResDict.free();
	  newResDict.initDict(acroForm->doc->getXRef());
	}
	dr->dictAdd(copyString(resType), &newResDict);
	for (j = 0; j < resDict.dictGetLength(); ++j) {
	  resName = resDict.dictGetKey(j);
	  resDict.dictGetValNF(j, &resObj);
	  newResDict.dictAdd(copyString(resName), &resObj);
	}
      }
      resDict.free();
    }
  }
  fieldDR.free();
}

// Look up an inheritable field dictionary entry.
Object *AcroFormField::fieldLookup(const char *key, Object *obj) {
  return fieldLookup(fieldObj.getDict(), key, obj);
}

Object *AcroFormField::fieldLookup(Dict *dict, const char *key, Object *obj) {
  Object parent;

  if (!dict->lookup(key, obj)->isNull()) {
    return obj;
  }
  obj->free();
  if (dict->lookup("Parent", &parent)->isDict()) {
    fieldLookup(parent.getDict(), key, obj);
  } else {
    // some fields don't specify a parent, so we check the AcroForm
    // dictionary just in case
    acroForm->acroFormObj.dictLookup(key, obj);
  }
  parent.free();
  return obj;
}
