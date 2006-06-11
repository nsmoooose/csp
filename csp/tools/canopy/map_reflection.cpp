/* Combat Simulator Project
 * Copyright 2006 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

// Computes rendering parameters for generating a texture map of the
// reflection of the cockpit from the canopy glass as seen by the
// pilot.  The output is used by the 'reflect' program to render the
// final texture map.
//
// For each texel in the output texture map, finds the corresponding
// point of reflection on the canopy surface, the reflection vector,
// and the approximate field of view of that texel.  The canopy
// geometry is read as a standard OSG model file (or other file
// formats that OSG supports, such as 3DS).  The canopy model should
// be positioned as it would in the full aircraft model, and must be
// aligned such that +X is right, +Y is forward, and +Z is up.
//
// Additional parameters such as the pilot viewpoint and texture
// projection must be specified.  The projection maps the surface
// of the canopy onto a cylinder positioned below the canopy.  The
// length of the cylinder should be the same or slightly greater
// than the length of the canopy.  Both the angle subtended by the
// canopy and the z coordinate of the cylinder axis must be specified.
// Lowering the cylinder reduces its curvature and tends to provide
// greater detail along the top of the canopy, compressing the mappnig
// at the sides.

#include <osg/ApplicationUsage>
#include <osg/ArgumentParser>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/Transform>
#include <osg/TriangleIndexFunctor>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <cstdio>
#include <iostream>


// Helper class for representing one triangle of the canopy geometry.  The
// vertex positions and normals are stored, as well as their coordinates in
// the projected space.
class Triangle {
public:
	// Construct a triangle from three vertices.  v0, v1, and v2 are the vertex
	// 3d coordinates.  n1, n2, and n3 are the vertex normals.  c0, c1, c2 are
	// the 2d coordinates of the vertices in the projected space.
	Triangle(
			osg::Vec3 const &v0, osg::Vec3 const &v1, osg::Vec3 const &v2,
			osg::Vec3 const &n0, osg::Vec3 const &n1, osg::Vec3 const &n2,
			osg::Vec2 const &c0, osg::Vec2 const &c1, osg::Vec2 const &c2) {
		_v[0] = v0;
		_v[1] = v1;
		_v[2] = v2;
		_n[0] = n0;
		_n[1] = n1;
		_n[2] = n2;
		_c[0] = c0;
		_c[1] = c1 - c0;
		_c[2] = c2 - c0;
		// constant factor used for intersection calculations
		_d = (_c[1].y() * _c[2].x() - _c[1].x() * _c[2].y());
		if (_d != 0.0) _d = 1.0 / _d;
	}

	// Get the bounding rectangle of this triangle in the projected space.
	void getBound(osg::Vec2 &lower, osg::Vec2 &upper) const {
		lower.x() = std::min(std::min(0.0f, _c[1].x()), _c[2].x());
		lower.y() = std::min(std::min(0.0f, _c[1].y()), _c[2].y());
		upper.x() = std::max(std::max(0.0f, _c[1].x()), _c[2].x());
		upper.y() = std::max(std::max(0.0f, _c[1].y()), _c[2].y());
		lower += _c[0];
		upper += _c[0];
	}

	// Test if the specified texel coordinates (in the projected space) fall inside
	// this triangle.  If so, return true, set 'surface' to the 3d coordinates of the
	// canopy surface that project to the texel, and set 'normal' to the interpolated
	// normal at that point on the surface.
	bool interpolate(osg::Vec2 const &texel, osg::Vec3 &surface, osg::Vec3 &normal) const {
		const double px = texel.x() - _c[0].x();
		const double py = texel.y() - _c[0].y();
		osg::Vec2 index;
		index.x() = (py * _c[2].x() - px * _c[2].y()) * _d;
		if (index.x() < 0.0) return false;
		index.y() = (py * _c[1].x() - px * _c[1].y()) * (-_d);
		if (index.y() < 0.0) return false;
		if (index.x() + index.y() > 1.0) return false;
		surface = blend(_v, index);
		normal = blend(_n, index);
		return true;
	}

	// Return the approximate center of the triangle.
	osg::Vec3 center() const {
		return blend(_v, osg::Vec2(0.33333f, 0.33333f));
	}

	// Compute the weighted average of three vectors in barycentric coordinates
	// c0 = 1 - c1 - c2, c1 = index.x(), c2 = index.y().
	static osg::Vec3 blend(osg::Vec3 const v[3], osg::Vec2 const &index) {
		return v[1] * index.x() + v[2] * index.y() + v[0] * (1.0f - index.x() - index.y());
	}

private:
	osg::Vec3 _n[3];
	osg::Vec3 _v[3];
	osg::Vec2 _c[3];
	double _d;
};


// A node visitor that traverses the canopy geometry rendering triangles to the
// projected map.
class ReflectVisitor: public osg::NodeVisitor {
public:
	// 'origin' and 'axis' define the cylindrical projection of the reflection map.
	// Currently axis must be along the y axis, although this could be generalized
	// without too much difficulty.  'angle' is the maximum angle subtended by the
	// canopy in the projected coordinates. 'viewpoint' is the pilot eye position
	// used to compute the reflection direction, and width and height are the
	// dimensions of the output texture.
	ReflectVisitor(osg::Vec3 const &origin, osg::Vec3 const &axis, double angle, osg::Vec3 const &viewpoint, int width, int height): osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN), _origin(origin), _axis(axis), _angle(angle), _viewpoint(viewpoint), _width(width), _height(height) {
		std::cout << viewpoint.x() << " " << viewpoint.y() << " " << viewpoint.z() << "\n";
		std::cout << width << " " << height << "\n";
	}

private:
	friend class Gatherer;

	// TriangleIndexFunctor base class that creates Triangle instances for each
	// triangle in a geometry and passes them back to the parent ReflectVisitor.
	class Gatherer {
	public:
		Gatherer(): _parent(0) {}
		// Initialize the visitor prior to passing it to a geometry instance.
		// 'matrix' is the current model transform to be applied to each vertex
		// and normal.  'origin' is the origin of the cylindrical projection.
		// 'vertices' and 'normals' are the vertex and normal arrays of the
		// geometry.  Note that normal binding must be per-vertex.
		void init(ReflectVisitor *parent, osg::Matrix const &matrix, osg::Vec3 const &origin, osg::Vec3Array const *vertices, osg::Vec3Array const *normals) {
			_parent = parent;
			assert(vertices && normals);
			const unsigned count = vertices->size();
			assert(normals->size() == count);
			_normals.resize(count);
			_vertices.resize(count);
			_coords.resize(count);
			for (unsigned i = 0; i < count; ++i) {
				_vertices[i] = (*vertices)[i] * matrix;
				_normals[i] = osg::Matrix::transform3x3((*normals)[i], matrix);
				osg::Vec3 v = _vertices[i] - origin;
				_coords[i] = osg::Vec2(atan2(v.z(), v.x()), v.y());
			}
		}

		// Called for each triangle in the geometry.  Construct a Triangle instance and
		// pass it to the parent ReflectVisitor.
		void operator()(int i, int j, int k) const {
			assert(_parent);
			_parent->render(Triangle(
				_vertices[i], _vertices[j], _vertices[k],
				_normals[i], _normals[j], _normals[k],
				_coords[i], _coords[j], _coords[k]));
		}

		std::vector<osg::Vec2> const &coords() const { return _coords; }

	private:
		ReflectVisitor *_parent;
		std::vector<osg::Vec3> _normals;
		std::vector<osg::Vec3> _vertices;
		std::vector<osg::Vec2> _coords;
	};

	// Generate output for the specified triangle.  Gatherer calls this method for each
	// triangle in the geometry.  Iterates over each output texel in the bounding box
	// of the triangle, checking for intersections and computing the corresponding
	// rendering parameters (such as the reflection vector).  Writes output to stdout.
	void render(Triangle const &tri) {
		const double edge = 0.5 * M_PI - _angle * 0.5;
		osg::Vec2 lower;
		osg::Vec2 upper;
		tri.getBound(lower, upper);

		// compute the bounding rectangle in texture coordinates
		const int y0 = static_cast<int>(_height * lower.y() / _axis.y());
		const int y1 = static_cast<int>(_height * upper.y() / _axis.y());
		const int x0 = static_cast<int>(_width * (lower.x() - edge) / (0.5 * _angle));
		const int x1 = static_cast<int>(_width * (upper.x() - edge) / (0.5 * _angle));

		// compute the approximate field of view per texel, taking it as constant
		// across the triangle.
		const float fov = getFieldOfView(tri.center()) * (180.0 / M_PI);

		for (int yi = y0; yi <= y1; ++yi) {
			if (yi < 0 || yi >= _height) continue;
			double y = _axis.y() * (1.0 * yi / _height);
			for (int xi = x0; xi <= x1; ++xi) {
				if (xi < 0 || xi >= _width) continue;
				double angle = edge + 0.5 * _angle * xi / _width;
				osg::Vec3 point;
				osg::Vec3 normal;
				if (tri.interpolate(osg::Vec2(angle, y), point, normal)) {
					osg::Vec3 look = (point - _viewpoint);
					look.normalize();
					normal.normalize();
					osg::Vec3 dir = look - normal * 2.0 * (look * normal);
					std::cout << xi << " " << yi << " " << point.x() << " " << point.y() << " " << point.z() << " " << dir.x() << " " << dir.y() << " " << dir.z() << " " << fov << "\n";
				}
			}
		}
	}

	// Get the approximate field of view for a texel at the specified position
	// as seen from the viewpoint.
	float getFieldOfView(osg::Vec3 const &p) const {
		osg::Vec3 xz(p.x() - _origin.x(), 0.0f, p.z() - _origin.z());
		float r = xz.length();
		float d = (p - _viewpoint).length();
		float dy = _axis.y() / _height;
		float dx = r * 0.5 * _angle / _width;
		return std::max(dx, dy) / d;
	}

	// For transform nodes, just accumulate the model transformation matrix.
	virtual void apply(osg::Transform &node) {
		osg::Matrix m = _matrix;
		node.computeLocalToWorldMatrix(_matrix, 0);
		traverse(node);
		_matrix = m;
	}

	// For geodes, construct a Gatherer to traverse the geometry and call
	// render() for each triangle.
	virtual void apply(osg::Geode &geode) {
		for (unsigned i = 0; i < geode.getNumDrawables(); ++i) {
			osg::Geometry *geom = geode.getDrawable(i)->asGeometry();
			if (!geom) continue;
			if (!geom->getNormalBinding() == osg::Geometry::BIND_PER_VERTEX) continue;
			osg::Vec3Array const *vertices = dynamic_cast<osg::Vec3Array const *>(geom->getVertexArray());
			osg::Vec3Array const *normals = dynamic_cast<osg::Vec3Array const *>(geom->getNormalArray());
			if (!normals || !vertices) continue;
			osg::TriangleIndexFunctor<Gatherer> gatherer;
			gatherer.init(this, _matrix, _origin, vertices, normals);
			geom->accept(gatherer);

			// Set new texture coordinates for the model geometry, which is optionally
			// saved at the end.
			std::vector<osg::Vec2> const &coords = gatherer.coords();
			if (!coords.empty()) {
				osg::Vec2Array *tc = new osg::Vec2Array(coords.size());
				const double edge = 0.5 * M_PI - _angle * 0.5;
				for (unsigned i = 0; i < coords.size(); ++i) {
					float x = (coords[i].x() - edge) / (0.5 * _angle);
					float y = coords[i].y() / _axis.y();
					(*tc)[i] = osg::Vec2(x, y);
				}
				geom->setTexCoordArray(0, tc);
			}
		}
		traverse(geode);
	}

	osg::Matrix _matrix;
	osg::Vec3 _origin;
	osg::Vec3 _axis;
	double _angle;
	osg::Vec3 _viewpoint;
	int _width;
	int _height;
};


// Traverse a scene graph to find the tightest possible global bounding box.
class BoundingBoxVisitor: public osg::NodeVisitor {
public:
	BoundingBoxVisitor(): osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN) {}

	// For transform nodes, just accumulate the model transformation matrix.
	virtual void apply(osg::Transform &node) {
		osg::Matrix m = _matrix;
		node.computeLocalToWorldMatrix(_matrix, 0);
		traverse(node);
		_matrix = m;
	}

	virtual void apply(osg::Geode &geode) {
		for (unsigned i = 0; i < geode.getNumDrawables(); ++i) {
			osg::Geometry const *geom = geode.getDrawable(i)->asGeometry();
			if (!geom) continue;
			osg::Vec3Array const *vertices = dynamic_cast<osg::Vec3Array const *>(geom->getVertexArray());
			if (!vertices) continue;
			for (unsigned j = 0; j < vertices->size(); ++j) {
				_bbox.expandBy((*vertices)[j] * _matrix);
			}
		}
		traverse(geode);
	}

	void reset() { _bbox.init(); }
	osg::BoundingBox const &bbox() const { return _bbox; }

private:
	osg::Matrix _matrix;
	osg::BoundingBox _bbox;
};


bool parseVector(std::string const &s, osg::Vec3 &vec) {
	float x, y, z;
	if (sscanf(s.c_str(), "%f,%f,%f", &x, &y, &z) == 3) {
		vec.set(x, y, z);
		return true;
	}
	return false;
}


int main(int argc, char **argv) {
	osg::ArgumentParser args(&argc, argv);
	args.getApplicationUsage()->setCommandLineUsage(args.getApplicationName() + " [options] model");
	args.getApplicationUsage()->addCommandLineOption("--angle <angle>", "projected angle subtended by the geometry in degrees");
	args.getApplicationUsage()->addCommandLineOption("--width <width>", "output texture width in pixels");
	args.getApplicationUsage()->addCommandLineOption("--height <height>", "output texture height in pixels");
	args.getApplicationUsage()->addCommandLineOption("--viewpoint <x,y,z>", "the view point in model coordinates");
	args.getApplicationUsage()->addCommandLineOption("--tc", "output file for geometry with texture coordinates");
	args.getApplicationUsage()->addCommandLineOption("-h or --help", "print usage message");

	int angle_degrees = 18;
	int width = 0;
	int height = 0;
	std::string tc;
	std::string viewpoint_string;
	osg::Vec3 viewpoint(0.0, 4.60, 1.2);

	args.read("--angle", angle_degrees);
	args.read("--width", width);
	args.read("--height", height);
	args.read("--viewpoint", viewpoint_string);
	args.read("--tc", tc);

	if (args.read("-h") || args.read("--help")) {
		args.getApplicationUsage()->write(std::cout, osg::ApplicationUsage::COMMAND_LINE_OPTION);
		return 1;
	}

	if (width <= 0 || height <= 0) {
		std::cerr << "Must specify positive --width and --height\n";
		return 1;
	}

	if (!parseVector(viewpoint_string, viewpoint)) {
		std::cerr << "Must specify --viewpoint x,y,z\n";
		return 1;
	}

	args.reportRemainingOptionsAsUnrecognized();

	if (args.argc() != 2) {
		args.getApplicationUsage()->write(std::cerr, osg::ApplicationUsage::COMMAND_LINE_OPTION);
		return 1;
	}

	std::string model_file = args[1];
	std::cerr << "Loading model file " << model_file << "\n";
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(model_file);

	BoundingBoxVisitor bbv;
	model->accept(bbv);
	std::cerr << "model bounds:\n";
	std::cerr << "  x: " << bbv.bbox().xMin() << " -> " << bbv.bbox().xMax() << "\n";
	std::cerr << "  y: " << bbv.bbox().yMin() << " -> " << bbv.bbox().yMax() << "\n";
	std::cerr << "  z: " << bbv.bbox().zMin() << " -> " << bbv.bbox().zMax() << "\n";

	// compute projection parameters from the model bounding box and angle_degrees.
	float angle = angle_degrees * M_PI / 180.0f;
	float origin_x = (bbv.bbox().xMin() + bbv.bbox().xMax()) * 0.5f;
	float origin_y = bbv.bbox().yMin();
	float origin_z = bbv.bbox().zMin() - (bbv.bbox().xMax() - origin_x) / atan(angle * 0.5f);
	float axis_y = bbv.bbox().yMax() - origin_y;

	osg::Vec3 origin(origin_x, origin_y, origin_z);
	osg::Vec3 axis(0.0, axis_y, 0.0);

	ReflectVisitor reflect(origin, axis, angle, viewpoint, width, height);
	model->accept(reflect);

	if (!tc.empty()) {
		osgDB::writeNodeFile(*model, tc);
	}

	return 0;
}

