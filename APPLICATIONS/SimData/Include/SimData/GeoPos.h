/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimData.
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

/**
 * @file GeoPos.h
 *
 * Geodetic coordinate class and conversions.
 */

/**
 * @namespace GeoRef 
 *
 * @brief Reference ellipsoids for geospacial coordinate transforms.
 */

#ifndef __SIMDATA_GEOPOS_H__
#define __SIMDATA_GEOPOS_H__

#include <string>
#include <SimData/Vector3.h>
#include <SimData/Export.h>


NAMESPACE_SIMDATA


/**
 * @brief Reference ellipsoid parameters.
 */
struct SIMDATA_EXPORT ReferenceEllipsoid {
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
	double A;    ///< equatorial radius
	double B;    ///< polar radius
	double R;    ///< volumetric mean radius (6371010 meters)
	double f;    ///< flattening @f$((A-B)/A)@f$
	double e;    ///< eccentricity @f$(\sqrt{2f-f^2})@f$
	double A_B;  ///< equatorial to polar radius ratio @f$(A/B)@f$
	double B_A;  ///< polar to equatorial radius ratio @f$(B/A)@f$
	double B2_A2;///< square of polar to equatorial radius ratio @f$(B^2/A^2)@f$
	double A2_B2;///< square of equatorial to polar radius ratio @f$(A^2/B^2)@f$
	double e2;   ///< eccentricity squared @f$(e^2)@f$
	double e1;   ///< eccentricity one @f$((A-B)/(A+B))@f$
	double ep;   ///< eccentricity prime @f$(\sqrt{A^2-B^2}/B)@f$
	double ep2;  ///< eccentricity prime squared @f$({e^{\prime}}^2)@f$
	
	double m_0;  ///< utm to lat,lon conversion constant m0
	double m_1;  ///< utm to lat,lon conversion constant m1
	double m_2;  ///< utm to lat,lon conversion constant m2
	double m_3;  ///< utm to lat,lon conversion constant m3
	
	double m_f;  ///< lat,lon to utm conversion constant f
	double m_a;  ///< lat,lon to utm conversion constant a
	double m_b;  ///< lat,lon to utm conversion constant b
	double m_c;  ///< lat,lon to utm conversion constant c
};


/**
 * Some standard reference ellipsoids.
 *
 * See for example:
 *     http://www.colorado.edu/geography/gcraft/notes/datum/datum.html
 * or
 *     http://www.nima.mil/GandG/tm83581/toc.htm
 */
namespace GeoRef {
	/**
	 * Airy 1830
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid Airy1830;
	/**
	 * Australian National
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid AustralianNational;
	/**
	 * World Geodetic System 1984
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid WGS84;
	/**
	 * World Geodetic System 1980
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid GRS80;
	/**
	 * World Geodetic System 1972
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid WGS72;
	/**
	 * Clarke 1866 
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid Clarke1866;
	/**
	 * North American Datum 1927
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid NAD27;
}



/**
 * @brief Position class using ECEF coordinates.
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
 * @ingroup BaseTypes
 * @deprecated Use LLA, UTM, and/or ECEF instead.
 */
class SIMDATA_EXPORT GeoPos: public Vector3 {
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
	GeoPos(Vector3 const &v): Vector3(), _ref(&GeoRef::WGS84) { *this = v; }

	/**
	 * Copy constructor
	 *
	 * The new GeoPos will use the same reference ellipsoid as the source.
	 */
	GeoPos(GeoPos const &g): Vector3(), _ref(&GeoRef::WGS84) { *this = g; }

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
	 * @param alt altitude above the reference ellipse (in meters)
	 */
	void setUTM(double northing, double easting, char zone, char designator, double alt = 0.0);
	
	/**
	 * Set the current position from UTM coordinates.
	 *
	 * @param northing UTM northing
	 * @param easting UTM easting
	 * @param zone UTM zone (e.g. "10T")
	 * @param alt altitude above the reference ellipse (in meters)
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
	virtual std::string asString() const { return Vector3::asString(); }

	/**
	 * Return a string representation of the type.
	 */
	virtual std::string typeString() const { return "type::GeoPos"; }
	
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


class UTM;
class LLA;
class ECEF;

/**
 * Convert from Earth centered, Earth fixed (ECEF) coondinates
 * to latitude, longitude, and altitude (LLA).
 *
 * @param ecef the source coordinates in ECEF 
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return the coordinates in LLA
 */
LLA ECEFtoLLA(ECEF const &ecef, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/**
 * Convert from Earth centered, Earth fixed (ECEF) coondinates
 * to Universal Transverse Mercator (UTM) coordinates.
 *
 * @param ecef the source coordinates in ECEF 
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return the coordinates in UTM
 */
UTM ECEFtoUTM(ECEF const &ecef, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/**
 * Convert from latitude, longitude, and altitude (LLA) to
 * Earth centered, Earth fixed (ECEF) coondinates.
 *
 * @param lla the source coordinates in LLA 
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return the coordinates in ECEF 
 */
ECEF LLAtoECEF(LLA const &lla, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/**
 * Convert from Universal Transverse Mercator (UTM) coordinates to
 * Earth centered, Earth fixed (ECEF) coondinates.
 *
 * @param utm the source coordinates in UTM
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return the coordinates in ECEF 
 */
ECEF UTMtoECEF(UTM const &utm, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/**
 * Convert from Universal Transverse Mercator (UTM) coordinates to
 * latitude, longitude, and altitude (LLA).
 *
 * @param utm the source coordinates in UTM
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return the coordinates in LLA
 */
LLA UTMtoLLA(UTM const &utm, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/**
 * Convert from latitude, longitude, and altitude (LLA) to Universal 
 * Transverse Mercator (UTM) coordinates.
 *
 * @param lla the source coordinates in LLA 
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @param _zone for a specific zone, independent of longitude
 * @return the coordinates in UTM
 */
UTM LLAtoUTM(LLA const &lla, ReferenceEllipsoid const &_ref = GeoRef::WGS84, char _zone=-1);

/**
 * Get the distance between two points along the surface of the 
 * reference ellipsoid.
 *
 * Both points are projected to altitude = 0, and the distance
 * calculated along a geodesic path of the reference ellipsoid.
 *
 * @param p point 1
 * @param q point 2
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return distance the geodesic distance
 * @return bearing the bearing to the specified point (in radians relative to true north)
 */
void SurfaceDistance(LLA const &p, 
                     LLA const &q, 
                     double &distance, 
                     double &bearing, 
                     ReferenceEllipsoid const &_ref = GeoRef::WGS84);


/**
 * Get the distance between two points along the surface of the
 * reference ellipsoid, including altitude.
 *
 * This method is very similar to SurfaceDistance, but includes the altitude
 * difference between the two points in an approximate way that can be used both
 * at close range and globally.
 * 
 * @param p point 1
 * @param q point 2
 * @param _ref the reference ellipsoid (the default is WGS-84)  
 * @return distance a combined geodesic and altitude distance 
 * @return bearing the bearing to the specified point (in radians relative to true north)
 */
void ShellDistance(LLA const &p, 
                   LLA const &q, 
                   double &distance, 
                   double &bearing, 
                   ReferenceEllipsoid const &_ref = GeoRef::WGS84);


/**
 * @brief Latitude, longitude, and altitude coordinates.
 *
 * A geospatial coordinate class representing latitude, longitude, and
 * altitude.
 *
 * There are two distinct XML formats for this type.  The first lists
 * latitude and longitude in degrees, followed by altitude in meters.
 * The second uses a degree-minute-second notation for the latitude and
 * longitude.  The notation for the second format is slighly non-standard
 * to avoid using the degree symbol.  Examples: 
 *
 *     @code <LLA>37.1 -122.43 100.0</LLA> @endcode
 * and 
 *     @code <LLA>37'6"0.0 -122'25"48.0 100.0</LLA> @endcode
 *
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT LLA: public BaseType {
	double _lat, _lon, _alt;
public:
	/**
	 * Construct a default LLA.
	 */
	LLA(): _lat(0.0), _lon(0.0), _alt(0.0) {}

	/**
	 * Construct a new LLA.
	 *
	 * @param lat latitude in radians.
	 * @param lon longitude in radians.
	 * @param alt altitude in meters relative to the reference ellipsoid.
	 */
	LLA(double lat, double lon, double alt=0.0): _lat(lat), _lon(lon), _alt(alt) {}

	/**
	 * Copy constructor to convert from UTM
	 */
	LLA(UTM const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/**
	 * Copy constructor to convert from ECEF
	 */
	LLA(ECEF const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/**
	 * Convert from UTM to LLA (using WGS84)
	 */
	LLA const &operator = (UTM const &);

	/**
	 * Convert from ECEF to LLA (using WGS84)
	 */
	LLA const &operator = (ECEF const &);

	virtual ~LLA() {}

	/**
	 * Set latitude, longitude, and altitude in radians
	 */
	void set(double lat, double lon, double alt=0.0) {
		_lat = lat;
		_lon = lon;
		_alt = alt;
	}

	/**
	 * Set latitude, longitude, and altitude in degrees
	 */
	void setDegrees(double lat, double lon, double alt=0.0);

	/**
	 * Get latitude in radians.
	 */
	inline double latitude() const { return _lat; }

	/**
	 * Get longitude in radians.
	 */
	inline double longitude() const { return _lon; }

	/**
	 * Get altitude in meters.
	 */
	inline double altitude() const { return _alt; }

	/**
	 * String representation.
	 */
	virtual std::string asString() const;

	/**
	 * Return a string representation of the type.
	 */
	virtual std::string typeString() const { return "type::LLA"; }
	
	/**
	 * Set the current position from XML character data.
	 *
	 * LLA coordinate format (lat, lon, alt): 
	 * 	x.x x.x x.x
	 *
	 * LLA coordinate format 2 (lat, lon, alt):
	 * 	x'x"x.x x'x"x.x x.x
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
};


/**
 * @brief Universal Transverse Mercator and altitude coordinates.
 *
 * A geospatial coordinate class using Universal Transverse Mercator (UTM)
 * coordinates plus altitude. 
 *
 * A sample XML tag for this type is @code<UTM>704300 3390210 13T 100.0</UTM>@endcode
 * which represents 704300E 3390210N 13T, 100.0 m above the reference ellipsoid.
 *
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT UTM: public BaseType {
	double _E, _N, _alt;
	char _zone, _designator;
public:
	/**
	 * Get the designator character for a given latitude.
	 */
	static char getDesignator(double latitude);

	/**
	 * Construct a default (invalid) UTM
	 */
	UTM(): _E(0.0), _N(0.0), _alt(0.0), _zone(0), _designator('X') {}

	/**
	 * Construct a UTM.
	 */
	UTM(double easting, double northing, char zone, char designator, double alt=0.0) {
		set(easting, northing, zone, designator, alt);
	}

	/**
	 * Convert from LLA to UTM
	 */
	UTM(LLA const &, ReferenceEllipsoid const & = GeoRef::WGS84, char zone = -1);

	/**
	 * Convert from ECEF to UTM
	 */
	UTM(ECEF const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/**
	 * Convert from LLA to UTM (using WGS84)
	 */
	UTM const &operator = (LLA const &);

	/**
	 * Convert from ECEF to UTM (using WGS84)
	 */
	UTM const &operator = (ECEF const &);

	virtual ~UTM() {}

	/**
	 * Set the current position from UTM coordinates.
	 *
	 * @param easting UTM easting
	 * @param northing UTM northing
	 * @param zone UTM zone
	 * @param designator UTM zone letter
	 * @param alt altitude above the reference ellipse (in meters)
	 */
	void set(double easting, double northing, char zone, char designator, double alt=0.0) {
		_E = easting;
		_N = northing;
		_zone = zone;
		_designator = designator;
		_alt = alt;
		// XXX check values
	}
	
	/**
	 * Set the current position from UTM coordinates.
	 *
	 * @param easting UTM easting
	 * @param northing UTM northing
	 * @param zone UTM zone (e.g. "10T")
	 * @param alt altitude above the reference ellipse (in meters)
	 */
	void set(double easting, double northing, const char *zone, double alt = 0.0);

	/**
	 * Get the easting coordinate.
	 */
	inline double easting() const { return _E; }

	/**
	 * Get the northing coordinate.
	 */
	inline double northing() const { return _N; }

	/**
	 * Get the zone.
	 */
	inline char zone() const { return _zone; }

	/**
	 * Get the latitude designator letter.
	 */
	inline char designator() const { return _designator; }

	/**
	 * Get altitude in meters.
	 */
	inline double altitude() const { return _alt; }

	/**
	 * String representation.
	 */
	virtual std::string asString() const;

	/**
	 * Return a string representation of the type.
	 */
	virtual std::string typeString() const { return "type::UTM"; }
	
	/**
	 * Set the current position from XML character data.
	 *
	 * UTM coordinate format (easting, northing, zone, alt):
	 * 	x.x x.x zone x.x
	 * where 'zone' is an integer followed by a UTM latitude designator,
	 * such as "10T"
	 */
	void parseXML(const char *);

	/**
	 * Check that the UTM coordinates are valid.
	 */
	bool valid() const;

	/**
	 * Serialize to a data archive
	 */
	virtual void pack(Packer&) const; 
	
	/**
	 * Deserialize from a data archive
	 */
	virtual void unpack(UnPacker&);
};

/**
 * @brief Earth-centered, earth-fixed coordinates.
 *
 * A geospatial coordinate class representing Earth Centered, Earth
 * Fixed coordinates.
 *
 * The XML format for this type is @code <ECEF> X Y Z </ECEF> @endcode.
 *
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT ECEF: public Vector3 {
public:
	/**
	 * Construct a default ECEF (at the center of the Earth).
	 */
	ECEF(): Vector3(0.0, 0.0, 0.0) {}

	/**
	 * Construct a new ECEF
	 */
	ECEF(double x, double y, double z): Vector3(x, y, z) {}

	/**
	 * Copy constructor to convert from UTM
	 */
	ECEF(UTM const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/**
	 * Copy constructor to convert from LLA
	 */
	ECEF(LLA const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/**
	 * Convert from UTM to ECEF (using WGS84)
	 */
	ECEF const &operator = (UTM const &);

	/**
	 * Convert from UTM to ECEF (using WGS84)
	 */
	ECEF const &operator = (LLA const &);

	/**
	 * Return a string representation of the type.
	 */
	virtual std::string typeString() const { return "type::ECEF"; }

	virtual ~ECEF() {}
};

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_GEOPOS_H__

