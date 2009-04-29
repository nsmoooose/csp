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
 * @file InputInterfaceWfAdapter.h
 *
 **/

#ifndef __CSPSIM_INPUTINTERFACEWFADAPTER_H__
#define __CSPSIM_INPUTINTERFACEWFADAPTER_H__

#include <map>
#include <string>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>
#include <csp/cspwf/Signal.h>

namespace csp {

namespace wf {
	class Signal;
	class Slot;
}

namespace input {

class InputInterface;

/// An adapter class for binding InputInterface action events to window
/// framework signals.
class CSPSIM_EXPORT InputInterfaceWfAdapter: public Referenced {
public:
	/// The input interface to which to bind signals.  The input interface
	/// instance must outlive the InputInterfaceWfAdapter.
	explicit InputInterfaceWfAdapter(InputInterface* inputInterface);
	virtual ~InputInterfaceWfAdapter();

	/// Get the signal associated with the specified action id, or NULL if no
	/// signal has been registered via registerActionSignal.
	virtual wf::Signal* getActionSignal(const std::string& id);

	/// Get or create a signal associated with the specified action id.  The
	/// signal will fire whenever the action event occurs.  The signal instance
	/// lifetime is the same as the InputInterfaceWfAdapter instance that created
	/// it.
	virtual wf::Signal* registerActionSignal(const std::string& id);

private:
	class ActionToSignalSlot;
	typedef std::map<std::string, Ref<ActionToSignalSlot> > ActionSignalMap;

	ActionSignalMap* m_ActionSignals;
	InputInterface* m_InputInterface;  // not owned
};

} // namespace input
} // namespace csp

#endif // __CSPSIM_INPUTINTERFACEWFADAPTER_H__
