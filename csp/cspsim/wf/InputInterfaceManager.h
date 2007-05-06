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
 * @file InputInterfaceManager.h
 *
 **/

#ifndef __CSPSIM_WF_INPUTINTERFACEMANAGER_H__
#define __CSPSIM_WF_INPUTINTERFACEMANAGER_H__

#include <map>
#include <string>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>
#include <csp/cspsim/wf/Signal.h>

CSP_NAMESPACE

class InputInterface;

namespace wf {

class Signal;
class Slot;

class CSPSIM_EXPORT InputInterfaceManager: public Referenced {
public:
	InputInterfaceManager(InputInterface* inputInterface);
	virtual ~InputInterfaceManager();
	
	virtual Signal* getActionSignal(const std::string& id);
	virtual Signal* registerActionSignal(const std::string& id);
	
private:
	class ActionToSignalSlot : public Referenced {
	public:	
		ActionToSignalSlot(const std::string& id, InputInterface* inputInterface);
		Signal* getSignal();
		
	private:
		Ref<Signal> m_Signal;
		void notify();
	};

	InputInterface* m_InputInterface;
	
	typedef std::map<std::string, Ref<ActionToSignalSlot> > ActionSignalMap;
	ActionSignalMap m_ActionSignals;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_INPUTINTERFACEMANAGER_H__
