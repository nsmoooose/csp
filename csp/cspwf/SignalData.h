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
 * @file SignalData.h
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <csp/cspwf/Export.h>

namespace csp {
namespace wf {

/** Python friendly object to be used as a base class
 *  for event data to a Slot. When Slot::notify is called this class
 *  is used as a parameter to the notify method. This makes it possible
 *  to send data from c++ together with the signal that is emitted.
 */ 
class CSPWF_EXPORT SignalData: public Referenced {
public:
	SignalData();
	virtual ~SignalData();
	
private:
};

} // namespace wf
} // namespace csp
