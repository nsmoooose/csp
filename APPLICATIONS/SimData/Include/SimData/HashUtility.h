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


/**
 * @file HashUtility.h
 *
 * Hash function utilities.
 */

#ifndef __HASHUTILITY_H__
#define __HASHUTILITY_H__

#include <string>
#include <iostream>
#include <SimData/Export.h>
#include <SimData/hash_map.h>
#include <SimData/GlibCsp.h>
#include <SimData/ns-simdata.h>

//typedef int int32;
//typedef char* cstring;

//typedef hash<const char*> hashstr;


//#define hashstr4 newhash4_cstring
//#define hashstr8 newhash8_cstring

// use 64-bit hash values

//#define hasht u8
#define hash_string newhasht_cstring
//#define hash_string hashstr8
//#define hasht_hash hashu8
//#define hasht_eq equ8

typedef  guint32  u4;   /* unsigned 4-byte type */
typedef  guint8   u1;   /* unsigned 1-byte type */
typedef  guint64  u8;   /* unsigned 8-byte type */
//typedef  u8       hasht;


NAMESPACE_SIMDATA

struct HashT;

extern u4 newhash4_cstring(const char*);
extern HashT newhasht_cstring(const char*);
//extern u8 newhash8_cstring(const char*);

//typedef  u8       hasht;
struct HashT {
	guint32 a, b;
	HashT() {
		a = b = 0;
	}
	HashT(guint32 x) {
		a = x; b = 0;
	}
	HashT(guint32 b_, guint32 a_) {
		a = a_; b = b_;
	}
	HashT(const HashT &x) {
		a = x.a; b = x.b;
	}
	HashT(std::string const &x) {
		*this = newhasht_cstring(x.c_str());
	}
	bool operator ==(guint32 x) const {
		return (b == 0 && a == x);
	}
	bool operator !=(guint32 x) const {
		return (b != 0 || a != x);
	}
	bool operator ==(HashT const &x) const {
		return (b == x.b && a == x.a);
	}
	bool operator !=(HashT const &x) const {
		return (b != x.b || a != x.a);
	}
	HashT & operator =(std::string &x) {
		*this = newhasht_cstring(x.c_str());
		return *this;
	}
	HashT & operator =(guint32 x) {
		a = x; b = 0;
		return *this;
	}
	HashT & operator =(HashT const &x) {
		a = x.a; b = x.b;
		return *this;
	}
	std::string str() const;
};

typedef HashT hasht;


struct eqstr {
	bool operator()(const char* s1, const char* s2) const {
		return (strcmp(s1, s2) == 0);
	}
};

struct eqint {
	bool operator()(int i1, int i2) const {
		return (i1 == i2);
	}
};

struct hashint {
	int operator()(int i) const {
		return i;
	}
};


struct hasht_eq {
	bool operator()(hasht i1, hasht i2) const {
		return ((i1.a == i2.a) && (i1.b == i2.b));
	}
};

struct hasht_hash {
	guint32 operator()(hasht i1) const {
		return i1.a;
	}
};

/*
struct equ8 {
	bool operator()(u8 i1, u8 i2) const {
		return (i1 == i2);
	}
};

struct hashu8 {
	u8 operator()(u8 i) const {
		return i;
	}
};
*/


class Object;
class ObjectProxyBase;

template <class T>
struct HASHT_MAP {
	typedef HASH_MAP<hasht, T, hasht_hash, hasht_eq> Type;
};

typedef HASH_MAP<hasht, int, hasht_hash, hasht_eq> hasht_map;
typedef HASH_MAP<hasht, Object*, hasht_hash, hasht_eq> cache_map;
typedef HASH_MAP<hasht, ObjectProxyBase*, hasht_hash, hasht_eq> proxy_map;

extern std::ostream & operator<<(std::ostream &o, const hasht &x);

NAMESPACE_END // namespace simdata

#endif // __HASHUTILITY_H__
