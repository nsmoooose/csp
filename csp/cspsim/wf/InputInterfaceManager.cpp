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
 * @file InputInterfaceManager.cpp
 *
 **/

#include <csp/cspsim/InputInterface.h>
#include <csp/cspsim/wf/SignalData.h>
#include <csp/cspsim/wf/InputInterfaceManager.h>

CSP_NAMESPACE

namespace wf {

InputInterfaceManager::InputInterfaceManager(InputInterface* inputInterface) : 
	m_InputInterface(inputInterface) {
}

InputInterfaceManager::~InputInterfaceManager() {
}

Signal* InputInterfaceManager::getActionSignal(const std::string& id) {
	ActionSignalMap::iterator signal = m_ActionSignals.find(id);
	if(signal != m_ActionSignals.end()) {
		return signal->second->getSignal();
	}
	
	return NULL;
}

Signal* InputInterfaceManager::registerActionSignal(const std::string& id) {
	// If there already is a action signal we return it.
	Signal* signal = getActionSignal(id);
	if(signal != NULL) {
		return signal;
	}
	
	// Otherwise we must insert a new one into our action map.
	Ref<ActionToSignalSlot> actionToSignalSlot = new ActionToSignalSlot(id, m_InputInterface);
	m_ActionSignals[id] = actionToSignalSlot;
	return actionToSignalSlot->getSignal();
}

InputInterfaceManager::ActionToSignalSlot::ActionToSignalSlot(const std::string& id, InputInterface* inputInterface) {
	m_Signal = new Signal;
	inputInterface->bindActionEvent(id, sigc::mem_fun(*this, &InputInterfaceManager::ActionToSignalSlot::notify));
}

Signal* InputInterfaceManager::ActionToSignalSlot::getSignal() {
	return m_Signal.get();
}

void InputInterfaceManager::ActionToSignalSlot::notify() {
	Ref<SignalData> data = new SignalData;
	m_Signal->emit(data.get());
}

} // namespace wf

CSP_NAMESPACE_END
