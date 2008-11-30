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
 * @file WindowManagerViewer.h
 *
 **/

#ifndef __CSPSIM_WF_WINDOWMANAGERVIEWER_H__
#define __CSPSIM_WF_WINDOWMANAGERVIEWER_H__

#include <csp/cspwf/WindowManager.h>

namespace csp {
namespace wf {

class CSPWF_EXPORT WindowManagerViewer : public WindowManager {
public:
	WindowManagerViewer();
	virtual ~WindowManagerViewer();

	virtual osg::ref_ptr<osg::Group> getRootNode();

	virtual Control* getControlAtPosition(int x, int y);
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_WINDOWMANAGERVIEWER_H__
