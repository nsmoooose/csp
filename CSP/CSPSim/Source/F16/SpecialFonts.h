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

#ifndef __SPECIAL_FONTS__
#define __SPECIAL_FONTS__

#include <osgText/Font>
#include <string>
#include <algorithm>


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
	 *
	 *  WARNING: It is untested and probably unwise to construct both a regular
	 *  font and a reverse font from the same font file.
	 */
	ReverseAltFont(std::string const &file) {
		Font *font = osgText::readFontFile(file);
		assert(font);
		setImplementation(font->getImplementation());
	}

	/** Construct a reverse font from an existing Font.  The source font should
	 *  not be accessed either before or after constructing the ReverseAltFont.
	 *  See the other constructor comment for more information.
	 */
	ReverseAltFont(Font *font): Font(font->getImplementation()) {
	}

	/** Override the default getGlyph method to generate reverse video
	 *  glyphs for character codes 128 through 255.  For character codes
	 *  less than 128 we use the base class implementation.  To generate
	 *  the reverse video glyphs we load the normal glyph for the character
	 *  code - 128 and use it to create a new glyph image with the alpha
	 *  value inverted.  The image is also expanded to fill the entire
	 *  character cell.
	 */
	virtual Glyph* getGlyph(unsigned int charcode) {
		SizeGlyphMap::iterator itr = _sizeGlyphMap.find(SizePair(_width,_height));
		if (itr != _sizeGlyphMap.end()) {
			GlyphMap& glyphmap = itr->second;
			GlyphMap::iterator gitr = glyphmap.find(charcode);
			if (gitr!=glyphmap.end()) return gitr->second.get();
		}
		if (_implementation.valid()) {
			if (charcode >= 128) {
				// use '0' as the reference glyph for sizing and centering the new glyph.  'M' would
				// probably be a better choice, but i haven't made that glyph yet in my test font ;-)
				Glyph *reference = getGlyph('0');
				Glyph *normal = getGlyph(charcode % 128);
				Glyph *reverse = new osgText::Font::Glyph;
				unsigned int reference_width = reference->s();
				unsigned int reference_height = reference->t();
				unsigned int source_width = normal->s();
				unsigned int source_height = normal->t();

				// the new glyph should fit the full character cell.  we expand the horizontal dimensions slightly
				// to compensate for texture blending at the edges of the glyph that might otherwise cause a gap
				// between adjacent reverse video characters.
				unsigned int cell_height = static_cast<unsigned int>(reference->getVerticalAdvance() + 0.5);
				unsigned int cell_width = static_cast<unsigned int>(reference->getHorizontalAdvance() + 0.5) + 2 * getGlyphImageMargin();

				// sanity checking
				cell_width = std::max(cell_width, source_width);
				cell_height = std::max(cell_height, source_height);
				unsigned int cell_data_size = cell_height * cell_width;
				unsigned int data_size = normal->getImageSizeInBytes();
				unsigned int bits_per_pixel = normal->getPixelSizeInBits();
				assert(bits_per_pixel == 8);
				assert(normal->r() == 1);
				assert(cell_height >= source_height);
				assert(cell_width >= source_width);
				assert(data_size = source_width * source_height);

				// create and initialize the image array for the new glyph
				unsigned char const *image = normal->data();
				unsigned char *data = new unsigned char[cell_data_size];
				for (unsigned char *init = data; init < data + cell_data_size; ) {
					*init++ = 0xff;  // opaque
				}

				// the new glyph cell is position based on the reference character.  this ensures that
				// the edges line up correctly for different characters.  the actual glyph needs to be
				// offset relative to the reference character based on the character metrics.  this
				// offset is hard-coded into the new image, since the metrics for the new glyph will
				// be based entirely on the reference glyph.
				osg::Vec2 glyph_offset = reference->getHorizontalBearing() - normal->getHorizontalBearing();
				unsigned int offset_top = static_cast<unsigned int>(std::max(0.0f, (cell_height - reference_height) / 2 - glyph_offset.y()));
				unsigned int offset_left = static_cast<unsigned int>(std::max(0.0f, (cell_width - reference_width) / 2 - glyph_offset.x()));
				unsigned char const *source = image;
				unsigned char *copy = data + offset_top * cell_width + offset_left;
				for (unsigned int j = 0; j < source_height; j++) {
					for (unsigned int i = 0; i < source_width; i++) {
						*copy++ = *source++ ^ 0xff;
					}
					copy += cell_width - source_width;
				}
				reverse->setImage(cell_width, cell_height, 1, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE, 1);

				// finally set the metrics such that the reference glyph would be centered in the
				// cell.  offsets for this specific glyph have already been accounted for when
				// generating the new glyph image.
				osg::Vec2 horizontal_bearing = reference->getHorizontalBearing();
				horizontal_bearing.x() -= (cell_width - reference_width) / 2;
				horizontal_bearing.y() -= (cell_height - reference_height) / 2;
				reverse->setHorizontalBearing(horizontal_bearing);

				// force uniform spacing, even if the font has variable width characters
				reverse->setHorizontalAdvance(reference->getHorizontalAdvance());

				// no adjustments to the vertical metrics for now; ReverseAltFont is intended for
				// horizontal text only.
				reverse->setVerticalBearing(reference->getVerticalBearing());
				reverse->setVerticalAdvance(reference->getVerticalAdvance());

				// finally add the new glyph so we don't repeat all this work!
				addGlyph(getWidth(), getHeight(), charcode, reverse);
				return reverse;
			} else {
				return _implementation->getGlyph(charcode);
			}
		}
		return 0;
	}
};


/** A specialized font for rendering normal and large video text.  Character
 *  codes 128-255 are scaled representations of the glyphs for glyphs for
 *  character codes 0-127.
 */
class ScaledAltFont: public osgText::Font {
	float m_Scale;
	bool m_VCenter;

public:
	ScaledAltFont(std::string const &file, float scale, bool center_vertical=true): m_Scale(scale), m_VCenter(center_vertical) {
		Font *font = osgText::readFontFile(file);
		assert(font);
		setImplementation(font->getImplementation());
	}

	ScaledAltFont(Font *font, float scale, bool center_vertical=true): Font(font->getImplementation()), m_Scale(scale), m_VCenter(center_vertical) { }

	/** Override the default getGlyph method to generate expanded
	 *  glyphs for character codes 128 through 255.  For character codes
	 *  less than 128 we use the base class implementation.
	 */
	virtual Glyph* getGlyph(unsigned int charcode) {
		SizeGlyphMap::iterator itr = _sizeGlyphMap.find(SizePair(_width,_height));
		if (itr != _sizeGlyphMap.end()) {
			GlyphMap& glyphmap = itr->second;
			GlyphMap::iterator gitr = glyphmap.find(charcode);
			if (gitr!=glyphmap.end()) return gitr->second.get();
		}
		if (_implementation.valid()) {
			if (charcode >= 128) {
				unsigned int normal_code = charcode % 128;
				// Ensure that the normal character is loaded.
				Glyph *normal_glyph = getGlyph(normal_code);
				// Reload a scaled version of the glyph
				unsigned int original_width = _width;
				unsigned int original_height = _height;
				unsigned int new_width = static_cast<unsigned int>(_width * m_Scale);
				unsigned int new_height = static_cast<unsigned int>(_height * m_Scale);
				setSize(new_width, new_height);
				Glyph *scaled_glyph = _implementation->getGlyph(normal_code);
				if (m_VCenter) {
					osg::Vec2 hbearing = scaled_glyph->getHorizontalBearing();
					hbearing.y() += 0.5f * (normal_glyph->t() - scaled_glyph->t());
					scaled_glyph->setHorizontalBearing(hbearing);
				}
				setSize(original_width, original_height);
				_sizeGlyphMap[SizePair(_width, _height)][charcode] = scaled_glyph;
				return scaled_glyph;
			} else {
				return _implementation->getGlyph(charcode);
			}
		}
		return 0;
	}
};

#endif // __SPECIAL_FONTS__

