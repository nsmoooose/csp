// Demeter Terrain Visualization Library by Clay Fowler
// Copyright (C) 2002 Clay Fowler

/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

//#ifdef DEMETER_MEMORYMANAGER
//#include "mmgr.h"
//#endif


#include "DemeterDrawable.h"

using namespace osg;

extern std::ofstream m_Logfile;


namespace Demeter
{



DemeterDrawable::DemeterDrawable()
{
	// force the support of display list off for this Drawable
	// since it is not appropriate for dynamic geometry.
	setSupportsDisplayList(false);
}

DemeterDrawable::DemeterDrawable(const DemeterDrawable& other,const CopyOp& copyop):
        Drawable(other,copyop),
        m_RefTerrain(other.m_RefTerrain) // assume shallow copy.
{
}

DemeterDrawable::~DemeterDrawable()
{
}

DemeterDrawable& DemeterDrawable::operator = (const DemeterDrawable& other)
{
	if (&other==this) return *this; // don't copy if assigning to self.
	m_RefTerrain = other.m_RefTerrain; // Increments ref count
	return *this;
}

Object* DemeterDrawable::cloneType() const
{
	// return a clone of this type of drawable.
	return new DemeterDrawable();
}

Object* DemeterDrawable::clone(const osg::CopyOp& copyop) const
{
	// return a clone of this object, via the copy constructor.
	return new DemeterDrawable(*this,copyop);
}

void DemeterDrawable::SetTerrain(Terrain* pTerrain)
{
	m_RefTerrain = pTerrain;
}

const char* DemeterDrawable::className() const
{
	return "DemeterDrawable";
}

void DemeterDrawable::drawImplementation(State& state) const
{
	if (m_RefTerrain.valid())
	{
		m_RefTerrain->ModelViewMatrixChanged();
		m_RefTerrain->Render();
	}
}

bool DemeterDrawable::computeBound() const
{
	if (m_RefTerrain.valid())
	{

		float width = m_RefTerrain->GetWidth();
		float height = m_RefTerrain->GetHeight();
		int latticeX, latticeY;
		m_RefTerrain->GetLatticePosition( latticeX, latticeY );
		_bbox._min.x() = width*latticeX;
		_bbox._min.y() = height*latticeY;
		_bbox._min.z() = 0.0f;
		_bbox._max.x() = width*(latticeX+1);
		_bbox._max.y() = height*(latticeY+1);
		_bbox._max.z() = m_RefTerrain->GetMaxElevation();
		_bbox_computed = true;
	}
	return true;
}


// DemeterLatticeDrawable


DemeterLatticeDrawable::DemeterLatticeDrawable()
{
    // force the support of display list off for this Drawable
    // since it is not appropriate for dynamic geometry.
	//setSupportsDisplayList(false);

	m_pLatticeLoadListener = new DemeterLatticeDrawableLoadListener;
	m_pLatticeLoadListener->setLatticeDrawable(this);

	setSupportsDisplayList(false);


}

DemeterLatticeDrawable::DemeterLatticeDrawable(const DemeterLatticeDrawable& other,const CopyOp& copyop):
        Drawable(other,copyop),
        m_RefTerrainLattice(other.m_RefTerrainLattice) // assume shallow copy.
{
}

DemeterLatticeDrawable::~DemeterLatticeDrawable()
{
}

DemeterLatticeDrawable& DemeterLatticeDrawable::operator = (const DemeterLatticeDrawable& other) 
{        
    if (&other==this) return *this; // don't copy if assigning to self. 
	m_RefTerrainLattice = other.m_RefTerrainLattice; // Increments ref count
	return *this;
}

Object* DemeterLatticeDrawable::cloneType() const 
{ 
    // return a clone of this type of drawable.
	return new DemeterLatticeDrawable();
}

Object* DemeterLatticeDrawable::clone(const osg::CopyOp& copyop) const
{
    // return a clone of this object, via the copy constructor.
    return new DemeterLatticeDrawable(*this,copyop);
}

void DemeterLatticeDrawable::SetTerrainLattice(TerrainLattice* pTerrainLattice)
{
	m_RefTerrainLattice = pTerrainLattice;
	m_RefTerrainLattice->AddTerrainLoadListener(*m_pLatticeLoadListener);
}

void DemeterLatticeDrawable::addTerrain( Terrain* pTerrain )
{

	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "DemeterLatticeDrawable::addTerrain() " << std::endl;
	}


//	DemeterDrawable * pDrawable = new DemeterDrawable;
//	pDrawable->SetTerrain(pTerrain);
//	osg::Geode * pGeode = new osg::Geode;
//	pGeode->addDrawable(pDrawable);
//	addChild(pGeode);
//	terrainOsgMap.insert( std::pair<Terrain*, osg::Geode *>( pTerrain, pGeode ));
}

void DemeterLatticeDrawable::removeTerrain( Terrain * pTerrain )
{

	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "DemeterLatticeDrawable::removeTerrain() " << std::endl;
	}



//	std::map<Terrain*, osg::Geode *>::iterator pos;
//	pos = terrainOsgMap.find(pTerrain);

//	osg::Geode * pGeode = pos->second;
//	removeChild(pGeode);
}


const char* DemeterLatticeDrawable::className() const 
{ 
	return "DemeterLatticeDrawable"; 
}

void DemeterLatticeDrawable::drawImplementation(State& state) const
{
	if (m_RefTerrainLattice.valid())
	{
		m_RefTerrainLattice->Tessellate();
	    m_RefTerrainLattice->Render();
	}
}

void DemeterLatticeDrawable::SetCameraPosition(float x, float y, float z)
{
	m_RefTerrainLattice->SetCameraPosition(x, y, z);

}


bool DemeterLatticeDrawable::computeBound() const
{
	if (m_RefTerrainLattice.valid())
	{

//		_bsphere.radius() = 1000000;
//		_bsphere.center().x() = 0;
//		_bsphere.center().y() = 0;
//		_bsphere.center().z() = 0;
//		_bsphere_computed = true;
		_bbox._min.x() = _bbox._min.y() = _bbox._min.z() = 0.0f;
		_bbox._max.x() = 1000000;
		_bbox._max.y() = 1000000;
		_bbox._max.z() = 10000;
		_bbox_computed = true;
	}
	return true;
}

};
