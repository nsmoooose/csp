// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

#include <csp/cspsim/sky/PhaseShader.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/Timing.h>

#include <osg/Image>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/Texture2D>

namespace csp {


class PhaseShader::Callback: public osg::NodeCallback {
public:
	Callback(PhaseShader *shader, osg::Texture2D *texture, osg::Vec3 const &dir_to_sun, double ambient): m_Shader(shader), m_Texture(texture), m_DirectionToSun(dir_to_sun), m_Ambient(ambient), m_FrameCount(0), m_Index(0) {
		assert(shader);
		assert(texture);
		m_StartTime = get_realtime();
		m_UpdateTime = 0.0;
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		bool done = false;
		if (nv && nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR) {
			++m_FrameCount;
			// 1024 pixels should be in the neighborhood of 1 ms on decent hardware (circa 2005).
			// for a 256x256 image this spreads the update over 64 frames.
			const int pixels = 1024;
			double t0 = get_realtime();
			if (m_Shader->incrementalReshade(m_DirectionToSun, m_Ambient, pixels, m_Index)) {
				m_Texture->setImage(m_Shader->getShadedImage());
				done = true;
			}
			m_UpdateTime += get_realtime() - t0;
		}
		traverse(node, nv);
		if (done) {
			if (node && node->getUpdateCallback() == this) {
				node->setUpdateCallback(0);
				double elapsed = get_realtime() - m_StartTime;
				CSPLOG(Prio_DEBUG, Cat_SCENE) << "Imposter phase shading complete after " << m_FrameCount << " frames, " << int(m_UpdateTime * 1000.0) << " over " << int(elapsed * 1000.0) << " ms";
			} else {
				CSPLOG(Prio_ERROR, Cat_SCENE) << "Unable to remove phase shader callback";
			}
		}
	}

private:
	Ref<PhaseShader> m_Shader;
	osg::ref_ptr<osg::Texture2D> m_Texture;
	osg::Vec3 m_DirectionToSun;
	double m_Ambient;
	int m_FrameCount;
	unsigned m_Index;
	double m_StartTime;
	double m_UpdateTime;
};


PhaseShader::~PhaseShader() {
}

void PhaseShader::setImage(osg::Image *image) {
	assert(image->getPixelFormat() == GL_RGBA);
	m_Unshaded = image;
	m_Shaded = new osg::Image(*image, osg::CopyOp::DEEP_COPY_IMAGES);
}

void PhaseShader::reshade(osg::Vec3 const &dir_to_sun, double ambient) {
	const unsigned pixels = m_Unshaded->s() * m_Unshaded->t();
	unsigned index = 0;
	bool done = incrementalReshade(dir_to_sun, ambient, pixels, index);
	assert(done);
}

bool PhaseShader::incrementalReshade(osg::Vec3 const &dir_to_sun, double ambient, int pixels, unsigned &index) {
	if (!m_Shaded) return true;
	const unsigned width = m_Unshaded->s();
	const unsigned height = m_Unshaded->t();

	assert(index < height);
	unsigned char const *src = m_Unshaded->data() + 4 * width * index + 3;
	unsigned char *dst = m_Shaded->data() + 4 * width * index + 3;

	// sharpness of the terminus (alpha per radian)
	const double terminus = 8.0;
	const double sx = 1.9999 / width;
	const double sz = 1.9999 / height;

	const unsigned end = std::min(height, index + pixels / width + 1);

	// local (x,y,z) coordinates with +y pointing toward the body, +x to the right, and +z up.
	for (unsigned j = index; j < end; ++j, ++index) {
		double z = j * sz - 1.0;
		double edge = 1.0 - z*z;
		for (unsigned i = 0; i < width; ++i) {
			double x = i * sx - 1.0;
			double x2 = x * x;
			unsigned char c = 0;
			if (x2 <= edge) {
				double y = -sqrt(edge - x2);  // negative y is the side facing the viewer
				double p = x * dir_to_sun.x() + y * dir_to_sun.y() + z * dir_to_sun.z();
				double a = asin(clampTo(p, -1.0, 1.0));
				double s = clampTo(0.5 + a * terminus, ambient, 1.0);
				c = static_cast<unsigned char>(std::max(1.0, *src * s));
			}
			*dst = c;
			dst += 4;
			src += 4;
		}
	}

	return index == height;
}

osg::NodeCallback *PhaseShader::makeCallback(osg::Texture2D *target, osg::Vec3 const &dir_to_sun, double ambient) {
	return new Callback(this, target, dir_to_sun, ambient);
}


} // namespace csp

