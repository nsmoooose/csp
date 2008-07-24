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

#include <csp/cspsim/Shader.h>
#include <csp/cspsim/StateStack.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/util/Log.h>

#include <osg/CullFace>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Node>
#include <osg/Program>
#include <osg/Shader>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TextureCubeMap>

//#include <iostream> // XXX

// I'm relatively new to shaders, particularly their organization within
// a large simulation/game (as opposed to small effects demos), so this
// design may have serious weaknesses.  The crux of the problem is that
// most objects need fogging, and high quality fogging is implemented as
// a shader.  3D models in the simulation use a variety of GL state
// settings (face culling, texture units, etc.), so it isn't easy to
// write a single, unified shader that can be applied to all models.
// Instead we parse each model and bind specific shaders to each node
// based on the GL state settings.  This is currently done at runtime,
// but there is no reason it couldn't be done offline (e.g., as part of
// converting a model to .ive format for faster loading).  --MR


namespace csp {

void Shader::setShaderPath(const std::string& path) {
	m_ShaderPath = path;
}

void Shader::addDefaultUniforms(osg::StateSet *ss) {
	ss->addUniform(m_Tex0.get());
	ss->addUniform(m_Tex1.get());
	ss->addUniform(m_Tex2.get());
	ss->addUniform(m_CullfaceOff.get());
}

bool Shader::applyShader(std::string const &effect, osg::StateSet *ss) {
	if (!ss) return false;
	if (effect == "none") {
		ss->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF);
		return true;
	}
	osg::Program *program = getEffect(effect);
	if (!program) {
		CSPLOG(ERROR, SCENE) << "Could not find shader effect '" << effect << "'";
		return false;
	}
	ss->setAttributeAndModes(program, osg::StateAttribute::ON);
	return true;
}

bool Shader::applyShader(std::string const &effect, osg::Node *node) {
	if (!node) return false;
	return applyShader(effect, node->getOrCreateStateSet());
}

osg::Program *Shader::getEffect(std::string const &effect) {
	osg::Program *program = 0;
	EffectMap::iterator iter = m_Effects.find(effect);
	if (iter == m_Effects.end()) {
		program = loadProgram(effect);
		m_Effects[effect] = program;
	} else {
		program = iter->second.get();
	}
	return program;
}

Shader *Shader::instance() {
	static Shader *shader = 0;
	if (!shader) shader = new Shader;
	return shader;
}

osg::Program *Shader::loadProgram(std::string const &basename) {
	osg::Shader *v_shader = new osg::Shader(osg::Shader::VERTEX);
	osg::Shader *f_shader = new osg::Shader(osg::Shader::FRAGMENT);
	bool ok = false;
	ok = v_shader->loadShaderSourceFromFile(ospath::join(m_ShaderPath, basename + ".vertex")) || ok;
	ok = f_shader->loadShaderSourceFromFile(ospath::join(m_ShaderPath, basename + ".fragment")) || ok;
	if (!ok) return 0;
	osg::Program *program = new osg::Program;
	program->addShader(v_shader);
	program->addShader(f_shader);
	return program;
}

Shader::Shader() {
	m_Tex0 = new osg::Uniform("tex0", 0);
	m_Tex1 = new osg::Uniform("tex1", 1);
	m_Tex2 = new osg::Uniform("tex2", 2);
	m_CullfaceOn = new osg::Uniform("cullface", true);
	m_CullfaceOff = new osg::Uniform("cullface", false);
}

// TODO: rather than hardcoding a set of shaders below...
//
// need more information about shaders (use an xml wrapper):
//   - texture units and texture types
//   - needs material?
//   - supports culling?
//   - configurable options
// state tracking:
//   - shouldn't this be done offline, substituting fx tags
//     as needed?  probably in the long term (far more efficient),
//     but stick with the runtime solution initially.

class Shader::Visitor::Impl {
public:
	void pushAndPop(osg::StateSet *ss) {
		if (ss) {
			push(ss);
			pop();
		}
	}

	void push(osg::StateSet *ss) {
		m_StateStack.push(ss);
		if (ss) {
			osg::Program *program = static_cast<osg::Program*>(ss->getAttribute(osg::StateAttribute::PROGRAM));
			if (program) {
				if (program->isFixedFunction() && program->getName() != "") {
					Shader::instance()->applyShader(program->getName(), ss);
				}
			} else {
				StateStack::AttributePair ap;
				ap = m_StateStack.getTextureAttribute(0, osg::StateAttribute::TEXTURE);
				bool texture = ap.first != 0;
				bool texture_2d = false;
				bool texture_cube = false;
				if (texture) {
					assert(ap.first != 0);
					// ugly, is there a cleaner introspection interface?
					if (dynamic_cast<osg::Texture2D const *>(ap.first) != 0) {
						texture_2d = true;
					} else if (dynamic_cast<osg::TextureCubeMap const *>(ap.first) != 0) {
						texture_cube = true;
					}
				}
				ap = m_StateStack.getAttribute(osg::StateAttribute::MATERIAL);
				bool material = ap.first != 0;
				//std::cout << "::  " << material << " " << texture << " " << texture_2d << " " << texture_cube << "\n";
				if (material) {
					if (!texture) {
						Shader::instance()->applyShader("plain", ss);
					} else if (texture_2d) {
						Shader::instance()->applyShader("object", ss);
					} else if (texture_cube) {
						Shader::instance()->applyShader("reflect", ss);
					} else {
						Shader::instance()->applyShader("red", ss);
					}
				}
			}
			setCulling(ss);
		}
	}

	void pop() {
		m_StateStack.pop();
	}

private:
	void setCulling(osg::StateSet *ss) {
		osg::StateSet::RefAttributePair const *attr = ss->getAttributePair(osg::StateAttribute::CULLFACE);
		const osg::CullFace *cullface = attr ? static_cast<const osg::CullFace*>(attr->first.get()) : 0;
		if (cullface) {
			bool on = ((attr->second & osg::StateAttribute::ON) != 0) && (cullface->getMode() == osg::CullFace::BACK);
			ss->addUniform(Shader::instance()->cullface(on), attr->second);
		}
	}

	StateStack m_StateStack;
};


Shader::Visitor::Visitor(): NodeVisitor(TRAVERSE_ALL_CHILDREN), m_Impl(new Impl) { }
Shader::Visitor::~Visitor() { delete m_Impl; }

void Shader::Visitor::apply(osg::Node &node) {
	//std::cout << "::> " << node.getName() << "\n";
	m_Impl->push(node.getStateSet());
	traverse(node);
	m_Impl->pop();
}

void Shader::Visitor::apply(osg::Geode &geode) {
	m_Impl->push(geode.getStateSet());
	for (unsigned i = 0; i < geode.getNumDrawables(); ++i) {
		m_Impl->pushAndPop(geode.getDrawable(i)->getStateSet());
	}
	m_Impl->pop();
}

} // namespace csp


