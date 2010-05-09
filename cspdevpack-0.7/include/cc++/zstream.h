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
 * @file zstream.h
 * @short compressed stream operations.
 **/

#ifndef	CCXX_ZSTREAM_H_
#define	CCXX_ZSTREAM_H_

#ifndef	CCXX_MISSING_H_
#include <cc++/missing.h>
#endif

#ifndef CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#ifndef	CCXX_STRING_H_
#include <cc++/string.h>
#endif

#ifndef	CCXX_EXCEPTION_H_
#include <cc++/exception.h>
#endif

#include <zlib.h>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifdef	COMMON_STD_EXCEPTION

class __EXPORT IOZException : public IOException
{
public:
	IOZException(const String &str) : IOException(str) {};
};

#endif

class __EXPORT IZStream : protected std::streambuf, public std::istream
{
private:
	gzFile fp;
	int doallocate();
	bool throwflag;

protected:
	size_t bufsize;
	char *gbuf;

	void allocate(size_t size);

	int underflow();

	int uflow();

public:
	IZStream(bool throwflag = false);
	IZStream(const char *name, size_t size = 512, bool tf = false);

	bool isOpen(void);

	void close(void);

	virtual ~IZStream();
	
	void open(const char *name, size_t size = 512);

	inline size_t getBufferSize(void)
		{return bufsize;};
};

class __EXPORT OZStream : protected std::streambuf, public std::ostream
{
private:
	gzFile fp;
	int doallocate();
	bool throwflag;

protected:
	size_t bufsize;
	char *pbuf;

	void allocate(size_t size);

	int overflow(int ch);

public:
	OZStream(bool throwflag = false);
	OZStream(const char *name, int level = Z_DEFAULT_COMPRESSION, size_t size = 512, bool tf = false);

	bool isOpen(void);

	void close(void);

	virtual ~OZStream();
	
	void open(const char *name, int level = Z_DEFAULT_COMPRESSION, size_t size = 512);

	inline size_t getBufferSize(void)
		{return bufsize;};
};

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
