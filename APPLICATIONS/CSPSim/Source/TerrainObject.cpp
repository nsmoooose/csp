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
#include "LogStream.h"

#include "Terrain.h"
#include "DemeterDrawable.h"
#include "TerrainTextureFactory.h"

#include <SimData/InterfaceRegistry.h>

#include <osg/Geode>
#include <osg/StateSet>


extern int g_ScreenWidth;
extern int g_ScreenHeight;


SIMDATA_REGISTER_INTERFACE(TerrainObject)


// Terrain Object is a wrapper class for the Demeter terrain engine
// and DemeterDrawable OSG class.

// The class contains all the parameters used by the Terrain Engine.
// When the SetActive function is called the Terrain Engine will be
// Initialized and a Drawable node will be added to the scene graph.

/** 
 * Default constructor for the Terrain Object. Sets default values.
 */
TerrainObject::TerrainObject(): simdata::Object()
{
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

TerrainObject::~TerrainObject()
{
	unload();
}

void TerrainObject::pack(simdata::Packer& p) const 
{
	Object::pack(p);
	p.pack(m_DynamicTextures);
	p.pack(m_TextureCompression);
	p.pack(m_PreloadTextures);
	p.pack(m_TextureFactory);
	p.pack(m_MaxTriangles);
	p.pack(m_DetailTextureFile);
	p.pack(m_TextureFile);
	p.pack(m_ElevationFile);
	p.pack(m_VertexSpacing);
	p.pack(m_VertexHeight);
	p.pack(m_Lattice);
	p.pack(m_DetailThreshold);
	p.pack(m_DetailTextureFile);
	p.pack(m_TextureFile);
	p.pack(m_ElevationFile);
	p.pack(m_LatticeBaseName);
	p.pack(m_LatticeElevExt);
	p.pack(m_LatticeTexExt);
	p.pack(m_LatticeWidth);
	p.pack(m_LatticeHeight);
	p.pack(m_Lattice);
}

void TerrainObject::unpack(simdata::UnPacker& p)
{
	Object::unpack(p);
	p.unpack(m_DynamicTextures);
	p.unpack(m_TextureCompression);
	p.unpack(m_PreloadTextures);
	p.unpack(m_TextureFactory);
	p.unpack(m_MaxTriangles);
	p.unpack(m_DetailTextureFile);
	p.unpack(m_TextureFile);
	p.unpack(m_ElevationFile);
	p.unpack(m_VertexSpacing);
	p.unpack(m_VertexHeight);
	p.unpack(m_Lattice);
	p.unpack(m_DetailThreshold);
	p.unpack(m_DetailTextureFile);
	p.unpack(m_TextureFile);
	p.unpack(m_ElevationFile);
	p.unpack(m_LatticeBaseName);
	p.unpack(m_LatticeElevExt);
	p.unpack(m_LatticeTexExt);
	p.unpack(m_LatticeWidth);
	p.unpack(m_LatticeHeight);
	p.unpack(m_Lattice);
}

void TerrainObject::unload() {
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

void TerrainObject::load() {
	if (m_Loaded) return;
	std::string terrain_path = g_Config.getPath("Paths", "TerrainPath", ".", true);
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
void TerrainObject::activate()
{
	if (!m_Active) {
		m_Active = true;
		load();
	} 
}

/**
 * Deactivate the terrain engine.
 */
void TerrainObject::deactivate()
{
	if (m_Active) {
		m_Active = false;
	}
}

bool TerrainObject::isActive() {
	return m_Active;
}

float TerrainObject::getElevation(float x, float y) const
{
	if (!m_Lattice)
	{
		if (m_Terrain.valid())
			return m_Terrain->GetElevation(x, y);
	}
	else
	{
		if (m_TerrainLattice.valid())
			return m_TerrainLattice->GetElevation(x,y);
	}
	return 0.0;

}

void TerrainObject::getNormal(float x, float y, float & normalX, 
                              float & normalY, float & normalZ) const
{
	normalX = 0; normalY = 0; normalZ = 1;
	if (!m_Lattice) {
		if (m_Terrain.valid()) {
			m_Terrain->GetNormal(x, y, normalX, normalY, normalZ);
		}
	} else {
		if (m_TerrainLattice.valid()) {
			m_TerrainLattice->GetNormal(x,y,normalX,normalY,normalZ);
		}
	}

}

int TerrainObject::createTerrain()
{
	CSP_LOG(CSP_ALL, CSP_DEBUG, "TerrainObject::createTerrain() " );
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
	return 1;
}


int TerrainObject::createTerrainLattice()
{

	CSP_LOG(CSP_ALL, CSP_DEBUG, "TerrainObject::createTerrainLattice()..." );

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
  
	m_TerrainLattice->SetCameraPosition(500000, 500000, 3000);

	CSP_LOG(CSP_ALL, CSP_DEBUG, " ...TerrainObject::createTerrainLattice()");

	return 1;
}

void TerrainObject::setCameraPosition(float x, float y, float z)
{
	if (m_TerrainLattice.valid()) {
		m_TerrainLattice->SetCameraPosition(x, y, z);
	}
}


osg::Node* TerrainObject::createTerrainLatticeNode(Demeter::TerrainLattice* pTerrainLattice)
{
	CSP_LOG(CSP_APP, CSP_INFO, "TerrainObject::createTerrainLatticeNode" );
    
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
		CSP_LOG(CSP_APP, CSP_ERROR, "Caught Exception in TerrainObject::createTerrainLatticeNode");
	}
    
    return pGeode;
}


osg::Node* TerrainObject::createTerrainNode(Demeter::Terrain* pTerrain)
{
	CSP_LOG(CSP_APP, CSP_INFO, "TerrainObject::createTerrainNode" );

	osg::Geode* pGeode = NULL;

	try {
		Demeter::DemeterDrawable* pDrawable = new Demeter::DemeterDrawable;
		pDrawable->SetTerrain(pTerrain);
		pGeode = new osg::Geode;
		pGeode->addDrawable(pDrawable);
	}
	catch(...) {
		CSP_LOG(CSP_APP, CSP_ERROR, "Caught Exception in TerrainObject::createTerrainNode");
	}

	return pGeode;
}

int TerrainObject::getTerrainPolygonsRendered() const
{
	if (m_TerrainLattice.valid()) {
		return m_TerrainLattice->GetLatticePolygonsRendered();
	} else if (m_Terrain.valid()) {
		return m_Terrain->GetTerrainPolygonsRendered();
	} else {
		return 0;
	}
}


void TerrainObject::updateDemeterSettings()
{
	std::string terrain_path = g_Config.getPath("Paths", "TerrainPath", ".", true);
	Demeter::Settings::GetInstance()->SetMediaPath(terrain_path.c_str());
	Demeter::Settings::GetInstance()->SetVerbose(false);
	Demeter::Settings::GetInstance()->SetScreenWidth(g_ScreenWidth);
	Demeter::Settings::GetInstance()->SetScreenHeight(g_ScreenHeight);
	Demeter::Settings::GetInstance()->SetPreloadTextures(m_PreloadTextures);
	Demeter::Settings::GetInstance()->SetTextureCompression(m_TextureCompression);
	Demeter::Settings::GetInstance()->SetUseDynamicTextures(m_DynamicTextures);
	Demeter::Settings::GetInstance()->SetDetailTextureRepeats(32);
	Demeter::Settings::GetInstance()->SetUseNormals(true);
}

