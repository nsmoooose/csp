/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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

#ifndef __SIMDATA_VECTOR_TEMPLATES__
#define __SIMDATA_VECTOR_TEMPLATES__

#define new_vector(a, b)	\
	%template(simdata_vector_##a) std::vector< b >;

new_vector(double, double);
new_vector(string, std::string);
new_vector(int, int);
new_vector(Real, SIMDATA(Real));
new_vector(Path, SIMDATA(Path));
new_vector(Curve, SIMDATA(Curve));
new_vector(Table, SIMDATA(Table));
new_vector(External, SIMDATA(External));
new_vector(Key, SIMDATA(Key));

#endif // __SIMDATA_VECTOR_TEMPLATES__
