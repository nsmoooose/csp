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

#include "Colorspace.h"
#include <cstdio>

#define PI 3.14159265358979323846264338327950288419716939937510

float Xn, Yn, Zn;
float unprime, vnprime, wnprime;

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
enum { CIE_A, CIE_B, CIE_C, CIE_D50, CIE_D55, CIE_D65, CIE_E } CIE_illuminants;

/**
 * CIE xyz coordinates of various CIE defined standard illuminants.
 */
float whitepoint_xyz[][3] = {
	{ 0.448, 0.407, 0.145 },          /* A   */
	{ 0.349, 0.352, 0.299 },          /* B   */
	{ 0.3101, 0.3162, 0.3737},        /* C   */
	{ 0.34570, 0.35854, 0.29576},     /* D50 */
	{ 0.3324, 0.3474, 0.3202},        /* D55 */
	{ 0.31271, 0.32899, 0.35830},     /* D65 */
	{ 0.333333, 0.333333, 0.333334},  /* E   */
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
void setWhite(float x, float y, float z) {
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
	if (x > 0.0) return pow(x, (1.0/3.0));
	if (x == 0.0) return 0.0;
	return -pow(fabs(x), (1.0/3.0));
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
	R = 1.0 - C;
	G = 1.0 - M;
	B = 1.0 - Y;
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
	C = 1.0 - R;
	M = 1.0 - G;
	Y = 1.0 - B;
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
		return n1 + (n2 - n1) * H / 60.0;
	}
	if (H < 180.0) {
		return n2;
	}
	if (H < 240.0) {
		return n1 + (n2 - n1) * (240.0 - H) / 60.0;
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
	m1 = 2.0 * L - m2;
	if (S == 0.0) {
		R = G = B = L;
	} else {
		R = hls_value(m1, m2, H + 120.0);
		G = hls_value(m1, m2, H);
		B = hls_value(m1, m2, H - 120.0);
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
	L = (rgbmax + rgbmin) * 0.5;
	if (d == 0.0) {
		S = 0.0;
		H = 0.0;
	} else {
		if (L <= 0.5) {
			S = d / (rgbmax + rgbmin);
		} else {
			S = d / (2.0 - rgbmax - rgbmin);
		}
		float s = 1.0 / d;
		rc = (rgbmax - R) * s;
		gc = (rgbmax - G) * s;
		bc = (rgbmax - B) * s;
		if (R == rgbmax) {
			H = bc - gc;
		} else 
		if (G == rgbmax) {
			H = 2.0 + rc - bc;
		} else {
			H = 4.0 + gc - rc;
		}
		H = H * 60.0;
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
		float hue = rmodp(H, 360.0) / 60.0;
		int i = (int) hue;
		float f = hue - (float) i;
		float p = V * (1.0 - S);
		float q = V * (1.0 - S * f);
		float t = V * (1.0 - S + S * f);
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
		float s = 1.0 / d;
		rc = (rgbmax - R) * s;
		gc = (rgbmax - G) * s;
		bc = (rgbmax - B) * s;
		if (R == rgbmax) {
			H = bc - gc;
		} else 
		if (G == rgbmax) {
			H = 2.0 + rc - bc;
		} else {
			H = 4.0 + gc - rc;
		}
		H = rmodp(H * 60.0, 360.0);
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
		H = 2.0 + R - B;
	} else {
		H = 4.0 + G - R;
	}
	H = H / 6.0;
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
	static float c = 2.9979246E+08;
	static float h = 6.626176E-34;
	static float k = 1.38066E-23;
	static float nmtom = 1.0E-09;
	float expon = h * c / ( nmtom * lambda * k * T );
	float denom = pow(nmtom,4.0) * pow(lambda, 5.0) * (exp(expon) - 1.0);
	float power = 8.0 * PI * h * c / denom;
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
		1000.0,  1200.0,  1400.0,  1500.0,  1600.0,
		1700.0,  1800.0,  1900.0,  2000.0,  2100.0,
		2200.0,  2300.0,  2400.0,  2500.0,  2600.0,
		2700.0,  2800.0,  2900.0,  3000.0,  3100.0,
		3200.0,  3300.0,  3400.0,  3500.0,  3600.0,
		3700.0,  3800.0,  3900.0,  4000.0,  4100.0,
		4200.0,  4300.0,  4400.0,  4500.0,  4600.0,
		4700.0,  4800.0,  4900.0,  5000.0,  5200.0,
		5400.0,  5600.0,  5800.0,  6000.0,  6500.0,
		7000.0,  7500.0,  8000.0,  8500.0,  9000.0,
		10000.0, 15000.0, 30000.0 
	};
	static const float xdat[] = {
		0.6526, 0.6249, 0.5984, 0.5856, 0.5731,
		0.5610, 0.5491, 0.5377, 0.5266, 0.5158,
		0.5055, 0.4956, 0.4860, 0.4769, 0.4681,
		0.4597, 0.4517, 0.4441, 0.4368, 0.4299,
		0.4233, 0.4170, 0.4109, 0.4052, 0.3997,
		0.3945, 0.3896, 0.3848, 0.3804, 0.3760,
		0.3719, 0.3680, 0.3643, 0.3607, 0.3573,
		0.3540, 0.3509, 0.3479, 0.3450, 0.3397,
		0.3347, 0.3301, 0.3259, 0.3220, 0.3135,
		0.3063, 0.3003, 0.2952, 0.2908, 0.2869,
		0.2806, 0.2637, 0.2501
	};
	static const float ydat[] = {
		0.3446, 0.3676, 0.3859, 0.3932, 0.3993,
		0.4043, 0.4083, 0.4112, 0.4133, 0.4146,
		0.4152, 0.4152, 0.4147, 0.4137, 0.4123,
		0.4106, 0.4086, 0.4064, 0.4041, 0.4015,
		0.3989, 0.3962, 0.3935, 0.3907, 0.3879,
		0.3851, 0.3822, 0.3795, 0.3767, 0.3740,
		0.3713, 0.3687, 0.3660, 0.3635, 0.3610,
		0.3586, 0.3562, 0.3539, 0.3516, 0.3472,
		0.3430, 0.3391, 0.3353, 0.3318, 0.3236,
		0.3165, 0.3103, 0.3048, 0.2999, 0.2956,
		0.2883, 0.2673, 0.2489
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
	z = 1.0 - x - y;
}

/**
 * Convert CIE XYZ to RGB709 color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void XYZ_to_RGB709(float X, float Y, float Z, float &R, float &G, float &B) {
	R =  3.240479*X-1.537150*Y-0.498535*Z;
	G = -0.969256*X+1.875992*Y+0.041556*Z;
	B =  0.055648*X-0.204043*Y+1.057311*Z;
}

/**
 * Convert RGB709 to CIE XYZ color coordinates.
 *
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void RGB709_to_XYZ(float R, float G, float B, float &X, float &Y, float &Z) {
	X = 0.412453*R+0.357580*G+0.180423*B;
	Y = 0.212671*R+0.715160*G+0.072169*B;
	Z = 0.019334*R+0.119193*G+0.950227*B;
}

/**
 * Convert RGB709 to CIEXYZ luminance Y.
 */
float getY709(float R, float G, float B) {
	return 0.212671*R + 0.71516*G + 0.072169*B;
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
		float z = 1.0 - x - y;
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
				Lstar = 903.3 * Y / Yn;
			} else
				if (Y <= Yn) {
					Lstar = 116.0 * cubert(Y/Yn) - 16.0;
				} else {
					Lstar = 100.0;
				}
			float uprime, vprime, wprime;
			XYZ_to_uvwp(X, Y, Z, uprime, vprime, wprime);
			ustar = 13.0 * Lstar * (uprime - unprime);
			vstar = 13.0 * Lstar * (vprime - vnprime);
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
			Y = Lstar * Yn / 903.3;
		} else
			if (Lstar <= 100.0) {
				Y = Yn * pow((Lstar + 16.0) / 116.0, 3.0);
			} else {
				Y = Yn;
			}
		float uprime = unprime + ustar / (13.0 * Lstar);
		float vprime = vnprime + vstar / (13.0 * Lstar);
		uvpY_to_XYZ(uprime, vprime, Y, X, Y, Z);
	}
}

/**
 * Correct out-of-range CIE L*u*v* color coordinates.
 */
void Luv_check(float &Lstar, float &ustar, float &vstar) {
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
	float denom = X + 15.0 * Y + 3.0 * Z;
	if (denom == 0.0) {
		uprime = vprime = wprime = 0.0;
	} else {
		uprime = 4.0 * X / denom;
		vprime = 9.0 * Y / denom;
		wprime = (-3.0 * X + 6.0 * Y + 3.0 * Z) / denom;
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
	X = 9.0 * Y * uprime / (4.0 * vprime);
	Z = - X / 3.0 - 5.0 * Y + 3.0 * Y / vprime;
}

/**
 * Convert CIE u'v' to CIE xyz color coordinates.
 * 
 * The CIE XYZ color system describes a color in terms of its components
 * of X, Y and Z primaries.  In ordinary circumstances, all three of
 * these components must be nonnegative.
 */
void uvp_to_xyz(float uprime, float vprime, float &x, float &y, float &z) {
	float denom = 6.0 * uprime - 16.0 * vprime + 12.0;
	if (denom == 0.0) {
		x = y = z = 0.0;
	} else {
		x = 9.0 * uprime / denom;
		y = 4.0 * vprime / denom;
		z = (-3.0 * uprime - 20.0 * vprime + 12.0) / denom;
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
	float denom = -2.0 * x + 12.0 * y + 3.0;
	if (denom == 0.0) {
		uprime = vprime = wprime = 0.0;
	} else {
		uprime = 4.0 * x / denom;
		vprime = 9.0 * y / denom;
		wprime = (-6.0 * x + 3.0 * y + 3.0) / denom;
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
};

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
			H = atan2(c, b);
			L = a;
			S = sqrt(b+c);
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
	composite(color, 1.0-alpha, alpha);
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

