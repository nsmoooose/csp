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
 * @file TerrainObject.cpp
 *
 **/


#include "TerrainObject.h"
#include "Config.h"
#include "Log.h"

#include <osg/LineSegment>
/*//
#include "Terrain.h"
#include "DemeterDrawable.h"
#include "TerrainTextureFactory.h"
#include <osg/Geode>
#include <osg/StateSet>
extern int g_ScreenWidth;
extern int g_ScreenHeight;

*/

#include <SimData/InterfaceRegistry.h>
#include <SimData/osg.h>


///SIMDATA_REGISTER_INTERFACE(TerrainObject)


/** 
 * Default constructor for the Terrain Object. Sets default values.
 */
TerrainObject::TerrainObject()
{
	m_Width = 0;
	m_Height = 0;
	m_Version = 0;
}

TerrainObject::~TerrainObject()
{
}

void TerrainObject::serialize(simdata::Archive &archive) {
	Object::serialize(archive);
	archive(m_Name);
	archive(m_Version);
	archive(m_Center);
	archive(m_Width);
	archive(m_Height);
}

void TerrainObject::postCreate() {
	CSP_LOG(TERRAIN, INFO, "Terrain " << m_Name << " (version " << m_Version << ")");
	CSP_LOG(TERRAIN, INFO, "   center = " << getCenter().asString());
	CSP_LOG(TERRAIN, INFO, "     size = " << getWidth() << " x " << getHeight() << " m");
	m_Map.set(getCenter(), getWidth(), getHeight());
}


TerrainObject::Intersection::Intersection() {
	_hit = false;
	_ratio = 0.0;
}

TerrainObject::Intersection::Intersection(simdata::Vector3 const &start, simdata::Vector3 const &end) {
	_start = start;
	_end = end;
	_hit = false;
	_ratio = 0.0;
}

float TerrainObject::Intersection::getDistance() const { 
	return _hit ? (_end - _start).length() * _ratio : -1.0f;
}

void TerrainObject::Intersection::setHit(float ratio, simdata::Vector3 const &normal) { 
	if (_hit && ratio > _ratio) return;
	_hit = true;
	_ratio = ratio;
	_point = _start + (_end - _start) * ratio;
	_normal = normal;
}


