/* -*- C++ -*- */
// Copyright (C) 1999-2002 Open Source Telecom Corporation.
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
#include <iomanip>

#ifdef	WIN32
#include <io.h>
#endif

#define LONGEST_FORMAT_STRING_POSSIBLE_IN_CHARS 8

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

/* In case anyone is wondering why SHA-512 and SHA-384 are not
 * implemented, it is the better way to make them is to use an 128-bit
 * unsigned integer. To my understanding, the gcc 3.* has either
 * introduced uint128_t or shall introduce it. When it does, something
 * similar to SHA64DigestHelper can easily be made, perhaps:
 *
 * "class SHA128DigestHelper : public SHADigest<uint128_t, 128>"
 *
 * -- Elizabeth Barham, November 20, 2002, Fort Worth, TX, USA
 */

/**
 * DigestException
 */
DigestException::DigestException(const std::string & what_arg) throw() :
  Exception(what_arg)
{ }

DigestException::~DigestException() throw()
{ }


/** 
 * SHAConstant::K
 *
 * Defined in "Descriptions of SHA-256, SHA-384, SHA-512",
 * <http://csrc.nist.gov/encryption/shs/sha256-384-512.pdf>, and is 32
 * bits wide for for SHA-256 but 64 bits wide for SHA-384 and SHA-512.
 */

/* note to use K32_ADJUST when attempting to access K with a 32 bit
 * integer.
 */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define K32_ADJUST 1
#else
#define K32_ADJUST 0
#endif

const uint64 SHAConstant::K[] = {
#if defined(WIN32) && !defined(__MINGW32__)
	0x428a2f98d728ae22,
	0x7137449123ef65cd,
	0xb5c0fbcfec4d3b2f,
	0xe9b5dba58189dbbc,
	0x3956c25bf348b538,
	0x59f111f1b605d019,
	0x923f82a4af194f9b,
	0xab1c5ed5da6d8118,
	0xd807aa98a3030242,
	0x12835b0145706fbe,
	0x243185be4ee4b28c,
	0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f,
	0x80deb1fe3b1696b1,
	0x9bdc06a725c71235,
	0xc19bf174cf692694,
	0xe49b69c19ef14ad2,
	0xefbe4786384f25e3,
	0x0fc19dc68b8cd5b5,
	0x240ca1cc77ac9c65,
	0x2de92c6f592b0275,
	0x4a7484aa6ea6e483,
	0x5cb0a9dcbd41fbd4,
	0x76f988da831153b5,
	0x983e5152ee66dfab,
	0xa831c66d2db43210,
	0xb00327c898fb213f,
	0xbf597fc7beef0ee4,
	0xc6e00bf33da88fc2,
	0xd5a79147930aa725,
	0x06ca6351e003826f,
	0x142929670a0e6e70,
	0x27b70a8546d22ffc,
	0x2e1b21385c26c926,
	0x4d2c6dfc5ac42aed,
	0x53380d139d95b3df,
	0x650a73548baf63de,
	0x766a0abb3c77b2a8,
	0x81c2c92e47edaee6,
	0x92722c851482353b,
	0xa2bfe8a14cf10364,
	0xa81a664bbc423001,
	0xc24b8b70d0f89791,
	0xc76c51a30654be30,
	0xd192e819d6ef5218,
	0xd69906245565a910,
	0xf40e35855771202a,
	0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8,
	0x1e376c085141ab53,
	0x2748774cdf8eeb99,
	0x34b0bcb5e19b48a8,
	0x391c0cb3c5c95a63,
	0x4ed8aa4ae3418acb,
	0x5b9cca4f7763e373,
	0x682e6ff3d6b2b8a3,
	0x748f82ee5defb2fc,
	0x78a5636f43172f60,
	0x84c87814a1f0ab72,
	0x8cc702081a6439ec,
	0x90befffa23631e28,
	0xa4506cebde82bde9,
	0xbef9a3f7b2c67915,
	0xc67178f2e372532b,
	0xca273eceea26619c,
	0xd186b8c721c0c207,
	0xeada7dd6cde0eb1e,
	0xf57d4f7fee6ed178,
	0x06f067aa72176fba,
	0x0a637dc5a2c898a6,
	0x113f9804bef90dae,
	0x1b710b35131c471b,
	0x28db77f523047d84,
	0x32caab7b40c72493,
	0x3c9ebe0a15c9bebc,
	0x431d67c49c100d4c,
	0x4cc5d4becb3e42b6,
	0x597f299cfc657e2a,
	0x5fcb6fab3ad6faec,
	0x6c44198c4a475817
#else
	0x428a2f98d728ae22LL,
	0x7137449123ef65cdLL,
	0xb5c0fbcfec4d3b2fLL,
	0xe9b5dba58189dbbcLL,
	0x3956c25bf348b538LL,
	0x59f111f1b605d019LL,
	0x923f82a4af194f9bLL,
	0xab1c5ed5da6d8118LL,
	0xd807aa98a3030242LL,
	0x12835b0145706fbeLL,
	0x243185be4ee4b28cLL,
	0x550c7dc3d5ffb4e2LL,
	0x72be5d74f27b896fLL,
	0x80deb1fe3b1696b1LL,
	0x9bdc06a725c71235LL,
	0xc19bf174cf692694LL,
	0xe49b69c19ef14ad2LL,
	0xefbe4786384f25e3LL,
	0x0fc19dc68b8cd5b5LL,
	0x240ca1cc77ac9c65LL,
	0x2de92c6f592b0275LL,
	0x4a7484aa6ea6e483LL,
	0x5cb0a9dcbd41fbd4LL,
	0x76f988da831153b5LL,
	0x983e5152ee66dfabLL,
	0xa831c66d2db43210LL,
	0xb00327c898fb213fLL,
	0xbf597fc7beef0ee4LL,
	0xc6e00bf33da88fc2LL,
	0xd5a79147930aa725LL,
	0x06ca6351e003826fLL,
	0x142929670a0e6e70LL,
	0x27b70a8546d22ffcLL,
	0x2e1b21385c26c926LL,
	0x4d2c6dfc5ac42aedLL,
	0x53380d139d95b3dfLL,
	0x650a73548baf63deLL,
	0x766a0abb3c77b2a8LL,
	0x81c2c92e47edaee6LL,
	0x92722c851482353bLL,
	0xa2bfe8a14cf10364LL,
	0xa81a664bbc423001LL,
	0xc24b8b70d0f89791LL,
	0xc76c51a30654be30LL,
	0xd192e819d6ef5218LL,
	0xd69906245565a910LL,
	0xf40e35855771202aLL,
	0x106aa07032bbd1b8LL,
	0x19a4c116b8d2d0c8LL,
	0x1e376c085141ab53LL,
	0x2748774cdf8eeb99LL,
	0x34b0bcb5e19b48a8LL,
	0x391c0cb3c5c95a63LL,
	0x4ed8aa4ae3418acbLL,
	0x5b9cca4f7763e373LL,
	0x682e6ff3d6b2b8a3LL,
	0x748f82ee5defb2fcLL,
	0x78a5636f43172f60LL,
	0x84c87814a1f0ab72LL,
	0x8cc702081a6439ecLL,
	0x90befffa23631e28LL,
	0xa4506cebde82bde9LL,
	0xbef9a3f7b2c67915LL,
	0xc67178f2e372532bLL,
	0xca273eceea26619cLL,
	0xd186b8c721c0c207LL,
	0xeada7dd6cde0eb1eLL,
	0xf57d4f7fee6ed178LL,
	0x06f067aa72176fbaLL,
	0x0a637dc5a2c898a6LL,
	0x113f9804bef90daeLL,
	0x1b710b35131c471bLL,
	0x28db77f523047d84LL,
	0x32caab7b40c72493LL,
	0x3c9ebe0a15c9bebcLL,
	0x431d67c49c100d4cLL,
	0x4cc5d4becb3e42b6LL,
	0x597f299cfc657e2aLL,
	0x5fcb6fab3ad6faecLL,
	0x6c44198c4a475817LL
#endif
};


/* *********************************************************************
   SHATumbler

   The idea of an odometer, and as each part of the message is read
   in, the odometer is "tumbled" forward, numbers looping, etc. so
   that we receive a nice hash when we are done.
   ********************************************************************* */

template <class int_type>
SHATumbler<int_type>::SHATumbler(int s): size(s)
{
	h = new int_type[size];

	//////////////////////////////////////////////////////////////
	// we next need to generate a temporary buffer in which to
	// place the string representing a single digit of this object.
	//
	// Its total size is determined via sizeof(int_type) in that
	// the character representation of int_type is twice its
	// physical size, e.g.
	//
	//       char c = 'z'
	//       sizeof(char) == 1; (normally)
	//       in hexadecimal, 'z' == 0x7a, two bytes
	//                              or sizeof(char) * 2
	//////////////////////////////////////////////////////////////

	int bytes_needed_for_rep = sizeof(int_type) * 2;
	tmp_buff = new char[bytes_needed_for_rep + 1];

	if(*format_string == 0) {
		sprintf(format_string, "%%0%d%sx", bytes_needed_for_rep, 
			(sizeof(int_type) < sizeof(long)) ? "" : (sizeof(int_type) == sizeof(long)) ? "l" : "ll");
	}
}

template <class int_type>
char SHATumbler<int_type>::format_string[LONGEST_FORMAT_STRING_POSSIBLE_IN_CHARS] = "";

template <class int_type>
SHATumbler<int_type>::SHATumbler(const SHATumbler<int_type>& copy)
{
	size = copy.size;
	h = new int_type[size];
	memcpy((void*)h,(void*)copy.h,sizeof(int_type) * size);
	tmp_buff = new char[sizeof(int_type) * 2 + 1];
}

template <class int_type>
SHATumbler<int_type> & SHATumbler<int_type>::operator=(const SHATumbler<int_type> & assign)
{
	memcpy((void*)h,(void*)assign.h,sizeof(int_type) * size);
	return *this;
}

template <class int_type>
int_type & SHATumbler<int_type>::operator[](int x)
{
	return h[x];
}

template <class int_type>
SHATumbler<int_type>::~SHATumbler()
{
	delete[] h;
	delete[] tmp_buff;
}

template <class int_type>
SHATumbler<int_type> SHATumbler<int_type>::operator+(const SHATumbler<int_type>& addend) const
{
	SHATumbler<int_type> rv = *this;
	rv += addend;
	return(rv);
}

template <class int_type>
SHATumbler<int_type>& SHATumbler<int_type>::operator+=(const SHATumbler<int_type>& addend)
{
	for(int i = 0; i < size; i++) {
		h[i] += addend.h[i];
	}
	return(*this);
}

template <class int_type>
std::ostream & SHATumbler<int_type>::toString(std::ostream & os)
{
	unsigned i;
	for(i = 0 ; i < (unsigned)size ; i++) {
		
#ifdef DEBUG_SHATUMBLER
		if(i != 0)
			os << " ";
#endif
		sprintf(tmp_buff, const_cast<const char *>(format_string), h[i]);
		os << tmp_buff;
	}
	return(os);
}

template <class int_type>
unsigned SHATumbler<int_type>::placeInBuffer(unsigned char * buffer)
{
	unsigned memsize = getSize();
	memcpy((void*) buffer, (void*) h, memsize);
	return(memsize);
}

template <class int_type>
unsigned SHATumbler<int_type>::getSize()
{
	unsigned memsize = sizeof(int_type) * size;
	return(memsize);
}

/* *********************************************************************
   SHADigest
   ********************************************************************* */
template <class uint_type, unsigned blockSizeInBytes>
SHADigest<uint_type, blockSizeInBytes>::SHADigest()
{
	initDigest();
}


template <class uint_type, unsigned blockSizeInBytes>
void SHADigest<uint_type, blockSizeInBytes>::padBuffer(unsigned char * buffer)
{
	int lengthOfTotalLength     = blockSizeInBytes / 8;
	uint_type totalLengthInBytes = totalLengthInBits / 8;
	int remainder = totalLengthInBytes % blockSizeInBytes;

	int fillTo;

	if( ((int)blockSizeInBytes - ( remainder + 1 )) >= lengthOfTotalLength ) {
		fillTo = (blockSizeInBytes - lengthOfTotalLength);
	} else {
		fillTo = blockSizeInBytes;
	}

	buffer[remainder++] = 0x80;

	do
	{
		memset(buffer + remainder, 0, fillTo - remainder);
		if(fillTo != (int)blockSizeInBytes) {
			*((uint_type*)(buffer + fillTo)) = totalLengthInBits;
		}
		processBlock(buffer);
		remainder = 0;
		fillTo -= lengthOfTotalLength;
	} while(fillTo >= (int)(blockSizeInBytes - lengthOfTotalLength));
	completed = true;
}

template <class uint_type, unsigned blockSizeInBytes>
void SHADigest<uint_type, blockSizeInBytes>::initDigest()
{
	totalLengthInBits = 0;
	completed = false;
}

template <class uint_type, unsigned blockSizeInBytes>
void SHADigest<uint_type, blockSizeInBytes>::putDigest(const unsigned char * buffer, unsigned length) 
  throw(DigestException)
{
	unsigned i;

	if(completed) {
		throw(DigestException("Corrupted Digest!"));
	}
  
	if( length > (totalLengthInBits / 8) + length ) {
		throw(DigestException("Digest Length Overflow!"));
	}

	// fill in tempBlock if necessarry, then process it...

	uint_type bytesInTempBlock;

	if(bytesInTempBlock = (totalLengthInBits/8) % blockSizeInBytes) {
		unsigned bytesLeftInTempBlock = blockSizeInBytes - 
			bytesInTempBlock;
		unsigned numToCopy;

		if(length < bytesLeftInTempBlock) {
			numToCopy = length;
		} else {
			numToCopy = bytesLeftInTempBlock;
		}

		memcpy(tempBlock + bytesInTempBlock, buffer, numToCopy);
		length -= numToCopy;
	}
	
	unsigned numBlocksToProcess = length / blockSizeInBytes;
	unsigned numBytesToStore   = length % blockSizeInBytes;

	// process incoming stream
	for(i = 0; i < numBlocksToProcess; i++) {
		processBlock(buffer + (i * blockSizeInBytes));
	}

  // place any data that could not be processed into tempBlock
	for(i = 0; i < numBytesToStore; i++) {
		tempBlock[i] = (buffer + 
				(numBlocksToProcess * blockSizeInBytes))[i];
	}
	totalLengthInBits += 8 * (bytesInTempBlock +  
				  (numBlocksToProcess * blockSizeInBytes) + 
				  numBytesToStore);
}

template <class uint_type, unsigned blockSizeInBytes>
SHADigest<uint_type, blockSizeInBytes>::SHADigest(const SHADigest<uint_type, blockSizeInBytes> & other) :
  totalLengthInBits(other.totalLengthInBits),
  completed(other.completed)
{
	copyTempBlock(other);
}

template <class uint_type, unsigned blockSizeInBytes>
SHADigest<uint_type, blockSizeInBytes> &  SHADigest<uint_type, blockSizeInBytes>::operator=(const SHADigest & other) {
	if(this == &other) {
		return *this;
	}

	this->completed = other.completed;
	this->totalLengthInBits = other.totalLengthInBits;
	copyTempBlock(other);
	return *this;
}

template <class uint_type, unsigned blockSizeInBytes>
void  SHADigest<uint_type, blockSizeInBytes>::copyTempBlock(const SHADigest & other)
{
	if(uint_type remainder = 
	   ( (totalLengthInBits / 8) % blockSizeInBytes)){
		memcpy( (void*)tempBlock, (void*)other.tempBlock,
			remainder * (sizeof(unsigned char)));
	}
}

/* *********************************************************************
   SHA64DigestHelper

   Base class for SHA-1 and SHA-256 which have much in common.
   ********************************************************************* */
SHA64DigestHelper::SHA64DigestHelper(unsigned tumblerSize):
	h(SHATumbler<uint32>(tumblerSize)), 
	a(SHATumbler<uint32>(tumblerSize))
{ }

SHATumbler<uint32> SHA64DigestHelper::getDigest() {
	if(!completed) {
		padBuffer(tempBlock);
	}
	return h;
}

std::ostream & SHA64DigestHelper::strDigest(std::ostream & os) {
	SHATumbler<uint32> hCopy = getDigest();
	os << hCopy;
	return(os);
}

SHA64DigestHelper::SHA64DigestHelper(const SHA64DigestHelper & other) :
	SHADigest<uint64, 64>(other),
	h(other.h),
	a(other.a)
{ }

SHA64DigestHelper & SHA64DigestHelper::operator=(const SHA64DigestHelper & other)
{
	if(this == &other) {
		return(*this);
	}

	SHADigest<uint64, 64>::operator=(other);

	this->h = other.h;
	this->a = other.a;

	return *this;
}

/* *********************************************************************
   SHA1Digest
   ********************************************************************* */
SHA1Digest::SHA1Digest(): SHA64DigestHelper(5)
{
  initDigest();
}

void SHA1Digest::initDigest()
{
	h[0] = 0x67452301;
	h[1] = 0xefcdab89;
	h[2] = 0x98badcfe;
	h[3] = 0x10325476;
	h[4] = 0xc3d2e1f0;
	SHADigest<uint64, 64>::initDigest();
}

SHA1Digest::SHA1Digest(const SHA1Digest & other): SHA64DigestHelper(other)
{ }

#define S(n,X) ( ( ( X ) << ( n ) ) | ( ( X ) >> ( 32 - n ) ) )

void SHA1Digest::processBlock(const unsigned char * buffer)
{
	uint32 W[80];
	unsigned t;

	memcpy((void*)W, (void*)buffer, sizeof(uint32) * 16);
	for(t = 16; t < 80; t++) {
		W[t] = S(1,(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]));
	}
	a = h;
	uint32 temp;
	for(t = 0; t < 80; t++) {

		/* The following switch statement combines the two
		   functions f(a,b,c) and K(t) as in the specs. */

		switch(t/20) {
		case(0):
			temp = ((a[1] & a[2]) | (~a[1] & a[3])) + 0x5a827999;
			break;
		case(1):
			temp = ((a[1] ^ a[2] ^ a[3]) + 0x6ed9eba1);
			break;
		case(2):
			temp = ((a[1] & a[2]) | (a[1] & a[3]) | (a[2] & a[3])) + 0x8f1bbcdc;
			break;
		default:
			temp = (a[1] ^ a[2] ^ a[3]) + 0xca62c1d6;
		}
		temp += S(5,a[0]) + a[4] + W[t];
		a[4] = a[3]; /* a[4] == "e" as in the specs */
		a[3] = a[2]; /* a[3] == "d", etc */
		a[2] = S(30,a[1]);
		a[1] = a[0];
		a[0] = temp;

#ifdef DEBUG_SHA1
		std::cerr << std::setw(2) << std::dec << t << ": " << a << 
			std::endl;
#endif
    
	}
	h += a;
}

/* *********************************************************************
   SHA256Digest
   ********************************************************************* */
SHA256Digest::SHA256Digest(): SHA64DigestHelper(8)
{
  initDigest();
}

SHA256Digest::SHA256Digest(const SHA256Digest & other):
  SHA64DigestHelper(other)
{ }


void SHA256Digest::initDigest() {
	h[0] = 0x6a09e667;
	h[1] = 0xbb67ae85;
	h[2] = 0x3c6ef372;
	h[3] = 0xa54ff53a;
	h[4] = 0x510e527f;
	h[5] = 0x9b05688c;
	h[6] = 0x1f83d9ab;
	h[7] = 0x5be0cd19;
	SHADigest<uint64, 64>::initDigest();
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/* RR == "right rotation by n bits" (Sn)                             */
/* RS == "right shift by n byts"    (Rn)                             */
/* (see p. 2, "Descriptions of SHA-256, SHA-384, SHA-512"            */
/*   <http://csrc.nist.gov/encryption/shs/sha256-384-512.pdf>)       */

#define RR(n,X) ( ( ( X ) >> n ) | ( ( X ) << ( 32 - n ) ) )
#define RS(n,X) ( ( ( X ) >> n ) )

#define Ch(x,y,z) ( ( ( x ) & ( y ) ) ^ (  ( ~ x  ) & ( z ) ) )
#define Maj(x,y,z) ( ( ( x ) & ( y ) ) ^ ( ( x ) & ( z ) ) ^ ( ( y ) & ( z ) ) )
#define E0(x) ( RR(2,x) ^ RR(13,x) ^ RR(22,x) )
#define E1(x) ( RR(6,x) ^ RR(11,x) ^ RR(25,x) )
#define o0(x) ( RR(7,x) ^ RR(18,x) ^ RS(3,x) )
#define o1(x) ( RR(17,x) ^ RR(19,x) ^ RS(10,x) )

void SHA256Digest::processBlock(const unsigned char * buffer) {
	uint32 t1, t2;
	uint32 W[64];
	uint32 * M = (uint32*)buffer;
	uint32 * K32 = (uint32*)SHAConstant::K;

	a = h;
	for(int j = 0; j < 64 ; j++) {
		if(j < 16) {
			W[j] = M[j];
		} else {
			W[j] = o1(W[j-2]) + W[j-7] + o0(W[j-15]) + W[j-16];
		}
		
		t1 = a[7] + E1(a[4]) + Ch(a[4],a[5],a[6]) + 
			K32[(j * 2) + K32_ADJUST] + W[j];
		t2 = E0(a[0]) + Maj(a[0],a[1],a[2]);
    
		a[7] = a[6];
		a[6] = a[5];
		a[5] = a[4];
		a[4] = a[3] + t1;
		a[3] = a[2];
		a[2] = a[1];
		a[1] = a[0];
		a[0] = t1 + t2;
#ifdef DEBUG_SHA256
		std::cerr << std::setw(2) << std::dec << j << std::hex 
			  << ": " << a << std::endl;
#endif
	}
	h += a;
}

#ifdef	CCXX_NAMESPACES
};
#endif	
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
