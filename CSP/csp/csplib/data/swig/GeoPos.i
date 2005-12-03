/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


%module GeoPos
%{
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/GeoPos.h>
%}

CSP_NAMESPACE
%rename(__repr__) LLA::asString() const;
%rename(__repr__) UTM::asString() const;
%rename(__repr__) ECEF::asString() const;
%ignore UTM::operator=(LLA const&);
%ignore UTM::operator=(ECEF const&);
%ignore LLA::operator=(UTM const&);
%ignore LLA::operator=(ECEF const&);
%ignore ECEF::operator=(UTM const&);
%ignore ECEF::operator=(LLA const&);
CSP_NAMESPACE_END

%include "csp/csplib/data/GeoPos.h"
