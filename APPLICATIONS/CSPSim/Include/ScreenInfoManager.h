// Combat Simulator Project - FlightSim Demo
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

#ifndef __SCREENINFOMANAGER_H__
#define __SCREENINFOMANAGER_H__

#include <osg/Projection>

#include "ScreenInfo.h"

class ScreenInfoManager:public osg::Projection
{
public:
	ScreenInfoManager(int ScreenWidth, int ScreenHeight);
	virtual ~ScreenInfoManager(){}
	void setStatus(std::string const & name, bool bvisible);
	bool ScreenInfoManager::getStatus(std::string const & name);
	void changeObjectStats(int ScreenWidth, int ScreenHeight,simdata::Pointer<DynamicObject> const& activeObject);
private:
	osg::MatrixTransform* m_modelview_abs;
	ScreenInfo* getScreenInfo(std::string const & name);
};

#endif // __SCREENINFOMANAGER_H__
