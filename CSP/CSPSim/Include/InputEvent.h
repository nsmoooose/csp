// Combat Simulator Project - CSPSim
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file InputEvent.h
 *
 * 
 */

#ifndef __INPUTEVENT_H__
#define __INPUTEVENT_H__

#include <fstream>
#include <memory>

#include <SDL/SDL_events.h>

#include <SimData/Date.h>

class Handle {
protected:
	simdata::SimTime m_EventTime;
	static const float m_Sampling;
public:
	Handle():
	  m_EventTime(0.0f) {
	 }
	virtual bool operator()(SDL_Event& event) = 0;
	virtual ~Handle();
};

class Play: public Handle {
public:
	virtual bool operator()(SDL_Event& event);
};

class Save: public Handle {
	std::ofstream m_of;
public:
	Save();
	~Save();
	virtual bool operator()(SDL_Event& event);
};

class Replay: public Handle {
	std::ifstream m_if;
public:
	Replay();
	~Replay();
	virtual bool operator()(SDL_Event& event);
};

class InputEvent {
	enum Mode {PLAY,REPLAY,RECORD};
	std::auto_ptr<Handle> m_Handle;
public:
	InputEvent();
	bool operator()(SDL_Event& event) const {
		return (*m_Handle)(event);
	}
};

#endif

