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

#ifndef __CSPSIM_SDLGRAPHICSWINDOW_H__
#define __CSPSIM_SDLGRAPHICSWINDOW_H__

#include <osgViewer/GraphicsWindow>

namespace csp {

struct ScreenSettings;

class SDLGraphicsWindow : public osgViewer::GraphicsWindowEmbedded
{
public:
	SDLGraphicsWindow(const char *caption, const ::csp::ScreenSettings & screenSettings);
	~SDLGraphicsWindow();

	virtual bool isSameKindAs(const Object* object) const { return dynamic_cast<const SDLGraphicsWindow*>(object)!=0; }
	virtual const char* libraryName() const { return "csp"; }
	virtual const char* className() const { return "SDLGraphicsWindow"; }

	virtual bool valid() const { return m_valid; }
	virtual void swapBuffersImplementation();

private:
	bool m_valid;
};

}
#endif // __CSPSIM_SDLGRAPHICSWINDOW_H__
