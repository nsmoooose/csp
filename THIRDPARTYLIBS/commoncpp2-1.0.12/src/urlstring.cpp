// Copyright (C) 2001-2002 Open Source Telecom Corporation.
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
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
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

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/urlstring.h>

#include <string>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#ifdef	WIN32
#include <io.h>
#endif
#include <cctype>

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

static const unsigned char alphabet[65] = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

CCXX_EXPORT(char*) b64Encode(const char *source, char *dest, unsigned limit)
{
	b64Encode((const unsigned char*)source,strlen(source),
			dest,limit);
	return dest;
}

CCXX_EXPORT(char*) b64Decode(char *source, char *dest)
{
	size_t srcsize = strlen(source);
	char* dst = dest?dest:source;
	size_t dstsize = 
		b64Decode(source,(unsigned char*)dst,srcsize+1);
	dst[dstsize] = 0;
	return dst;
}

CCXX_EXPORT(size_t) b64Encode(const unsigned char *src, size_t srcsize,
		               char *dst, size_t dstsize)
{
	if (!dstsize) return 0;
	
	char* pdst = dst;
	unsigned bits;
	
	while(srcsize >= 3 && dstsize > 4)
	{
		bits = (((unsigned)src[0])<<16) | (((unsigned)src[1])<<8) 
			| ((unsigned)src[2]);
		src += 3;
		srcsize -= 3;
		*(pdst++) = alphabet[bits >> 18];
	    	*(pdst++) = alphabet[(bits >> 12) & 0x3f];
	    	*(pdst++) = alphabet[(bits >> 6) & 0x3f];
	    	*(pdst++) = alphabet[bits & 0x3f];
		dstsize -= 4;
	}
	if (srcsize && dstsize > 4) 
	{
		bits = ((unsigned)src[0])<<16;
		*(pdst++) = alphabet[bits >> 18];
		if (srcsize == 1)
		{
			*(pdst++) = alphabet[(bits >> 12) & 0x3f];
	    		*(pdst++) = '=';
		}
		else
		{
			bits |= ((unsigned)src[1])<<8;
			*(pdst++) = alphabet[(bits >> 12) & 0x3f];
	    		*(pdst++) = alphabet[(bits >> 6) & 0x3f];
		}
	    	*(pdst++) = '=';
	}
	*pdst = 0;
	return pdst-dst;
}

CCXX_EXPORT(size_t) b64Decode(const char *src,
		unsigned char *dst, size_t dstsize)
{
        char decoder[256];
    	int i, bits, c;

	unsigned char *pdst = dst;
	
	for (i = 0; i < 256; ++i)
		decoder[i] = 64;
    	for (i = 0; i < 64 ; ++i) 
		decoder[alphabet[i]] = i;

    	bits = 1;

	while(*src)
	{ 
		c = (unsigned char)(*(src++));
		if (c == '=')
		{
			if (bits & 0x40000)
			{
				if (dstsize < 2) break;
				*(pdst++) = (bits >> 10);
				*(pdst++) = (bits >> 2) & 0xff;
				break;
			}
			if (bits & 0x1000 && dstsize)
				*(pdst++) = (bits >> 4);
			break;
		}
		// skip invalid chars
		if (decoder[c] == 64)
			continue;
		bits = (bits << 6) + decoder[c];
		if (bits & 0x1000000) 
		{
			if (dstsize < 3) break;
			*(pdst++) = (bits >> 16);
			*(pdst++) = (bits >> 8) & 0xff;
			*(pdst++) = (bits & 0xff);
		    	bits = 1;
			dstsize -= 3;
		} 
	}
	return pdst-dst;
}


CCXX_EXPORT(char*) urlDecode(char *source, char *dest)
{
	char *ret;
	char hex[3];

	if(!dest)
		dest = source;
	else
		*dest = 0;

	ret = dest;

	if(!source)
		return dest;

	while(*source)
	{
		switch(*source)
		{
		case '+':
			*(dest++) = ' ';
			break;
		case '%':
			// NOTE: wrong input can finish with "...%" giving
			// buffer overflow, cut string here
			if(source[1])
			{
				hex[0] = source[1];
				++source;
				if(source[1])
				{
					hex[1] = source[1];
					++source;
				}
				else
					hex[1] = 0;
			}
			else
				hex[0] = hex[1] = 0;	
			hex[2] = 0;
			*(dest++) = (char)strtol(hex, NULL, 16);
			break;
		default:
			*(dest++) = *source;
		}
		++source;
	}
	*dest = 0;
	return ret;
}	

CCXX_EXPORT(char*) urlEncode(const char *source, char *dest, unsigned max)	
{
	static const char *hex = "0123456789abcdef";
	unsigned len = 0;
	unsigned char ch;
	char *ret = dest;

	*dest = 0;
	if(!source)
		return dest;

	while(len < max - 4 && *source)
	{
		ch = (unsigned char)*source;
		if(*source == ' ')
			*(dest++) = '+';
		else if(isalnum(*source) || strchr("/.-", *source))
			*(dest++) = *source;
		else
		{
			*(dest++) = '%';
			// char in C++ can be more than 8bit
			*(dest++) = hex[(ch >> 4)&0xF];
			*(dest++) = hex[ch % 16];
		}	
		++source;
	}
	*dest = 0;
	return ret;
}

/** @relates URLStream
 * Encode a STL string using base64 coding into a STL string
 * @return base 64 encoded string
 * @param src source string
 */
CCXX_EXPORT(std::string) b64Encode(const std::string& src)
{
  size_t limit = (src.length()+2)/3*4+1;  // size + null must be included
  char* buffer = new char[limit];

  unsigned size = b64Encode((const unsigned char *)src.c_str(), src.length(), buffer, limit);
  buffer[size] = '\0';

  std::string final = string(buffer);
  delete buffer;
  return final;
}

/** @relates URLStream
 * Decode a STL string using base64 coding into an STL String.
 * Destination size should be at least strlen(src)/4*3.
 * Destination are not string terminated (It's just a octet stream).
 * @return decoded string
 * @param src     source string
 */
CCXX_EXPORT(std::string) b64Decode(const std::string& src)
{
  size_t limit = src.length()/4*3;
  unsigned char* buffer = new unsigned char[limit+1];

  unsigned size = b64Decode(src.c_str(), buffer, limit);
  buffer[size] = '\0';

  std::string final = string((char*)buffer);
  delete buffer;
  return final;
}

/** @relates URLStream
 * Encode a octet stream using base64 coding into a STL string
 * @return base 64 encoded string
 * @param src     source buffer
 * @param srcsize source buffer size
 */
CCXX_EXPORT(std::string) b64Encode(const unsigned char *src, size_t srcsize)
{
  size_t limit = (srcsize+2)/3*4+1;
  char* buffer = new char[limit];

  unsigned size = b64Encode(src, srcsize, buffer, limit);
  buffer[size] = '\0';

  std::string final = string(buffer);
  delete buffer;
  return final;
}

/** @relates URLStream
 * Decode an STL string encoded using base64.
 * Destination size should be at least strlen(src)/4*3.
 * Destination are not string terminated (It's just a octet stream).
 * @return number of octets written into destination buffer
 * @param src     source string
 * @param dst     destination octet buffer
 * @param dstsize destination buffer size
 */
CCXX_EXPORT(size_t) b64Decode(const std::string& src,
  unsigned char *dst, size_t dstsize)
{
  return b64Decode(src.c_str(), dst, dstsize);
}

#ifdef	CCXX_NAMESPACES
};
#endif
