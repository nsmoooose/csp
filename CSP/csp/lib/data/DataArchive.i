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

%module DataArchive
%{
#include <csp/lib/data/Object.h>
#include <csp/lib/data/DataArchive.h>
%}


%import "csp/lib/util/Exception.i"

namespace std {
    %template(vector_hasht) vector<CSP(hasht)>;
}


%import "csp/lib/data/Object.i"

%typemap(out) FP {
    $result = PyFile_FromFile($1.f, const_cast<char*>($1.name.c_str()),
                    const_cast<char*>($1.mode.c_str()), 0);
}

%include "csp/lib/data/DataArchive.h"

