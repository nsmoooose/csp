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
 * @file HashUtility.cpp
 *
 * Hash functions utilities
 *
 * The hash functions coded here are from "Hash Functions for Hash
 * Table Lookup" by Robert J. Jenkins Jr., and are Public Domain.
 * See <http://burtleburtle.net/bob/hash/evahash.html>
 */

#include <sstream>
#include <iomanip>

#include <csp/csplib/util/HashUtility.h>


namespace csp {


// The mixing step
#define mix(a,b,c) \
{ \
	a=a-b;  a=a-c;  a=a^(c>>13); \
	b=b-c;  b=b-a;  b=b^(a<<8);  \
	c=c-a;  c=c-b;  c=c^(b>>13); \
	a=a-b;  a=a-c;  a=a^(c>>12); \
	b=b-c;  b=b-a;  b=b^(a<<16); \
	c=c-a;  c=c-b;  c=c^(b>>5);  \
	a=a-b;  a=a-c;  a=a^(c>>3);  \
	b=b-c;  b=b-a;  b=b^(a<<10); \
	c=c-a;  c=c-b;  c=c^(b>>15); \
}

/** A character string to 32-bit hash function.
 *
 *  @author Bob Jenkins, December 1996, Public Domain.
 *
 *  @param k the string to hash
 *  @param length the length of the string in bytes.
 *  @param  initval the previous hash, or an arbitrary value
 */
inline uint32_t newhash(uint8_t const *k, uint32_t length, uint32_t initval)
{
	uint32_t a,b,c;           // the internal state
	uint32_t len;             // how many key bytes still need mixing

	// Set up the internal state
	len = length;
	a = b = 0x9e3779b9;  // the golden ratio; an arbitrary value
	c = initval;         // variable initialization of internal state

	// handle most of the key

	while (len >= 12) {
		a=a+(k[0]+((uint32_t)k[1]<<8)+((uint32_t)k[2]<<16) +((uint32_t)k[3]<<24));
		b=b+(k[4]+((uint32_t)k[5]<<8)+((uint32_t)k[6]<<16) +((uint32_t)k[7]<<24));
		c=c+(k[8]+((uint32_t)k[9]<<8)+((uint32_t)k[10]<<16)+((uint32_t)k[11]<<24));
		mix(a,b,c);
		k = k+12; len = len-12;
	}

	// handle the last 11 bytes
	c = c+length;
	switch(len)  { // all the case statements fall through
	case 11: c=c+((uint32_t)k[10]<<24);  /* fall through */
	case 10: c=c+((uint32_t)k[9]<<16);   /* fall through */
	case 9 : c=c+((uint32_t)k[8]<<8);    /* fall through */
	// the first byte of c is reserved for the length
	case 8 : b=b+((uint32_t)k[7]<<24);   /* fall through */
	case 7 : b=b+((uint32_t)k[6]<<16);   /* fall through */
	case 6 : b=b+((uint32_t)k[5]<<8);    /* fall through */
	case 5 : b=b+k[4]; /* fall through */
	case 4 : a=a+((uint32_t)k[3]<<24);   /* fall through */
	case 3 : a=a+((uint32_t)k[2]<<16);   /* fall through */
	case 2 : a=a+((uint32_t)k[1]<<8);    /* fall through */
	case 1 : a=a+k[0];                 /* fall through */
	 // case 0: nothing left to add
	}
	mix(a,b,c);
	// report the result
	return c;
}


/** Generate a 32-bit hash from a string
 */
uint32_t newhash4_cstring(std::string const &str) {
	return newhash((uint8_t const*)str.c_str(), str.size(), 0);
}

/** Generate a 64-bit hash (HashT) from a string
 */
HashT newhasht_cstring(std::string const &str) {
	uint32_t h0, h1;
	h0 = newhash(reinterpret_cast<uint8_t const*>(str.c_str()), str.size(), 0);
	h1 = newhash(reinterpret_cast<uint8_t const*>(str.c_str()), str.size(), 1);
	return HashT(h1, h0);
}

// 32-bit hash helpers

uint32_t hash_uint32(uint32_t x) {
	return newhash(reinterpret_cast<uint8_t const*>(&x), 4, 0);
}

uint32_t hash_uint32(uint64_t x) {
	return newhash(reinterpret_cast<uint8_t const*>(&x), 8, 0);
}

uint32_t hash_uint32(std::string const &x) {
	return newhash(reinterpret_cast<uint8_t const*>(x.c_str()), x.size(), 0);
}

uint32_t hash_uint32(const char *buffer, const int len) {
	return newhash(reinterpret_cast<uint8_t const*>(buffer), len, 0);
}


std::string HashT::str() const {
	std::ostringstream repr;
	repr << *this;
	return repr.str();
}

std::ostream & operator<<(std::ostream &o, const hasht &x) {
	return o << "(" << std::hex << std::setw(8) << std::setfill('0') << x.b << ":"
	                            << std::setw(8) << std::setfill('0') << x.a
	                << std::dec << std::setw(0) << std::setfill(' ') << ")";
}


} // namespace csp

