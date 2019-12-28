// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_PARSER_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_PARSER_H_

#include <map>
#include <memory>
#include <set>

#include "core/fxcrt/include/fx_basic.h"

class CPDF_Array;
class CPDF_CryptoHandler;
class CPDF_Dictionary;
class CPDF_Document;
class CPDF_IndirectObjectHolder;
class CPDF_Object;
class CPDF_SecurityHandler;
class CPDF_StreamAcc;
class CPDF_SyntaxParser;
class IFX_FileRead;

class CPDF_Parser {
 public:
  enum Error {
    SUCCESS = 0,
    FILE_ERROR,
    FORMAT_ERROR,
    PASSWORD_ERROR,
    HANDLER_ERROR
  };

  CPDF_Parser();
  ~CPDF_Parser();

  Error StartParse(IFX_FileRead* pFile);
  uint32_t GetPermissions() const;

  void SetPassword(const FX_CHAR* password) { m_Password = password; }
  CFX_ByteString GetPassword() { return m_Password; }
  CPDF_Dictionary* GetTrailer() const { return m_pTrailer; }
  FX_FILESIZE GetLastXRefOffset() const { return m_LastXRefOffset; }
  CPDF_Document* GetDocument() const { return m_pDocument.get(); }

  uint32_t GetRootObjNum();
  uint32_t GetInfoObjNum();
  CPDF_Array* GetIDArray();

  CPDF_Dictionary* GetEncryptDict() const { return m_pEncryptDict; }

  CPDF_Object* ParseIndirectObject(CPDF_IndirectObjectHolder* pObjList,
                                   uint32_t objnum);

  uint32_t GetLastObjNum() const;
  bool IsValidObjectNumber(uint32_t objnum) const;
  FX_FILESIZE GetObjectPositionOrZero(uint32_t objnum) const;
  uint8_t GetObjectType(uint32_t objnum) const;
  uint16_t GetObjectGenNum(uint32_t objnum) const;
  bool IsVersionUpdated() const { return m_bVersionUpdated; }
  bool IsObjectFreeOrNull(uint32_t objnum) const;
  FX_BOOL IsFormStream(uint32_t objnum, FX_BOOL& bForm);
  CPDF_CryptoHandler* GetCryptoHandler();
  IFX_FileRead* GetFileAccess() const;

  FX_FILESIZE GetObjectOffset(uint32_t objnum) const;
  FX_FILESIZE GetObjectSize(uint32_t objnum) const;

  void GetIndirectBinary(uint32_t objnum, uint8_t*& pBuffer, uint32_t& size);
  int GetFileVersion() const { return m_FileVersion; }
  FX_BOOL IsXRefStream() const { return m_bXRefStream; }

  CPDF_Object* ParseIndirectObjectAt(CPDF_IndirectObjectHolder* pObjList,
                                     FX_FILESIZE pos,
                                     uint32_t objnum);

  CPDF_Object* ParseIndirectObjectAtByStrict(
      CPDF_IndirectObjectHolder* pObjList,
      FX_FILESIZE pos,
      uint32_t objnum,
      FX_FILESIZE* pResultPos);

  Error StartAsyncParse(IFX_FileRead* pFile);

  uint32_t GetFirstPageNo() const { return m_dwFirstPageNo; }

 protected:
  struct ObjectInfo {
    ObjectInfo() : pos(0), type(0), gennum(0) {}

    FX_FILESIZE pos;
    uint8_t type;
    uint16_t gennum;
  };

  void CloseParser();
  CPDF_Object* ParseDirect(CPDF_Object* pObj);
  FX_BOOL LoadAllCrossRefV4(FX_FILESIZE pos);
  FX_BOOL LoadAllCrossRefV5(FX_FILESIZE pos);
  bool LoadCrossRefV4(FX_FILESIZE pos, FX_FILESIZE streampos, FX_BOOL bSkip);
  FX_BOOL LoadCrossRefV5(FX_FILESIZE* pos, FX_BOOL bMainXRef);
  CPDF_Dictionary* LoadTrailerV4();
  FX_BOOL RebuildCrossRef();
  Error SetEncryptHandler();
  void ReleaseEncryptHandler();
  FX_BOOL LoadLinearizedAllCrossRefV4(FX_FILESIZE pos, uint32_t dwObjCount);
  FX_BOOL LoadLinearizedCrossRefV4(FX_FILESIZE pos, uint32_t dwObjCount);
  FX_BOOL LoadLinearizedAllCrossRefV5(FX_FILESIZE pos);
  Error LoadLinearizedMainXRefTable();
  CPDF_StreamAcc* GetObjectStream(uint32_t number);
  FX_BOOL IsLinearizedFile(IFX_FileRead* pFileAccess, uint32_t offset);
  void SetEncryptDictionary(CPDF_Dictionary* pDict);
  void ShrinkObjectMap(uint32_t size);
  // A simple check whether the cross reference table matches with
  // the objects.
  bool VerifyCrossRefV4();

  std::unique_ptr<CPDF_Document> m_pDocument;
  std::unique_ptr<CPDF_SyntaxParser> m_pSyntax;
  bool m_bOwnFileRead;
  int m_FileVersion;
  CPDF_Dictionary* m_pTrailer;
  CPDF_Dictionary* m_pEncryptDict;
  FX_FILESIZE m_LastXRefOffset;
  FX_BOOL m_bXRefStream;
  std::unique_ptr<CPDF_SecurityHandler> m_pSecurityHandler;
  CFX_ByteString m_bsRecipient;
  CFX_ByteString m_FilePath;
  CFX_ByteString m_Password;
  std::map<uint32_t, ObjectInfo> m_ObjectInfo;
  std::set<FX_FILESIZE> m_SortedOffset;
  CFX_ArrayTemplate<CPDF_Dictionary*> m_Trailers;
  bool m_bVersionUpdated;
  CPDF_Object* m_pLinearized;
  uint32_t m_dwFirstPageNo;
  uint32_t m_dwXrefStartObjNum;

  // A map of object numbers to indirect streams. Map owns the streams.
  std::map<uint32_t, std::unique_ptr<CPDF_StreamAcc>> m_ObjectStreamMap;

  // Mapping of object numbers to offsets. The offsets are relative to the first
  // object in the stream.
  using StreamObjectCache = std::map<uint32_t, uint32_t>;

  // Mapping of streams to their object caches. This is valid as long as the
  // streams in |m_ObjectStreamMap| are valid.
  std::map<CPDF_StreamAcc*, StreamObjectCache> m_ObjCache;

  // All indirect object numbers that are being parsed.
  std::set<uint32_t> m_ParsingObjNums;

  friend class CPDF_DataAvail;

 private:
  enum class ParserState {
    kDefault,
    kComment,
    kWhitespace,
    kString,
    kHexString,
    kEscapedString,
    kXref,
    kObjNum,
    kPostObjNum,
    kGenNum,
    kPostGenNum,
    kTrailer,
    kBeginObj,
    kEndObj
  };
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_PARSER_H_
