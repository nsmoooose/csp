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

%include "std_vector.i"

#define new_vector(a, b)	\
	%template(v##a) std::vector< b >;

#ifndef __VECTOR_TEMPLATES__
#define __VECTOR_TEMPLATES__
namespace std {
	%template(vector_d) vector<double>;
//	%template(vector_s) vector<Spread>;
//	%template(vector_f) vector<float>;
//	%template(vector_i) vector<int>;
}
#endif // __VECTOR_TEMPLATES__
