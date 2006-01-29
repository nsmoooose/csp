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
 * @file cspsim/sound/OggLoader.cpp
 * @brief Loads OggVorbis sound files (.ogg).
 **/


#include <csp/cspsim/sound/Loader.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Uniform.h>

#include <openalpp/sample.h>
#include <vorbis/vorbisfile.h>

#include <csp/csplib/util/undef.h>

CSP_NAMESPACE

class OggLoader: public SoundFileLoader {
public:
	static bool loadOgg(std::string const &filename, std::string &data, ALenum &format, ALsizei &freq) {
		FILE *f = fopen(filename.c_str(), "rb");
		if (!f) {
			CSPLOG(ERROR, AUDIO) << "error opening file sound sample " << filename;
			return false;
		}
		OggVorbis_File ogg;
		ov_open(f, &ogg, NULL, 0);
		vorbis_info *info = ov_info(&ogg, -1);
		format = (info->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		freq = info->rate;
		const int endian = (CSP_LE == CSP_BYTE_ORDER) ? 0 : 1;
		int bs;
		while (1) {
			char buf[32*1024];
			int bytes = ov_read(&ogg, buf, sizeof(buf), endian, 2, 1, &bs);
			if (bytes == 0) break;
			data.insert(data.end(), buf, buf + bytes);
		}
		CSPLOG(DEBUG, AUDIO) << "loaded ogg sample " << filename << " (" << data.size() << " bytes)";
		ov_clear(&ogg);
		return true;
	}

private:

friend void __registerOgg();
	OggLoader() { registerExtension(this, ".ogg"); }

	virtual openalpp::Sample *_load(std::string const &filename) {
		std::string data;
		ALenum format;
		ALsizei freq;
		if (!loadOgg(filename, data, format, freq)) return 0;
		return new openalpp::Sample(format, (ALvoid*) data.data(), data.size(), freq);
	}
};

void __registerOgg() {
	static OggLoader __ogg;
}


CSP_NAMESPACE_END

