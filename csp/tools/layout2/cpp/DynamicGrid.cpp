// CSPLayout
// Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US


#include <csp/tools/layout2/cpp/DynamicGrid.h>

#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>

#include <cmath>
#include <cassert>


class DynamicGrid::GridCallback: public osg::NodeCallback {
public:
	virtual void operator()(Node* node, osg::NodeVisitor* nv) {
		DynamicGrid *dynamic_grid = dynamic_cast<DynamicGrid*>(node);
		if (dynamic_grid) {
			dynamic_grid->update();
		}
		osg::NodeCallback::operator()(node, nv);
	}
};


/** Helper class for managing the scene graph representation of a single grid.
 */
class DynamicGrid::Grid {
public:
	/** Construct scene graph elements for a grid, using the specified polygon-offset.
	 */
	Grid(float offset) {
		_place = new osg::PositionAttitudeTransform;
		_material = new osg::Material();
		_material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.4,1.0));
		_material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.4,1.0));
		_place->getOrCreateStateSet()->setAttributeAndModes(_material.get(), osg::StateAttribute::ON);
		_place->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
		_place->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(offset, -5.0), osg::StateAttribute::ON);
		setTransparency(0.8);
		osg::PolygonMode *poly = new osg::PolygonMode;
		poly->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		_place->getOrCreateStateSet()->setAttribute(poly);
	}

	/** Set the node (typically a geode) that contains the grid drawable.
	 */
	void setNode(osg::Node *node) {
		assert(node);
		_grid = node;
		_place->addChild(node);
	}

	/** Set the scale of the grid relative to the raw grid drawable.
	 */
	void setScale(float s) {
		_place->setScale(osg::Vec3(s, s, s));
	}

	/** Set the grid transparency (0.0 = visible, 1.0 = invisible)
	 */
	void setTransparency(float t) {
		if (t > 1.0) t = 1.0;
		_material->setTransparency(osg::Material::FRONT_AND_BACK, t);
	}

	/** Add this grid to the specified group.
	 */
	void addToGroup(osg::Group* group) {
		group->addChild(_place.get());
	}

	/** Position the grid so that the origin is at the specified location.
	 */
	void setPosition(osg::Vec3 const &pos) {
		_place->setPosition(pos);
	}

private:
	osg::ref_ptr<osg::Node> _grid;
	osg::ref_ptr<osg::PositionAttitudeTransform> _place;
	osg::ref_ptr<osg::Material> _material;
};


DynamicGrid::DynamicGrid(): _gridA(0), _gridB(0), _level(-1), _look(0.0, 0.0, 0.0), _eye(0.0, 0.0, 1.0), _fade(0.0) {
	// construct the grids, using polygon offset to keep the coarse grid above the fine grid.
	_gridA = new Grid(-5.0);
	_gridB = new Grid(-2.5);
	// use 10 cm grid scale for the base grid and scale it up to 1 m for the finest grid.
	// for some reason the grid transparency/color renders incorrectly for scale factors
	// less than 10.
	osg::Node *grid = makeGrid(100, 0.1);
	// both coarse and fine grids share the same grid geode.
	_gridA->setNode(grid);
	_gridB->setNode(grid);
	_gridA->addToGroup(this);
	_gridB->addToGroup(this);
	setUpdateCallback(new GridCallback);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
}

DynamicGrid::~DynamicGrid() {
	delete _gridA;
	delete _gridB;
}

void DynamicGrid::setLook(osg::Vec3 const &look, osg::Vec3 const &eye) {
	_look = look;
	_eye = eye;
	osg::Vec3 dir(look - eye);
	dir.normalize();
	// fade out at oblique viewing angles to hide the finite extent of the
	// grid and prevent aliasing.  the grids are fully transparent when
	// _fade >= 1.0, which occurs for viewing angles exceeding 60 degrees
	// off-vertical.
	_fade = 2.0 * (1.0 - std::abs(dir.z()));
}

void DynamicGrid::update() {
	double z = _eye.z() * 1.0;
	if (z < 10.0) z = 10.0;
	double scale = log(z)/log(10.0);

	// level is a logarithmic scale in camera distance from the grid plane:
	//   1 for z < 100
	//   2 for 100 <= z < 1000
	//   3 for 1000 <= z < 10^4
	//   etc.
	int level = static_cast<int>(floor(scale));

	// when the level increases, the fine grid is rescaled to match the old coarse grid
	// and the coarse grid is scaled up by a factor of 10.  the reverse happens when the
	// level decreases.
	if (level != _level) {
		// scale must be >= 10 to prevent transparency/grid-color artifacts; not sure why
		// (maybe a bug in OSG or the GL drivers?).  to compensate the raw grid spacing is
		// set to 10 cm instead of 1 m (see the ctor).
		_scaleB = exp(level * log(10.0));
		_scaleA = _scaleB * 10.0;
		_gridA->setScale(_scaleA);
		_gridB->setScale(_scaleB);
		_level = level;
	}

	// fade factor for the fine grid, which becomes increasingly transparent as the camera
	// zooms out.  it is fully transparent before the level changes to hide the transition.
	double f = scale - level;

	// set the grid transparencies using the viewing angle and fine grid fade factors.
	_gridA->setTransparency(0.6 + 0.4 * _fade);
	_gridB->setTransparency(0.6 + f * f * 0.4 + 0.4 * _fade);

	if (_fade < 1.0) {
		// reposition the grids by multiples of the grid spacing to keep the center near the
		// look point.  we also offset the grids slightly in Z, which together with the
		// PolygonOffset state attribute helps to ensure the correct Z ordering at both
		// near and far viewing ranges.
		osg::Vec3 offsetA(floor(_look.x() / _scaleA) * _scaleA, floor(_look.y() / _scaleA) * _scaleA, 0.02);
		osg::Vec3 offsetB(floor(_look.x() / _scaleB) * _scaleB, floor(_look.y() / _scaleB) * _scaleB, 0.01);
		_gridA->setPosition(offsetA);
		_gridB->setPosition(offsetB);
	}
}

// Construct a grid geometry using two overlapping quadstrips (one for X, one for Y).
// Sets the color to white and the vertex normals to +Z.
osg::Node *DynamicGrid::makeGrid(int size, float spacing) {
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();
	osg::Vec3Array* grid_points = new osg::Vec3Array(4*(size+1));
	osg::DrawElementsUShort *grids_x = new osg::DrawElementsUShort(osg::PrimitiveSet::QUAD_STRIP);
	osg::DrawElementsUShort *grids_y = new osg::DrawElementsUShort(osg::PrimitiveSet::QUAD_STRIP);
	grids_x->reserve(4*(size+1));
	grids_y->reserve(4*(size+1));
	for (int i = 0; i <= size ; ++i) {
		float x = (i - size/2) * spacing;
		float y = size / 2 * spacing;
		(*grid_points)[2*i].set(x, y, 0.0);
		(*grid_points)[2*i+1].set(x, -y, 0.0);
		(*grid_points)[2*(i+size)+2].set(y, x, 0.0);
		(*grid_points)[2*(i+size)+3].set(-y, x, 0.0);
		grids_x->push_back(2*i);
		grids_x->push_back(2*i+1);
		grids_y->push_back(2*(i+size)+2);
		grids_y->push_back(2*(i+size)+3);
	}
	geom->setVertexArray(grid_points);
	geom->addPrimitiveSet(grids_y);
	geom->addPrimitiveSet(grids_x);

	osg::Vec3Array* color_array = new osg::Vec3Array;
	color_array->push_back(osg::Vec3(1.0, 1.0, 1.0));
	geom->setColorArray(color_array);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0, 0, 1));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	geom->setUseDisplayList(true);
	geode->addDrawable(geom);
	return geode;
}


