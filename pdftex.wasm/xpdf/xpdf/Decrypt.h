//========================================================================
//
// Decrypt.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef DECRYPT_H
#define DECRYPT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"

//------------------------------------------------------------------------
// Decrypt
//------------------------------------------------------------------------

class Decrypt {
public:

  // Generate a file key.  The <fileKey> buffer must have space for at
  // least 16 bytes.  Checks <ownerPassword> and then <userPassword>
  // and returns true if either is correct.  Sets <ownerPasswordOk> if
  // the owner password was correct.  Either or both of the passwords
  // may be NULL, which is treated as an empty string.
  static GBool makeFileKey(int encVersion, int encRevision, int keyLength,
			   GString *ownerKey, GString *userKey,
			   GString *ownerEnc, GString *userEnc,
			   int permissions, GString *fileID,
			   GString *ownerPassword, GString *userPassword,
			   Guchar *fileKey, GBool encryptMetadata,
			   GBool *ownerPasswordOk);

private:

  static void r6Hash(Guchar *key, int keyLen, const char *pwd, int pwdLen,
		     char *userKey);
  static GBool makeFileKey2(int encVersion, int encRevision, int keyLength,
			    GString *ownerKey, GString *userKey,
			    int permissions, GString *fileID,
			    GString *userPassword, Guchar *fileKey,
			    GBool encryptMetadata);
};

//------------------------------------------------------------------------
// DecryptStream
//------------------------------------------------------------------------

struct DecryptRC4State {
  Guchar state[256];
  Guchar x, y;
  int buf;
};

struct DecryptAESState {
  Guint w[44];
  Guchar state[16];
  Guchar cbc[16];
  Guchar buf[16];
  int bufIdx;
};

struct DecryptAES256State {
  Guint w[60];
  Guchar state[16];
  Guchar cbc[16];
  Guchar buf[16];
  int bufIdx;
};

class DecryptStream: public FilterStream {
public:

  DecryptStream(Stream *strA, Guchar *fileKeyA,
		CryptAlgorithm algoA, int keyLengthA,
		int objNumA, int objGenA);
  virtual ~DecryptStream();
  virtual Stream *copy();
  virtual StreamKind getKind() { return strWeird; }
  virtual void reset();
  virtual int getChar();
  virtual int lookChar();
  virtual GBool isBinary(GBool last);
  virtual Stream *getUndecodedStream() { return this; }

private:

  Guchar fileKey[32];
  CryptAlgorithm algo;
  int keyLength;
  int objNum, objGen;
  int objKeyLength;
  Guchar objKey[32];

  union {
    DecryptRC4State rc4;
    DecryptAESState aes;
    DecryptAES256State aes256;
  } state;
};

//------------------------------------------------------------------------

struct MD5State {
  Gulong a, b, c, d;
  Guchar buf[64];
  int bufLen;
  int msgLen;
  Guchar digest[16];
};

extern void rc4InitKey(Guchar *key, int keyLen, Guchar *state);
extern Guchar rc4DecryptByte(Guchar *state, Guchar *x, Guchar *y, Guchar c);
void md5Start(MD5State *state);
void md5Append(MD5State *state, Guchar *data, int dataLen);
void md5Finish(MD5State *state);
extern void md5(Guchar *msg, int msgLen, Guchar *digest);
extern void aesKeyExpansion(DecryptAESState *s,
			    Guchar *objKey, int objKeyLen,
			    GBool decrypt);
extern void aesEncryptBlock(DecryptAESState *s, Guchar *in);
extern void aesDecryptBlock(DecryptAESState *s, Guchar *in, GBool last);

#endif
