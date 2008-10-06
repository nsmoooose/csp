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
 * @file DemeterTerrain.h
 *
 **/

#ifndef __CSPSIM_DEMETERTERRAIN_H__
#define __CSPSIM_DEMETERTERRAIN_H__

#include <csp/cspsim/TerrainObject.h>

#include <csp/csplib/data/External.h>
#include <csp/csplib/data/GeoPos.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector3.h>

#include <string>

namespace Demeter {
	class Terrain;
	class TerrainLattice;
	class TerrainTextureFactory;
}

#include <osg/ref_ptr>

namespace osg { class Node; }


namespace csp {

/**
 * class DemeterTerrain
 *
 */

class DemeterTerrain: public TerrainObject {
public:
	CSP_DECLARE_OBJECT(DemeterTerrain)

	DemeterTerrain();
	virtual ~DemeterTerrain();

	void activate();
	void deactivate();
	bool isActive();

	virtual void testLineOfSight(Intersection &, IntersectionHint &);
	virtual float getGroundElevation(double x, double y, Vector3 &normal, IntersectionHint &) const;
	virtual float getGroundElevation(double x, double y, IntersectionHint &) const;

	void setCameraPosition(double, double, double);

	int getTerrainPolygonsRendered() const;
	osg::Node *getNode() { return m_TerrainNode.get(); }

	float getCellWidth() const { return m_CellWidth; }
	float getCellHeight() const { return m_CellHeight; }
	int getLatticeWidth() const { return m_LatticeWidth; }
	int getLatticeHeight() const { return m_LatticeHeight; }

	Vector3 getOrigin(Vector3 const &) const;
	virtual void endDraw();

	virtual void setScreenSizeHint(int width, int height);

	void setUseDynamicTextures(bool value) { m_DynamicTextures = value; }
	bool getUseDynamicTextures() { return m_DynamicTextures; }

	void setUseTextureCompression(bool value) { m_TextureCompression = value; }
	bool getUseTextureCompression() { return m_TextureCompression; }

	void setPreloadTextures(bool value) { m_PreloadTextures = value; }
	bool getPreloadTextures() { return m_PreloadTextures; }

	void setUseFileTextureFactory(bool value) { m_UseFileTextureFactory = value; }
	bool getUseFileTextureFactory() { return m_UseFileTextureFactory; }

	void setUseTextureFactory(bool value) { m_TextureFactory = value; }
	bool getUseTextureFactory() { return m_TextureFactory; }

	void setMaxTriangles(int value) { m_MaxTriangles = value; }
	int getMaxTriangles() { return m_MaxTriangles; }

	void setLattice(bool value) { m_Lattice = value; }
	bool getLattice() { return m_Lattice; }

	void setLatticeWidth(int value) { m_LatticeWidth = value; }
	int getLatticeWidth() { return m_LatticeWidth; }

	void setLatticeHeight(int value) { m_LatticeHeight = value; }
	int getLatticeHeight() { return m_LatticeHeight; }

	void setLatticeTilesWidth(int value) { m_LatticeTilesWidth = value; }
	int getLatticeTilesWidth() { return m_LatticeTilesWidth; }

	void setLatticeTilesHeight(int value) { m_LatticeTilesHeight = value; }
	int getLatticeTilesHeight() { return m_LatticeTilesHeight; }
	
	void setDetailTextureFile(External value) { m_DetailTextureFile = value; }
	External getDetailTextureFile() { return m_DetailTextureFile; }

	void setLatticeBaseName(const char* value) { m_LatticeBaseName = value; }
	std::string getLatticeBaseName() { return m_LatticeBaseName; }

	void setLatticeElevationExtension(const char* value) { m_LatticeElevExt = value; }
	std::string getLatticeElevationExtension() { return m_LatticeElevExt; }

	void setLatticeTextureExtension(const char* value) { m_LatticeTexExt = value; }
	std::string getLatticeTextureExtension() { return m_LatticeTexExt; }

	void setTextureFile(External value) { m_TextureFile = value; }
	External getTextureFile() { return m_TextureFile; }

	void setElevationFile(External value) { m_ElevationFile = value; }
	External getElevationFile() { return m_ElevationFile; }

	void setVertexSpacing(float value) { m_VertexSpacing = value; }
	float getVertexSpacing() { return m_VertexSpacing; }

	void setVertexHeight(float value) { m_VertexHeight = value; }
	float getVertexHeight() { return m_VertexHeight; }

	void setDetailThreshold(float value) { m_DetailThreshold = value; }
	float getDetailThreshold() { return m_DetailThreshold; }

	virtual void postCreate();

protected:
	bool m_DynamicTextures;
	bool m_TextureCompression;
	bool m_PreloadTextures;
	bool m_TextureFactory;
	int m_MaxTriangles;
	External m_DetailTextureFile;
	External m_TextureFile;
	External m_ElevationFile;
	float m_VertexSpacing;
	float m_VertexHeight;
	bool m_Lattice;
	float m_DetailThreshold;
	std::string m_LatticeBaseName;
	std::string m_LatticeElevExt;
	std::string m_LatticeTexExt;
	int m_LatticeWidth;
	int m_LatticeHeight;
    int m_LatticeTilesWidth;
    int m_LatticeTilesHeight;
	Vector3 m_Offset;
	float m_CellWidth, m_CellHeight;
	int m_ScreenWidth;
	int m_ScreenHeight;
    bool m_UseFileTextureFactory;

protected:

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

} // namespace csp

#endif // __CSPSIM_DEMETERTERRAIN_H__

