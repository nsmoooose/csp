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
 * @file DemeterTerrain.h
 *
 **/

#ifndef __DEMETERTERRAIN_H__
#define __DEMETERTERRAIN_H__


#include <SimData/Object.h>
#include <SimData/External.h>
#include <SimData/GeoPos.h>
#include <SimData/Vector3.h>
#include <SimData/InterfaceRegistry.h>

#include "TerrainObject.h"

#include <string>

namespace Demeter {
	class Terrain;
	class TerrainLattice;
	class TerrainTextureFactory;
}

#include <osg/ref_ptr>
#include <osg/Node>


/**
 * class DemeterTerrain
 *
 */

class DemeterTerrain: public TerrainObject
{
public:
	SIMDATA_OBJECT(DemeterTerrain, 0, 0)
		
	EXTEND_SIMDATA_XML_INTERFACE(DemeterTerrain, TerrainObject)
		SIMDATA_XML("detail_texture_file", DemeterTerrain::m_DetailTextureFile, true)
		SIMDATA_XML("texture_file", DemeterTerrain::m_TextureFile, true)
		SIMDATA_XML("elevation_file", DemeterTerrain::m_ElevationFile, true)
		SIMDATA_XML("lattice_basename", DemeterTerrain::m_LatticeBaseName, true)
		SIMDATA_XML("lattice_elevation_extension", DemeterTerrain::m_LatticeElevExt, true)
		SIMDATA_XML("lattice_texture_extension", DemeterTerrain::m_LatticeTexExt, true)
		SIMDATA_XML("use_texture_factory", DemeterTerrain::m_TextureFactory, false)
		SIMDATA_XML("lattice_width", DemeterTerrain::m_LatticeWidth, true)
		SIMDATA_XML("lattice_height", DemeterTerrain::m_LatticeHeight, true)
		SIMDATA_XML("max_triangles", DemeterTerrain::m_MaxTriangles, true)
		SIMDATA_XML("detail_threshold", DemeterTerrain::m_DetailThreshold, true)
		SIMDATA_XML("vertex_height", DemeterTerrain::m_VertexHeight, true)
		SIMDATA_XML("vertex_spacing", DemeterTerrain::m_VertexSpacing, true)
		SIMDATA_XML("use_dynamic_textures", DemeterTerrain::m_DynamicTextures, false)
		SIMDATA_XML("use_texture_compression", DemeterTerrain::m_TextureCompression, false)
		SIMDATA_XML("preload_textures", DemeterTerrain::m_PreloadTextures, false)
		SIMDATA_XML("lattice", DemeterTerrain::m_Lattice, true)
	END_SIMDATA_XML_INTERFACE
	
	DemeterTerrain();
	virtual ~DemeterTerrain();

	void activate();
	void deactivate();
	bool isActive();

	virtual void testLineOfSight(Intersection &, IntersectionHint &);
	virtual float getGroundElevation(double x, double y, simdata::Vector3 &normal, IntersectionHint &);
	virtual float getGroundElevation(double x, double y, IntersectionHint &);

	void setCameraPosition(double, double, double);

	int getTerrainPolygonsRendered() const;
	osg::Node *getNode() { return m_TerrainNode.get(); }

	float getCellWidth() const { return m_CellWidth; }
	float getCellHeight() const { return m_CellHeight; }
	int getLatticeWidth() const { return m_LatticeWidth; }
	int getLatticeHeight() const { return m_LatticeHeight; }

	simdata::Vector3 getOrigin(simdata::Vector3 const &) const;
	virtual void endDraw();

protected:
	
	bool m_DynamicTextures;
	bool m_TextureCompression;
	bool m_PreloadTextures;
	bool m_TextureFactory;
	int m_MaxTriangles;
	simdata::External m_DetailTextureFile;
	simdata::External m_TextureFile;
	simdata::External m_ElevationFile;
	float m_VertexSpacing;
	float m_VertexHeight;
	bool m_Lattice;
	float m_DetailThreshold;
	std::string m_LatticeBaseName;
	std::string m_LatticeElevExt;
	std::string m_LatticeTexExt;
	int m_LatticeWidth;
	int m_LatticeHeight;
	simdata::Vector3 m_Offset;
	float m_CellWidth, m_CellHeight;

	
protected:

	virtual void postCreate();

	int createTerrainLattice();
	int createTerrain();

	void load();
	void unload();

	osg::Node* createTerrainNode(Demeter::Terrain* pTerrain);
	osg::Node* createTerrainLatticeNode(Demeter::TerrainLattice* pTerrainLattice);

	osg::ref_ptr<osg::Node> m_TerrainNode;

	void updateDemeterSettings();
	
	bool m_Active;
	bool m_Loaded;

	// XXX check that these should be mutable...it's not clear why they are.
	mutable osg::ref_ptr<Demeter::Terrain> m_Terrain;
	mutable osg::ref_ptr<Demeter::TerrainLattice> m_TerrainLattice;

	Demeter::TerrainTextureFactory * m_TerrainTextureFactory;
};


#endif // __DEMETERTERRAIN_H__

