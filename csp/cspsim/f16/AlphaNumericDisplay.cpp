// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file AlphaNumericDisplay.cpp
 *
 **/


#include <csp/cspsim/f16/AlphaNumericDisplay.h>
#include <cassert>
#include <iostream>

namespace csp {

AlphaNumericDisplay::AlphaNumericDisplay(unsigned width, unsigned height): m_Width(width), m_Height(height), m_DirtyCount(0) {
	m_Width = width;
	m_Height = height;
	m_DirtyLineCount = new int[height];
	m_Lines = new char*[height];
	for (unsigned i = 0; i < height; ++i) {
		m_Lines[i] = new char[width + 1];
		m_DirtyLineCount[i] = 0;
	}
	clear();
}

AlphaNumericDisplay::~AlphaNumericDisplay() {
	for (unsigned i = 0; i < m_Height; ++i) delete[] m_Lines[i];
	delete[] m_Lines;
	delete[] m_DirtyLineCount;
}

void AlphaNumericDisplay::write(unsigned x, unsigned y, const char *text, Video video) {
	assert(y < m_Height);
	char *d = m_Lines[y];
	char shift = static_cast<char>((video == NORMAL) ? 0 : 0x80);
	bool changed = false;
	for (unsigned i = x; *text && i < m_Width; ++i) {
		char character = *text++ | shift;
		changed = changed || (d[i] != character);
		d[i] = character;
	}
	if (changed) {
		m_DirtyLineCount[y]++;
		m_DirtyCount++;
	}
}

void AlphaNumericDisplay::clear() {
	for (unsigned i = 0; i < m_Height; ++i) {
		for (unsigned int j = 0; j < m_Width; ++j) m_Lines[i][j] = ' ';
		m_Lines[i][m_Width] = 0;
		m_DirtyLineCount[i]++;
	}
	m_DirtyCount++;
}

void AlphaNumericDisplay::dump() const {
	for (unsigned i = 0; i < m_Height; ++i) {
		std::string line(m_Lines[i]);
		// strip inverse
		for (unsigned j=0; j < line.size(); ++j) line[j] = line[j] & '\377';
		std::cout << line << "\n";
	}
}

void AlphaNumericDisplay::dump(InfoList &info) const {
	for (unsigned i = 0; i < m_Height; ++i) {
		std::string line(m_Lines[m_Height - 1 - i]);
		// strip inverse
		for (unsigned j=0; j < line.size(); ++j) line[j] = line[j] & '\377';
		info.push_back(line);
	}
}

int AlphaNumericDisplay::getDirtyCount(unsigned i) const {
	assert(i < m_Height);
	return m_DirtyLineCount[i];
}

const char* AlphaNumericDisplay::getLine(unsigned i) const {
	assert(i < m_Height);
	return m_Lines[i];
}

} // namespace csp

