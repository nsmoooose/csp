/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*- */

/*
 * ChunkLod.cpp

 * Author(s): Vladimir Vukicevic <vladimir@poox.com>
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
 * OpenSceneGraph Public License for more details.
 * 
 * Modifications by Mark Rose <mkrose@users.sourceforge.net>, May 2003,
 * see include/osgChunkLod/ChunkLod for details.
 *
 */


// TODO
//   cleanup and improve statistic accumulation and reporting
//   fix bounding box rendering to work with vertex programs
//   clean up various multitexturing hacks
//   fix multitexturing texture alignment at chunk boundaries


int allocated = 0;
int s_texture_count = 0;
int s_textures_bound = 0;
int s_child_count = 0;



#define _USE_MATH_DEFINES
#include <math.h>

#include <osg/Timer>
#include <osg/Notify>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/State>
#include <osg/AlphaFunc>
#include <osg/GLExtensions>

#include <osgChunkLod/ChunkLod>
#include <osgChunkLod/ChunkLodLoader>
#include <osgChunkLod/MmapFile>
#include <osgChunkLod/MultiTextureDetails>
#include <osgChunkLod/RayStrip>


#ifdef USE_CG
#include <osgNV/VectorParameterValue>
#include <osgNVCg/Parameter>
#endif


#ifdef USE_NV
#include <osgNV/StateMatrixParameterValue>
#endif



#include <osg/buffered_value>
#include <osg/ref_ptr>
#include <osg/Image>

#ifndef GL_STATIC_ATI
#define GL_STATIC_ATI				0x8760
#define GL_DYNAMIC_ATI				0x8761
#define GL_PRESERVE_ATI				0x8762
#define GL_DISCARD_ATI				0x8763
#define GL_OBJECT_BUFFER_SIZE_ATI	0x8764
#define GL_OBJECT_BUFFER_USAGE_ATI	0x8765
#define GL_ARRAY_OBJECT_BUFFER_ATI	0x8766
#define GL_ARRAY_OBJECT_OFFSET_ATI	0x8767
#endif



namespace osgChunkLod {


ChunkLodTree::ChunkLodTree (const char *chunksrc,
                            const TextureQuadTree* texture_quadtree,
                            const osg::Image* heightmap_image,
                            const int height_scale)
{
	_latticeWidth = 1000.0;
	_latticeHeight = 1000.0;
	_lattice_x = 0;
	_lattice_z = 0;
	_origin_x = 0.0;
	_origin_z = 0.0;
	_offset_x = 0.0;
	_offset_z = 0.0;
	_boundIndex = 0;

	chu_file = new MmapFile (chunksrc);

	textureQuadtree = texture_quadtree;
	_heightimage = heightmap_image;
	_heightscale = height_scale;

	// verify that the given file is a valid CHU file
	unsigned int tag = chu_file->readUI32 ();
	if (tag != (('C') | ('H' << 8) | ('U' << 16))) {
		throw "not a valid .chu file";
	}

	unsigned int format_version = chu_file->readUI16 ();
	if (format_version != 9 && format_version != 10) {
		throw "wrong format version .chu file";
	}

	treeDepth = chu_file->readUI16 ();
	errorLODmax = chu_file->readFloat ();
	verticalScale = chu_file->readFloat ();
	baseChunkDimension = chu_file->readFloat ();
	chunkCount = chu_file->readUI32 ();

	chunkTable = new ChunkLod *[chunkCount];
	allocated += chunkCount * sizeof(ChunkLod*);
	memset (chunkTable, 0, sizeof (chunkTable[0]) * chunkCount);

	chunksAllocated = 0;
	chunks = new ChunkLod[chunkCount];
	allocated += chunkCount * sizeof(ChunkLod);

	_fov = float(50.0 * M_PI / 180.0);
	_screenWidth = 1024;
	_maxTexelSize = 1.0;
	_maxPixelError = 10.0;
	_updateParameters();

	printf ("errorLODmax: %f verticalScale: %f baseDim: %f chunkCount: %d\n", errorLODmax, verticalScale, baseChunkDimension, chunkCount);

	unsigned long maxvertices = 0;

	chunksAllocated++;
	chunks[0].lod = 0;
	chunks[0].parent = 0;
	chunks[0].read (chu_file, treeDepth - 1, this, maxvertices, format_version);
	chunks[0].lookupNeighbors (this);

	printf ("Max Vertices: %ld\n", maxvertices);
	maxvertices = maxvertices | 0xf; // pad to 8.
	vertexBuffer = new float [maxvertices * 4];	// allocate a vertex buffer
	allocated += sizeof(float)*maxvertices*4;
	vertexBufferSize = maxvertices * 4;

	loader = new ChunkLodLoader (this, chu_file, textureQuadtree);

#ifdef USE_CG
	_cgContext = new osgNVCg::Context;
	
	// XXX
	osg::ref_ptr<osg::State> s = new osg::State;
	if (_cgContext->getHandle(*s) != 0) {
		_canUseVertexProgram = true;
		_useVertexProgram = true;

		_cgProgram = new osgNVCg::Program (_cgContext,osgNVCg::Program::ARBVP1);
		_cgProgram->setFileName ("ChunkVertexMorph.cg");

		// set up some base stuffs
		_cgScaleValParam = _cgProgram->addVectorParameter("scaleVal");
		_cgOffsetValParam = _cgProgram->addVectorParameter("offsetVal");
	} else {
		// no ARB_vertex_program support
		_canUseVertexProgram = false;
		_useVertexProgram = false;
		_cgContext = NULL;
	}
#else
#ifdef USE_NV
	_canUseVertexProgram = true;
	_useVertexProgram = true;

	_vp = new osgNVExt::VertexProgram;
	_vp->readCodeFromFile("nvMorph.nvv");
	_vp->addParameter(new osgNVExt::VertexProgramParameter(0, new osgNV::StateMatrixParameterValue(osgNV::StateMatrixParameterValue::MODELVIEW_PROJECTION)));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(4, new osgNV::StateMatrixParameterValue(osgNV::StateMatrixParameterValue::MODELVIEW)));
	_vp_texgen0 = new osgNV::VectorParameterValue(osg::Vec4(1.0,1.0,1.0,1.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(8, _vp_texgen0.get()));
	_vp_texgen1 = new osgNV::VectorParameterValue(osg::Vec4(1.0,1.0,1.0,1.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(9,_vp_texgen1.get()));
	_vp_texgen2 = new osgNV::VectorParameterValue(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(10,_vp_texgen2.get()));
	_vp_texgen3 = new osgNV::VectorParameterValue(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(11,_vp_texgen3.get()));
	_vp_scale = new osgNV::VectorParameterValue(osg::Vec4(0.0, 0.0, 0.0, 0.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(12,_vp_scale.get()));
	_vp_offset = new osgNV::VectorParameterValue(osg::Vec4(0.0, 0.0, 0.0, 0.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(13,_vp_offset.get()));
	osgNV::VectorParameterValue *_vp_gfog = new osgNV::VectorParameterValue(osg::Vec4(0.002, 30000.0, 0.0, 0.0));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(14,_vp_gfog));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(15,new osgNV::VectorParameterValue(osg::Vec4(0.0, 0.0, 0.0, 1.0))));
	_vp->addParameter(new osgNVExt::VertexProgramParameter(16,new osgNV::VectorParameterValue(osg::Vec4(0.0, 0.0, 10000.0, 1.0))));
#else
	_canUseVertexProgram = false;
	_useVertexProgram = false;
#endif
#endif
}

void ChunkLodTree::apply(osg::StateSet *ss) {
#ifdef USE_NV
	assert(_vp.valid());
	ss->setAttributeAndModes(_vp.get());
#endif
}

ChunkLodTree::~ChunkLodTree ()
{
	allocated -= chunkCount * sizeof(ChunkLod*);
	allocated -= sizeof(float)*vertexBufferSize;
	allocated -= chunkCount * sizeof(ChunkLod);
#ifdef DUMP_ALLOC
	std::cerr << "FREEING FINAL " << allocated << "\n";
#endif
	delete [] chunkTable;
	delete [] chunks;
	delete [] vertexBuffer;
	delete loader;
	delete chu_file;
}

void
ChunkLodTree::loaderUseThread (bool use)
{
	 loader->useThread (use);
}

void
ChunkLodTree::useVertexProgram (bool use)
{
#if defined(USE_CG) || defined(USE_NV)
	if (_canUseVertexProgram) {
		_useVertexProgram = true;
	}
#endif
}

void
ChunkLodTree::update (const osg::Vec3& viewpoint, osg::State& s)
{
	if (chunks[0].data == NULL) {
		loader->requestLoad (&chunks[0], 1.0f);
	}

	sChunksInUse = 0;
	sChunksWithData = 0;
	sChunksWithTexture = 0;
	sBytesInUse = 0;

#ifdef DUMP_STATS
	int last_texture_count = s_texture_count;
	int last_child_count = s_child_count;
#endif

	s_texture_count = 0;
	s_child_count = 0;

	if (chunks[0].split) {
		chunks[0].clear();
	}
	chunks[0].update (this, viewpoint);
	if (textureQuadtree) {
		chunks[0].updateTexture (this, viewpoint);
	}
	chunks[0].cull (*this, s);

#ifdef DUMP_STATS
	if (last_texture_count != s_texture_count) 
		std::cout << "TEXTURES = " << s_texture_count << "\n";
	if (last_child_count != s_child_count) 
		std::cout << "CHILDREN = " << s_child_count << "\n";
#endif

	loader->syncLoader();
}

int
ChunkLodTree::render (osg::State& s, MultiTextureDetails &details)
{
	int triangle_count = 0;
#ifdef DUMP_STATS
	int last_textures_bound = s_textures_bound;
#endif
	s_textures_bound = 0;

	if (_useVertexProgram) {
#ifdef USE_CG
		cgGLEnableProfile (cgGetProgramProfile (_cgProgram->getHandle(s)));
#endif
	}

	if (chunks[0].data == NULL) {
		// no data in root node; should happen only briefly until it's loaded
	} else {
		triangle_count += chunks[0].render (*this, s, details, textureQuadtree == NULL ? true : false);
	}


	if (_useVertexProgram) {
#ifdef USE_CG
		cgGLDisableProfile (cgGetProgramProfile (_cgProgram->getHandle(s)));
#endif
	}

#ifdef DUMP_STATS
	if (last_textures_bound != s_textures_bound)
		std::cout << "TEXTURLES BOUND = " << s_textures_bound << "\n";
#endif

	return triangle_count;
}

void
ChunkLodTree::setQuality (float max_pixel_error,
                          float max_texel_size)
{
	_maxPixelError = max_pixel_error;
	_maxTexelSize = max_texel_size;
	_updateParameters();
}

void
ChunkLodTree::_updateParameters() {
	const float tan_half_FOV = tanf (0.5f * _fov);
	const float K = _screenWidth / tan_half_FOV;

	distanceLODmax = (errorLODmax / _maxPixelError) * K;
	std::cout << "CHUNKLOD parameters: " << distanceLODmax << " " << errorLODmax << " " << _maxPixelError << " " << K << "\n";
	textureDistanceLODmax = 1.0f;
	if (textureQuadtree) {
		float texel_size_LODmax = baseChunkDimension / (textureQuadtree->tileSize() - 1);
		textureDistanceLODmax = (texel_size_LODmax / _maxTexelSize) * K;
		std::cout << "CHUNKTQT parameters: " << textureDistanceLODmax << " " << texel_size_LODmax << " " << _maxTexelSize << " " << K << "\n";
	}
}


void
ChunkLodTree::setCameraParameters ( int screen_width_pixels,
                                    float horizontal_FOV_degrees)
{
	_screenWidth = screen_width_pixels;
	_fov = horizontal_FOV_degrees * float(M_PI) / 180.0f;
	_updateParameters();
}

void
ChunkLodTree::setCameraPosition(double x, double z)
{
	int ix = static_cast<int>(x / _latticeWidth + 0.5);
	int iz = static_cast<int>(z / _latticeHeight + 0.5);
	if (ix != _lattice_x || iz != _lattice_z) {
		_boundIndex++;
		// use bbox_extent since bbox_center depends on the camera position
		_origin_x = ix * _latticeWidth + _offset_x;
		_origin_z = iz * _latticeHeight + _offset_z;
		_lattice_x = ix;
		_lattice_z = iz;
		std::cout << "CLOD " << x << " ORIGIN TO " << _origin_x << ", " << _origin_z << "\n";
		chunks[0].updateOrigin(*this);
	}
}

void 
ChunkLodTree::getLocalOrigin(double &x, double &y, double &z) const 
{
	// include the offset that brings the origin to the center
	// of the terrain
	x = _origin_x - _offset_x;
	y = 0.0;
	z = _origin_z - _offset_z;
}

void 
ChunkLodTree::_getInternalOrigin(double &x, double &y, double &z) const 
{
	// omit the offset that brings the origin to the center
	// of the terrain
	x = _origin_x;
	y = 0.0;
	z = _origin_z;
}

void 
ChunkLodTree::_setInternalOffset(double x, double y, double z)  
{
	//std::cout << "INTERNAL OFFSET: " << x << ", " << z << "\n";
	_offset_x = x;
	_offset_z = z;
	_origin_x = _lattice_x * _latticeWidth + _offset_x;
        _origin_z = _lattice_z * _latticeHeight + _offset_z;
}

void 
ChunkLodTree::setLatticeDimensions(double width, double height) {
	_latticeWidth = width;
	_latticeHeight = height;
	// XXX update?
}

bool 
ChunkLodTree::intersect(ChunkLodIntersect &test) const {
	test.reset();
	chunks[0].intersect(test, verticalScale);
	return test.getHit();
}

bool 
ChunkLodTree::findElevation(ChunkLodElevationTest &test) const {
	test.reset();
	chunks[0].findElevation(test, verticalScale);
	return test.getHit();
}

// for multiple correlated tests this method is much less efficient 
// than calling findElevation() directly and reusing the 
// ChunkLodElevationTest instance.  here we discard all knowledge about 
// which triangle was last intersected, so each call requires an 
// exhaustive search of the triangle strip.
float
ChunkLodTree::simpleHeightAt (const float x, const float z) const
{
	ChunkLodElevationTest test;
	test.setPosition(x, z);
	findElevation(test);
	return test.getElevation();
}

void
ChunkLodTree::getBoundingBox (osg::Vec3* box_center, osg::Vec3* box_extent) const
{
	chunks[0].computeBoundingBox (box_center, box_extent);
}

unsigned short
ChunkLodTree::_computeLod (const osg::Vec3& center,
                           const osg::Vec3& extent,
                           const osg::Vec3& viewpoint) const
{
	osg::Vec3 disp = viewpoint - center;
	disp[0] = fmax (0.0f, fabsf (disp[0]) - extent[0]);
	disp[1] = fmax (0.0f, fabsf (disp[1]) - extent[1]);
	disp[2] = fmax (0.0f, fabsf (disp[2]) - extent[2]);

	float d = disp.length();
	return iclamp (((treeDepth << 8) - 1) - int (log2f (fmax (1, d / distanceLODmax)) * 256), 0, 0x0FFFF);
}

int
ChunkLodTree::_computeTextureLod (const osg::Vec3& center,
                                     const osg::Vec3& extent,
                                     const osg::Vec3& viewpoint) const
{
	osg::Vec3 disp = viewpoint - center;
	disp[0] = fmax (0.0f, fabsf (disp[0]) - extent[0]);
	disp[1] = fmax (0.0f, fabsf (disp[1]) - extent[1]);
	disp[2] = fmax (0.0f, fabsf (disp[2]) - extent[2]);

	float d = disp.length();
	return (treeDepth - 1 - int (log2f (fmax (1, d / textureDistanceLODmax))));
}

void 
ChunkLodTree::_addChunk(int label, ChunkLod *chunklod) 
{
	assert (chunkTable[label] == 0);
	chunkTable[label] = chunklod;
}

ChunkLod* 
ChunkLodTree::_newChunk() 
{
	return &(chunks[chunksAllocated++]);
}

//////////////////////////////////
////// ChunkLod methods
//////////////////////////////////

static void
countChunkStats (ChunkLod* c)
{
	return;
}

void
ChunkLod::clear ()
{
	// performance stuff
	// ChunkLodTree::sChunksInUse++;
	// ChunkLodTree::sChunksWithData++;
	// ChunkLodTree::sBytesInUse += data->getDataSize();
	// if (textureId) ChunkLodTree::sChunksWithTexture++;

	if (texture.valid()) s_texture_count++;
	if (!hasChildren() || !split) s_child_count++;
	if (split) {
		split = false;
		if (hasChildren ()) {
			for (int i = 0; i < 4; i++) {
				children[i]->clear();
			}
		}
	} else {
		if (hasChildren ()) {
			for (int i = 0; i < 4; i++) {
				countChunkStats (children[i]);
        		}
        	}
	}
}

void
ChunkLod::update (ChunkLodTree* tree, const osg::Vec3& viewpoint)
{
	unsigned short desired_lod = tree->_computeLod (lores_center, lores_extent, viewpoint);

	if (hasChildren () && desired_lod > (lod | 0xFF) && canSplit (tree)) {
		// the LOD level is higher than what we can do; we need to split
		doSplit (tree, viewpoint);

		for (int i = 0; i < 4; i++) {
			children[i]->update (tree, viewpoint);
		}
	} else {
		// this chunk can satisfy the LOD requirements
		if ((lod & 0xFF00) == 0) {
			// root chunk, check if we have a valid morph value
			lod = iclamp (desired_lod, lod & 0xFF00, lod | 0x0FF);
			assert ((lod >> 8) == level);
		}

		// get ready to preload our children,
		// and unload our grandchildren and descendants
		if (hasChildren()) {
			float priority = 0.0f;
			if (desired_lod > (lod & 0xFF00)) {
				priority = (lod & 0x0FF) / 255.0f;
			}

			if (priority < 0.5f) {
				for (int i = 0; i < 4; i++) {
					children[i]->requestUnloadSubtree (tree);
				}
			} else {
				for (int i = 0; i < 4; i++) {
					children[i]->warmUpData (tree, priority);
				}
			}
		}
	}
}

void
ChunkLod::updateTexture (ChunkLodTree* tree, const osg::Vec3& viewpoint)
{
	if (tree->getTextureQuadtree() == NULL) return;

	if (level >= tree->getTextureQuadtree()->depth()) {
		// There's no texture for this detail level, so it will
		// get bound up above us
		texture = NULL;
		return;
	}

	int desiredTexLevel = tree->_computeTextureLod (lores_center, lores_extent, viewpoint);

	if (texture.valid()) {
		assert (parent == NULL || parent->texture.valid());

		// decide if we should release our texture
		if (data == NULL || desiredTexLevel < level) {
			// release this texture and any of our descendants'
			// textures.
			requestUnloadTextures (tree);
		} else {
			if (hasChildren()) {
				for (int i = 0; i < 4; i++) {
					children[i]->updateTexture (tree, viewpoint);
				}
			}
		}
	} else {
		// decide if we should load our texture
		if (desiredTexLevel >= level && data) {
			tree->getLoader()->requestLoadTexture (this);
		} else {
			// nothing to do
			// could ensure that our children don't have textures loaded for debugging
		}
	}
}

void
ChunkLod::doSplit (ChunkLodTree* tree, const osg::Vec3& viewpoint)
{
	if (split == false) {
		assert (this->canSplit (tree));
		assert (hasResidentData());

		split = true;

		if (hasChildren ()) {
			for (int i = 0; i < 4; i++) {
				ChunkLod* c = children[i];
				short desired_lod = tree->_computeLod (lores_center, lores_extent, viewpoint);
				c->lod = iclamp (desired_lod, c->lod & 0xFF00, c->lod | 0x0FF);
			}
		}

		for (ChunkLod* p = parent; p && p->split == false; p = p->parent) {
			p->doSplit (tree, viewpoint);
		}
	}
}

bool
ChunkLod::canSplit (ChunkLodTree* tree)
{
	if (split) {
		return true;
	}

	if (hasChildren() == false) {
		return false;
	}

	// now check whether all our children have data
	// and do the requests if we don't

	bool can_split = true;
	for (int i = 0; i < 4; i++) {
		ChunkLod* c = children[i];
		if (c->hasResidentData() == false) {
			tree->getLoader()->requestLoad (c, 1.0f);
			can_split = false;
		}
	}

	// check ancestors
	for (ChunkLod* p = parent; p && p->split == false; p = p->parent) {
		if (p->canSplit(tree) == false) {
			can_split = false;
		}
	}

	// and check neighbors, to make sure their LOD is close enough
	for (int i = 0; i < 4; i++) {
		ChunkLod* n = neighbor[i].chunk;

		// TODO -- pull this outside of here
		const int MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE = 2;

		for (int count = 0;
		     n && count < MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE;
		     count++)
		{
			n = n->parent;
		}

		if (n && n->canSplit (tree) == false) {
			can_split = false;
		}
	}

	return can_split;
}

void
ChunkLod::unloadData()
{
	delete data;
	data = NULL;
}

void
ChunkLod::warmUpData (ChunkLodTree* tree, float priority)
{
	// request that our data be loaded, and unload our children's data
	if (data == NULL) {
		tree->getLoader()->requestLoad (this, priority);
	} 

	// if our priority is fairly high, only unload our grandchildren
	// otherwise unload our children
	if (hasChildren()) {
		if (priority < 0.5f) {
			for (int i = 0; i < 4; i++) {
				children[i]->requestUnloadSubtree (tree);
			}
		} else {
			// TODO -- we could call c->warmUpData here with a low priority and get (almost)
			// the same effect.  would be interesting to analyze pre-load misses
			for (int i = 0; i < 4; i++) {
				ChunkLod* c = children[i];
				if (c->hasChildren()) {
					for (int j = 0; j < 4; j++) {
						c->children[j]->requestUnloadSubtree (tree);
					}
				}
			}
		}
	}
}

void
ChunkLod::requestUnloadSubtree (ChunkLodTree* tree)
{
	if (data) {
		if (hasChildren ()) {
			for (int i = 0; i < 4; i++) {
				children[i]->requestUnloadSubtree (tree);
			}
		}
		tree->getLoader()->requestUnload (this);
	}
}

void
ChunkLod::requestUnloadTextures (ChunkLodTree* tree)
{
	if (texture.valid()) {
		if (hasChildren ()) {
			for (int i = 0; i < 4; i++) {
				children[i]->requestUnloadTextures (tree);
			}
		}
		tree->getLoader()->requestUnloadTexture (this);
	}
}

void
draw_box(osg::Vec3 const &min, osg::Vec3 const &max) {
	glBegin(GL_LINES);
	glVertex3f(min.x(), min.y(), min.z());
	glVertex3f(min.x(), max.y(), min.z());
	glVertex3f(min.x(), min.y(), max.z());
	glVertex3f(min.x(), max.y(), max.z());
	glVertex3f(max.x(), min.y(), min.z());
	glVertex3f(max.x(), max.y(), min.z());
	glVertex3f(max.x(), min.y(), max.z());
	glVertex3f(max.x(), max.y(), max.z());
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(min.x(), min.y(), min.z());
	glVertex3f(min.x(), min.y(), max.z());
	glVertex3f(max.x(), min.y(), max.z());
	glVertex3f(max.x(), min.y(), min.z());
	glVertex3f(min.x(), min.y(), min.z());
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(min.x(), max.y(), min.z());
	glVertex3f(min.x(), max.y(), max.z());
	glVertex3f(max.x(), max.y(), max.z());
	glVertex3f(max.x(), max.y(), min.z());
	glVertex3f(min.x(), max.y(), min.z());
	glEnd();
}

void
ChunkLod::cull (const ChunkLodTree& c,
                   osg::State& s)
{
	osg::BoundingBox bbox(lores_center - lores_extent, lores_center + lores_extent);

	if (s.getViewFrustum().contains (bbox)) {
		culled = false;
		if (split) {
			for (int i = 0; i < 4; i++) {
				children[i]->cull (c, s);
			}
		}
	} else {
		culled = true;
	}
}

int
ChunkLod::render (const ChunkLodTree& c,
                  osg::State& s,
		  MultiTextureDetails &details,
                  bool texture_bound)
{
	if (culled) {
		return 0;
	}

	int triangle_count = 0;
	if (!texture_bound) { 
		if (texture.valid()) {
			// this is safe, since split will always be false
			// if we have no children
			if (split == false ||
			    !children[0]->texture.valid() ||
			    !children[1]->texture.valid() ||
			    !children[2]->texture.valid() ||
			    !children[3]->texture.valid())
			{
				//std::cout << "+BIND\n";
				bindTexture (s, details); 
				//std::cout << "-BIND\n";
				texture_bound = true;
			}
		}
	}

	if (split) {
		for (int i = 0; i < 4; i++) {
			triangle_count += children[i]->render (c, s, details, texture_bound);
		}
	} else {
		triangle_count += data->render (c, *this, s, details, lores_center, lores_extent);
// XXX XXX 
//   		Removed temporarily, as this is currently incompatible with
//   		the vertex program that does geomorphing.
//
		/*
		s.applyTextureMode(0, GL_TEXTURE_2D, false);
		details.setMode(s, false);
		float f = (lod & 255) / 255.0f;     //xxx
		glColor3f(f, 1 - f, texture_bound ? 1.0 : 0.0);
		draw_box(lores_center - lores_extent, lores_center + lores_extent); 
		// XXX XXX should push/pop texture states!
		details.setMode(s, true);
		s.applyTextureMode(0, GL_TEXTURE_2D, true);
		*/
	}

	return triangle_count;
}

// XXX huge hack just to get things working, please clean me up!
float test_tex_extent;
float test_tex_cx;
float test_tex_cz;

// set up and bind the texture so it stretches over the entire
// X-Z extent of the bounding box
void
ChunkLod::bindTexture (osg::State& s, MultiTextureDetails &details)
{
	s_textures_bound++;

	float xsz = lores_extent[0] * 2.0 * (513.0f / 512.0f);
	float zsz = lores_extent[2] * 2.0 * (513.0f / 512.0f);
	float x0 = lores_center[0] - lores_extent[0] - (xsz / 512.0f) * 0.5f;
	float z0 = lores_center[2] - lores_extent[2] - (zsz / 512.0f) * 0.5f;

	test_tex_cx = lores_center[0];
	test_tex_cz = lores_center[2];
	test_tex_extent = lores_extent[0];

	// this will bind it and prepare the texture (mipmap generation);
	// we really want to generate mipmaps in the thread, but doing so
	// would require us to do them in software (not a big deal, just need
	// code)

	details.map(s, lores_extent[0] * 2.0, lores_extent[2] * 2.0,
	               lores_center[0] - lores_extent[0],
		       lores_center[2] - lores_extent[2]);


	// standard texturing
	s.applyTextureAttribute (0, texture.get()); 
	s.applyTextureMode (0, GL_TEXTURE_2D, true); 

	float p[4] = { 0, 0, 0, 0 };

	p[0] = 1.0f / xsz;
	p[3] = -x0 / xsz;
	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv (GL_S, GL_OBJECT_PLANE, p);
	
	p[0] = 0;
	p[2] = 1.0f / zsz;
	p[3] = -z0 / zsz;
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv (GL_T, GL_OBJECT_PLANE, p);
}

void
ChunkLod::read (MmapFile* in, 
                int recursion_count, 
                ChunkLodTree* tree, 
                unsigned long& max_vertices, 
                int format_version)
{
	split = false;
	data = NULL;

	//	int pos = in->tell();
	label = in->readSI32 ();

	if (label >= tree->getChunkCount() || label < 0) {
		assert (0);
		// oops
	}

	tree->_addChunk(label, this);

	for (int i = 0; i < 4; i++) {
		neighbor[i].label = in->readSI32 ();
	}

	level = in->readUI8 ();
	x = in->readUI16 ();
	z = in->readUI16 ();
	miny = in->readSI16 ();
	maxy = in->readSI16 ();

	unsigned long verts = 0;
	if (format_version >= 10) {
		verts = in->readUI16 ();
	}
	dataFilePosition = in->readUI32 ();

	if (format_version < 10) {
		// this causes a pretty annoying slowdown at 
		// initialization time, and can cause mmap to 
		// explode for large chu files.  the vertex
		// counts really should be stored in the header,
		// so please use version 10 or higher!
		//
		// jump to the dataFilePosition to read the number of 
		// vertices for this chunk
		unsigned long cur = in->tell();
		in->seek (dataFilePosition);
		verts = in->readUI16 ();
		in->seek (cur);
	}

	if (verts > max_vertices) {
		max_vertices = verts;
	}

	// get center and extent
	_internalComputeBoundingBox (*tree);
	if (level == 0) {
		// first time through.  center the top level chunk
		tree->_setInternalOffset(center_x, 0.0, center_z);
		_internalComputeBoundingBox (*tree);
	}

	// recurse to children
	if (recursion_count > 0) {
		for (int i = 0; i < 4; i++) {
			children[i] = tree->_newChunk();
			children[i]->lod = lod + 0x100;
			children[i]->parent = this;
			children[i]->read(in, recursion_count - 1, 
			                  tree, max_vertices, 
			                  format_version);
		}
	} else {
		for (int i = 0; i < 4; i++) {
			children[i] = NULL;
		}
	}

}

void
ChunkLod::lookupNeighbors (ChunkLodTree* tree)
{
	for (int i = 0; i < 4; i++) {
		if (neighbor[i].label < -1 || neighbor[i].label >= tree->getChunkCount()) {
			assert (0);
			neighbor[i].label = -1;
		}

		if (neighbor[i].label == -1) {
			neighbor[i].chunk = NULL;
		} else {
			neighbor[i].chunk = tree->getChunk(neighbor[i].label);
		}
	}

	if (hasChildren()) {
		for (int i = 0; i < 4; i++) {
			children[i]->lookupNeighbors (tree);
		}
	}
}

void
ChunkLod::_internalComputeBoundingBox (const ChunkLodTree& tree)
{
	float level_factor = (1 << (tree.getDepth() - 1 - level));

	double origin_x;
	double origin_y;
	double origin_z;
	tree._getInternalOrigin(origin_x, origin_y, origin_z);

	center_y = (maxy + miny) * 0.5f * tree.getVerticalScale();
	extent_y = (maxy - miny) * 0.5f * tree.getVerticalScale();

	center_x = (x + 0.5) * level_factor * tree.getBaseChunkDimension() - origin_x;
	center_z = (z + 0.5) * level_factor * tree.getBaseChunkDimension() - origin_z;

	const float EXTRA_BOX_SIZE = 1e-3f;
	extent_x = level_factor * tree.getBaseChunkDimension() * 0.5f + EXTRA_BOX_SIZE;
	extent_z = extent_x;

	lores_center.set(static_cast<float>(center_x),
	                 static_cast<float>(center_y),
	                 static_cast<float>(center_z));
	lores_extent.set(static_cast<float>(extent_x),
	                 static_cast<float>(extent_y),
	                 static_cast<float>(extent_z));
}


void 
ChunkLod::updateOrigin(ChunkLodTree const & tree) {
	_internalComputeBoundingBox(tree);
	if (hasChildren()) {
		for (int i = 0; i < 4; i++) {
			children[i]->updateOrigin(tree);
		}
	}
}

osg::BoundingBox
ChunkLod::getBoundingBox() const {
	return osg::BoundingBox(lores_center - lores_extent, lores_center + lores_extent);
}

void 
ChunkLod::intersect (ChunkLodIntersect &test, float vscale) const
{
	osg::BoundingBox bb = getBoundingBox();
	//std::cout << lores_center << " - " << lores_extent << std::endl;
	osg::LineSegment const *segment = test.getLineSegment();
	if (!segment || !segment->intersect(bb)) return;
	//std::cout << "in bbox \n";
	if (hasChildren() && split) {
		//std::cout << "split\n";
		for (int i = 0; i < 4; i++) {
			children[i]->intersect(test, vscale);
			if (test.getHit() && !test.getExhaustive()) return;
		}
		// XXX if children were hit don't test this level?
		if (test.getHit()) return; 
		// XXX if children were not hit, there shouldn't be much
		// point in testing the parent, but it does seem to help!
	} //else {
	if (hasResidentData()) {
		if (data->vertexInfo.vertices != 0) {
			int index = test.getIndex();
			float u, v, d = 1.01;
			float f = (lod & 0x0FF) / 255.0f;
			osg::Vec3 normal;
			osg::Vec3 scale = lores_extent / (1 << 14);
			osg::Vec3 offset = lores_center;
			scale.y() = vscale;
			offset.y() = 0.0;
			if (test.getHit()) d = test.getRatio();
			bool hit = intersectRayStrip(segment,
			                             data->vertexInfo.vertices, 
			                             data->vertexInfo.indices, 
			                             data->vertexInfo.indexCount, 
			                             1.0-f, 
			                             offset, 
			                             scale, 
			                             index, 
						     normal,
						     test.getExhaustive(),
			                             d, u, v);
			if (hit) {
				// fix triangles with inverted normals
				if (normal.y() < 0.0) normal = -normal;
				test.setHit(d, index, normal);
			} else {
				//std::cout << "intersect failed\n";
			}
		} else {
			//std::cout << "NO DATA (strip==0)\n";
		}
	} else {
		//std::cout << "NO DATA\n";
	}
	//}
}
	
void 
ChunkLod::findElevation (ChunkLodElevationTest &test, float vscale) const
{
	osg::BoundingBox bb = getBoundingBox();
	if (test.cull(bb)) return;
	if (hasChildren() && split) {
		for (int i = 0; i < 4; i++) {
			children[i]->findElevation(test, vscale);
			if (test.getHit()) return;
		}
		return;
	} 
	if (hasResidentData()) {
		if (data->vertexInfo.vertices != 0) {
			float f = (lod & 0x0FF) / 255.0f;
			osg::Vec3 scale = lores_extent / (1 << 14);
			osg::Vec3 offset = lores_center;
			scale.y() = vscale;
			offset.y() = 0.0;
			bool hit = intersectVerticalStrip(test,
			                             data->vertexInfo.vertices, 
			                             data->vertexInfo.indices, 
			                             data->vertexInfo.indexCount, 
			                             1.0-f, 
			                             offset, 
			                             scale);
			if (hit) {
				test._setMaxHeight(bb.yMax());
				test._setChunk(label);
			} else {
				std::cout << "ETEST failed!" << scale << " " << offset << "\n";
			}
		}
	}
}

////////////////////////
///// ChunkLodData methods
////////////////////////

int
ChunkLodData::render (const ChunkLodTree& c,
                      const ChunkLod& chunk,
                      osg::State& s,
		      MultiTextureDetails &details,
                      const osg::Vec3& box_center,
                      const osg::Vec3& box_extent)
{
	int triangle_count = 0;
	float *buffer = c.getVertexBuffer();
	unsigned long buffer_size = c.getVertexBufferSize();

	assert (((unsigned long) (3*vertexInfo.vertexCount)) <= buffer_size);

	float f = (chunk.lod & 0x0FF) / 255.0f;

	// first morph the vertices
	const float sx = box_extent.x() / (1 << 14);
	const float sz = box_extent.z() / (1 << 14);

	const float offsetx = box_center.x();
	const float offsetz = box_center.z();

	const float one_minus_f = (1.0f - f);

	if (!c.useVertexProgram()) {
		// perform the morphing in-place
		float *verts = buffer;
		const float verticalScale = c.getVerticalScale();
		const ChunkLodVertex *v = vertexInfo.vertices;
		for (int i = vertexInfo.vertexCount; --i >= 0;) {
			*verts++ = offsetx + v->v[0] * sx;
			*verts++ = (v->v[1] + v->y_delta * one_minus_f) * verticalScale;
			*verts++ = offsetz + v->v[2] * sz;
			v++;
		}
		// then do the drawing using the vertex and index buffers
		glColor3f (1.0f, 1.0f, 1.0f);
		s.setVertexPointer (3, GL_FLOAT, 0, buffer);
	        glDrawElements (GL_TRIANGLE_STRIP, vertexInfo.indexCount, GL_UNSIGNED_SHORT, vertexInfo.indices);
	} else {
#ifdef USE_CG
		c.getCgScaleValueParameter()->set (sx, c.getVerticalScale(), sz);
		c.getCgOffsetValueParameter()->set (offsetx, 1.0f - f, offsetz);
		s.applyAttribute (c.getCgProgram());
#else
#ifdef USE_NV
		c._vp_scale->set(sx, c.getVerticalScale(), sz);
		c._vp_offset->set(offsetx, 1.0f - f, offsetz);

		float tgsx = 1.0 / (box_extent.x() * 2.0 * (513.0f / 512.0f));
		float tgsz = 1.0 / (box_extent.z() * 2.0 * (513.0f / 512.0f));
		float tgx0 = offsetx - box_extent.x(); // XXX add single texel overlap!
		float tgz0 = offsetz - box_extent.z();

		{
			float extent = box_extent.x();
			float scale = extent / test_tex_extent / (1<<15);
			float x0 = 0.5*(offsetx - test_tex_cx) / test_tex_extent + 0.5; 
			float z0 = 0.5*(offsetz - test_tex_cz) / test_tex_extent + 0.5; 
			c._vp_texgen0->set(scale, scale, x0, z0);
		}

		tgx0 = tgz0 = -1.0*(1<<14);
		float detail_scale = box_extent.x() / (1<<14);
		tgsx = tgsz = detail_scale / details.getDetailScale(0);
		c._vp_texgen1->set(tgsx, tgsz, tgx0, tgz0);
		tgsx = tgsz = detail_scale / details.getDetailScale(1);
		c._vp_texgen2->set(tgsx, tgsz, tgx0, tgz0);
		tgsx = tgsz = detail_scale / details.getDetailScale(2);
		c._vp_texgen3->set(tgsx, tgsz, tgx0, tgz0);
		c._vp->CustomVertexProgram::apply(s);
		// this is an ugly hack to force the vertex parameters to be
		// reloaded in between stateset changes
		static osg::ref_ptr<osgNVExt::VertexProgram> dummy = new osgNVExt::VertexProgram;
		s.haveAppliedAttribute(dummy.get());
		bool applied = s.applyAttribute(c._vp.get());
		if (!applied) return 0;
		
#endif
#endif

#if defined(USE_CG) || defined(USE_NV)
		// If we're using a vertex program, we might be using a server-side vertex object
		if (vertexInfo._vertexArray != 0) {
			// yep, we have a vertex object
			const ChunkLodTree::Extensions* ext = ChunkLodTree::getExtensions (0, true);
			::exit(0);
			ext->glArrayObjectATI (GL_VERTEX_ARRAY, 4, GL_FLOAT, 0, vertexInfo._vertexArray, 0);
			glEnableClientState (GL_VERTEX_ARRAY);
			glDrawArrays (GL_TRIANGLE_STRIP, 0, vertexInfo.indexCount);
		} else {
			// nope, no vertex array; fill up the buffer with non-morphed values and copy.
			// this ought to be a memcpy()
			// FIXME make vertexInfo store this data as a *float
			// ^^^^^ actually, the height test works best with shorts, but this is probably
			//       more important....

/*
			float *verts = buffer;
			const ChunkLodVertex *v = vertexInfo.vertices;

			for (int i = vertexInfo.vertexCount; --i >= 0;) {
				*verts++ = v->v[0];
				*verts++ = v->v[1]; 
				*verts++ = v->v[2];
				*verts++ = v->y_delta;
				v++;
			}

			s.setVertexPointer (4, GL_FLOAT, 0, buffer);
*/
			s.setVertexPointer (4, GL_FLOAT, 0, vertexInfo.floatVertices);
			glDrawElements (GL_TRIANGLE_STRIP, vertexInfo.indexCount, GL_UNSIGNED_SHORT, vertexInfo.indices);
		}

		//s.dirtyAllAttributes ();		// needed to make sure we can apply the same context again (To update params)
#endif
	}
	
	triangle_count += vertexInfo.triangleCount;

    return triangle_count;
}

ChunkLodData::ChunkLodData (MmapFile* mf)
{
	vertexInfo.read (mf);
}

ChunkLodVertexInfo::~ChunkLodVertexInfo ()
{
	if (vertices) {
		allocated -= vertexCount * sizeof(ChunkLodVertex);
		delete [] vertices;
		vertices = NULL;
	}
	if (indices) {
		allocated -= indexCount * 2;
		delete [] indices;
		indices = NULL;
	}
	if (floatVertices) {
		allocated -= vertexCount * 4 * sizeof(float);
		delete[] floatVertices;
		floatVertices = NULL;
	}
#ifdef DUMP_ALLOC
    	std::cerr << "FREEING " << allocated << "\n";
#endif
	if (_vertexArray) {
		const ChunkLodTree::Extensions* ext = ChunkLodTree::getExtensions (0, true);
		ext->glFreeObjectBufferATI (_vertexArray);
		_vertexArray = 0;
	}
}

/*
 * this function gets called from the reader thread;
 * as such, it should be free of OpenGL calls
 */

void
ChunkLodVertexInfo::read (MmapFile* mf)
{
	vertexCount = mf->readUI16();
	vertices = new ChunkLodVertex[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		vertices[i].v[0] = mf->readUI16();
		vertices[i].v[1] = mf->readUI16();
		vertices[i].v[2] = mf->readUI16();
		vertices[i].y_delta = mf->readUI16();
	}

	floatVertices = new float[vertexCount * 4];
	for (int j = 0; j < vertexCount; j++) {
		floatVertices[4*j+0] = vertices[j].v[0];
		floatVertices[4*j+1] = vertices[j].v[1];
		floatVertices[4*j+2] = vertices[j].v[2];
		floatVertices[4*j+3] = vertices[j].y_delta;
	}

	indexCount = mf->readUI32();
	if (indexCount > 0) {
		indices = new unsigned short[indexCount];
	} else {
		indices = NULL;
	}
	allocated += (vertexCount * sizeof(ChunkLodVertex) + indexCount * 2);
	#ifdef DUMP_ALLOC
	std::cerr << "ALLOCATED " << allocated << "\n";
	#endif

	for (int i = 0; i < indexCount; i++) {
		indices[i] = mf->readUI16();
	}

	triangleCount = mf->readUI32();
}

/*
 * convert the data in this chunk to an object array
  */
void
ChunkLodVertexInfo::convertToObjectArray ()
{
	// Explode the vertices into a server-side vertex buffer
	const ChunkLodTree::Extensions* ext = ChunkLodTree::getExtensions (0, true);
	if (!ext->isVertexArrayObjectSupported ()) {
		return;		// no vertex array buffer support, return
	}

	float *fbuf = new float[indexCount * 4];
	for (int i = 0; i < indexCount; i++) {
		fbuf[i*4 + 0] = vertices[indices[i]].v[0];
		fbuf[i*4 + 1] = vertices[indices[i]].v[1];
		fbuf[i*4 + 2] = vertices[indices[i]].v[2];
		fbuf[i*4 + 3] = vertices[indices[i]].y_delta;
	}
	_vertexArray = ext->glNewObjectBufferATI (sizeof(float) * indexCount * 4, fbuf, GL_STATIC_ATI);
	delete [] fbuf;
	delete [] indices;
	delete [] vertices;
	indices = NULL;
	vertices = NULL;
}

////
//// extensions management
////

typedef osg::buffered_value< osg::ref_ptr<ChunkLodTree::Extensions> > BufferedExtensions;
static BufferedExtensions s_extensions;

const ChunkLodTree::Extensions*
ChunkLodTree::getExtensions (unsigned int contextID, bool createIfNotInitialized)
{
	if (!s_extensions[contextID] && createIfNotInitialized) s_extensions[contextID] = new Extensions;
	return s_extensions[contextID].get();
}

void
ChunkLodTree::setExtensions (unsigned int contextID, ChunkLodTree::Extensions* extensions)
{
	s_extensions[contextID] = extensions;
}

ChunkLodTree::Extensions::Extensions ()
{
	setupGLExtensions();
}

ChunkLodTree::Extensions::Extensions (const ChunkLodTree::Extensions& rhs) :
	osg::Referenced(),
	_vertexArrayObjectSupported (rhs._vertexArrayObjectSupported),
	_glNewObjectBufferATI (rhs._glNewObjectBufferATI),
	_glUpdateObjectBufferATI (rhs._glUpdateObjectBufferATI),
	_glFreeObjectBufferATI (rhs._glFreeObjectBufferATI),
	_glArrayObjectATI (rhs._glArrayObjectATI),
	_glVariantObjectArrayATI (rhs._glVariantObjectArrayATI)
{
}

void
ChunkLodTree::Extensions::lowestCommonDenominator (const ChunkLodTree::Extensions& rhs)
{
	// blah
	return;
}

void
ChunkLodTree::Extensions::setupGLExtensions ()
{
	_vertexArrayObjectSupported = osg::isGLExtensionSupported ("GL_ATI_vertex_array_object");

	std::cout << "NV VAR = " <<  osg::isGLExtensionSupported ("GL_NV_vertex_array_range") << "\n";;
	std::cout << "NV VAR2 = " <<  osg::isGLExtensionSupported ("GL_NV_vertex_array_range2") << "\n";;

#define GET_FUNC(f)  _##f = osg::getGLExtensionFuncPtr (#f)
	GET_FUNC (glNewObjectBufferATI);
	GET_FUNC (glUpdateObjectBufferATI);
	GET_FUNC (glFreeObjectBufferATI);
	GET_FUNC (glArrayObjectATI);
	GET_FUNC (glVariantObjectArrayATI);
#undef GET_FUNC
}

GLuint
ChunkLodTree::Extensions::glNewObjectBufferATI (GLsizei size, const void *pointer, GLenum usage) const
{
	if (_glNewObjectBufferATI) {
		typedef GLuint (APIENTRY * glNewObjectBufferATIPtr) (GLsizei size, const void *pointer, GLenum usage);
		return ((glNewObjectBufferATIPtr) _glNewObjectBufferATI) (size, pointer, usage);
	} else {
		osg::notify(osg::WARN) << "Error: glNewObjectBufferATI is not supported by OpenGL" << std::endl;
	}
	return 0;
}

void
ChunkLodTree::Extensions::glUpdateObjectBufferATI (GLuint buffer, GLuint offset, GLsizei size, const void *pointer, GLenum preserve) const
{
	if (_glUpdateObjectBufferATI) {
		typedef void (APIENTRY * glUpdateObjectBufferATIPtr) (GLuint buffer, GLuint offset, GLsizei size, const void *pointer, GLenum preserve);
		((glUpdateObjectBufferATIPtr) _glUpdateObjectBufferATI) (buffer, offset, size, pointer, preserve);
	} else {
		osg::notify(osg::WARN) << "Error: glUpdateObjectBufferATI is not supported by OpenGL" << std::endl;
	}
}

void
ChunkLodTree::Extensions::glFreeObjectBufferATI (GLuint buffer) const
{
	if (_glFreeObjectBufferATI) {
		typedef void (APIENTRY * glFreeObjectBufferATIPtr) (GLuint buffer);
		((glFreeObjectBufferATIPtr) _glFreeObjectBufferATI) (buffer);
	} else {
		osg::notify(osg::WARN) << "Error: glFreeObjectBufferATI is not supported by OpenGL" << std::endl;
	}
}

void
ChunkLodTree::Extensions::glArrayObjectATI (GLenum array, GLint size, GLenum type, GLsizei stride, GLuint buffer, GLuint offset) const
{
	if (_glArrayObjectATI) {
		typedef void (APIENTRY * glArrayObjectATIPtr) (GLenum array, GLint size, GLenum type, GLsizei stride, GLuint buffer, GLuint offset);
		((glArrayObjectATIPtr) _glArrayObjectATI) (array, size, type, stride, buffer, offset);
	} else {
		osg::notify(osg::WARN) << "Error: glArrayObjectATI is not supported by OpenGL" << std::endl;
	}
}

void
ChunkLodTree::Extensions::glVariantObjectArrayATI (GLuint id, GLenum type, GLsizei stride, GLuint buffer, GLuint offset) const
{
	if (_glVariantObjectArrayATI) {
		typedef void (APIENTRY * glVariantObjectArrayATIPtr) (GLuint id, GLenum type, GLsizei stride, GLuint buffer, GLuint offset);
		((glVariantObjectArrayATIPtr) _glVariantObjectArrayATI) (id, type, stride, buffer, offset);
	} else {
		osg::notify(osg::WARN) << "Error: glVariantObjectArrayATI is not supported by OpenGL" << std::endl;
	}
}

} // namespace osgChunkLod {
