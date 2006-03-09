/*
 * RayStrip.cpp
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

#include <csp/modules/chunklod/RayStrip>
#include <csp/modules/chunklod/ChunkLod>


namespace osgChunkLod {

typedef ChunkLodVertex Vertex;

inline osg::Vec3 toVec3(Vertex const &v, osg::Vec3 const &offset, osg::Vec3 const &scale, float morph = 0.0) {
	return osg::Vec3(scale.x() * v.v[0], scale.y() * (v.v[1] + morph * v.y_delta), scale.z() * v.v[2]) + offset;
}

bool intersectRayStrip(
		osg::LineSegment const *segment,
		Vertex const *vertices,
		unsigned short const *indices,
		int n_indices,
		float morph,
		osg::Vec3 const &offset,
		osg::Vec3 const &scale,
		int &index,
		osg::Vec3 &normal,
		bool exhaustive,
		float &t,
		float &u,
		float &v) {
	if (n_indices < 3 || vertices == 0 || indices == 0 || !segment) {
		assert(0);
		return false;
	}
	osg::Vec3 origin = segment->start();
	osg::Vec3 dir = segment->end() - origin;
	osg::Vec3 edge1, edge2, B, C;
	int start = index;
	if (start < 2 || start >= n_indices) start = 2;
	int i = start-2;
	B = toVec3(vertices[indices[i++]], offset, scale, morph);
	C = toVec3(vertices[indices[i++]], offset, scale, morph);
	edge1 = C - B;
	float epsilon = 1.0e-12 * dir.length();
	//for (int i = 2; i < n_indices; i++) {
	int watchdog=65536;

	bool hit = false;
	float _t, _u, _v;

	while (1) {
		if (--watchdog == 0) {
			std::cout << "infinite loop\n";
			std::cout << n_indices << " " << start << " " << i << "\n";
		}
		if (watchdog < 0 && abs(start - i) < 10) {
			std::cout << i << " " << start << "\n";
		}
			
		if (i >= n_indices) {
			i = 0;
			if (start <= 2) break;
			B = toVec3(vertices[indices[i++]], offset, scale, morph);
			C = toVec3(vertices[indices[i++]], offset, scale, morph);
			edge1 = C - B;
		}
		B = C;
		C = toVec3(vertices[indices[i++]], offset, scale, morph);
		if (i == start) break;
		edge2 = -edge1;
		edge1 = C - B;
		osg::Vec3 pvec = dir ^ edge2;
		float det = edge1 * pvec;
		if (det > -epsilon && det < epsilon) continue;
		float inv_det = 1.0 / det;
		osg::Vec3 tvec = origin - B;
		_u = (tvec * pvec) * inv_det;
		if (_u < 0.0f || _u > 1.0) continue;
		osg::Vec3 qvec = tvec ^ edge1;
		_v = (dir * qvec) * inv_det;
		if (_v < 0.0f || _u+_v > 1.0) continue;
		_t = (edge2 * qvec) * inv_det;
		if (_t < 0.0 || _t > 1.0) continue;
		if (!hit || _t < t) {
			t = _t;
			u = _u;
			v = _v;
			index = i-1;
			//if (index < 0) index += n_indices;
			normal = edge1 ^ edge2;
			normal.normalize();
			//std::cout << "HIT @ " << index << " of " << n_indices << "\n";
			//std::cout << origin << " " << dir << "\n";
			hit = true;
		}
		if (!exhaustive) break;
	}
	//std::cout << "MISS\n";
	return hit;
}


inline int getX(Vertex const *vertices, unsigned short const *indices, int index) {
	return int((vertices + indices[index])->v[0]) << 1;
}

inline int getZ(Vertex const *vertices, unsigned short const *indices, int index) {
	return int((vertices + indices[index])->v[2]) << 1;
}

bool intersectVerticalStrip(
		ChunkLodElevationTest &test,
		Vertex const *vertices,
		unsigned short const *indices,
		int n_indices,
		float morph,
		osg::Vec3 const &offset,
		osg::Vec3 const &scale) {
	osg::Vec3 pos = test.getPoint();
	int index = test.getIndex();
	if (n_indices < 3 || vertices == 0 || indices == 0) {
		std::cerr << "osgChunkLod::intersectVerticalStrip() invalid chunk.\n";
		assert(0);
		return false;
	}
	int start = index;
	if (start < 2 || start >= n_indices) start = 2;
	int i = start-2;
	pos.y() = 0.0;
	// get integer coordinates of test point, scaled by 2 (see getX and getZ)
	int x_pos = int(floor(2.0 * (pos.x()-offset.x()) / scale.x()));
	int z_pos = int(floor(2.0 * (pos.z()-offset.z()) / scale.z()));
	// comp is our vertex test accumulator.  each of the three lower bits
	// represents a coordinate comparison with one of the vertices of a
	// triangle.  values of 0 and 7 mean all vertices are to the left or
	// to the right of the test point (no hit).  values of 1-6 indicate
	// enclosure in one axis.  repeating the test for the other axis is
	// necessary to detect an intersection.
	register int comp = 0;
	// initialize with our first 2 tests
	comp |= getX(vertices, indices, i++) > x_pos ? 2 : 0;
	comp |= getX(vertices, indices, i++) > x_pos ? 4 : 0;
	int remaining=n_indices;
	while (--remaining >= 0) {
		// tight loop, find triangles that straddle x
		comp = (comp >> 1) | (getX(vertices, indices, i++) > x_pos ? 4 : 0);
		if (comp != 0 && comp != 7) {
			// x test succeded, now check z
			int z_comp = getZ(vertices, indices, i-3) > z_pos ? 1 : 0;
			z_comp |= getZ(vertices, indices, i-2) > z_pos ? 2 : 0;
			z_comp |= getZ(vertices, indices, i-1) > z_pos ? 4 : 0;
			if (z_comp != 0 && z_comp != 7) {
				// localized in x and z, now the real test
				osg::Vec3 A = toVec3(vertices[indices[i-1]], offset, scale, morph);
				osg::Vec3 B = toVec3(vertices[indices[i-2]], offset, scale, morph);
				osg::Vec3 C = toVec3(vertices[indices[i-3]], offset, scale, morph);
				osg::Vec3 edge2 = A - B;
				osg::Vec3 edge1 = C - B;
				osg::Vec3 normal = edge2 ^ edge1;
				osg::Vec3 R = pos - B;
				float u = (R.x() * edge2.z() - R.z() * edge2.x()) / normal.y();
#if 0
				std::cout << morph << ", scale=" << scale << ", offset=" << offset << "\n";
				std::cout << "COMP=" << comp << ", ZCOMP=" << z_comp << "\n";
				std::cout << "VX = " << vertices[indices[index]].v[0] << ", " << vertices[indices[index]].v[2] << "\n";
				std::cout << "VX = " << vertices[indices[index-1]].v[0] << ", " << vertices[indices[index-1]].v[2] << "\n";
				std::cout << "VX = " << vertices[indices[index-2]].v[0] << ", " << vertices[indices[index-2]].v[2] << "\n";
				std::cout << "IX = " << (0.5*x_pos) << ", " << (0.5*z_pos) << "\n";
				std::cout << pos << " => " << A << ":" << B << ":" << C << " ln2=" << normal.length2() << "\n";
				std::cout << "N=" << normal << "\n";
				std::cout << "U=" << u << "\n";
				static int xxx = 0; xxx++;
				if (xxx > 40) // XXX XXX
					::exit(1);
#endif
				if (u >= 0.0f && u <= 1.0f) {
					float v = (R.z() * edge1.x() - R.x() * edge1.z()) / normal.y();
					//std::cout << "V=" << v << "\n";
					if (v >= 0.0f && v+u <= 1.0) {
						// success, we're in!
						//std::cout << "HIT @ " << index << " of " << n_indices << ", start=" << start << "\n";
						index = i - 1;
						normal.normalize();
						float height = - R * normal / normal.y();
						// fix triangles with inverted normals
						if (normal.y() < 0.0) normal = -normal;
						test._setHit(height, index, normal);
						test._setVertices(A, B, C);
						test._setParameters(offset, scale);
						test._setSourceVertices(vertices[indices[i-1]],
						                        vertices[indices[i-2]],
						                        vertices[indices[i-3]]);
						return true;
					}
				}
			}
		}
		// miss, check index rollover
		if (i == n_indices) {
			if (start <= 2) {
				return false;
			}
			comp = (comp >> 1) | (getX(vertices, indices, 0) > x_pos ? 4 : 0);
			comp = (comp >> 1) | (getX(vertices, indices, 1) > x_pos ? 4 : 0);
			i = 2;
		}
	}
	return false;
}



} // namespace osgChunkLod

