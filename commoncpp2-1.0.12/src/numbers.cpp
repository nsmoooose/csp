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
#include <cc++/numbers.h>
#include <cstdlib>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

Number::Number(char *buf, unsigned width)
{
	if(width > 10)
		width = 10;
	if(width < 1)
		width = 1;
	size = width;

	buffer = buf;
}

long Number::getValue(void)
{
	int count = size;
	bool sign = false;
	long ret = 0;
	char *bp = buffer;

	if(*bp == '-')
	{
		--count;
		++bp;
		sign = true;
	}
	else if(*bp == '+')
	{
		--count;
		++bp;
	}
	while(count && *bp >='0' && *bp <='9')
	{
		ret = ret * 10l + (*bp - '0');
		--count;
		++bp;
	}
	
	if(sign)
		ret = -ret;
	return ret;
}	

void Number::setValue(long value)
{
	int count = size;
	char *bp = buffer;
	long max = 1;
	int exp;
	bool z = false;
	
	if(value < 0)
	{
		value = -value;
		--count;
		*(bp++) = '-';
	}
	
	exp = count;
	while(--exp)
		max *= 10;

	while(max)
	{
		if(value >= max || z)
		{
			--count;
			*(bp++) = '0' + (value / max);
		}
		if(value >= max)
		{
			z = true;
			value -= (value / max) * max;
		}
		max /= 10;
	}
	while(count-- && *bp >= '0' && *bp <='9')
		*(bp++) = ' ';
}

long Number::operator=(long value)
{
	setValue(value);
	return value;
}

long Number::operator+=(long value)
{
	value = getValue() + value;
	setValue(value);
	return value;
}

long Number::operator-=(long value)
{
	value = getValue() - value;
	setValue(value);
	return value;
}

long Number::operator--()
{
	long val = getValue();
	setValue(--val);
	return val;
}

long Number::operator++()
{
	long val = getValue();
	setValue(++val);
	return val;
}

int Number::operator==(Number &num)
{
	return getValue() == num.getValue();
}

int Number::operator!=(Number &num)
{
	return getValue() != num.getValue();
}

int Number::operator<(Number &num)
{
	return getValue() < num.getValue();
}

int Number::operator<=(Number &num)
{
	return getValue() <= num.getValue();
}

int Number::operator>(Number &num)
{
	return getValue() > num.getValue();
}

int Number::operator>=(Number &num)
{
	return getValue() >= num.getValue();
}
 
ZNumber::ZNumber(char *buf, unsigned size) :
Number(buf, size)
{}

void ZNumber::setValue(long value)
{
	int count = size;
	char *bp = buffer;
	long max = 1;
	int exp;
	
	if(value < 0)
	{
		value = -value;
		--count;
		*(bp++) = '-';
	}
	
	exp = count;
	while(--exp)
		max *= 10;

	while(max)
	{
		--count;
		*(bp++) = '0' + (value / max);
		value -= (value / max) * max;
		max /= 10;
	}
}
 
long ZNumber::operator=(long value)
{
	setValue(value);
	return value;
}

long operator+(Number &num, long val)
{
	return num.getValue() + val;
}

long operator+(long val, Number &num)
{
	return num.getValue() + val;
}

long operator-(Number &num, long val)
{
	return num.getValue() - val;
}

long operator-(long val, Number &num)
{
	return num.getValue() - val;
}

#ifdef	CCXX_NAMESPACES
};
#endif
