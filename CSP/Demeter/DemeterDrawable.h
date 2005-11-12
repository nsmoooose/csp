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
#include <osg/Version>

// OSG_VERSION_MAJOR was first defined in 0.9.9.
#ifndef OSG_VERSION_MAJOR
// The interface to Drawable::computeBound changed in 0.9.9.
#  define OSG_OLD_COMPUTE_BOUND
#endif

#include "Terrain.h"

namespace Demeter
{
    class TERRAIN_API DemeterDrawable : public osg::Drawable
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

            virtual void                drawImplementation(osg::State&) const;
            
        protected:
        
#ifdef OSG_OLD_COMPUTE_BOUND
				virtual bool computeBound() const;
#else
				virtual osg::BoundingBox computeBound() const;
#endif
				mutable osg::ref_ptr<Terrain>       m_RefTerrain;
     };


	class DemeterLatticeDrawableLoadListener;

	// DemeterLatticeDrawable, 
	// provides a wrapper so a Demeter lattice can be used within
	// OpenSceneGraph. A Drawable class was chosen over other osg
	// classes like group so that the latticeDrawable object can
	// handle the draw/drawImmediateMode method.
	// 
    class TERRAIN_API DemeterLatticeDrawable : public osg::Drawable
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

            virtual void                drawImplementation(osg::State&) const;

			virtual void     SetCameraPosition(float, float, float);
            
        protected:
        
#ifdef OSG_OLD_COMPUTE_BOUND
			virtual bool computeBound() const;
#else
			virtual osg::BoundingBox computeBound() const;
#endif
			mutable osg::ref_ptr<TerrainLattice>    m_RefTerrainLattice;
			DemeterLatticeDrawableLoadListener * m_pLatticeLoadListener;

			std::map<Terrain*, osg::Geode *> terrainOsgMap;
    };



class TERRAIN_API DemeterLatticeDrawableLoadListener : public TerrainLoadListener
{
public:

	DemeterLatticeDrawableLoadListener()
	{
		m_pLatticeDrawable = NULL;
	}

    virtual void TerrainLoaded(Terrain* pTerrain)
    {
        int x,y;
        pTerrain->GetLatticePosition(x,y);
        cout << "Terrain at " << x << "," << y << " loaded!" << endl;

		m_pLatticeDrawable->addTerrain(pTerrain);
        // We could do application-specific actions here, such as loading
        // application objects for this region of the world
    }

    virtual void TerrainUnloading(Terrain* pTerrain)
    {
        int x,y;
        pTerrain->GetLatticePosition(x,y);
        cout << "Terrain at " << x << "," << y << " unloading!" << endl;
		m_pLatticeDrawable->removeTerrain(pTerrain);
        // We could do application-specific actions here, such as unloading
        // application objects for this region of the world
    }

	void setLatticeDrawable(DemeterLatticeDrawable * pLatticeDrawable)
	{
		m_pLatticeDrawable = pLatticeDrawable;
	}

protected:
	DemeterLatticeDrawable * m_pLatticeDrawable;
};


}

#endif
