
#include <csp/cspsim/sky/SkyShader.h>
#include <csp/csplib/util/Math.h>
#include <iostream>

CSP_NAMESPACE

#define CUSTOM_NIGHT
#define CUSTOM_EYE
#define CUSTOM_F
//#define CUSTOM_Y
//#define CUSTOM_SUNSET
//#define CUSTOM_SUNSET_OLD

#if 0
// CUSTOMIZATION PARAMETERS
// --------------------------------------------------------------------------
// RGB addative sky values for midnight sky color under a full moon)
#define FULLMOON_RED   0.008
#define FULLMOON_GREEN 0.035
#define FULLMOON_BLUE  0.140
// CIE luminesence (Y) looking into the sun at the zenith.
#define OVER_LUMINESCENCE 1.2 //1.5
// parameters for the attenuation function which cuts the intensity
// as the sun nears the horizon.  Large values of SUNSET_SHARPNESS localizes
// the drop at the horizon.  NIGHT_BASE is the scaling when the sun is 90
// degrees below the horizon.
#define SUNSET_SHARPNESS 4.0
#define NIGHT_BASE 0.02
// sharpness of the halo around the sun (0.1 to 2.0)
#define HALO_SHARPNESS 0.5
// --------------------------------------------------------------------------
#endif

const float TURBIDITY = 2.8f;

SkyShader::SkyShader() {
	m_OverLuminescence = 1.2;
	m_HaloSharpness = 0.3; //0.5;
	m_SunsetSharpness = 50.0;
	m_SunsetElevation = -5.0 * (PI/180.0);  // half-intensity elevation
	m_NightBase = 0.01;
	//m_FullMoonColor = Color(0.008, 0.035, 0.140, Color::RGB);
	//m_FullMoonColor = Color(0.008, 0.086, 0.183, Color::RGB);
	//m_FullMoonColor = Color(0.008, 0.016, 0.183, Color::RGB);
	m_FullMoonColor = Color(0.004, 0.008, 0.0915, Color::RGB);
	setTurbidity(TURBIDITY);
	setSunElevation(0.0);
}

void SkyShader::getSkyCoefficients(float T, sky_c &skylight) {
	skylight.x[0] = -0.01925 * T - 0.25922;
	skylight.x[1] = -0.06651 * T + 0.00081;
	skylight.x[2] = -0.00041 * T + 0.21247;
	skylight.x[3] = -0.06409 * T - 0.89887;
	skylight.x[4] = -0.00325 * T + 0.04517;
	skylight.y[0] = -0.01669 * T - 0.26078;
	skylight.y[1] = -0.09496 * T + 0.00921;
	skylight.y[2] = -0.00792 * T + 0.21023;
	skylight.y[3] = -0.04405 * T - 1.65369;
	skylight.y[4] = -0.01092 * T + 0.05291;
	skylight.Y[0] =  0.17872 * T - 1.46303;
	skylight.Y[1] = -0.35540 * T + 0.42749;
	skylight.Y[2] = -0.02266 * T + 5.32505;
	skylight.Y[3] =  0.12064 * T - 2.57705;
	skylight.Y[4] = -0.06696 * T + 0.37027;
}

float SkyShader::F(float theta, float gamma, coeff &p) {
	float cos_g = cos(gamma);
	//gamma *= std::max(1.0, 10.0 * std::min(1.0, 10.0 * sin(m_SunElevation)));
#ifdef CUSTOM_F
	float cos_t = fabs(cos(theta)) + 0.09;
#else
	float cos_t = cos(theta);
#endif
	return (1.0 + p[0]*exp(p[1]/cos_t))*(1.0+p[2]*exp(p[3]*gamma)+p[4]*cos_g*cos_g);
}

float SkyShader::FastPerez(float theta, float gamma, float factor, float z, coeff &p) {
	if (theta > 1.57) theta = 1.57;
	return factor * z * F(theta, gamma, p);
}

float SkyShader::Perez(float theta, float gamma, float theta_s, float z, coeff &p) {
	if (theta > 1.57) theta = 1.57;
	//if (theta_s > 1.57) theta_s = 1.57;
	//if (theta_s >  1.50) theta_s = 1.50 + ((theta_s-1.50) * 0.5);
	float denom = F(0.0, theta_s, p);
	if (denom != 0.0) z /= denom;
	return z * F(theta, gamma, p);
}

Color SkyShader::getZenith(float T, float theta_s) {
	float s = theta_s;
	float s2 = s*s;
	float s3 = s2*s;
	float t = T;
	float t2 = t*t;
	float xa =  0.00166 * s3 - 0.00375 * s2 + 0.00209 * s;
	float xb = -0.02903 * s3 + 0.06377 * s2 - 0.03202 * s + 0.00394;
	float xc =  0.11693 * s3 - 0.21196 * s2 + 0.06052 * s + 0.25886;
	float xz = xa * t2 + xb * t + xc;
	float ya =  0.00275 * s3 - 0.00610 * s2 + 0.00317 * s;
	float yb = -0.04214 * s3 + 0.08970 * s2 - 0.04153 * s + 0.00516;
	float yc =  0.15346 * s3 - 0.26756 * s2 + 0.06670 * s + 0.26688;
	float yz = ya * t2 + yb * t + yc;
	float chi = (0.4444444 - T * 0.008333333) * (PI - 2.0*s);
	float Yz = (4.0453 * T - 4.9710) * tan(chi) - 0.2155 * T + 2.4192;
	// Yz is in units of kcd/m^2

#ifdef CUSTOM_SUNSET_OLD
	// FIXME
	// Ad hoc function for post-sunset sky.
	if (Yz < 0.2) Yz = 0.2 - (0.2-Yz)*(0.2-Yz)*0.05;
	//if (Yz < 0.05) Yz = 0.05;
#endif
	
	return Color(xz, yz, Yz, Color::CIExyY, false);
}


void SkyShader::setTurbidity(float T) {
	m_Turbidity = T;
	m_Dirty = true;
}

void SkyShader::setSunElevation(float h) {
	m_AzimuthCorrection = 0.0;
	if (h < -PI) {
		h += 2.0 * PI * int((PI - h) / (2.0 * PI));
	} else
	if (h > PI) {
		h -= 2.0 * PI * int((PI + h) / (2.0 * PI));
	}
	if (h > 0.5 *PI) {
		h = PI - h;
		m_AzimuthCorrection = PI;
	} else
	if (h < -0.5*PI) {
		h = -PI - h;
		m_AzimuthCorrection =  -PI;
	}
	m_SunElevation = h;
	m_Dirty = true;
}

void SkyShader::_computeBase() {
	m_Dirty = false;

	m_SunTheta = 0.5*PI - m_SunElevation;
	m_SunVector[0] = 0.0;
	m_SunVector[1] = sin(m_SunTheta);
	m_SunVector[2] = cos(m_SunTheta);

	getSkyCoefficients(m_Turbidity, m_Coefficients);
	
	float theta = m_SunTheta;
	//if (theta >  1.50) theta = 1.50 + ((m_SunTheta-1.50) * 0.5);
	float denom;
	denom = F(0.0, theta, m_Coefficients.x);
	if (denom == 0.0) denom = 1.0;
	m_PerezFactor.x = 1.0 / denom;
	denom = F(0.0, theta, m_Coefficients.y);
	if (denom == 0.0) denom = 1.0;
	m_PerezFactor.y = 1.0 / denom;
	denom = F(0.0, theta, m_Coefficients.Y);
	if (denom == 0.0) denom = 1.0;
	m_PerezFactor.Y = 1.0 / denom;

	m_Zenith = getZenith(m_Turbidity, m_SunTheta);
	m_MaxY = 1.0;

#ifdef CUSTOM_EYE
	m_PupilFactor = 0.5f / std::min(std::max(m_Zenith.getC(), 0.0175f), 0.5f);
#endif

#ifdef CUSTOM_Y
	// only rescale down to the horizon, bad things happen if we go further
	if (m_SunTheta >= 0.5*PI) {
		m_MaxY = Perez(0.5*PI, 0.0, 0.5*PI, m_Zenith.getC(), m_Coefficients.Y);
	} else {
		m_MaxY = Perez(m_SunTheta, 0.0, m_SunTheta, m_Zenith.getC(), m_Coefficients.Y);
	}
#endif

#ifdef CUSTOM_SUNSET
	// F(sun_h) is my own attenuation function which cuts the intensity as the
	// sun drops below the horizon.  Large values of SHARPNESS localize the drop
	// at m_SunElevation.  NightBase is scaling with the sun 90 degrees below
	// the horizon.
	m_F = (atan((m_SunElevation - m_SunsetElevation)*m_SunsetSharpness)/PI+0.5) * (1.0 - m_NightBase) + m_NightBase;
#endif

	// TODO rather than adhoc scaling of Y, model the eye's response to normalize
	// Y over a given range --- the current scaling does this at high values of Y
	// but probably has the opposite affect at low values of Y, where the eye
	// accomodates to the low light level.

}


void _XYZ_to_RGB709(float X, float Y, float Z, float &R, float &G, float &B) {
	R =  3.240479f*X-1.537150f*Y-0.498535f*Z;
	G = -0.969256f*X+1.875992f*Y+0.041556f*Z;
	B =  0.055648f*X-0.204043f*Y+1.057311f*Z;
}

void _xyY_to_XYZ(float x, float y, float Y_, float &X, float &Y, float &Z) {
	Y = Y_;
	if (Y_ == 0.0) {
		X = Z = 0.0;
	} else {
		// any check for y == 0.0?
		X = x * Y / y;
		if (X < 0.0) X = 0.0;
		float z = 1.0f - x - y;
		Z = z * Y / y;
		if (Z < 0.0) Z = 0.0;
	}
}


Color SkyShader::SkyColor(float elevation, float azimuth, float dark, float &intensity) {
	if (m_Dirty) _computeBase();
	float theta = 0.5f*PI - elevation;
	float A = azimuth + m_AzimuthCorrection;
	float v[3] = {sin(A)*sin(theta), cos(A)*sin(theta), cos(theta)};
	float dot = v[0]*m_SunVector[0]+v[1]*m_SunVector[1]+v[2]*m_SunVector[2];
	// numerical error can cause abs(dot) to exceed 1...
	if (dot < -1.0f) dot = -1.0f; else
	if (dot >  1.0f) dot =  1.0f;
	float gamma = acos(dot);
	float ciex = FastPerez(theta, gamma, m_PerezFactor.x, m_Zenith.getA(), m_Coefficients.x);
	float ciey = FastPerez(theta, gamma, m_PerezFactor.y, m_Zenith.getB(), m_Coefficients.y);
	float cieY = FastPerez(theta, gamma, m_PerezFactor.Y, m_Zenith.getC(), m_Coefficients.Y);
	//std::cout << ":: " << theta << " " << gamma << " " << m_PerezFactor.Y << " " << m_Coefficients.Y << " " << m_Zenith.getC() << " " << cieY << "\n";

#ifdef CUSTOM_Y
	//  scale to Y = 1.0 looking directly at the sun
	if (m_MaxY > 0.0f) cieY = m_OverLuminescence * cieY / m_MaxY;
	
	////Color txyY(ciex, ciey, cieY, Color::CIExyY, false);
	////cout << txyY << endl;

	// model for cloudy days:
	//cieY = zenith.getC() * (1+2*cos(theta))/3.0;

	// taking sqrt(Y) softens the sky a bit, spreading the blue more
	// unformly.  this should probably be done instead by blending with
	// a single blue translucent surface across the field of view, where
	// the transparency depends on altitude.
	//float oldY = cieY;
	if (cieY < 0.0f) cieY = 0.0f;
	cieY = pow((double)cieY, (double)m_HaloSharpness) * m_F;
	//if (cieY > m_OverLuminescence) cieY = m_OverLuminescence;
	if (cieY > 1.0) cieY = 1.0;
#endif // CUSTOM

#ifdef CUSTOM_EYE
	//cieY = std::max(0.0f, 0.3f * logf(1.0f +  cieY * m_PupilFactor));
	cieY = std::max(0.0f, 0.3f * logf(1.0f +  cieY));
#endif

	float X_, Y_, Z_, r_, g_, b_;
	_xyY_to_XYZ(ciex, ciey, 1.0, X_, Y_, Z_);
	_XYZ_to_RGB709(X_, Y_, Z_, r_, g_, b_);
	//float h_, s_, v_;
	//RGB_to_HSV(r_, g_, b_, h_, s_, v_);
	//v_ = std::min(2.0f, cieY);
	//HSV_to_RGB(h_, s_, v_, r_, g_, b_);

	double f = std::min(1.0f, cieY) / getY709(r_, g_, b_); //std::max(r_, std::max(g_, b_));
	r_ = std::min(1.0, std::max(0.0, f * r_));
	g_ = std::min(1.0, std::max(0.0, f * g_));
	b_ = std::min(1.0, std::max(0.0, f * b_));

	Color rgb(r_, g_, b_, Color::RGB, true);

	intensity = cieY;

	//Color xyY(ciex, ciey, cieY, Color::CIExyY, false);
	//cout << xyY << endl;
	//Color rgb = xyY.toRGB();
	////cout << rgb << endl;
#ifdef CUSTOM_NIGHT
	rgb.composite(m_FullMoonColor, 1.0f, dark);
	rgb.check();
#endif // CUSTOM
	return rgb;
}

CSP_NAMESPACE_END

