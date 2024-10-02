#pragma once
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
 * @file EventMapping.h
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/HashUtility.h>

#include <csp/cspsim/input/RawEvent.h>

#include <string>
#include <vector>
#include <set>

namespace csp {
namespace input {

class EventMapping: public Referenced
{
public:
	EventMapping();

	typedef Ref<EventMapping> RefT;

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

	class Script: protected std::vector<Action> {
		friend class EventMapping;
	public:
		Script(): idx(0) {}
		Action const *getAction() const { return &((*this)[idx]); }
		bool advance() const {
			idx++;
			if (idx >= int(this->size())) idx = 0;
			return (idx > 0);
		}
		void jump(int i) const {
			if (i >= 0 && i < int(this->size())) idx = i;
		}
		int getIndex() const { return idx; }
	private:
		mutable int idx;
	};

	typedef HashMap<int, Script>::Type script_map;
	typedef HashMap<int, Motion>::Type motion_map;
	typedef HashMap<int, Axis>::Type axis_map;
	typedef script_map::const_iterator EventScript;
	typedef std::set<int> UsedJoysticks;

	int getKeyID(unsigned int key, unsigned int modifierMask, RawEvent::Keyboard::Type type) const;
	int getJoystickButtonID(unsigned int joystick, unsigned int button, RawEvent::JoystickButton::Type type, int jmod, int mode) const;
	int getJoystickAxisID(unsigned int joystick, unsigned int axis) const;
	int getMouseButtonID(unsigned int button, RawEvent::MouseButton::Type type, unsigned int modifierMask, int mode) const;
	int getMouseMotionID(unsigned int buttonMask, unsigned int modifierMask, int mode) const;

	Script const *getKeyScript(unsigned int key, unsigned int modifierMask, RawEvent::Keyboard::Type type) const;
	Script const *getJoystickButtonScript(unsigned int joystick, unsigned int button, RawEvent::JoystickButton::Type type, int jmod, int mode) const;
	Script const *getMouseButtonScript(unsigned int button, RawEvent::MouseButton::Type type, unsigned int modifierMask, int mode) const;
	Axis const *getJoystickAxis(unsigned int joystick, unsigned int axis) const;
	Motion const *getMouseMotion(unsigned int buttonMask, unsigned int modifierMask, int mode) const;
	
	void addKeyMap(unsigned int key, unsigned int modifierMask, RawEvent::Keyboard::Type type, Script const &s);
	void addJoystickButtonMap(unsigned int joystick, unsigned int button, RawEvent::JoystickButton::Type type, int jmod, int mode, Script const &s);
	void addJoystickAxisMap(unsigned int joystick, unsigned int axis, Axis const &a); 
	void addMouseMotionMap(unsigned int buttonMask, unsigned int modifierMask, int mode, Motion const &motion);
	void addMouseButtonMap(unsigned int button, RawEvent::MouseButton::Type type, unsigned int modifierMask, int mode, Script const &s);
	
	void parseMap(const char *line, EventMapping::Script &script);
	void parseAction(const char *line, EventMapping::Script &script);
	void parseBinding(const char *line);
	bool load(std::string const &path);

	const UsedJoysticks &getUsedJoysticks() const { return m_usedJoysticks; }
	const std::vector<hasht> &getBindings() const { return m_Bindings; }

private:
	script_map m_KeyMap;
	script_map m_JoystickButtonMap;	
	axis_map   m_JoystickAxisMap;
	script_map m_MouseButtonMap;
	motion_map m_MouseMotionMap;
	
	UsedJoysticks m_usedJoysticks;
	std::vector<hasht> m_Bindings;
};

} // namespace input
} // namespace csp
