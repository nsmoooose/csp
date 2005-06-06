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


#include <CockpitInterface.h>


CockpitSwitch::CockpitSwitch(simdata::Enumeration *states, std::string const &channel, std::string const &command, std::string const &initial): m_Command(command) {
	assert(states);
	simdata::EnumLink initial_value(*states, initial);
	m_State = DataChannel<simdata::EnumLink>::newShared(channel, initial_value);
}

CockpitSwitch::CockpitSwitch(std::string const &states, std::string const &channel, std::string const &command, std::string const &initial): m_Command(command) {
	assert(!states.empty());
	simdata::Enumeration enumeration(states);
	simdata::EnumLink initial_value(enumeration, initial);
	m_State = DataChannel<simdata::EnumLink>::newShared(channel, initial_value);
}

CockpitSwitch::CockpitSwitch(DataChannel<simdata::EnumLink> *channel, std::string const &command): m_Command(command), m_State(channel) {
}

void CockpitSwitch::registerHandlers(InputInterface *input) {
	assert(input);
	if (m_Command.empty()) return;
	const simdata::Enumeration &enumeration = m_State->value().getEnumeration();
	if (enumeration.size() == 2) {
		input->bindActionEvent(m_Command + "_TOGGLE", SigC::slot(*this, &CockpitSwitch::onToggle));
	} else {
		input->bindActionEvent(m_Command + "_CYCLE_NEXT", SigC::slot(*this, &CockpitSwitch::onCycleNext));
		input->bindActionEvent(m_Command + "_CYCLE_PREV", SigC::slot(*this, &CockpitSwitch::onCyclePrev));
	}
	for (int i = 0; i < enumeration.size(); ++i) {
		std::string command = m_Command + "_" + enumeration[i].getToken();
		ConvertStringToUpper(command);
		input->bindActionEvent(command, SigC::bind(SigC::slot(*this, &CockpitSwitch::onSelect), i));
	}
}

void CockpitSwitch::registerChannels(Bus *bus) {
	bus->registerChannel(m_State.get());
}

void CockpitSwitch::onToggle(int, int) {
	std::cout << m_Command << " toggle\n";
	m_State->value().cycle();
}

void CockpitSwitch::onCycleNext(int, int) {
	std::cout << m_Command << " next\n";
	m_State->value().cycle();
}

void CockpitSwitch::onCyclePrev(int, int) {
	std::cout << m_Command << " prev\n";
	m_State->value().cycleBack();
}

void CockpitSwitch::onSelect(int, int, int state) {
	m_State->value().set(state);
	std::cout << m_Command << " select " << m_State->value().getToken() << "\n";
}

void CockpitInterface::bindEvents(InputInterface *input) {
	assert(input);
	for (unsigned i = 0; i < m_Elements.size(); ++i) {
		m_Elements[i]->registerHandlers(input);
	}
}

void CockpitInterface::addElement(CockpitElement *element) {
	m_Elements.push_back(element);
}

void CockpitInterface::registerChannels(Bus *bus) {
	for (unsigned i = 0; i < m_Elements.size(); ++i) {
		m_Elements[i]->registerChannels(bus);
	}
}


