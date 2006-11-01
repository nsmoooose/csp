// Combat Simulator Project
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
 * @file ITheme.h
 *
 **/

#ifndef __CSPSIM_WF_ITHEME_H__
#define __CSPSIM_WF_ITHEME_H__

#include <csp/csplib/util/Referenced.h>

#include <osg/ref_ptr>
#include <osg/Vec4>
#include <vector>

namespace osg {
	class Group;
	class Geode;
	class Geometry;
}

CSP_NAMESPACE

namespace wf {

struct Point;
struct Size;
class Button;
class Window;

class Theme : public Referenced {
public:
	virtual osg::Group* buildWindow(const Window& window) const =0;
	virtual osg::Group* buildButton(const Button& button) const =0;

	virtual float getBorderWidth() const =0;
	virtual float getCaptionHeight() const =0;
	virtual std::string getCaptionFont() const =0;


	/*
	
	virtual osg::Geode buildButton(
		const Button& button	// The button to generate geometry for.
		)=0;
	*/
	virtual ~Theme()=0{};	
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_ITHEME_H__
