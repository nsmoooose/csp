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
 * @file TerrainObject.h
 *
 * Abstract interface to the terrain engine.  Any terrain engine
 * that operates in a flat X-Y-Z coordinate space and implements
 * this interface can be used with CSPSim.
 *
 **/

#ifndef __CSPSIM_TERRAINOBJECT_H__
#define __CSPSIM_TERRAINOBJECT_H__

#include <csp/cspsim/Export.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/GeoPos.h>
#include <csp/csplib/data/Vector3.h>

#include <osg/ref_ptr>

namespace osg {
	class Node;
	class LineSegment;
}

CSP_NAMESPACE

class Projection;


/**
 * Abstract base class for all terrain modules.
 */
class CSPSIM_EXPORT TerrainObject: public Object {
public:
	CSP_EXPORT_OBJECT(CSPSIM_EXPORT)
	CSP_DECLARE_ABSTRACT_OBJECT(TerrainObject)

	/* currently the only two terrain engines are demeter and
	 * chunk lod.  the former doesn't have hints, the latter
	 * needs only a short integer.  to be really OO here we
	 * should have an abstract, reference counted hint class
	 * that is allocated by the specific terrain objects and
	 * dynamic_cast to the correct type before use.  at the
	 * moment this would entail fair amount of memory and
	 * processing overhead, so we just sidestep the entire
	 * issue and use integers.  if another engine is added
	 * in the future that requires more elaborate hinting,
	 * we can deal with it at that time.
	 */
	typedef int IntersectionHint;

	/** Class for doing line segment intersection tests against
	    the terrain mesh. */
	class Intersection {
	public:
		Intersection();
		Intersection(Vector3 const &start, Vector3 const &end);
		inline float getRatio() const { return _ratio; }
		inline float getDistance() const;
		inline bool getHit() const { return _hit; }
		inline Vector3 const &getStart() const { return _start; }
		inline Vector3 const &getEnd() const { return _end; }
		inline void setEndPoints(Vector3 const &start, Vector3 const &end) {
			_start = start;
			_end = end;
		}
		inline Vector3 const &getPoint() const { return _point; }
		inline Vector3 const &getNormal() const { return _normal; }
		inline void reset() { _hit = false; }
		void setHit(float ratio, Vector3 const &normal);
	protected:
		bool _hit;
		float _ratio;
		Vector3 _start;
		Vector3 _end;
		Vector3 _point;
		Vector3 _normal;
	};

	TerrainObject();
	virtual ~TerrainObject();

	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual bool isActive() = 0;
	virtual void setScreenSizeHint(int, int) {}
	virtual void setCameraPosition(double, double, double) = 0;
	virtual void testLineOfSight(Intersection &, IntersectionHint &) = 0;
	virtual float getGroundElevation(double x, double y, Vector3 &normal, IntersectionHint &) const = 0;
	virtual float getGroundElevation(double x, double y, IntersectionHint &) const = 0;

	virtual int getTerrainPolygonsRendered() const = 0;
	virtual osg::Node *getNode() = 0;

	LLA const & getCenter() const { return m_Center; }
	inline float getWidth() const { return m_Width; }
	inline float getHeight() const { return m_Height; }
	inline std::string const &getName() const { return m_Name; }
	inline int getVersion() const { return m_Version; }
	const Projection* getProjection() const { return m_Map.get(); }
	virtual Vector3 getOrigin(Vector3 const &) const = 0;
	virtual void endDraw() {}

protected:
	
	LLA m_Center;
	float m_Width, m_Height;
	Ref<Projection> m_Map;
	std::string m_Name;
	int m_Version;

	virtual void postCreate();

};

CSP_NAMESPACE_END

#endif // __CSPSIM_TERRAINOBJECT_H__

