
/*
 * ChunkLodDrawable.cpp
 *
 * Author(s): Vladimir Vukicevic <vladimir@pobox.com>
 * 
 * The code follows closely after Thatcher Ulrich's sample ChunkLOD implementation.
 * See http://tulrich.com/geekstuff/chunklod.html for more information.
 * Original algorithm by Thatcher Ulrich, released into the public domain.
 *
 * The osgChunkLod library is open source and may be redistributed and/or modified
 * under the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more _details.
 * 
 * Modifications by Mark Rose <mkrose@users.sourceforge.net>, May 2003,
 * see include/osgChunkLod/ChunkLod for details.
 *
 */

#include <csp/modules/chunklod/ChunkLodDrawable>
#include <csp/modules/chunklod/MultiTextureDetails>
#include <osg/Timer>
#include <osg/CullFace>
#include <osg/TexGen>
#include <osg/StateSet>
#include <cstdio>

//#include "RegisterCombinerDetails.cpp"

namespace osgChunkLod {

ChunkLodDrawable::ChunkLodDrawable()
{
	setSupportsDisplayList(false);
	_tree = NULL;
	osg::Timer t;
	_last_tick = t.tick();
	_triangle_count = 0;
	_lastBoundIndex = -1;
	osg::StateSet *ss = getOrCreateStateSet();
	osg::CullFace *cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	ss->setAttributeAndModes(cull);
	osg::TexGen *texgen = new osg::TexGen();
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);
	ss->setTextureAttributeAndModes(0, texgen);
//	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	//_details = new RegisterCombinerDetails;
	_details = new MultiTextureARBDetails;
	
	_details->addDetail("detail_texture.png", 3125);
	_details->addDetail("detail5.png", 625);
	_details->addDetail("detail4.png", 125);
	
	_details->init(ss);

	// TODO delete _details in dtor
}

ChunkLodDrawable::ChunkLodDrawable(const ChunkLodDrawable& t, const osg::CopyOp &copyop): osg::Drawable(t, copyop) {
	_tree = t._tree;
}

ChunkLodDrawable::~ChunkLodDrawable() {
}

ChunkLodDrawable& ChunkLodDrawable::operator=(const ChunkLodDrawable& t) {
	_tree = t._tree;
	return (*this);
}

osg::Object* ChunkLodDrawable::cloneType() const {
	return new ChunkLodDrawable();
}

osg::Object* ChunkLodDrawable::clone(const osg::CopyOp& copyop) const {
	return new ChunkLodDrawable(*this, copyop);
}

bool ChunkLodDrawable::isSameKindAs(const osg::Object* obj) const {
	return dynamic_cast<const ChunkLodDrawable*>(obj) != NULL;
}

const char* ChunkLodDrawable::className() const {
	return "ChunkLodDrawable";
}

const char* ChunkLodDrawable::libraryName() const {
	return "osgChunkLod";
}

void ChunkLodDrawable::setChunkLodTree(ChunkLodTree* tree) {
	_tree = tree;
	if (tree != NULL) {
		osg::StateSet *ss = getOrCreateStateSet();
		tree->apply(ss);
	}
}

void ChunkLodDrawable::drawImplementation(osg::RenderInfo& info) const {
	static int frame = 0;

	_triangles_rendered = 0;
	_triangles_per_second = 0;

	if (_tree != NULL) {
		if (_lastBoundIndex != _tree->getBoundIndex()) {
			computeBound();
			_lastBoundIndex = _tree->getBoundIndex();
		}
		ChunkLodTree *tree = const_cast<ChunkLodTree*>(_tree);

		osg::Matrix mv = info.getState()->getModelViewMatrix();
		mv.invert(mv);
		osg::Vec3 viewpoint = mv.getTrans();

		info.getState()->disableTexCoordPointersAboveAndIncluding(0); // NEW
		info.getState()->disableAllVertexArrays();

		_details->enable(*info.getState());

		osg::Timer t;
		osg::Timer_t update_t = t.tick();

		tree->update(viewpoint, *info.getState());
		//osg::Timer_t render_t = t.tick();
		_triangles_rendered = tree->render(*info.getState(), *_details);
		_triangle_count += _triangles_rendered;


		float dt = t.delta_s(update_t, t.tick());
		if (dt > 0.0030) {
			//float utime = t.delta_s(update_t, render_t) * 1000.0;
			dt *= 1000.0;
			//std::cout << "PAUSE c-lod render " << dt << " ms (" << utime << ", " << (dt - utime) << ")\n";
		}

		// stats
		float d = t.delta_s(_last_tick, t.tick());
		if (d > 1.0f) {
			//std::cout << "update tree for view from " << viewpoint << "\n";
			//std::cout << _triangles_rendered << " triangles rendered\n";
			_triangles_per_second = static_cast<unsigned long>(_triangle_count / d);
			_last_tick = t.tick();
			_triangle_count = 0;
		} 


		_details->disable(*info.getState());

		info.getState()->disableAllVertexArrays();
		info.getState()->dirtyTexCoordPointersAboveAndIncluding(0);
		info.getState()->setActiveTextureUnit(0);

		frame++;
	}
}

osg::BoundingBox ChunkLodDrawable::computeBound() const {
	osg::BoundingBox bbox;
	if (_tree != NULL) {
		osg::Vec3 center, extent;
		ChunkLodTree *t = const_cast<ChunkLodTree*>(_tree);
		t->getBoundingBox(&center, &extent);
		bbox._min = center - extent;
		bbox._max = center + extent;
	}
	return bbox;
}

}

