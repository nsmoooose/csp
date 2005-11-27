// Combat Simulator Project - FlightSim Demo
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
 * @file Projection.cpp
 *
 * Classes for projecting geospatial coordinates onto the
 * 2D battlefield and theater map.
 *
 **/


#include <Projection.h>

#include <cstdio>
#include <iostream>


simdata::LLA GnomonicProjection::getCenter() const {
	return simdata::LLA(m_Lat0, m_Lon0);
}

void GnomonicProjection::setCenter(simdata::LLA const &center) {
	m_Lat0 = center.latitude();
	m_Lon0 = center.longitude();
	m_S = sin(m_Lat0);
	m_C = cos(m_Lat0);
}

GnomonicProjection::GnomonicProjection() {
	m_Lat0 = 0.0;
	m_Lon0 = 0.0;
	m_R = 1.0;
	m_S = 0.0;
	m_C = 1.0;
}

GnomonicProjection::GnomonicProjection(double lat, double lon, double radius) {
	m_Lat0 = lat;
	m_Lon0 = lon;
	m_R = radius;
	m_S = sin(lat);
	m_C = cos(lat);
}

simdata::Vector3 GnomonicProjection::convert(simdata::LLA const &pos) const {
	double y = pos.longitude() - m_Lon0;
	double cplat = cos(pos.latitude());
	double splat = sin(pos.latitude());
	double x = cplat * sin(y);
	y = -m_S * cplat * cos(y) + m_C * splat;
	return simdata::Vector3(tan(x)*m_R, tan(y)*m_R, pos.altitude());
}

simdata::LLA GnomonicProjection::convert(simdata::Vector3 const &pos) const {
	double x0 = atan(pos.x() / m_R);
	double y = atan(pos.y() / m_R);
	double x = sqrt(1.0 - x0*x0 - y*y);
	double z = m_S * x + m_C * y;
	x = m_C * x - m_S * y;
	return simdata::LLA(asin(z), atan2(x0, x) + m_Lon0, pos.z());
}

simdata::Vector3 GnomonicProjection::getNorth(simdata::LLA const &pos) const {
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
	return simdata::Vector3(x, y, 0.0).normalized();
}

simdata::Vector3 GnomonicProjection::getNorth(simdata::Vector3 const &pos) const {
	double x0 = atan(pos.x() / m_R);
	double y = atan(pos.y() / m_R);
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
	return simdata::Vector3(x, y, 0.0).normalized();
}


void SecantGnomonicProjection::_secantMap() {
	double R = 6371010.0;
	double angle = 0.5 * std::max(m_SizeX, m_SizeY) / R;
	double scale = 1.0 / cos(angle) / cos(angle);
	scale = 0.5 * (1.0 + scale) / scale;
	setRadius(R * scale);
}

void SecantGnomonicProjection::_constructTables() {
	int i, j;
	// fixed spacing of 50 km
	const float spacing = 50000.0;
	const int nx = int(0.5 * m_SizeX / spacing);
	const int ny = int(0.5 * m_SizeY / spacing);
	std::vector< std::vector<float> > breaks(2);
	std::vector<float> dataX, dataY;
	breaks[0].resize(2*nx+1);
	breaks[1].resize(2*ny+1);
	for (i = -nx; i <= nx; i++) {
		breaks[0][i+nx] = i*spacing;
	}
	for (i = -ny; i <= ny; i++) {
		breaks[1][i+ny] = i*spacing;
	}
	dataX.reserve((2*nx+1)*(2*ny+1));
	dataY.reserve((2*nx+1)*(2*ny+1));
	for (i = -nx; i <= nx; i++) {
		for (j = -ny; j <= ny; j++) {
			simdata::Vector3 pos(i*spacing, j*spacing, 0.0);
			simdata::Vector3 n = GnomonicProjection::getNorth(pos);
			dataX.push_back(static_cast<float>(n.x()));
			dataY.push_back(static_cast<float>(n.y()));
		}
	}
	m_NorthX.load(dataX, breaks);
	m_NorthY.load(dataY, breaks);
	m_NorthX.interpolate(simdata::Table2::Dim(nx)(ny), simdata::Interpolation::LINEAR);
	m_NorthY.interpolate(simdata::Table2::Dim(nx)(ny), simdata::Interpolation::LINEAR);
	m_Valid = true;
}

SecantGnomonicProjection::SecantGnomonicProjection(double lat, double lon, double size_x, double size_y) : GnomonicProjection(lat, lon) {
	m_Valid = false;
	m_SizeX = size_x;
	m_SizeY = size_y;
	_secantMap();
	_constructTables();
}

SecantGnomonicProjection::SecantGnomonicProjection() {
	m_Valid = false;
	m_SizeX = 1.0;
	m_SizeY = 1.0;
}

void SecantGnomonicProjection::set(simdata::LLA const &center, double size_x, double size_y) {
	setCenter(center);
	m_SizeX = size_x;
	m_SizeY = size_y;
	_secantMap();
	_constructTables();
}

void SecantGnomonicProjection::dump() const {
	assert(m_Valid);
	std::cout << "P2 256 256 255\n";
	for (int j = 0; j < 256; j++) {
		float y = static_cast<float>((j/256.0 - 0.5)*m_SizeY);
		for (int i = 0; i < 256; i++) {
			float x = static_cast<float>((i/256.0 - 0.5)*m_SizeX);
			double a = atan2(m_NorthX[x][y], m_NorthY[x][y]);
			int v = static_cast<int>(a * 127.5 / 3.1416) - 128;
			if (v < 0) v += 255;
			std::cout << v << " ";
			if (i % 8 == 7) {
				std::cout << "\n";
			}
		}
	}
}

simdata::Vector3 SecantGnomonicProjection::getNorth(simdata::Vector3 const &pos) const {
	assert(m_Valid);
	float x = static_cast<value_t>(simdata::clampTo(pos.x(),-m_SizeX,m_SizeX));
	float y = static_cast<value_t>(simdata::clampTo(pos.y(),-m_SizeY, m_SizeY));
	return simdata::Vector3(m_NorthX[x][y], m_NorthY[x][y], 0.0);
}

simdata::Vector3 SecantGnomonicProjection::getUp(simdata::Vector3 const &) const {
	return simdata::Vector3::ZAXIS;
}

