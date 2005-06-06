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
 * @file ChunkLodTerrain.cpp
 *
 **/


#include "ChunkLodTerrain.h"
#include "Config.h"

#include <SimCore/Util/Log.h>
#include <SimData/FileUtility.h>
#include <SimData/osg.h>

#include <osg/Geode>
#include <osg/Matrix>
#include <osg/MatrixTransform>

#include <osgChunkLod/ChunkLodDrawable>
#include <osgChunkLod/TextureQuadTree>


/**
 * TODO
 *  signal handler for screen size and fov changes
 */


SIMDATA_REGISTER_INTERFACE(ChunkLodTerrain)


// The class contains all the parameters used by the Terrain Engine.
// When the SetActive function is called the Terrain Engine will be
// Initialized and a Drawable node will be added to the scene graph.

/**
 */
ChunkLodTerrain::ChunkLodTerrain()
{
	m_Terrain = NULL;
	m_Drawable = NULL;

	m_UseLoaderThread = true;
	
	m_ChunkFile.setSource("");
	m_TextureFile.setSource("");
	m_ElevationFile.setSource("");
	m_ElevationScale = 1.0;
	
	m_Active = false;
	m_Loaded = false;

	m_LatticeWidth = 1000.0;
	m_LatticeHeight = 1000.0;

	//m_ElevationTest = new osgChunkLod::ChunkLodIntersect();
	//m_ElevationTest->setExhaustive(false);
	m_ElevationTest = new osgChunkLod::ChunkLodElevationTest();

	m_CoordinateTransform.makeRotate(osg::Z_AXIS, -osg::Y_AXIS);

	m_BaseTexelSize = 1.0;
	m_BaseScreenError = 3.0;
	//m_ElevationScale;
	m_LatticeWidth = 8000.0;
	m_LatticeHeight = 8000.0;

	// arbitrary default
	m_ScreenWidth = 1280;
	m_ScreenHeight = 1024;
}

ChunkLodTerrain::~ChunkLodTerrain()
{
	unload();
	if (m_ElevationTest) {
		delete m_ElevationTest;
		m_ElevationTest = NULL;
	}
}

void ChunkLodTerrain::unload() {
	if (m_Active) deactivate();
	if (m_Loaded) {
		if (m_Drawable.valid()) {
			m_Drawable->setChunkLodTree(NULL);
			m_Drawable = NULL;
		}
		if (m_Terrain != NULL) {
			delete m_Terrain;
			m_Terrain = NULL;
		}
		if (m_Texture != NULL) {
			delete m_Texture;
			m_Texture = NULL;
		}
		m_ElevationMap = NULL;
		m_Geode = NULL;
		m_Node = NULL;
		m_Loaded = false;
	}
}

void ChunkLodTerrain::setScreenSizeHint(int width, int height) {
	m_ScreenWidth = width;
	m_ScreenHeight = height;
	if (m_Loaded) {
		m_Terrain->setCameraParameters(width, 60.0); // XXX
	}
}

void ChunkLodTerrain::load() {
	if (m_Loaded) return;
	std::string terrain_path = getDataPath("TerrainPath");
	std::string chu_file = simdata::ospath::join(terrain_path, m_ChunkFile.getSource());
	std::string tqt_file = simdata::ospath::join(terrain_path, m_TextureFile.getSource());
	m_Texture = new osgChunkLod::TextureQuadTree(tqt_file.c_str());
	int scale = int(m_ElevationScale);  	// XXX this isn't used by ChunkLodTree currently, and probably
						// shouldn't be an int
	m_Terrain = new osgChunkLod::ChunkLodTree(chu_file.c_str(), m_Texture, m_ElevationMap.get(), scale);
	m_Terrain->loaderUseThread(m_UseLoaderThread);
	m_Terrain->setQuality(m_BaseScreenError, m_BaseTexelSize);
	m_Terrain->setCameraParameters(m_ScreenWidth, 60.0); // XXX
	m_Terrain->setLatticeDimensions(m_LatticeWidth, m_LatticeHeight);

	std::cout << "TERRAIN CREATED\n";
	m_Drawable = new osgChunkLod::ChunkLodDrawable();
	m_Drawable->setChunkLodTree(m_Terrain);
	m_Geode = new osg::Geode;
	m_Geode->addDrawable(m_Drawable.get());
	m_Node = new osg::MatrixTransform;
	m_Node->addChild(m_Geode.get());
	m_Node->setMatrix(m_CoordinateTransform);
	m_Loaded = true;
}
	

/**
 * Activate the terrain engine.
 */
void ChunkLodTerrain::activate()
{
	if (!m_Active) {
		m_Active = true;
		load();
	}
}

/**
 * Deactivate the terrain engine.
 */
void ChunkLodTerrain::deactivate()
{
	if (m_Active) {
		m_Active = false;
	}
}

bool ChunkLodTerrain::isActive() {
	return m_Active;
}


void ChunkLodTerrain::testLineOfSight(Intersection &test, IntersectionHint &hint) {
	test.reset();
	if (!m_Terrain) return;
	osgChunkLod::ChunkLodIntersect cl_test;
	cl_test.setIndex(hint);
	osg::Vec3 start = simdata::toOSG(test.getStart() - m_Origin);
	osg::Vec3 end = simdata::toOSG(test.getEnd() - m_Origin);
	cl_test.setLineSegment(new osg::LineSegment(start, end));
	m_Terrain->intersect(cl_test);
	if (cl_test.getHit()) {
		test.setHit(cl_test.getRatio(), simdata::fromOSG(cl_test.getNormal()));
		hint = cl_test.getIndex();
	}
}

float ChunkLodTerrain::getGroundElevation(double x, double y, IntersectionHint &hint) {
	if (!m_Terrain) return 0.0;
	m_ElevationTest->setIndex(hint);
	/*
	m_ElevationTest->setElevationTest(x - m_Origin.x(), -(y - m_Origin.y()));
	m_ElevationTest->setExhaustive(false);
	m_Terrain->intersect(*m_ElevationTest);
	*/
	m_ElevationTest->setPosition(x - m_Origin.x(), -(y - m_Origin.y()));
	m_Terrain->findElevation(*m_ElevationTest);
	if (m_ElevationTest->getHit()) {
		hint = m_ElevationTest->getIndex();
		return m_ElevationTest->getElevation();
	}
	return 0.0;
}

float ChunkLodTerrain::getGroundElevation(double x, double y, simdata::Vector3 &normal, IntersectionHint &hint) {
	normal = simdata::Vector3::ZAXIS;
	if (!m_Terrain) {
		return 0.0;
	}
	m_ElevationTest->setIndex(hint);
	/*
	m_ElevationTest->setElevationTest(x - m_Origin.x(), -(y - m_Origin.y()));
	m_ElevationTest->setExhaustive(false);
	m_Terrain->intersect(*m_ElevationTest);
	*/
	m_ElevationTest->setPosition(x - m_Origin.x(), -(y - m_Origin.y()));
	//simdata::SimTime t1 = simdata::SimDate::getSystemTime();
	m_Terrain->findElevation(*m_ElevationTest);
	//simdata::SimTime t2 = simdata::SimDate::getSystemTime();
	if (m_ElevationTest->getHit()) {
		osg::Vec3 norm = m_ElevationTest->getNormal();
		normal.set(norm.x(), -norm.z(), norm.y());
		if (normal.z() <= 0.0) std::cout << normal << "\n";
		assert(normal.z() > 0.0);
		/*
		static int XXX = 0; XXX++; // XXX XXX
		//if ((XXX % 30 == 0))
			std::cout << "HIT " << m_ElevationTest->getElevation() << "m @ X=" << x << ", N=" << normal << "\n";
		if (XXX > 30) ::exit(0);
		*/

		/*
		static simdata::SimTime t = 0;
		t += t2 - t1;
		static int XXX = 0; XXX++;
		if (XXX % 30 == 0) {
			std::cout << t * 1000.0 / 30.0 << "ms, index = " << m_ElevationTest->getIndex() << ", was " << hint << "\n";
			t = 0;
		}
		*/
		hint = m_ElevationTest->getIndex();
		return m_ElevationTest->getElevation();
	}
	std::cout << "NO ELEVATION DATA\n";
	return 0.0;
}


void ChunkLodTerrain::setCameraPosition(double x, double y, double z)
{
	if (m_Terrain != 0) {
		CSP_LOG(TERRAIN, DEBUG, "Terrain camera @ " << (simdata::Vector3(x,y,z)));
		// osgChunkLod terrain is in the x-z plane
		m_Terrain->setCameraPosition(x, y);
		m_Terrain->getLocalOrigin(x, y, z);
		// translate coordinates
		m_Origin.set(x, -z, y);
		CSP_LOG(TERRAIN, DEBUG, "Terrain origin @ " << (simdata::Vector3(x,y,z)));
	} else {
		m_Origin = simdata::Vector3::ZERO;
	}
}


int ChunkLodTerrain::getTerrainPolygonsRendered() const
{
	if (m_Drawable.valid()) {
		return m_Drawable->getTrianglesRendered();
	}
	return 0;
}


simdata::Vector3 ChunkLodTerrain::getOrigin(simdata::Vector3 const &) const {
	return m_Origin;
}

void ChunkLodTerrain::postCreate() {
	TerrainObject::postCreate();
}

osg::Node *ChunkLodTerrain::getNode() {
	return m_Node.get();
}

