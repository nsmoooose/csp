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

%module HashUtility
%{
#include "SimData/HashUtility.h"
%}

#include "SimData/Namespace.h"

typedef  unsigned long  int  u4;   /* unsigned 4-byte type */
typedef  unsigned      char  u1;   /* unsigned 1-byte type */

%rename(hash_string) SIMDATA(newhasht_cstring)(const char*);

NAMESPACE_SIMDATA

struct HashT {
	HashT();
	HashT(guint32 x);
	HashT(guint32 b_, guint32 a_);
	HashT(const HashT &x);
	bool operator ==(guint32 x) const;
	bool operator !=(guint32 x) const;
	bool operator ==(HashT const &x) const;
	bool operator !=(HashT const &x) const;
};

typedef HashT hasht;

HashT newhasht_cstring(const char*);

NAMESPACE_SIMDATA_END

