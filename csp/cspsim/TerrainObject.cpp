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
 * @file TerrainObject.cpp
 *
 **/


#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/Projection.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/osg.h>

#include <osg/LineSegment>

namespace csp {

CSP_XML_BEGIN(TerrainObject)
	CSP_DEF("name", m_Name, true)
	CSP_DEF("version", m_Version, true)
	CSP_DEF("center", m_Center, true)
	CSP_DEF("width", m_Width, true)
	CSP_DEF("height", m_Height, true)
	CSP_DEF("shader", m_Shader, false)
CSP_XML_END


/**
 * Default constructor for the Terrain Object. Sets default values.
 */
TerrainObject::TerrainObject() : m_Width(0), m_Height(0), m_Version(0) {
}

TerrainObject::~TerrainObject() {
}

void TerrainObject::postCreate() {
	CSPLOG(INFO, TERRAIN) << "Terrain " << m_Name << " (version " << m_Version << ")";
	CSPLOG(INFO, TERRAIN) << "   center = " << getCenter().asString();
	CSPLOG(INFO, TERRAIN) << "     size = " << getWidth() << " x " << getHeight() << " m";
	SecantGnomonicProjection *projection = new SecantGnomonicProjection;
	projection->set(getCenter(), getWidth(), getHeight());
	m_Map = projection;
}

TerrainObject::Intersection::Intersection() {
	_hit = false;
	_ratio = 0.0;
}

TerrainObject::Intersection::Intersection(Vector3 const &start, Vector3 const &end) {
	_start = start;
	_end = end;
	_hit = false;
	_ratio = 0.0;
}

float TerrainObject::Intersection::getDistance() const {
	return _hit ? (_end - _start).length() * _ratio : -1.0f;
}

void TerrainObject::Intersection::setHit(float ratio, Vector3 const &normal) {
	if (_hit && ratio > _ratio) return;
	_hit = true;
	_ratio = ratio;
	_point = _start + (_end - _start) * ratio;
	_normal = normal;
}

} // namespace csp

