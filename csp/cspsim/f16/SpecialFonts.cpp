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
 * @file SpecialFonts.cpp
 *
 **/

#include <csp/cspsim/f16/SpecialFonts.h>
#include <csp/cspsim/Config.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/thread/Synchronization.h>

#include <osg/Version>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <algorithm>
#include <cassert>
#include <map>

namespace csp {

namespace {

// osgText::readFontFile caches results by filename, but sharing a single FontImplementation
// between multiple special font classes does not work (FontImplementation supports only a
// single facade).  Instead we manage our own cache that is keyed on both filename and special
// font type.  Note that font path lookup currently uses the config font directory (as set in
// the .ini file) rather than the full osg search path.
class SpecialFontCache {
public:
	static void clear() {
		MutexLock lock(m_Mutex);
		m_Cache.clear();
	}

	static osgText::Font* load(std::string const &filename, std::string const &cache_prefix, osgText::Font *prototype) {
		MutexLock lock(m_Mutex);
		std::string key(cache_prefix + filename);
		osgText::Font *font = get(key);
		if (font) return font;
		std::string fontdir = getDataPath("FontPath");
		std::string fontpath = ospath::join(fontdir, filename);
		if (!ospath::exists(fontpath)) {
			CSPLOG(ERROR, DATA) << "Unable to find font " << fontpath;
			return 0;
		}
		// force the file to be reloaded; the osg cached version (if any) is used for
		// regular osgText::Font instances.
		osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
		options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
		osg::Object* object = osgDB::readObjectFile(fontpath, options.get());
		font = dynamic_cast<osgText::Font*>(object);
		if (!font) {
			CSPLOG(ERROR, DATA) << "Invalid font " << fontpath;
			// if the object has zero references then delete it by doing another unref().
			if (object && object->referenceCount()==0) object->unref();
			return 0;
		}

		// grab a reference to the implementation and delete the original font instance.
		// we must delete the original instance before assigning the implementation to
		// the prototype, since the Font destructor clears implementation->facade_.
		// this is a hack, much like the entire caching scheme implemented here.  it
		// would be nice if osg provided a better mechanism for creating custom font
		// overrides.  of course an alternative approach would be to simply specialize
		// the fonts themselves rather than the rendering code, but that is not very
		// flexible.
		osg::ref_ptr<osgText::Font::FontImplementation> implementation(font->getImplementation());
		{ osg::ref_ptr<osgText::Font> cleaner(font); } // ref and unref to delete.

		assert(prototype);
		CSPLOG(INFO, DATA) << "Assigning font implementation " << font->getImplementation() << " to " << prototype;
		prototype->setImplementation(implementation.get());
		set(key, prototype);
		return prototype;
	}

private:
	typedef std::map<std::string, osg::ref_ptr<osgText::Font> > CacheMap;
	static CacheMap m_Cache;
	static Mutex m_Mutex;

	static osgText::Font *get(std::string const &key) {
		CacheMap::iterator iter = m_Cache.find(key);
		return (iter == m_Cache.end()) ? 0 : iter->second.get();
	}

	static void set(std::string const &key, osgText::Font *font) {
		m_Cache.insert(std::make_pair(key, osg::ref_ptr<osgText::Font>(font)));
	}
};

SpecialFontCache::CacheMap SpecialFontCache::m_Cache;
Mutex SpecialFontCache::m_Mutex;

} // namespace

osgText::Font* ReverseAltFont::load(std::string const &filename) {
	osg::ref_ptr<osgText::Font> prototype = new ReverseAltFont;
	return SpecialFontCache::load(filename, "REV:", prototype.get());
}

osgText::Font* ScaledAltFont::load(std::string const &filename, float scale, bool center_vertical) {
	std::string prefix = stringprintf("SCA(%e,%d):", scale, center_vertical ? 1 : 0);
	osg::ref_ptr<osgText::Font> prototype = new ScaledAltFont(scale, center_vertical);
	return SpecialFontCache::load(filename, prefix, prototype.get());
}

ReverseAltFont::ReverseAltFont() {
	CSPLOG(INFO, DATA) << "ReverseAltFont " << this;
}

ReverseAltFont::~ReverseAltFont() {
	CSPLOG(INFO, DATA) << "~ReverseAltFont " << this << ", imp=" << getImplementation();
}

osgText::Glyph* ReverseAltFont::getGlyph(const osgText::FontResolution& fontRes, unsigned int charcode) {
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_glyphMapMutex);
		FontSizeGlyphMap::iterator itr = _sizeGlyphMap.find(fontRes);
		if (itr != _sizeGlyphMap.end()) {
			GlyphMap& glyphmap = itr->second;
			GlyphMap::iterator gitr = glyphmap.find(charcode);
			if (gitr!=glyphmap.end()) return gitr->second.get();
		}
	}
	if (_implementation.valid()) {
		if (charcode >= 128) {
			// use '0' as the reference glyph for sizing and centering the new glyph.  'M' would
			// probably be a better choice, but i haven't made that glyph yet in my test font ;-)
			osgText::Glyph *reference = getGlyph(fontRes, '0');
			osgText::Glyph *normal = getGlyph(fontRes, charcode % 128);
			osgText::Glyph *reverse = new osgText::Glyph(this, 0);
			int reference_width = reference->s();
			int reference_height = reference->t();
			int source_width = normal->s();
			int source_height = normal->t();

			// the new glyph should fit the full character cell.  we expand the horizontal dimensions slightly
			// to compensate for texture blending at the edges of the glyph that might otherwise cause a gap
			// between adjacent reverse video characters.
			int cell_height = static_cast<int>(reference->getVerticalAdvance() + 0.5);
			int cell_width = static_cast<int>(reference->getHorizontalAdvance() + 0.5) + 2 * getGlyphImageMargin();

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
			for (int j = 0; j < source_height; j++) {
				for (int i = 0; i < source_width; i++) {
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
			addGlyph(fontRes, charcode, reverse);
			return reverse;
		} else {
			osgText::Glyph *g = _implementation->getGlyph(fontRes,charcode);
			addGlyph (fontRes, charcode, g);
			return g;
		}
	}
	return 0;
}

osgText::Glyph* ScaledAltFont::getGlyph(const osgText::FontResolution& fontRes, unsigned int charcode) {
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_glyphMapMutex);
		FontSizeGlyphMap::iterator itr = _sizeGlyphMap.find(fontRes);
		if (itr != _sizeGlyphMap.end()) {
			GlyphMap& glyphmap = itr->second;
			GlyphMap::iterator gitr = glyphmap.find(charcode);
			if (gitr!=glyphmap.end()) return gitr->second.get();
		}
	}
	if (_implementation.valid()) {
		if (charcode >= 128) {
			unsigned int normal_code = charcode % 128;
			// Reload a scaled version of the glyph
			osgText::Glyph *normal_glyph = getGlyph(fontRes, normal_code);

			const osgText::FontResolution new_fontRes(fontRes.first * m_Scale, fontRes.second * m_Scale);
			osgText::Glyph *scaled_glyph = _implementation->getGlyph(new_fontRes, normal_code);
			if (m_VCenter) {
				osg::Vec2 hbearing = scaled_glyph->getHorizontalBearing();
				hbearing.y() += 0.5f * (normal_glyph->t() - scaled_glyph->t());
				scaled_glyph->setHorizontalBearing(hbearing);
			}
			addGlyph (fontRes, charcode, scaled_glyph);
			return scaled_glyph;
		} else {
			osgText::Glyph* g = _implementation->getGlyph(fontRes, charcode);
			addGlyph (fontRes, charcode, g);
			return g;
		}
	}
	return 0;
}

} // namespace csp
