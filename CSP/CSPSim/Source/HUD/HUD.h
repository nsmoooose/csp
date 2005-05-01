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


#ifndef __HUD_H__
#define __HUD_H__


#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

#if !defined(__GNUC__) && !defined(snprintf)
#define snprintf _snprintf
#endif

namespace osg { class ClipNode; }
namespace osg { class Geode; }
namespace osg { class Geometry; }


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
class HUD {

public:
	// HUD elements and helper classes.
	class SymbolMaker;
	class Element;
	class MoveableElement;
	class DirectionElement;
	class FloatingFrame;
	class ElementText;
	class LabelElement;

	// A frame is an abstract element container that provides a local coordinate system.
	class Frame {
		virtual void _addElement(Element *element) = 0;
	public:
		virtual ~Frame() { }
		void addElement(Element *element) { _addElement(element); }
		template <class T> void addElement(osg::ref_ptr<T> &element) { _addElement(element.get()); }
	};

protected:
	// Forward frame is a special frame that is used internally by the HUD to define
	// the primary 2D coordinate system for non-direction elements.   The origin of
	// the frame is aligned with the forward vector (y-axis in body coordinates) relative
	// to the pilot's point of view.  Thus if the pilot's head moves off center, the
	// forward frame will be similarly displaced relative to the HUD glass (as though
	// the elements in the frame were positioned at infinity).
	class ForwardFrame: public Frame {
		osg::ref_ptr<osg::PositionAttitudeTransform> m_Transform;
		virtual void _addElement(Element *element);
	public:
		osg::PositionAttitudeTransform *transform() { return m_Transform.get(); }
		osg::PositionAttitudeTransform const *transform() const { return m_Transform.get(); }
		ForwardFrame(): m_Transform(new osg::PositionAttitudeTransform) { }
	};

public:
	HUD();
	~HUD();

	/** Set the dimension (width, height) of the HUD display in meters.  Elements outside
	 *  of these bounds will be clipped.
	 */
	void setDimensions(float x, float y);

	/** Update the view point.  This method is called by SceneModel when the HUD is first
	 *  added to the scene.  It should also be called whenever the pilot's head moves.
	 */
	void setViewPoint(osg::Vec3 const &view_point);

	/** Set the origin of the HUD in body coordinates.  This position is managed through
	 *  ObjectModel.
	 */
	void setOrigin(osg::Vec3 const &origin);

	/** Get the scene graph node of the HUD.
	 */
	osg::PositionAttitudeTransform *hud();

	/** Get the forward frame.
	 */
	Frame *forwardFrame();

	/** Get the origin of the forward frame relative to the HUD origin in body coordinates.
	 */
	osg::Vec3 getForwardFrameOrigin() const;

	/** Add an element to the forward frame.  Do not add floating elements with this method
	 *  since will not be updated correctly if the pilot's viewpoint changes; instead use
	 *  addFloatingElement.
	 */
	void addFrameElement(Element *element);

	/** Convenience method for adding a frame element by reference.
	 */
	template <class T>
	void addFrameElement(osg::ref_ptr<T> &element) {
		addFrameElement(element.get());
	}

	/** Add a floating element which will be positioned based on a direction relative to
	 *  the pilot's point of view.
	 */
	void addFloatingElement(Element *element);

	/** Convenience method for adding a floading element by reference.
	 */
	template <class T>
	void addFloatingElement(osg::ref_ptr<T> &element) {
		addFloatingElement(element.get());
	}

private:
	void updateView();

	osg::ref_ptr<osg::ClipNode> m_ClipNode;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_Placement;
	std::vector<osg::ref_ptr<Element> > m_FloatingElements;
	osg::Vec3 m_ViewPoint;
	osg::Vec3 m_Origin;
	ForwardFrame m_ForwardFrame;
};


/** A tool for drawing symbols.  Can be used either as a symbol factory or as a dynamic
 *  symbol.  To use as a factory, draw a symbol and then add it to a geode.  Once added
 *  to a geode the SymbolMaker can be discarded.  To use as a dynamic symbol, add to a
 *  geode but retain the SymbolMaker.  Call erase() to clear the symbol and the draw a
 *  new symbol.  The symbol can be updated in this way as needed without directly changing
 *  the containing geode.
 *
 *  All drawing commands use meters and coordinates are relative to the origin of the symbol.
 */
class HUD::SymbolMaker {
	osg::ref_ptr<osg::Vec3Array> m_Vertices;
	osg::ref_ptr<osg::Geometry> m_Geometry;
	bool m_DrawMode;
	unsigned m_DrawStart;
	unsigned m_DrawCount;
	float m_DrawPointX;
	float m_DrawPointY;
public:
	SymbolMaker();
	~SymbolMaker();
	// Add a circle (actually a polygon with n_sides) to the symbol.
	void addCircle(float x, float y, float radius, int n_sides);

	// Construct a completely new symbol.  This method allows a symbol maker to
	// be reused as a factory.
	void reset();

	// Erase the symbol but remain connected to the containing geode.  Allows a symbol
	// to be updated dynamically
	void erase();

	// Add a rectangle from (x0, y0) to (x1, y1) as a distinct primitive.
	void addRectangle(float x0, float y0, float x1, float y1);

	// Add a line as a distinct primitive.
	void addLine(float x0, float y0, float x1, float y1);

	// Draw a line; see beginDrawLines.
	void drawLine(float x0, float y0, float x1, float y1);

	// Prepare to draw a line using lineTo; see beginDrawLines.
	void moveTo(float x, float y);

	// Draw a line following moveTo or extend a line following lineTo; see
	// beginDrawLines.
	void lineTo(float x, float y);

	// Enables line drawing mode for drawing a set of lines that will be part
	// of a single primitive.  This is slightly more efficient (esp. for a large
	// number of lines) that using individual calls to addLine.  The drawLine,
	// moveTo, and lineTo commands can be used to extend the primitive until
	// endDrawLines is called to exit drawing mode.  These commands cannot be
	// used outside of drawing mode.  Conversely all other commands (e.g.,
	// addRectangle) cannot be used in drawing mode.
	void beginDrawLines();

	// Finalize a primitive constructed in drawing mode; see beginDrawLines.
	void endDrawLines();

	// Add the symbol to a geode for display in the scene graph.
	void addToGeode(osg::Geode *geode);
};


/** A base class for elements (symbols and text) that can be added to the HUD.
 *  Although not abstract, this base class is not very useful since it provides
 *  no way to position itself in the HUD.
 */
class HUD::Element: public osg::Referenced {
	osg::ref_ptr<osg::Geode> m_Element;
public:
	Element();
	virtual ~Element();
	osg::Geode *geode() { return m_Element.get(); }
	virtual osg::Node *node();

	/** Called by the enclosing frame to update the position of the element
	 *  when the pilot's point of view changes.
	 */
	virtual void updateView(osg::Vec3 const &, osg::Vec3 const &) { }

	/** Remove all drawables from the element's geode.
	 */
	virtual void clear();

	/** Add a symbol from a symbol maker to this element.  The origin of the
	 *  symbol is placed at the origin of the element.
	 */
	virtual void addSymbol(SymbolMaker &symbol) { symbol.addToGeode(geode()); }

	/** Add a new text item to the geode and return a pointer to it.
	 */
	virtual osgText::Text *addText();

	/** Show or hide the element.  This method if overridden in a base class must
	 *  remain extremely cheap to execute.
	 */
	virtual void show(bool visible) { node()->setNodeMask(visible ? ~0 : 0); }

	/** Test if the element is hidden (or visible).  This method if overridden in
	 *  a base class must remain extremely cheap to execute.
	 */
	virtual bool isHidden() { return node()->getNodeMask() == 0; }
};


/** An Element class that can be positioned in a frame using x,y coordinates
 *  and rotated.
 */
class HUD::MoveableElement: public HUD::Element {
	osg::ref_ptr<osg::PositionAttitudeTransform> m_Transform;
public:
	MoveableElement();
	virtual osg::Vec3 position() const;
	/** Set the position of the element in the coordinates of the enclosing frame.
	 */
	virtual void setPosition(float x, float y);

	/** Set the rotation angle (positive is clockwise).  TODO check that!
	 */
	virtual void setOrientation(float angle);

	virtual osg::Node *node() { return m_Transform.get(); }
};


/** An Element class that can be rotated and positioned along a specified
 *  direction relative to the pilot's point of view.  This is useful for
 *  constructing floating elements like flight path markers and pitch ladders.
 */
class HUD::DirectionElement: public HUD::MoveableElement {
	osg::Vec3 m_Direction;
	osg::Vec3 m_ViewPoint;
	osg::Vec3 m_Origin;
public:
	/** Set the dArection of the element relative to the point of view in body
	 *  coordinates.
	 */
	virtual void setDirection(osg::Vec3 const &direction);

	/** An internal method called by the enclosing frame to update the position of
	 *  the element when the point of view changes.
	 */
	virtual void updateView(osg::Vec3 const &origin, osg::Vec3 const &view_point);
};


/** A frame for containing other elements that is itself a direction element.
 *  The allows a group of elements to be placed along a direction vector.
 */
class HUD::FloatingFrame: public HUD::DirectionElement, public HUD::Frame {
	virtual void _addElement(Element *element);
};


/** A subclass of osgText::Text for use with HUD elements.  Its main utility
 *  is in providing printf like functionality for setting HUD text.  Binds
 *  itself directly to an element's Geode.  An alternative (particularly for
 *  static text) is to just use the addText method of Element.
 */
class HUD::ElementText: public osgText::Text {
	Element *m_Element;
public:
	/** Construct an ElementText and add it to the specified element.
	 */
	ElementText(Element *element);

	// Warning: element() will be invalid if the parent element has been destroyed.
	// We don't keep an internal reference in order to prevent a reference cycle.
	// If the element has been added to the HUD, this pointer should be valid as
	// long as the HUD isn't destroyed.
	Element *element() { return m_Element; }

	/** Set the position of the text within the element.
	 */
	void setPosition(float x, float y);

	/** Update the text using printf-style formatting (one-arg only).
	 */
	template <typename A>
	void print(const char *format, A const &a) {
		char label[80];
		snprintf(label, sizeof(label), format, a);
		setText(label);
	}

	/** Update the text using printf-style formatting (two-args only).
	 */
	template <typename A, typename B>
	void print(const char *format, A const &a, B const &b) {
		char label[80];
		snprintf(label, sizeof(label), format, a, b);
		setText(label);
	}
};


/** A text-only element that manages an osgText::Text instance.
 */
class HUD::LabelElement: public HUD::MoveableElement {
	osgText::Text* m_Text;
public:
	LabelElement();
	osgText::Text *text() { return m_Text; }
};


#endif // __HUD_H__
