#pragma once
// Combat Simulator Project
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
 * @file SimpleSceneManager.h
 *
 **/

#include <csp/cspsim/battlefield/SceneManager.h>

namespace csp {

class VirtualScene;
class FeatureGroup;
class DynamicObject;


/** SceneManager subclass that serves as a minimal adapter to VirtualScene.
 */
class SimpleSceneManager: public SceneManager
{
private:
	Ref<VirtualScene> m_Scene;

	// (internal) specialized hide/show methods
	void scheduleHideFeature(Ref<FeatureGroup> feature);
	void scheduleShowFeature(Ref<FeatureGroup> feature);
	void scheduleHideDynamic(Ref<DynamicObject> dynamic);
	void scheduleShowDynamic(Ref<DynamicObject> dynamic);

	// main (internal) entry point for showing or hiding an object.
	void scheduleVisibilityChange(ObjectRef const& object, bool hide);

public:

	/** Construct a new SimpleSceneManager.
	 *
	 *  @param scene The VirtualScene class that serves as an interface to the scene graph.
	 *  @param visilibity_range The range, in meters, around the camera in which objects are visible.
	 */
	SimpleSceneManager(Ref<VirtualScene> scene, double visibility_range);

	virtual ~SimpleSceneManager();

	virtual void scheduleHide(ObjectRef const& object);
	virtual void scheduleShow(ObjectRef const& object);
	virtual void setCamera(Vector3 const &eye_point, Vector3 const &look_pos, Vector3 const &up_vec);

};

} // namespace csp
