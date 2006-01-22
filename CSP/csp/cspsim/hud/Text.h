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
 * @file Text.h
 *
 * TODO Rename DisplayText.h
 **/


#ifndef __CSPSIM_HUD_TEXT_H__
#define __CSPSIM_HUD_TEXT_H__

#include <csp/csplib/util/Namespace.h>

#include <osg/ref_ptr>
#include <osg/Referenced>

#include <string>

namespace osgText { class Text; }
namespace osgText { class Font; }

CSP_NAMESPACE

/** An abstract base class for a font/style that can be applied to
 *  an osgText::Text element.
 */
class DisplayFont: public osg::Referenced {
protected:
	virtual ~DisplayFont() {}
public:
	typedef osg::ref_ptr<DisplayFont> RefT;
	virtual osgText::Font const *font() const = 0;
	virtual double getHeight() const = 0;
	virtual double getWidth() const { return getHeight() * 0.8; }
	virtual void apply(osgText::Text *text) = 0;
	template <class C> void apply(osg::ref_ptr<C> &text) {
		apply(text.get());
	}
};

// TODO convert LabelFormatter and subclasses to use std::string + stringprintf.

/** An abstract base class for classes that format floating point values
 *  as text.
 */
class LabelFormatter: public osg::Referenced {
public:
	virtual void format(char *buffer, int len, float value) = 0;
};


/** A LabelFormatter that uses sprintf to format a floating point value.
 */
class StandardFormatter: public LabelFormatter {
	std::string m_Format;
public:
	StandardFormatter(const char *format): m_Format(format) { }
	virtual void format(char *buffer, int len, float value);
};

CSP_NAMESPACE_END

#endif // __CSPSIM_HUD_TEXT_H__

