/* SimData: Data Infrastructure for Simulations
 * Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
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
 * @brief Geodetic coordinate class and conversions.
 */


#ifndef __SIMDATA_GEOPOS_H__
#define __SIMDATA_GEOPOS_H__

#include <string>
#include <SimData/Vector3.h>
#include <SimData/Export.h>


NAMESPACE_SIMDATA


/** Reference ellipsoid parameters.
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
		e2         = e*e;
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


/** Reference ellipsoids for geospacial coordinate transforms.
 *
 *  See for example:
 *      http://www.colorado.edu/geography/gcraft/notes/datum/datum.html
 *  or
 *      http://www.nima.mil/GandG/tm83581/toc.htm
 */
namespace GeoRef {
	/** Airy 1830
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid Airy1830;
	/** Australian National
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid AustralianNational;
	/** World Geodetic System 1984
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid WGS84;
	/** World Geodetic System 1980
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid GRS80;
	/** World Geodetic System 1972
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid WGS72;
	/** Clarke 1866
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid Clarke1866;
	/** North American Datum 1927
	 */
	extern SIMDATA_EXPORT const ReferenceEllipsoid NAD27;
}


class UTM;
class LLA;
class ECEF;

/** Convert from Earth centered, Earth fixed (ECEF) coondinates
 *  to latitude, longitude, and altitude (LLA).
 *
 *  @param ecef the source coordinates in ECEF
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 *  @return the coordinates in LLA
 */
LLA ECEFtoLLA(ECEF const &ecef, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/** Convert from Earth centered, Earth fixed (ECEF) coondinates
 *  to Universal Transverse Mercator (UTM) coordinates.
 *
 *  @param ecef the source coordinates in ECEF
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 *  @return the coordinates in UTM
 */
UTM ECEFtoUTM(ECEF const &ecef, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/** Convert from latitude, longitude, and altitude (LLA) to
 *  Earth centered, Earth fixed (ECEF) coondinates.
 *
 *  @param lla the source coordinates in LLA
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 *  @return the coordinates in ECEF
 */
ECEF LLAtoECEF(LLA const &lla, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/** Convert from Universal Transverse Mercator (UTM) coordinates to
 *  Earth centered, Earth fixed (ECEF) coondinates.
 *
 *  @param utm the source coordinates in UTM
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 *  @return the coordinates in ECEF
 */
ECEF UTMtoECEF(UTM const &utm, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/** Convert from Universal Transverse Mercator (UTM) coordinates to
 *  latitude, longitude, and altitude (LLA).
 *
 *  @param utm the source coordinates in UTM
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 *  @return the coordinates in LLA
 */
LLA UTMtoLLA(UTM const &utm, ReferenceEllipsoid const &_ref = GeoRef::WGS84);

/** Convert from latitude, longitude, and altitude (LLA) to Universal
 *  Transverse Mercator (UTM) coordinates.
 *
 *  @param lla the source coordinates in LLA
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 *  @param _zone for a specific zone, independent of longitude
 *  @return the coordinates in UTM
 */
UTM LLAtoUTM(LLA const &lla, ReferenceEllipsoid const &_ref = GeoRef::WGS84, char _zone=-1);

/** Get the distance between two points along the surface of the
 *  reference ellipsoid.
 *
 *  Both points are projected to altitude = 0, and the distance
 *  calculated along a geodesic path of the reference ellipsoid.
 *
 *  @param p point 1
 *  @param q point 2
 *  @param distance Output: the geodesic distance
 *  @param bearing Output: the bearing to the specified point (in radians relative to true north)
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 */
void SurfaceDistance(LLA const &p,
                     LLA const &q,
                     double &distance,
                     double &bearing,
                     ReferenceEllipsoid const &_ref = GeoRef::WGS84);


/** Get the distance between two points along the surface of the
 *  reference ellipsoid, including altitude.
 *
 *  This method is very similar to SurfaceDistance, but includes the altitude
 *  difference between the two points in an approximate way that can be used both
 *  at close range and globally.
 *
 *  @param p point 1
 *  @param q point 2
 *  @param distance Output: a combined geodesic and altitude distance
 *  @param bearing Output: the bearing to the specified point (in radians relative to true north)
 *  @param _ref the reference ellipsoid (the default is WGS-84)
 */
void ShellDistance(LLA const &p,
                   LLA const &q,
                   double &distance,
                   double &bearing,
                   ReferenceEllipsoid const &_ref = GeoRef::WGS84);


/** Latitude, longitude, and altitude coordinates.
 *
 *  A geospatial coordinate class representing latitude, longitude, and
 *  altitude.
 *
 *  There are two distinct XML formats for this type.  The first lists
 *  latitude and longitude in degrees, followed by altitude in meters.
 *  The second uses a degree-minute-second notation for the latitude and
 *  longitude.  The notation for the second format is slighly non-standard
 *  to avoid using the degree symbol.  Examples:
 *
 *      @code <LLA>37.1 -122.43 100.0</LLA> @endcode
 *  and
 *      @code <LLA>37'6"0.0 -122'25"48.0 100.0</LLA> @endcode
 *
 *  @ingroup BaseTypes
 */
class SIMDATA_EXPORT LLA {
	double _lat, _lon, _alt;

public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::LLA"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Parse the character data from an XML \<LLA\> tag.
	 *
	 *  LLA coordinate format (lat, lon, alt): @n
	 *   <tt> x.x x.x x.x </tt>
	 *
	 *  LLA coordinate format 2 (lat, lon, alt): @n
	 *   <tt> x'x"x.x x'x"x.x x.x </tt>
	 */
	void parseXML(const char*);

	/// XML post processing.
	void convertXML() {}

public:

	/** Construct a default LLA.
	 */
	LLA(): _lat(0.0), _lon(0.0), _alt(0.0) {}

	/** Construct a new LLA.
	 *
	 *  @param lat latitude in radians.
	 *  @param lon longitude in radians.
	 *  @param alt altitude in meters relative to the reference ellipsoid.
	 */
	LLA(double lat, double lon, double alt=0.0): _lat(lat), _lon(lon), _alt(alt) {}

	/** Copy constructor to convert from UTM.
	 */
	LLA(UTM const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/** Copy constructor to convert from ECEF.
	 */
	LLA(ECEF const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/** Convert from UTM to LLA (using WGS84).
	 */
	LLA const &operator = (UTM const &);

	/** Convert from ECEF to LLA (using WGS84).
	 */
	LLA const &operator = (ECEF const &);

	/** Set latitude, longitude, and altitude in radians.
	 */
	void set(double lat, double lon, double alt=0.0) {
		_lat = lat;
		_lon = lon;
		_alt = alt;
	}

	/** Set latitude, longitude, and altitude in degrees.
	 */
	void setDegrees(double lat, double lon, double alt=0.0);

	/** Get latitude in radians.
	 */
	inline double latitude() const { return _lat; }

	/** Get longitude in radians.
	 */
	inline double longitude() const { return _lon; }

	/** Get altitude in meters.
	 */
	inline double altitude() const { return _alt; }

	/** Get the latitude in degrees.
	 */
	inline double latitudeInDegrees() const { return toDegrees(_lat); }

	/** Get the longitude in degrees.
	 */
	inline double longitudeInDegrees() const { return toDegrees(_lon); }

	/** Set the latitude in degrees.
	 */
	inline void setLatitudeInDegrees(double latitude) { _lat = toRadians(latitude); }

	/** Set the longitude in degrees.
	 */
	inline void setLongitudeInDegrees(double longitude) { _lon = toDegrees(longitude); }

	/** Set the latitude in radians.
	 */
	inline void setLatitude(double latitude) { _lat = latitude; }

	/** Set the longitude in radians.
	 */
	inline void setLongitude(double longitude) { _lon = longitude; }

	/** Set the altitude in meters.
	 */
	inline void setAltitude(double altitude) { _alt = altitude; }
};


/** Universal Transverse Mercator and altitude coordinates.
 *
 *  A geospatial coordinate class using Universal Transverse Mercator (UTM)
 *  coordinates plus altitude.
 *
 *  A sample XML tag for this type is @code <UTM>704300 3390210 13T 100.0</UTM> @endcode
 *  which represents 704300E 3390210N 13T, 100.0 m above the reference ellipsoid.
 *
 *  @ingroup BaseTypes
 */
class SIMDATA_EXPORT UTM {
	double _E, _N, _alt;
	char _zone, _designator;

public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::UTM"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Parse the character data from an XML \<UTM\> tag.
	 *
	 *  UTM coordinate format (easting, northing, zone, alt): @n
	 *    <tt>x.x x.x zone x.x</tt> @n
	 *  where @c zone is an integer followed by a UTM latitude designator,
	 *  such as @c 10T
	 */
	void parseXML(const char*);

	/// XML post processing.
	void convertXML() {}

public:

	/** Get the designator character for a given latitude.
	 */
	static char getDesignator(double latitude);

	/** Construct a default (invalid) UTM
	 */
	UTM(): _E(0.0), _N(0.0), _alt(0.0), _zone(0), _designator('X') {}

	/** Construct a UTM.
	 */
	UTM(double easting_, double northing_, char zone_, char designator_, double alt=0.0) {
		set(easting_, northing_, zone_, designator_, alt);
	}

	/** Convert from LLA to UTM
	 */
	UTM(LLA const &lla, ReferenceEllipsoid const &ref = GeoRef::WGS84, char zone = -1);

	/** Convert from ECEF to UTM
	 */
	UTM(ECEF const &ecef, ReferenceEllipsoid const &ref = GeoRef::WGS84);

	/** Convert from LLA to UTM (using WGS84)
	 */
	UTM const &operator = (LLA const &lla);

	/** Convert from ECEF to UTM (using WGS84)
	 */
	UTM const &operator = (ECEF const &ecef);

	/** Set the current position from UTM coordinates.
	 *
	 *  @param easting_ UTM easting
	 *  @param northing_ UTM northing
	 *  @param zone_ UTM zone
	 *  @param designator_ UTM zone letter
	 *  @param alt altitude above the reference ellipse (in meters)
	 */
	void set(double easting_, double northing_,
	         char zone_, char designator_, double alt=0.0) {
		_E = easting_;
		_N = northing_;
		_zone = zone_;
		_designator = designator_;
		_alt = alt;
		// XXX check values
	}
	
	/** Set the current position from UTM coordinates.
	 *
	 *  @param easting_ UTM easting
	 *  @param northing_ UTM northing
	 *  @param zone_ UTM zone (e.g. "10T")
	 *  @param alt altitude above the reference ellipse (in meters)
	 */
	void set(double easting_, double northing_, const char *zone_, double alt = 0.0);

	/** Get the easting coordinate.
	 */
	inline double easting() const { return _E; }

	/** Get the northing coordinate.
	 */
	inline double northing() const { return _N; }

	/** Get the zone.
	 */
	inline char zone() const { return _zone; }

	/** Get the latitude designator letter.
	 */
	inline char designator() const { return _designator; }

	/** Get altitude in meters.
	 */
	inline double altitude() const { return _alt; }

	/** Check that the UTM coordinates are valid.
	 */
	bool valid() const;
};


/** Earth-centered, earth-fixed coordinates.
 *
 *  A geospatial coordinate class representing Earth Centered, Earth
 *  Fixed coordinates.
 *
 *  The XML format for this type is @code <ECEF> X Y Z </ECEF> @endcode
 *
 *  @ingroup BaseTypes
 */
class SIMDATA_EXPORT ECEF: public Vector3 {
public: // BaseType (most methods inherited from Vector3)

	/// Type representation.
	std::string typeString() const { return "type::ECEF"; }

public:

	/** Construct a default ECEF (at the center of the Earth).
	 */
	ECEF(): Vector3(0.0, 0.0, 0.0) {}

	/** Construct a new ECEF
	 */
	ECEF(double x_, double y_, double z_): Vector3(x_, y_, z_) {}

	/** Copy constructor to convert from UTM
	 */
	ECEF(UTM const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/** Copy constructor to convert from LLA
	 */
	ECEF(LLA const &, ReferenceEllipsoid const & = GeoRef::WGS84);

	/** Convert from UTM to ECEF (using WGS84)
	 */
	ECEF const &operator = (UTM const &);

	/** Convert from UTM to ECEF (using WGS84)
	 */
	ECEF const &operator = (LLA const &);
};


SIMDATA_EXPORT std::ostream &operator <<(std::ostream &o, LLA const &q);
SIMDATA_EXPORT std::ostream &operator <<(std::ostream &o, UTM const &q);
SIMDATA_EXPORT std::ostream &operator <<(std::ostream &o, ECEF const &q);

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_GEOPOS_H__

