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

#ifndef __CSPSIM_SHADER_H__
#define __CSPSIM_SHADER_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/HashUtility.h>

#include <osg/NodeVisitor>
#include <osg/ref_ptr>

#include <string>

namespace osg { class Geode; }
namespace osg { class Node; }
namespace osg { class StateSet; }
namespace osg { class Program; }
namespace osg { class Uniform; }

CSP_NAMESPACE

/** A very basic interface for loading and applying shaders.  Shader
 *  is a singleton, and shader programs are loaded on demand from
 *  the 'shaders' subdirectory of the default data directory (see
 *  Config.h).
 */
class Shader {
public:
	/** Visits an OSG subgraph, applying shaders to specific StateSets.
	 *  Tracks OSG state to decide which shader is most appropriate (since
	 *  a given shader only works for a specific combination of texture
	 *  units and other state variables).  If no compatible shaders are
	 *  found, the node will be rendered with a default shader that renders
	 *  it translucent red for more easy identification in the simulation.
	 *  Nodes can be protected from this binding mechanism by declaring
	 *  empty "Program {}" tags in their stateset definition.
	 */
	class Visitor: public osg::NodeVisitor {
	public:
		Visitor();
		virtual ~Visitor();
		virtual void apply(osg::Node &node);
		virtual void apply(osg::Geode &geode);
	private:
		class Impl;
		Impl* m_Impl;
	};

	/** Apply a shader to a StateSet.  The shader is loaded from files
	 *  in the {datadir}/shaders subdirectory.  The shader files must
	 *  be named {effect}.vertex and {effect}.fragment.
	 */
	bool applyShader(std::string const &effect, osg::StateSet *ss);

	/** Apply a shader to a Node, creating a StateSet for the node if
	 *  necessary.  See applyShader(std::string const&, osg::StateSet*).
	 */
	bool applyShader(std::string const &effect, osg::Node *node);

	/** Add default uniforms to the global stateset.  These uniforms are
	 *  used by most shaders and the default values may be overridden
	 *  within the scene graph (Shader::Visitor does this).
	 */
	void addDefaultUniforms(osg::StateSet *ss);

	/** Singleton accessor.
	 */
	static Shader *instance();

	osg::Uniform *cullface(bool on) {
		return on ? m_CullfaceOn.get() : m_CullfaceOff.get();
	}

private:
	// Singleton.
	Shader();

	/** Get the shader program for the specified effect.  Returns null if
	 *  the effect could not be found or compiled.  Programs are loaded
	 *  lazily and cached.
	 */
	osg::Program *getEffect(std::string const &effect);

	/** Load the shaders {basename}.vertex and {basename}.fragment from
	 *  the {datadir}/shaders subdirectory.  The two shaders are added
	 *  to a new program and returned.  The caller assumes ownership of
	 *  the program (i.e. it should be assigned to a ref_ptr).
	 */
	osg::Program *loadProgram(std::string const &basename);

	/** Effects cache.
	 */
	typedef HashMap<std::string, osg::ref_ptr<osg::Program> >::Type EffectMap;
	EffectMap m_Effects;

	/** Standard uniforms available to all effects.
	 */
	osg::ref_ptr<osg::Uniform> m_Tex0;  // tex0
	osg::ref_ptr<osg::Uniform> m_Tex1;  // tex1
	osg::ref_ptr<osg::Uniform> m_Tex2;  // tex2
	osg::ref_ptr<osg::Uniform> m_CullfaceOn;  // cullface
	osg::ref_ptr<osg::Uniform> m_CullfaceOff;  // cullface
};


CSP_NAMESPACE_END

#endif // __CSPSIM_SHADER_H__

