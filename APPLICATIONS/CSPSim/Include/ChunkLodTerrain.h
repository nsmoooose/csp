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
 * @file ChunkLodTerrain.h
 *
 **/

#ifndef __CHUNKLODTERRAIN_H__
#define __CHUNKLODTERRAIN_H__


#include <SimData/Object.h>
#include <SimData/External.h>
#include <SimData/Vector3.h>
#include <SimData/Pack.h>
#include <SimData/InterfaceRegistry.h>

#include "TerrainObject.h"

namespace osgChunkLod {
	class ChunkLodTree;
	class ChunkLodDrawable;
	class ChunkLodIntersect;
	class ChunkLodElevationTest;
	class TextureQuadTree;
}

namespace osg {
	class Image;
	class Geode;
	class MatrixTransform;
}



/**
 * class ChunkLodTerrain
 *
 * @author Mark Rose <mkrose@users.sourceforge.net>
 */

class ChunkLodTerrain: public TerrainObject
{
public:
	SIMDATA_OBJECT(ChunkLodTerrain, 0, 0)
		
	EXTEND_SIMDATA_XML_INTERFACE(ChunkLodTerrain, TerrainObject)
		SIMDATA_XML("chunk_file", ChunkLodTerrain::m_ChunkFile, true)
		SIMDATA_XML("texture_file", ChunkLodTerrain::m_TextureFile, true)
		SIMDATA_XML("lattice_width", ChunkLodTerrain::m_LatticeWidth, true)
		SIMDATA_XML("lattice_height", ChunkLodTerrain::m_LatticeHeight, true)
		SIMDATA_XML("base_screen_error", ChunkLodTerrain::m_BaseScreenError, true)
		SIMDATA_XML("base_texel_size", ChunkLodTerrain::m_BaseTexelSize, true)
		SIMDATA_XML("use_loader_thread", ChunkLodTerrain::m_UseLoaderThread, true)
	END_SIMDATA_XML_INTERFACE
	
	ChunkLodTerrain();
	virtual ~ChunkLodTerrain();

	void activate();
	void deactivate();
	bool isActive();

	virtual void testLineOfSight(Intersection &, IntersectionHint &);
	virtual float getGroundElevation(double x, double y, simdata::Vector3 &normal, IntersectionHint &);
	virtual float getGroundElevation(double x, double y, IntersectionHint &);

	void setCameraPosition(double, double, double);

	int getTerrainPolygonsRendered() const;
	osg::Node *getNode();

	float getLatticeWidth() const { return m_LatticeWidth; }
	float getLatticeHeight() const { return m_LatticeHeight; }

	simdata::Vector3 getOrigin(simdata::Vector3 const &) const;

protected:
	
	bool m_UseLoaderThread;
	float m_BaseTexelSize;
	float m_BaseScreenError;
	float m_ElevationScale;
	simdata::External m_ChunkFile;
	simdata::External m_TextureFile;
	simdata::External m_ElevationFile;
	float m_LatticeWidth;
	float m_LatticeHeight;
	simdata::Vector3 m_Origin;
	
protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void postCreate();

	int createTerrain();

	void load();
	void unload();

	osgChunkLod::ChunkLodTree *m_Terrain;
	osgChunkLod::TextureQuadTree *m_Texture;
	osg::ref_ptr<osgChunkLod::ChunkLodDrawable> m_Drawable;
	osg::ref_ptr<osg::Image> m_ElevationMap;
	osg::ref_ptr<osg::Geode> m_Geode;
	osg::ref_ptr<osg::MatrixTransform> m_Node;
	osg::Matrix m_CoordinateTransform;

	//osgChunkLod::ChunkLodIntersect *m_ElevationTest;
	osgChunkLod::ChunkLodElevationTest *m_ElevationTest;

	bool m_Active;
	bool m_Loaded;
};


#endif // __CHUNKLODTERRAIN_H__

