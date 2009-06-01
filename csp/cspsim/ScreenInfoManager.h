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
 * @file ScreenInfoManager.h
 *
 **/

#ifndef __CSPSIM_SCREENINFOMANAGER_H__
#define __CSPSIM_SCREENINFOMANAGER_H__

#include <csp/cspsim/ScreenInfo.h>

#include <osg/Group>

namespace csp {

class ScreenInfoManager: public osg::Group {
public:
	ScreenInfoManager(int screen_width, int screen_height);
	virtual ~ScreenInfoManager(){}
	void setStatus(std::string const &name, bool visible);
	bool getStatus(std::string const &name);
	void changeObjectStats(int screen_width, int screen_height, Ref<DynamicObject> const& active_object);
	void addMessage(std::string const &line);
private:
	osg::Group* m_camera;
	ScreenInfo* getScreenInfo(std::string const &name);
};

} // namespace csp

#endif // __CSPSIM_SCREENINFOMANAGER_H__
