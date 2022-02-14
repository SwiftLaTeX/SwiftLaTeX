//========================================================================
//
// Outline.cc
//
// Copyright 2002-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "Error.h"
#include "Link.h"
#include "TextString.h"
#include "Outline.h"

//------------------------------------------------------------------------

Outline::Outline(Object *outlineObj, XRef *xref) {
  Object first, last;

  items = NULL;
  if (!outlineObj->isDict()) {
    return;
  }
  outlineObj->dictLookupNF("First", &first);
  outlineObj->dictLookupNF("Last", &last);
  if (first.isRef() && last.isRef()) {
    items = OutlineItem::readItemList(&first, &last, NULL, xref);
  }
  first.free();
  last.free();
}

Outline::~Outline() {
  if (items) {
    deleteGList(items, OutlineItem);
  }
}

//------------------------------------------------------------------------

OutlineItem::OutlineItem(Object *itemRefA, Dict *dict,
			 OutlineItem *parentA, XRef *xrefA) {
  Object obj1;

  xref = xrefA;
  title = NULL;
  action = NULL;
  kids = NULL;
  parent = parentA;

  if (dict->lookup("Title", &obj1)->isString()) {
    title = new TextString(obj1.getString());
  }
  obj1.free();

  if (!dict->lookup("Dest", &obj1)->isNull()) {
    action = LinkAction::parseDest(&obj1);
  } else {
    obj1.free();
    if (!dict->lookup("A", &obj1)->isNull()) {
      action = LinkAction::parseAction(&obj1);
    }
  }
  obj1.free();

  itemRefA->copy(&itemRef);
  dict->lookupNF("First", &firstRef);
  dict->lookupNF("Last", &lastRef);
  dict->lookupNF("Next", &nextRef);

  startsOpen = gFalse;
  if (dict->lookup("Count", &obj1)->isInt()) {
    if (obj1.getInt() > 0) {
      startsOpen = gTrue;
    }
  }
  obj1.free();

  pageNum = -1;
}

OutlineItem::~OutlineItem() {
  close();
  if (title) {
    delete title;
  }
  if (action) {
    delete action;
  }
  itemRef.free();
  firstRef.free();
  lastRef.free();
  nextRef.free();
}

GList *OutlineItem::readItemList(Object *firstItemRef, Object *lastItemRef,
				 OutlineItem *parentA, XRef *xrefA) {
  GList *items;
  OutlineItem *item, *sibling;
  Object obj;
  Object *p;
  OutlineItem *ancestor;
  int i;

  items = new GList();
  if (!firstItemRef->isRef() || !lastItemRef->isRef()) {
    return items;
  }
  p = firstItemRef;
  do {
    if (!p->fetch(xrefA, &obj)->isDict()) {
      obj.free();
      break;
    }
    item = new OutlineItem(p, obj.getDict(), parentA, xrefA);
    obj.free();

    // check for loops with parents
    for (ancestor = parentA; ancestor; ancestor = ancestor->parent) {
      if (p->getRefNum() == ancestor->itemRef.getRefNum() &&
	  p->getRefGen() == ancestor->itemRef.getRefGen()) {
	error(errSyntaxError, -1, "Loop detected in outline");
	break;
      }
    }
    if (ancestor) {
      delete item;
      break;
    }

    // check for loops with siblings
    for (i = 0; i < items->getLength(); ++i) {
      sibling = (OutlineItem *)items->get(i);
      if (p->getRefNum() == sibling->itemRef.getRefNum() &&
	  p->getRefGen() == sibling->itemRef.getRefGen()) {
	error(errSyntaxError, -1, "Loop detected in outline");
	break;
      }
    }
    if (i < items->getLength()) {
      delete item;
      break;
    }

    items->append(item);
    if (p->getRefNum() == lastItemRef->getRef().num &&
	p->getRefGen() == lastItemRef->getRef().gen) {
      break;
    }
    p = &item->nextRef;
    if (!p->isRef()) {
      break;
    }
  } while (p);
  return items;
}

void OutlineItem::open() {
  if (!kids) {
    kids = readItemList(&firstRef, &lastRef, this, xref);
  }
}

void OutlineItem::close() {
  if (kids) {
    deleteGList(kids, OutlineItem);
    kids = NULL;
  }
}

Unicode *OutlineItem::getTitle() {
  return title ? title->getUnicode() : (Unicode *)NULL;
}

int OutlineItem::getTitleLength() {
  return title ? title->getLength() : 0;
}
