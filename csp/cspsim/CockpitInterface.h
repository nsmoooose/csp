#pragma once
// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file CockpitInterface.h
 *
 **/

#include <csp/cspsim/Bus.h>
#include <csp/cspsim/input/InputInterface.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Enum.h>
#include <sigc++/sigc++.h>
#include <string>

namespace csp {

/** Base class for elements of the cockpit that can receive input events
 *  and control data channels.
 */
class CockpitElement: public Referenced, public sigc::trackable {
friend class CockpitInterface;
public:
	typedef Ref<CockpitElement> RefT;

protected:
	virtual void registerHandlers(input::InputInterface*) { }
	virtual void registerChannels(Bus*) { }
};


/** A multistate switch that wraps an enumerated data channel.  Automatically defines
 *  event handlers for setting the switch state and cycling forward and backward
 *  through the states.  Handles the following input events, where {COMMAND} is
 *  specified by the command parameter passed to the constructor:
 *
 *  {COMMAND}_{STATE} select a specific state
 *  {COMMAND}_CYCLE_NEXT cycle to the next state, defined only for enums with more than 2 states.
 *  {COMMAND}_CYCLE_PREV cycle to the previous state, defined only for enums with more than 2 states.
 *  {COMMAND}_TOGGLE toggle to the state, defined only for enums with 2 states.
 *
 *  Here {STATE} is the set of enum tokens, converted to upper case (i.e., one event per state).
 */
class CockpitSwitch: public CockpitElement {
public:
	typedef Ref<CockpitSwitch> RefT;
	// if command == "", no input handlers will be registered.
	CockpitSwitch(Enumeration const *enumeration, std::string const &channel, std::string const &command, std::string const &initial);
	CockpitSwitch(std::string const &enumeration, std::string const &channel, std::string const &command, std::string const &initial);
	CockpitSwitch(DataChannel<EnumLink> *channel, std::string const &command);
	EnumLink &state() { return m_State->value(); }

protected:
	virtual void registerHandlers(input::InputInterface*);
	virtual void registerChannels(Bus *bus);

private:
	std::string m_Command;
	DataChannel<EnumLink>::RefT m_State;

	void onToggle();
	void onCycleNext();
	void onCyclePrev();
	void onSelect(int state);
};


/** A container class that manages multiple cockpit elements.
 */
class CockpitInterface: public Referenced {

public:
	virtual void registerChannels(Bus *bus);
	virtual void bindEvents(input::InputInterface *pInterface);
	virtual CockpitElement* addElement(CockpitElement *element);

private:
	std::vector<CockpitElement::RefT> m_Elements;
};

} // namespace csp
