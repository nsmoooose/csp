#ifndef _GRAPHICSOBJECT_H_
#define _GRAPHICSOBJECT_H_

#include <graphics\StandardGraphics.h>

#include <windows.h>
#include <GL/gl.h>

/** \class GraphicsObject
 * This is the base class for the Object Modeling hierarchy.
 * Each derived class must implement a draw method, to render
 * itself to the current display.
 */

class GraphicsObject
{
public:
	GraphicsObject() {}
	virtual ~GraphicsObject() {}

	virtual void draw(StandardGraphics * display) = 0;
};



#endif