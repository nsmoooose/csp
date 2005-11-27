// Combat Simulator Project
// Copyright (C) 2003-2005 The Combat Simulator Project
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
 * @file ControlSurfacesChannels.h
 *
 **/


#ifndef __CSPSIM_CONTROLSURFACES_CHANNELS_H__
#define __CSPSIM_CONTROLSURFACES_CHANNELS_H__

#include <csp/lib/util/Namespace.h>

CSP_NAMESPACE

namespace bus {

struct ControlSurfaces {
	static const char *AileronDeflection;
	static const char *ElevatorDeflection;
	static const char *RudderDeflection;
	static const char *AirbrakeDeflection;
	static const char *LeadingEdgeFlapDeflection;
	static const char *TrailingEdgeFlapDeflection;
};

} // namespace bus

CSP_NAMESPACE_END

#endif // __CSPSIM_CONTROLSURFACES_CHANNELS_H__

