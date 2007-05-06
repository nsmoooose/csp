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
 * @file Signal.h
 *
 **/

#ifndef __CSPSIM_WF_SIGNAL_H__
#define __CSPSIM_WF_SIGNAL_H__

#include <set>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>
#include <csp/cspsim/wf/Slot.h>

CSP_NAMESPACE

namespace wf {

class SignalData;
class Slot;

/** This is a python friendly class to use when it is needed to connect 
 *  signals from C++ to python. 
 */
class CSPSIM_EXPORT Signal : public Referenced {
public:
	Signal();
	virtual ~Signal();
	
	// Connects a slot to this signal. When the emit method is called the 
	// Slot::notify method is called. To receive the signal you just needs
	// to override the nofify method.
	virtual void connect(Slot* slot);
	
	// Disconnects a slot from this signal. No signal is sent to the slot after 
	// this call.
	virtual void disconnect(Slot* slot);
	
	// Emits the signal to all connected slots. The Slot::notify method is called
	// with the SignalData class as a parameter.
	virtual void emit(SignalData* event);
	
	// Disposes this object by disconnecting all slots.
	virtual void dispose();
	
private:
	typedef std::vector<Ref<Slot> > SlotVector;
	SlotVector m_InvocationList;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_SIGNAL_H__
