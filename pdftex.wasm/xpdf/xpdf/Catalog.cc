//========================================================================
//
// Catalog.cc
//
// Copyright 1996-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <stddef.h>
#include <limits.h>
#include "gmem.h"
#include "gmempp.h"
#include "gfile.h"
#include "GList.h"
#include "Object.h"
#include "CharTypes.h"
#include "PDFDoc.h"
#include "XRef.h"
#include "Array.h"
#include "Dict.h"
#include "Page.h"
#include "Error.h"
#include "Link.h"
#include "Form.h"
#include "TextString.h"
#include "Catalog.h"

//------------------------------------------------------------------------
// PageTreeNode
//------------------------------------------------------------------------

class PageTreeNode {
public:

  PageTreeNode(Ref refA, int countA, PageTreeNode *parentA);
  ~PageTreeNode();

  Ref ref;
  int count;
  PageTreeNode *parent;
  GList *kids;			// [PageTreeNode]
  PageAttrs *attrs;
};

PageTreeNode::PageTreeNode(Ref refA, int countA, PageTreeNode *parentA) {
  ref = refA;
  count = countA;
  parent = parentA;
  kids = NULL;
  attrs = NULL;
}

PageTreeNode::~PageTreeNode() {
  delete attrs;
  if (kids) {
    deleteGList(kids, PageTreeNode);
  }
}

//------------------------------------------------------------------------
// EmbeddedFile
//------------------------------------------------------------------------

class EmbeddedFile {
public:

  EmbeddedFile(TextString *nameA, Object *streamRefA);
  ~EmbeddedFile();

  TextString *name;
  Object streamRef;
};

EmbeddedFile::EmbeddedFile(TextString *nameA, Object *streamRefA) {
  name = nameA;
  streamRefA->copy(&streamRef);
}

EmbeddedFile::~EmbeddedFile() {
  delete name;
  streamRef.free();
}

//------------------------------------------------------------------------
// PageLabelNode
//------------------------------------------------------------------------

class PageLabelNode {
public:

  PageLabelNode(int firstPageA, Dict *dict);
  ~PageLabelNode();

  int firstPage;		// first page number covered by this node
  int lastPage;			// last page number covered by this node
  TextString *prefix;		// label prefix (may be empty)
  int start;			// value of the numeric portion of this
				//   label for the first page in the range
  char style;			// page label style
};

PageLabelNode::PageLabelNode(int firstPageA, Dict *dict) {
  Object prefixObj, styleObj, startObj;

  // convert page index to page number
  firstPage = firstPageA + 1;

  // lastPage will be filled in later
  lastPage = -1;

  if (dict->lookup("P", &prefixObj)->isString()) {
    prefix = new TextString(prefixObj.getString());
  } else {
    prefix = new TextString();
  }
  prefixObj.free();

  style = '\0';
  if (dict->lookup("S", &styleObj)->isName()) {
    if (strlen(styleObj.getName()) == 1) {
      style = styleObj.getName()[0];
    }
  }
  styleObj.free();

  start = 1;
  if (dict->lookup("St", &startObj)->isInt()) {
    start = startObj.getInt();
  }
  startObj.free();
}

PageLabelNode::~PageLabelNode() {
  delete prefix;
}

//------------------------------------------------------------------------
// Catalog
//------------------------------------------------------------------------

Catalog::Catalog(PDFDoc *docA) {
  Object catDict;
  Object obj, obj2;

  ok = gTrue;
  doc = docA;
  xref = doc->getXRef();
  pageTree = NULL;
  pages = NULL;
  pageRefs = NULL;
  numPages = 0;
  baseURI = NULL;
  form = NULL;
  embeddedFiles = NULL;
#if MULTITHREADED
  gInitMutex(&pageMutex);
#endif

  xref->getCatalog(&catDict);
  if (!catDict.isDict()) {
    error(errSyntaxError, -1, "Catalog object is wrong type ({0:s})",
	  catDict.getTypeName());
    goto err1;
  }

  // read page tree
  if (!readPageTree(&catDict)) {
    goto err1;
  }

  // read named destination dictionary
  catDict.dictLookup("Dests", &dests);

  // read root of named destination tree
  if (catDict.dictLookup("Names", &obj)->isDict())
    obj.dictLookup("Dests", &nameTree);
  else
    nameTree.initNull();
  obj.free();

  // read base URI
  if (catDict.dictLookup("URI", &obj)->isDict()) {
    if (obj.dictLookup("Base", &obj2)->isString()) {
      baseURI = obj2.getString()->copy();
    }
    obj2.free();
  }
  obj.free();
  if (!baseURI || baseURI->getLength() == 0) {
    if (baseURI) {
      delete baseURI;
    }
    if (doc->getFileName()) {
      baseURI = makePathAbsolute(grabPath(doc->getFileName()->getCString()));
      if (baseURI->getChar(0) == '/') {
	baseURI->insert(0, "file://localhost");
      } else {
	baseURI->insert(0, "file://localhost/");
      }
    } else {
      baseURI = new GString("file://localhost/");
    }
  }

  // get the metadata stream
  catDict.dictLookup("Metadata", &metadata);

  // get the structure tree root
  catDict.dictLookup("StructTreeRoot", &structTreeRoot);

  // get the outline dictionary
  catDict.dictLookup("Outlines", &outline);

  // get the AcroForm dictionary
  catDict.dictLookup("AcroForm", &acroForm);

  // get the NeedsRendering flag
  // NB: Form::load() uses this value
  needsRendering = catDict.dictLookup("NeedsRendering", &obj)->isBool() &&
                   obj.getBool();
  obj.free();

  // create the Form
  // (if acroForm is a null object, this will still create an AcroForm
  // if there are unattached Widget-type annots)
  form = Form::load(doc, this, &acroForm);

  // get the OCProperties dictionary
  catDict.dictLookup("OCProperties", &ocProperties);

  // get the list of embedded files
  readEmbeddedFileList(catDict.getDict());

  // get the ViewerPreferences object
  catDict.dictLookupNF("ViewerPreferences", &viewerPrefs);

  pageLabels = NULL;
  if (catDict.dictLookup("PageLabels", &obj)->isDict()) {
    readPageLabelTree(&obj);
  }
  obj.free();

  catDict.free();
  return;

 err1:
  catDict.free();
  dests.initNull();
  nameTree.initNull();
  ok = gFalse;
}

Catalog::~Catalog() {
  int i;

  if (pageTree) {
    delete pageTree;
  }
  if (pages) {
    for (i = 0; i < numPages; ++i) {
      if (pages[i]) {
	delete pages[i];
      }
    }
    gfree(pages);
    gfree(pageRefs);
  }
#if MULTITHREADED
  gDestroyMutex(&pageMutex);
#endif
  dests.free();
  nameTree.free();
  if (baseURI) {
    delete baseURI;
  }
  metadata.free();
  structTreeRoot.free();
  outline.free();
  acroForm.free();
  if (form) {
    delete form;
  }
  ocProperties.free();
  if (embeddedFiles) {
    deleteGList(embeddedFiles, EmbeddedFile);
  }
  if (pageLabels) {
    deleteGList(pageLabels, PageLabelNode);
  }
  viewerPrefs.free();
}

Page *Catalog::getPage(int i) {
  Page *page;

#if MULTITHREADED
  gLockMutex(&pageMutex);
#endif
  if (!pages[i-1]) {
    loadPage(i);
  }
  page = pages[i-1];
#if MULTITHREADED
  gUnlockMutex(&pageMutex);
#endif
  return page;
}

Ref *Catalog::getPageRef(int i) {
  Ref *pageRef;

#if MULTITHREADED
  gLockMutex(&pageMutex);
#endif
  if (!pages[i-1]) {
    loadPage(i);
  }
  pageRef = &pageRefs[i-1];
#if MULTITHREADED
  gUnlockMutex(&pageMutex);
#endif
  return pageRef;
}

void Catalog::doneWithPage(int i) {
#if MULTITHREADED
  gLockMutex(&pageMutex);
#endif
  if (pages[i-1]) {
    delete pages[i-1];
    pages[i-1] = NULL;
  }
#if MULTITHREADED
  gUnlockMutex(&pageMutex);
#endif
}

GString *Catalog::readMetadata() {
  GString *s;
  Dict *dict;
  Object obj;
  char buf[4096];
  int n;

  if (!metadata.isStream()) {
    return NULL;
  }
  dict = metadata.streamGetDict();
  if (!dict->lookup("Subtype", &obj)->isName("XML")) {
    error(errSyntaxWarning, -1, "Unknown Metadata type: '{0:s}'",
	  obj.isName() ? obj.getName() : "???");
  }
  obj.free();
  s = new GString();
  metadata.streamReset();
  while ((n = metadata.streamGetBlock(buf, sizeof(buf))) > 0) {
    s->append(buf, n);
  }
  metadata.streamClose();
  return s;
}

int Catalog::findPage(int num, int gen) {
  int i;

#if MULTITHREADED
  gLockMutex(&pageMutex);
#endif
  for (i = 0; i < numPages; ++i) {
    if (!pages[i]) {
      loadPage(i+1);
    }
    if (pageRefs[i].num == num && pageRefs[i].gen == gen) {
#if MULTITHREADED
      gUnlockMutex(&pageMutex);
#endif
      return i + 1;
    }
  }
#if MULTITHREADED
  gUnlockMutex(&pageMutex);
#endif
  return 0;
}

LinkDest *Catalog::findDest(GString *name) {
  LinkDest *dest;
  Object obj1, obj2;
  GBool found;

  // try named destination dictionary then name tree
  found = gFalse;
  if (dests.isDict()) {
    if (!dests.dictLookup(name->getCString(), &obj1)->isNull()) {
      found = gTrue;
    } else {
      obj1.free();
    }
  }
  if (!found && nameTree.isDict()) {
    if (!findDestInTree(&nameTree, name, &obj1)->isNull()) {
      found = gTrue;
    } else {
      obj1.free();
    }
  }
  if (!found) {
    return NULL;
  }

  // construct LinkDest
  dest = NULL;
  if (obj1.isArray()) {
    dest = new LinkDest(obj1.getArray());
  } else if (obj1.isDict()) {
    if (obj1.dictLookup("D", &obj2)->isArray()) {
      dest = new LinkDest(obj2.getArray());
    } else {
      error(errSyntaxWarning, -1, "Bad named destination value");
    }
    obj2.free();
  } else {
    error(errSyntaxWarning, -1, "Bad named destination value");
  }
  obj1.free();
  if (dest && !dest->isOk()) {
    delete dest;
    dest = NULL;
  }

  return dest;
}

Object *Catalog::findDestInTree(Object *tree, GString *name, Object *obj) {
  Object names, name1;
  Object kids, kid, limits, low, high;
  GBool done, found;
  int cmp, i;

  // leaf node
  if (tree->dictLookup("Names", &names)->isArray()) {
    done = found = gFalse;
    for (i = 0; !done && i < names.arrayGetLength(); i += 2) {
      if (names.arrayGet(i, &name1)->isString()) {
	cmp = name->cmp(name1.getString());
	if (cmp == 0) {
	  names.arrayGet(i+1, obj);
	  found = gTrue;
	  done = gTrue;
	} else if (cmp < 0) {
	  done = gTrue;
	}
      }
      name1.free();
    }
    names.free();
    if (!found) {
      obj->initNull();
    }
    return obj;
  }
  names.free();

  // root or intermediate node
  done = gFalse;
  if (tree->dictLookup("Kids", &kids)->isArray()) {
    for (i = 0; !done && i < kids.arrayGetLength(); ++i) {
      if (kids.arrayGet(i, &kid)->isDict()) {
	if (kid.dictLookup("Limits", &limits)->isArray()) {
	  if (limits.arrayGet(0, &low)->isString() &&
	      name->cmp(low.getString()) >= 0) {
	    if (limits.arrayGet(1, &high)->isString() &&
		name->cmp(high.getString()) <= 0) {
	      findDestInTree(&kid, name, obj);
	      done = gTrue;
	    }
	    high.free();
	  }
	  low.free();
	}
	limits.free();
      }
      kid.free();
    }
  }
  kids.free();

  // name was outside of ranges of all kids
  if (!done) {
    obj->initNull();
  }

  return obj;
}

GBool Catalog::readPageTree(Object *catDict) {
  Object topPagesRef, topPagesObj, countObj;
  int i;

  if (!catDict->dictLookupNF("Pages", &topPagesRef)->isRef()) {
    error(errSyntaxError, -1, "Top-level pages reference is wrong type ({0:s})",
	  topPagesRef.getTypeName());
    topPagesRef.free();
    return gFalse;
  }
  if (!topPagesRef.fetch(xref, &topPagesObj)->isDict()) {
    error(errSyntaxError, -1, "Top-level pages object is wrong type ({0:s})",
	  topPagesObj.getTypeName());
    topPagesObj.free();
    topPagesRef.free();
    return gFalse;
  }
  if (topPagesObj.dictLookup("Count", &countObj)->isInt()) {
    numPages = countObj.getInt();
    if (numPages == 0 || numPages > 50000) {
      // 1. Acrobat apparently scans the page tree if it sees a zero
      //    count.
      // 2. Absurdly large page counts result in very slow loading,
      //    because other code tries to fetch pages 1 through n.
      // In both cases: ignore the given page count and scan the tree
      // instead.
      numPages = countPageTree(&topPagesObj);
    }
  } else {
    // assume we got a Page node instead of a Pages node
    numPages = 1;
  }
  countObj.free();
  if (numPages < 0) {
    error(errSyntaxError, -1, "Invalid page count");
    topPagesObj.free();
    topPagesRef.free();
    numPages = 0;
    return gFalse;
  }
  pageTree = new PageTreeNode(topPagesRef.getRef(), numPages, NULL);
  topPagesObj.free();
  topPagesRef.free();
  pages = (Page **)greallocn(pages, numPages, sizeof(Page *));
  pageRefs = (Ref *)greallocn(pageRefs, numPages, sizeof(Ref));
  for (i = 0; i < numPages; ++i) {
    pages[i] = NULL;
    pageRefs[i].num = -1;
    pageRefs[i].gen = -1;
  }
  return gTrue;
}

int Catalog::countPageTree(Object *pagesObj) {
  Object kids, kid;
  int n, n2, i;

  if (!pagesObj->isDict()) {
    return 0;
  }
  if (pagesObj->dictLookup("Kids", &kids)->isArray()) {
    n = 0;
    for (i = 0; i < kids.arrayGetLength(); ++i) {
      kids.arrayGet(i, &kid);
      n2 = countPageTree(&kid);
      if (n2 < INT_MAX - n) {
	n += n2;
      } else {
	error(errSyntaxError, -1, "Page tree contains too many pages");
	n = INT_MAX;
      }
      kid.free();
    }
  } else {
    n = 1;
  }
  kids.free();
  return n;
}

void Catalog::loadPage(int pg) {
  loadPage2(pg, pg - 1, pageTree);
}

void Catalog::loadPage2(int pg, int relPg, PageTreeNode *node) {
  Object pageRefObj, pageObj, kidsObj, kidRefObj, kidObj, countObj;
  PageTreeNode *kidNode, *p;
  PageAttrs *attrs;
  int count, i;

  if (relPg >= node->count) {
    error(errSyntaxError, -1, "Internal error in page tree");
    pages[pg-1] = new Page(doc, pg);
    return;
  }

  // if this node has not been filled in yet, it's either a leaf node
  // or an unread internal node
  if (!node->kids) {

    // check for a loop in the page tree
    for (p = node->parent; p; p = p->parent) {
      if (node->ref.num == p->ref.num && node->ref.gen == p->ref.gen) {
	error(errSyntaxError, -1, "Loop in Pages tree");
	pages[pg-1] = new Page(doc, pg);
	return;
      }
    }

    // fetch the Page/Pages object
    pageRefObj.initRef(node->ref.num, node->ref.gen);
    if (!pageRefObj.fetch(xref, &pageObj)->isDict()) {
      error(errSyntaxError, -1, "Page tree object is wrong type ({0:s})",
	    pageObj.getTypeName());
      pageObj.free();
      pageRefObj.free();
      pages[pg-1] = new Page(doc, pg);
      return;
    }

    // merge the PageAttrs
    attrs = new PageAttrs(node->parent ? node->parent->attrs
			               : (PageAttrs *)NULL,
			  pageObj.getDict());

    // if "Kids" exists, it's an internal node
    if (pageObj.dictLookup("Kids", &kidsObj)->isArray()) {

      // save the PageAttrs
      node->attrs = attrs;

      // read the kids
      node->kids = new GList();
      for (i = 0; i < kidsObj.arrayGetLength(); ++i) {
	if (kidsObj.arrayGetNF(i, &kidRefObj)->isRef()) {
	  if (kidRefObj.fetch(xref, &kidObj)->isDict()) {
	    if (kidObj.dictLookup("Count", &countObj)->isInt()) {
	      count = countObj.getInt();
	    } else {
	      count = 1;
	    }
	    countObj.free();
	    node->kids->append(new PageTreeNode(kidRefObj.getRef(), count,
						node));
	  } else {
	    error(errSyntaxError, -1, "Page tree object is wrong type ({0:s})",
		  kidObj.getTypeName());
	  }
	  kidObj.free();
	} else {
	  error(errSyntaxError, -1,
		"Page tree reference is wrong type ({0:s})",
		kidRefObj.getTypeName());
	}
	kidRefObj.free();
      }

    } else {
      
      // create the Page object
      pageRefs[pg-1] = node->ref;
      pages[pg-1] = new Page(doc, pg, pageObj.getDict(), attrs);
      if (!pages[pg-1]->isOk()) {
	delete pages[pg-1];
	pages[pg-1] = new Page(doc, pg);
      }

    }

    kidsObj.free();
    pageObj.free();
    pageRefObj.free();
  }

  // recursively descend the tree
  if (node->kids) {
    for (i = 0; i < node->kids->getLength(); ++i) {
      kidNode = (PageTreeNode *)node->kids->get(i);
      if (relPg < kidNode->count) {
	loadPage2(pg, relPg, kidNode);
	break;
      }
      relPg -= kidNode->count;
    }

    // this will only happen if the page tree is invalid
    // (i.e., parent count > sum of children counts)
    if (i == node->kids->getLength()) {
      error(errSyntaxError, -1, "Invalid page count in page tree");
      pages[pg-1] = new Page(doc, pg);
    }
  }
}

Object *Catalog::getDestOutputProfile(Object *destOutProf) {
  Object catDict, intents, intent, subtype;
  int i;

  if (!xref->getCatalog(&catDict)->isDict()) {
    goto err1;
  }
  if (!catDict.dictLookup("OutputIntents", &intents)->isArray()) {
    goto err2;
  }
  for (i = 0; i < intents.arrayGetLength(); ++i) {
    intents.arrayGet(i, &intent);
    if (!intent.isDict()) {
      intent.free();
      continue;
    }
    if (!intent.dictLookup("S", &subtype)->isName("GTS_PDFX")) {
      subtype.free();
      intent.free();
      continue;
    }
    subtype.free();
    if (!intent.dictLookup("DestOutputProfile", destOutProf)->isStream()) {
      destOutProf->free();
      intent.free();
      goto err2;
    }
    intent.free();
    intents.free();
    catDict.free();
    return destOutProf;
  }

 err2:
  intents.free();
 err1:
  catDict.free();
  return NULL;
}

void Catalog::readEmbeddedFileList(Dict *catDict) {
  Object obj1, obj2;
  char *touchedObjs;

  // read the embedded file name tree
  if (catDict->lookup("Names", &obj1)->isDict()) {
    if (obj1.dictLookup("EmbeddedFiles", &obj2)->isDict()) {
      readEmbeddedFileTree(&obj2);
    }
    obj2.free();
  }
  obj1.free();

  // look for file attachment annotations
  touchedObjs = (char *)gmalloc(xref->getNumObjects());
  memset(touchedObjs, 0, xref->getNumObjects());
  readFileAttachmentAnnots(catDict->lookupNF("Pages", &obj1), touchedObjs);
  obj1.free();
  gfree(touchedObjs);
}

void Catalog::readEmbeddedFileTree(Object *node) {
  Object kidsObj, kidObj;
  Object namesObj, nameObj, fileSpecObj;
  int i;

  if (node->dictLookup("Kids", &kidsObj)->isArray()) {
    for (i = 0; i < kidsObj.arrayGetLength(); ++i) {
      if (kidsObj.arrayGet(i, &kidObj)->isDict()) {
	readEmbeddedFileTree(&kidObj);
      }
      kidObj.free();
    }
  } else {
    if (node->dictLookup("Names", &namesObj)->isArray()) {
      for (i = 0; i+1 < namesObj.arrayGetLength(); ++i) {
	namesObj.arrayGet(i, &nameObj);
	namesObj.arrayGet(i+1, &fileSpecObj);
	readEmbeddedFile(&fileSpecObj, &nameObj);
	nameObj.free();
	fileSpecObj.free();
      }
    }
    namesObj.free();
  }
  kidsObj.free();
}

void Catalog::readFileAttachmentAnnots(Object *pageNodeRef,
				       char *touchedObjs) {
  Object pageNode, kids, kid, annots, annot, subtype, fileSpec, contents;
  int i;

  // check for an invalid object reference (e.g., in a damaged PDF file)
  if (pageNodeRef->getRefNum() < 0 ||
      pageNodeRef->getRefNum() >= xref->getNumObjects()) {
    return;
  }

  // check for a page tree loop
  if (pageNodeRef->isRef()) {
    if (touchedObjs[pageNodeRef->getRefNum()]) {
      return;
    }
    touchedObjs[pageNodeRef->getRefNum()] = 1;
    xref->fetch(pageNodeRef->getRefNum(), pageNodeRef->getRefGen(), &pageNode);
  } else {
    pageNodeRef->copy(&pageNode);
  }

  if (pageNode.isDict()) {
    if (pageNode.dictLookup("Kids", &kids)->isArray()) {
      for (i = 0; i < kids.arrayGetLength(); ++i) {
	readFileAttachmentAnnots(kids.arrayGetNF(i, &kid), touchedObjs);
	kid.free();
      }
    } else {
      if (pageNode.dictLookup("Annots", &annots)->isArray()) {
	for (i = 0; i < annots.arrayGetLength(); ++i) {
	  if (annots.arrayGet(i, &annot)->isDict()) {
	    if (annot.dictLookup("Subtype", &subtype)
		  ->isName("FileAttachment")) {
	      if (annot.dictLookup("FS", &fileSpec)) {
		readEmbeddedFile(&fileSpec,
				 annot.dictLookup("Contents", &contents));
		contents.free();
	      }
	      fileSpec.free();
	    }
	    subtype.free();
	  }
	  annot.free();
	}
      }
      annots.free();
    }
    kids.free();
  }

  pageNode.free();
}

void Catalog::readEmbeddedFile(Object *fileSpec, Object *name1) {
  Object name2, efObj, streamObj;
  GString *s;
  TextString *name;

  if (fileSpec->isDict()) {
    if (fileSpec->dictLookup("UF", &name2)->isString()) {
      name = new TextString(name2.getString());
    } else {
      name2.free();
      if (fileSpec->dictLookup("F", &name2)->isString()) {
	name = new TextString(name2.getString());
      } else if (name1 && name1->isString()) {
	name = new TextString(name1->getString());
      } else {
	s = new GString("?");
	name = new TextString(s);
	delete s;
      }
    }
    name2.free();
    if (fileSpec->dictLookup("EF", &efObj)->isDict()) {
      if (efObj.dictLookupNF("F", &streamObj)->isRef()) {
	if (!embeddedFiles) {
	  embeddedFiles = new GList();
	}
	embeddedFiles->append(new EmbeddedFile(name, &streamObj));
      } else {
	delete name;
      }
      streamObj.free();
    } else {
      delete name;
    }
    efObj.free();
  }
}

int Catalog::getNumEmbeddedFiles() {
  return embeddedFiles ? embeddedFiles->getLength() : 0;
}

Unicode *Catalog::getEmbeddedFileName(int idx) {
  return ((EmbeddedFile *)embeddedFiles->get(idx))->name->getUnicode();
}

int Catalog::getEmbeddedFileNameLength(int idx) {
  return ((EmbeddedFile *)embeddedFiles->get(idx))->name->getLength();
}

Object *Catalog::getEmbeddedFileStreamRef(int idx) {
  return &((EmbeddedFile *)embeddedFiles->get(idx))->streamRef;
}

Object *Catalog::getEmbeddedFileStreamObj(int idx, Object *strObj) {
  ((EmbeddedFile *)embeddedFiles->get(idx))->streamRef.fetch(xref, strObj);
  if (!strObj->isStream()) {
    strObj->free();
    return NULL;
  }
  return strObj;
}

void Catalog::readPageLabelTree(Object *root) {
  PageLabelNode *label0, *label1;
  int i;

  pageLabels = new GList();
  readPageLabelTree2(root);

  if (pageLabels->getLength() == 0) {
    deleteGList(pageLabels, PageLabelNode);
    pageLabels = NULL;
    return;
  }

  // set lastPage in each node
  label0 = (PageLabelNode *)pageLabels->get(0);
  for (i = 1; i < pageLabels->getLength(); ++i) {
    label1 = (PageLabelNode *)pageLabels->get(i);
    label0->lastPage = label1->firstPage - 1;
    label0 = label1;
  }
  label0->lastPage = numPages;
}

void Catalog::readPageLabelTree2(Object *node) {
  Object nums, num, labelObj, kids, kid;
  int i;

  if (!node->isDict()) {
    return;
  }

  if (node->dictLookup("Nums", &nums)->isArray()) {
    for (i = 0; i < nums.arrayGetLength() - 1; i += 2) {
      if (nums.arrayGet(i, &num)->isInt()) {
	if (nums.arrayGet(i+1, &labelObj)->isDict()) {
	  pageLabels->append(new PageLabelNode(num.getInt(),
					       labelObj.getDict()));
	}
	labelObj.free();
      }
      num.free();
    }
  }
  nums.free();

  if (node->dictLookup("Kids", &kids)->isArray()) {
    for (i = 0; i < kids.arrayGetLength(); ++i) {
      kids.arrayGet(i, &kid);
      readPageLabelTree2(&kid);
      kid.free();
    }
  }
  kids.free();
}

TextString *Catalog::getPageLabel(int pageNum) {
  PageLabelNode *label;
  TextString *ts;
  int pageRangeNum;
  GString *suffix;

  if (!pageLabels || !(label = findPageLabel(pageNum))) {
    return NULL;
  }

  ts = new TextString(label->prefix);

  pageRangeNum = label->start + (pageNum - label->firstPage);

  suffix = NULL;
  if (label->style == 'D') {
    suffix = GString::format("{0:d}", pageRangeNum);
  } else if (label->style == 'R') {
    suffix = makeRomanNumeral(pageRangeNum, gTrue);
  } else if (label->style == 'r') {
    suffix = makeRomanNumeral(pageRangeNum, gFalse);
  } else if (label->style == 'A') {
    suffix = makeLetterLabel(pageRangeNum, gTrue);
  } else if (label->style == 'a') {
    suffix = makeLetterLabel(pageRangeNum, gFalse);
  }
  if (suffix) {
    ts->append(suffix);
    delete suffix;
  }

  return ts;
}

PageLabelNode *Catalog::findPageLabel(int pageNum) {
  PageLabelNode *label;
  int i;

  //~ this could use a binary search
  for (i = 0; i < pageLabels->getLength(); ++i) {
    label = (PageLabelNode *)pageLabels->get(i);
    if (pageNum >= label->firstPage && pageNum <= label->lastPage) {
      return label;
    }
  }
  return NULL;
}

GString *Catalog::makeRomanNumeral(int num, GBool uppercase) {
  GString *s;

  s = new GString();
  while (num >= 1000) {
    s->append(uppercase ? 'M' : 'm');
    num -= 1000;
  }
  if (num >= 900) {
    s->append(uppercase ? "CM" : "cm");
    num -= 900;
  } else if (num >= 500) {
    s->append(uppercase ? 'D' : 'd');
    num -= 500;
  } else if (num >= 400) {
    s->append(uppercase ? "CD" : "cd");
    num -= 400;
  }
  while (num >= 100) {
    s->append(uppercase ? 'C' : 'c');
    num -= 100;
  }
  if (num >= 90) {
    s->append(uppercase ? "XC" : "xc");
    num -= 90;
  } else if (num >= 50) {
    s->append(uppercase ? 'L' : 'l');
    num -= 50;
  } else if (num >= 40) {
    s->append(uppercase ? "XL" : "xl");
    num -= 40;
  }
  while (num >= 10) {
    s->append(uppercase ? 'X' : 'x');
    num -= 10;
  }
  if (num >= 9) {
    s->append(uppercase ? "IX" : "ix");
    num -= 9;
  } else if (num >= 5) {
    s->append(uppercase ? 'V' : 'v');
    num -= 5;
  } else if (num >= 4) {
    s->append(uppercase ? "IV" : "iv");
    num -= 4;
  }
  while (num >= 1) {
    s->append(uppercase ? 'I' : 'i');
    num -= 1;
  }
  return s;
}

GString *Catalog::makeLetterLabel(int num, GBool uppercase) {
  GString *s;
  int m, n, i;

  m = (num - 1) / 26 + 1;
  n = (num - 1) % 26;
  s = new GString();
  for (i = 0; i < m; ++i) {
    s->append((char)((uppercase ? 'A' : 'a') + n));
  }
  return s;
}

int Catalog::getPageNumFromPageLabel(TextString *pageLabel) {
  PageLabelNode *label;
  int pageNum, prefixLength, i, n;

  if (!pageLabels) {
    return -1;
  }
  for (i = 0; i < pageLabels->getLength(); ++i) {
    label = (PageLabelNode *)pageLabels->get(i);
    prefixLength = label->prefix->getLength();
    if (pageLabel->getLength() < prefixLength ||
	memcmp(pageLabel->getUnicode(), label->prefix->getUnicode(),
	       prefixLength * sizeof(Unicode))) {
      continue;
    }
    if (label->style == '\0' && pageLabel->getLength() == prefixLength) {
      return label->firstPage;
    }
    if (!convertPageLabelToInt(pageLabel, prefixLength, label->style, &n)) {
      continue;
    }
    if (n < label->start) {
      continue;
    }
    pageNum = label->firstPage + n - label->start;
    if (pageNum <= label->lastPage) {
      return pageNum;
    }
  }
  return -1;
}

// Attempts to convert pageLabel[prefixLength .. end] to an integer,
// following the specified page label style.  If successful, sets *n
// and returns true; else returns false.
GBool Catalog::convertPageLabelToInt(TextString *pageLabel, int prefixLength,
				     char style, int *n) {
  Unicode *u;
  Unicode delta;
  int len, i;

  len = pageLabel->getLength();
  if (len <= prefixLength) {
    return gFalse;
  }
  u = pageLabel->getUnicode();
  if (style == 'D') {
    *n = 0;
    for (i = prefixLength; i < len; ++i) {
      if (u[i] < (Unicode)'0' || u[i] > (Unicode)'9') {
	return gFalse;
      }
      *n = *n * 10 + (u[i] - (Unicode)'0');
    }
    return gTrue;
  } else if (style == 'R' || style == 'r') {
    delta = style - 'R';
    *n = 0;
    i = prefixLength;
    while (i < len && u[i] == (Unicode)'M' + delta) {
      *n += 1000;
      ++i;
    }
    if (i+1 < len && u[i] == (Unicode)'C' + delta &&
	u[i+1] == (Unicode)'M' + delta) {
      *n += 900;
      i += 2;
    } else if (i < len && u[i] == (Unicode)'D' + delta) {
      *n += 500;
      ++i;
    } else if (i+1 < len && u[i] == (Unicode)'C' + delta &&
	       u[i+1] == (Unicode)'D' + delta) {
      *n += 400;
      i += 2;
    }
    while (i < len && u[i] == (Unicode)'C' + delta) {
      *n += 100;
      ++i;
    }
    if (i+1 < len && u[i] == (Unicode)'X' + delta &&
	u[i+1] == (Unicode)'C' + delta) {
      *n += 90;
      i += 2;
    } else if (i < len && u[i] == (Unicode)'L' + delta) {
      *n += 50;
      ++i;
    } else if (i+1 < len && u[i] == (Unicode)'X' + delta &&
	       u[i+1] == (Unicode)'L' + delta) {
      *n += 40;
      i += 2;
    }
    while (i < len && u[i] == (Unicode)'X' + delta) {
      *n += 10;
      ++i;
    }
    if (i+1 < len && u[i] == (Unicode)'I' + delta &&
	u[i+1] == (Unicode)'X' + delta) {
      *n += 9;
      i += 2;
    } else if (i < len && u[i] == (Unicode)'V' + delta) {
      *n += 5;
      ++i;
    } else if (i+1 < len && u[i] == (Unicode)'I' + delta &&
	       u[i+1] == (Unicode)'V' + delta) {
      *n += 4;
      i += 2;
    }
    while (i < len && u[i] == (Unicode)'I' + delta) {
      *n += 1;
      ++i;
    }
    return i == len;
  } else if (style == 'A' || style == 'a') {
    if (u[prefixLength] < (Unicode)style ||
	u[prefixLength] > (Unicode)style + 25) {
      return gFalse;
    }
    for (i = prefixLength + 1; i < len; ++i) {
      if (u[i] != u[prefixLength]) {
	return gFalse;
      }
    }
    *n = (len - prefixLength - 1) * 26 + (u[i] - (Unicode)style) + 1;
    return gTrue;
  }
  return gFalse;
}
