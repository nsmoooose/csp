// CSPLayout - Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
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

#include "Handle.h"

#include <osg/Geode>
#include <osg/Geometry>


osg::Geometry *Handle::makeDiamond(osg::Vec4 const &color, osg::Vec3 const &pos, float size) {
	float vv[][3] =
	{
		{   0.0,   0.0,  size * 0.8164966 },
		{  size,   0.0,   0.0 },
		{   0.0,  size,   0.0 },
		{ -size,   0.0,   0.0 },
		{   0.0, -size,   0.0 },
		{  size,   0.0,   0.0 },
		{   0.0,   0.0, -size * 0.8164966 },
		{  size,   0.0,   0.0 },
		{   0.0, -size,   0.0 },
		{ -size,   0.0,   0.0 },
		{   0.0,  size,   0.0 },
		{  size,   0.0,   0.0 },
	};

	osg::Vec3Array& v = *(new osg::Vec3Array(12));
	osg::Vec4Array& l = *(new osg::Vec4Array(1));

	l[0] = color;

	for (int i = 0; i < 12; i++ )
	{
		v[i][0] = vv[i][0] + pos.x();
		v[i][1] = vv[i][1] + pos.y();
		v[i][2] = vv[i][2] + pos.z();
	}

	osg::Geometry *geom = new osg::Geometry;
	geom->setVertexArray(&v);
	geom->setColorArray(&l);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,6));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,6,6));

	return geom;
}

osg::Geode *Handle::makeHandle(osg::Vec4 const &color, osg::Vec3 const &pos, float size) {
	osg::Geode *handle = new osg::Geode();
	handle->addDrawable(makeDiamond(color, pos, size));
	handle->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
	return handle;
}
