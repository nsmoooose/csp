// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file Animation.h
 *
 * Classes for driving 3D animations.
 **/

#ifndef __CSPSIM_ANIMATION_H__
#define __CSPSIM_ANIMATION_H__

#include <csp/csplib/data/Object.h>
#include <osg/NodeCallback>
#include <typeinfo>

namespace osg { class AnimationPath; }
namespace osg { class AnimationPathCallback; }
namespace osg { class Node; }

namespace csp {

class Bus;


/** Base class for animation callbacks.
 *
 *  This class serves as an update callback for a transformation node that
 *  animates part of a model scene graph.  AnimationCallbacks are created by
 *  Animation instances whene a SceneModel is generated and typically retain
 *  a reference to their parent Animation instance to access constant control
 *  parameters.
 */
class AnimationCallback: public osg::NodeCallback {
public:
	AnimationCallback();
	virtual ~AnimationCallback();

	/** Attach this callback to the specified node.  If the node already has
	 *  one or more associated AnimationCallbacks, this callback will be nested
	 *  after the previously bound callbacks.
	 */
	void bind(osg::Node &node);

	/** Bind the callback to the data channels that control the animation.
	 */
	virtual bool bindChannels(Bus*) { return true; }

	/** Called (via VirtualScene) when a child geode has been picked by the mouse.
	 *  The default implementation does nothing, but subclasses may handle the event
	 *  (e.g., by toggling a switch) and return true to prevent further processing.
	 */
	virtual bool pick(int /*flags*/) { return false; }

	/** Get the elapsed time in seconds since the last call to elapsedTime.
	 */
	double elapsedTime(const osg::NodeVisitor *nv);

private:
	double m_LastUpdateTime;
};


/** Base class for defining animations of 3d model components.
 *
 *  Animation instances are bound to ObjectModels to provide data about
 *  particular model animations.  Animation instances, like ObjectModels,
 *  are shared by multiple SceneModel instances.  The actual animation of
 *  the 3d model is performed by customized AnimationCallbacks that are
 *  created by the Animation instances and bound directly to model nodes.
 */
class Animation: public Object {
public:
	CSP_DECLARE_ABSTRACT_OBJECT(Animation)

	Animation(): m_LOD(0) { }

	/** Create a new animation callback and bind it to the specified node.
	 */
	virtual AnimationCallback *newCallback(osg::Node *node) const = 0;

	/** Get the label of the node that this animation should be bound to.  The
	 *  convention for labelling nodes is "ANIM: anim_label: part_label" where
	 *  anim_label defines the animation and corresponds to the string returned
	 *  by this method.
	 */
	const std::string &getNodeLabel() const { return m_NodeLabel; }

	/** Get the minimum level of detail at which this animation should be
	 *  active.  This is intended to reduce the time spent processing animations
	 *  that are too small to be visible.  The active level of detail depends on
	 *  the distance from the camera to the object.
	 */
	int getLOD() const { return m_LOD; }

	/** Test if this animation expects to be bound to a osg::Switch node.  If the
	 *  node is some other type of group, a new switch node should be inserted
	 *  between the node and its children.  This is to facilitate modeling
	 *  programs that have no notion of an osg::Switch (i.e., most of them).
	 */
	virtual bool needsSwitch() const { return false; }

private:
	std::string m_NodeLabel;
	int m_LOD;
};

	void registerAnimationObjects();

} // namespace csp

#endif // __CSPSIM_ANIMATION_H__

