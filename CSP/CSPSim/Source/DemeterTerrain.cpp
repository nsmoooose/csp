// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002, 2004 The Combat Simulator Project
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
 * @file DemeterTerrain.cpp
 *
 **/


#include "DemeterTerrain.h"
#include "Config.h"
#include "Log.h"

#include <Demeter/Terrain.h>
#include <Demeter/DemeterDrawable.h>
#include <Demeter/TerrainTextureFactory.h>

#include <SimData/InterfaceRegistry.h>

#include <osg/Geode>
#include <osg/StateSet>


extern int g_ScreenWidth;
extern int g_ScreenHeight;


SIMDATA_REGISTER_INTERFACE(DemeterTerrain)


// Terrain Object is a wrapper class for the Demeter terrain engine
// and DemeterDrawable OSG class.

// The class contains all the parameters used by the Terrain Engine.
// When the SetActive function is called the Terrain Engine will be
// Initialized and a Drawable node will be added to the scene graph.

/**
 * Default constructor for the Terrain Object. Sets default values.
 */
DemeterTerrain::DemeterTerrain()
{
	CSP_LOG(TERRAIN, DEBUG, "DemeterTerrain::DemeterTerrain" );
	m_TerrainLattice = NULL;
	m_Terrain = NULL;
	m_TerrainTextureFactory = NULL;

	m_DynamicTextures = false;
	m_TextureCompression = true;
	m_PreloadTextures = true;
	m_TextureFactory = false;
	m_MaxTriangles = 50000;
	
	m_DetailTextureFile.setSource("");
	m_TextureFile.setSource("");
	m_ElevationFile.setSource("");
	
	m_VertexSpacing = 1.0;
	m_VertexHeight = 1.0;
	m_Lattice = false;
	m_Active = false;
	m_Loaded = false;
	m_DetailThreshold = 12.0;

	m_LatticeBaseName = "";
	m_LatticeElevExt = "";
	m_LatticeTexExt = "";
	m_LatticeWidth = 0;
	m_LatticeHeight = 0;
}

DemeterTerrain::~DemeterTerrain()
{
	unload();
}

void DemeterTerrain::unload() {
	if (m_Active) deactivate();
	if (m_Loaded) {
		m_TerrainLattice = NULL;
		m_TerrainNode = NULL;
		m_TerrainLattice = NULL;
		m_Terrain = NULL;
		if (m_TerrainTextureFactory) {
			delete m_TerrainTextureFactory;
			m_TerrainTextureFactory = NULL;
		}
		m_Loaded = false;
	}
}

void DemeterTerrain::load() {
	if (m_Loaded) return;
	std::string terrain_path = getDataPath("TerrainPath");
	if (m_Lattice)
	{
		// create lattice
		Demeter::Settings::GetInstance()->SetMediaPath(terrain_path.c_str());
		createTerrainLattice();
		m_TerrainNode = createTerrainLatticeNode(m_TerrainLattice.get());
		m_TerrainNode->setName("TerrainLatticeNode");
	}
	else
	{
		// create single terrain node
		Demeter::Settings::GetInstance()->SetMediaPath(terrain_path.c_str());
		createTerrain();
		m_TerrainNode = createTerrainNode(m_Terrain.get());
		m_TerrainNode->setName("TerrainNode");
	}
	m_Loaded = true;
}
	

/**
 * Activate the terrain engine.
 */
void DemeterTerrain::activate()
{
	if (!m_Active) {
		m_Active = true;
		load();
	}
}

/**
 * Deactivate the terrain engine.
 */
void DemeterTerrain::deactivate()
{
	if (m_Active) {
		m_Active = false;
	}
}

bool DemeterTerrain::isActive() {
	return m_Active;
}


void DemeterTerrain::testLineOfSight(Intersection &test, IntersectionHint &hint) {
	// FIXME not implemented
	return;
}

float DemeterTerrain::getGroundElevation(double x, double y, IntersectionHint &hint) {
	x += m_Offset.x();
	y += m_Offset.y();
	float z = 0.0;
	if (!m_Lattice) {
		if (m_Terrain.valid()) {
			z = m_Terrain->GetElevation(x, y);
		}
	} else {
		if (m_TerrainLattice.valid()) {
			z = m_TerrainLattice->GetElevation(x, y);
		}
	}
	return z;
}

float DemeterTerrain::getGroundElevation(double x, double y, simdata::Vector3 &normal, IntersectionHint &hint) {
	x += m_Offset.x();
	y += m_Offset.y();
	float z = 0.0;
	float zx = 0.0, zy = 0.0;
	if (!m_Lattice) {
		if (m_Terrain.valid()) {
			z = m_Terrain->GetElevation(x, y);
			zx = m_Terrain->GetElevation(x+1.0, y+0.0) - z;
			zy = m_Terrain->GetElevation(x+0.0, y+1.0) - z;
			/* XXX broken
			float normalX, normalY, normalZ;
			m_Terrain->GetNormal(x, y, normalX, normalY, normalZ);
			normal->set(normalX, normalY, normalZ);
			*/
		}
	} else {
		if (m_TerrainLattice.valid()) {
			z = m_TerrainLattice->GetElevation(x, y);
			zx = m_TerrainLattice->GetElevation(x+1.0, y+0.0) - z;
			zy = m_TerrainLattice->GetElevation(x+0.0, y+1.0) - z;
			/*
			float normalX, normalY, normalZ;
			m_TerrainLattice->GetNormal(x, y, normalX, normalY, normalZ);
			normal->set(normalX, normalY, normalZ);
			*/
		}
	}
	// poor man's normal
	normal.set(-zx, -zy, 1.0);
	normal.normalize();
	return z;
}


int DemeterTerrain::createTerrain()
{
	CSP_LOG(TERRAIN, DEBUG, "DemeterTerrain::createTerrain() " );
	updateDemeterSettings();
	if (!m_TextureFactory) {
		if (m_DetailTextureFile.getSource() == "") {
			m_Terrain = new Demeter::Terrain(m_ElevationFile.getSource().c_str(),m_TextureFile.getSource().c_str(),NULL,
	   		m_VertexSpacing,m_VertexHeight,m_MaxTriangles);
		} else {
			m_Terrain = new Demeter::Terrain(m_ElevationFile.getSource().c_str(),m_TextureFile.getSource().c_str(),
			m_DetailTextureFile.getSource().c_str(),m_VertexSpacing,m_VertexHeight,m_MaxTriangles);
		}
	} else {
		if (m_DetailTextureFile.getSource() == "") {
			m_Terrain = new Demeter::Terrain(m_ElevationFile.getSource().c_str(),
			                                  NULL,
			                                  NULL, 
			                                  m_VertexSpacing,
			                                  m_VertexHeight,
			                                  m_MaxTriangles, 
			                                  false, 0, 0, 4,4);
		} else {
			m_Terrain = new Demeter::Terrain(m_ElevationFile.getSource().c_str(), 
			                                  NULL,
			                                  m_DetailTextureFile.getSource().c_str(),
			                                  m_VertexSpacing,
			                                  m_VertexHeight,
			                                  m_MaxTriangles, 
			                                  false, 0, 0, 4,4);
		}
		// just to catch your attention ;-)  it may be ok to just delete any
		// pre-existing terraintexturefactory.
		assert(!m_TerrainTextureFactory);
		m_TerrainTextureFactory = new Demeter::TerrainTextureFactory();
		m_TerrainTextureFactory->SetTerrain(m_Terrain.get());
		m_Terrain->SetTextureFactory(m_TerrainTextureFactory);
	}
	m_Terrain->SetDetailThreshold(m_DetailThreshold);
	assert(m_Terrain->GetWidth() == m_Width);
	assert(m_Terrain->GetHeight() == m_Height);
	return 1;
}


int DemeterTerrain::createTerrainLattice()
{

	CSP_LOG(TERRAIN, DEBUG, "DemeterTerrain::createTerrainLattice()..." );

	//float detailThreshold = Config.GetFloat("TerraindetailThreshold");
    
	updateDemeterSettings();
	
	//char terrainTextureDetail[256];
//	Config.GetString(terrainTextureDetail, "TerrainTextureDetail");

//	if (m_DetailTextureFile == "")
//	{
//		m_TerrainLattice = new Demeter::TerrainLattice(m_LatticeBaseName.c_str(),
//			m_LatticeElevExt.c_str(), m_LatticeTexExt.c_str(),		
//			NULL, m_VertexSpacing, m_VertexHeight, m_MaxTriangles,
//			false, m_LatticeWidth, m_LatticeHeight);
//	}
//	else
//	{
//		m_TerrainLattice = new Demeter::TerrainLattice(m_LatticeBaseName.c_str(),
//			m_LatticeElevExt.c_str(), m_LatticeTexExt.c_str(),		
//			m_DetailTextureFile.c_str(), m_VertexSpacing, m_VertexHeight, m_MaxTriangles,
//			false, m_LatticeWidth, m_LatticeHeight);
//
//	}


	m_TerrainLattice = new Demeter::TerrainLattice(m_LatticeBaseName.c_str(),
			m_LatticeElevExt.c_str(), /*m_LatticeTexExt.c_str() */ NULL,		
			m_DetailTextureFile.getSource().c_str(), m_VertexSpacing, m_VertexHeight, m_MaxTriangles,
			//NULL, m_VertexSpacing, m_VertexHeight, m_MaxTriangles,
			true, m_LatticeWidth, m_LatticeHeight);
	
	// just to catch your attention ;-)  it may be ok to just delete any
	// pre-existing terraintexturefactory.
	assert(!m_TerrainTextureFactory);
	m_TerrainTextureFactory = new Demeter::TerrainTextureFactory();
	m_TerrainTextureFactory->SetTerrainLattice(m_TerrainLattice.get());
	m_TerrainLattice->SetTextureFactory(m_TerrainTextureFactory, 2, 2);

	m_TerrainLattice->SetDetailThreshold(m_DetailThreshold);
	CSP_LOG(TERRAIN, DEBUG, "Terrain size: " << m_TerrainLattice->GetWidth() << "(" << m_Width << "), " <<
						    m_TerrainLattice->GetHeight() << "(" << m_Height << ")");
	assert(m_TerrainLattice->GetWidth() == m_Width);
	assert(m_TerrainLattice->GetHeight() == m_Height);
  
  	// XXX it appears to be critical that the camera position be setup correctly
	// here at the start.  doing so afterward can result in very strange texture
	// anomalies in demeter.   don't know why, but it is a problem in general
	// since we don't know exactly where the sim will start at this point!
	//m_TerrainLattice->SetCameraPosition(512000, 512000, 3000);///?
	//m_TerrainLattice->SetCameraPosition(374790, 397450, 1000);///?

	CSP_LOG(TERRAIN, DEBUG, " ...DemeterTerrain::createTerrainLattice()");

	return 1;
}

void DemeterTerrain::setCameraPosition(double x, double y, double z)
{
	if (m_TerrainLattice.valid()) {
		CSP_LOG(TERRAIN, DEBUG, "Terrain camera @ " << (m_Offset + simdata::Vector3(x,y,z)));
		m_TerrainLattice->SetCameraPosition(x+m_Offset.x(), y+m_Offset.y(), z+m_Offset.z());
	}
}


osg::Node* DemeterTerrain::createTerrainLatticeNode(Demeter::TerrainLattice* pTerrainLattice)
{
	CSP_LOG(TERRAIN, INFO, "DemeterTerrain::createTerrainLatticeNode" );
    
	Demeter::DemeterLatticeDrawable* pLatticeDrawable = NULL;
	osg::Geode* pGeode = NULL;

	try {
//		osg::Depth* depth = new osg::Depth;
//		depth->setFunction(osg::Depth::LEQUAL);
//		osg::StateSet *dstate = new osg::StateSet;
//		dstate->setAttributeAndModes(depth,osg::StateAttribute::ON );

		pLatticeDrawable = new Demeter::DemeterLatticeDrawable;
		pLatticeDrawable->SetTerrainLattice(pTerrainLattice);

//		pLatticeDrawable->setStateSet( dstate );

		pGeode = new osg::Geode;
		pGeode->addDrawable(pLatticeDrawable);
	}
	catch(...) {
		CSP_LOG(TERRAIN, ERROR, "Caught Exception in DemeterTerrain::createTerrainLatticeNode");
	}
    
    return pGeode;
}


osg::Node* DemeterTerrain::createTerrainNode(Demeter::Terrain* pTerrain)
{
	CSP_LOG(TERRAIN, INFO, "DemeterTerrain::createTerrainNode" );

	osg::Geode* pGeode = NULL;

	try {
		Demeter::DemeterDrawable* pDrawable = new Demeter::DemeterDrawable;
		pDrawable->SetTerrain(pTerrain);
		pGeode = new osg::Geode;
		pGeode->addDrawable(pDrawable);
	}
	catch(...) {
		CSP_LOG(TERRAIN, ERROR, "Caught Exception in DemeterTerrain::createTerrainNode");
	}

	return pGeode;
}

int DemeterTerrain::getTerrainPolygonsRendered() const
{
	if (m_TerrainLattice.valid()) {
		return m_TerrainLattice->GetLatticePolygonsRendered();
	} else if (m_Terrain.valid()) {
		return m_Terrain->GetTerrainPolygonsRendered();
	} else {
		return 0;
	}
}


void DemeterTerrain::updateDemeterSettings()
{
	std::string terrain_path = getDataPath("TerrainPath");
	bool verbose = g_Config.getBool("Debug", "Demeter", false, true);
	Demeter::Settings::GetInstance()->SetMediaPath(terrain_path.c_str());
	Demeter::Settings::GetInstance()->SetVerbose(verbose);
	Demeter::Settings::GetInstance()->SetScreenWidth(g_ScreenWidth);
	Demeter::Settings::GetInstance()->SetScreenHeight(g_ScreenHeight);
	Demeter::Settings::GetInstance()->SetPreloadTextures(m_PreloadTextures);
	Demeter::Settings::GetInstance()->SetTextureCompression(m_TextureCompression);
	Demeter::Settings::GetInstance()->SetUseDynamicTextures(m_DynamicTextures);
	Demeter::Settings::GetInstance()->SetDetailTextureRepeats(32);
	Demeter::Settings::GetInstance()->SetUseNormals(true);
}


void DemeterTerrain::postCreate() {
	TerrainObject::postCreate();
	CSP_LOG(TERRAIN, INFO, "Terrain Center @ " << getCenter().asString());
	CSP_LOG(TERRAIN, INFO, "Terrain Size " << getWidth() << " x " << getHeight() << " m");
	m_Offset.set(getWidth()*0.5, getHeight()*0.5, 0.0);
	m_CellWidth = getWidth() / m_LatticeWidth;
	m_CellHeight = getHeight() / m_LatticeHeight;
}

simdata::Vector3 DemeterTerrain::getOrigin(simdata::Vector3 const &pos) const {
	int X = (int) ((pos.x() + m_Offset.x()) / getCellWidth());
	int Y = (int) ((pos.y() + m_Offset.y()) / getCellHeight());
	return simdata::Vector3(X * getCellWidth() - m_Offset.x(), Y * getCellHeight() - m_Offset.y(), 0.0);
}

void DemeterTerrain::endDraw() {
	Demeter::Settings::GetInstance()->LogStats();
}

