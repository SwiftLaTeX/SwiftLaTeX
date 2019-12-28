/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TEXTBLOCKSELECTION_H
#define TEXTBLOCKSELECTION_H

#include <QTextCursor>
#include <QTextBlock>

class TextBlockSelection
{
public:

    bool isValid() const{ return !firstBlock.isNull() && !lastBlock.isNull(); }
    void clear() { firstBlock = lastBlock = QTextCursor(); }

    QTextCursor firstBlock; // defines the first block
    QTextCursor lastBlock; // defines the last block
    int firstVisualColumn; // defines the first visual column of the selection
    int lastVisualColumn; // defines the last visual column of the selection
    enum Anchor {TopLeft = 0, TopRight, BottomLeft, BottomRight} anchor;
    TextBlockSelection():firstVisualColumn(0), lastVisualColumn(0), anchor(BottomRight){}
    int firstNonSpace(const QString &text) const
    {
    int i = 0;
    while (i < text.size()) {
        if (!text.at(i).isSpace())
            return i;
        ++i;
    }
    return i;
    }
    bool cursorIsAtBeginningOfLine(const QTextCursor &cursor) const
    {
    QString text = cursor.block().text();
    int fns = firstNonSpace(text);
    return (cursor.position() - cursor.block().position() <= fns);
    }
    void moveAnchor(int blockNumber, int visualColumn)
      {
      if (visualColumn >= 0) {
	  if (anchor % 2) {
	      lastVisualColumn = visualColumn;
	      if (lastVisualColumn < firstVisualColumn) {
		  qSwap(firstVisualColumn, lastVisualColumn);
		  anchor = (Anchor) (anchor - 1);
	      }
	  } else {
	      firstVisualColumn = visualColumn;
	      if (firstVisualColumn > lastVisualColumn) {
		  qSwap(firstVisualColumn, lastVisualColumn);
		  anchor = (Anchor) (anchor + 1);
	      }
	  }
      }

      if (blockNumber >= 0 && blockNumber < firstBlock.document()->blockCount()) {
	  if (anchor <= TopRight) {
	      firstBlock.setPosition(firstBlock.document()->findBlockByNumber(blockNumber).position());
	      if (firstBlock.blockNumber() > lastBlock.blockNumber()) {
		  qSwap(firstBlock, lastBlock);
		  anchor = (Anchor) (anchor + 2);
	      }
	  } else {
	      lastBlock.setPosition(firstBlock.document()->findBlockByNumber(blockNumber).position());
	      if (lastBlock.blockNumber() < firstBlock.blockNumber()) {
		  qSwap(firstBlock, lastBlock);
		  anchor = (Anchor) (anchor - 2);
	      }
	  }
      }
      firstBlock.movePosition(QTextCursor::StartOfBlock);
      lastBlock.movePosition(QTextCursor::EndOfBlock);
      }
    inline int anchorColumnNumber() const { return (anchor % 2) ? lastVisualColumn : firstVisualColumn; }
    inline int anchorBlockNumber() const {
        return (anchor <= TopRight ? firstBlock.blockNumber() : lastBlock.blockNumber()); }
        
    int tabSize;    
    int columnAt(const QString &text, int position) const
      {
	  int column = 0;
	  for (int i = 0; i < position; ++i) {
	      if (text.at(i) == QLatin1Char('\t'))
		  column = column - (column % tabSize) + tabSize;
	      else
		  ++column;
	  }
	  return column;
      }    
    int positionAtColumn(const QString &text, int column, int *offset) const
      {
	  int col = 0;
	  int i = 0;
	  while (i < text.size() && col < column) {
	      if (text.at(i) == QLatin1Char('\t'))
		  col = col - (col % tabSize) + tabSize;
	      else
		  ++col;
	      ++i;
	  }
	  if (offset)
	      *offset = column - col;
	  return i;
      }
    QTextCursor selection() const
      {
      QTextCursor cursor = firstBlock;
      if (anchor <= TopRight) {
	  cursor.setPosition(lastBlock.block().position() + positionAtColumn(lastBlock.block().text(), lastVisualColumn,0));
	  cursor.setPosition(firstBlock.block().position() + positionAtColumn(firstBlock.block().text(), firstVisualColumn,0),
			    QTextCursor::KeepAnchor);
      } else {
	  cursor.setPosition(firstBlock.block().position() + positionAtColumn(firstBlock.block().text(), firstVisualColumn,0));
	  cursor.setPosition(lastBlock.block().position() + positionAtColumn(lastBlock.block().text(), lastVisualColumn,0),
			    QTextCursor::KeepAnchor);
      }
      return cursor;      
      }
    void fromSelection(const QTextCursor &selection)
      {
      firstBlock = selection;
      firstBlock.setPosition(selection.selectionStart());
      firstVisualColumn = columnAt(firstBlock.block().text(), firstBlock.positionInBlock());
      lastBlock = selection;
      lastBlock.setPosition(selection.selectionEnd());
      lastVisualColumn = columnAt(lastBlock.block().text(), lastBlock.positionInBlock());
      if (selection.anchor() > selection.position())
	  anchor = TopLeft;
      else
	  anchor = BottomRight;

      firstBlock.movePosition(QTextCursor::StartOfBlock);
      lastBlock.movePosition(QTextCursor::EndOfBlock);      
      }
};

#endif
