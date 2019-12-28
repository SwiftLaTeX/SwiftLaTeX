// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_CFWL_MESSAGE_H_
#define XFA_FWL_CORE_CFWL_MESSAGE_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_error.h"

enum class CFWL_MessageType {
  None = 0,

  Activate,
  Close,
  Cursor,
  Deactivate,
  DropFiles,
  Key,
  KillFocus,
  Mouse,
  MouseWheel,
  Post,
  SetFocus,
  Size,
  TaskClicked,
  WindowMove,
  WindowWillMove
};

enum class FWL_MouseCommand {
  LeftButtonDown,
  LeftButtonUp,
  LeftButtonDblClk,
  RightButtonDown,
  RightButtonUp,
  RightButtonDblClk,
  MiddleButtonDown,
  MiddleButtonUp,
  MiddleButtonDblClk,
  Move,
  Enter,
  Leave,
  Hover
};

enum class FWL_KeyCommand { KeyDown, KeyUp, Char };

class IFWL_Widget;

class CFWL_Message {
 public:
  CFWL_Message();
  virtual ~CFWL_Message();

  virtual CFWL_Message* Clone();
  virtual FWL_Error GetClassName(CFX_WideString& wsClass) const;
  virtual CFWL_MessageType GetClassID() const;

  uint32_t Release();
  CFWL_Message* Retain();

  IFWL_Widget* m_pSrcTarget;
  IFWL_Widget* m_pDstTarget;
  uint32_t m_dwExtend;

 private:
  uint32_t m_dwRefCount;
};

inline CFWL_Message::CFWL_Message()
    : m_pSrcTarget(nullptr),
      m_pDstTarget(nullptr),
      m_dwExtend(0),
      m_dwRefCount(1) {}

inline CFWL_Message::~CFWL_Message() {}

inline CFWL_Message* CFWL_Message::Clone() {
  return nullptr;
}

inline FWL_Error CFWL_Message::GetClassName(CFX_WideString& wsClass) const {
  return FWL_Error::Succeeded;
}

inline CFWL_MessageType CFWL_Message::GetClassID() const {
  return CFWL_MessageType::None;
}

inline uint32_t CFWL_Message::Release() {
  m_dwRefCount--;
  uint32_t dwRefCount = m_dwRefCount;
  if (!m_dwRefCount)
    delete this;
  return dwRefCount;
}

inline CFWL_Message* CFWL_Message::Retain() {
  m_dwRefCount++;
  return this;
}

#define FWL_MESSAGE_CLASS_DEF(classname, msgType, ...)              \
  class classname : public CFWL_Message {                           \
   public:                                                          \
    classname();                                                    \
    ~classname() override;                                          \
    CFWL_Message* Clone() override;                                 \
    FWL_Error GetClassName(CFX_WideString& wsClass) const override; \
    CFWL_MessageType GetClassID() const override;                   \
    __VA_ARGS__                                                     \
  };

#define FWL_MESSAGE_FUNCTION_DEF(classname, msgType, ...)                   \
  inline classname::classname() {}                                          \
  inline classname::~classname() {}                                         \
  inline CFWL_Message* classname::Clone() { return new classname(*this); }  \
  inline FWL_Error classname::GetClassName(CFX_WideString& wsClass) const { \
    wsClass = L## #classname;                                               \
    return FWL_Error::Succeeded;                                            \
  }                                                                         \
  inline CFWL_MessageType classname::GetClassID() const { return msgType; } \
  __VA_ARGS__

#define FWL_MESSAGE_DEF(classname, msgType, ...)         \
  FWL_MESSAGE_CLASS_DEF(classname, msgType, __VA_ARGS__) \
  FWL_MESSAGE_FUNCTION_DEF(classname, msgType)

FWL_MESSAGE_DEF(CFWL_MsgActivate, CFWL_MessageType::Activate)

FWL_MESSAGE_DEF(CFWL_MsgDeactivate, CFWL_MessageType::Deactivate)

FWL_MESSAGE_DEF(CFWL_MsgMouse, CFWL_MessageType::Mouse, FX_FLOAT m_fx;
                FX_FLOAT m_fy;
                uint32_t m_dwFlags;
                FWL_MouseCommand m_dwCmd;)

FWL_MESSAGE_DEF(CFWL_MsgMouseWheel, CFWL_MessageType::MouseWheel, FX_FLOAT m_fx;
                FX_FLOAT m_fy;
                FX_FLOAT m_fDeltaX;
                FX_FLOAT m_fDeltaY;
                uint32_t m_dwFlags;)

FWL_MESSAGE_DEF(CFWL_MsgSetFocus,
                CFWL_MessageType::SetFocus,
                IFWL_Widget* m_pKillFocus;)

FWL_MESSAGE_DEF(CFWL_MsgKillFocus,
                CFWL_MessageType::KillFocus,
                IFWL_Widget* m_pSetFocus;)

FWL_MESSAGE_DEF(CFWL_MsgKey, CFWL_MessageType::Key, uint32_t m_dwKeyCode;
                uint32_t m_dwFlags;
                FWL_KeyCommand m_dwCmd;)

FWL_MESSAGE_DEF(CFWL_MsgCursor, CFWL_MessageType::Cursor)

FWL_MESSAGE_DEF(CFWL_MsgSize, CFWL_MessageType::Size, int32_t m_iWidth;
                int32_t m_iHeight;)

FWL_MESSAGE_DEF(CFWL_MsgWindowMove, CFWL_MessageType::WindowMove, FX_FLOAT m_fx;
                FX_FLOAT m_fy;)

FWL_MESSAGE_CLASS_DEF(CFWL_MsgDropFiles,
                      CFWL_MessageType::DropFiles,
                      CFWL_MsgDropFiles(const CFWL_MsgDropFiles& copy);
                      FX_FLOAT m_fx;
                      FX_FLOAT m_fy;
                      CFX_WideStringArray m_files;)
FWL_MESSAGE_FUNCTION_DEF(
    CFWL_MsgDropFiles,
    CFWL_MessageType::DropFiles,
    inline CFWL_MsgDropFiles::CFWL_MsgDropFiles(const CFWL_MsgDropFiles& copy) {
      m_pDstTarget = copy.m_pDstTarget;
      m_pSrcTarget = copy.m_pSrcTarget;
      m_fx = copy.m_fx;
      m_fy = copy.m_fy;
      m_files.Append(copy.m_files);
    })

FWL_MESSAGE_DEF(CFWL_MsgTaskClicked,
                CFWL_MessageType::TaskClicked,
                FX_FLOAT m_fx;
                FX_FLOAT m_fy;)

FWL_MESSAGE_DEF(CFWL_MsgClose, CFWL_MessageType::Close)

FWL_MESSAGE_DEF(CFWL_MsgWindowWillMove, CFWL_MessageType::WindowWillMove)

#endif  // XFA_FWL_CORE_CFWL_MESSAGE_H_
