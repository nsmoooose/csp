// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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


/**
 * @file SceneManager.h
 *
 **/

#ifndef __SIMCORE_BATTLEFIELD_SCENEMANAGER_H__
#define __SIMCORE_BATTLEFIELD_SCENEMANAGER_H__


#include <SimData/Ref.h>

class SimObject;

namespace simdata {
	class Vector3;
}


/** Abstract base class for managing sim objects in the scene graph.
 *
 *  SceneManager acts as an adapter between the Battlefield and the
 *  scene graph (e.g. VirtualScene).  The Battlefield class uses the
 *  SceneManager to schedule the addition and removal of objects from
 *  the scene based on visibility (ie. proximity to the camera).
 *  In addition, camera position updates are passed to the scene
 *  graph class from the battlefield via the SceneManager.
 *
 *  NB: SceneManager methods should only be called by the Battlefield
 *  class.
 *
 *  The most basic SceneManager subclass would simply forward show
 *  and hide events on the the scene graph class.  More advanced
 *  possibilities include, for example:
 *    - construction of scene graph elements in a separate thread
 *      (rather than blocking).
 *    - caching scene graph elements.
 *    - smoothing the transition at the edge of the visibile range
 *      using alpha blending.
 */
class SceneManager: public simdata::Referenced
{
friend class Battlefield;

protected:
	typedef simdata::Ref<SimObject> ObjectRef;

	/** SceneManager is a friend of SimObject, allowing it to modify
	 *  the visible flag.  Subclasses shoud use this method to update
	 *  the visible flag as soon as objects are scheduled to be shown
	 *  or hidden.
	 */
	static void setVisible(ObjectRef const& object, bool visible);

	/** The visibility range is the radius (in meters) around the camera
	 *  in which objects will be added to the scene.  It is immutable in
	 *  order to simplify the battlefield implementation.
	 *  TODO make the visibility range be object-size dependent; the
	 *  value here may remain as the upper limit.
	 */
	const double m_VisibilityRange;

public:
	/** Construct a new scene manager.
	 *
	 *  @param visibility_range The radius (in meters) around the camera within
	 *                          which objects are visible (ie. added to the scene).
	 */
	SceneManager(double visibility_range): m_VisibilityRange(visibility_range) { }

	virtual ~SceneManager() { }

	/** Get the visible range (in meters).
	 */
	inline double getVisibleRange() const { return m_VisibilityRange; }

private:
	/** Schedule an object to be removed from the scene graph.  The object can be either
	 *  a DynamicObject or a FeatureGroup.  The object must already be marked as visible.
	 *  The visibility flag (object->isVisible()) is changed immediately, but removal from
	 *  the scene graph may be deferred.  Test object->isInScene() to determine if an object
	 *  sprite is still present in the scene graph.
	 */
	virtual void scheduleHide(ObjectRef const&) = 0;

	/** Schedule an object to be added to the scene graph.  The object can be either
	 *  a DynamicObject or a FeatureGroup.  The object must not already be marked as
	 *  visible.  The visibility flag (object->isVisible()) is changed immediately,
	 *  but addition to the scene graph may be deferred.  Test object->isInScene() to
	 *  determine if an object sprite is present in the scene graph.
	 */
	virtual void scheduleShow(ObjectRef const&) = 0;

	/** Update the camera position and orientation.
	 *
	 *  Called by the battlefield whenever the camera parameters change.
	 */
	virtual void setCamera(simdata::Vector3 const &eye_point, simdata::Vector3 const &look_pos, simdata::Vector3 const &up_vec) = 0;
};


#endif // __SIMCORE_BATTLEFIELD_SCENEMANAGER_H__

