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


#ifndef __F16_ALPHA_NUMERIC_DISPLAY_H__
#define __F16_ALPHA_NUMERIC_DISPLAY_H__

#include <cstdio>
#include <string>
#include <vector>
#include <SimData/Ref.h>

#if !defined(__GNUC__) && !defined(snprintf)
#define snprintf _snprintf
#endif

class AlphaNumericDisplay: public simdata::Referenced {
public:
	typedef enum { NORMAL, INVERSE } Video;
	typedef simdata::Ref<AlphaNumericDisplay> Ref;
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
	void setClean();
	void setClean(unsigned i);
	bool isDirty() const { return m_Dirty; }
	bool isDirty(unsigned i) const;
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
	bool m_Dirty;
	bool *m_DirtyLines;
};


#endif // __F16_ALPHA_NUMERIC_DISPLAY_H__

