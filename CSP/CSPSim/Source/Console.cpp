// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Console.cpp
 *
 **/

// TODO
//  doing a full page layout is just too damned slow.  need to
//  convert to a line-based approach, where only the last line
//  is formatted.  even then, the getTextWidth routine is really
//  inefficient.  maybe there are some font querry methods that
//  are faster?  seems osg needs an optimized fixed-font text
//  class.

#include "Console.h"

#include <string>
#include <vector>

#include <osg/StateSet>
#include <osg/BlendFunc>

#include <osgText/Text>
#include <osgText/Font>

using namespace osgConsole;



Console::Console(int x, int y, int w, int h, int border)
{
	_left = x;
	_bottom = y;
	_width = w;
	_height = h;
	_border = border;
	_cursor_enabled = true;
	_cursor_pos = 0;
	_cursor_x = 0.0;
	_cursor_y = 0.0;
	_cursor_blink_phase = 0;
	_buf_lines = 256;
	_text = new(osgText::Text);
	_token = new(osgText::Text);
	_cursor = new(osgText::Text);
	_tabstop = 4;
	_buffer.push_back(BufferLine());
	setFont("screeninfo.ttf", 20);
	setCursorCharacter('_');
	addDrawable(_text.get());
	addDrawable(_cursor.get());
}

Console::Console(const Console &copy, const osg::CopyOp &copyop)
:	osg::Geode(copy, copyop),
	_text(static_cast<osgText::Text *>(copyop(copy._text.get()))),
	_cursor(static_cast<osgText::Text *>(copyop(copy._cursor.get()))),
	_token(static_cast<osgText::Text *>(copyop(copy._token.get()))),
	_buffer(copy._buffer),
	_tabstop(copy._tabstop),
	_left(copy._left),
	_bottom(copy._bottom),
	_width(copy._width),
	_height(copy._height),
	_border(copy._border),
	_cursor_width(copy._cursor_width),
	_cursor_enabled(copy._cursor_enabled),
	_cursor_char(copy._cursor_char),
	_cursor_pos(copy._cursor_pos),
	_cursor_x(copy._cursor_x),
	_buf_lines(copy._buf_lines)
{
}

void Console::setCursorCharacter(char c) {
	_cursor_char = c;
	std::string cursor;
	cursor += c;
	_cursor->setText(cursor);
	_cursor_width = getTextWidth(cursor);
}

void Console::setFont(std::string const &font, int size) {
	_setFont(_text, font, size);
	_setFont(_token, font, size);
	_setFont(_cursor, font, size);
	setCursorCharacter(_cursor_char);
	setTabStop(_tabstop);
}

void Console::_setFont(osg::ref_ptr<osgText::Text> &text, std::string const &font, int size) {
	text->setFont(font);
	text->setFontSize(size, size);
	text->setCharacterSize(size, 1.0);
	text->setColor(osg::Vec4(1, 1, 1, 1));
	text->setAlignment(osgText::Text::LEFT_TOP);
}

void Console::setTabStop(int tabstop) {
	_tabstop = tabstop;
}
	

#if 0
void Console::drawImplementation(osg::State &state) const
{
#ifdef OSG093
	split_buffer_lines();
#endif

	// if the buffer is empty, then we add a blank line,
	// it's needed to properly draw the cursor character;
	// if the buffer is not empty, then we check whether
	// its line count is below the maximum number of
	// lines allowed (if not, cut the exceeding lines)
	if (buffer_.empty()) {
		buffer_.push_back(Buffer_line());
	} else {
		while (static_cast<int>(buffer_.size()) > buf_lines_) {
			buffer_.pop_front();
		}
	}
	
	// save depth and color buffer attributes
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_BLEND);	

	// background: first pass, with depth mask disabled

	glDepthMask(false);
	glColor4f(0.0f, 0.0f, 0.2f, 0.4f);
	glEnable(GL_BLEND);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex2f(left_, bottom_);
	glVertex2f(left_+width_, bottom_);
	glVertex2f(left_+width_, bottom_+height_);
	glVertex2f(left_, bottom_+height_);
	glEnd();

	// render the text
	
	state.apply(text_->getStateSet());	
	draw_text(state);
	state.apply(getStateSet());

	// restore GL attributes
	glPopAttrib();
}
#endif

float Console::getTextWidth(std::string const &text) const {
	_token->setText(expandTabs(text));
	osg::BoundingBox const &bbox = _token->getBound();
	return bbox.xMax() - bbox.xMin();
}

float Console::getTextWidth(char c) const {
	std::string text;
	text += c;
	return getTextWidth(text);
}

void Console::splitBufferLines() const
{
	BufferType temp_buf;
	BufferType::const_iterator i;
	for (i=_buffer.begin(); i!=_buffer.end(); ++i) {
		float line_width = 0;
		std::string new_line;
		std::string::const_iterator j;
		for (j=i->line.begin(); j!=i->line.end(); ++j) {
			line_width += getTextWidth(*j);
			if (line_width >= _width - 2*_border) {
				temp_buf.push_back(BufferLine(new_line, line_width));
				new_line.clear();
				line_width = 0;
			}
			new_line.push_back(*j);
		}
		temp_buf.push_back(BufferLine(new_line, line_width));
	}
	_buffer.swap(temp_buf);
}

void Console::eat()
{
	if (_buffer.empty()) return;

	if (!_buffer.back().line.empty()) {
		//char c = *_buffer.back().line.rbegin();
		//_buffer.back().width -= getTextWidth(c);
		_buffer.back().line.resize(_buffer.back().line.size()-1);
	} else {
		if (_buffer.size() > 1) {
			_buffer.pop_back();
			if (!_buffer.empty()) {
				eat();
			}
		}
	}
	doLayout();
}

void Console::setLine(const std::string &line) {
	_buffer.back().line = line;
	doLayout();
}

void Console::setCursor(int pos)
{
	_cursor_pos = pos;
	std::string left(_buffer.back().line, 0, pos);
	_cursor_x = getTextWidth(left);
	_cursor->setPosition(osg::Vec3(_left+_border+_cursor_x, _bottom+_height-_border-_cursor_y, 0));
}

void Console::print(char c, bool layout)
{
	if (_buffer.empty()) _buffer.push_back(BufferLine(std::string(), 0));

	if (c == '\n' || c == '\r') {
		_buffer.push_back(BufferLine(std::string(), 0));
		if (layout) doLayout();
		return;
	}

	/*
	float char_width = getTextWidth(c);
	float line_width = _buffer.back().width + char_width;
	if (line_width >= _width - 2*_border) {
		_buffer.push_back(BufferLine(std::string(), char_width));
	} else {
		_buffer.back().width += char_width;
	}
	*/

	_buffer.back().line.push_back(c);

	if (layout) doLayout();
}

void Console::doLayout()
{
//	splitBufferLines();

	float font_height = _text->getCharacterHeight();
	BufferType::size_type num_lines = (BufferType::size_type) ((_height - 2*_border) / font_height);
	if (num_lines > _buffer.size()) num_lines = _buffer.size();

	float y = 0; 

	std::string text = "";
	for (BufferType::size_type n=_buffer.size()-num_lines; n<_buffer.size(); ++n) {
		text = text + expandTabs(_buffer[n].line) + "\n";
		y += font_height;
	}
	_text->setPosition(osg::Vec3(_left + _border, _bottom + _height - _border, 0));
	_text->setText(text);

	_cursor_y = y; // - font_height;
	_cursor->setPosition(osg::Vec3(_left+_border+_cursor_x, _bottom+_height-_border-_cursor_y, 0));
}

std::string Console::expandTabs(const std::string &s) const
{
	std::string r;
	r.reserve(s.size());
	std::string::const_iterator i;
	for (i=s.begin(); i!=s.end(); ++i) {
		if (*i == '\t') {
			for (int j=0; j<_tabstop; ++j) {
				r.push_back(' ');
			}
		} else {
			r.push_back(*i);
		}
	}
	return r;
}

void Console::print(const char *s)
{
	while (*s) {
		print(*s, false);
		++s;
	}
	doLayout();
}

void Console::print(const std::string &s)
{
	print(s.c_str());
}

