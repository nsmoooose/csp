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


#ifndef __COCKPIT_INTERFACE_H__
#define __COCKPIT_INTERFACE_H__

#include <Bus.h>
#include <InputInterface.h>
#include <SimData/Ref.h>
#include <SimData/Enum.h>
#include <sigc++/sigc++.h>
#include <string>


/** Base class for elements of the cockpit that can receive input events
 *  and control data channels.
 */
class CockpitElement: public simdata::Referenced, public SigC::Object {
friend class CockpitInterface;
public:
	typedef simdata::Ref<CockpitElement> Ref;

protected:
	virtual void registerHandlers(InputInterface*) { }
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
	// if command == "", no input handlers will be registered.
	CockpitSwitch(simdata::Enumeration *enumeration, std::string const &channel, std::string const &command, std::string const &initial);
	CockpitSwitch(std::string const &enumeration, std::string const &channel, std::string const &command, std::string const &initial);
	CockpitSwitch(DataChannel<simdata::EnumLink> *channel, std::string const &command);
	simdata::EnumLink &state() { return m_State->value(); }

protected:
	virtual void registerHandlers(InputInterface*);
	virtual void registerChannels(Bus *bus);

private:
	std::string m_Command;
	DataChannel<simdata::EnumLink>::Ref m_State;

	void onToggle(int, int);
	void onCycleNext(int, int);
	void onCyclePrev(int, int);
	void onSelect(int, int, int state);
};


/** A container class that manages multiple cockpit elements.
 */
class CockpitInterface: public simdata::Referenced {

public:
	virtual void registerChannels(Bus *bus);
	virtual void bindEvents(InputInterface *pInterface);
	virtual void addElement(CockpitElement *element);

private:
	std::vector<CockpitElement::Ref> m_Elements;
};


#endif // __COCKPIT_INTERFACE_H__

