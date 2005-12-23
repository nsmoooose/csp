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
 * @file Colorspace.cpp
 *
 * The vast majority of the colorspace manipulation routines
 * included here are adapted from Fortran90 functions written
 * by John Burkardt.  His excellent colorspace library is
 * available from the Pittsburgh Supercomputing Center:
 *
 *    http://www.psc.edu/~burkardt/src/colors/colors.html
 *
 * Translation of John's routines from Fortran90 to C/C++
 * was done by Mark Rose <mrose@stm.lbl.gov>.
 */

#include <csp/cspsim/Colorspace.h>

#include <cassert>
#include <cmath>
#include <cstdio>


CSP_NAMESPACE

// global whitepoint
namespace {
	float wpPI = 3.14159265358979f;
	float Xn, Yn, Zn;
	float unprime, vnprime, wnprime;
}


/**
 * CIE defined standard illuminants.
 *
 * 'A', the CIE illuminant A, light from a tungsten lamp, at 500 watts;
 *
 * 'B', the CIE illuminant B, direct sunlight, at 500 watts;
 *
 * 'C', the CIE illuminant C, average sunlight, at 500 watts;
 * This is used as the reference white for NTSC color encoding.
 *
 * 'D50' or 'D5000', the CIE illuminant used in graphics printing,
 * bright tunsten illumination;
 *
 * 'D55' or 'D5500', a CIE illuminant that approximates a cloudy bright
 * day;
 *
 * 'D65' or 'D6500', the CIE illuminant that approximates daylight;
 * This is used as the reference white for SMPTE, PAL/EBU, and HDTV
 * color encoding.
 *
 * 'E', the CIE illuminant E, normalized reference source.
 */
typedef enum { CIE_A, CIE_B, CIE_C, CIE_D50, CIE_D55, CIE_D65, CIE_E } CIE_illuminants;

/**
 * CIE xyz coordinates of various CIE defined standard illuminants.
 */
float whitepoint_xyz[][3] = {
	{ 0.448f, 0.407f, 0.145f },          /* A   */
	{ 0.349f, 0.352f, 0.299f },          /* B   */
	{ 0.3101f, 0.3162f, 0.3737f},        /* C   */
	{ 0.34570f, 0.35854f, 0.29576f},     /* D50 */
	{ 0.3324f, 0.3474f, 0.3202f},        /* D55 */
	{ 0.31271f, 0.32899f, 0.35830f},     /* D65 */
	{ 0.333333f, 0.333333f, 0.333334f},  /* E   */
};

/**
 * Set the whitepoint reference.
 *
 * In additive image reproduction, the white point is the chromaticity of
 * the colour reproduced by equal red, green and blue components.  White
 * point is a function of the ratio (or balance) of power among the
 * primaries. In subtractive reproduction, white is the SPD of the
 * illumination, multiplied by the SPD of the media. There is no unique
 * physical or perceptual definition of white, so to achieve accurate
 * colour interchange you must specify the characteristics of your white.
 *
 * It is often convenient for purposes of calculation to define white as
 * a uniform SPD. This white reference is known as the equal-energy
 * illuminant, or CIE Illuminant E.
 *
 * (text excerpt from Poynton's Color FAQ, Copyright Charles Poynton 1997)
 */
void setWhite(float x, float y, float /*z*/) {
	float X, Y, Z;
	xyY_to_XYZ(x, y, 1.0, X, Y, Z);
	XYZ_check(X, Y, Z);
	Xn = X;
	Yn = Y;
	Zn = Z;
	XYZ_to_uvwp(X, Y, Z, unprime, vnprime, wnprime);
}

/**
 * Set the whitepoint reference to CIE illuminant D65.
 *
 * You should use this unless you have a good reason to use something else.
 * The print industry commonly uses D50 and photography commonly uses D55.
 * These represent compromises between the conditions of indoor (tungsten)
 * and daylight viewing.
 *
 * (text excerpt from Poynton's Color FAQ, Copyright Charles Poynton 1997)
 */
void setWhiteD65() {
	float* xyz = whitepoint_xyz[CIE_D65];
	setWhite(xyz[0], xyz[1], xyz[2]);
}

/**
 * Cube root respecting sign.
 */
static float cubert(float x) {
	if (x > 0.0) return powf(x, (1.0f/3.0f));
	if (x == 0.0) return 0.0;
	return -powf(fabsf(x), (1.0f/3.0f));
}

/**
 * Floating point version of x mod y.
 */
static float rmodp(float x, float y) {
	assert(y != 0.0);
	int n = int(x/y);
	if (n==0) return x;
	if (n<0) n--;
	return x - n*y;
}

#define LIMIT_ZERO_TO_ONE(x) \
	if (x < 0.0) x = 0.0; else if (x > 1.0) x = 1.0;
	

/**
 * Correct out-of-range RGB color coordinates.
 */
void RGB_check(float &R, float &G, float &B) {
	LIMIT_ZERO_TO_ONE(R);
	LIMIT_ZERO_TO_ONE(G);
	LIMIT_ZERO_TO_ONE(B);
}

/**
 * Correct out-of-range CMY color coordinates.
 */
void CMY_check(float &C, float &M, float &Y) {
	LIMIT_ZERO_TO_ONE(C);
	LIMIT_ZERO_TO_ONE(M);
	LIMIT_ZERO_TO_ONE(Y);
}

/**
 * Convert CMY to RGB.
 *
 * The CMY color system describes a color based on the amounts of the
 * base colors cyan, magenta, and yellow.  Thus, a particular color
 * has three coordinates, (C,M,Y).  Each coordinate must be between
 * 0 and 1.  Black is (1,1,1) and white is (0,0,0).
 *
 * The RGB color system describes a color based on the amounts of the
 * base colors red, green, and blue.  Thus, a particular color
 * has three coordinates, (R,G,B).  Each coordinate must be between
 * 0 and 1.
 */
void CMY_to_RGB(float C, float M, float Y, float &R, float &G, float &B) {
	R = 1.0f - C;
	G = 1.0f - M;
	B = 1.0f - Y;
}

/**
 * Convert RGB to CMY.
 *
 * The CMY color system describes a color based on the amounts of the
 * base colors cyan, magenta, and yellow.  Thus, a particular color
 * has three coordinates, (C,M,Y).  Each coordinate must be between
 * 0 and 1.  Black is (1,1,1) and white is (0,0,0).
 *
 * The RGB color system describes a color based on the amounts of the
 * base colors red, green, and blue.  Thus, a particular color
 * has three coordinates, (R,G,B).  Each coordinate must be between
 * 0 and 1.
 */
void RGB_to_CMY(float R, float G, float B, float &C, float &M, float &Y) {
	C = 1.0f - R;
	M = 1.0f - G;
	Y = 1.0f - B;
}

/**
 * Range check and correct HLS values.
 */
void HLS_check(float &H, float &L, float &S) {
	H = rmodp(H, 360.0);
	LIMIT_ZERO_TO_ONE(L);
	LIMIT_ZERO_TO_ONE(S);
}

/**
 * A utility function used by HLS_to_RGB.
 */
static float hls_value(float n1, float n2, float H) {
	H = rmodp(H, 360.0);
	if (H < 60.0) {
		return n1 + (n2 - n1) * H / 60.0f;
	}
	if (H < 180.0) {
		return n2;
	}
	if (H < 240.0) {
		return n1 + (n2 - n1) * (240.0f - H) / 60.0f;
	}
	return n1;
}

/**
 * Convert HLS to RGB.
 *
 * The HLS color system describes a color based on the qualities of
 * hue, lightness, and saturation.  A particular color has three
 * coordinates, (H,L,S).  The L and S coordinates must be between
 * 0 and 1, while the H coordinate must be between 0 and 360, and
 * is interpreted as an angle.
 *
 * The RGB color system describes a color based on the amounts of the
 * base colors red, green, and blue.  Thus, a particular color
 * has three coordinates, (R,G,B).  Each coordinate must be between
 * 0 and 1.
 */
void HLS_to_RGB(float H, float L, float S, float &R, float &G, float &B) {
	float m1, m2;
	if (L <= 0.5) {
		m2 = L + L*S;
	} else {
		m2 = L + S - L*S;
	}
	m1 = 2.0f * L - m2;
	if (S == 0.0) {
		R = G = B = L;
	} else {
		R = hls_value(m1, m2, H + 120.0f);
		G = hls_value(m1, m2, H);
		B = hls_value(m1, m2, H - 120.0f);
	}
}

/**
 * Convert RGB to HLS.
 *
 * The HLS color system describes a color based on the qualities of
 * hue, lightness, and saturation.  A particular color has three
 * coordinates, (H,L,S).  The L and S coordinates must be between
 * 0 and 1, while the H coordinate must be between 0 and 360, and
 * is interpreted as an angle.
 *
 * The RGB color system describes a color based on the amounts of the
 * base colors red, green, and blue.  Thus, a particular color
 * has three coordinates, (R,G,B).  Each coordinate must be between
 * 0 and 1.
 */
void RGB_to_HLS(float R, float G, float B, float &H, float &L, float &S) {
	float rgbmax = (R > G && R > B) ? R : ((G > B) ? G : B);
	float rgbmin = (R < G && R < B) ? R : ((G < B) ? G : B);
	float d = (rgbmax - rgbmin);
	float rc, gc, bc;
	L = (rgbmax + rgbmin) * 0.5f;
	if (d == 0.0) {
		S = 0.0;
		H = 0.0;
	} else {
		if (L <= 0.5) {
			S = d / (rgbmax + rgbmin);
		} else {
			S = d / (2.0f - rgbmax - rgbmin);
		}
		float s = 1.0f / d;
		rc = (rgbmax - R) * s;
		gc = (rgbmax - G) * s;
		bc = (rgbmax - B) * s;
		if (R == rgbmax) {
			H = bc - gc;
		} else
		if (G == rgbmax) {
			H = 2.0f + rc - bc;
		} else {
			H = 4.0f + gc - rc;
		}
		H = H * 60.0f;
	}
}

/**
 * Correct out-of-range HSV color coordinates.
 */
void HSV_check(float &H, float &S, float &V) {
	H = rmodp(H, 360.0);
	LIMIT_ZERO_TO_ONE(S);
	LIMIT_ZERO_TO_ONE(V);
}

/**
 * Convert HSV to RGB.
 *
 * The HSV color system describes a color based on the three qualities
 * of hue, saturation, and value.  A given color will be represented
 * by three numbers, (H,S,V).  H, the value of hue, is an angle
 * between 0 and 360 degrees, with 0 representing red.  S is the
 * saturation, and is between 0 and 1.  Finally, V is the "value",
 * a measure of brightness, which goes from 0 for black, increasing
 * to a maximum of 1 for the brightest colors.  The HSV color system
 * is sometimes also called HSB, where the B stands for brightness.
 *
 * The RGB color system describes a color based on the amounts of the
 * base colors red, green, and blue.  Thus, a particular color
 * has three coordinates, (R,G,B).  Each coordinate must be between
 * 0 and 1.
 */
void HSV_to_RGB(float H, float S, float V, float &R, float &G, float &B) {
	if (S == 0.0) {
		R = G = B = V;
	} else {
		float hue = rmodp(H, 360.0) / 60.0f;
		int i = (int) hue;
		float f = hue - (float) i;
		float p = V * (1.0f - S);
		float q = V * (1.0f - S * f);
		float t = V * (1.0f - S + S * f);
		switch (i) {
			case 0:
				R = V;
				G = t;
				B = p;
				break;
			case 1:
				R = q;
				G = V;
				B = p;
				break;
			case 2:
				R = p;
				G = V;
				B = t;
				break;
			case 3:
				R = p;
				G = q;
				B = V;
				break;
			case 4:
				R = t;
				G = p;
				B = V;
				break;
			case 5:
				R = V;
				G = p;
				B = q;
				break;
			default:
				assert(0);
		}
	}
}

/**
 * Convert RGB to HSV.
 *
 * The HSV color system describes a color based on the three qualities
 * of hue, saturation, and value.  A given color will be represented
 * by three numbers, (H,S,V).  H, the value of hue, is an angle
 * between 0 and 360 degrees, with 0 representing red.  S is the
 * saturation, and is between 0 and 1.  Finally, V is the "value",
 * a measure of brightness, which goes from 0 for black, increasing
 * to a maximum of 1 for the brightest colors.  The HSV color system
 * is sometimes also called HSB, where the B stands for brightness.
 *
 * The RGB color system describes a color based on the amounts of the
 * base colors red, green, and blue.  Thus, a particular color
 * has three coordinates, (R,G,B).  Each coordinate must be between
 * 0 and 1.
 */
void RGB_to_HSV(float R, float G, float B, float &H, float &S, float &V) {
	float rgbmax = (R > G && R > B) ? R : ((G > B) ? G : B);
	float rgbmin = (R < G && R < B) ? R : ((G < B) ? G : B);
	float d = (rgbmax - rgbmin);
	V = rgbmax;
	if (V != 0.0) {
		S = d / V;
	} else {
		S = 0.0;
	}
	if (S == 0.0) {
		H = 0.0;
	} else {
		float rc, gc, bc;
		float s = 1.0f / d;
		rc = (rgbmax - R) * s;
		gc = (rgbmax - G) * s;
		bc = (rgbmax - B) * s;
		if (R == rgbmax) {
			H = bc - gc;
		} else
		if (G == rgbmax) {
			H = 2.0f + rc - bc;
		} else {
			H = 4.0f + gc - rc;
		}
		H = rmodp(H * 60.0f, 360.0);
	}
}

/**
 * Convert RGB to a hue value between 0 and 1.
 *
 * The hue computed here should be the same as the H value computed
 * for HLS and HSV, except that it ranges from 0 to 1 instead of
 * 0 to 360.
 *
 * A monochromatic color ( white, black, or a shade of gray) does not
 * have a hue.  This routine will return a special value of H = -1
 * for such cases.
 */
float RGB_to_hue(float R, float G, float B) {
	LIMIT_ZERO_TO_ONE(R);
	LIMIT_ZERO_TO_ONE(G);
	LIMIT_ZERO_TO_ONE(B);
	float rgbmax = (R > G && R > B) ? R : ((G > B) ? G : B);
	float rgbmin = (R < G && R < B) ? R : ((G < B) ? G : B);
	float d = (rgbmax - rgbmin);
	float H;
	if (d == 0.0)  return -1.0; // no hue
	if (R == rgbmax) {
		H = B - G;
	} else
	if (G == rgbmax) {
		H = 2.0f + R - B;
	} else {
		H = 4.0f + G - R;
	}
	H = H / 6.0f;
	LIMIT_ZERO_TO_ONE(H);
	return H;
}


/**
 * Evaluates the black body power spectrum at a given temperature.
 *
 * Planck's law gives the spectral power distribution function of
 * radiation from a black body, per unit volume per infinitesimal
 * increment of wavelength, as:
 *
 *     SPD(Lambda,T)
 *        = 1/Volume * dPower / dLambda
 *        = 8 * Pi * H * C /
 *          ( (10**(-9))**4 * Lambda**5 * ( EXP ( P ) - 1 ) )
 *
 * where
 *
 *   P = H * C / ( ( Lambda * 10**(-9) ) * K * T );
 *   Lambda = Wavelength, in nanometers;
 *   T = Temperature of the black body, in degrees Kelvin;
 *   H = Planck's constant, in joule-seconds;
 *   C = Speed of light, in meters/second;
 *   K = Boltzmann's constant, in joules / degrees Kelvin;
 *   Volume = Volume of the cavity, in cubic meters.
 */
float T_to_spd(float T, float lambda) {
	static float c = 2.9979246E+08f;
	static float h = 6.626176E-34f;
	static float k = 1.38066E-23f;
	static float nmtom = 1.0E-09f;
	float expon = h * c / ( nmtom * lambda * k * T );
	float denom = powf(nmtom,4.0f) * powf(lambda, 5.0f) * (expf(expon) - 1.0f);
	float power = 8.0f * wpPI * h * c / denom;
	return power;
}

/**
 * Simple linear interpolation in a table.
 *
 * @param n The size of the tables.
 * @param x The value of the independent variable.
 * @param X The tabulated values X values, which should be in
 *          ascending order.
 * @param Y The tabulated values Y values.
 * @returns The interpolated Y value.
 */
static float interp(int n, float x, const float *X, const float *Y) {
	if (x < X[0]) return Y[0];
	if (x > X[n-1]) return Y[n-1];
	for (int i=0; i < n; i++) {
		if (X[i] <= x && x <= X[i+1]) {
			return ((X[i+1]-x)*Y[i]+(x-X[i])*Y[i+1])/(X[i+1]-X[i]);
		}
	}
	return Y[n-1];
}

/**
 * Returns CIE xyz chromaticities for black body radiation.
 *
 * The CIE xyz system defines a color in terms of its normalized
 * chromaticities (x,y,z), without reference to the absolute strength
 * or luminance of the color.
 *
 * @param T Temperature, in degrees Kelvin, of the black body.
 *          Data is only available for T in the range of 1,000 to
 *          30,000 degrees.  Input values of T outside this range
 *          will result in output values of X and Y at the nearest
 *          endpoint of the data range.
 */
void T_to_xyz(float T, float &x, float &y, float &z) {
	static const int n = 53;
	static const float Tdat[] = {
		1000.0f,  1200.0f,  1400.0f,  1500.0f,  1600.0f,
		1700.0f,  1800.0f,  1900.0f,  2000.0f,  2100.0f,
		2200.0f,  2300.0f,  2400.0f,  2500.0f,  2600.0f,
		2700.0f,  2800.0f,  2900.0f,  3000.0f,  3100.0f,
		3200.0f,  3300.0f,  3400.0f,  3500.0f,  3600.0f,
		3700.0f,  3800.0f,  3900.0f,  4000.0f,  4100.0f,
		4200.0f,  4300.0f,  4400.0f,  4500.0f,  4600.0f,
		4700.0f,  4800.0f,  4900.0f,  5000.0f,  5200.0f,
		5400.0f,  5600.0f,  5800.0f,  6000.0f,  6500.0f,
		7000.0f,  7500.0f,  8000.0f,  8500.0f,  9000.0f,
		10000.0f, 15000.0f, 30000.0f
	};
	static const float xdat[] = {
		0.6526f, 0.6249f, 0.5984f, 0.5856f, 0.5731f,
		0.5610f, 0.5491f, 0.5377f, 0.5266f, 0.5158f,
		0.5055f, 0.4956f, 0.4860f, 0.4769f, 0.4681f,
		0.4597f, 0.4517f, 0.4441f, 0.4368f, 0.4299f,
		0.4233f, 0.4170f, 0.4109f, 0.4052f, 0.3997f,
		0.3945f, 0.3896f, 0.3848f, 0.3804f, 0.3760f,
		0.3719f, 0.3680f, 0.3643f, 0.3607f, 0.3573f,
		0.3540f, 0.3509f, 0.3479f, 0.3450f, 0.3397f,
		0.3347f, 0.3301f, 0.3259f, 0.3220f, 0.3135f,
		0.3063f, 0.3003f, 0.2952f, 0.2908f, 0.2869f,
		0.2806f, 0.2637f, 0.2501f
	};
	static const float ydat[] = {
		0.3446f, 0.3676f, 0.3859f, 0.3932f, 0.3993f,
		0.4043f, 0.4083f, 0.4112f, 0.4133f, 0.4146f,
		0.4152f, 0.4152f, 0.4147f, 0.4137f, 0.4123f,
		0.4106f, 0.4086f, 0.4064f, 0.4041f, 0.4015f,
		0.3989f, 0.3962f, 0.3935f, 0.3907f, 0.3879f,
		0.3851f, 0.3822f, 0.3795f, 0.3767f, 0.3740f,
		0.3713f, 0.3687f, 0.3660f, 0.3635f, 0.3610f,
		0.3586f, 0.3562f, 0.3539f, 0.3516f, 0.3472f,
		0.3430f, 0.3391f, 0.3353f, 0.3318f, 0.3236f,
		0.3165f, 0.3103f, 0.3048f, 0.2999f, 0.2956f,
		0.2883f, 0.2673f, 0.2489f
	};
	if (T < Tdat[0]) {
		x = xdat[0];
		y = ydat[0];
	} else
		if (T > Tdat[n-1]) {
			x = xdat[n-1];
			y = ydat[n-1];
		} else {
			x = interp(n, T, Tdat, xdat);
			y = interp(n, T, Tdat, ydat);
		}
	z = 1.0f - x - y;
}

/**
 * Convert CIE XYZ to RGB709 color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void XYZ_to_RGB709(float X, float Y, float Z, float &R, float &G, float &B) {
	R =  3.240479f*X-1.537150f*Y-0.498535f*Z;
	G = -0.969256f*X+1.875992f*Y+0.041556f*Z;
	B =  0.055648f*X-0.204043f*Y+1.057311f*Z;
}

/**
 * Convert RGB709 to CIE XYZ color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void RGB709_to_XYZ(float R, float G, float B, float &X, float &Y, float &Z) {
	X = 0.412453f*R+0.357580f*G+0.180423f*B;
	Y = 0.212671f*R+0.715160f*G+0.072169f*B;
	Z = 0.019334f*R+0.119193f*G+0.950227f*B;
}

/**
 * Convert RGB709 to CIEXYZ luminance Y.
 */
float getY709(float R, float G, float B) {
	return 0.212671f*R + 0.71516f*G + 0.072169f*B;
}

/**
 * Convert CIE xyY to CIE XYZ color coordinates.
 *
 * The CIE xyY system defines a color in terms of its normalized
 * chromaticities (x,y), plus the value of Y, which allows the
 * normalizing factor to be determined.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void xyY_to_XYZ(float x, float y, float Y_, float &X, float &Y, float &Z) {
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

/**
 * Correct out-of-range CIE xyY color coordinates.
 */
void xyY_check(float &x, float &y, float &Y) {
	if (Y < 0.0) Y = 0.0;
	if (x < 0.0) x = 0.0;
	if (y < 0.0) y = 0.0;
	float sum = x + y;
	if (sum > 1.0) {
		x /= sum;
		y /= sum;
	}
}

/**
 * Correct out-of-range CIE XYZ color coordinates.
 */
void XYZ_check(float &X, float &Y, float &Z) {
	if (X < 0.0) X = 0.0;
	if (Y < 0.0) Y = 0.0;
	if (Z < 0.0) Z = 0.0;
}

/**
 * Convert CIE XYZ to CIE xyY color coordinates.
 *
 * The CIE xyY system defines a color in terms of its normalized
 * chromaticities (x,y), plus the value of Y, which allows the
 * normalizing factor to be determined.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void XYZ_to_xyY(float X, float Y, float Z, float &x, float &y, float &Y_) {
	float sum = X + Y + Z;
	if (sum == 0.0) {
		x = y = 0.0;
	} else {
		x = X / sum;
		y = Y / sum;
	}
	Y_ = Y;
}

/**
 * Convert CIE XYZ to CIE L*u*v* color coordinates.
 *
 * The CIE L*u*v* color system describes a color based on three
 * qualities: L* is CIE lightness, u* and v* contain chromatic
 * information.  A given color will be represented by three
 * numbers, (L*,u*,v*).  L* ranges between 0 and 100.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void XYZ_to_Luv(float X, float Y, float Z, float &Lstar, float &ustar, float &vstar) {
	if (Y == 0.0) {
		Lstar = ustar = vstar = 0.0;
	} else {
		if (Y <= 0.0) {
			Lstar = 0.0;
		} else
			if (Y <= 0.008856 * Yn) {
				Lstar = 903.3f * Y / Yn;
			} else
				if (Y <= Yn) {
					Lstar = 116.0f * cubert(Y/Yn) - 16.0f;
				} else {
					Lstar = 100.0;
				}
			float uprime, vprime, wprime;
			XYZ_to_uvwp(X, Y, Z, uprime, vprime, wprime);
			ustar = 13.0f * Lstar * (uprime - unprime);
			vstar = 13.0f * Lstar * (vprime - vnprime);
	}
}

/**
 * Convert CIE L*u*v* to CIE XYZ color coordinates.
 *
 * The CIE L*u*v* color system describes a color based on three
 * qualities: L* is CIE lightness, u* and v* contain chromatic
 * information.  A given color will be represented by three
 * numbers, (L*,u*,v*).  L* ranges between 0 and 100.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void Luv_to_XYZ(float Lstar, float ustar, float vstar, float &X, float &Y, float &Z) {
	if (Lstar <= 0.0) {
		X = Y = Z = 0.0;
	} else {
		if (Lstar <= 903.3 * 0.008856) {
			Y = Lstar * Yn / 903.3f;
		} else
			if (Lstar <= 100.0) {
				Y = Yn * powf((Lstar + 16.0f) / 116.0f, 3.0f);
			} else {
				Y = Yn;
			}
		float uprime = unprime + ustar / (13.0f * Lstar);
		float vprime = vnprime + vstar / (13.0f * Lstar);
		uvpY_to_XYZ(uprime, vprime, Y, X, Y, Z);
	}
}

/**
 * Correct out-of-range CIE L*u*v* color coordinates.
 */
void Luv_check(float &Lstar, float &/*ustar*/, float &/*vstar*/) {
	if (Lstar < 0.0) Lstar = 0.0;
	else if (Lstar > 100.0) Lstar = 100.0;
}

/**
 * Convert CIE XYZ to CIE u'v'w' color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void XYZ_to_uvwp(float X, float Y, float Z, float &uprime, float &vprime, float &wprime) {
	float denom = X + 15.0f * Y + 3.0f * Z;
	if (denom == 0.0) {
		uprime = vprime = wprime = 0.0;
	} else {
		uprime = 4.0f * X / denom;
		vprime = 9.0f * Y / denom;
		wprime = (-3.0f * X + 6.0f * Y + 3.0f * Z) / denom;
	}
}

/**
 * Convert CIE u'v'Y to CIE XYZ color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void uvpY_to_XYZ(float uprime, float vprime, float Y_, float &X, float &Y, float &Z) {
	Y = Y_;
	X = 9.0f * Y * uprime / (4.0f * vprime);
	Z = - X / 3.0f - 5.0f * Y + 3.0f * Y / vprime;
}

/**
 * Convert CIE u'v' to CIE xyz color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void uvp_to_xyz(float uprime, float vprime, float &x, float &y, float &z) {
	float denom = 6.0f * uprime - 16.0f * vprime + 12.0f;
	if (denom == 0.0) {
		x = y = z = 0.0;
	} else {
		x = 9.0f * uprime / denom;
		y = 4.0f * vprime / denom;
		z = (-3.0f * uprime - 20.0f * vprime + 12.0f) / denom;
	}
}

/**
 * Convert CIE xy to CIE u'v'w' color coordinates.
 *
 * The CIE xy system defines a color in terms of its normalized
 * chromaticities (x,y), without reference to the absolute strength
 * or luminance of the color.
 */
void xy_to_uvwp(float x, float y, float &uprime, float &vprime, float &wprime) {
	float denom = -2.0f * x + 12.0f * y + 3.0f;
	if (denom == 0.0) {
		uprime = vprime = wprime = 0.0;
	} else {
		uprime = 4.0f * x / denom;
		vprime = 9.0f * y / denom;
		wprime = (-6.0f * x + 3.0f * y + 3.0f) / denom;
	}
}

/**
 * Initialize the color system.
 */
void initColor() {
	setWhiteD65();
}


/*--------------------------------------------------------------*/


const char * Color::space_labels[] = {
	"INVALID", "CIELab", "CIEL*u*v*", "CIExyY", "CIEXYZ", "CMY",
	"HLS", "HSV", "RGB"};

void Color::set(float A, float B, float C, space_t S, bool check) {
	if (check) {
		switch (S) {
			case RGB:
				RGB_check(A, B, C);
				break;
			case HSV:
				HSV_check(A, B, C);
				break;
			case HLS:
				HLS_check(A, B, C);
				break;
			case CMY:
				CMY_check(A, B, C);
				break;
			case CIEXYZ:
				XYZ_check(A, B, C);
				break;
			case CIExyY:
				xyY_check(A, B, C);
				break;
			case CIELuv:
				Luv_check(A, B, C);
				break;
			default:
				assert(0);
		}
	}
	space = S;
	a = A;
	b = B;
	c = C;
}


Color Color::toRGB(bool internal_check) const {
	float R, G, B;
	float X, Y, Z;
	switch (space) {
		case RGB:
			return *this;
		case CMY:
			CMY_to_RGB(a, b, c, R, G, B);
			break;
		case HLS:
			HLS_to_RGB(a, b, c, R, G, B);
			break;
		case HSV:
			HSV_to_RGB(a, b, c, R, G, B);
			break;
		case CIEXYZ: //convert to RGB709
			XYZ_to_RGB709(a, b, c, R, G, B);
			break;
		case CIExyY:
			xyY_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_RGB709(X, Y, Z, R, G, B);
			break;	
		case CIELuv:
			Luv_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_RGB709(X, Y, Z, R, G, B);
			break;	
		default:
			return Color(0.0, 0.0, 0.0, INVALID);
	}
	RGB_check(R, G, B);
	return Color(R, G, B, RGB);
}

Color Color::toCIEXYZ(bool internal_check) const {
	float X, Y, Z;
	float R, G, B;
	switch (space) {
		case CIEXYZ:
			return *this;
		case RGB:
			RGB709_to_XYZ(a, b, c, X, Y, Z);
			break;
		case HLS:
			HLS_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			break;
		case HSV:
			HSV_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			break;
		case CMY:
			CMY_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			break;
		case CIExyY:
			xyY_to_XYZ(a, b, c, X, Y, Z);
			break;
		case CIELuv:
			Luv_to_XYZ(a, b, c, X, Y, Z);
			break;
		default:
			return Color(0.0, 0.0, 0.0, INVALID);
	}
	XYZ_check(X, Y, Z);
	return Color(X, Y, Z, CIEXYZ);
}


Color Color::toCIExyY(bool internal_check) const {
	float x, y;
	float R, G, B;
	float X, Y, Z;
	switch (space) {
		case CIExyY:
			return *this;
		case CIEXYZ:
			XYZ_to_xyY(a, b, c, x, y, Y);
			break;
		case CIELuv:
			Luv_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_xyY(X, Y, Z, x, y, Y);
			break;
		case RGB:
			RGB709_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_xyY(X, Y, Z, x, y, Y);
			break;
		case CMY:
			CMY_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_xyY(X, Y, Z, x, y, Y);
			break;
		case HLS:
			HLS_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_xyY(X, Y, Z, x, y, Y);
			break;
		case HSV:
			HSV_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_xyY(X, Y, Z, x, y, Y);
			break;
		default:
			return Color(0.0, 0.0, 0.0, INVALID);
	}
	xyY_check(x, y, Y);
	return Color(x, y, Y, CIExyY);
}

Color Color::toCIELuv(bool internal_check) const {
	float L, u, v;
	float X, Y, Z;
	float R, G, B;
	switch (space) {
		case CIELuv:
			return *this;
		case CIExyY:
			xyY_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_Luv(X, Y, Z, L, u, v);
			break;
		case CIEXYZ:
			XYZ_to_Luv(a, b, c, L, u, v);
			break;
		case RGB:
			RGB709_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_Luv(X, Y, Z, L, u, v);
		case CMY:
			CMY_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_Luv(X, Y, Z, L, u, v);
			break;
		case HLS:
			HLS_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_Luv(X, Y, Z, L, u, v);
			break;
		case HSV:
			HSV_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB709_to_XYZ(R, G, B, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_Luv(X, Y, Z, L, u, v);
			break;
		default:
			return Color(0.0, 0.0, 0.0, INVALID);
	}
	Luv_check(L, u, v);
	return Color(L, u, v, CIELuv);
}

Color Color::toHLS(bool internal_check) const {
	float H, L, S;
	float R, G, B;
	float X, Y, Z;
	switch (space) {
		case HLS:
			return *this;
		case CIELuv:
			H = atan2f(c, b);
			L = a;
			S = sqrtf(b+c);
			break;
		case CIExyY:
			xyY_to_XYZ(a, b, c, X, Y, Z);
			if (internal_check) XYZ_check(X, Y, Z);
			XYZ_to_RGB709(X, Y, Z, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB_to_HLS(R, G, B, H, L, S);
			break;
		case CIEXYZ:
			XYZ_to_RGB709(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB_to_HLS(R, G, B, H, L, S);
			break;
		case RGB:
			RGB_to_HLS(a, b, c, H, L, S);
			break;
		case CMY:
			CMY_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB_to_HLS(R, G, B, H, L, S);
			break;
		case HSV:
			HSV_to_RGB(a, b, c, R, G, B);
			if (internal_check) RGB_check(R, G, B);
			RGB_to_HLS(R, G, B, H, L, S);
			break;
		default:
			return Color(0.0, 0.0, 0.0, INVALID);
	}
	HLS_check(H, L, S);
	return Color(H, L, S, HLS);
}

std::ostream &operator<<(std::ostream &os, const Color &c) {
	os << "[" << c.getSpaceLabel() << ": "
		<< c.getA() << ", "
		<< c.getB() << ", "
		<< c.getC() << "]";
	return os;
}

void Color::scale(float f) {
	a *= f;
	b *= f;
	c *= f;
}

void Color::blend(Color const &color, float alpha) {
	composite(color, 1.0f-alpha, alpha);
}

void Color::composite(Color const &color, float alpha1, float alpha2) {
	a = a * alpha1 + color.getA() * alpha2;
	b = b * alpha1 + color.getB() * alpha2;
	c = c * alpha1 + color.getC() * alpha2;
}

void Color::check() {
	switch (space) {
		case RGB:
			RGB_check(a, b, c);
			break;
		case CIExyY:
			xyY_check(a, b, c);
			break;
		case CIEXYZ:
			XYZ_check(a, b, c);
			break;
		case HLS:
			HLS_check(a, b, c);
			break;
		case CIELuv:
			Luv_check(a, b, c);
			break;
		case CMY:
			CMY_check(a, b, c);
			break;
		case HSV:
			HSV_check(a, b, c);
			break;
		default:
			break;
	}
}

CSP_NAMESPACE_END

