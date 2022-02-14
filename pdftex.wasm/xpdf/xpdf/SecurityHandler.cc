//========================================================================
//
// SecurityHandler.cc
//
// Copyright 2004 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmempp.h"
#include "GString.h"
#include "PDFDoc.h"
#include "Decrypt.h"
#include "Error.h"
#include "GlobalParams.h"
#include "PDFCore.h"
#include "SecurityHandler.h"

//------------------------------------------------------------------------
// SecurityHandler
//------------------------------------------------------------------------

SecurityHandler *SecurityHandler::make(PDFDoc *docA, Object *encryptDictA) {
  Object filterObj;
  SecurityHandler *secHdlr;

  encryptDictA->dictLookup("Filter", &filterObj);
  if (filterObj.isName("Standard")) {
    secHdlr = new StandardSecurityHandler(docA, encryptDictA);
  } else if (filterObj.isName()) {
    error(errSyntaxError, -1, "Couldn't find the '{0:s}' security handler",
	  filterObj.getName());
    secHdlr = NULL;
  } else {
    error(errSyntaxError, -1,
	  "Missing or invalid 'Filter' entry in encryption dictionary");
    secHdlr = NULL;
  }
  filterObj.free();
  return secHdlr;
}

SecurityHandler::SecurityHandler(PDFDoc *docA) {
  doc = docA;
}

SecurityHandler::~SecurityHandler() {
}

GBool SecurityHandler::checkEncryption(GString *ownerPassword,
				       GString *userPassword) {
  void *authData;
  GBool ok;
  int i;

  if (ownerPassword || userPassword) {
    authData = makeAuthData(ownerPassword, userPassword);
  } else {
    authData = NULL;
  }
  ok = authorize(authData);
  if (authData) {
    freeAuthData(authData);
  }
  for (i = 0; !ok && i < 3; ++i) {
    if (!(authData = getAuthData())) {
      break;
    }
    ok = authorize(authData);
    if (authData) {
      freeAuthData(authData);
    }
  }
  if (!ok) {
    error(errCommandLine, -1, "Incorrect password");
  }
  return ok;
}

//------------------------------------------------------------------------
// StandardSecurityHandler
//------------------------------------------------------------------------

class StandardAuthData {
public:

  StandardAuthData(GString *ownerPasswordA, GString *userPasswordA) {
    ownerPassword = ownerPasswordA;
    userPassword = userPasswordA;
  }

  ~StandardAuthData() {
    if (ownerPassword) {
      delete ownerPassword;
    }
    if (userPassword) {
      delete userPassword;
    }
  }

  GString *ownerPassword;
  GString *userPassword;
};

StandardSecurityHandler::StandardSecurityHandler(PDFDoc *docA,
						 Object *encryptDictA):
  SecurityHandler(docA)
{
  Object versionObj, revisionObj, lengthObj;
  Object ownerKeyObj, userKeyObj, ownerEncObj, userEncObj;
  Object permObj, fileIDObj, fileIDObj1;
  Object cryptFiltersObj, streamFilterObj, stringFilterObj;
  Object cryptFilterObj, cfmObj, cfLengthObj;
  Object encryptMetadataObj;

  ok = gFalse;
  fileID = NULL;
  ownerKey = NULL;
  userKey = NULL;
  ownerEnc = NULL;
  userEnc = NULL;
  fileKeyLength = 0;

  //--- get the main parameters
  encryptDictA->dictLookup("V", &versionObj);
  encryptDictA->dictLookup("R", &revisionObj);
  encryptDictA->dictLookup("Length", &lengthObj);
  encryptDictA->dictLookup("O", &ownerKeyObj);
  encryptDictA->dictLookup("U", &userKeyObj);
  encryptDictA->dictLookup("OE", &ownerEncObj);
  encryptDictA->dictLookup("UE", &userEncObj);
  encryptDictA->dictLookup("P", &permObj);
  doc->getXRef()->getTrailerDict()->dictLookup("ID", &fileIDObj);
  if (!versionObj.isInt() ||
      !revisionObj.isInt() ||
      !permObj.isInt() ||
      !ownerKeyObj.isString() ||
      !userKeyObj.isString()) {
    error(errSyntaxError, -1, "Invalid encryption parameters");
    goto done;
  }
  encVersion = versionObj.getInt();
  encRevision = revisionObj.getInt();
  encAlgorithm = cryptRC4;
  // revision 2 forces a 40-bit key - some buggy PDF generators
  // set the Length value incorrectly
  if (encRevision == 2 || !lengthObj.isInt()) {
    fileKeyLength = 5;
  } else {
    fileKeyLength = lengthObj.getInt() / 8;
  }
  encryptMetadata = gTrue;

  //--- check for a crypt filter (which can modify the parameters)
  //~ this currently only handles a subset of crypt filter functionality
  //~ (in particular, it ignores the EFF entry in encryptDictA, and
  //~ doesn't handle the case where StmF, StrF, and EFF are not all the
  //~ same)
  if ((encVersion == 4 || encVersion == 5) &&
      (encRevision == 4 || encRevision == 5 || encRevision == 6)) {
    encryptDictA->dictLookup("CF", &cryptFiltersObj);
    encryptDictA->dictLookup("StmF", &streamFilterObj);
    encryptDictA->dictLookup("StrF", &stringFilterObj);
    if (cryptFiltersObj.isDict() &&
	streamFilterObj.isName() &&
	stringFilterObj.isName() &&
	!strcmp(streamFilterObj.getName(), stringFilterObj.getName())) {
      if (!strcmp(streamFilterObj.getName(), "Identity")) {
	// no encryption on streams or strings
	stringFilterObj.free();
	streamFilterObj.free();
	cryptFiltersObj.free();
	goto done;
      }
      if (cryptFiltersObj.dictLookup(streamFilterObj.getName(),
				     &cryptFilterObj)->isDict()) {
	cryptFilterObj.dictLookup("CFM", &cfmObj);
	if (cfmObj.isName("V2")) {
	  if (cryptFilterObj.dictLookup("Length",
					&cfLengthObj)->isInt()) {
	    fileKeyLength = cfLengthObj.getInt();
	  }
	  cfLengthObj.free();
	  encVersion = 2;
	  encRevision = 3;
	} else if (cfmObj.isName("AESV2")) {
	  if (cryptFilterObj.dictLookup("Length",
					&cfLengthObj)->isInt()) {
	    fileKeyLength = cfLengthObj.getInt();
	  }
	  cfLengthObj.free();
	  encVersion = 2;
	  encRevision = 3;
	  encAlgorithm = cryptAES;
	} else if (cfmObj.isName("AESV3")) {
	  if (cryptFilterObj.dictLookup("Length",
					&cfLengthObj)->isInt()) {
	    fileKeyLength = cfLengthObj.getInt();
	  }
	  cfLengthObj.free();
	  if (fileKeyLength == 16) {
	    // this isn't allowed by the spec, but Adobe supports it
	    encVersion = 2;
	    encRevision = 3;
	    encAlgorithm = cryptAES;
	  } else {
	    encVersion = 5;
	    if (encRevision != 5 && encRevision != 6) {
	      encRevision = 6;
	    }
	    encAlgorithm = cryptAES256;
	  }
	}
	cfmObj.free();
      }
      cryptFilterObj.free();
    }
    stringFilterObj.free();
    streamFilterObj.free();
    cryptFiltersObj.free();
    if (encryptDictA->dictLookup("EncryptMetadata",
				 &encryptMetadataObj)->isBool()) {
      encryptMetadata = encryptMetadataObj.getBool();
    }
    encryptMetadataObj.free();
  }

  //--- version-specific parameters
  if (encRevision <= 4) {
    if (ownerKeyObj.getString()->getLength() != 32 ||
	userKeyObj.getString()->getLength() != 32) {
      error(errSyntaxError, -1, "Invalid encryption key length");
      // this is non-fatal -- see below
    }
  } else if (encRevision <= 6) {
    // the spec says 48 bytes, but Acrobat pads them out longer
    if (ownerKeyObj.getString()->getLength() < 48 ||
	userKeyObj.getString()->getLength() < 48 ||
	!ownerEncObj.isString() ||
	ownerEncObj.getString()->getLength() != 32 ||
	!userEncObj.isString() ||
	userEncObj.getString()->getLength() != 32) {
      error(errSyntaxError, -1, "Invalid encryption key length");
      goto done;
    }
  }
  permFlags = permObj.getInt();
  ownerKey = ownerKeyObj.getString()->copy();
  userKey = userKeyObj.getString()->copy();
  if (encRevision <= 4) {
    // Adobe apparently zero-pads the U value (and maybe the O value?)
    // if it's short
    while (ownerKey->getLength() < 32) {
      ownerKey->append((char)0x00);
    }
    while (userKey->getLength() < 32) {
      userKey->append((char)0x00);
    }
  }
  if (encVersion >= 1 && encVersion <= 2 &&
      encRevision >= 2 && encRevision <= 3) {
    if (fileIDObj.isArray()) {
      if (fileIDObj.arrayGet(0, &fileIDObj1)->isString()) {
	fileID = fileIDObj1.getString()->copy();
      } else {
	fileID = new GString();
      }
      fileIDObj1.free();
    } else {
      fileID = new GString();
    }
    if (fileKeyLength > 16 || fileKeyLength <= 0) {
      fileKeyLength = 16;
    }
    ok = gTrue;
  } else if (encVersion == 5 && (encRevision == 5 || encRevision == 6)) {
    fileID = new GString(); // unused for V=R=5
    ownerEnc = ownerEncObj.getString()->copy();
    userEnc = userEncObj.getString()->copy();
    if (fileKeyLength > 32 || fileKeyLength <= 0) {
      fileKeyLength = 32;
    }
    ok = gTrue;
  } else {
    error(errUnimplemented, -1,
	  "Unsupported version/revision ({0:d}/{1:d}) of Standard security handler",
	  encVersion, encRevision);
  }

 done:
  fileIDObj.free();
  permObj.free();
  userEncObj.free();
  ownerEncObj.free();
  userKeyObj.free();
  ownerKeyObj.free();
  lengthObj.free();
  revisionObj.free();
  versionObj.free();
}

StandardSecurityHandler::~StandardSecurityHandler() {
  if (fileID) {
    delete fileID;
  }
  if (ownerKey) {
    delete ownerKey;
  }
  if (userKey) {
    delete userKey;
  }
  if (ownerEnc) {
    delete ownerEnc;
  }
  if (userEnc) {
    delete userEnc;
  }
}

GBool StandardSecurityHandler::isUnencrypted() {
  return encVersion == -1 && encRevision == -1;
}

void *StandardSecurityHandler::makeAuthData(GString *ownerPassword,
					    GString *userPassword) {
  return new StandardAuthData(ownerPassword ? ownerPassword->copy()
			                    : (GString *)NULL,
			      userPassword ? userPassword->copy()
			                   : (GString *)NULL);
}

void *StandardSecurityHandler::getAuthData() {
  PDFCore *core;
  GString *password;

  if (!(core = doc->getCore()) ||
      !(password = core->getPassword())) {
    return NULL;
  }
  return new StandardAuthData(password, password->copy());
}

void StandardSecurityHandler::freeAuthData(void *authData) {
  delete (StandardAuthData *)authData;
}

GBool StandardSecurityHandler::authorize(void *authData) {
  GString *ownerPassword, *userPassword;

  if (!ok) {
    return gFalse;
  }
  if (authData) {
    ownerPassword = ((StandardAuthData *)authData)->ownerPassword;
    userPassword = ((StandardAuthData *)authData)->userPassword;
  } else {
    ownerPassword = NULL;
    userPassword = NULL;
  }
  if (!Decrypt::makeFileKey(encVersion, encRevision, fileKeyLength,
			    ownerKey, userKey, ownerEnc, userEnc,
			    permFlags, fileID,
			    ownerPassword, userPassword, fileKey,
			    encryptMetadata, &ownerPasswordOk)) {
    return gFalse;
  }
  return gTrue;
}
