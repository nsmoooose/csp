/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#ifndef __GEOPOS_H__
#define __GEOPOS_H__

#include <string>
#include <SimData/Vector3.h>


NAMESPACE_SIMDATA


/**
 * Reference ellipsoid parameters.
 *
 */
struct ReferenceEllipsoid {
	ReferenceEllipsoid(double semi_major, double semi_minor) {
		A          = semi_major;
		B          = semi_minor;
		R          = 6371010.0;
		f          = (A-B)/A;
		e          = sqrt(2*f-f*f);
		A_B        = A/B;
		B_A        = B/A;
		B2_A2      = (B*B)/(A*A);
		A2_B2      = 1.0 / B2_A2;
		e2	   = e*e;
		e1         = (A-B)/(A+B);
		ep         = sqrt(A*A-B*B)/B;
		ep2        = ep*ep;
		// utm2ll constants
		m_0        = 1.0 - 0.25 * e2 - 0.046875*e2*e2 - 0.01953125*e2*e2*e2;
		m_1        = -(0.375*e2 + 0.09375*e2*e2 + 0.0439453125*e2*e2*e2);
		m_2        = 0.05859375*e2*e2 + 0.0439453125*e2*e2*e2;
		m_3        = -0.01139322916667*e2*e2*e2;
		// ll2utm constants
		m_f        = 1.0/(A*(1.0 - e2*(0.25 + e2*(0.046875 + e2*0.01953125))));
		m_a        = e1 * (1.5 - 0.84375*e1*e1);
		m_b        = e1 * e1 * (1.3125 - 1.71875*e1*e1);
		m_c        = 1.5729166666667*e1*e1*e1;
	}
	double A;    // equatorial radius
	double B;    // polar radius
	double R;    // volumetric mean radius
	double f;    // flattening
	double e;    // eccentricity
	double A_B;
	double B_A;
	double B2_A2;
	double A2_B2;
	double e2;
	double e1;
	double ep;
	double ep2;
	
	double m_0;
	double m_1;
	double m_2;
	double m_3;
	
	double m_f;
	double m_a;
	double m_b;
	double m_c;
};


// some standard reference ellipsoids
namespace GeoRef {
	extern const ReferenceEllipsoid Airy1830;
	extern const ReferenceEllipsoid AustralianNational;
	extern const ReferenceEllipsoid WGS84;
	extern const ReferenceEllipsoid GRS80;
	extern const ReferenceEllipsoid WGS72;
	extern const ReferenceEllipsoid Clarke1866;
	extern const ReferenceEllipsoid NAD27;
}



/**
 * GeoPos position class.
 *
 * Extends a cartesian 3-vector to Earth-Centered, Earth-Fixed (ECEF) 
 * coordinates.  Helper methods are provided for lazy conversion to
 * and from Geodetic and UTM coordinates using a standard reference
 * ellipsoid.
 *
 * Due to subclassing of Vector3, modifications of the ECEF vector do
 * not automatically set the dirty flags used for lazy evaluations of
 * other coordinate representations.  After calculations that modify
 * the underlying ECEF vector, you *must* call setDirty() manually to
 * ensure that subsequent conversions to Geodetic and UTM coordinates
 * function properly.
 * 
 */
class GeoPos: public Vector3 {
public:

	/**
	 * Default constructor.
	 *
	 * The WGS84 reference ellipsoid will be set implicity, and the
	 * position will be set to (0, 0, 0) --- the center of the Earth.
	 */
	GeoPos(): Vector3(), _ref(&GeoRef::WGS84) {
		_stale_lla = true;
		_stale_utm = true;
	}
	
	/**
	 * Copy constructor from a standard vector.  
	 *
	 * The WGS84 reference ellipsoid will be set implicity.
	 */
	GeoPos(Vector3 const &v): _ref(&GeoRef::WGS84) { *this = v; }

	/**
	 * Copy constructor
	 *
	 * The new GeoPos will use the same reference ellipsoid as the source.
	 */
	GeoPos(GeoPos const &g): _ref(&GeoRef::WGS84) { *this = g; }

	/**
	 * Copy operator.
	 *
	 * The reference ellipsoid will not be modified.
	 */
	const GeoPos &operator=(Vector3 const &v);
	
	/**
	 * Copy operator.
	 *
	 * The reference ellipsoid is also copied.
	 */
	const GeoPos &operator=(GeoPos const &g);

	/**
	 * Destructor.
	 */
	virtual ~GeoPos() {}

	/**
	 * Get the absolute distance between two points in ECEF coordinates.
	 *
	 * This is the normal cartesian distance between two points;  the 
	 * curvature of the ellipsoid is not involved.
	 */
	double getSlantRange(GeoPos const &) const;

	/**
	 * Get the distance between to points along the surface of the 
	 * reference ellipsoid.
	 *
	 * Both points are projected to altitude = 0, and the distance
	 * calculated along a geodesic path of the reference ellipsoid.
	 * 
	 * @return distance the geodesic distance
	 * @return bearing the bearing to the specified point (in radians relative to true north)
	 */
	void getSurfaceDistance(GeoPos const &, double &distance, double &bearing) const;

	/**
	 * Get the distance between two points along the surface of the
	 * reference ellipsoid, including altitude.
	 *
	 * This method is very similar to getSurfaceDistance, but includes the altitude
	 * difference between the two points in an approximate way that can be used both
	 * at close range and globally.
	 * 
	 * @return distance a combined geodesic and altitude distance 
	 * @return bearing the bearing to the specified point (in radians relative to true north)
	 */
	void getShellDistance(GeoPos const &, double &distance, double &bearing) const;

	/**
	 * Get a local cartesian coordinate frame.
	 *
	 * @return localX a unit vector, tangent to the reference ellipsoid, pointing east
	 * @return localY a unit vector, tangent to the reference ellipsoid, pointing north
	 * @return localZ a nuti vector, normal to the reference ellipsoid
	 */
	void getLocalFrame(Vector3 &localX, Vector3 &localY, Vector3 &localZ) const;
	
	/**
	 * Get the current latitude, longitude, and altitude relative to the reference ellipsoid.
	 *
	 * @return lat latitude in radians
	 * @return lon longitude in radians
	 * @return alt altitude relative to the reference ellipse (in meters)
	 */
	void getLLA(double &lat, double &lon, double &alt) const;
	
	/**
	 * Set the position using latitude, longitude, and altitude.
	 * 
	 * @param lat latitude in radians
	 * @param lon longitude in radians
	 * @param alt altitude in meters
	 */
	void setLLA(double lat, double lon, double alt = 0.0);

	/**
	 * Get the current latitude.
	 * 
	 * @return latitude in radians
	 */
	double getLongitude() const { if (_stale_lla) _updateLLA(); return _lon; }

	/** 
	 * Get the current longitude.
	 * 
	 * @return longitude in radians
	 */
	double getLatitude() const { if (_stale_lla) _updateLLA(); return _lat; }

	/**
	 * Get the current altitude.
	 *
	 * @return altitude relative to the reference ellipse (in meters)
	 */
	double getAltitude() const { if (_stale_lla) _updateLLA(); return _alt; }

	/**
	 * Internal method to update latitude, longitude, and altitude.
	 *
	 * You do not need to call this directly.
	 */
	void _updateLLA() const;

	/**
	 * Get the current position in UTM coordinates.
	 *
	 * @return northing UTM northing
	 * @return easting UTM easting
	 * @return zone UTM zone
	 * @return designator UTM zone letter
	 */
	void getUTM(double &northing, double &easting, char &zone, char &designator) const;

	/**
	 * Set the current position from UTM coordinates.
	 *
	 * @param northing UTM northing
	 * @param easting UTM easting
	 * @param zone UTM zone
	 * @param designator UTM zone letter
	 * @alt altitude above the reference ellipse (in meters)
	 */
	void setUTM(double northing, double easting, char zone, char designator, double alt = 0.0);
	
	/**
	 * Set the current position from UTM coordinates.
	 *
	 * @param northing UTM northing
	 * @param easting UTM easting
	 * @param zone UTM zone (e.g. "10T")
	 * @alt altitude above the reference ellipse (in meters)
	 */
	void setUTM(double northing, double easting, const char *zone, double alt = 0.0);
	
	/**
	 * Get the UTM northing coordinate for the current position.
	 */
	double getNorthing() const { if (_stale_utm) _updateUTM(); return _northing; }

	/**
	 * Get the UTM easting coordinate for the current position.
	 */
	double getEasting() const { if (_stale_utm) _updateUTM(); return _easting; }

	/** 
	 * Get the UTM zone number for the current position.
	 */
	int getZoneNumber() const { if (_stale_utm) _updateUTM(); return _zone; }

	/**
	 * Get the UTM zone letter for the current position.
	 */
	char getZoneLetter() const { if (_stale_utm) _updateUTM(); return _designator; }

	/**
	 * Internal method to update UTM northing, easting, and zone.
	 *
	 * You do not need to call this directly.
	 */
	void _updateUTM() const;

	/**
	 * Internal class method to get the UTM zone letter for a given latitude.
	 *
	 * @param lat latitude in radians
	 */
	static char _getUTMDesignator(double lat);

	/**
	 * Set the reference ellipsoid used for LLA and UTM conversions.
	 *
	 * The default ellipsoid is WGS-84.  The reference ellipsoid can be
	 * specified independently for each GeoPos instance, but mixing results
	 * from different ellipsoids is not recommended.
	 */
	void setReferenceEllipsoid(ReferenceEllipsoid const & = GeoRef::WGS84);

	/**
	 * Get the current reference ellipsoid.
	 */
	ReferenceEllipsoid const &getReferenceEllipsoid() const { return *_ref; }

	/**
	 * Set flags to indicate that the underlying ECEF vector has changed.
	 *
	 * Call this method after any computations that modify the ECEF vector to
	 * ensure that LLA and UTM coordinates will be updated correctly.
	 */
	void setDirty() { _stale_lla = _stale_utm = true; }

	/**
	 * String representation.
	 */
	virtual std::string asString() { return Vector3::asString(); }
	
	/**
	 * Set the current position from XML character data.
	 *
	 * ECEF coordinate format (x, y, z):
	 * 	x.x x.x x.x
	 *
	 * LLA coordinate format (lat, lon, alt): 
	 * 	G x.x x.x x.x
	 *
	 * LLA coordinate format 2 (lat, lon, alt):
	 * 	G x'x"x.x x'x"x.x x.x
	 *
	 * UTM coordinate format (northing, easting, zone, alt):
	 * 	G x.x x.x zone x.x
	 * where 'zone' is an integer followed by a UTM latitude designator,
	 * such as "10T"
	 *
	 * Note that the letters preceeding all formats other than ECEF are required.
	 *
	 * TODO:
	 *     * use different prefix letters for LLA and UTM
	 *     * allow the source reference ellipsoid to be specified
	 */
	void parseXML(const char *);

	/**
	 * Serialize to a data archive
	 */
	virtual void pack(Packer&) const; 
	
	/**
	 * Deserialize from a data archive
	 */
	virtual void unpack(UnPacker&);

protected:

	// geodetic latitude
	mutable double _lat;
	// longitude
	mutable double _lon;
	// altitude above reference ellipse
	mutable double _alt;
	// lla needs update
	mutable bool _stale_lla;
	// utm needs update
	mutable bool _stale_utm;
	// utm northing
	mutable double _northing;
	// utm easting
	mutable double _easting;
	// utm zone
	mutable char _zone;
	// utm designator
	mutable char _designator;
	// reference ellipsoid
	ReferenceEllipsoid const *_ref;
	
	/**
	 * Compute ECEF from LLA
	 */
	void iterateECEF(double p, double z, double x, double y, int iter=0) const;
};



NAMESPACE_END


#endif //__GEOPOS_H__

