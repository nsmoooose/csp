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
 * @file Colorspace.h
 *
 * The routines in this library come from a number of sources,
 * but a very good place to look is COLORS.F90, available from
 * http://www.psc.edu/~burkardt/src/colors/colors.html.
 * 
 **/

#ifndef __COLORSPACE_H__
#define __COLORSPACE_H__

/*
 * Colorspace conversions and corrections.
 */
void initColor();
void RGB_check(float &R, float &G, float &B);
void CMY_check(float &C, float &M, float &Y);
void CMY_to_RGB(float C, float M, float Y, float &R, float &G, float &B);
void RGB_to_CMY(float R, float G, float B, float &C, float &M, float &Y);
void HLS_check(float &H, float &L, float &S);
void HLS_to_RGB(float H, float L, float S, float &R, float &G, float &B);
void RGB_to_HLS(float R, float G, float B, float &H, float &L, float &S);
void RGB_to_HSV(float R, float G, float B, float &H, float &S, float &V);
void HSV_to_RGB(float H, float S, float V, float &R, float &G, float &B);
void HSV_check(float &H, float &S, float &V);
float RGB_to_hue(float R, float G, float B);
float T_to_spd(float T, float lambda);
void T_to_xyz(float T, float &x, float &y, float &z);
void uvp_to_xyz(float uprime, float vprime, float &x, float &y, float &z);
void uvpY_to_XYZ(float uprime, float vprime, float Y_, float &X, float &Y, float &Z);
void xy_to_uvwp(float x, float y, float &uprime, float &vprime, float &wprime);
void xyY_check(float &x, float &y, float &Y);
void xyY_to_XYZ(float x, float y, float Y_, float &X, float &Y, float &Z);
void XYZ_to_Luv(float X, float Y, float Z, float &Lstar, float &ustar, float &vstar);
void XYZ_to_RGB709(float X, float Y, float Z, float &R, float &G, float &B);
void RGB709_to_XYZ(float R, float G, float B, float &X, float &Y, float &Z);
float getY709(float R, float G, float B);
void XYZ_to_uvwp(float X, float Y, float Z, float &uprime, float &vprime, float &wprime);
void XYZ_to_xyY(float X, float Y, float Z, float &x, float &y, float &Y_);
void XYZ_check(float &X, float &Y, float &Z);
void Luv_to_XYZ(float Lstar, float ustar, float vstar, float &X, float &Y, float &Z);
void Luv_check(float &Lstar, float &ustar, float &vstar);


/**
 * A colorspace conversion class.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class Color {
	//static const float uwp = 4.0*xw / (15.0*yw+xw+3.0*zw);
	//static const float vwp = 9.0*yw / (15.0*yw+xw+3.0*zw);
	static const char *space_labels[];
public:
	typedef enum {INVALID, CIELab, CIELuv, CIExyY, CIEXYZ, CMY, HLS, HSV, RGB} space_t;

	/**
	 * Default constructor, sets black RGB.
	 */
	Color() {
		_set_nocheck(0.0, 0.0, 0.0, RGB);
	}
	
	/**
	 * Construct a new color using a specific colorspace.
	 */
	Color(float A, float B, float C, space_t S=RGB, bool check=true) {
		set(A, B, C, S, check);
	}

	/**
	 * Copy constructor.
	 */
	Color(const Color &color) {
		_set_nocheck(color.a, color.b, color.c, color.space);
	}

	/**
	 * Assignment (copy).
	 */
	const Color &operator=(const Color &color) {
		_set_nocheck(color.a, color.b, color.c, color.space);
		return *this;
	}

	/**
	 * Set to a new color using a specific colorspace.
	 */
	void set(float A, float B, float C, space_t S=RGB, bool check=true);
	
	/**
	 * Test if the colorspace is RGB.
	 */
	bool isRGB() const { return space == RGB; }
	
	/**
	 * Test if the colorspace s CMY.
	 */
	bool isCMY() const { return space == CMY; }

	/**
	 * Test if the colorspace is HLS.
	 */
	bool isHLS() const { return space == HLS; }
	
	/**
	 * Test if the colorspace is HSV.
	 */
	bool isHSV() const { return space == HSV; }
	
	/**
	 * Test if the colorspace is CIE L*a*b*.
	 */
	bool isCIELab() const { return space == CIELab; }
	
	/**
	 * Test if the colorspace is CIE L*u*v*.
	 */
	bool isCIELuv() const { return space == CIELuv; }
	
	/**
	 * Test if the colorspace is CIE xyY.
	 */
	bool isCEIxyY() const { return space == CIExyY; }
	
	/**
	 * Test if the colorspace is CIE XYZ.
	 */
	bool isCEIXYZ() const { return space == CIEXYZ; }

	/**
	 * Test for a specific colorspace.
	 */
	bool isSpace(space_t S) const { return space == S; }
	
	/**
	 * Get the color compenonents in the current colorspace.
	 */
	void get(float &A, float &B, float &C) const {
		A = a;
		B = b;
		C = c;
	}

	/**
	 * Get the first color compenent. (e.g. red in RGB)
	 */
	float getA() const { return a; }
	
	/**
	 * Get the second color compenent. (e.g. green in RGB)
	 */
	float getB() const { return b; }
	
	/**
	 * Get the first color compenent. (e.g. blue in RGB)
	 */
	float getC() const { return c; }

	/**
	 * Get the current colorspace. (e.g. RGB)
	 */
	space_t getSpace() const { return space; }

	/**
	 * Get the current colorspace as a text string (e.g. "CIE L*u*v*")
	 */
	const char *getSpaceLabel() const { return space_labels[space]; }

	/**
	 * Get the CIE luminance of using RGB709.
	 */
	float getLuminance() const {
		Color RGB = toRGB();
		return getY709(RGB.getA(), RGB.getB(), RGB.getC());
	}

	/**
	 * Convert to RGB colorspace.
	 *
	 * @param internal_check Correct color components at each colorspace conversion
	 *                       for conversions requiring intermediate colorspaces.  This
	 *                       should generally be set to true.
	 */
	Color toRGB(bool internal_check=true) const;
	
	/**
	 * Convert to CIE XYZ colorspace.
	 *
	 * @param internal_check Correct color components at each colorspace conversion
	 *                       for conversions requiring intermediate colorspaces.  This
	 *                       should generally be set to true.
	 */
	Color toCIEXYZ(bool internal_check=true) const;
	
	/**
	 * Convert to CIE xyY colorspace.
	 *
	 * @param internal_check Correct color components at each colorspace conversion
	 *                       for conversions requiring intermediate colorspaces.  This
	 *                       should generally be set to true.
	 */
	Color toCIExyY(bool internal_check=true) const;
	
	/**
	 * Convert to CIE L*u*v* colorspace.
	 *
	 * @param internal_check Correct color components at each colorspace conversion
	 *                       for conversions requiring intermediate colorspaces.  This
	 *                       should generally be set to true.
	 */
	Color toCIELuv(bool internal_check=true) const;
	
	/**
	 * Convert to HLS colorspace.
	 *
	 * @param internal_check Correct color components at each colorspace conversion
	 *                       for conversions requiring intermediate colorspaces.  This
	 *                       should generally be set to true.
	 */
	Color toHLS(bool internal_check=true) const;

	/**
	 * Dump to an output stream.
	 *
	 * Format is "[SPACE: A, B, C]".
	 */
	friend std::ostream &operator<<(std::ostream &os, const Color &c);

	/**
	 * Scale all components by a fixed value.
	 */
	void scale(float f);

	/**
	 * Blend two colors.
	 *
	 * @param color color to blend
	 * @param alpha blend factor (zero is unchanged, one is copy)
	 */
	void blend(Color const &color, float alpha);

	/**
	 * Composite two colors.
	 *
	 * @param color second color
	 * @param alpha1 amount of original color 
	 * @param alpha2 amount of second color 
	 */
	void composite(Color const &color, float alpha1, float alpha2);

	/**
	 * Force components to valid ranges.
	 */
	void check();
	
private:

	float a, b, c;
	space_t space;

	/**
	 * Assign new components in a specific colorspace without
	 * checknig and correcting their ranges.  
	 *
	 * Use internally if you already know the compenonts are
	 * valid.
	 */
	void _set_nocheck(float A, float B, float C, space_t S=RGB) {
		space = S;
		a = A;
		b = B;
		c = C;
	}

};
	

#endif // __COLORSPACE_H__

