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


#ifndef __CSP_LAYOUT_DYNAMICGRID_H__
#define __CSP_LAYOUT_DYNAMICGRID_H__

#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/Group>


/**
 * A scene element that provides a 2D reference grid.  Two grids, one
 * coarse and one fine, are alternated to create the illusion of unlimited
 * scale division as a function of camera distance from the grid.  The
 * grid is only visible when the view is nearly perpendicular to the
 * grid surface (defined as the x-y plane).
 */
class DynamicGrid: public osg::Group {

	class Grid;
	class GridCallback;

	Grid *_gridA;     // coarse grid
	Grid *_gridB;     // fine grid
	int _level;
	osg::Vec3 _look;  // current view point on the grid
	osg::Vec3 _eye;   // current camera position
	float _fade;      // 0-1 transparency of fine grid
	float _scaleA;
	float _scaleB;

public:
	DynamicGrid();
	~DynamicGrid();

	/**
	 * Call this function to update the grid visibility based on the
	 * viewing direction.  The grid fades out as the viewing direction
	 * moves away from +/- Z to prevent aliasing and to hide the finite
	 * extent of the grid.
	 *
	 * @param look The view target (at z=0).
	 * @param eye The camera position.
	 */
	void setLook(osg::Vec3 const &look, osg::Vec3 const &eye);

	/**
	 * Get the minimum grid scale at the current camera position.
	 */
	float getGridScale() const {
		// the 0.1 factor compensates for an extra scaling factor that was
		// introduced to work around rendering anomalies; see update().
		return _scaleB * 0.1;
	}

private:
	/** Callback used to update the grid scale and transparency.
	 */
	void update();

	/**
	 * Helper function to create the grid geometry.
	 *
	 * @param size The number of divisions.
	 * @param spacing The distance between divisions.
	 */
	static osg::Node *makeGrid(int size, float spacing);
};

#endif // __CSP_LAYOUT_DYNAMICGRID_H__
