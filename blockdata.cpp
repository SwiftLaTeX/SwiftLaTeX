/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *   contains some code from CLedit (C) 2010 Heinz van Saanen -GPL         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "blockdata.h"


QVector<ParenthesisInfo *> BlockData::parentheses() {
	return m_parentheses;
}
QVector<LatexBlockInfo *> BlockData::latexblocks() {
	return m_latexblocks;
}

void BlockData::insertPar( ParenthesisInfo *info ) {
	int i = 0;
	while (
		i < m_parentheses.size() &&
		info->position > m_parentheses.at(i)->position )
		++i;
	m_parentheses.insert( i, info );
}

void BlockData::insertLat( LatexBlockInfo *info ) {
	int i = 0;
	while (
		i < m_latexblocks.size() &&
		info->position > m_latexblocks.at(i)->position )
		++i;
	m_latexblocks.insert( i, info );
}
