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
 * @file TerrainObject.h
 *
 **/

#ifndef __TERRAINOBJECT_H__
#define __TERRAINOBJECT_H__

#include "Terrain.h"
#include "DemeterDrawable.h"
#include "TerrainTextureFactory.h"

#include <SimData/Object.h>
#include <SimData/Types.h>
#include <SimData/InterfaceRegistry.h>


class VirtualBattlefield;

/**
 * class TerrainObject
 *
 * @author unknown
 */

class TerrainObject: public simdata::Object
{
public:
	SIMDATA_OBJECT(TerrainObject, 0, 0)
		
	BEGIN_SIMDATA_XML_INTERFACE(TerrainObject)
		SIMDATA_XML("detail_texture_file", TerrainObject::m_DetailTextureFile, true)
		SIMDATA_XML("texture_file", TerrainObject::m_TextureFile, true)
		SIMDATA_XML("elevation_file", TerrainObject::m_ElevationFile, true)
		SIMDATA_XML("lattice_basename", TerrainObject::m_LatticeBaseName, true)
		SIMDATA_XML("lattice_elevation_extension", TerrainObject::m_LatticeElevExt, true)
		SIMDATA_XML("lattice_texture_extension", TerrainObject::m_LatticeTexExt, true)
		SIMDATA_XML("use_texture_factory", TerrainObject::m_TextureFactory, false)
		SIMDATA_XML("lattice_width", TerrainObject::m_LatticeWidth, true)
		SIMDATA_XML("lattice_height", TerrainObject::m_LatticeHeight, true)
		SIMDATA_XML("max_triangles", TerrainObject::m_MaxTriangles, true)
		SIMDATA_XML("detail_threshold", TerrainObject::m_DetailThreshold, true)
		SIMDATA_XML("vertex_height", TerrainObject::m_VertexHeight, true)
		SIMDATA_XML("vertex_spacing", TerrainObject::m_VertexSpacing, true)
		SIMDATA_XML("use_dynamic_textures", TerrainObject::m_DynamicTextures, false)
		SIMDATA_XML("use_texture_compression", TerrainObject::m_TextureCompression, false)
		SIMDATA_XML("preload_textures", TerrainObject::m_PreloadTextures, false)
		SIMDATA_XML("lattice", TerrainObject::m_Lattice, true)
	END_SIMDATA_XML_INTERFACE
	
	TerrainObject();
	virtual ~TerrainObject();

	void activate(VirtualBattlefield*);
	void deactivate();
	bool isActive();
	float getElevation(float x, float y) const;
	void getNormal(float x, float y, float & normalX, float & normalY, float & normalZ ) const;
	void setCameraPosition(float, float, float);

	int getTerrainPolygonsRendered() const;
	osg::Node *getNode() { return m_TerrainNode; }

	
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

	
protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);

	int createTerrainLattice();
	int createTerrain();

	void load();
	void unload();

	VirtualBattlefield *m_Battlefield;

	osg::Node* createTerrainNode(Demeter::Terrain* pTerrain);
	osg::Node* createTerrainLatticeNode(Demeter::TerrainLattice* pTerrainLattice);

	//osg::Node* m_TerrainLatticeNode;
	osg::Node* m_TerrainNode;
	//osg::ref_ptr<osg::Node> m_TerrainLatticeNode;
	//osg::ref_ptr<osg::Node> m_TerrainNode;

	void updateDemeterSettings();
	
	bool m_Active;
	bool m_Loaded;

	Demeter::Terrain* m_pTerrain;

	mutable osg::ref_ptr<Demeter::TerrainLattice> m_pTerrainLattice;

	Demeter::DemeterDrawable* m_pDrawable;
	Demeter::DemeterLatticeDrawable* m_pLatticeDrawable ;

	Demeter::TerrainTextureFactory * m_pTerrainTextureFactory;

};


#endif // __TERRAINOBJECT_H__

