//========================================================================
//
// Gfx.h
//
// Copyright 1996-2016 Glyph & Cog, LLC
//
//========================================================================

#ifndef GFX_H
#define GFX_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "gfile.h"
#include "GfxState.h"

class GString;
class GList;
class PDFDoc;
class XRef;
class Array;
class Stream;
class Parser;
class Dict;
class Function;
class OutputDev;
class GfxFontDict;
class GfxFont;
class Gfx;
class PDFRectangle;
class AnnotBorderStyle;

//------------------------------------------------------------------------

enum GfxClipType {
  clipNone,
  clipNormal,
  clipEO
};

enum TchkType {
  tchkBool,			// boolean
  tchkInt,			// integer
  tchkNum,			// number (integer or real)
  tchkString,			// string
  tchkName,			// name
  tchkArray,			// array
  tchkProps,			// properties (dictionary or name)
  tchkSCN,			// scn/SCN args (number of name)
  tchkNone			// used to avoid empty initializer lists
};

#define maxArgs 33

struct Operator {
  char name[4];
  int numArgs;
  TchkType tchk[maxArgs];
  void (Gfx::*func)(Object args[], int numArgs);
};

//------------------------------------------------------------------------

class GfxResources {
public:

  GfxResources(XRef *xref, Dict *resDict, GfxResources *nextA);
  ~GfxResources();

  GfxFont *lookupFont(char *name);
  GfxFont *lookupFontByRef(Ref ref);
  GBool lookupXObject(const char *name, Object *obj);
  GBool lookupXObjectNF(const char *name, Object *obj);
  void lookupColorSpace(const char *name, Object *obj);
  GfxPattern *lookupPattern(const char *name
			    );
  GfxShading *lookupShading(const char *name
			    );
  GBool lookupGState(const char *name, Object *obj);
  GBool lookupPropertiesNF(const char *name, Object *obj);

  GfxResources *getNext() { return next; }

private:

  GfxFontDict *fonts;
  Object xObjDict;
  Object colorSpaceDict;
  Object patternDict;
  Object shadingDict;
  Object gStateDict;
  Object propsDict;
  GfxResources *next;
};

//------------------------------------------------------------------------
// GfxMarkedContent
//------------------------------------------------------------------------

enum GfxMarkedContentKind {
  gfxMCOptionalContent,
  gfxMCActualText,
  gfxMCOther
};

class GfxMarkedContent {
public:

  GfxMarkedContent(GfxMarkedContentKind kindA, GBool ocStateA) {
    kind = kindA;
    ocState = ocStateA;
  }
  ~GfxMarkedContent() {}

  GfxMarkedContentKind kind;
  GBool ocState;		// true if drawing is enabled, false if
				//   disabled
};

//------------------------------------------------------------------------
// Gfx
//------------------------------------------------------------------------

class Gfx {
public:

  // Constructor for regular output.
  Gfx(PDFDoc *docA, OutputDev *outA, int pageNum, Dict *resDict,
      double hDPI, double vDPI, PDFRectangle *box,
      PDFRectangle *cropBox, int rotate,
      GBool (*abortCheckCbkA)(void *data) = NULL,
      void *abortCheckCbkDataA = NULL);

  // Constructor for a sub-page object.
  Gfx(PDFDoc *docA, OutputDev *outA, Dict *resDict,
      PDFRectangle *box, PDFRectangle *cropBox,
      GBool (*abortCheckCbkA)(void *data) = NULL,
      void *abortCheckCbkDataA = NULL);

  ~Gfx();

  // Interpret a stream or array of streams.  <objRef> should be a
  // reference wherever possible (for loop-checking).
  void display(Object *objRef, GBool topLevel = gTrue);

  // Display an annotation, given its appearance (a Form XObject),
  // border style, and bounding box (in default user space).
  void drawAnnot(Object *strRef, AnnotBorderStyle *borderStyle,
		 double xMin, double yMin, double xMax, double yMax);

  // Save graphics state.
  void saveState();

  // Restore graphics state.
  void restoreState();

  // Get the current graphics state object.
  GfxState *getState() { return state; }

  void drawForm(Object *strRef, Dict *resDict, double *matrix, double *bbox,
		GBool transpGroup = gFalse, GBool softMask = gFalse,
		GfxColorSpace *blendingColorSpace = NULL,
		GBool isolated = gFalse, GBool knockout = gFalse,
		GBool alpha = gFalse, Function *transferFunc = NULL,
		GfxColor *backdropColor = NULL);

  // Take all of the content stream stack entries from <oldGfx>.  This
  // is useful when creating a new Gfx object to handle a pattern,
  // etc., where it's useful to check for loops that span both Gfx
  // objects.  This function should be called immediately after the
  // Gfx constructor, i.e., before processing any content streams with
  // the new Gfx object.
  void takeContentStreamStack(Gfx *oldGfx);

  // Clear the state stack and the marked content stack.
  void endOfPage();

private:

  PDFDoc *doc;
  XRef *xref;			// the xref table for this PDF file
  OutputDev *out;		// output device
  GBool subPage;		// is this a sub-page object?
  GBool printCommands;		// print the drawing commands (for debugging)
  GfxResources *res;		// resource stack
  int opCounter;		// operation counter (used to decide when
				//   to check for an abort)

  GfxState *state;		// current graphics state
  GBool fontChanged;		// set if font or text matrix has changed
  GfxClipType clip;		// do a clip?
  int ignoreUndef;		// current BX/EX nesting level
  double baseMatrix[6];		// default matrix for most recent
				//   page/form/pattern
  int formDepth;
  GBool ocState;		// true if drawing is enabled, false if
				//   disabled
  GList *markedContentStack;	// BMC/BDC/EMC stack [GfxMarkedContent]

  Parser *parser;		// parser for page content stream(s)
  GList *contentStreamStack;	// stack of open content streams, used
				//   for loop-checking

  GBool				// callback to check for an abort
    (*abortCheckCbk)(void *data);
  void *abortCheckCbkData;

  static Operator opTab[];	// table of operators

  GBool checkForContentStreamLoop(Object *ref);
  void go(GBool topLevel);
  GBool execOp(Object *cmd, Object args[], int numArgs);
  Operator *findOp(char *name);
  GBool checkArg(Object *arg, TchkType type);
  GFileOffset getPos();

  // graphics state operators
  void opSave(Object args[], int numArgs);
  void opRestore(Object args[], int numArgs);
  void opConcat(Object args[], int numArgs);
  void opSetDash(Object args[], int numArgs);
  void opSetFlat(Object args[], int numArgs);
  void opSetLineJoin(Object args[], int numArgs);
  void opSetLineCap(Object args[], int numArgs);
  void opSetMiterLimit(Object args[], int numArgs);
  void opSetLineWidth(Object args[], int numArgs);
  void opSetExtGState(Object args[], int numArgs);
  void doSoftMask(Object *str, Object *strRef, GBool alpha,
		  GfxColorSpace *blendingColorSpace,
		  GBool isolated, GBool knockout,
		  Function *transferFunc, GfxColor *backdropColor);
  void opSetRenderingIntent(Object args[], int numArgs);
  GfxRenderingIntent parseRenderingIntent(const char *name);

  // color operators
  void opSetFillGray(Object args[], int numArgs);
  void opSetStrokeGray(Object args[], int numArgs);
  void opSetFillCMYKColor(Object args[], int numArgs);
  void opSetStrokeCMYKColor(Object args[], int numArgs);
  void opSetFillRGBColor(Object args[], int numArgs);
  void opSetStrokeRGBColor(Object args[], int numArgs);
  void opSetFillColorSpace(Object args[], int numArgs);
  void opSetStrokeColorSpace(Object args[], int numArgs);
  void opSetFillColor(Object args[], int numArgs);
  void opSetStrokeColor(Object args[], int numArgs);
  void opSetFillColorN(Object args[], int numArgs);
  void opSetStrokeColorN(Object args[], int numArgs);

  // path segment operators
  void opMoveTo(Object args[], int numArgs);
  void opLineTo(Object args[], int numArgs);
  void opCurveTo(Object args[], int numArgs);
  void opCurveTo1(Object args[], int numArgs);
  void opCurveTo2(Object args[], int numArgs);
  void opRectangle(Object args[], int numArgs);
  void opClosePath(Object args[], int numArgs);

  // path painting operators
  void opEndPath(Object args[], int numArgs);
  void opStroke(Object args[], int numArgs);
  void opCloseStroke(Object args[], int numArgs);
  void opFill(Object args[], int numArgs);
  void opEOFill(Object args[], int numArgs);
  void opFillStroke(Object args[], int numArgs);
  void opCloseFillStroke(Object args[], int numArgs);
  void opEOFillStroke(Object args[], int numArgs);
  void opCloseEOFillStroke(Object args[], int numArgs);
  void doPatternFill(GBool eoFill);
  void doPatternStroke();
  void doPatternText();
  void doPatternImageMask(Object *ref, Stream *str, int width, int height,
			  GBool invert, GBool inlineImg, GBool interpolate);
  void doTilingPatternFill(GfxTilingPattern *tPat,
			   GBool stroke, GBool eoFill, GBool text);
  void doShadingPatternFill(GfxShadingPattern *sPat,
			    GBool stroke, GBool eoFill, GBool text);
  void opShFill(Object args[], int numArgs);
  void doFunctionShFill(GfxFunctionShading *shading);
  void doFunctionShFill1(GfxFunctionShading *shading,
			 double x0, double y0,
			 double x1, double y1,
			 GfxColor *colors, int depth);
  void doAxialShFill(GfxAxialShading *shading);
  void doRadialShFill(GfxRadialShading *shading);
  void doGouraudTriangleShFill(GfxGouraudTriangleShading *shading);
  void gouraudFillTriangle(double x0, double y0, double *color0,
			   double x1, double y1, double *color1,
			   double x2, double y2, double *color2,
			   GfxGouraudTriangleShading *shading, int depth);
  void doPatchMeshShFill(GfxPatchMeshShading *shading);
  void fillPatch(GfxPatch *patch, GfxPatchMeshShading *shading, int depth);
  void doEndPath();

  // path clipping operators
  void opClip(Object args[], int numArgs);
  void opEOClip(Object args[], int numArgs);

  // text object operators
  void opBeginText(Object args[], int numArgs);
  void opEndText(Object args[], int numArgs);

  // text state operators
  void opSetCharSpacing(Object args[], int numArgs);
  void opSetFont(Object args[], int numArgs);
  void doSetFont(GfxFont *font, double size);
  void opSetTextLeading(Object args[], int numArgs);
  void opSetTextRender(Object args[], int numArgs);
  void opSetTextRise(Object args[], int numArgs);
  void opSetWordSpacing(Object args[], int numArgs);
  void opSetHorizScaling(Object args[], int numArgs);

  // text positioning operators
  void opTextMove(Object args[], int numArgs);
  void opTextMoveSet(Object args[], int numArgs);
  void opSetTextMatrix(Object args[], int numArgs);
  void opTextNextLine(Object args[], int numArgs);

  // text string operators
  void opShowText(Object args[], int numArgs);
  void opMoveShowText(Object args[], int numArgs);
  void opMoveSetShowText(Object args[], int numArgs);
  void opShowSpaceText(Object args[], int numArgs);
  void doShowText(GString *s);
  void doIncCharCount(GString *s);

  // XObject operators
  void opXObject(Object args[], int numArgs);
  GBool doImage(Object *ref, Stream *str, GBool inlineImg);
  void doForm(Object *strRef, Object *str);

  // in-line image operators
  void opBeginImage(Object args[], int numArgs);
  Stream *buildImageStream(GBool *haveLength);
  void opImageData(Object args[], int numArgs);
  void opEndImage(Object args[], int numArgs);

  // type 3 font operators
  void opSetCharWidth(Object args[], int numArgs);
  void opSetCacheDevice(Object args[], int numArgs);

  // compatibility operators
  void opBeginIgnoreUndef(Object args[], int numArgs);
  void opEndIgnoreUndef(Object args[], int numArgs);

  // marked content operators
  void opBeginMarkedContent(Object args[], int numArgs);
  void opEndMarkedContent(Object args[], int numArgs);
  void opMarkPoint(Object args[], int numArgs);

  GfxState *saveStateStack();
  void restoreStateStack(GfxState *oldState);
  void pushResources(Dict *resDict);
  void popResources();
};

#endif
