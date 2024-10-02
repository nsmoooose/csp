#pragma once
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
 * @file DisplayTools.h
 *
 **/

#include <csp/csplib/util/Uniform.h>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

namespace osg { class Geode; }
namespace osg { class Geometry; }

namespace csp {
namespace hud {
namespace display {

/** A tool for drawing symbols.  Can be used either as a symbol factory or as a dynamic
 *  symbol.  To use as a factory, draw a symbol and then add it to a geode.  Once added
 *  to a geode the SymbolMaker can be discarded.  To use as a dynamic symbol, add to a
 *  geode but retain the SymbolMaker.  Call erase() to clear the symbol and the draw a
 *  new symbol.  The symbol can be updated in this way as needed without directly changing
 *  the containing geode.
 *
 *  All drawing commands use meters and coordinates are relative to the origin of the symbol.
 */
class SymbolMaker {
	osg::ref_ptr<osg::Vec3Array> m_Vertices;
	osg::ref_ptr<osg::Vec4Array> m_Colors;
	osg::ref_ptr<osg::Geometry> m_Geometry;
	bool m_DrawMode;
	unsigned m_DrawStart;
	unsigned m_DrawCount;
	float m_DrawPointX;
	float m_DrawPointY;
public:
	typedef osg::Vec4 Color;
	static const Color Pen;

	SymbolMaker();
	~SymbolMaker();
	// Add a circle (actually a polygon with n_sides) to the symbol.
	void addCircle(float x, float y, float radius, int n_sides, Color const &color=Pen);

	// Construct a completely new symbol.  This method allows a symbol maker to
	// be reused as a factory.
	void reset();

	// Erase the symbol but remain connected to the containing geode.  Allows a symbol
	// to be updated dynamically
	void erase();

	// Add a rectangle from (x0, y0) to (x1, y1) as a distinct primitive.
	void addRectangle(float x0, float y0, float x1, float y1, bool filled=false, Color const &color=Pen);

	// Add a line as a distinct primitive.
	void addLine(float x0, float y0, float x1, float y1, Color const &color=Pen);

	// Draw a line; see beginDrawLines.
	void drawLine(float x0, float y0, float x1, float y1, Color const &color=Pen);

	// Prepare to draw a line using lineTo; see beginDrawLines.
	void moveTo(float x, float y);

	// Draw a line following moveTo or extend a line following lineTo; see
	// beginDrawLines.
	void lineTo(float x, float y, Color const &color=Pen);

	// Set the default pen color.
	void setPenColor(Color const &color);

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

private:
	// CAUTION: must be called before adding new vertices.
	void addColorValues(Color const &color, unsigned n);
	Color m_Color;
	bool m_NoColor;
};


/** A base class for elements (symbols and text) that can be added to the HUD.
 *  Although not abstract, this base class is not very useful since it provides
 *  no way to position itself in the HUD.
 */
class Element: public osg::Referenced {
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

	/** Set the line width for symbols.  The linewidth is measured in pixels and
	 *  is currently independent of the viewing angle.  The default linewidth is
	 *  typically one pixel unless set by a parent node.
	 */
	virtual void setLineWidth(double width);

	/** Set the line stipple pattern for symbols.  Each bit in pattern represents
	 *  scale pixels in the rendered line.  Set bits show the corresponding line
	 *  segment while unset bits mask line segments.  The stipple spacing is
	 *  independent of the viewing angle.
	 */
	virtual void setLineStipple(uint8 scale, uint16 pattern);

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
class MoveableElement: public Element {
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
class DirectionElement: public MoveableElement {
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


/** A subclass of osgText::Text for use with HUD elements.  Binds itself
 *  directly to an element's Geode.  An alternative (particularly for
 *  static text) is to just use the addText method of Element.
 */
class ElementText: public osgText::Text {
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
};


/** A text-only element that manages an osgText::Text instance.
 */
class LabelElement: public MoveableElement {
	osgText::Text* m_Text;
public:
	LabelElement();
	osgText::Text *text() { return m_Text; }
};

/** An abstract element container that provides a local coordinate system.
 */
class Frame {
	virtual void _addElement(Element *element) = 0;
public:
	virtual ~Frame() { }
	void addElement(Element *element) { _addElement(element); }
	template <class T> void addElement(osg::ref_ptr<T> &element) { _addElement(element.get()); }
};

/** A frame for containing other elements that is itself a direction element.
 *  The allows a group of elements to be placed along a direction vector.
 */
class FloatingFrame: public DirectionElement, public Frame {
	virtual void _addElement(Element *element);
};

} // end namespace display
} // end namespace hud
} // end namespace csp
