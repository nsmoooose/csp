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
 * @file EventMapping.h
 *
 **/


#ifndef __EVENTMAPPING_H__
#define __EVENTMAPPING_H__

#include "SDL_events.h"
#include "SDL_keysym.h"
#include "SDL_keyboard.h"

#include <string>
#include <vector>

#include <SimData/HashUtility.h>


class EventMapping 
{
public:
	EventMapping();

	struct LinearCorrection {
		int low, high;
		int offset;
		double scale;
	};

	struct Axis {
		Axis(std::string const &id_ = ""): id(id_) {}
		std::string id;
	};
	
	struct Action {
		Action(std::string const &id_ = "", 
		       double time_ = 0.0, 
		       int jmod_ = -1, 
		       int mode_ = -1,
		       int loop_ = -1,
		       int stop_ = -1): 
			id(id_), mode(mode_), jmod(jmod_), loop(loop_), stop(stop_), time(time_) {}
		std::string id;
		int mode;
		int jmod;
		int loop;
		int stop;
		double time;
	};

	struct Motion {
		Motion(std::string const &id_ = ""): id(id_) {}
		std::string id;
	};

	typedef std::vector<Action> Script;
	typedef HASH_MAP<int, Script> script_map;
	typedef HASH_MAP<int, Motion> motion_map;
	typedef script_map::const_iterator EventScript;
			
	int getKeyID(int device, SDL_keysym const &key, int state, int mode) const;
	int getJoystickButtonID(int device, int button, int state, int jmod, int mode) const;
	int getMouseButtonID(int device, int button, int state, int kmod, int mode) const;
	int getMouseMotionID(int device, int state, int kmod, int mode) const;

	Script const *getKeyScript(int device, SDL_keysym const &key, int state, int mode) const;
	Script const *getJoystickButtonScript(int device, int button, int state, int jmod, int mode) const;
	Script const *getMouseButtonScript(int device, int button, int state, int kmod, int mode) const;
	Axis const *getJoystickAxis(int device, int axis) const;
	Motion const *getMouseMotion(int device, int state, int kmod, int mode) const;
	
	void addKeyMap(int device, SDL_keysym &key, int state, int mode, Script const &s);
	void addJoystickButtonMap(int device, int button, int state, int jmod, int mode, Script const &s);
	void addJoystickAxisMap(int device, int axis, Axis const &a); 
	void addMouseMotionMap(int device, int state, int kmod, int mode, Motion const &motion);
	void addMouseButtonMap(int device, int button, int state, int kmod, int mode, Script const &s);
	
	// convenience method for testing purposes only
	void addKeyMap(int device, SDLKey vkey, SDLMod kmode, int state, int mode, Action const &action);
	// convenience method for testing purposes only
	void addJoystickButtonMap(int device, int button, int state, int jmod, int mode, Action const &action);

	void parseMap(const char *line, EventMapping::Script &script);
	void parseAction(const char *line, EventMapping::Script &script);
	void parseBinding(const char *line);
	virtual bool load(std::string const &path);

	const std::vector<simdata::hasht> &getBindings() const { return m_Bindings; }

private:
	script_map m_KeyMap;
	script_map m_JoystickButtonMap;	
	script_map m_MouseButtonMap;
	motion_map m_MouseMotionMap;
	
	std::vector<simdata::hasht> m_Bindings;
	
	Axis m_JoystickAxisMap[4][8];
};


#endif // __EVENTMAPPING_H__

