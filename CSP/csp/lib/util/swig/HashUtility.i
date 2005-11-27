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
#include <csp/lib/util/HashUtility.h>
%}

#include "csp/lib/util/Namespace.h"

%rename(hash_string) CSP(newhasht_cstring)(const char*);

CSP_NAMESPACE

%rename(__repr__) HashT::str;

struct HashT {
	HashT();
	HashT(uint32 x);
	HashT(uint32 b_, uint32 a_);
	HashT(const HashT &x);
	bool operator ==(uint32 x) const;
	bool operator !=(uint32 x) const;
	bool operator ==(HashT const &x) const;
	bool operator !=(HashT const &x) const;
	std::string str() const;
};

typedef HashT hasht;

HashT newhasht_cstring(const char*);

CSP_NAMESPACE_END

