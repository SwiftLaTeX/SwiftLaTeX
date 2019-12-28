// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_box.h"

#include "xfa/fxfa/parser/cxfa_corner.h"
#include "xfa/fxfa/parser/xfa_object.h"

namespace {

void GetStrokesInternal(CXFA_Node* pNode,
                        CXFA_StrokeArray& strokes,
                        FX_BOOL bNull) {
  strokes.RemoveAll();
  if (!pNode)
    return;

  strokes.SetSize(8);
  int32_t i, j;
  for (i = 0, j = 0; i < 4; i++) {
    CXFA_Corner corner =
        CXFA_Corner(pNode->GetProperty(i, XFA_Element::Corner, i == 0));
    if (corner || i == 0)
      strokes.SetAt(j, corner);
    else if (bNull)
      strokes.SetAt(j, CXFA_Stroke(nullptr));
    else if (i == 1)
      strokes.SetAt(j, strokes[0]);
    else if (i == 2)
      strokes.SetAt(j, strokes[0]);
    else
      strokes.SetAt(j, strokes[2]);

    j++;
    CXFA_Edge edge =
        CXFA_Edge(pNode->GetProperty(i, XFA_Element::Edge, i == 0));
    if (edge || i == 0)
      strokes.SetAt(j, edge);
    else if (bNull)
      strokes.SetAt(j, CXFA_Stroke(nullptr));
    else if (i == 1)
      strokes.SetAt(j, strokes[1]);
    else if (i == 2)
      strokes.SetAt(j, strokes[1]);
    else
      strokes.SetAt(j, strokes[3]);

    j++;
  }
}

static int32_t Style3D(const CXFA_StrokeArray& strokes, CXFA_Stroke& stroke) {
  int32_t iCount = strokes.GetSize();
  if (iCount < 1)
    return 0;

  stroke = strokes[0];
  for (int32_t i = 1; i < iCount; i++) {
    CXFA_Stroke find = strokes[i];
    if (!find)
      continue;

    if (!stroke)
      stroke = find;
    else if (stroke.GetStrokeType() != find.GetStrokeType())
      stroke = find;
    break;
  }
  int32_t iType = stroke.GetStrokeType();
  if (iType == XFA_ATTRIBUTEENUM_Lowered || iType == XFA_ATTRIBUTEENUM_Raised ||
      iType == XFA_ATTRIBUTEENUM_Etched ||
      iType == XFA_ATTRIBUTEENUM_Embossed) {
    return iType;
  }
  return 0;
}

}  // namespace

int32_t CXFA_Box::GetHand() const {
  if (!m_pNode)
    return XFA_ATTRIBUTEENUM_Even;
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Hand);
}

int32_t CXFA_Box::GetPresence() const {
  if (!m_pNode)
    return XFA_ATTRIBUTEENUM_Hidden;
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Presence);
}

int32_t CXFA_Box::CountEdges() const {
  if (!m_pNode)
    return 0;
  return m_pNode->CountChildren(XFA_Element::Edge);
}

CXFA_Edge CXFA_Box::GetEdge(int32_t nIndex) const {
  return CXFA_Edge(
      m_pNode ? m_pNode->GetProperty(nIndex, XFA_Element::Edge, nIndex == 0)
              : nullptr);
}

void CXFA_Box::GetStrokes(CXFA_StrokeArray& strokes) const {
  GetStrokesInternal(m_pNode, strokes, FALSE);
}

FX_BOOL CXFA_Box::IsCircular() const {
  if (!m_pNode)
    return FALSE;
  return m_pNode->GetBoolean(XFA_ATTRIBUTE_Circular);
}

FX_BOOL CXFA_Box::GetStartAngle(FX_FLOAT& fStartAngle) const {
  fStartAngle = 0;
  if (!m_pNode)
    return FALSE;

  CXFA_Measurement ms;
  FX_BOOL bRet = m_pNode->TryMeasure(XFA_ATTRIBUTE_StartAngle, ms, FALSE);
  if (bRet)
    fStartAngle = ms.GetValue();

  return bRet;
}

FX_BOOL CXFA_Box::GetSweepAngle(FX_FLOAT& fSweepAngle) const {
  fSweepAngle = 360;
  if (!m_pNode)
    return FALSE;

  CXFA_Measurement ms;
  FX_BOOL bRet = m_pNode->TryMeasure(XFA_ATTRIBUTE_SweepAngle, ms, FALSE);
  if (bRet)
    fSweepAngle = ms.GetValue();

  return bRet;
}

CXFA_Fill CXFA_Box::GetFill(FX_BOOL bModified) const {
  if (!m_pNode)
    return CXFA_Fill(nullptr);

  CXFA_Node* pFillNode = m_pNode->GetProperty(0, XFA_Element::Fill, bModified);
  return CXFA_Fill(pFillNode);
}

CXFA_Margin CXFA_Box::GetMargin() const {
  return CXFA_Margin(m_pNode ? m_pNode->GetChild(0, XFA_Element::Margin)
                             : nullptr);
}

int32_t CXFA_Box::Get3DStyle(FX_BOOL& bVisible, FX_FLOAT& fThickness) const {
  if (IsArc())
    return 0;

  CXFA_StrokeArray strokes;
  GetStrokesInternal(m_pNode, strokes, TRUE);
  CXFA_Stroke stroke(nullptr);
  int32_t iType = Style3D(strokes, stroke);
  if (iType) {
    bVisible = stroke.IsVisible();
    fThickness = stroke.GetThickness();
  }
  return iType;
}
