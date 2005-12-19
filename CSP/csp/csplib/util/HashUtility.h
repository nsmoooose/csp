/* Combat Simulator Project
 * Copyright (C) 2002, 2005 Mark Rose <mkrose@users.sf.net>
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
 * @brief Hash function utilities.
 */

#ifndef __CSPLIB_UTIL_HASHUTILITY_H__
#define __CSPLIB_UTIL_HASHUTILITY_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Uniform.h>
#include <csp/csplib/util/hash_map.h>
#include <csp/csplib/util/Namespace.h>

#include <iosfwd>
#include <string>


CSP_NAMESPACE


// Log a fatal error and abort the program.  Also declared in Log.h, but
// redeclared here for convenience.  Defined in LogStream.cpp
extern void CSPLIB_EXPORT fatal(std::string const &msg);


/**
 * @defgroup HashUtils Hash Utilities
 */

//@{

struct HashT;

extern CSPLIB_EXPORT uint32 newhash4_cstring(std::string const &);
extern CSPLIB_EXPORT HashT newhasht_cstring(std::string const &);

/** CSP standard 32-bit hash functions.
 */
extern CSPLIB_EXPORT uint32 hash_uint32(uint32);
extern CSPLIB_EXPORT uint32 hash_uint32(uint64);
extern CSPLIB_EXPORT uint32 hash_uint32(std::string const &);
extern CSPLIB_EXPORT uint32 hash_uint32(const char *buffer, const int len);


/** A 64-bit hash value.
 */
struct CSPLIB_EXPORT HashT {
	uint32 a, b;

	/** Construct a default hash value (0).
	 */
	HashT() {
		a = b = 0;
	}

	/** Construct a specific hash value.
	 *
	 *  The high 32-bits will be zero.
	 *
	 *  @param x The low 32-bits of the hash value.
	 */
	HashT(uint32 x) {
		a = x; b = 0;
	}

	/** Construct a specific hash value.
	 *
	 *  @param b_ The high 32-bits of the hash value.
	 *  @param a_ The low 32-bits of the hash value.
	 */
	HashT(uint32 b_, uint32 a_) {
		a = a_; b = b_;
	}

	/** Copy constructor
	 */
	HashT(const HashT &x) {
		a = x.a; b = x.b;
	}

	/** Hash a string.
	 */
	HashT(std::string const &x) {
		*this = newhasht_cstring(x);
	}

	/** Compare this hash value for equality.
	 *
	 *  @param x The low 32-bits of the hash value to compare to.  The
	 *           high 32-bits are zero.
	 */
	bool operator ==(uint32 x) const {
		return (b == 0 && a == x);
	}

	/** Compare this hash value for inequality.
	 *
	 *  @param x The low 32-bits of the hash value to compare to.  The
	 *           high 32-bits are zero.
	 */
	bool operator !=(uint32 x) const {
		return (b != 0 || a != x);
	}

	/** Compare two hash values for equality.
	 */
	bool operator ==(HashT const &x) const {
		return (b == x.b && a == x.a);
	}

	/** Compare two hash values for inequality.
	 */
	bool operator !=(HashT const &x) const {
		return (b != x.b || a != x.a);
	}

	/** Compare two hash values for ordering.
	 */
	bool operator <(HashT const &x) const {
		return (b < x.b || (b == x.b && a < x.a));
	}

	/** Hash a string by assignment.
	 */
	HashT & operator =(std::string &x) {
		*this = newhasht_cstring(x);
		return *this;
	}

	/** Assign a specific hash value.
	 *
	 *  @param x The low 32-bits to assign.  The high 32-bits are
	 *           zeroed.
	 */
	HashT & operator =(uint32 x) {
		a = x; b = 0;
		return *this;
	}

	/** Assign a specific hash value.
	 */
	HashT & operator =(HashT const &x) {
		a = x.a; b = x.b;
		return *this;
	}

	/** Convert to an unsigned 64-bit integer.
	 */
	uint64 u64() const {
		return (static_cast<uint64>(a) << 32) | b;
	}

	/** Get a string representation of this hash.
	 */
	std::string str() const;
};

typedef HashT hasht;

/** The primary string to hash function used internally by CSP.
 */
inline HashT hash_string(std::string const &key) { return newhasht_cstring(key); }



template <class C> struct Equal: public std::binary_function<C, C, bool> {
	bool operator()(const C& __x, const C& __y) const { return __x == __y; }
};

template <> struct Equal<const char*>: public std::binary_function<const char*, const char*, bool> {
	bool operator()(const char* __x, const char* __y) const { return strcmp(__x, __y) == 0; }
};

template <class C> struct Less: public std::binary_function<C, C, bool> {
	bool operator()(const C& __x, const C& __y) const { return __x < __y; }
};

template <> struct Less<const char*>: public std::binary_function<const char*, const char*, bool> {
	bool operator()(const char* __x, const char* __y) const { return strcmp(__x, __y) < 0; }
};

template <class C> struct Hash {
	std::size_t operator()(const C& __x) const { return CSP_HASH<C>()(__x); }
};

template <> struct Hash<int64> {
	std::size_t operator()(const int64& __x) const { return static_cast<std::size_t>(__x); }
};

template <> struct Hash<uint64> {
	std::size_t operator()(const uint64& __x) const { return static_cast<std::size_t>(__x); }
};

template <> struct Hash<hasht> {
	std::size_t operator()(const hasht& __x) const { return static_cast<std::size_t>(__x.u64()); }
};

template <> struct Hash<std::string> {
	std::size_t operator()(const std::string& __x) const { return CSP_HASH<const char*>()(__x.c_str()); }
};


#if defined(_MSC_VER) && (_MSC_VER >= 1300)
template<class C> struct HashCompare: public CSP_HASH<C> {
	inline std::size_t operator()(C const &x) const { return Hash<C>()(x); }
	inline bool operator()(C const &x, C const &y) const { return Less<C>()(x, y); }
};
#else
template<class C> struct HashCompare {
	inline std::size_t operator()(C const &x) const { return Hash<C>()(x); }
	inline bool operator()(C const &x, C const &y) const { return Equal<C>()(x, y); }
};
#endif


/** Adapter type for platform-specific hash_map classes.
 */
template <class key, class val, class cmp = HashCompare<key> > struct HashMap {
#if defined(_MSC_VER) && (_MSC_VER >= 1300)
	typedef CSP_HASHMAP<key, val, cmp> Type;
#else
	typedef CSP_HASHMAP<key, val, cmp, cmp> Type;
#endif
};


extern CSPLIB_EXPORT std::ostream & operator<<(std::ostream &o, const hasht &x);


// 32-bit fingerprint functions.
typedef uint32 fprint32;
inline fprint32 fingerprint(uint32 x) { return hash_uint32(x); }
inline fprint32 fingerprint(uint64 x) { return hash_uint32(x); }
inline fprint32 fingerprint(std::string const &x) { return hash_uint32(x); }
inline fprint32 fingerprint(const char *buffer, const int len) { return hash_uint32(buffer, len); }

// Combine 32 bit hashes to produce a fingerprint that depends on the
// order of the values.
inline fprint32 make_ordered_fingerprint(fprint32 x, fprint32 y) {
	return (x << 11) ^ (x >> 21) ^ y;
}


// Combine 32 bit hashes to produce a fingerprint that is independent
// of the order of the values.
inline fprint32 make_unordered_fingerprint(fprint32 x, fprint32 y) {
	return x + y;
}

//@}


CSP_NAMESPACE_END


#endif // __CSPLIB_UTIL_HASHUTILITY_H__

