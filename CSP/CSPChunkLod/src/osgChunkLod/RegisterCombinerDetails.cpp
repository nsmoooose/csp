/*
 * RegisterCombinerDetails.cpp
 *
 * Copyright 2003 Mark Rose <mkrose@users.sourceforge.net>
 * 
 * The osgChunkLod library is open source and may be redistributed and/or modified
 * under the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
 * 
 * Modifications by Mark Rose <mkrose@users.sourceforge.net>, May 2003,
 * see include/osgChunkLod/ChunkLod for details.
 *
 */

#include <osg/Texture2D>
#include <osg/Image>
#include <osg/TexGen>

#ifdef USE_NV
#include <osgNV/Version>
#include <osgNVExt/RegisterCombiners>
#include <osgNVExt/CombinerInput>
#include <osgNVExt/CombinerOutput>
#include <osgNVExt/FinalCombinerInput>
#endif

#include <osgDB/ReadFile>
#include <string>

#include <osgChunkLod/MultiTextureDetails>

namespace osgChunkLod {

class RegisterCombinerDetails: public MultiTextureDetails {
public:

#ifdef USE_NV
	RegisterCombinerDetails() {
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &_textureUnitCount);
	}

	void init(osg::StateSet *ss) {
		_rc = new osgNVExt::RegisterCombiners;
		_rc->setNumCombiners(2);
		osg::Vec3 light_dir(0.0, 0.707, 0.707);
		_rc->setConstantColor0(osg::Vec4(light_dir, 0));
		_rc->setConstantColor1(osg::Vec4(1,1,1,1));

		_rc->addParameter(new osgNVExt::CombinerInput(0, "RGB A CONSTANT_COLOR_0 SIGNED_IDENTITY NORMAL RGB")); // L
		_rc->addParameter(new osgNVExt::CombinerInput(0, "RGB B TEXTURE_1 EXPAND NORMAL RGB")); // NORMAL
		_rc->addParameter(new osgNVExt::CombinerInput(0, "RGB C PRIMARY_COLOR EXPAND NORMAL RGB")); // H
		_rc->addParameter(new osgNVExt::CombinerInput(0, "RGB D TEXTURE_1 EXPAND NORMAL RGB")); // NORMAL
		_rc->addParameter(new osgNVExt::CombinerOutput(0, "RGB SPARE_0 SPARE_1 DISCARD SCALE_NONE BIAS_NONE 1 1 0"));

		_rc->addParameter(new osgNVExt::CombinerInput(1, "RGB A TEXTURE_0 EXPAND NORMAL RGB")); // DIFFUSE
		_rc->addParameter(new osgNVExt::CombinerInput(1, "RGB B SPARE_0 UNSIGNED_IDENTITY NORMAL RGB")); // N.L 
		_rc->addParameter(new osgNVExt::CombinerInput(1, "RGB C SPARE_1 UNSIGNED_IDENTITY NORMAL RGB")); // N.H
		_rc->addParameter(new osgNVExt::CombinerInput(1, "RGB D SPARE_1 UNSIGNED_IDENTITY NORMAL RGB")); // N.H
		_rc->addParameter(new osgNVExt::CombinerOutput(1, "RGB SPARE_0 SPARE_1 DISCARD SCALE_NONE BIAS_NONE 0 0 0"));

		// TODO alpha mux
		//_rc->addParameter(new osgNV::CombinerInput(1, "ALPHA A SPARE_1 UNSIGNED_IDENTITY NORMAL RGB")); // N.H

		_rc->addParameter(new osgNVExt::FinalCombinerInput("A CONSTANT_COLOR_1 UNSIGNED_IDENTITY RGB"));
		_rc->addParameter(new osgNVExt::FinalCombinerInput("B E_TIMES_F        UNSIGNED_IDENTITY RGB"));
		_rc->addParameter(new osgNVExt::FinalCombinerInput("C ZERO             UNSIGNED_IDENTITY RGB"));
		_rc->addParameter(new osgNVExt::FinalCombinerInput("D SPARE_0          UNSIGNED_IDENTITY RGB"));
		_rc->addParameter(new osgNVExt::FinalCombinerInput("E SPARE_1          UNSIGNED_IDENTITY RGB"));
		_rc->addParameter(new osgNVExt::FinalCombinerInput("F SPARE_1          UNSIGNED_IDENTITY RGB"));
		_rc->addParameter(new osgNVExt::FinalCombinerInput("G ZERO             UNSIGNED_INVERT   ALPHA"));

		ss->setAttributeAndModes(_rc.get());
	}


	void enable(osg::State &state) const {
		int n = static_cast<int>(_detailTextures.size());
		for (int i = 0; i < n; i++) {
			state.applyTextureAttribute (i+1, _detailTextures[i].get());
			state.applyTextureMode (i+1, GL_TEXTURE_2D, true); // new

//			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
	
			state.applyTextureMode (i+1, GL_TEXTURE_GEN_S, true); // new
			state.applyTextureMode (i+1, GL_TEXTURE_GEN_T, true); // new
		}
	}

	void disable(osg::State &state) const {
		int n = static_cast<int>(_detailTextures.size());
		for (int i = 0; i < n; i++) {
			state.applyTextureMode (i+1, GL_TEXTURE_2D, false); // new
			state.applyTextureMode (i+1, GL_TEXTURE_GEN_S, false); // new
			state.applyTextureMode (i+1, GL_TEXTURE_GEN_T, false); // new
		}
	}

	void setMode(osg::State &state, bool enabled) const {
		int n = static_cast<int>(_detailTextures.size());
		for (int i = 0; i < n; i++) {
			state.applyTextureMode(i+1, GL_TEXTURE_2D, enabled);
		}
	}


protected:
	osg::ref_ptr<osgNVExt::RegisterCombiners> _rc;
#endif

};

} // namespace osgChunkLod


