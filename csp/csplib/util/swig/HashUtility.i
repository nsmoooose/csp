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

%module HashUtility
%{
#include <csp/csplib/util/HashUtility.h>
%}

%rename(hash_string) csp::newhasht_cstring(const char*);

namespace csp {

%rename(__repr__) HashT::str;

struct HashT {
	HashT();
	HashT(uint32_t x);
	HashT(uint32_t b_, uint32_t a_);
	HashT(const HashT &x);
	bool operator ==(uint32_t x) const;
	bool operator !=(uint32_t x) const;
	bool operator ==(HashT const &x) const;
	bool operator !=(HashT const &x) const;
	std::string str() const;
};

typedef HashT hasht;

HashT newhasht_cstring(const char*);

} // namespace csp

