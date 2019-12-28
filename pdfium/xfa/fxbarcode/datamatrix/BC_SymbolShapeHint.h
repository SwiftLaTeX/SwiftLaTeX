// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_SYMBOLSHAPEHINT_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_SYMBOLSHAPEHINT_H_

class CBC_SymbolShapeHint {
 public:
  CBC_SymbolShapeHint();
  virtual ~CBC_SymbolShapeHint();

  enum SymbolShapeHint {
    FORCE_NONE,
    FORCE_SQUARE,
    FORCE_RECTANGLE,
  };
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_SYMBOLSHAPEHINT_H_
