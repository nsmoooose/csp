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

#include <iostream>
#include <string>

#include <SimData/Export.h>
#include <SimData/GlibCsp.h>
#include <SimData/hash_map.h>
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

extern SIMDATA_EXPORT u4 newhash4_cstring(const char*);
extern HashT newhasht_cstring(const char*);
//extern u8 newhash8_cstring(const char*);

//typedef  u8       hasht;
struct SIMDATA_EXPORT HashT {
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


#if defined(_MSC_VER) && (_MSC_VER == 1300) 
	struct eqint: public std::hash_compare<int const> {
		size_t operator ()(int const& i) const {
			return i;
		}
		bool operator()(int const& i1, int const& i2) const {
			return (i1 < i2);
		}
	};
#else
	struct eqint {
		bool operator()(int i1, int i2) const {
			return (i1 == i2);
		}
	};
#endif

struct hashint {
	int operator()(int i) const {
		return i;
	}
};


#if defined(_MSC_VER) && (_MSC_VER == 1300) 
	struct eqstr: public std::hash_compare<char const*>
		{
			size_t operator()(char const* s ) const {
				return newhash4_cstring(s);
			}
			bool operator()(char const* s1, char const* s2) const {
				return (strcmp(s1, s2) < 0);
			}
		};
#else
	struct eqstr {
		bool operator()(const char* s1, const char* s2) const {
			return (strcmp(s1, s2) == 0);
		}
	
	};
#endif


struct hasht_hash {
	guint32 operator()(hasht i1) const {
		return i1.a;
	}
};

#if defined(_MSC_VER) && (_MSC_VER == 1300) 
	struct hasht_eq: public std::hash_compare<hasht const> {
		size_t operator()( hasht const& i1 ) const {
		return i1.a;
			}
		bool operator()(hasht const& i1, hasht const& i2) const {
			return (i1.a < i2.a || (i1.a == i2.a && i1.b < i2.b));
		}
	};
#else
	struct hasht_eq {
		bool operator()(hasht i1, hasht i2) const {
			return ((i1.a == i2.a) && (i1.b == i2.b));
		}
	};
#endif

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

/*
 * Helpers for storing a hash map of string->int.
*/
struct SIMDATA_EXPORT hashstring {
	static HASH<const char*> h;
	int operator()(const std::string &s) const {
		return h(s.c_str());
	}
};

#if defined(_MSC_VER) && (_MSC_VER == 1300) 
	struct eqstring: public std::hash_compare<std::string const> {
		size_t operator()( std::string const& a ) const {
			return eqstr()(a.c_str());
		}
		bool operator()(std::string const& a, std::string const& b) const {
				return (a.compare(b) < 0);
		}
	};
#else
	struct eqstring {
		bool operator()(const std::string &a, const std::string &b) const {
			return a == b;
		}
	};
#endif


class Object;
class ObjectProxyBase;

template <class T>
	struct HASHT_MAP {
		typedef HASH_MAPS<hasht, T, hasht_hash, hasht_eq>::Type Type;
	};
typedef HASH_MAPS<hasht, int, hasht_hash, hasht_eq>::Type hasht_map;
typedef HASH_MAPS<hasht, Object*, hasht_hash, hasht_eq>::Type cache_map;
typedef HASH_MAPS<hasht, ObjectProxyBase*, hasht_hash, hasht_eq>::Type proxy_map;

typedef HASH_MAPS<const std::string, int, hashstring, eqstring>::Type string_map;

extern std::ostream & operator<<(std::ostream &o, const hasht &x);

NAMESPACE_END // namespace simdata

#endif // __HASHUTILITY_H__
