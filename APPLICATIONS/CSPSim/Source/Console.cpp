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

#include "Console.h"

#include <string>
#include <vector>

#include <osg/StateSet>
#include <osg/BlendFunc>

#include <osgText/Text>
#include <osgText/Font>

using namespace osgConsole;


Console::Console(int x, int y, int w, int h, int border)
:	osg::Drawable(),
	text_(new osgText::Text(new osgText::BitmapFont("arial.ttf", 20))),
	tab_size_(4),
	left_(x),
	bottom_(y),
	width_(w),
	height_(h),
	border_(border),
	cursor_enabled_(true),
	cursor_char_('_'),
	cursor_pos_(0),
	cursor_x_(0.0),
	csr_blink_phase_(0),
	buf_lines_(256)
{
	setStateSet(new osg::StateSet);
	text_->setColor(osg::Vec4(1, 1, 1, 1));
	text_->setAlignment(osgText::Text::LEFT_TOP);
}

Console::Console(const Console &copy, const osg::CopyOp &copyop)
:	osg::Drawable(copy, copyop),
	text_(static_cast<osgText::Text *>(copyop(copy.text_.get()))),
	tab_size_(copy.tab_size_),
	left_(copy.left_),
	bottom_(copy.bottom_),
	width_(copy.width_),
	height_(copy.height_),
	border_(copy.border_),
	cursor_enabled_(copy.cursor_enabled_),
	cursor_char_(copy.cursor_char_),
	cursor_pos_(copy.cursor_pos_),
	cursor_x_(copy.cursor_x_),
	buffer_(copy.buffer_),
	buf_lines_(copy.buf_lines_)
{
}

void Console::drawImplementation(osg::State &state) const
{
	// if the font has not been created yet, create it
	// and split the buffer lines where they exceed the
	// maximum line width
	if (!text_->getFont()->isCreated()) {
		text_->getFont()->create(state);
		split_buffer_lines();
	}

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
	glColor4f(0, 0, 0.2, 0.4);
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

void Console::split_buffer_lines() const
{
	Buffer_type temp_buf;
	Buffer_type::const_iterator i;
	for (i=buffer_.begin(); i!=buffer_.end(); ++i) {
		float line_width = 0;
		std::string new_line;
		std::string::const_iterator j;
		for (j=i->line.begin(); j!=i->line.end(); ++j) {
			char str[2];
			str[0] = *j;
			str[1] = 0;
			osgText::EncodedText text;
			if (*j == '\t') {
				text.setText((unsigned char*)" ");
				line_width += text_->getFont()->getWidth(&text) * tab_size_;
			} else {
				text.setText((unsigned char*)str);
				line_width += text_->getFont()->getWidth(&text);
			}
			if (line_width >= width_ - 2*border_) {
				temp_buf.push_back(Buffer_line(new_line, line_width));
				new_line.clear();
				line_width = 0;
			}
			new_line.push_back(*j);
		}
		temp_buf.push_back(Buffer_line(new_line, line_width));
	}
	buffer_.swap(temp_buf);
}

void Console::eat()
{
	if (buffer_.empty()) return;

	if (!buffer_.back().line.empty()) {
		if (text_->getFont()->isCreated()) {
			char str[2];
			str[0] = *buffer_.back().line.rbegin();
			str[1] = 0;
			osgText::EncodedText text;
			if (str[0] == '\t') {
				text.setText((unsigned char*)" ");
				buffer_.back().width -= text_->getFont()->getWidth(&text) * tab_size_;
			} else {
				text.setText((unsigned char*)str);
				buffer_.back().width -= text_->getFont()->getWidth(&text);
			}
		}
		buffer_.back().line.resize(buffer_.back().line.size()-1);
	} else {
		if (buffer_.size() > 1) {
			buffer_.pop_back();
			if (!buffer_.empty()) {
				eat();
			}
		}
	}

	dirtyDisplayList();
}

void Console::setline(const std::string &line) {
	buffer_.back().line = line;
	dirtyDisplayList();
}

void Console::setCursor(int pos)
{
	cursor_pos_ = pos;
	cursor_x_ = 0;
	if (text_->getFont()->isCreated()) {
		osgText::EncodedText text;
		std::string left(buffer_.back().line, 0, pos);
		text.setText((unsigned char const *)left.c_str());
		cursor_x_ = text_->getFont()->getWidth(&text);
		dirtyDisplayList();
	} 
}

void Console::print(char c)
{
	if (buffer_.empty()) buffer_.push_back(Buffer_line(std::string(), 0));

	if (c == '\n' || c == '\r') {
		buffer_.push_back(Buffer_line(std::string(), 0));
		dirtyDisplayList();
		return;
	}

	if (text_->getFont()->isCreated()) {
		char str[2];
		str[0] = c;
		str[1] = 0;
		float char_width;
		osgText::EncodedText text;
		if (c == '\t') {
			text.setText((unsigned char*)" ");
			char_width = text_->getFont()->getWidth(&text) * tab_size_;
		} else {
			text.setText((unsigned char*)str);
			char_width = text_->getFont()->getWidth(&text);
		}
		float line_width = buffer_.back().width + char_width;
		if (line_width >= width_-2*border_) {
			buffer_.push_back(Buffer_line(std::string(), char_width));
		} else {
			buffer_.back().width += char_width;
		}
	}		

	buffer_.back().line.push_back(c);

	dirtyDisplayList();
}

void Console::draw_text(osg::State &state) const
{
	float font_height = text_->getFont()->getHeight() + 2;
	Buffer_type::size_type num_lines = (Buffer_type::size_type) ((height_ - 2*border_) / font_height);
	if (num_lines > buffer_.size()) num_lines = buffer_.size();

	float y = 0; 
	for (Buffer_type::size_type n=buffer_.size()-num_lines; n<buffer_.size(); ++n) {
		// TODO linewrap!
		//text_->setText(expand_tabs(buffer_[n].line));
		text_->setText(buffer_[n].line);
		text_->setPosition(osg::Vec3(left_ + border_, bottom_ + height_ - border_ - y, 0));
		text_->drawImplementation(state);
		y += font_height;
	}

	if (cursor_enabled_ && csr_blink_phase_ < 0.5f) {
		char csr_str[2];
		csr_str[0] = cursor_char_;
		csr_str[1] = 0;
		text_->setText(csr_str);
		osgText::EncodedText text;
		text.setText((unsigned char*)csr_str);
		float cursor_width = text_->getFont()->getWidth(&text);
		float cursor_x = cursor_x_; //buffer_.back().width;
		float cursor_y = y - font_height;
		if (cursor_x + cursor_width >= width_-border_) {
			cursor_x = 0;
			cursor_y += font_height;
		}
		text_->setPosition(osg::Vec3(left_+border_+cursor_x, bottom_+height_-border_-cursor_y, 0));
		text_->drawImplementation(state);
	}
}

std::string Console::expand_tabs(const std::string &s) const
{
	std::string r;
	std::string::const_iterator i;
	for (i=s.begin(); i!=s.end(); ++i) {
		if (*i == '\t') {
			for (int j=0; j<tab_size_; ++j) {
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
		print(*s);
		++s;
	}
}

void Console::print(const std::string &s)
{
	print(s.c_str());
}
