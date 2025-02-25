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

%module Vector3
%include "attribute.i"

%attribute(Vector3, double, x, get_x, set_x);
%attribute(Vector3, double, y, get_y, set_y);
%attribute(Vector3, double, z, get_z, set_z);

%{
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Matrix3.h>
%}

%rename(__repr__) csp::Vector3::asString() const;

%include <csp/csplib/data/Vector3.h>
