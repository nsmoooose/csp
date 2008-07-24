// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file Projection.h
 *
 * Classes for projecting geospatial coordinates onto the
 * 2D battlefield and theater map.
 *
 **/

#ifndef __CSPSIM_PROJECTION_H__
#define __CSPSIM_PROJECTION_H__

#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/GeoPos.h>
#include <csp/csplib/data/LUT.h>
#include <csp/csplib/util/Ref.h>

namespace csp {

/** An interface for converting between geospatial coordinates and 2D world
 *  coordinates.
 */
class Projection: public Referenced {
public:
	typedef Ref<Projection> RefT;
	typedef Ref<const Projection> CRefT;

	virtual ~Projection() { }

	virtual LLA getCenter() const = 0;
	virtual Vector3 convert(LLA const &pos) const = 0;
	virtual LLA convert(Vector3 const &pos) const = 0;
	virtual Vector3 getNorth(LLA const &pos) const = 0;
	virtual Vector3 getNorth(Vector3 const &pos) const = 0;
	virtual Vector3 getUp(Vector3 const &pos) const = 0;
};


/**
 * A gnomonic projection of a sphere onto a tangent (or secant) plane.
 * The projection maps geodesics to straight lines.  Distances far
 * from the projection origin, while distances are contracted near the
 * origin in the case of a secant plane (distances are true at the
 * origin for a tangent plane).
 */
class GnomonicProjection: public Projection {
	double m_Lat0, m_Lon0, m_R;
	double m_S, m_C, m_W;

public:
	double getRadius() const { return m_R; }
	void setRadius(double radius) { m_R = radius; }

	virtual LLA getCenter() const;
	void setCenter(LLA const &center);

	GnomonicProjection();
	GnomonicProjection(double lat, double lon, double radius=1.0);

	virtual Vector3 convert(LLA const &pos) const;
	virtual LLA convert(Vector3 const &pos) const;
	virtual Vector3 getNorth(LLA const &pos) const;
	virtual Vector3 getNorth(Vector3 const &pos) const;
};


/**
 * A secant gnomonic projection for a theater, with fast North vector
 * lookups.
 */
class SecantGnomonicProjection: public GnomonicProjection {
	typedef float value_t;
private:
	Table2 m_NorthX, m_NorthY;
	double m_SizeX, m_SizeY;
	bool m_Valid;

	/** Adjust the projection to reduce the maximum scale distortion
	 *  within the theater bounds.
	 */
	void _secantMap();

	/** Generate the North vector lookup tables.
	 */
	void _constructTables();

public:
	/** Construct a new Projection using gnomonic map centered
	 *  at (lat, lon), and of the specified dimensions (in meters).
	 */
	SecantGnomonicProjection(double lat, double lon, double size_x, double size_y);
	SecantGnomonicProjection();

	void set(LLA const &center, double size_x, double size_y);

	/**
	 * Write a PGM image of the angular deviation of true north from
	 * the Y axis. (for testing purposes)
	 */
	void dump() const;

	/**
	 * Get the local North vector.  The input position is clamped
	 * to within the theater bounds.
	 */
	virtual Vector3 getNorth(Vector3 const &pos) const;
	virtual Vector3 getUp(Vector3 const &pos) const;
};

} // namespace csp

#endif // __CSPSIM_PROJECTION_H__


