// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file SpecialFonts.h
 *
 * Defines osgText::Font subclasses for specialized font rendering.  The
 * general approach is to provide modified glyphs of character codes 0
 * to 127 as pseudo-characters in the range 128 to 255.  Setting the
 * high bit of the (8-bit) character code results in an alternate rendering
 * of the original glyph.
 *
 **/

#ifndef __CSPSIM_F16_SPECIAL_FONTS__
#define __CSPSIM_F16_SPECIAL_FONTS__

#include <csp/csplib/util/Namespace.h>

#include <osgText/Font>
#include <string>

namespace csp {

/** A specialized font for rendering normal and reverse video text.  Character
 *  codes 128-255 are reverse video representations of the glyphs for glyphs
 *  for character codes 0-127.
 */
class ReverseAltFont: public osgText::Font {
public:
	/** Construct a reverse font from the specified font file.  The first 128
	 *  glyphs are rendered normally, while the second 128 glyphs are rendered
	 *  as reverse-video copies of the first 128 glyphs.  The original glyphs
	 *  above character 127 are not available.
	 */
	static osgText::Font* load(std::string const &filename);

	/** Override the default getGlyph method to generate reverse video
	 *  glyphs for character codes 128 through 255.  For character codes
	 *  less than 128 we use the base class implementation.  To generate
	 *  the reverse video glyphs we load the normal glyph for the character
	 *  code - 128 and use it to create a new glyph image with the alpha
	 *  value inverted.  The image is also expanded to fill the entire
	 *  character cell.
	 */
	virtual Glyph* getGlyph(unsigned int charcode);

	~ReverseAltFont();
private:
	// Created only via load()
	ReverseAltFont();
};


/** A specialized font for rendering normal and large video text.  Character
 *  codes 128-255 are scaled representations of the glyphs for glyphs for
 *  character codes 0-127.
 */
class ScaledAltFont: public osgText::Font {
public:
	static osgText::Font* load(std::string const &filename, float scale, bool center_vertical=true);

	/** Override the default getGlyph method to generate expanded
	 *  glyphs for character codes 128 through 255.  For character codes
	 *  less than 128 we use the base class implementation.
	 */
	virtual Glyph* getGlyph(unsigned int charcode);

private:
	// Created only via load()
	ScaledAltFont(float scale, bool center_vertical): m_Scale(scale), m_VCenter(center_vertical) { }

	float m_Scale;
	bool m_VCenter;
};

} // namespace csp

#endif // __SPECIAL_FONTS__

