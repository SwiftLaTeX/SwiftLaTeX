// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_INCLUDE_FX_BASIC_H_
#define CORE_FXCRT_INCLUDE_FX_BASIC_H_

#include <algorithm>
#include <memory>

#include "core/fxcrt/include/fx_memory.h"
#include "core/fxcrt/include/fx_stream.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CFX_BinaryBuf {
 public:
  CFX_BinaryBuf();
  explicit CFX_BinaryBuf(FX_STRSIZE size);
  ~CFX_BinaryBuf();

  uint8_t* GetBuffer() const { return m_pBuffer.get(); }
  FX_STRSIZE GetSize() const { return m_DataSize; }

  void Clear();
  void EstimateSize(FX_STRSIZE size, FX_STRSIZE alloc_step = 0);
  void AppendBlock(const void* pBuf, FX_STRSIZE size);
  void AppendString(const CFX_ByteString& str) {
    AppendBlock(str.c_str(), str.GetLength());
  }

  void AppendByte(uint8_t byte) {
    ExpandBuf(1);
    m_pBuffer.get()[m_DataSize++] = byte;
  }

  void InsertBlock(FX_STRSIZE pos, const void* pBuf, FX_STRSIZE size);
  void Delete(int start_index, int count);

  // Takes ownership of |pBuf|.
  void AttachData(uint8_t* pBuf, FX_STRSIZE size);

  // Releases ownership of |m_pBuffer| and returns it.
  uint8_t* DetachBuffer();

 protected:
  void ExpandBuf(FX_STRSIZE size);

  FX_STRSIZE m_AllocStep;
  FX_STRSIZE m_AllocSize;
  FX_STRSIZE m_DataSize;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pBuffer;
};

class CFX_ByteTextBuf : public CFX_BinaryBuf {
 public:
  FX_STRSIZE GetLength() const { return m_DataSize; }
  CFX_ByteString MakeString() const {
    return CFX_ByteString(m_pBuffer.get(), m_DataSize);
  }
  CFX_ByteStringC AsStringC() const {
    return CFX_ByteStringC(m_pBuffer.get(), m_DataSize);
  }

  void AppendChar(int ch) { AppendByte(static_cast<uint8_t>(ch)); }
  CFX_ByteTextBuf& operator<<(int i);
  CFX_ByteTextBuf& operator<<(uint32_t i);
  CFX_ByteTextBuf& operator<<(double f);
  CFX_ByteTextBuf& operator<<(const FX_CHAR* pStr) {
    return *this << CFX_ByteStringC(pStr);
  }
  CFX_ByteTextBuf& operator<<(const CFX_ByteString& str) {
    return *this << str.AsStringC();
  }
  CFX_ByteTextBuf& operator<<(const CFX_ByteStringC& lpsz);
  CFX_ByteTextBuf& operator<<(const CFX_ByteTextBuf& buf);
};

class CFX_WideTextBuf : public CFX_BinaryBuf {
 public:
  void AppendChar(FX_WCHAR wch);
  FX_STRSIZE GetLength() const { return m_DataSize / sizeof(FX_WCHAR); }
  FX_WCHAR* GetBuffer() const {
    return reinterpret_cast<FX_WCHAR*>(m_pBuffer.get());
  }

  CFX_WideStringC AsStringC() const {
    return CFX_WideStringC(reinterpret_cast<const FX_WCHAR*>(m_pBuffer.get()),
                           m_DataSize / sizeof(FX_WCHAR));
  }
  CFX_WideString MakeString() const {
    return CFX_WideString(reinterpret_cast<const FX_WCHAR*>(m_pBuffer.get()),
                          m_DataSize / sizeof(FX_WCHAR));
  }

  void Delete(int start_index, int count) {
    CFX_BinaryBuf::Delete(start_index * sizeof(FX_WCHAR),
                          count * sizeof(FX_WCHAR));
  }

  CFX_WideTextBuf& operator<<(int i);
  CFX_WideTextBuf& operator<<(double f);
  CFX_WideTextBuf& operator<<(const FX_WCHAR* lpsz);
  CFX_WideTextBuf& operator<<(const CFX_WideStringC& str);
  CFX_WideTextBuf& operator<<(const CFX_WideString& str);
  CFX_WideTextBuf& operator<<(const CFX_WideTextBuf& buf);
};

class CFX_FileBufferArchive {
 public:
  CFX_FileBufferArchive();
  ~CFX_FileBufferArchive();

  void Clear();
  bool Flush();
  int32_t AppendBlock(const void* pBuf, size_t size);
  int32_t AppendByte(uint8_t byte);
  int32_t AppendDWord(uint32_t i);
  int32_t AppendString(const CFX_ByteStringC& lpsz);

  // |pFile| must outlive the CFX_FileBufferArchive.
  void AttachFile(IFX_StreamWrite* pFile);

 private:
  static const size_t kBufSize = 32768;

  size_t m_Length;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pBuffer;
  IFX_StreamWrite* m_pFile;
};

class CFX_CharMap {
 public:
  static CFX_ByteString GetByteString(uint16_t codepage,
                                      const CFX_WideStringC& wstr);

  static CFX_WideString GetWideString(uint16_t codepage,
                                      const CFX_ByteStringC& bstr);

  CFX_CharMap() = delete;
};

class CFX_UTF8Decoder {
 public:
  CFX_UTF8Decoder() { m_PendingBytes = 0; }

  void Clear();

  void Input(uint8_t byte);

  void AppendChar(uint32_t ch);

  void ClearStatus() { m_PendingBytes = 0; }

  CFX_WideStringC GetResult() const { return m_Buffer.AsStringC(); }

 protected:
  int m_PendingBytes;

  uint32_t m_PendingChar;

  CFX_WideTextBuf m_Buffer;
};

class CFX_UTF8Encoder {
 public:
  CFX_UTF8Encoder() {}

  void Input(FX_WCHAR unicode);
  void AppendStr(const CFX_ByteStringC& str) { m_Buffer << str; }
  CFX_ByteStringC GetResult() const { return m_Buffer.AsStringC(); }

 protected:
  CFX_ByteTextBuf m_Buffer;
};

class CFX_BasicArray {
 protected:
  explicit CFX_BasicArray(int unit_size);
  CFX_BasicArray(const CFX_BasicArray&) = delete;
  ~CFX_BasicArray();

  FX_BOOL SetSize(int nNewSize);
  FX_BOOL Append(const CFX_BasicArray& src);
  FX_BOOL Copy(const CFX_BasicArray& src);
  uint8_t* InsertSpaceAt(int nIndex, int nCount);
  FX_BOOL RemoveAt(int nIndex, int nCount);
  FX_BOOL InsertAt(int nStartIndex, const CFX_BasicArray* pNewArray);
  const void* GetDataPtr(int index) const;

 protected:
  uint8_t* m_pData;
  int m_nSize;
  int m_nMaxSize;
  int m_nUnitSize;
};

template <class TYPE>
class CFX_ArrayTemplate : public CFX_BasicArray {
 public:
  CFX_ArrayTemplate() : CFX_BasicArray(sizeof(TYPE)) {}

  int GetSize() const { return m_nSize; }

  int GetUpperBound() const { return m_nSize - 1; }

  FX_BOOL SetSize(int nNewSize) { return CFX_BasicArray::SetSize(nNewSize); }

  void RemoveAll() { SetSize(0); }

  const TYPE GetAt(int nIndex) const {
    if (nIndex < 0 || nIndex >= m_nSize) {
      PDFIUM_IMMEDIATE_CRASH();
    }
    return ((const TYPE*)m_pData)[nIndex];
  }

  FX_BOOL SetAt(int nIndex, TYPE newElement) {
    if (nIndex < 0 || nIndex >= m_nSize) {
      return FALSE;
    }
    ((TYPE*)m_pData)[nIndex] = newElement;
    return TRUE;
  }

  TYPE& ElementAt(int nIndex) {
    if (nIndex < 0 || nIndex >= m_nSize) {
      PDFIUM_IMMEDIATE_CRASH();
    }
    return ((TYPE*)m_pData)[nIndex];
  }

  const TYPE* GetData() const { return (const TYPE*)m_pData; }

  TYPE* GetData() { return (TYPE*)m_pData; }

  FX_BOOL SetAtGrow(int nIndex, TYPE newElement) {
    if (nIndex < 0)
      return FALSE;

    if (nIndex >= m_nSize && !SetSize(nIndex + 1))
      return FALSE;

    ((TYPE*)m_pData)[nIndex] = newElement;
    return TRUE;
  }

  FX_BOOL Add(TYPE newElement) {
    if (m_nSize < m_nMaxSize) {
      m_nSize++;
    } else if (!SetSize(m_nSize + 1)) {
      return FALSE;
    }
    ((TYPE*)m_pData)[m_nSize - 1] = newElement;
    return TRUE;
  }

  FX_BOOL Append(const CFX_ArrayTemplate& src) {
    return CFX_BasicArray::Append(src);
  }

  FX_BOOL Copy(const CFX_ArrayTemplate& src) {
    return CFX_BasicArray::Copy(src);
  }

  TYPE* GetDataPtr(int index) {
    return (TYPE*)CFX_BasicArray::GetDataPtr(index);
  }

  TYPE* AddSpace() { return (TYPE*)CFX_BasicArray::InsertSpaceAt(m_nSize, 1); }

  TYPE* InsertSpaceAt(int nIndex, int nCount) {
    return (TYPE*)CFX_BasicArray::InsertSpaceAt(nIndex, nCount);
  }

  const TYPE operator[](int nIndex) const {
    if (nIndex < 0 || nIndex >= m_nSize) {
      *(volatile char*)0 = '\0';
    }
    return ((const TYPE*)m_pData)[nIndex];
  }

  TYPE& operator[](int nIndex) {
    if (nIndex < 0 || nIndex >= m_nSize) {
      *(volatile char*)0 = '\0';
    }
    return ((TYPE*)m_pData)[nIndex];
  }

  FX_BOOL InsertAt(int nIndex, TYPE newElement, int nCount = 1) {
    if (!InsertSpaceAt(nIndex, nCount)) {
      return FALSE;
    }
    while (nCount--) {
      ((TYPE*)m_pData)[nIndex++] = newElement;
    }
    return TRUE;
  }

  FX_BOOL RemoveAt(int nIndex, int nCount = 1) {
    return CFX_BasicArray::RemoveAt(nIndex, nCount);
  }

  FX_BOOL InsertAt(int nStartIndex, const CFX_BasicArray* pNewArray) {
    return CFX_BasicArray::InsertAt(nStartIndex, pNewArray);
  }

  int Find(TYPE data, int iStart = 0) const {
    if (iStart < 0) {
      return -1;
    }
    for (; iStart < (int)m_nSize; iStart++)
      if (((TYPE*)m_pData)[iStart] == data) {
        return iStart;
      }
    return -1;
  }
};

#ifdef PDF_ENABLE_XFA
typedef CFX_ArrayTemplate<CFX_WideStringC> CFX_WideStringCArray;
typedef CFX_ArrayTemplate<FX_FLOAT> CFX_FloatArray;
typedef CFX_ArrayTemplate<uint8_t> CFX_ByteArray;
typedef CFX_ArrayTemplate<int32_t> CFX_Int32Array;

template <class ObjectClass>
class CFX_ObjectArray : public CFX_BasicArray {
 public:
  CFX_ObjectArray() : CFX_BasicArray(sizeof(ObjectClass)) {}

  ~CFX_ObjectArray() { RemoveAll(); }

  void Add(const ObjectClass& data) {
    new ((void*)InsertSpaceAt(m_nSize, 1)) ObjectClass(data);
  }

  ObjectClass& Add() {
    return *(ObjectClass*)new ((void*)InsertSpaceAt(m_nSize, 1)) ObjectClass();
  }

  void* AddSpace() { return InsertSpaceAt(m_nSize, 1); }

  int32_t Append(const CFX_ObjectArray& src,
                 int32_t nStart = 0,
                 int32_t nCount = -1) {
    if (nCount == 0) {
      return 0;
    }
    int32_t nSize = src.GetSize();
    if (!nSize) {
      return 0;
    }
    ASSERT(nStart > -1 && nStart < nSize);
    if (nCount < 0) {
      nCount = nSize;
    }
    if (nStart + nCount > nSize) {
      nCount = nSize - nStart;
    }
    if (nCount < 1) {
      return 0;
    }
    nSize = m_nSize;
    InsertSpaceAt(m_nSize, nCount);
    ObjectClass* pStartObj = (ObjectClass*)GetDataPtr(nSize);
    nSize = nStart + nCount;
    for (int32_t i = nStart; i < nSize; i++, pStartObj++) {
      new ((void*)pStartObj) ObjectClass(src[i]);
    }
    return nCount;
  }

  int32_t Copy(const CFX_ObjectArray& src,
               int32_t nStart = 0,
               int32_t nCount = -1) {
    if (nCount == 0) {
      return 0;
    }
    int32_t nSize = src.GetSize();
    if (!nSize) {
      return 0;
    }
    ASSERT(nStart > -1 && nStart < nSize);
    if (nCount < 0) {
      nCount = nSize;
    }
    if (nStart + nCount > nSize) {
      nCount = nSize - nStart;
    }
    if (nCount < 1) {
      return 0;
    }
    RemoveAll();
    SetSize(nCount);
    ObjectClass* pStartObj = (ObjectClass*)m_pData;
    nSize = nStart + nCount;
    for (int32_t i = nStart; i < nSize; i++, pStartObj++) {
      new ((void*)pStartObj) ObjectClass(src[i]);
    }
    return nCount;
  }

  int GetSize() const { return m_nSize; }

  ObjectClass& operator[](int index) const {
    ASSERT(index < m_nSize);
    return *(ObjectClass*)CFX_BasicArray::GetDataPtr(index);
  }

  ObjectClass* GetDataPtr(int index) {
    return (ObjectClass*)CFX_BasicArray::GetDataPtr(index);
  }

  void RemoveAt(int index) {
    ASSERT(index < m_nSize);
    ((ObjectClass*)GetDataPtr(index))->~ObjectClass();
    CFX_BasicArray::RemoveAt(index, 1);
  }

  void RemoveAll() {
    for (int i = 0; i < m_nSize; i++) {
      ((ObjectClass*)GetDataPtr(i))->~ObjectClass();
    }
    CFX_BasicArray::SetSize(0);
  }
};
typedef CFX_ObjectArray<CFX_ByteString> CFX_ByteStringArray;
typedef CFX_ObjectArray<CFX_WideString> CFX_WideStringArray;
#endif  // PDF_ENABLE_XFA

template <class DataType, int FixedSize>
class CFX_FixedBufGrow {
 public:
  explicit CFX_FixedBufGrow(int data_size) {
    if (data_size > FixedSize) {
      m_pGrowData.reset(FX_Alloc(DataType, data_size));
      return;
    }
    FXSYS_memset(m_FixedData, 0, sizeof(DataType) * FixedSize);
  }
  operator DataType*() { return m_pGrowData ? m_pGrowData.get() : m_FixedData; }

 private:
  DataType m_FixedData[FixedSize];
  std::unique_ptr<DataType, FxFreeDeleter> m_pGrowData;
};

#ifdef PDF_ENABLE_XFA
class CFX_MapPtrToPtr {
 protected:
  struct CAssoc {
    CAssoc* pNext;
    void* key;
    void* value;
  };

 public:
  CFX_MapPtrToPtr(int nBlockSize = 10);
  ~CFX_MapPtrToPtr();

  int GetCount() const { return m_nCount; }
  bool IsEmpty() const { return m_nCount == 0; }

  FX_BOOL Lookup(void* key, void*& rValue) const;

  void* GetValueAt(void* key) const;

  void*& operator[](void* key);

  void SetAt(void* key, void* newValue) { (*this)[key] = newValue; }

  FX_BOOL RemoveKey(void* key);

  void RemoveAll();

  FX_POSITION GetStartPosition() const {
    return m_nCount == 0 ? nullptr : (FX_POSITION)-1;
  }

  void GetNextAssoc(FX_POSITION& rNextPosition,
                    void*& rKey,
                    void*& rValue) const;

  uint32_t GetHashTableSize() const { return m_nHashTableSize; }

  void InitHashTable(uint32_t hashSize, FX_BOOL bAllocNow = TRUE);

 protected:
  CAssoc** m_pHashTable;

  uint32_t m_nHashTableSize;

  int m_nCount;

  CAssoc* m_pFreeList;

  struct CFX_Plex* m_pBlocks;

  int m_nBlockSize;

  uint32_t HashKey(void* key) const;

  CAssoc* NewAssoc();

  void FreeAssoc(CAssoc* pAssoc);

  CAssoc* GetAssocAt(void* key, uint32_t& hash) const;
};

template <class KeyType, class ValueType>
class CFX_MapPtrTemplate : public CFX_MapPtrToPtr {
 public:
  CFX_MapPtrTemplate() : CFX_MapPtrToPtr(10) {}

  FX_BOOL Lookup(KeyType key, ValueType& rValue) const {
    void* pValue = nullptr;
    if (!CFX_MapPtrToPtr::Lookup((void*)(uintptr_t)key, pValue)) {
      return FALSE;
    }
    rValue = (ValueType)(uintptr_t)pValue;
    return TRUE;
  }

  ValueType& operator[](KeyType key) {
    return (ValueType&)CFX_MapPtrToPtr::operator[]((void*)(uintptr_t)key);
  }

  void SetAt(KeyType key, ValueType newValue) {
    CFX_MapPtrToPtr::SetAt((void*)(uintptr_t)key, (void*)(uintptr_t)newValue);
  }

  FX_BOOL RemoveKey(KeyType key) {
    return CFX_MapPtrToPtr::RemoveKey((void*)(uintptr_t)key);
  }

  void GetNextAssoc(FX_POSITION& rNextPosition,
                    KeyType& rKey,
                    ValueType& rValue) const {
    void* pKey = nullptr;
    void* pValue = nullptr;
    CFX_MapPtrToPtr::GetNextAssoc(rNextPosition, pKey, pValue);
    rKey = (KeyType)(uintptr_t)pKey;
    rValue = (ValueType)(uintptr_t)pValue;
  }
};
#endif  // PDF_ENABLE_XFA

class CFX_PtrList {
 protected:
  struct CNode {
    CNode* pNext;
    CNode* pPrev;
    void* data;
  };

 public:
  CFX_PtrList(int nBlockSize = 10);

  FX_POSITION GetHeadPosition() const { return (FX_POSITION)m_pNodeHead; }
  FX_POSITION GetTailPosition() const { return (FX_POSITION)m_pNodeTail; }

  void* GetNext(FX_POSITION& rPosition) const {
    CNode* pNode = (CNode*)rPosition;
    rPosition = (FX_POSITION)pNode->pNext;
    return pNode->data;
  }

  void* GetPrev(FX_POSITION& rPosition) const {
    CNode* pNode = (CNode*)rPosition;
    rPosition = (FX_POSITION)pNode->pPrev;
    return pNode->data;
  }

  FX_POSITION GetNextPosition(FX_POSITION pos) const {
    return ((CNode*)pos)->pNext;
  }

  FX_POSITION GetPrevPosition(FX_POSITION pos) const {
    return ((CNode*)pos)->pPrev;
  }

  void* GetAt(FX_POSITION rPosition) const {
    CNode* pNode = (CNode*)rPosition;
    return pNode->data;
  }

  int GetCount() const { return m_nCount; }
  FX_POSITION AddTail(void* newElement);
  FX_POSITION AddHead(void* newElement);

  void SetAt(FX_POSITION pos, void* newElement) {
    CNode* pNode = (CNode*)pos;
    pNode->data = newElement;
  }
  FX_POSITION InsertAfter(FX_POSITION pos, void* newElement);

  FX_POSITION Find(void* searchValue, FX_POSITION startAfter = nullptr) const;
  FX_POSITION FindIndex(int index) const;

  void RemoveAt(FX_POSITION pos);
  void RemoveAll();

 protected:
  CNode* m_pNodeHead;
  CNode* m_pNodeTail;
  int m_nCount;
  CNode* m_pNodeFree;
  struct CFX_Plex* m_pBlocks;
  int m_nBlockSize;

  CNode* NewNode(CNode* pPrev, CNode* pNext);
  void FreeNode(CNode* pNode);

 public:
  ~CFX_PtrList();
};

#ifdef PDF_ENABLE_XFA
typedef void (*PD_CALLBACK_FREEDATA)(void* pData);
#endif  // PDF_ENABLE_XFA

class CFX_BitStream {
 public:
  void Init(const uint8_t* pData, uint32_t dwSize);

  uint32_t GetBits(uint32_t nBits);

  void ByteAlign();

  FX_BOOL IsEOF() { return m_BitPos >= m_BitSize; }

  void SkipBits(uint32_t nBits) { m_BitPos += nBits; }

  void Rewind() { m_BitPos = 0; }

  uint32_t GetPos() const { return m_BitPos; }

  uint32_t BitsRemaining() const {
    return m_BitSize >= m_BitPos ? m_BitSize - m_BitPos : 0;
  }

 protected:
  uint32_t m_BitPos;

  uint32_t m_BitSize;

  const uint8_t* m_pData;
};

template <class ObjClass>
class CFX_CountRef {
 public:
  using Ref = CFX_CountRef<ObjClass>;

  class CountedObj : public ObjClass {
   public:
    CountedObj() {}
    CountedObj(const CountedObj& src) : ObjClass(src) {}

    int m_RefCount;
  };

  CFX_CountRef() : m_pObject(nullptr) {}
  CFX_CountRef(const Ref& ref) : m_pObject(ref.m_pObject) {
    if (m_pObject)
      m_pObject->m_RefCount++;
  }

  ~CFX_CountRef() { SetNull(); }

  ObjClass* New() {
    SetNull();
    m_pObject = new CountedObj;
    m_pObject->m_RefCount = 1;
    return m_pObject;
  }

  void operator=(const Ref& ref) {
    if (ref.m_pObject)
      ref.m_pObject->m_RefCount++;
    SetNull();
    m_pObject = ref.m_pObject;
  }

  bool IsNull() const { return !m_pObject; }
  bool NotNull() const { return !IsNull(); }

  const ObjClass* GetObject() const { return m_pObject; }
  ObjClass* GetModify() {
    if (!m_pObject) {
      m_pObject = new CountedObj;
      m_pObject->m_RefCount = 1;
    } else if (m_pObject->m_RefCount > 1) {
      m_pObject->m_RefCount--;
      CountedObj* pOldObject = m_pObject;
      m_pObject = new CountedObj(*pOldObject);
      m_pObject->m_RefCount = 1;
    }
    return m_pObject;
  }

  void SetNull() {
    if (!m_pObject) {
      return;
    }
    m_pObject->m_RefCount--;
    if (m_pObject->m_RefCount <= 0) {
      delete m_pObject;
    }
    m_pObject = nullptr;
  }

  bool operator==(const Ref& ref) const { return m_pObject == ref.m_pObject; }

 protected:
  CountedObj* m_pObject;
};

class IFX_Pause {
 public:
  virtual ~IFX_Pause() {}
  virtual FX_BOOL NeedToPauseNow() = 0;
};

template <typename T>
class CFX_AutoRestorer {
 public:
  explicit CFX_AutoRestorer(T* location)
      : m_Location(location), m_OldValue(*location) {}
  ~CFX_AutoRestorer() { *m_Location = m_OldValue; }

 private:
  T* const m_Location;
  const T m_OldValue;
};

#define FX_DATALIST_LENGTH 1024
template <size_t unit>
class CFX_SortListArray {
 protected:
  struct DataList {
    int32_t start;

    int32_t count;
    uint8_t* data;
  };

 public:
  CFX_SortListArray() : m_CurList(0) {}

  ~CFX_SortListArray() { Clear(); }

  void Clear() {
    for (int32_t i = m_DataLists.GetUpperBound(); i >= 0; i--) {
      DataList list = m_DataLists.ElementAt(i);
      FX_Free(list.data);
    }
    m_DataLists.RemoveAll();
    m_CurList = 0;
  }

  void Append(int32_t nStart, int32_t nCount) {
    if (nStart < 0) {
      return;
    }
    while (nCount > 0) {
      int32_t temp_count = std::min(nCount, FX_DATALIST_LENGTH);
      DataList list;
      list.data = FX_Alloc2D(uint8_t, temp_count, unit);
      list.start = nStart;
      list.count = temp_count;
      Append(list);
      nCount -= temp_count;
      nStart += temp_count;
    }
  }

  uint8_t* GetAt(int32_t nIndex) {
    if (nIndex < 0) {
      return nullptr;
    }
    if (m_CurList < 0 || m_CurList >= m_DataLists.GetSize()) {
      return nullptr;
    }
    DataList* pCurList = m_DataLists.GetDataPtr(m_CurList);
    if (!pCurList || nIndex < pCurList->start ||
        nIndex >= pCurList->start + pCurList->count) {
      pCurList = nullptr;
      int32_t iStart = 0;
      int32_t iEnd = m_DataLists.GetUpperBound();
      int32_t iMid = 0;
      while (iStart <= iEnd) {
        iMid = (iStart + iEnd) / 2;
        DataList* list = m_DataLists.GetDataPtr(iMid);
        if (nIndex < list->start) {
          iEnd = iMid - 1;
        } else if (nIndex >= list->start + list->count) {
          iStart = iMid + 1;
        } else {
          pCurList = list;
          m_CurList = iMid;
          break;
        }
      }
    }
    return pCurList ? pCurList->data + (nIndex - pCurList->start) * unit
                    : nullptr;
  }

 protected:
  void Append(const DataList& list) {
    int32_t iStart = 0;
    int32_t iEnd = m_DataLists.GetUpperBound();
    int32_t iFind = 0;
    while (iStart <= iEnd) {
      int32_t iMid = (iStart + iEnd) / 2;
      DataList* cur_list = m_DataLists.GetDataPtr(iMid);
      if (list.start < cur_list->start + cur_list->count) {
        iEnd = iMid - 1;
      } else {
        if (iMid == iEnd) {
          iFind = iMid + 1;
          break;
        }
        DataList* next_list = m_DataLists.GetDataPtr(iMid + 1);
        if (list.start < next_list->start) {
          iFind = iMid + 1;
          break;
        } else {
          iStart = iMid + 1;
        }
      }
    }
    m_DataLists.InsertAt(iFind, list);
  }
  int32_t m_CurList;
  CFX_ArrayTemplate<DataList> m_DataLists;
};
template <typename T1, typename T2>
class CFX_ListArrayTemplate {
 public:
  void Clear() { m_Data.Clear(); }

  void Add(int32_t nStart, int32_t nCount) { m_Data.Append(nStart, nCount); }

  T2& operator[](int32_t nIndex) {
    uint8_t* data = m_Data.GetAt(nIndex);
    ASSERT(data);
    return (T2&)(*(volatile T2*)data);
  }

  T2* GetPtrAt(int32_t nIndex) { return (T2*)m_Data.GetAt(nIndex); }

 protected:
  T1 m_Data;
};
typedef CFX_ListArrayTemplate<CFX_SortListArray<sizeof(FX_FILESIZE)>,
                              FX_FILESIZE> CFX_FileSizeListArray;

#ifdef PDF_ENABLE_XFA
class IFX_Retainable {
 public:
  virtual uint32_t Retain() = 0;
  virtual uint32_t Release() = 0;

 protected:
  virtual ~IFX_Retainable() {}
};
#define FX_IsOdd(a) ((a)&1)
#endif  // PDF_ENABLE_XFA

class CFX_Vector_3by1 {
 public:
  CFX_Vector_3by1() : a(0.0f), b(0.0f), c(0.0f) {}

  CFX_Vector_3by1(FX_FLOAT a1, FX_FLOAT b1, FX_FLOAT c1)
      : a(a1), b(b1), c(c1) {}

  FX_FLOAT a;
  FX_FLOAT b;
  FX_FLOAT c;
};
class CFX_Matrix_3by3 {
 public:
  CFX_Matrix_3by3()
      : a(0.0f),
        b(0.0f),
        c(0.0f),
        d(0.0f),
        e(0.0f),
        f(0.0f),
        g(0.0f),
        h(0.0f),
        i(0.0f) {}

  CFX_Matrix_3by3(FX_FLOAT a1,
                  FX_FLOAT b1,
                  FX_FLOAT c1,
                  FX_FLOAT d1,
                  FX_FLOAT e1,
                  FX_FLOAT f1,
                  FX_FLOAT g1,
                  FX_FLOAT h1,
                  FX_FLOAT i1)
      : a(a1), b(b1), c(c1), d(d1), e(e1), f(f1), g(g1), h(h1), i(i1) {}

  CFX_Matrix_3by3 Inverse();

  CFX_Matrix_3by3 Multiply(const CFX_Matrix_3by3& m);

  CFX_Vector_3by1 TransformVector(const CFX_Vector_3by1& v);

  FX_FLOAT a;
  FX_FLOAT b;
  FX_FLOAT c;
  FX_FLOAT d;
  FX_FLOAT e;
  FX_FLOAT f;
  FX_FLOAT g;
  FX_FLOAT h;
  FX_FLOAT i;
};

uint32_t GetBits32(const uint8_t* pData, int bitpos, int nbits);

#endif  // CORE_FXCRT_INCLUDE_FX_BASIC_H_
