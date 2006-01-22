// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file HUD.h
 *
 **/


#ifndef __CSPSIM_HUD_HUD_H__
#define __CSPSIM_HUD_HUD_H__

#include <csp/cspsim/hud/Display.h>
#include <csp/cspsim/hud/DisplayTools.h>

namespace osg { class Material; }

CSP_NAMESPACE

/** A container class representing a Heads Up Display.  The display is composed
 *  of one or more Elements.  The HUD defines a fixed frame to contain elements
 *  that are positioned relative to the HUD using X,Y coordinates, and also
 *  accepts floating elements that are positioned based on direction relative to
 *  the point of view.  Changes in the camera position are detected and used to
 *  automatically update the position of floating elements.
 *
 *  The HUD is positioned via ObjectModel and bound to the scene graph through
 *  SceneModel.  HUD is not meant to be subclassed.  Instead subclass System for
 *  a specialized HUD implementation.  The System subclass should create a HUD
 *  instance and populate it with customized elements.
 */
class HUD: public Display {

protected:
	// Forward frame is a special frame that is used internally by the HUD to define
	// the primary 2D coordinate system for non-direction elements.   The origin of
	// the frame is aligned with the forward vector (y-axis in body coordinates) relative
	// to the pilot's point of view.  Thus if the pilot's head moves off center, the
	// forward frame will be similarly displaced relative to the HUD glass (as though
	// the elements in the frame were positioned at infinity).
	class ForwardFrame: public display::Frame {
		osg::ref_ptr<osg::PositionAttitudeTransform> m_Transform;
		virtual void _addElement(display::Element *element);
	public:
		osg::PositionAttitudeTransform *transform() { return m_Transform.get(); }
		osg::PositionAttitudeTransform const *transform() const { return m_Transform.get(); }
		ForwardFrame(): m_Transform(new osg::PositionAttitudeTransform) { }
	};

public:
	HUD();
	~HUD();

	/** Update the view point.  This method is called by SceneModel when the HUD is first
	 *  added to the scene.  It should also be called whenever the pilot's head moves.
	 */
	void setViewPoint(osg::Vec3 const &view_point);

	/** Set the origin of the HUD in body coordinates.  This position is managed through
	 *  ObjectModel.
	 */
	void setOrigin(osg::Vec3 const &origin);

	/** Set the HUD display color.
	 */
	void setColor(osg::Vec4 const &color);

	// TODO replace with Animation.Display; for backwards compatability only.
	osg::PositionAttitudeTransform *hud();

	/** Get the forward frame.
	 */
	display::Frame *forwardFrame();

	/** Get the origin of the forward frame relative to the HUD origin in body coordinates.
	 */
	osg::Vec3 getForwardFrameOrigin() const;

	/** Add an element to the forward frame.  Do not add floating elements with this method
	 *  since will not be updated correctly if the pilot's viewpoint changes; instead use
	 *  addFloatingElement.
	 */
	void addFrameElement(display::Element *element);

	/** Convenience method for adding a frame element by reference.
	 */
	template <class T>
	void addFrameElement(osg::ref_ptr<T> &element) {
		addFrameElement(element.get());
	}

	/** Add a floating element which will be positioned based on a direction relative to
	 *  the pilot's point of view.
	 */
	void addFloatingElement(display::Element *element);

	/** Convenience method for adding a floading element by reference.
	 */
	template <class T>
	void addFloatingElement(osg::ref_ptr<T> &element) {
		addFloatingElement(element.get());
	}

private:
	void updateView();

	osg::ref_ptr<osg::Material> m_Material;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_Placement;  // TODO remove
	std::vector<osg::ref_ptr<display::Element> > m_FloatingElements;
	osg::Vec3 m_ViewPoint;
	osg::Vec3 m_Origin;
	ForwardFrame m_ForwardFrame;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_HUD_HUD_H__
