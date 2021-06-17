//========================================================================
//
// Zoox.cc
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "GHash.h"
#include "Zoox.h"

//~ all of this code assumes the encoding is UTF-8 or ASCII or something
//~   similar (ISO-8859-*)

//------------------------------------------------------------------------

static char nameStartChar[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, // 30
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 50
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 70
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 80
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 90
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // a0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // b0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // c0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // d0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // e0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // f0
};

static char nameChar[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, // 20
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 30
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 50
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 70
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 80
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 90
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // a0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // b0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // c0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // d0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // e0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // f0
};

//------------------------------------------------------------------------

ZxNode::ZxNode() {
  next = NULL;
  parent = NULL;
  firstChild = lastChild = NULL;
}

ZxNode::~ZxNode() {
  ZxNode *child;

  while (firstChild) {
    child = firstChild;
    firstChild = firstChild->next;
    delete child;
  }
}

ZxNode *ZxNode::deleteChild(ZxNode *child) {
  ZxNode *p1, *p2;

  for (p1 = NULL, p2 = firstChild;
       p2 && p2 != child;
       p1 = p2, p2 = p2->next) ;
  if (!p2) {
    return NULL;
  }
  if (p1) {
    p1->next = child->next;
  } else {
    firstChild = child->next;
  }
  child->parent = NULL;
  child->next = NULL;
  return child;
}

void ZxNode::appendChild(ZxNode *child) {
  ZxNode *p1;

  if (child->parent || child->next) {
    return;
  }
  if (firstChild) {
    for (p1 = firstChild; p1->next; p1 = p1->next) ;
    p1->next = child;
  } else {
    firstChild = child;
  }
  child->parent = this;
}

void ZxNode::insertChildAfter(ZxNode *child, ZxNode *prev) {
  if (child->parent || child->next || (prev && prev->parent != this)) {
    return;
  }
  if (prev) {
    child->next = prev->next;
    prev->next = child;
  } else {
    child->next = firstChild;
    firstChild = child;
  }
  child->parent = this;
}

ZxElement *ZxNode::findFirstElement(const char *type) {
  ZxNode *child;
  ZxElement *result;

  if (isElement(type)) {
    return (ZxElement *)this;
  }
  for (child = firstChild; child; child = child->next) {
    if ((result = child->findFirstElement(type))) {
      return result;
    }
  }
  return NULL;
}

ZxElement *ZxNode::findFirstChildElement(const char *type) {
  ZxNode *child;

  for (child = firstChild; child; child = child->next) {
    if (child->isElement(type)) {
      return (ZxElement *)child;
    }
  }
  return NULL;
}

GList *ZxNode::findAllElements(const char *type) {
  GList *results;

  results = new GList();
  findAllElements(type, results);
  return results;
}

void ZxNode::findAllElements(const char *type, GList *results) {
  ZxNode *child;

  if (isElement(type)) {
    results->append(this);
  }
  for (child = firstChild; child; child = child->next) {
    child->findAllElements(type, results);
  }
}

GList *ZxNode::findAllChildElements(const char *type) {
  GList *results;
  ZxNode *child;

  results = new GList();
  for (child = firstChild; child; child = child->next) {
    if (child->isElement(type)) {
      results->append(child);
    }
  }
  return results;
}

void ZxNode::addChild(ZxNode *child) {
  if (lastChild) {
    lastChild->next = child;
    lastChild = child;
  } else {
    firstChild = lastChild = child;
  }
  child->parent = this;
  child->next = NULL;
}

//------------------------------------------------------------------------

ZxDoc::ZxDoc() {
  xmlDecl = NULL;
  docTypeDecl = NULL;
  root = NULL;
}

ZxDoc *ZxDoc::loadMem(const char *data, Guint dataLen) {
  ZxDoc *doc;

  doc = new ZxDoc();
  if (!doc->parse(data, dataLen)) {
    delete doc;
    return NULL;
  }
  return doc;
}

ZxDoc *ZxDoc::loadFile(const char *fileName) {
  ZxDoc *doc;
  FILE *f;
  char *data;
  Guint dataLen;

  if (!(f = fopen(fileName, "rb"))) {
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  dataLen = (Guint)ftell(f);
  if (!dataLen) {
    fclose(f);
    return NULL;
  }
  fseek(f, 0, SEEK_SET);
  data = (char *)gmalloc(dataLen);
  if (fread(data, 1, dataLen, f) != dataLen) {
    fclose(f);
    gfree(data);
    return NULL;
  }
  fclose(f);
  doc = loadMem(data, dataLen);
  gfree(data);
  return doc;
}

ZxDoc::~ZxDoc() {
}

static bool writeToFileFunc(void *stream, const char *data, int length) {
  return (int)fwrite(data, 1, length, (FILE *)stream) == length;
}

bool ZxDoc::writeFile(const char *fileName) {
  FILE *f;

  if (!(f = fopen(fileName, "wb"))) {
    return false;
  }
  write(&writeToFileFunc, f);
  fclose(f);
  return true;
}

void ZxDoc::addChild(ZxNode *node) {
  if (node->isXMLDecl() && !xmlDecl) {
    xmlDecl = (ZxXMLDecl *)node;
  } else if (node->isDocTypeDecl() && !docTypeDecl) {
    docTypeDecl = (ZxDocTypeDecl *)node;
  } else if (node->isElement() && !root) {
    root = (ZxElement *)node;
  }
  ZxNode::addChild(node);
}

bool ZxDoc::parse(const char *data, Guint dataLen) {
  parsePtr = data;
  parseEnd = data + dataLen;
  
  parseSpace();
  parseXMLDecl(this);
  parseMisc(this);
  parseDocTypeDecl(this);
  parseMisc(this);
  if (match("<")) {
    parseElement(this);
  }
  parseMisc(this);
  return root != NULL;
}

void ZxDoc::parseXMLDecl(ZxNode *par) {
  GString *version, *encoding, *s;
  bool standalone;

  if (!match("<?xml")) {
    return;
  }
  parsePtr += 5;
  parseSpace();

  // version
  version = NULL;
  if (match("version")) {
    parsePtr += 7;
    parseSpace();
    if (match("=")) {
      ++parsePtr;
      parseSpace();
      version = parseQuotedString();
    }
  }
  if (!version) {
    version = new GString("1.0");
  }
  parseSpace();
	
  // encoding
  encoding = NULL;
  if (match("encoding")) {
    parsePtr += 8;
    parseSpace();
    if (match("=")) {
      ++parsePtr;
      parseSpace();
      encoding = parseQuotedString();
    }
  }
  parseSpace();

  // standalone
  standalone = false;
  if (match("standalone")) {
    parsePtr += 10;
    parseSpace();
    if (match("=")) {
      ++parsePtr;
      parseSpace();
      s = parseQuotedString();
      standalone = !s->cmp("yes");
      delete s;
    }
  }
  parseSpace();

  if (match("?>")) {
    parsePtr += 2;
  }

  par->addChild(new ZxXMLDecl(version, encoding, standalone));
}

//~ this just skips everything after the name
void ZxDoc::parseDocTypeDecl(ZxNode *par) {
  GString *name;
  int state;
  char c, quote;

  if (!match("<!DOCTYPE")) {
    return;
  }
  parsePtr += 9;
  parseSpace();

  name = parseName();
  parseSpace();

  state = 0;
  quote = '\0';
  while (parsePtr < parseEnd && state < 4) {
    c = *parsePtr++;
    switch (state) {
    case 0: // not in square brackets; not in quotes
      if (c == '>') {
	state = 4;
      } else if (c == '"' || c == '\'') {
	state = 1;
      } else if (c == '[') {
	state = 2;
      }
      break;
    case 1: // not in square brackets; in quotes
      if (c == quote) {
	state = 0;
      }
      break;
    case 2: // in square brackets; not in quotes
      if (c == ']') {
	state = 0;
      } else if (c == '"' || c == '\'') {
	state = 3;
      }
      break;
    case 3: // in square brackets; in quotes
      if (c == quote) {
	state = 2;
      }
      break;
    }
  }

  par->addChild(new ZxDocTypeDecl(name));
}

// assumes match("<")
void ZxDoc::parseElement(ZxNode *par) {
  GString *type;
  ZxElement *elem;
  ZxAttr *attr;

  ++parsePtr;
  type = parseName();
  elem = new ZxElement(type);
  parseSpace();
  while ((attr = parseAttr())) {
    elem->addAttr(attr);
    parseSpace();
  }
  if (match("/>")) {
    parsePtr += 2;
  } else if (match(">")) {
    ++parsePtr;
    parseContent(elem);
  }
  par->addChild(elem);
}

ZxAttr *ZxDoc::parseAttr() {
  GString *name, *value;
  const char *start;
  char quote, c;
  int x, n;

  name = parseName();
  parseSpace();
  if (!match("=")) {
    delete name;
    return NULL;
  }
  ++parsePtr;
  parseSpace();
  if (!(parsePtr < parseEnd && (*parsePtr == '"' || *parsePtr == '\''))) {
    delete name;
    return NULL;
  }
  quote = *parsePtr++;
  value = new GString();
  while (parsePtr < parseEnd && *parsePtr != quote) {
    if (*parsePtr == '&') {
      ++parsePtr;
      if (parsePtr < parseEnd && *parsePtr == '#') {
	++parsePtr;
	if (parsePtr < parseEnd && *parsePtr == 'x') {
	  ++parsePtr;
	  x = 0;
	  while (parsePtr < parseEnd) {
	    c = *parsePtr;
	    if (c >= '0' && c <= '9') {
	      x = (x << 4) + (c - '0');
	    } else if (c >= 'a' && c <= 'f') {
	      x = (x << 4) + (10 + c - 'a');
	    } else if (c >= 'A' && c <= 'F') {
	      x = (x << 4) + (10 + c - 'A');
	    } else {
	      break;
	    }
	    ++parsePtr;
	  }
	  if (parsePtr < parseEnd && *parsePtr == ';') {
	    ++parsePtr;
	  }
	  appendUTF8(value, x);
	} else {
	  x = 0;
	  while (parsePtr < parseEnd) {
	    c = *parsePtr;
	    if (c >= '0' && c <= '9') {
	      x = x * 10 + (c - '0');
	    } else {
	      break;
	    }
	    ++parsePtr;
	  }
	  if (parsePtr < parseEnd && *parsePtr == ';') {
	    ++parsePtr;
	  }
	  appendUTF8(value, x);
	}
      } else {
	start = parsePtr;
	for (++parsePtr;
	     parsePtr < parseEnd && *parsePtr != ';' &&
	       *parsePtr != quote && *parsePtr != '&';
	     ++parsePtr) ;
	n = (int)(parsePtr - start);
	if (parsePtr < parseEnd && *parsePtr == ';') {
	  ++parsePtr;
	}
	if (n == 2 && !strncmp(start, "lt", 2)) {
	  value->append('<');
	} else if (n == 2 && !strncmp(start, "gt", 2)) {
	  value->append('>');
	} else if (n == 3 && !strncmp(start, "amp", 3)) {
	  value->append('&');
	} else if (n == 4 && !strncmp(start, "apos", 4)) {
	  value->append('\'');
	} else if (n == 4 && !strncmp(start, "quot", 4)) {
	  value->append('"');
	} else {
	  value->append(start - 1, (int)(parsePtr - start) + 1);
	}
      }
    } else {
      start = parsePtr;
      for (++parsePtr;
	   parsePtr < parseEnd && *parsePtr != quote && *parsePtr != '&';
	   ++parsePtr) ;
      value->append(start, (int)(parsePtr - start));
    }
  }
  if (parsePtr < parseEnd && *parsePtr == quote) {
    ++parsePtr;
  }
  return new ZxAttr(name, value);
}

// this consumes the end tag
void ZxDoc::parseContent(ZxElement *par) {
  GString *endType;

  endType = (new GString("</"))->append(par->getType());

  while (parsePtr < parseEnd) {
    if (match(endType->getCString())) {
      parsePtr += endType->getLength();
      parseSpace();
      if (match(">")) {
	++parsePtr;
      }
      break;
    } else if (match("<?")) {
      parsePI(par);
    } else if (match("<![CDATA[")) {
      parseCDSect(par);
    } else if (match("<!--")) {
      parseComment(par);
    } else if (match("<")) {
      parseElement(par);
    } else {
      parseCharData(par);
    }
  }

  delete endType;
}

void ZxDoc::parseCharData(ZxElement *par) {
  GString *data;
  const char *start;
  char c;
  int x, n;

  data = new GString();
  while (parsePtr < parseEnd && *parsePtr != '<') {
    if (*parsePtr == '&') {
      ++parsePtr;
      if (parsePtr < parseEnd && *parsePtr == '#') {
	++parsePtr;
	if (parsePtr < parseEnd && *parsePtr == 'x') {
	  ++parsePtr;
	  x = 0;
	  while (parsePtr < parseEnd) {
	    c = *parsePtr;
	    if (c >= '0' && c <= '9') {
	      x = (x << 4) + (c - '0');
	    } else if (c >= 'a' && c <= 'f') {
	      x = (x << 4) + (10 + c - 'a');
	    } else if (c >= 'A' && c <= 'F') {
	      x = (x << 4) + (10 + c - 'A');
	    } else {
	      break;
	    }
	    ++parsePtr;
	  }
	  if (parsePtr < parseEnd && *parsePtr == ';') {
	    ++parsePtr;
	  }
	  appendUTF8(data, x);
	} else {
	  x = 0;
	  while (parsePtr < parseEnd) {
	    c = *parsePtr;
	    if (c >= '0' && c <= '9') {
	      x = x * 10 + (c - '0');
	    } else {
	      break;
	    }
	    ++parsePtr;
	  }
	  if (parsePtr < parseEnd && *parsePtr == ';') {
	    ++parsePtr;
	  }
	  appendUTF8(data, x);
	}
      } else {
	start = parsePtr;
	for (++parsePtr;
	     parsePtr < parseEnd && *parsePtr != ';' &&
	       *parsePtr != '<' && *parsePtr != '&';
	     ++parsePtr) ;
	n = (int)(parsePtr - start);
	if (parsePtr < parseEnd && *parsePtr == ';') {
	  ++parsePtr;
	}
	if (n == 2 && !strncmp(start, "lt", 2)) {
	  data->append('<');
	} else if (n == 2 && !strncmp(start, "gt", 2)) {
	  data->append('>');
	} else if (n == 3 && !strncmp(start, "amp", 3)) {
	  data->append('&');
	} else if (n == 4 && !strncmp(start, "apos", 4)) {
	  data->append('\'');
	} else if (n == 4 && !strncmp(start, "quot", 4)) {
	  data->append('"');
	} else {
	  data->append(start - 1, (int)(parsePtr - start) + 1);
	}
      }
    } else {
      start = parsePtr;
      for (++parsePtr;
	   parsePtr < parseEnd && *parsePtr != '<' && *parsePtr != '&';
	   ++parsePtr) ;
      data->append(start, (int)(parsePtr - start));
    }
  }
  par->addChild(new ZxCharData(data, true));
}

void ZxDoc::appendUTF8(GString *s, int c) {
  if (c <= 0x7f) {
    s->append((char)c);
  } else if (c <= 0x7ff) {
    s->append((char)(0xc0 + (c >> 6)));
    s->append((char)(0x80 + (c & 0x3f)));
  } else if (c <= 0xffff) {
    s->append((char)(0xe0 + (c >> 12)));
    s->append((char)(0x80 + ((c >> 6) & 0x3f)));
    s->append((char)(0x80 + (c & 0x3f)));
  } else if (c <= 0x1fffff) {
    s->append((char)(0xf0 + (c >> 18)));
    s->append((char)(0x80 + ((c >> 12) & 0x3f)));
    s->append((char)(0x80 + ((c >> 6) & 0x3f)));
    s->append((char)(0x80 + (c & 0x3f)));
  } else if (c <= 0x3ffffff) {
    s->append((char)(0xf8 + (c >> 24)));
    s->append((char)(0x80 + ((c >> 18) & 0x3f)));
    s->append((char)(0x80 + ((c >> 12) & 0x3f)));
    s->append((char)(0x80 + ((c >> 6) & 0x3f)));
    s->append((char)(0x80 + (c & 0x3f)));
  } else if (c <= 0x7fffffff) {
    s->append((char)(0xfc + (c >> 30)));
    s->append((char)(0x80 + ((c >> 24) & 0x3f)));
    s->append((char)(0x80 + ((c >> 18) & 0x3f)));
    s->append((char)(0x80 + ((c >> 12) & 0x3f)));
    s->append((char)(0x80 + ((c >> 6) & 0x3f)));
    s->append((char)(0x80 + (c & 0x3f)));
  }
}

// assumes match("<![CDATA[")
void ZxDoc::parseCDSect(ZxNode *par) {
  const char *start;

  parsePtr += 9;
  start = parsePtr;
  while (parsePtr < parseEnd - 3) {
    if (!strncmp(parsePtr, "]]>", 3)) {
      par->addChild(new ZxCharData(new GString(start, (int)(parsePtr - start)),
				   false));
      parsePtr += 3;
      return;
    }
    ++parsePtr;
  }
  parsePtr = parseEnd;
  par->addChild(new ZxCharData(new GString(start, (int)(parsePtr - start)),
			       false));
}

void ZxDoc::parseMisc(ZxNode *par) {
  while (1) {
    if (match("<!--")) {
      parseComment(par);
    } else if (match("<?")) {
      parsePI(par);
    } else if (parsePtr < parseEnd && (*parsePtr == '\x20' ||
				       *parsePtr == '\x09' ||
				       *parsePtr == '\x0d' ||
				       *parsePtr == '\x0a')) {
      ++parsePtr;
    } else {
      break;
    }
  }
}

// assumes match("<!--")
void ZxDoc::parseComment(ZxNode *par) {
  const char *start;

  parsePtr += 4;
  start = parsePtr;
  while (parsePtr <= parseEnd - 3) {
    if (!strncmp(parsePtr, "-->", 3)) {
      par->addChild(new ZxComment(new GString(start, (int)(parsePtr - start))));
      parsePtr += 3;
      return;
    }
    ++parsePtr;
  }
  parsePtr = parseEnd;
}

// assumes match("<?")
void ZxDoc::parsePI(ZxNode *par) {
  GString *target;
  const char *start;

  parsePtr += 2;
  target = parseName();
  parseSpace();
  start = parsePtr;
  while (parsePtr <= parseEnd - 2) {
    if (!strncmp(parsePtr, "?>", 2)) {
      par->addChild(new ZxPI(target, new GString(start,
						 (int)(parsePtr - start))));
      parsePtr += 2;
      return;
    }
    ++parsePtr;
  }
  parsePtr = parseEnd;
  par->addChild(new ZxPI(target, new GString(start, (int)(parsePtr - start))));
}

//~ this accepts all chars >= 0x80
//~ this doesn't check for properly-formed UTF-8
GString *ZxDoc::parseName() {
  GString *name;

  name = new GString();
  if (parsePtr < parseEnd && nameStartChar[*parsePtr & 0xff]) {
    name->append(*parsePtr++);
    while (parsePtr < parseEnd && nameChar[*parsePtr & 0xff]) {
      name->append(*parsePtr++);
    }
  }
  return name;
}

GString *ZxDoc::parseQuotedString() {
  GString *s;
  const char *start;
  char quote;

  if (parsePtr < parseEnd && (*parsePtr == '"' || *parsePtr == '\'')) {
    quote = *parsePtr++;
    start = parsePtr;
    while (parsePtr < parseEnd && *parsePtr != quote) {
      ++parsePtr;
    }
    s = new GString(start, (int)(parsePtr - start));
    if (parsePtr < parseEnd && *parsePtr == quote) {
      ++parsePtr;
    }
  } else {
    s = new GString();
  }
  return s;
}

void ZxDoc::parseSpace() {
  while (parsePtr < parseEnd && (*parsePtr == '\x20' ||
				 *parsePtr == '\x09' ||
				 *parsePtr == '\x0d' ||
				 *parsePtr == '\x0a')) {
    ++parsePtr;
  }
}

bool ZxDoc::match(const char *s) {
  int n;

  n = (int)strlen(s);
  return parseEnd - parsePtr >= n && !strncmp(parsePtr, s, n);
}

bool ZxDoc::write(ZxWriteFunc writeFunc, void *stream) {
  ZxNode *child;

  for (child = getFirstChild(); child; child = child->getNextChild()) {
    if (!child->write(writeFunc, stream)) {
      return false;
    }
    if (!(*writeFunc)(stream, "\n", 1)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------

ZxXMLDecl::ZxXMLDecl(GString *versionA, GString *encodingA, bool standaloneA) {
  version = versionA;
  encoding = encodingA;
  standalone = standaloneA;
}

ZxXMLDecl::~ZxXMLDecl() {
  delete version;
  if (encoding) {
    delete encoding;
  }
}

bool ZxXMLDecl::write(ZxWriteFunc writeFunc, void *stream) {
  GString *s;
  bool ok;

  s = new GString("<?xml version=\"");
  s->append(version);
  s->append("\"");
  if (encoding) {
    s->append(" encoding=\"");
    s->append(encoding);
    s->append("\"");
  }
  if (standalone) {
    s->append(" standlone=\"yes\"");
  }
  s->append("?>");
  ok = (*writeFunc)(stream, s->getCString(), s->getLength());
  delete s;
  return ok;
}

//------------------------------------------------------------------------

ZxDocTypeDecl::ZxDocTypeDecl(GString *nameA) {
  name = nameA;
}

ZxDocTypeDecl::~ZxDocTypeDecl() {
  delete name;
}

bool ZxDocTypeDecl::write(ZxWriteFunc writeFunc, void *stream) {
  GString *s;
  bool ok;

  s = new GString("<!DOCTYPE ");
  s->append(name);
  s->append(">");
  ok = (*writeFunc)(stream, s->getCString(), s->getLength());
  delete s;
  return ok;
}

//------------------------------------------------------------------------

ZxComment::ZxComment(GString *textA) {
  text = textA;
}

ZxComment::~ZxComment() {
  delete text;
}

bool ZxComment::write(ZxWriteFunc writeFunc, void *stream) {
  GString *s;
  bool ok;

  s = new GString("<!--");
  s->append(text);
  s->append("-->");
  ok = (*writeFunc)(stream, s->getCString(), s->getLength());
  delete s;
  return ok;
}

//------------------------------------------------------------------------

ZxPI::ZxPI(GString *targetA, GString *textA) {
  target = targetA;
  text = textA;
}

ZxPI::~ZxPI() {
  delete target;
  delete text;
}

bool ZxPI::write(ZxWriteFunc writeFunc, void *stream) {
  GString *s;
  bool ok;

  s = new GString("<?");
  s->append(target);
  s->append(" ");
  s->append(text);
  s->append("?>");
  ok = (*writeFunc)(stream, s->getCString(), s->getLength());
  delete s;
  return ok;
}

//------------------------------------------------------------------------

ZxElement::ZxElement(GString *typeA) {
  type = typeA;
  attrs = new GHash();
  firstAttr = lastAttr = NULL;
}
  
ZxElement::~ZxElement() {
  delete type;
  deleteGHash(attrs, ZxAttr);
}

bool ZxElement::isElement(const char *typeA) {
  return !type->cmp(typeA);
}

ZxAttr *ZxElement::findAttr(const char *attrName) {
  return (ZxAttr *)attrs->lookup(attrName);
}

void ZxElement::addAttr(ZxAttr *attr) {
  attrs->add(attr->getName(), attr);
  if (lastAttr) {
    lastAttr->next = attr;
    lastAttr= attr;
  } else {
    firstAttr = lastAttr = attr;
  }
  attr->parent = this;
  attr->next = NULL;
}

bool ZxElement::write(ZxWriteFunc writeFunc, void *stream) {
  GString *s;
  ZxAttr *attr;
  ZxNode *child;
  bool ok;

  s = new GString("<");
  s->append(type);
  for (attr = firstAttr; attr; attr = attr->getNextAttr()) {
    s->append(" ");
    s->append(attr->name);
    s->append("=\"");
    appendEscapedAttrValue(s, attr->value);
    s->append("\"");
  }
  if ((child = getFirstChild())) {
    s->append(">");
  } else {
    s->append("/>");
  }
  ok = (*writeFunc)(stream, s->getCString(), s->getLength());
  delete s;
  if (!ok) {
    return false;
  }
  if (child) {
    for (; child; child = child->getNextChild()) {
      if (!child->write(writeFunc, stream)) {
	return false;
      }
    }
    s = new GString();
    s->append("</");
    s->append(type);
    s->append(">");
    ok = (*writeFunc)(stream, s->getCString(), s->getLength());
    delete s;
    if (!ok) {
      return false;
    }
  }
  return true;
}

void ZxElement::appendEscapedAttrValue(GString *out, GString *s) {
  char c;
  int i;

  for (i = 0; i < s->getLength(); ++i) {
    c = s->getChar(i);
    if (c == '<') {
      out->append("&lt;");
    } else if (c == '>') {
      out->append("&gt;");
    } else if (c == '&') {
      out->append("&amp;");
    } else if (c == '"') {
      out->append("&quot;");
    } else {
      out->append(c);
    }
  }
}

//------------------------------------------------------------------------

ZxAttr::ZxAttr(GString *nameA, GString *valueA) {
  name = nameA;
  value = valueA;
  parent = NULL;
  next = NULL;
}

ZxAttr::~ZxAttr() {
  delete name;
  delete value;
}

//------------------------------------------------------------------------

ZxCharData::ZxCharData(GString *dataA, bool parsedA) {
  data = dataA;
  parsed = parsedA;
}

ZxCharData::~ZxCharData() {
  delete data;
}

bool ZxCharData::write(ZxWriteFunc writeFunc, void *stream) {
  GString *s;
  char c;
  int i;
  bool ok;

  s = new GString();
  if (parsed) {
    for (i = 0; i < data->getLength(); ++i) {
      c = data->getChar(i);
      if (c == '<') {
	s->append("&lt;");
      } else if (c == '>') {
	s->append("&gt;");
      } else if (c == '&') {
	s->append("&amp;");
      } else {
	s->append(c);
      }
    }
  } else {
    s->append("<![CDATA[");
    s->append(data);
    s->append("]]>");
  }
  ok = (*writeFunc)(stream, s->getCString(), s->getLength());
  delete s;
  return ok;
}
