// Copyright (C) 2002 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other files
// to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
//
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

/**
 * @file urlstring.h
 * @short URL and Base64 strings codec functions.
 **/

#ifndef	CCXX_URLSTRING_H_
#define	CCXX_URLSTRING_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#include <string>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

/** @relates URLStream
 * Decode an url parameter (ie "\%20" -> " ")
 * @param source string
 * @param dest destination buffer. If NULL source is used
 */
CCXX_EXPORT(char*) urlDecode(char *source, char *dest = NULL);

/** @relates URLStream
 * Encode an url parameter (ie " " -> "+")
 * @param source string
 * @param dest destination buffer. Do not overlap with source
 * @param size destination buffer size.
 */
CCXX_EXPORT(char*) urlEncode(const char *source, char *dest, unsigned size);

/** @relates URLStream
 * Decode a string using base64 coding.
 * Destination size should be at least strlen(src)+1.
 * Destination will be a string, so is always terminated .
 * This function is deprecated, base64 can use binary source, not only string
 * use overloaded b64Decode.
 * @return string coded
 * @param src  source buffer
 * @param dest destination buffer. If NULL src is used
 */
CCXX_EXPORT(char*) b64Decode(char *src, char *dest = NULL);

/** @relates URLStream
 * Encode a string using base64 coding.
 * Destination size should be at least strlen(src)/4*3+1.
 * Destination is string terminated.
 * This function is deprecated, coded stream can contain terminator character
 * use overloaded b64Encode instead.
 * @return destination buffer
 * @param source source string
 * @param dest   destination octet buffer
 * @param size   destination buffer size
 */
CCXX_EXPORT(char*) b64Encode(const char *source, char *dest, unsigned size);

/** @relates URLStream
 * Encode a octet stream using base64 coding.
 * Destination size should be at least (srcsize+2)/3*4+1.
 * Destination will be a string, so is always terminated 
 * (unless you pass dstsize == 0).
 * @return size of string written not counting terminator
 * @param src     source buffer
 * @param srcsize source buffer size
 * @param dst     destination buffer
 * @param dstsize destination buffer size
 */
CCXX_EXPORT(size_t) b64Encode(const unsigned char *src, size_t srcsize,
	       char *dst, size_t dstsize);

/** @relates URLStream
 * Decode a string using base64 coding.
 * Destination size should be at least strlen(src)/4*3.
 * Destination are not string terminated (It's just a octet stream).
 * @return number of octets written into destination buffer
 * @param src     source string
 * @param dst     destination octet buffer
 * @param dstsize destination buffer size
 */
CCXX_EXPORT(size_t) b64Decode(const char *src,
		unsigned char *dst, size_t dstsize);

/** @relates URLStream
 * Encode a STL string using base64 coding into a STL string
 * @return base 64 encoded string
 * @param src source string
 */
CCXX_EXPORT(std::string) b64Encode(const std::string& src);

/** @relates URLStream
 * Decode a STL string using base64 coding into an STL String.
 * Destination size should be at least strlen(src)/4*3.
 * Destination are not string terminated (It's just a octet stream).
 * @return decoded string
 * @param src     source string
 */
CCXX_EXPORT(std::string) b64Decode(const std::string& src);

/** @relates URLStream
 * Encode a octet stream using base64 coding into a STL string
 * @return base 64 encoded string
 * @param src     source buffer
 * @param srcsize source buffer size
 */
CCXX_EXPORT(std::string) b64Encode(const unsigned char *src, size_t srcsize);

/** @relates URLStream
 * Decode a string using base64 coding.
 * Destination size should be at least strlen(src)/4*3.
 * Destination are not string terminated (It's just a octet stream).
 * @return number of octets written into destination buffer
 * @param src     source string
 * @param dst     destination octet buffer
 * @param dstsize destination buffer size
 */
CCXX_EXPORT(size_t) b64Decode(const std::string& src,
		unsigned char *dst, size_t dstsize);

#ifdef	CCXX_NAMESPACES
};
#endif

#endif
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
