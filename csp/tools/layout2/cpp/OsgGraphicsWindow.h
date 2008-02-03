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


#ifndef __CSP_LAYOUT_OSGGRAPHICSWINDOW_H__
#define __CSP_LAYOUT_OSGGRAPHICSWINDOW_H__

#include <string>
#include <osgViewer/Viewer>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Signal.h>
#include <csp/tools/layout2/cpp/Scene.h>

namespace csp {
namespace layout {

class FeatureGraph;
	
/* This is a graphical window that we can draw 3D into. It is called
from a wxPython window (wx.glcanvas.GLCanvas inherited class). This 
makes it possible to integrate wxPython and open scene graph. */
class OsgGraphicsWindow {
public:
	typedef sigc::signal<void> VoidSignal;

	/* This signal is sent to inform wxPython that the current
	open gl context shall be used.*/
	VoidSignal SetCurrent;
	/* This signal is sent to inform wxPython that is can swap
	buffers to display the last rendered frame.*/
	VoidSignal SwapBuffers;

	OsgGraphicsWindow();
	~OsgGraphicsWindow();
	
	/* Render a new frame. This method is used from the OnIdle
	event in wxPython. */
	virtual void Frame();
	
	/* Change the size of the rendering surface */
	virtual void setSize(int x, int y);

	osg::Vec3 getCameraPosition() const;

	void handleKeyDown(int key);
	void handleKeyUp(int key);
	void handleMouseMotion(int x, int y);
	void handleMouseButtonDown(int x, int y, int button);
	void handleMouseButtonUp(int x, int y, int button);

	std::string getTrackballInformation();
	
	void moveCameraToHome();

	FeatureGraph* graph();

private:
	class Implementation;
	Implementation *m_Implementation;
};

} // namespace layout
} // namespace csp


#endif // __CSP_LAYOUT_SCENEGRAPHICSWINDOW_H__
