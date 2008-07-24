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
 * @file CockpitInterface.cpp
 *
 **/


#include <csp/cspsim/CockpitInterface.h>
#include <csp/csplib/util/StringTools.h>

namespace csp {

CockpitSwitch::CockpitSwitch(Enumeration const *states, std::string const &channel, std::string const &command, std::string const &initial): m_Command(command) {
	assert(states);
	EnumLink initial_value(*states, initial);
	m_State = DataChannel<EnumLink>::newSharedPush(channel, initial_value);
}

CockpitSwitch::CockpitSwitch(std::string const &states, std::string const &channel, std::string const &command, std::string const &initial): m_Command(command) {
	assert(!states.empty());
	Enumeration enumeration(states);
	EnumLink initial_value(enumeration, initial);
	m_State = DataChannel<EnumLink>::newSharedPush(channel, initial_value);
}

CockpitSwitch::CockpitSwitch(DataChannel<EnumLink> *channel, std::string const &command): m_Command(command), m_State(channel) {
}

void CockpitSwitch::registerHandlers(InputInterface *input) {
	assert(input);
	if (m_Command.empty()) return;
	const Enumeration &enumeration = m_State->value().getEnumeration();
	if (enumeration.size() == 2) {
		input->bindActionEvent(m_Command + "_TOGGLE", sigc::mem_fun(*this, &CockpitSwitch::onToggle));
	} else {
		input->bindActionEvent(m_Command + "_CYCLE_NEXT", sigc::mem_fun(*this, &CockpitSwitch::onCycleNext));
		input->bindActionEvent(m_Command + "_CYCLE_PREV", sigc::mem_fun(*this, &CockpitSwitch::onCyclePrev));
	}
	for (int i = 0; i < enumeration.size(); ++i) {
		std::string command = m_Command + "_" + enumeration[i].getToken();
		ConvertStringToUpper(command);
		input->bindActionEvent(command, sigc::bind(sigc::mem_fun(*this, &CockpitSwitch::onSelect), i));
	}
}

void CockpitSwitch::registerChannels(Bus *bus) {
	bus->registerChannel(m_State.get());
}

void CockpitSwitch::onToggle() {
	m_State->value().cycle();
	m_State->push();
}

void CockpitSwitch::onCycleNext() {
	m_State->value().cycle();
	m_State->push();
}

void CockpitSwitch::onCyclePrev() {
	m_State->value().cycleBack();
	m_State->push();
}

void CockpitSwitch::onSelect(int state) {
	m_State->value().set(state);
	m_State->push();
}

void CockpitInterface::bindEvents(InputInterface *input) {
	assert(input);
	for (unsigned i = 0; i < m_Elements.size(); ++i) {
		m_Elements[i]->registerHandlers(input);
	}
}

CockpitElement* CockpitInterface::addElement(CockpitElement *element) {
	m_Elements.push_back(element);
	return element;
}

void CockpitInterface::registerChannels(Bus *bus) {
	for (unsigned i = 0; i < m_Elements.size(); ++i) {
		m_Elements[i]->registerChannels(bus);
	}
}


} // namespace csp

