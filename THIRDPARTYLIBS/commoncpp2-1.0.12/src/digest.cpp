// Copyright (C) 1999-2001 Open Source Telecom Corporation.
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

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/digest.h>

#include <cstdio>

#ifdef	WIN32
#include <io.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

Digest::Digest() :
streambuf()
#ifdef	HAVE_OLD_IOSTREAM
,ostream()
#else
,ostream((streambuf *)this)
#endif
{ 
#ifdef	HAVE_OLD_IOSTREAM
	init((streambuf *)this);
#endif
}

ChecksumDigest::ChecksumDigest() :
Digest()
{
	csum = 0;
}

int ChecksumDigest::overflow(int c)
{
	csum += c;
	return c;
}

unsigned ChecksumDigest::getDigest(unsigned char *buffer)
{
	*buffer = csum;
	return 1;
}

void ChecksumDigest::putDigest(const unsigned char *buffer, unsigned len)
{
	while(len--)
		csum += *(buffer++);
}

ostream &ChecksumDigest::strDigest(ostream &os)
{
	char buf[3];

	sprintf(buf, "%02x", csum);
	os << buf;
	return os;
}

CRC16Digest::CRC16Digest() :
Digest()
{
	crc16 = 0;
}

int CRC16Digest::overflow(int c)
{
	crc16 = (unsigned char)(crc16 >> 8) | (crc16 << 8);
	crc16 ^= (unsigned char)(c);
	crc16 ^= (unsigned char)(crc16 & 0xff) >> 4;
	crc16 ^= (crc16 << 8) << 4;
	crc16 ^= ((crc16 & 0xff) << 4) << 1;
	return c;
}

unsigned CRC16Digest::getDigest(unsigned char *buffer)
{
	memcpy(buffer, &crc16, 2);
	return 2;
}

void CRC16Digest::putDigest(const unsigned char *buffer, unsigned len)
{
	while(len--)
		overflow(*buffer++);
}


ostream &CRC16Digest::strDigest(ostream &os)
{
	char buf[5];

	sprintf(buf, "%04x", (unsigned)crc16);
	os << buf;
	return os;
}


CRC32Digest::CRC32Digest() 
      : Digest()
{
   initDigest();
   crc32 = 0;
}

void CRC32Digest::initDigest(void)
{
      // the generator polynomial used here is the same as Ethernet
      // x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
   const unsigned long POLYNOMIAL = 0x04C11DB7;

      // Initialize the accumulator to all ones
   crc_reg = 0xFFFFFFFF;

      // Initialize the lookup table
   register int i,j;
   register unsigned long crc;
   
   for (i = 0; i < 256; i++)
   {
      crc = ( (unsigned long) i << 24 );
      for (j = 0; j < 8; j++)
      {
         if (crc & 0x80000000)
            crc = (crc << 1) ^ POLYNOMIAL;
         else
            crc <<= 1;
      }
      crc_table[i] = crc;
   }
}

unsigned char CRC32Digest::overflow(unsigned char octet)
{  
   crc_reg = crc_table[((crc_reg >> 24) ^ octet) & 0xFF] ^ (crc_reg << 8);
   crc32 = ~crc_reg;
   
   return octet;
}

unsigned CRC32Digest::getDigest(unsigned char *buffer)
{
   memcpy(buffer, &crc32, 4);
   return 4;
}

void CRC32Digest::putDigest(const unsigned char *buffer, unsigned len)
{
   while(len--)
      overflow(*buffer++);
}

ostream& CRC32Digest::strDigest(ostream &os)
{
   char buf[9];
   
   sprintf(buf, "%08lx", (unsigned long)crc32);
   os << buf;
   return os;
}


#ifdef	CCXX_NAMESPACES
};
#endif	
	
