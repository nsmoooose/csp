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
 * @file HUDText.h
 *
 * TODO These classes are not HUD specific and should be moved elsewhere.
 **/


#ifndef __HUDTEXT_H__
#define __HUDTEXT_H__

#include <string>
#include <osg/ref_ptr>
#include <osg/Referenced>

namespace osgText { class Text; }


/** An abstract base class for a font/style that can be applied to
 *  an osgText::Text element.
 */
class HUDFont: public osg::Referenced {
protected:
	virtual ~HUDFont() {}
public:
	typedef osg::ref_ptr<HUDFont> Ref;
	virtual void apply(osgText::Text *text) = 0;
	template <class C> void apply(osg::ref_ptr<C> &text) {
		apply(text.get());
	}
};


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


#endif // __HUDTEXT_H__

