#pragma once
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
#include <osgGA/GUIEventHandler>
#include <csp/cspsim/input/HID.h>

union SDL_Event;

namespace csp {

class SDLEventHandler : public osgGA::GUIEventHandler
{
public:
	SDLEventHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);
	bool handle(const SDL_Event& sdlEvent);

	template <class EventType>
	bool onEvent(const EventType &event)
	{
		bool handled = onCurrentScreenEvent(event);
		if ( !handled )
		{
			handled = onActiveObjectEvent(event);
		}
		return handled;
	}

	void runInputScripts();

protected:
	template <class EventType>
	bool onCurrentScreenEvent(EventType &event)
	{
		Ref<input::HID> currentScreenInterface = getCurrentScreenInterface();
		if ( !currentScreenInterface ) return false;

		return currentScreenInterface->onEvent(event);
	}

	template <class EventType>
	bool onActiveObjectEvent(EventType &event)
	{
		Ref<input::HID> activeObjectInterface = getActiveObjectInterface();
		if ( !activeObjectInterface ) return false;

		return activeObjectInterface->onEvent(event);
	}

	void runCurrentScreenInputScript();
	void runActiveObjectInputScript();

	Ref<input::HID> getCurrentScreenInterface();
	Ref<input::HID> getActiveObjectInterface();

	bool handleMouseMotion(const osgGA::GUIEventAdapter& ea);
	bool handleResize(int width, int height);

	bool m_drag;
	bool m_lastMousePositionUnknown;
	float m_lastMousePositionX;
	float m_lastMousePositionY;
};

}
