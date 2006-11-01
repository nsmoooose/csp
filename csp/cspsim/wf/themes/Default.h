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
 * @file Default.h
 *
 **/

#ifndef __CSPSIM_WF_THEMES_DEFAULT_H__
#define __CSPSIM_WF_THEMES_DEFAULT_H__

#include <csp/csplib/util/Referenced.h>
#include <csp/cspsim/wf/Theme.h>

CSP_NAMESPACE

namespace wf {
namespace themes {

class Default : public Theme {
public:
	Default();
	virtual ~Default();

	virtual osg::Geometry* Default::BuildRectangle(
		float x1, float y1, float x2, float y2, float z,
		const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3, const osg::Vec4& c4) const;
//	virtual osg::Geometry* BuildRectangle(const Point& topLeft, const Point& bottomRight, float zPos, const osg::Vec4& color, bool shadow) const;
	
	virtual float getBorderWidth() const;
	virtual float getCaptionHeight() const;
	virtual std::string Default::getCaptionFont() const;

	virtual osg::Group* buildWindow(const Window& window) const;
	virtual osg::Group* buildButton(const Button& button) const;
	
private:
	virtual osg::Geometry* buildRightShadow(float x1, float y1, float x2, float y2, float z, const osg::Vec4& color, float shadowSpace, float shadowSize) const;
	virtual osg::Geometry* buildBottomShadow(float x1, float y1, float x2, float y2, float z, const osg::Vec4& color, float shadowSpace, float shadowSize) const;

    virtual float getShadowSpace() const;
    virtual float getShadowSize() const;



protected:
	std::string m_Text;
};

} // namespace themes
} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_THEMES_DEFAULT_H__

