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
 * @file Projection.h
 *
 * Classes for projecting geospatial coordinates onto the
 * 2D battlefield and theater map.
 *
 **/

#ifndef __PROJECTION_H__
#define __PROJECTION_H__


#include <SimData/Vector3.h>
#include <SimData/Math.h>
#include <SimData/GeoPos.h>
#include <SimData/Interpolate.h>

#include <iostream>
#include <cstdio>


/**
 * A gnomonic projection of a sphere onto a tangent (or secant) plane.
 * The projection maps geodesics to straight lines.  Distances far
 * from the projection origin, while distances are contracted near the
 * origin in the case of a secant plane (distances are true at the
 * origin for a tangent plane).
 */
class GnomonicMap {
	double m_Lat0, m_Lon0, m_R;
	double m_S, m_C, m_W;
public:
	double getRadius() const { return m_R; }

	void setRadius(double radius) {
		m_R = radius;
	}

	simdata::LLA getCenter() const {
		return simdata::LLA(m_Lat0, m_Lon0);
	}

	void setCenter(simdata::LLA const &center) {
		m_Lat0 = center.latitude();
		m_Lon0 = center.longitude();
		m_S = sin(m_Lat0);
		m_C = cos(m_Lat0);
	}

	GnomonicMap() {
		m_Lat0 = 0.0;
		m_Lon0 = 0.0;
		m_R = 1.0;
		m_S = 0.0;
		m_C = 1.0;
	}

	GnomonicMap(double lat, double lon, double radius=1.0) {
		m_Lat0 = lat;
		m_Lon0 = lon;
		m_R = radius;
		m_S = sin(lat);
		m_C = cos(lat);
	}

	simdata::Vector3 convert(simdata::GeoPos const &pos) {
		double lat, lon, alt;
		pos.getLLA(lat, lon, alt);
		return convert(simdata::LLA(lat, lon, alt));
	}

	simdata::Vector3 convert(simdata::LLA const &pos) const {
		double y = pos.longitude() - m_Lon0;
		double cplat = cos(pos.latitude());
		double splat = sin(pos.latitude());
		double x = cplat * sin(y);
		y = -m_S * cplat * cos(y) + m_C * splat;
		return simdata::Vector3(tan(x)*m_R, tan(y)*m_R, pos.altitude());
	}

	simdata::LLA convert(simdata::Vector3 const &pos) const {
		double x0 = atan(pos.x / m_R);
		double y = atan(pos.y / m_R);
		double x = sqrt(1.0 - x0*x0 - y*y);
		double z = m_S * x + m_C * y;
		x = m_C * x - m_S * y;
		return simdata::LLA(asin(z), atan2(x0, x) + m_Lon0, pos.z);
	}

	simdata::Vector3 getNorth(simdata::LLA const &pos) const {
		double y = pos.longitude() - m_Lon0;
		double splat = sin(pos.latitude());
		double cplat = cos(pos.latitude());
		double sy = sin(y);
		double cy = cos(y);
		double x = cplat * sy;
		y = -m_S * cplat * cy + m_C * splat;
		y = cos(y);
		y = m_S * cy * splat / (y*y);
		x = cos(x);
		x = -sy * splat / (x*x);
		return Normalized(simdata::Vector3(x, y, 0.0));
	}

	simdata::Vector3 getNorth(simdata::Vector3 const &pos) const {
		double x0 = atan(pos.x / m_R);
		double y = atan(pos.y / m_R);
		double x = sqrt(1.0 - x0*x0 - y*y);
		double z = m_S * x + m_C * y;
		x = m_C * x - m_S * y;
		y = atan2(x0, x);
		double splat = z;
		double cplat = sqrt(1.0-z*z);
		double sy = sin(y);
		double cy = cos(y);
		x = cplat * sy;
		y = -m_S * cplat * cy + m_C * splat;
		y = cos(y);
		y = m_S * cy * splat / (y*y);
		x = cos(x);
		x = -sy * splat / (x*x);
		return Normalized(simdata::Vector3(x, y, 0.0));
	}
};


/**
 * A secant gnomonic projection for a theater, with fast North vector
 * lookups.
 */
class Projection: public GnomonicMap {
private:
	simdata::Table m_NorthX, m_NorthY;
	double m_SizeX, m_SizeY;
	bool m_Valid;

	/**
	 * Adjust the projection to reduce the maximum scale distortion
	 * within the theater bounds.
	 */
	void _secantMap() {
		double R = 6371010.0;
		double angle = 0.5 * std::max(m_SizeX, m_SizeY) / R;
		double scale = 1.0 / cos(angle) / cos(angle);
		scale = 0.5 * (1.0 + scale) / scale;
		setRadius(R * scale);
	}

	/**
	 * Generate the North vector lookup tables.
	 */
	void _constructTables() {
		int i, j;
		// fixed spacing of 50 km
		float spacing = 50000.0;
		int nx = int(0.5 * m_SizeX / spacing);
		int ny = int(0.5 * m_SizeY / spacing);
		simdata::Table::vector_t breaks;
		breaks.resize(2*nx+1);
		for (i = -nx; i <= nx; i++) {
			breaks[i+nx] = i*spacing;
		}
		m_NorthX.setXBreaks(breaks);
		m_NorthY.setXBreaks(breaks);
		breaks.resize(2*ny+1);
		for (i = -ny; i <= ny; i++) {
			breaks[i+ny] = i*spacing;
		}
		m_NorthX.setYBreaks(breaks);
		m_NorthY.setYBreaks(breaks);
		m_NorthX.setSpacing(spacing, spacing);
		m_NorthY.setSpacing(spacing, spacing);
		simdata::Table::vector_t dataX;
		simdata::Table::vector_t dataY;
		dataX.reserve((2*nx+1)*(2*ny+1));
		dataY.reserve((2*nx+1)*(2*ny+1));
		for (j = -ny; j <= ny; j++) {
			for (i = -nx; i <= nx; i++) {
				simdata::Vector3 pos(i*spacing, j*spacing, 0.0);
				simdata::Vector3 n = GnomonicMap::getNorth(pos);
				dataX.push_back(n.x);
				dataY.push_back(n.y);
			}
		}
		m_NorthX.setData(dataX);
		m_NorthY.setData(dataY);
		m_NorthX.interpolate();
		m_NorthY.interpolate();
		m_Valid = true;
	}

public:
	/**
	 * Construct a new Projection using gnomonic map centered
	 * at (lat, lon), and of the specified dimensions (in meters).
	 */
	Projection(double lat, double lon, double size_x, double size_y)
		: GnomonicMap(lat, lon) 
	{
		m_Valid = false;
		m_SizeX = size_x;
		m_SizeY = size_y;
		_secantMap();
		_constructTables();
	}

	Projection() {
		m_Valid = false;
		m_SizeX = 1.0;
		m_SizeY = 1.0;
	}

	void set(simdata::LLA const &center, double size_x, double size_y) {
		setCenter(center);
		m_SizeX = size_x;
		m_SizeY = size_y;
		_secantMap();
		_constructTables();
	}

	/**
	 * Write a PGM image of the angular deviation of true north from
	 * the Y axis. (for testing purposes)
	 */
	void dump() const {
		assert(m_Valid);
		int i, j;
		float x, y;
		std::cout << "P2 256 256 255\n";
		for (j = 0; j < 256; j++) {
			y = (j/256.0 - 0.5)*m_SizeY;
			for (i = 0; i < 256; i++) {
				x = (i/256.0 - 0.5)*m_SizeX;
				double a = atan2(m_NorthX.getValue(x, y), m_NorthY.getValue(x, y));
				int v = int(a * 127.5 / 3.1416) - 128;
				if (v < 0) v += 255;
				std::cout << v << " ";
				if (i % 8 == 7) {
					std::cout << "\n";
				}
			}
		}
	}

	/**
	 * Get the local North vector.  The input position is clamped
	 * to within the theater bounds.
	 */
	simdata::Vector3 getNorth(simdata::Vector3 const &pos) const {
		assert(m_Valid);
		double x = std::min(m_SizeX, std::max(-m_SizeX, pos.x));
		double y = std::min(m_SizeY, std::max(-m_SizeY, pos.y));
		return simdata::Vector3(m_NorthX.getValue(x, y), m_NorthY.getValue(x, y), 0.0);
	}

	inline simdata::Vector3 getUp(simdata::Vector3 const &pos) const {
		return simdata::Vector3::ZAXIS;
	}

};


#endif // __PROJECTION_H__


