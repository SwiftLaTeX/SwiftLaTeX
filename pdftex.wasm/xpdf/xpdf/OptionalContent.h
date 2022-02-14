//========================================================================
//
// OptionalContent.h
//
// Copyright 2008-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef OPTIONALCONTENT_H
#define OPTIONALCONTENT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "Object.h"
#include "CharTypes.h"

class GString;
class GList;
class PDFDoc;
class TextString;
class XRef;
class OptionalContentGroup;
class OCDisplayNode;

//------------------------------------------------------------------------

class OptionalContent {
public:

  OptionalContent(PDFDoc *doc);
  ~OptionalContent();

  // Walk the list of optional content groups.
  int getNumOCGs();
  OptionalContentGroup *getOCG(int idx);

  // Find an OCG by indirect reference.
  OptionalContentGroup *findOCG(Ref *ref);

  // Get the root node of the optional content group display tree
  // (which does not necessarily include all of the OCGs).
  OCDisplayNode *getDisplayRoot() { return display; }

  // Evaluate an optional content object -- either an OCG or an OCMD.
  // If <obj> is a valid OCG or OCMD, sets *<visible> and returns
  // true; otherwise returns false.
  GBool evalOCObject(Object *obj, GBool *visible);

private:

  GBool evalOCVisibilityExpr(Object *expr, int recursion);

  XRef *xref;
  GList *ocgs;			// all OCGs [OptionalContentGroup]
  OCDisplayNode *display;	// root node of display tree 
};

//------------------------------------------------------------------------

// Values from the optional content usage dictionary.
enum OCUsageState {
  ocUsageOn,
  ocUsageOff,
  ocUsageUnset
};

//------------------------------------------------------------------------

class OptionalContentGroup {
public:

  static OptionalContentGroup *parse(Ref *refA, Object *obj);
  ~OptionalContentGroup();

  GBool matches(Ref *refA);

  Unicode *getName();
  int getNameLength();
  OCUsageState getViewState() { return viewState; }
  OCUsageState getPrintState() { return printState; }
  GBool getState() { return state; }
  void setState(GBool stateA) { state = stateA; }
  GBool getInViewUsageAppDict() { return inViewUsageAppDict; }
  void setInViewUsageAppDict() { inViewUsageAppDict = gTrue; }

private:

  OptionalContentGroup(Ref *refA, TextString *nameA,
		       OCUsageState viewStateA, OCUsageState printStateA);

  Ref ref;
  TextString *name;
  OCUsageState viewState,	// suggested state when viewing
               printState;	// suggested state when printing
  GBool state;			// current state (on/off)
  GBool inViewUsageAppDict;	// true if this OCG is listed in a
				//   usage app dict with Event=View

  friend class OCDisplayNode;
};

//------------------------------------------------------------------------

class OCDisplayNode {
public:

  static OCDisplayNode *parse(Object *obj, OptionalContent *oc, XRef *xref,
			      int recursion = 0);
  OCDisplayNode();
  ~OCDisplayNode();

  Unicode *getName();
  int getNameLength();
  OptionalContentGroup *getOCG() { return ocg; }
  int getNumChildren();
  OCDisplayNode *getChild(int idx);
  OCDisplayNode *getParent() { return parent; }

private:

  OCDisplayNode(GString *nameA);
  OCDisplayNode(OptionalContentGroup *ocgA);
  void addChild(OCDisplayNode *child);
  void addChildren(GList *childrenA);
  GList *takeChildren();

  TextString *name;		// display name
  OptionalContentGroup *ocg;	// NULL for display labels
  OCDisplayNode *parent;	// parent node; NULL at root
  GList *children;		// NULL if there are no children
				//   [OCDisplayNode]
};

#endif
