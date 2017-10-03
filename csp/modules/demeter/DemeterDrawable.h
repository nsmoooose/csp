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

#ifndef _DEMETER_DRAWABLE_
#define _DEMETER_DRAWABLE_



#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Version>

// OSG_VERSION_MAJOR was first defined in 0.9.9.
#ifndef OSG_VERSION_MAJOR
// The interface to Drawable::computeBound changed in 0.9.9.
#  define OSG_OLD_COMPUTE_BOUND
#endif

#include "Terrain.h"

namespace Demeter
{
    class DEMETER_EXPORT DemeterDrawable : public osg::Drawable
    {
        public:

                                        DemeterDrawable();
                                        DemeterDrawable(const DemeterDrawable&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
                                        
            virtual                     ~DemeterDrawable();
            
            DemeterDrawable&            operator = (const DemeterDrawable&) ;

            virtual osg::Object*        cloneType() const;
            virtual osg::Object*        clone(const osg::CopyOp& copyop) const;
            
            virtual const char*         className() const;
            

            void                        SetTerrain(Terrain* pTerrain);            
            Terrain*                    GetTerrain() { return m_RefTerrain.get(); }
            const Terrain*              GetTerrain() const { return m_RefTerrain.get(); }

            virtual void                drawImplementation(osg::RenderInfo&) const;
            
        protected:
				virtual osg::BoundingBox computeBoundingBox() const;

				mutable osg::ref_ptr<Terrain>       m_RefTerrain;
     };


	class DemeterLatticeDrawableLoadListener;

	// DemeterLatticeDrawable, 
	// provides a wrapper so a Demeter lattice can be used within
	// OpenSceneGraph. A Drawable class was chosen over other osg
	// classes like group so that the latticeDrawable object can
	// handle the draw/drawImmediateMode method.
	// 
    class DEMETER_EXPORT DemeterLatticeDrawable : public osg::Drawable
    {
        public:

                                        DemeterLatticeDrawable();
                                        DemeterLatticeDrawable(const DemeterLatticeDrawable&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
                                        
            virtual                     ~DemeterLatticeDrawable();
            
            DemeterLatticeDrawable&            operator = (const DemeterLatticeDrawable&) ;

            virtual osg::Object*        cloneType() const;
            virtual osg::Object*        clone(const osg::CopyOp& copyop) const;
            
            virtual const char*         className() const;
            

            void                        SetTerrainLattice(TerrainLattice* pTerrainLattice);            
            TerrainLattice*                    GetTerrainLattice() { return m_RefTerrainLattice.get(); }
            const TerrainLattice*              GetTerrainLattice() const { return m_RefTerrainLattice.get(); }

			void addTerrain( Terrain* pTerrain );
			void removeTerrain( Terrain * pTerrain );

            virtual void                drawImplementation(osg::RenderInfo&) const;

			virtual void     SetCameraPosition(float, float, float);
            
        protected:
			virtual osg::BoundingBox computeBoundingBox() const;

			mutable osg::ref_ptr<TerrainLattice>    m_RefTerrainLattice;
			DemeterLatticeDrawableLoadListener * m_pLatticeLoadListener;

			std::map<Terrain*, osg::Geode *> terrainOsgMap;
    };



class DEMETER_EXPORT DemeterLatticeDrawableLoadListener : public TerrainLoadListener
{
public:
	DemeterLatticeDrawableLoadListener();
    virtual void TerrainLoaded(Terrain* pTerrain);
    virtual void TerrainUnloading(Terrain* pTerrain);
	void setLatticeDrawable(DemeterLatticeDrawable * pLatticeDrawable);

protected:
	DemeterLatticeDrawable * m_pLatticeDrawable;
};


}

#endif
