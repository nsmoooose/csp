// Copyright (C) 1999-2005 Open Source Telecom Corporation.
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
// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however    
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License. 
//
// This exception applies only to the code released under the name GNU
// Common C++.  If you copy code from other releases into a copy of GNU 
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
//
// If you write modifications of your own for GNU Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.
//

/**
 * @file digest.h
 * @short Digest algorithms: checksum, CRC and MD5.
 **/

#ifndef	CCXX_DIGEST_H_
#define	CCXX_DIGEST_H_

#ifndef CCXX_MISSING_H_
#include <cc++/missing.h>
#endif

#ifndef CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#ifndef	CCXX_EXCEPTION_H_
#include <cc++/exception.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

/**
 * The digest base class is used for implementing and deriving one way
 * hashing functions.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short base class for hashing services.
 */
class __EXPORT Digest : protected std::streambuf, public std::ostream
{
protected:
	Digest();

	/**
	 * Get the size of a digest in octets.
	 *
	 * @return number of bytes in digest.
	 */
	virtual unsigned getSize(void) = 0;

	/**
	 * Copy the binary digest buffer to user memory.
	 *
	 * @return number of bytes in digest.
	 * @param buffer to write into.
	 */
	virtual unsigned getDigest(unsigned char *buffer) = 0;

	/**
	 * Put data into the digest bypassing the stream subsystem.
	 *
	 * @param buffer to read from.
	 * @param length of data.
	 */
	virtual void putDigest(const unsigned char *buffer, unsigned length) = 0;

	/**
	 * print a digest string for export.
	 *
	 * @return string representation of digest.
	 */
	virtual std::ostream &strDigest(std::ostream &os) = 0;

	friend std::ostream &operator<<(std::ostream &os, Digest &ia)
		{return ia.strDigest(os);};

public:
	/**
	 * Reset the digest table to an initial default value.
	 */
	virtual void initDigest(void) = 0;
};

/**
 * A simple checksum digest function.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short checksum hash function.
 */
class __EXPORT ChecksumDigest : public Digest
{
private:
	unsigned char csum;

protected:
	int overflow(int c);
	std::ostream &strDigest(std::ostream &os);

public:
	ChecksumDigest();

	void initDigest(void)
		{csum = 0;};

	unsigned getSize(void)
		{return 1;};

	unsigned getDigest(unsigned char *buffer);

	void putDigest(const unsigned char *buffer, unsigned length);
};

/**
 * A crc16 collection/compution hash accumulator class.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short crc16 computation hash.
 */
class __EXPORT CRC16Digest : public Digest
{
private:
	unsigned short crc16;

protected:
	int overflow(int c);

	std::ostream &strDigest(std::ostream &os);

public:
	CRC16Digest();

	inline void initDigest(void)
		{crc16 = 0;};

	inline unsigned getSize(void)
		{return 2;};

	unsigned getDigest(unsigned char *buffer);

	void putDigest(const unsigned char *buffer, unsigned length);
};

/**
 * A crc32 collection/computation hash accumulator class.
 *
 * @author Kevin Kraatz <kraatz@arlut.utexas.edu>
 * @short crc32 computation hash.
 */

class __EXPORT CRC32Digest : public Digest
{
private:
   unsigned long crc_table[256];
   unsigned long crc_reg;
   unsigned long crc32;
   
protected:
   unsigned char overflow(unsigned char octet);

   std::ostream &strDigest(std::ostream &os);
   
public:
   CRC32Digest();
   
   void initDigest(void);
   
   inline unsigned getSize(void) {return 4;}
   
   unsigned getDigest(unsigned char *buffer);
   
   void putDigest(const unsigned char *buffer, unsigned length);
};

/**
 * A md5 collection/computation accululator class.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short md5 hash accumulation.
 */
class __EXPORT MD5Digest : public Digest
{
private:
	unsigned long state[4];
	unsigned long count[2];
	unsigned char buf[64];
	unsigned bpos;
	unsigned char md5[16];
	bool updated;

protected:
	int overflow(int c);

	void update(void);

	void commit(void);

	std::ostream &strDigest(std::ostream &os);

public:
	MD5Digest();

	void initDigest(void);

	inline unsigned getSize(void)
		{return 16;};

	unsigned getDigest(unsigned char *buffer);

	void putDigest(const unsigned char *buffer, unsigned len);
};

#ifdef	COMMON_STD_EXCEPTION
/**
 * DigestException
 *
 * Exception classes that pertain to errors when making or otherwise
 * working with digests.
 *
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short Exceptions involving digests.
 */
class __EXPORT DigestException : public Exception {
public:
	DigestException(const String &str) : Exception(str) {};
};
#endif

#ifdef	HAVE_64_BITS 
/**
 * SHATumbler
 *
 * Class used by the SHA Digest Classes.
 *
 * Represents a "tumbler" group, similar to a row in a combination
 * lock. Each part is made to roll-over, its size dependent upon
 * int_type.
 *
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short SHA Helper Class */
template <class int_type>
class __EXPORT SHATumbler {
public:
	SHATumbler(int);

	SHATumbler(const SHATumbler&);
	SHATumbler& operator=(const SHATumbler&);

	int_type& operator[](int);

	~SHATumbler();
  
	SHATumbler operator+(const SHATumbler& addend) const;
	SHATumbler& operator+=(const SHATumbler& addend);
	std::ostream & toString(std::ostream & os);

	friend std::ostream &operator<<(std::ostream &os, SHATumbler<int_type>& ia)
	{return ia.toString(os);};

	unsigned getSize();
	unsigned placeInBuffer(unsigned char *);
    
private:
	int_type * h;
	int size;

	char * tmp_buff;

	static char format_string[];
};

/**
 * SHAConstant
 *
 * Note that this constant is set in sha.cpp and that each constant in
 * the array is 64 bits wide. The constants themselves pertain to both
 * SHA-256 and SHA-512, the only difference is that SHA-256 expects
 * her constants to be 32 bits wide and only 64 are needed.
 *
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short Contains SHA constants
 */
class __EXPORT SHAConstant {
protected:
	const static uint64 K[];
};

/**
 * SHADigest
 *
 * Generic Class that is the base class of the various SHA*Digest
 * classes.
 *
 * uint_type == The "Unsigned Integer Type" which is large enough to
 * hold the total length of a given message in bits. For SHA1 and
 * SHA256, this is 64 bits, so we use "uint64";
 *
 * "blockSizeInBytes" == various SHA digests use different message
 * size blocks - the basic units in which to split a message up in.
 * For example, SHA1 and SHA256 use 512 bit blocks, and so in
 * bytes this is 64.
 *
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short Base class for the SHA*Digests
 */
template <class uint_type, unsigned blockSizeInBytes>
class __EXPORT SHADigest : public Digest {
private:
	uint_type totalLengthInBits;

	void copyTempBlock(const SHADigest &);

protected:
	unsigned char tempBlock[blockSizeInBytes];
	void initDigest(void);

	virtual void processBlock(const unsigned char * buffer) = 0;
	void padBuffer(unsigned char * buffer);

	bool completed;

	SHADigest();
	SHADigest(const SHADigest & other);
	SHADigest & operator=(const SHADigest & other);

public:
	unsigned getSize(void) = 0;
	void putDigest(const unsigned char * buffer, unsigned length) 
		THROWS(DigestException);
	std::ostream & strDigest(std::ostream &os) = 0;

};

/**
 * SHA64DigestHelper
 *
 * Simply a base class for the SHA*Digest classes that share
 * functionality after their "unsigned int" type and block size are
 * known (uint64 and 64)
 *
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short Base class for the 64-bit wide SHA*Digests.
 */
class __EXPORT SHA64DigestHelper: public SHADigest<uint64, 64> {
protected:
	SHATumbler<uint32> h;
	SHATumbler<uint32> a;

	SHA64DigestHelper(unsigned);
	SHATumbler<uint32> getDigest();

public:
	unsigned getDigest(unsigned char * buffer) 
	{ return getDigest().placeInBuffer(buffer); }
	std::ostream & strDigest(std::ostream & os);

	SHA64DigestHelper(const SHA64DigestHelper & other);
	SHA64DigestHelper & operator=(const SHA64DigestHelper & other);
};

/**
 * SHA1Digest
 *
 * SHA1Digest proper.
 *
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short SHA-1 Digest Implementation
 */
class __EXPORT SHA1Digest : public SHA64DigestHelper {
protected:
	void processBlock(const unsigned char * buffer);

public:
	SHA1Digest();
	void initDigest();
	unsigned getSize() { return 20; }
	SHA1Digest(const SHA1Digest & other);
	SHA1Digest & operator=(const SHA1Digest & other)
	{ *((SHA64DigestHelper*)this) = other; return *this; }
};

/**
 * SHA256Digest
 * @author Elizabeth Barham <lizzy@soggytrousers.net>
 * @short SHA-256 Digest Implementation
 */
class __EXPORT SHA256Digest : public SHA64DigestHelper, public SHAConstant {
protected:
	void processBlock(const unsigned char * buffer);

public:
	SHA256Digest();
	SHA256Digest(const SHA256Digest &);
	void initDigest();
	unsigned getSize() { return 32; }
	SHA256Digest & operator=(const SHA256Digest & other)
	{ *((SHA64DigestHelper*)this) = other; return *this; }
};
#endif

#ifdef	CCXX_NAMESPACES
}
#endif

#endif
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */

