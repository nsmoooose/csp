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
 * @file Slot.h
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <csp/cspwf/Export.h>

namespace csp {
namespace wf {

class SignalData;

/** A python friendly slot implementation. By inheriting this class in python
 *  and override the notify() method you can receive signals sent from C++
 */
class CSPWF_EXPORT Slot : public Referenced {
public:
	Slot();
	virtual ~Slot();
	
	// This method is called when a signal is emitted to all connected slots.
	virtual void notify(SignalData* event);
	
private:
};

} // namespace wf
} // namespace csp
