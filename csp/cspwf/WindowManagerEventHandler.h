// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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

#ifndef __CSPSIM_WF_WINDOWMANAGEREVENTHANDLER_H__
#define __CSPSIM_WF_WINDOWMANAGEREVENTHANDLER_H__

#include <osgGA/GUIEventHandler>

#include <csp/csplib/util/Ref.h>
#include <csp/cspwf/Export.h>

namespace csp {
namespace wf {

class WindowManagerViewer;

/** Default event handler to be used with osgGA and osgViewer::Viewer
 * classes. It will forward all mouse and screen resize events into
 * the window manager specified in the constructor.
 */
class CSPWF_EXPORT WindowManagerEventHandler : public osgGA::GUIEventHandler {
public:
	WindowManagerEventHandler(WindowManagerViewer* windowManager);
	virtual ~WindowManagerEventHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&, osg::Object*, osg::NodeVisitor*);

private:
	Ref<WindowManagerViewer> m_WindowManager;
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_WINDOWMANAGEREVENTHANDLER_H__
