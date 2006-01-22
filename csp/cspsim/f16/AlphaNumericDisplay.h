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
 * @file AlphaNumericDisplay.h
 *
 **/


#ifndef __CSPSIM_F16_ALPHA_NUMERIC_DISPLAY_H__
#define __CSPSIM_F16_ALPHA_NUMERIC_DISPLAY_H__

#include <csp/csplib/util/Ref.h>
#include <cstdio>
#include <string>
#include <vector>

#if !defined(__GNUC__) && !defined(snprintf)
#define snprintf _snprintf
#endif

CSP_NAMESPACE

class AlphaNumericDisplay: public Referenced {
public:
	typedef Ref<AlphaNumericDisplay> RefT;
	typedef enum { NORMAL, INVERSE } Video;
	AlphaNumericDisplay(unsigned width, unsigned height);
	~AlphaNumericDisplay();
	template <typename T>
	void write(unsigned x, unsigned y, const char *format, T const &value, Video mode=NORMAL) {
		char buffer[40];
		snprintf(buffer, sizeof(buffer), format, value);
		write(x, y, buffer, mode);
	}
	void write(unsigned x, unsigned y, const char *text, Video mode=NORMAL);
	void write(unsigned x, unsigned y, std::string const &text, Video mode=NORMAL) { write(x, y, text.c_str(), mode); }
	int getDirtyCount() const { return m_DirtyCount; }
	int getDirtyCount(unsigned i) const;
	void clear();
	void dump() const;

	typedef std::vector<std::string> InfoList;
	void dump(InfoList &info) const;

	unsigned width() const { return m_Width; }
	unsigned height() const { return m_Height; }
	const char* getLine(unsigned i) const;

private:
	unsigned m_Width;
	unsigned m_Height;
	char **m_Lines;
	int m_DirtyCount;
	int *m_DirtyLineCount;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_F16_ALPHA_NUMERIC_DISPLAY_H__

