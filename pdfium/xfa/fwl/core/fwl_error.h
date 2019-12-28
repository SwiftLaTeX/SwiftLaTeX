// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_FWL_ERROR_H_
#define XFA_FWL_CORE_FWL_ERROR_H_

enum class FWL_Error {
  Succeeded = 0,
  Indefinite,
  ParameterInvalid,
  PropertyInvalid,
  IntermediateValueInvalid,
  MethodNotSupported
};

#endif  // XFA_FWL_CORE_FWL_ERROR_H_
