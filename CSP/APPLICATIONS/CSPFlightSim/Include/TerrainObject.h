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

class TerrainSymbol;

/**
 * class TerrainObject
 *
 * @author unknown
 */
class TerrainObject
{
public:
	TerrainObject();
	virtual ~TerrainObject();

	void SetActive(bool value);
	float GetElevation(float x, float y);
	void GetNormal(float x, float y, float & normalX, 
		float & normalY, float & normalZ );
	void setCameraPosition(float, float, float);

	int getTerrainPolygonsRendered();


protected:

	int createTerrainLattice();
	int createTerrain();

	osg::Node* CreateTerrainNode(Demeter::Terrain* pTerrain);
	osg::Node* CreateTerrainLatticeNode(Demeter::TerrainLattice* pTerrainLattice);


	void UpdateDemeterSettings();


	bool m_bDynamicTextures;
	bool m_bTextureCompression;
	bool m_bPreloadTextures;
	bool m_bTextureFactory;
	int m_iMaxTriangles;
	std::string m_sDetailTextureFile;
	std::string m_sTextureFile;
	std::string m_sElevationFile;
	float m_fVertexSpacing;
	float m_fVertexHeight;
	bool m_bLattice;
	bool m_bActive;
	float m_fDetailThreshold;

	std::string m_sLatticeBaseName;
	std::string m_sLatticeElevExt;
	std::string m_sLatticeTexExt;
	int m_sLatticeWidth;
	int m_sLatticeHeight;

	friend class TerrainSymbol;

    Demeter::Terrain* m_pTerrain;

	Demeter::TerrainLattice* m_pTerrainLattice;

	Demeter::DemeterDrawable* m_pDrawable;
	Demeter::DemeterLatticeDrawable* m_pLatticeDrawable ;

	Demeter::TerrainTextureFactory * m_pTerrainTextureFactory;

};

#endif // __TERRAINOBJECT_H__

