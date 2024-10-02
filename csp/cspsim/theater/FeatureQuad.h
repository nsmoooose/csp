#pragma once
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
 * @file FeatureQuad.h
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/External.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector3.h>

#include <osg/ref_ptr>

namespace osg {
	class Geometry;
	class StateSet;
}

namespace csp {

/**
 * class FeatureQuad (STATIC)
 *
 * A texture painted on a single quad.
 */
class FeatureQuad: public Object {
	External m_Texture;
	float m_Width, m_Height;
	float m_OffsetX, m_OffsetY;
	bool m_Lighting;
	mutable osg::ref_ptr<osg::Geometry> m_Geometry;
	mutable osg::ref_ptr<osg::StateSet> m_StateSet;

	osg::Geometry *makeGeometry() const;
	osg::StateSet* makeStateSet() const;

public:
	CSP_DECLARE_STATIC_OBJECT(FeatureQuad)

	FeatureQuad();
	virtual ~FeatureQuad();

	float getWidth() const { return m_Width; }
	float getHeight() const { return m_Height; }
	float getOffsetX() const { return m_OffsetX; }
	float getOffsetY() const { return m_OffsetY; }
	bool getLighting() const { return m_Lighting; }

	osg::Geometry * getGeometry() const;
	osg::StateSet * getStateSet() const;

};

} // namespace csp
