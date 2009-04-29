
#include <csp/cspsim/sky/SkyShader.h>
#include <csp/csplib/util/Math.h>
#include <iostream>

namespace csp {

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
	m_OverLuminescence = 1.2f;
	m_HaloSharpness = 0.3f; //0.5f;
	m_SunsetSharpness = 50.0f;
	m_SunsetElevation = static_cast<float>( -5.0 * (PI/180.0) );  // half-intensity elevation
	m_NightBase = 0.01f;
	//m_DarkSkyColor = Color(0.008f, 0.035f, 0.140f, Color::RGB);
	//m_DarkSkyColor = Color(0.008f, 0.086f, 0.183f, Color::RGB);
	m_DarkSkyColor = Color(0.008f, 0.016f, 0.183f, Color::RGB);
	//m_DarkSkyColor = Color(0.004f, 0.008f, 0.0915f, Color::RGB);
	m_DarkAdjustment = 0.0f;
	setTurbidity(TURBIDITY);
	setSunElevation(0.0);
}

void SkyShader::getSkyCoefficients(float T, sky_c &skylight) {
	skylight.x[0] = -0.01925f * T - 0.25922f;
	skylight.x[1] = -0.06651f * T + 0.00081f;
	skylight.x[2] = -0.00041f * T + 0.21247f;
	skylight.x[3] = -0.06409f * T - 0.89887f;
	skylight.x[4] = -0.00325f * T + 0.04517f;
	skylight.y[0] = -0.01669f * T - 0.26078f;
	skylight.y[1] = -0.09496f * T + 0.00921f;
	skylight.y[2] = -0.00792f * T + 0.21023f;
	skylight.y[3] = -0.04405f * T - 1.65369f;
	skylight.y[4] = -0.01092f * T + 0.05291f;
	skylight.Y[0] =  0.17872f * T - 1.46303f;
	skylight.Y[1] = -0.35540f * T + 0.42749f;
	skylight.Y[2] = -0.02266f * T + 5.32505f;
	skylight.Y[3] =  0.12064f * T - 2.57705f;
	skylight.Y[4] = -0.06696f * T + 0.37027f;
}

float SkyShader::FasterF(float cos_theta, float gamma, float cos_gamma, coeff &p) {
#ifdef CUSTOM_F
	cos_theta = fabs(cos_theta) + 0.09f;
#endif
	return (1.0f + p[0]*exp(p[1]/cos_theta))*(1.0f+p[2]*exp(p[3]*gamma)+p[4]*cos_gamma*cos_gamma);
}


float SkyShader::F(float theta, float gamma, coeff &p) {
	float cos_g = cos(gamma);
	//gamma *= std::max(1.0, 10.0 * std::min(1.0, 10.0 * sin(m_SunElevation)));
#ifdef CUSTOM_F
	float cos_t = fabs(cos(theta)) + 0.09f;
#else
	float cos_t = cos(theta);
#endif
	return (1.0f + p[0]*exp(p[1]/cos_t))*(1.0f+p[2]*exp(p[3]*gamma)+p[4]*cos_g*cos_g);
}

float SkyShader::FastPerez(float theta, float gamma, float factor, float z, coeff &p) {
	if (theta > 1.57f) theta = 1.57f;
	return factor * z * F(theta, gamma, p);
}

float SkyShader::Perez(float theta, float gamma, float theta_s, float z, coeff &p) {
	if (theta > 1.57f) theta = 1.57f;
	//if (theta_s > 1.57f) theta_s = 1.57f;
	//if (theta_s >  1.50f) theta_s = 1.50f + ((theta_s-1.50f) * 0.5f);
	float denom = F(0.0f, theta_s, p);
	if (denom != 0.0f) z /= denom;
	return z * F(theta, gamma, p);
}

Color SkyShader::getZenith(float T, float theta_s) {
	float s = theta_s;
	float s2 = s*s;
	float s3 = s2*s;
	float t = T;
	float t2 = t*t;
	float xa =  0.00166f * s3 - 0.00375f * s2 + 0.00209f * s;
	float xb = -0.02903f * s3 + 0.06377f * s2 - 0.03202f * s + 0.00394f;
	float xc =  0.11693f * s3 - 0.21196f * s2 + 0.06052f * s + 0.25886f;
	float xz = xa * t2 + xb * t + xc;
	float ya =  0.00275f * s3 - 0.00610f * s2 + 0.00317f * s;
	float yb = -0.04214f * s3 + 0.08970f * s2 - 0.04153f * s + 0.00516f;
	float yc =  0.15346f * s3 - 0.26756f * s2 + 0.06670f * s + 0.26688f;
	float yz = ya * t2 + yb * t + yc;
	float chi = static_cast<float>( (0.4444444 - T * 0.008333333) * (PI - 2.0*s) );
	float Yz = (4.0453f * T - 4.9710f) * tan(chi) - 0.2155f * T + 2.4192f;
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
		h += static_cast<float>( 2.0 * PI * int((PI - h) / (2.0 * PI)) );
	} else
	if (h > PI) {
		h -= static_cast<float>( 2.0 * PI * int((PI + h) / (2.0 * PI)) );
	}
	if (h > 0.5 *PI) {
		h = static_cast<float>( PI - h );
		m_AzimuthCorrection = static_cast<float>( PI );
	} else
	if (h < -0.5*PI) {
		h = static_cast<float>( -PI - h );
		m_AzimuthCorrection =  static_cast<float>( -PI );
	}
	m_SunElevation = h;
	m_DarkAdjustment = clampTo(6.0f * (m_SunElevation - toRadians(-25.0f)), 0.0f, 1.0f);
	m_Dirty = true;
}

void SkyShader::_computeBase() {
	m_Dirty = false;

	m_SunTheta = static_cast<float>( 0.5*PI - m_SunElevation );
	m_SunVector[0] = 0.0f;
	m_SunVector[1] = sin(m_SunTheta);
	m_SunVector[2] = cos(m_SunTheta);

	getSkyCoefficients(m_Turbidity, m_Coefficients);
	
	float theta = m_SunTheta;
	//if (theta >  1.50) theta = 1.50 + ((m_SunTheta-1.50) * 0.5);
	float denom;
	denom = F(0.0f, theta, m_Coefficients.x);
	if (denom == 0.0f) denom = 1.0f;
	m_PerezFactor.x = 1.0f / denom;
	denom = F(0.0f, theta, m_Coefficients.y);
	if (denom == 0.0f) denom = 1.0f;
	m_PerezFactor.y = 1.0f / denom;
	denom = F(0.0f, theta, m_Coefficients.Y);
	if (denom == 0.0f) denom = 1.0f;
	m_PerezFactor.Y = 1.0f / denom;

	m_Zenith = getZenith(m_Turbidity, m_SunTheta);
	m_MaxY = 1.0f;

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
	if (Y_ == 0.0f) {
		X = Z = 0.0f;
	} else {
		// any check for y == 0.0?
		X = x * Y / y;
		if (X < 0.0f) X = 0.0f;
		float z = 1.0f - x - y;
		Z = z * Y / y;
		if (Z < 0.0f) Z = 0.0f;
	}
}


Color SkyShader::SkyColor(float elevation, float azimuth, float &intensity) {
	if (m_Dirty) _computeBase();
	float theta = static_cast<float>( 0.5*PI - elevation );
	float A = azimuth + m_AzimuthCorrection;
	float v[3] = {sin(A)*sin(theta), cos(A)*sin(theta), cos(theta)};
	float dot = v[0]*m_SunVector[0]+v[1]*m_SunVector[1]+v[2]*m_SunVector[2];
	// numerical error can cause abs(dot) to exceed 1...
	if (dot < -1.0f) dot = -1.0f; else
	if (dot >  1.0f) dot =  1.0f;
	float gamma = acos(dot);

#ifdef SLOW_PEREZ
	float ciex = FastPerez(theta, gamma, m_PerezFactor.x, m_Zenith.getA(), m_Coefficients.x);
	float ciey = FastPerez(theta, gamma, m_PerezFactor.y, m_Zenith.getB(), m_Coefficients.y);
	float cieY = FastPerez(theta, gamma, m_PerezFactor.Y, m_Zenith.getC(), m_Coefficients.Y);
#else
	float cos_theta = cos(std::min(theta, 1.5708f));
	float ciex = m_PerezFactor.x * m_Zenith.getA() * FasterF(cos_theta, gamma, dot, m_Coefficients.x);
	float ciey = m_PerezFactor.y * m_Zenith.getB() * FasterF(cos_theta, gamma, dot, m_Coefficients.y);
	float cieY = m_PerezFactor.Y * m_Zenith.getC() * FasterF(cos_theta, gamma, dot, m_Coefficients.Y);
#endif

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
	_xyY_to_XYZ(ciex, ciey, 1.0f, X_, Y_, Z_);
	_XYZ_to_RGB709(X_, Y_, Z_, r_, g_, b_);

	double f = std::min(1.0f, cieY) / getY709(r_, g_, b_);
	r_ = static_cast<float>( std::min(1.0, std::max(0.0, f * r_)) );
	g_ = static_cast<float>( std::min(1.0, std::max(0.0, f * g_)) );
	b_ = static_cast<float>( std::min(1.0, std::max(0.0, f * b_)) );

	Color rgb(r_, g_, b_, Color::RGB, true);

	intensity = cieY;

	if (m_DarkAdjustment > 0.0f) {
		rgb.composite(m_DarkSkyColor, 1.0f, m_DarkAdjustment);
		rgb.check();
	}
	return rgb;
}

} // namespace csp

