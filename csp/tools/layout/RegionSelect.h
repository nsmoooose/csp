// CSPLayout
// Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US


#ifndef __CSP_LAYOUT_REGIONSELECT_H__
#define __CSP_LAYOUT_REGIONSELECT_H__

#include <osg/ref_ptr>
#include <osg/Array>
#include <osg/Group>
#include <osg/Vec3>

namespace osg { class Geometry; }

/**
 * A scene element that provides a 2D selection box.
 */
class RegionSelect: public osg::Group {
public:
	RegionSelect();

	void hide();
	void show();
	void update(float, float, float, float);

private:
	osg::ref_ptr<osg::Vec3Array> _vertex_array;
	osg::ref_ptr<osg::Geometry> _box;
};

#endif // __CSP_LAYOUT_REGIONSELECT_H__
