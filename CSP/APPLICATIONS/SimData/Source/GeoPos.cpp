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

 
/**
 * @file GeoPos.cpp
 * 
 *
 */


#include <SimData/GeoPos.h>
#include <SimData/GlibCsp.h>

#include <cmath>

#ifndef min
#define min(a, b) ((a)<(b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a)>(b) ? (a) : (b))
#endif

NAMESPACE_SIMDATA

namespace GeoRef {
	const ReferenceEllipsoid Airy1830           (6377563.396, 6356256.909);
	const ReferenceEllipsoid AustralianNational (6378160.000, 6356774.719);
	const ReferenceEllipsoid International24    (6378388.000, 6356912.000);
	const ReferenceEllipsoid WGS84              (6378137.000, 6356752.314);
	const ReferenceEllipsoid GRS80              (6378137.000, 6356752.314);
	const ReferenceEllipsoid WGS72              (6378135.000, 6356750.520);
	const ReferenceEllipsoid Clarke1866         (6378206.400, 6356583.800);
	const ReferenceEllipsoid NAD27              (6378206.400, 6356583.800);
}

GeoPos const &GeoPos::operator=(Vector3 const &v) {
	x = v.x;
	y = v.y;
	z = v.z;
	_stale_lla = true;
	_stale_utm = true;
	return *this;
}

GeoPos const &GeoPos::operator=(GeoPos const &g) {
	x = g.x;
	y = g.y;
	z = g.z;
	_ref = g._ref;
	_stale_lla = g._stale_lla;
	_stale_utm = g._stale_utm;
	if (!_stale_lla) {
		_lat = g._lat;
		_lon = g._lon;
		_alt = g._alt;
	}
	if (!_stale_utm) {
		_northing = g._northing;
		_easting = g._easting;
		_zone = g._zone;
		_designator = g._designator;
	}
	return *this;
}

double GeoPos::getSlantRange(GeoPos const &p) const {
	return (*this - p).Length();
}

void GeoPos::getSurfaceDistance(GeoPos const &p, double &distance, double &bearing) const {
	double U1 = atan2((1-_ref->f) * tan(getLatitude()), 1.0);
	double U2 = atan2((1-_ref->f) * tan(p.getLatitude()), 1.0);
	double s_U1 = sin(U1);
	double c_U1 = cos(U1);
	double s_U2 = sin(U2);
	double c_U2 = cos(U2);
	double psU = s_U1 * s_U2;
	double pcU = c_U1 * c_U2;
	double L = p.getLongitude() - getLongitude();
	double lam = L;
	double last = 100.0;
	double s_lam;
	double c_lam;
	double t1;
	double t2;
	double s2_sig=0.0;
	double c_sig=0.0;
	double sig=0.0;
	double s_sig;
	double s_alp;
	double c2_alp;
	double c_2sigm=0.0;
	double u;
	double u2=0.0;
	double c;
	while (fabs(lam - last) > 0.00001) {
		last = lam;
		s_lam = sin(lam);
		c_lam = cos(lam);
		t1 = c_U2 * s_lam;
		t2 = c_U1 * s_U2 - s_U1 * c_U2 * c_lam;
		s2_sig = t1*t1 + t2*t2;
		c_sig = s_U1 * s_U2 + c_U1 * c_U2 * c_lam;
		sig = acos(c_sig);
		s_sig = sqrt(s2_sig);
		if (s2_sig == 0.0) {
			if (pcU * s_lam > 0.0) {
				s_alp = 1.0;
			} else
			if (pcU * s_lam < 0.0) {
				s_alp = -1.0;
			} else {
				s_alp = 0.0;
			}
		} else {
			s_alp = pcU * s_lam / s_sig;
			s_alp = min(1.0, max(-1.0, s_alp));
		}
		c2_alp = 1.0 - s_alp*s_alp;
		if (c2_alp == 0.0) {
			if (psU > 0.0) {
				c_2sigm = 1.0;
			} else
			if (psU < 0.0) {
				c_2sigm = -1.0;
			} else {
				c_2sigm = 0.0;
			}
		} else {
			c_2sigm = c_sig - 2.0 * psU / c2_alp;
			c_2sigm = min(1.0, max(-1.0, c_2sigm));
		}
		u = cos(asin(s_alp));
		u2 = u * u * (_ref->A2_B2 - 1);
		c = _ref->f * c2_alp * (4.0 + _ref->f * (4.0 - 3.0 * c2_alp)) / 16.0;
		lam = L + (1.0 - c) * _ref->f * s_alp * (sig + c * s_sig * (c_2sigm + c * c_sig * (-1.0 + 2.0 * c_2sigm * c_2sigm)));
	}
	double a = 1.0 + u2 * (64.0 + u2 * (-12.0 + 5.0 * u2)) / 256.0;
	double b = u2 * (128.0 + u2 * (-64.0 + 37.0 * u2)) / 512.0;
	double dsig = b * sqrt(s2_sig) * (c_2sigm + 0.25 * b * c_sig * (-1.0 + 2.0 * c_2sigm * c_2sigm));
	distance = _ref->B * a * (sig - dsig);
	bearing = atan2(c_U2 * sin(lam), c_U1 * s_U2 - s_U1 * c_U2 * cos(lam));
}

void GeoPos::getShellDistance(GeoPos const &p, double &distance, double &bearing) const { 
	double da = getAltitude() - p.getAltitude();
        getSurfaceDistance(p, distance, bearing);
	distance = sqrt(distance*distance + da*da);
}

void GeoPos::getLocalFrame(Vector3 &localX, Vector3 &localY, Vector3 &localZ) const {
	GeoPos up;
	up.setLLA(getLatitude(), getLongitude(), getAltitude()+1.0);
	localZ = up - *this;
	localZ.Normalize();
	localX = simdata::Cross(Vector3::ZAXIS, localZ);
	if (localX.LengthSquared() == 0.0) {
		localX = Vector3::XAXIS;
	} else {
		localX.Normalize();
	}
	localY = simdata::Cross(localZ, localX);
}

void GeoPos::parseXML(const char* cdata) {
	double X, Y, Z;
	// G 1239878 1928378 10T 234.033
	// G 37.91283 -120.12398 234.033
	// G 37'59"20 -120'4"43 234.033
	// 609595.0 239409.0 109399.0
	if (cdata) {
		const char *c = cdata;
		while (*c != 0 && (*c == ' ' || *c == '\t' || *c == '\r' || *c == '\n')) c++;
		if (*c == 'G' || *c == 'g') {
			c++;
			if (strchr(c, '\'')) {
				int lat, latm, lon, lonm;
				int n = sscanf(c, "%d'%d\"%lf %d'%d\"%lf %lf", &lat, &latm, &X, &lon, &lonm, &Y, &Z);
				if (n != 7) throw ParseException("SYNTAX ERROR: expecting 'lat'min\"sec lon'min\"sec altitude'");
				X = lat + latm / 60.0 + X / 3600.0;
				Y = lon + lonm / 60.0 + Y / 3600.0;
				setLLA(X, Y, Z);
			} else {
				const char *d = c;
				while (*d) {
					if (*d >= 'C' && *d <= 'X') break;
					if (*d >= 'c' && *d <= 'x') break;
					d++;
				}
				if (*d) {
					int A;
					char B;
					int n = sscanf(c, "%lf %lf %d%c %lf", &X, &Y, &A, &B, &Z);
					if (n != 5) throw ParseException("SYNTAX ERROR: expecting 'northing easting zone altitude'");
					setUTM(X, Y, A, B, Z);
				} else {
					int n = sscanf(c, "%lf %lf %lf", &X, &Y, &Z);
					if (n != 3) throw ParseException("SYNTAX ERROR: expecting 'latitude longitude altitude'");
					setLLA(X, Y, Z);
				}
			}
		} else {
			int n = sscanf(c, "%lf %lf %lf", &X, &Y, &Z);
			if (n != 3) throw ParseException("SYNTAX ERROR: expecting 'x y z'");
			x = X;
			y = Y;
			z = Z;
		}
	} else throw ParseException("SYNTAX ERROR: empty vector");
}

void GeoPos::pack(Packer &p) const {
	Vector3::pack(p);
}

void GeoPos::unpack(UnPacker &p) {
	Vector3::unpack(p);
	_stale_lla = true;
	_stale_utm = true;
}


void GeoPos::_updateLLA() const {
	_lon = atan2(y, x);
	double p = sqrt(x*x+y*y);
	double q = _ref->A_B * z;
	double h = 1.0 / sqrt(p*p+q*q);
	iterateECEF(p, z, p*h, q*h);
}

void GeoPos::setUTM(double northing, double easting, char zone, char designator, double alt)
{
	// central-meridian scale factor
	static double k0 = 0.9996;

	double nu, T, T2, S, C, CP, SP, R, D, D2, M;
	double mu, phi;
	double x, y;

	_easting = easting;
	_northing = northing;
	_zone = zone;
	_designator = designator;
	
	x = easting - 500000.0; //remove 500,000 meter offset for longitude
	y = northing;

	if ((designator - 'N') < 0) {
		y -= 10000000.0;  //remove 10,000,000 meter offset used for southern hemisphere
	}

	double lon0 = D2R * ((getZoneNumber() - 1) * 6 - 180 + 3);  //+3 puts origin in middle of zone

	M = y / k0;

	mu = M * _ref->m_f;

	phi = mu + _ref->m_a*sin(2.0*mu) + _ref->m_b*sin(4.0*mu) + _ref->m_c*sin(6.0*mu);

	C = cos(phi);
	S = sin(phi);
	T = S/C;
	nu = _ref->A/sqrt(1.0-_ref->e2*S*S);
	T2 = T * T;
	CP = C * C * _ref->ep2;
	SP = 1.0 - S * S * _ref->e2;
	R = _ref->A * _ref->B2_A2 / (SP*sqrt(SP));
	D = x/(nu*k0);
	D2 = D*D;

	double lat, lon;

	lat = phi - (nu*T/R)*(D2*(0.5 - D2*((120.0+90.0*T2+CP*(300.0-120.0*CP)-270.0*_ref->ep2)
			+(61.0+90.0*T2+298.0*CP+45.0*T2*T2-252.0*_ref->ep2-3.0*CP*CP)*D2)/720.0));
			
	lon = D * ( 1.0 - 
	            D2*( (1.0+2.0*T2+CP)/6.0 - 
	                  ( 5.0 - 
			    CP*(2.0 + 3.0*CP) + 
			    8.0*_ref->ep2 + 
			    T2*(28.0 + 24.0*T2)
			  )*D2/120.0
			)
		  ) / C;
		  
	lon += lon0;

	setLLA(lat, lon, alt);
	_stale_utm = false;
}

void GeoPos::setUTM(double northing, double easting, const char *code, double alt) 
{
	char zone=0, designator='Z';
	if (code) {
		char c0 = code[0];
		char c1 = code[1];
		char c2 = code[2];
		zone = c0 - '0';
		if (c2) {
			zone *= 10;
			zone += c1 - '0';
			designator = c2;
		} else designator = c1;
	}
	// TODO verify ranges and throw exceptions
	setUTM(northing, easting, zone, designator, alt);
}

void GeoPos::_updateUTM() const
{
	// central-meridian scale factor
	static double k0 = 0.9996;
	double lon = getLongitude();
	double lat = getLatitude();
	double lon0 = 0.0;
	double nu, T, T2, C, CP, A, A2, A4, M, S;
	
	//Make sure the longitude is between -180.00 .. 179.9
	if (lon >= G_PI) {
		int n = (int) (0.5 * lon / G_PI + 0.5);
		lon -= n * 2.0 * G_PI;
	} else
	if (lon < -G_PI) {
		int n = (int) (0.5 * lon / G_PI - 0.5);
		lon -= n * 2.0 * G_PI;
	}

	_zone = -1;
	
  	// special zone for Norway, lat 56 - 64 deg, lon 3 - 12 deg
  	if (lat >= 0.9773843811168 && lat < 1.1170107212764 && 
	    lon >= 0.0523598775598 && lon < 0.2094395102393) {
		_zone = 32;
		lon0 = 0.1308996938996;
	} else
  	// special zones for Svalbard, lat 72 - 84
	if( lat >= 1.2566370614359 && lat < 1.4660765716752) {
		if (lon < 0.0) {
		} else 
		if (lon < 0.157079632679490) { // 0 - 9 deg
			_zone = 31; 
			lon0 = 0.078539816339744;
		} else 
		if (lon < 0.366519142918809) { // 9 - 21 deg
			_zone = 33; 
			lon0 = 0.261799387799149;
		} else
		if (lon < 0.575958653158129) { // 21 - 33 deg
			_zone = 35; 
			lon0 = 0.471238898038469;
		} else
		if (lon < 0.733038285837618) { // 33 - 42 deg
			_zone = 37; 
			lon0 = 0.654498469497874;
		}
	}
	
	if (_zone == -1) {
		_zone = char((lon / G_PI + 1.0) * 30.0) + 1;
		lon0 = G_PI / 180.0 * ((int(_zone) - 1)*6 - 180 + 3); 
	}

	S = sin(lat);
	C = cos(lat);
	T = S/C;
	T2 = T * T;

	nu = _ref->A/sqrt(1.0-_ref->e2*S*S);
	CP = _ref->ep2*C*C;
	A = C*(lon - lon0);
	A2 = A * A;
	A4 = A2 * A2;

	// approximation for length of arc of a meridian from equator to lat
	M = _ref->A*(_ref->m_0*lat + _ref->m_1*sin(2.0*lat) + _ref->m_2*sin(4.0*lat) - _ref->m_3*sin(6.0*lat));
	
	_easting = k0 * nu * ( A + 
	                      (1.0 - T2 + CP) * A2 * A / 6 + 
	                      (5.0 + T2*(T2-18.0) + 72*CP - 58.0*_ref->ep2) * A4 *A / 120.0
	                     ) + 500000.0;

	_northing = k0 * ( M +
	                   nu * T * ( 0.5*A2 +
		                      (5.0 - T2 + CP*(9.0+4.0*CP))*A4/24.0 +
			 	      (61.0 + T2*(T2-58.0) + 600.0*CP - 330.0*_ref->ep2)*A4*A2/720.0
			            )
		         );

	if (lat < 0.0) {
		_northing += 10000000.0; //10000000 meter offset for southern hemisphere
	}

	_designator = _getUTMDesignator(lat);
	_stale_utm = false;
}

/**
 * Get the UTM designator for a given latitude, or 'Z' if outside
 * the UTM limits (80S to 84N)
 *
 * @param lat latitude in radians
 */
char GeoPos::_getUTMDesignator(double latitude)
{
	static const char designator[] = "CDEFGHJKLMNPQRSTUVWXX";
	latitude *= 180.0 / G_PI;
	if (latitude < -80.0 || latitude > 84.0) return 'Z';
	return designator[(int)(latitude + 80.0)>>3];
}

void GeoPos::setReferenceEllipsoid(ReferenceEllipsoid const &ref) {
	_ref = &ref;
	_stale_lla = true;
	_stale_utm = true;
}

void GeoPos::setLLA(double lat, double lon, double alt) {
	double latp = atan2(_ref->B_A * tan(lat), 1.0);
	double c_latp = cos(latp);
	double s_lon = sin(lon);
	double c_lon = cos(lon);
	double x0 = _ref->A * c_lon * c_latp;
	double y0 = _ref->A * s_lon * c_latp;
	double z0 = _ref->B * sin(latp);
	double z_delta = alt * sin(lat);
	double p_delta = alt * cos(lat);
	_lat = lat;
	_lon = lon;
	_alt = alt;
	x = x0 + p_delta * c_lon;
	y = y0 + p_delta * s_lon;
	z = z0 + z_delta;
	_stale_utm = true;
	_stale_lla = false;
}

void GeoPos::getLLA(double &lat, double &lon, double &alt) const {
	if (_stale_lla) _updateLLA();
	lat = _lat;
	lon = _lon;
	alt = _alt;
}

void GeoPos::getUTM(double &northing, double &easting, char &zone, char &designator) const {
	if (_stale_utm) _updateUTM();
	northing = _northing;
	easting = _easting;
	zone = _zone;
	designator = _designator;
}

// FIXME negative altitudes come out positive.  are there any other bad
// side-effects of being inside the ellipsoid?
void GeoPos::iterateECEF(double p, double z_, double x_, double y_, int iter) const {
	x_ *= _ref->A;
	y_ *= _ref->B;
	double alt = sqrt((z_-y_)*(z_-y_)+(p-x_)*(p-x_));
	double sy = _ref->A2_B2 * y_;
	double h = 1.0/sqrt(sy*sy + x_*x_);
	double dz = z - y_ - alt * sy * h;
	double dp = p - x_ - alt * x_ * h;
	if (iter > 15) { 
		_lat = atan2(sy, x_);
		_alt = alt;
	} else {
		y_ = _ref->A_B * (y_ + dz);
		x_ = x_ + dp;
		double h = 1.0 / sqrt(y_*y_ + x_*x_);
		iterateECEF(p, z_, x_*h, y_*h, iter+1);
	}
}
/*
void GeoPos::iterateECEF(double p, double z, double lat, int iter=0) {
	double x = _ref->A * cos(lat);
	double y = _ref->B * sin(lat);
	double alt = sqrt((z-y)*(z-y)+(p-x)*(p-x));
	double sy = _ref->A2_B2 * y;
	double h = 1.0/sqrt(sy*sy + x*x);
	double dz = z - y - alt * sy * h;
	double dp = p - x - alt * x * h;
	double corr = atan2(_ref->A_B * (y + dz), x + dp);
	if (iter > 15 || abs(corr-_lat) < 0.00000001) {
		_lat = atan2(sy, x);
		_alt = alt;
	} else {
		iterateECEF(p, z, corr, iter+1);
	}
}
*/


NAMESPACE_END

