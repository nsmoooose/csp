// CSPLayout
// Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
//
// Based in part on osgpick sample code
// OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield
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


#ifndef __CSP_LAYOUT_VIEW_H__
#define __CSP_LAYOUT_VIEW_H__

#include "LayoutNodes.h"

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <string>
#include <vector>
#include <osg/Vec3>
#include <OpenThreads/Mutex>
#include <SimData/Object.h>

namespace osgProducer { class Viewer; }
class FeatureGraph;
class ViewManipulator;
class PickHandler;
class DynamicGrid;


// TODO factor generic code from view and graph callbacks into a separate header.
/** Callback for receiving view events.
 */
class ViewCallback: public osg::Referenced {
public:
	virtual ~ViewCallback() {}
	virtual void onRotateMode() {}
	virtual void onMoveMode() {}
	virtual void onUpdatePosition(float mouse_x, float mouse_y, float pos_x, float pos_y, float dx, float dy, float angle) {}
	virtual void onKey(std::string const &) {}
};

typedef int CallbackId;


/** View class for the 3D layout.  Contains and provides access to the FeatureGraph
 *  and ViewManipulator.
 */
class View {
public:
	View();
	~View();
	int init(int argc, char **argv);
	void run();
	void quit();

	/** Get the FeatureGraph.
	 */
	FeatureGraph* graph() { return m_FeatureGraph; }

	/** Set the view to look along the X-axis.  The sign of the viewing direction
	 *  projection onto the X-axis is not changed.
	 */
	void setViewX();

	/** Set the view to look along the Y-axis.  The sign of the viewing direction
	 *  projection onto the Y-axis is not changed.
	 */
	void setViewY();

	/** Set the view to look straight down along the Z-axis, with "up" in the +Y direction.
	 */
	void setViewZ();

	void setCamera(float x0, float y0, float z0, float x1, float y1, float z1);

	/** Get the current camera position.
	 */
	osg::Vec3 getCameraPosition() const;

	/** Get the current camera look point (on the Z=0 plane).
	 */
	osg::Vec3 getCameraTarget() const;

	/** Get the distance from the camera to the look point.
	 */
	float getCameraDistance() const;

	/** Get the (x, y) world coordinates of the mouse pointer (in normalized
	 *  event coordinates) projected onto the Z=0 plane.
	 */
	void screenToSurface(float x, float y, float &surface_x, float &surface_y);

	/** Transform normalized mouse event coordinates to window coordinates.
	 */
	bool computePixelCoords(float x, float y, float& pixel_x, float& pixel_y);

	/** Update the world coordinates of the mouse pointer from normalized event coordinates.
	 */
	void updateMouseCoordinates(float xnorm, float ynorm);

	/** Send a view update-position event.  Unknown or irrelevant parameters should be set to
	 *  zero.
	 *   @param norm_x normalized mouse pointer X coordinate.
	 *   @param norm_y normalized mouse pointer Y coordinate.
	 *   @param position_x world X coordinate.
	 *   @param position_y world Y coordinate.
	 *   @param dx world X displacement.
	 *   @param dy world Y displacement.
	 *   @param angle angle change.
	 */
	void updatePosition(float norm_x, float norm_y, float position_x, float position_y, float dx, float dy, float angle);

	/** Send a view event to indicate a key event captured (but ignored) by the 3D view window.
	 */
	void sendKey(std::string const &key);

#ifdef SWIG
	void getMouseCoordinates(float &OUTPUT, float &OUTPUT);
#else
	/** Get the current mouse pointer position in world coordinates.   The Python to
	 *  this method takes no arguments and returns a tuple (x, y).
	 */
	void getMouseCoordinates(float &x, float &y);
#endif

	/** Helper method for accessing ViewManipulator::centerViewOnNode().
	 */
	void centerViewOnNode(LayoutNodePath *path, LayoutNode *node);

	/** Get the scene graph viewer.
	 */
	osgProducer::Viewer *getViewer() { return m_Viewer; }

	/** Helper method for accessing ViewManipulator::setMoveMode().
	 */
	void setMoveMode();

	/** Helper method for accessing ViewManipulator::setRotateMode().
	 */
	void setRotateMode();

	/** Add a view callback for receiving view events.
	 */
	CallbackId addCallback(ViewCallback *callback);

	/** Remove a view callback, where id is the value returned by addCallback().
	 */
	void removeCallback(CallbackId id);

	/** Fire callbacks to indicate a change in the editing mode (move/rotate).
	 */
	void signalEditMode();

	/** Delete the currently selected nodes.
	 */
	void deleteSelectedNodes();

	/** Get the scale of the fine grid for the current camera position.
	 */
	float getGridScale() const;

private:
	void setViewAxis(osg::Vec3 const &axis, osg::Vec3 const &up);
	void prepareScene();
	void makeLights(osg::Group *group);
	void makeGround(osg::Group *group);

	bool m_Quit;

	osgProducer::Viewer *m_Viewer;
	FeatureGraph *m_FeatureGraph;
	ViewManipulator *m_Manipulator;
	PickHandler *m_Picker;
	osg::ref_ptr<DynamicGrid> m_DynamicGrid;

	float m_MouseX;
	float m_MouseY;

	typedef std::list< osg::ref_ptr<ViewCallback> > ViewCallbackList;
	ViewCallbackList m_ViewCallbacks;
};


#endif // __CSP_LAYOUT_VIEW_H__
