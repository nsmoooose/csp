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

void DemeterDrawable::drawImplementation(RenderInfo& /*state*/) const
{
	if (m_RefTerrain.valid())
	{
		m_RefTerrain->ModelViewMatrixChanged();
		m_RefTerrain->Render();
	}
}

osg::BoundingBox DemeterDrawable::computeBoundingBox() const {
	osg::BoundingBox bbox;
	if (m_RefTerrain.valid()) {
		float width = m_RefTerrain->GetWidth();
		float height = m_RefTerrain->GetHeight();
		int latticeX, latticeY;
		m_RefTerrain->GetLatticePosition(latticeX, latticeY);
		bbox._min.x() = width*latticeX;
		bbox._min.y() = height*latticeY;
		bbox._min.z() = 0.0f;
		bbox._max.x() = width*(latticeX+1);
		bbox._max.y() = height*(latticeY+1);
		bbox._max.z() = m_RefTerrain->GetMaxElevation();
	}
	return bbox;
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

void DemeterLatticeDrawable::addTerrain(Terrain* /*pTerrain*/)
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

void DemeterLatticeDrawable::removeTerrain( Terrain * /*pTerrain*/ )
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

void DemeterLatticeDrawable::drawImplementation(RenderInfo& /*state*/) const
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

osg::BoundingBox DemeterLatticeDrawable::computeBoundingBox() const {
	osg::BoundingBox bbox(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	if (m_RefTerrainLattice.valid()) {
		bbox._min.x() = bbox._min.y() = bbox._min.z() = 0.0f;
		bbox._max.x() = 1000000;
		bbox._max.y() = 1000000;
		bbox._max.z() = 10000;
	}
	return bbox;
}

DemeterLatticeDrawableLoadListener::DemeterLatticeDrawableLoadListener() : m_pLatticeDrawable(NULL){
}

void DemeterLatticeDrawableLoadListener::TerrainLoaded(Terrain* pTerrain) {
	int x,y;
	pTerrain->GetLatticePosition(x,y);
	std::cout << "Terrain at " << x << "," << y << " loaded!" << std::endl;

	m_pLatticeDrawable->addTerrain(pTerrain);
	// We could do application-specific actions here, such as loading
	// application objects for this region of the world
}

void DemeterLatticeDrawableLoadListener::TerrainUnloading(Terrain* pTerrain) {
	int x,y;
	pTerrain->GetLatticePosition(x,y);
	std::cout << "Terrain at " << x << "," << y << " unloading!" << std::endl;
	m_pLatticeDrawable->removeTerrain(pTerrain);
	// We could do application-specific actions here, such as unloading
	// application objects for this region of the world
}

void DemeterLatticeDrawableLoadListener::setLatticeDrawable(DemeterLatticeDrawable * pLatticeDrawable) {
	m_pLatticeDrawable = pLatticeDrawable;
}

}
