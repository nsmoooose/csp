/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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


/**
 * @file HashUtility.h
 *
 * Hash function utilities.
 */

#ifndef __SIMDATA_HASHUTILITY_H__
#define __SIMDATA_HASHUTILITY_H__

# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning(disable : 4251)
# endif


#include <SimData/Export.h>
#include <SimData/Uniform.h>
#include <SimData/hash_map.h>
#include <SimData/Namespace.h>

#include <iostream>
#include <string>


NAMESPACE_SIMDATA



/**
 * @defgroup HashUtils Hash Utilities
 */

//@{

struct HashT;

extern SIMDATA_EXPORT uint32 newhash4_cstring(std::string const &);
extern SIMDATA_EXPORT HashT newhasht_cstring(std::string const &);


/** A 64-bit hash value.
 */
struct SIMDATA_EXPORT HashT {
	uint32 a, b;
	HashT() {
		a = b = 0;
	}
	HashT(uint32 x) {
		a = x; b = 0;
	}
	HashT(uint32 b_, uint32 a_) {
		a = a_; b = b_;
	}
	HashT(const HashT &x) {
		a = x.a; b = x.b;
	}
	HashT(std::string const &x) {
		*this = newhasht_cstring(x);
	}
	bool operator ==(uint32 x) const {
		return (b == 0 && a == x);
	}
	bool operator !=(uint32 x) const {
		return (b != 0 || a != x);
	}
	bool operator ==(HashT const &x) const {
		return (b == x.b && a == x.a);
	}
	bool operator !=(HashT const &x) const {
		return (b != x.b || a != x.a);
	}
	HashT & operator =(std::string &x) {
		*this = newhasht_cstring(x);
		return *this;
	}
	HashT & operator =(uint32 x) {
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

//#define hash_string newhasht_cstring
inline HashT hash_string(std::string const &key) { return newhasht_cstring(key); }



/** Integer equality functor for hash_map.
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1300) 
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

/** Integer hash functor for hash_map.
 */
struct hashint {
	int operator()(int i) const {
		return i;
	}
};


/** const char* equality functor for hash_map.
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1300) 
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

/** hasht hash functor for hash_map.
 */
struct hasht_hash {
	uint32 operator()(hasht i1) const {
		return i1.a;
	}
};

/** hasht equality functor for hash_map.
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1300) 
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


/** String hash functor for hash_map.
 */
struct SIMDATA_EXPORT hashstring {
	static HASH<const char*> h;
	int operator()(const std::string &s) const {
		return h(s.c_str());
	}
};

/** String equality functor for hash_map.
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1300) 
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

/** hash_map specialization type for hasht to T maps.
 */
template <class T>
struct HASHT_MAP {
	typedef typename HASH_MAPS<hasht, T, hasht_hash, hasht_eq>::Type Type;
};

/** A hasht to int map.
 */
typedef HASH_MAPS<hasht, int, hasht_hash, hasht_eq>::Type hasht_map;

/** A hasht to ObjectProxyBase* map.
 */
typedef HASH_MAPS<hasht, ObjectProxyBase*, hasht_hash, hasht_eq>::Type proxy_map;

//typedef HASH_MAPS<const std::string, int, hashstring, eqstring>::Type string_map;

extern std::ostream & operator<<(std::ostream &o, const hasht &x);


//@}


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_HASHUTILITY_H__

