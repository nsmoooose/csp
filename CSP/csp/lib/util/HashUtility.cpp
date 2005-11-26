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

#include <csp/lib/util/HashUtility.h>


CSP_NAMESPACE


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
inline uint32 newhash(register uint8 const *k, uint32 length, uint32 initval)
{
	register uint32 a,b,c;  // the internal state
	uint32 len;             // how many key bytes still need mixing

	// Set up the internal state
	len = length;
	a = b = 0x9e3779b9;  // the golden ratio; an arbitrary value
	c = initval;         // variable initialization of internal state

	// handle most of the key

	while (len >= 12) {
		a=a+(k[0]+((uint32)k[1]<<8)+((uint32)k[2]<<16) +((uint32)k[3]<<24));
		b=b+(k[4]+((uint32)k[5]<<8)+((uint32)k[6]<<16) +((uint32)k[7]<<24));
		c=c+(k[8]+((uint32)k[9]<<8)+((uint32)k[10]<<16)+((uint32)k[11]<<24));
		mix(a,b,c);
		k = k+12; len = len-12;
	}

	// handle the last 11 bytes
	c = c+length;
	switch(len)  { // all the case statements fall through
	case 11: c=c+((uint32)k[10]<<24);
	case 10: c=c+((uint32)k[9]<<16);
	case 9 : c=c+((uint32)k[8]<<8);
	// the first byte of c is reserved for the length
	case 8 : b=b+((uint32)k[7]<<24);
	case 7 : b=b+((uint32)k[6]<<16);
	case 6 : b=b+((uint32)k[5]<<8);
	case 5 : b=b+k[4];
	case 4 : a=a+((uint32)k[3]<<24);
	case 3 : a=a+((uint32)k[2]<<16);
	case 2 : a=a+((uint32)k[1]<<8);
	case 1 : a=a+k[0];
	 // case 0: nothing left to add
	}
	mix(a,b,c);
	// report the result
	return c;
}


/** Generate a 32-bit hash from a string
 */
uint32 newhash4_cstring(std::string const &str) {
	return newhash((uint8 const*)str.c_str(), str.size(), 0);
}

/** Generate a 64-bit hash (HashT) from a string
 */
HashT newhasht_cstring(std::string const &str) {
	uint32 h0, h1;
	h0 = newhash(reinterpret_cast<uint8 const*>(str.c_str()), str.size(), 0);
	h1 = newhash(reinterpret_cast<uint8 const*>(str.c_str()), str.size(), 1);
	return HashT(h1, h0);
}

// 32-bit hash helpers

uint32 hash_uint32(uint32 x) {
	return newhash(reinterpret_cast<uint8 const*>(&x), 4, 0);
}

uint32 hash_uint32(uint64 x) {
	return newhash(reinterpret_cast<uint8 const*>(&x), 8, 0);
}

uint32 hash_uint32(std::string const &x) {
	return newhash(reinterpret_cast<uint8 const*>(x.c_str()), x.size(), 0);
}

uint32 hash_uint32(const char *buffer, const int len) {
	return newhash(reinterpret_cast<uint8 const*>(buffer), len, 0);
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


CSP_NAMESPACE_END

