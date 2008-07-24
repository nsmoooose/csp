// Combat Simulator Project
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
 * @file InputEvent.cpp
 *
 */

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/InputEvent.h>
#include <csp/cspsim/Config.h>

#include <stdexcept>
#include <iostream>

namespace csp {

const float Handle::m_Sampling = 1.0f/210;

Handle::~Handle() {
}

bool Play::operator()(SDL_Event& event) {
	return SDL_PollEvent(&event) != 0;
}

Save::Save():
	m_of(g_Config.getString("DemoMode", "FileName", "Record.csp", false).c_str(), std::ios_base::binary) {
}

Save::~Save() {
	m_of.close();
}

bool Save::operator()(SDL_Event& event) {
	bool result = SDL_PollEvent(&event) != 0;
	SimTime d2 = CSPSim::theSim->getElapsedTime();
	if (d2 - m_EventTime > m_Sampling) {
		m_EventTime = d2;
		m_of.write(reinterpret_cast<char*>(&event), sizeof(SDL_Event));
		m_of.write(reinterpret_cast<char*>(&m_EventTime), sizeof(SimTime));
	}
	return result;
}

Replay::Replay():
	m_if(g_Config.getString("DemoMode", "FileName", "Record.csp", false).c_str(), std::ios_base::binary) {
	if (!m_if)
		throw std::runtime_error("Replay::Replay(): error opening record file in read mode");
}

Replay::~Replay() {
	m_if.close();
}

bool Replay::operator()(SDL_Event& event) {
	SDL_Event e;
	bool result = SDL_PollEvent(&e) != 0;
	if (e.type != SDL_QUIT && (e.type != SDL_KEYDOWN || e.key.keysym.sym != SDLK_ESCAPE)) {
		SimTime d2 = CSPSim::theSim->getElapsedTime();
		if (d2 - m_EventTime > 2*m_Sampling) {
			m_if.read(reinterpret_cast<char*>(&event), sizeof(SDL_Event));
			m_if.read(reinterpret_cast<char*>(&m_EventTime), sizeof(SimTime));
			result = true;
		}
	} else {
		event = e;
	}
	return result;
}

InputEvent::InputEvent() {
	Mode mode = static_cast<Mode>(g_Config.getInt("DemoMode", "Mode"));
	try {
		switch (mode) {
		case PLAY:
			m_Handle.reset(new Play);
			break;
		case RECORD:
			m_Handle.reset(new Save);
			break;
		case REPLAY:
			m_Handle.reset(new Replay);
			break;
		}
	}
	catch(const std::exception& e) {
		std::cerr << "\nException caught in InputEvent::InputEvent(): " << e.what() << ".\n";
		std::cerr << "Using play handle.\n" << std::endl;
		m_Handle.reset(new Play);
	}
}

} // namespace csp

