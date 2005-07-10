/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This file is part of SimData.
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

%module External
%{
#include "SimData/External.h"
%}

%rename(__repr__) SIMDATA(External)::asString() const;
%rename(__eq__) SIMDATA(External)::operator==(SIMDATA(External) const &) const;
%rename(__eq__) SIMDATA(External)::operator==(std::string const &) const;
%rename(__ne__) SIMDATA(External)::operator!=(SIMDATA(External) const &) const;
%rename(__ne__) SIMDATA(External)::operator!=(std::string const &) const;

%include "SimData/External.h"

