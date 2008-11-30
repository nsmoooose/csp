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


/**
 * @file WindowManagerSceneView.h
 *
 **/

#include <csp/cspwf/WindowManager.h>

#ifndef __CSPSIM_WF_WINDOWMANAGERSCENEVIEW_H__
#define __CSPSIM_WF_WINDOWMANAGERSCENEVIEW_H__

namespace csp {
namespace wf {

class CSPWF_EXPORT WindowManagerSceneView : public WindowManager {
public:
	WindowManagerSceneView(osg::State* state, int screenWidth, int screenHeight);
	virtual ~WindowManagerSceneView();

	virtual osgUtil::SceneView* getSceneView();

	virtual Control* getControlAtPosition(int x, int y);

	virtual void onUpdate(float dt);
	virtual void onRender();

private:
	osg::ref_ptr<osgUtil::SceneView> m_View;

};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_WINDOWMANAGERSCENEVIEW_H__
