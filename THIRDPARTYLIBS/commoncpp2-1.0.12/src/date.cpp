// Copyright (C) 1999-2003 Open Source Telecom Corporation.
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
#include <cc++/thread.h>
#include <cc++/exception.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cmath>

#ifdef	WIN32
#include "time.h"
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
using std::string;
using namespace std;
#ifdef __BORLANDC__
using std::time_t;
using std::tm;
using std::localtime;
#endif
#endif

Date::Date()
{
	time_t now;
	struct tm *dt;
	time(&now);
	dt = localtime(&now);

	toJulian(dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday);
}

Date::Date(struct tm *dt)
{
	toJulian(dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday);
}

Date::Date(time_t tm)
{
	struct tm *dt = localtime(&tm);
	toJulian(dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday);
}

Date::Date(char *str, size_t size)
{
	setDate(str, size);
}

void Date::setDate(const char *str, size_t size)
{
	time_t now;
	struct tm *dt;
	int year = 0;
	const char *mstr = str;
	const char *dstr = str;

	if(!size)
		size = strlen(str);
//0000
	if(size == 4) 
	{
#ifdef DEBUG
		cout << "Date::SetDate(): 0000" << endl;
#endif
		time(&now);
		dt = localtime(&now);
		year = dt->tm_year + 1900;
		mstr = str;
		dstr = str + 2;
	}		
//00/00
	else if(size == 5)
	{
#ifdef DEBUG
		cout << "Date::SetDate(): 00/00" << endl;
#endif
		time(&now);
		dt = localtime(&now);
		year = dt->tm_year + 1900;
		mstr = str;
		dstr = str + 3;
	}
//000000
	else if(size == 6)
	{
#ifdef DEBUG
		cout << "Date::SetDate(): 000000" << endl;
#endif
		time(&now);
		dt = localtime(&now);
		ZNumber nyear((char*)str, 2);
		year = ((dt->tm_year + 1900) / 100) * 100 + nyear();
		mstr = str + 2;
		dstr = str + 4;
	}
//00000000
	else if(size == 8 && str[2] >= '0' && str[2] <= '9' && str[5] >= '0' && str[5] <= '9')
	{
#ifdef DEBUG
		cout << "Date::SetDate(): 00000000" << endl;
#endif
		ZNumber nyear((char*)str, 4);
		year = nyear();
		mstr = str + 4;
		dstr = str + 6;
	}
//00/00/00
	else if(size == 8)
	{
#ifdef DEBUG
		cout << "Date::SetDate(): 00/00/00" << endl;
#endif
		time(&now);
		dt = localtime(&now);
		ZNumber nyear((char*)str, 2);
		year = ((dt->tm_year + 1900) / 100) * 100 + nyear();
		mstr = str + 3;
		dstr = str + 6;
	}

//0000/00/00
	else if(size == 10)
	{
#ifdef DEBUG
		cout << "Date::SetDate(): 0000/00/00" << endl;
#endif
		ZNumber nyear((char*)str, 4);
		year = nyear();
		mstr = str + 5;
		dstr = str + 8;
	}
	else if(getException() == Thread::throwObject)
	{
		throw this;
	}
	else if(getException() == Thread::throwException)
	{
		throw Exception("Date::setDate(): Invalid date.");
	}	    	
	else {
		julian = 0x7fffffffl;
		return;
	}
	
	ZNumber nmonth((char*)mstr, 2);
	ZNumber nday((char*)dstr, 2);
	toJulian(year, nmonth(), nday());
}

Date::Date(int year, unsigned month, unsigned day)
{
	toJulian(year, month, day);
};

bool Date::isValid(void) const
{
	if(julian == 0x7fffffffl)
		return false;
	return true;
}

char *Date::getDate(char *buf) const
{
	fromJulian(buf);
	return buf;
}

time_t Date::getDate(tm* dt) const
{
	char buf[9];
	memset(dt, 0, sizeof(tm));
	fromJulian(buf);
	Number nyear(buf, 4);
	Number nmonth(buf + 4, 2);
	Number nday(buf + 6, 2);
	
	dt->tm_year = nyear() - 1900;
	dt->tm_mon = nmonth() - 1;
	dt->tm_mday = nday();

	return mktime(dt); // to fill in day of week etc.
}

time_t Date::getDate(void) const
{
	struct tm dt;
	return getDate(&dt);
}

int Date::getYear(void) const
{
	char buf[9];
	fromJulian(buf);
	Number num(buf, 4);
	return num();
}

unsigned Date::getMonth(void) const
{
	char buf[9];
	fromJulian(buf);
	Number num(buf + 4, 2);
	return num();
}

unsigned Date::getDay(void) const
{
	char buf[9];
	fromJulian(buf);
	Number num(buf + 6, 2);
	return num();
}

unsigned Date::getDayOfWeek(void) const
{
	return (unsigned)((julian + 1l) % 7l);
}

string Date::operator()() const
{
	char buf[9];

	fromJulian(buf);
	string date(buf);

	return date;
}

long Date::getValue(void) const
{
	char buf[9];
	fromJulian(buf);
	return atol(buf);
}

Date& Date::operator++()
{
	++julian;
	update();
	return *this;
}

Date& Date::operator--()
{
	--julian;
	update();
	return *this;
}

Date& Date::operator+=(long val)
{
	julian += val;
	update();
	return *this;
}

Date& Date::operator-=(long val)
{
	julian -= val;
	update();
	return *this;
}

int Date::operator==(Date &d)
{
	return julian == d.julian;
}

int Date::operator!=(Date &d)
{
	return julian != d.julian;
}

int Date::operator<(Date &d)
{
	return julian < d.julian;
}

int Date::operator<=(Date &d)
{
	return julian <= d.julian;
}

int Date::operator>(Date &d)
{
	return julian > d.julian;
}

int Date::operator>=(Date &d)
{
	return julian >= d.julian;
}

void Date::toJulian(long year, long month, long day)
{
	julian = 0x7fffffffl;
	
	if(month < 1 || month > 12 || day < 1 || day > 31 || year == 0) {
		if(getException() == Thread::throwObject) {
			throw this;
		}
		else if(getException() == Thread::throwException) {
			throw Exception("Date::toJulian(): Invalid date.");
		}
		return;
	}

	if(year < 0)
		year--;

	julian = day - 32075l +
		1461l * (year + 4800l + ( month - 14l) / 12l) / 4l +
		367l * (month - 2l - (month - 14l) / 12l * 12l) / 12l -
		3l * ((year + 4900l + (month - 14l) / 12l) / 100l) / 4l;
}

void Date::fromJulian(char *buffer) const
{
// The following conversion algorithm is due to 
// Henry F. Fliegel and Thomas C. Van Flandern:

	ZNumber nyear(buffer, 4);
	ZNumber nmonth(buffer + 4, 2);
	ZNumber nday(buffer + 6, 2);

	double i, j, k, l, n;

	l = julian + 68569.0;
	n = int( 4 * l / 146097.0);
	l = l - int( (146097.0 * n + 3)/ 4 );
	i = int( 4000.0 * (l+1)/1461001.0);
	l = l - int(1461.0*i/4.0) + 31.0;
	j = int( 80 * l/2447.0);
	k = l - int( 2447.0 * j / 80.0);
	l = int(j/11);
	j = j+2-12*l;
	i = 100*(n - 49) + i + l;
	nyear = int(i);
	nmonth = int(j);
	nday = int(k);
	
	buffer[8] = '\0';
}

Date operator+(Date &date, long val)
{
	Date d = date;
	d.julian += val;
	d.update();
	return d;
}

Date operator+(long val, Date &date)
{
	Date d = date;
	d.julian += val;
	d.update();
	return d;
}

Date operator-(Date &date, long val)
{
	Date d = date;
	d.julian -= val;
	d.update();
	return d;
}

Date operator-(long val, Date &date)
{
	Date d = date;
	d.julian -= val;
	d.update();
	return d;
}

Time::Time()
{
	time_t now;
	struct tm *dt;
	time(&now);
	dt = localtime(&now);

	toSeconds(dt->tm_hour, dt->tm_min, dt->tm_sec);
}

Time::Time(struct tm *dt)
{
	toSeconds(dt->tm_hour, dt->tm_min, dt->tm_sec);
}

Time::Time(time_t tm)
{
	struct tm *dt = localtime(&tm);
	toSeconds(dt->tm_hour, dt->tm_min, dt->tm_sec);
}

Time::Time(char *str, size_t size)
{
	setTime(str, size);
}

Time::Time(int hour, int minute, int second)
{
	toSeconds(hour, minute, second);
}

bool Time::isValid(void) const
{
	if(seconds == -1)
		return false;
	return true;
}

char *Time::getTime(char *buf) const
{
	fromSeconds(buf);
	return buf;
}

time_t Time::getTime(void) const
{
	char buf[7];
	struct tm dt;
	memset(&dt, 0, sizeof(dt));
	fromSeconds(buf);
	ZNumber nhour(buf, 2);
	ZNumber nminute(buf + 2, 2);
	ZNumber nsecond(buf + 4, 2);
	
	dt.tm_hour = nhour();
	dt.tm_min = nminute();
	dt.tm_sec = nsecond();
	return mktime(&dt);
}

int Time::getHour(void) const
{
	char buf[7];
	fromSeconds(buf);
	ZNumber num(buf, 2);
	return num();
}

int Time::getMinute(void) const
{
	char buf[7];
	fromSeconds(buf);
	ZNumber num(buf + 2, 2);
	return num();
}

int Time::getSecond(void) const
{
	char buf[7];
	fromSeconds(buf);
	ZNumber num(buf + 4, 2);
	return num();
}

void Time::setTime(char *str, size_t size)
{
	int seconds = 00;

	if(!size)
		size = strlen(str);

//00:00
	if (size == 5)
	{
		seconds = 00;
	}
//00:00:00	
	else if (size == 8)
	{
		ZNumber nsecond(str + 6, 2);
		seconds = nsecond();
	}
	else if(getException() == Thread::throwObject)
	{
		throw this;
	}
	else if(getException() == Thread::throwException)
	{
		throw Exception("Time::setTime(): Invalid time.");
	}	    	
	else {
		return;
	}

	ZNumber nhour(str, 2);
	ZNumber nminute(str+3, 2);
	toSeconds(nhour(), nminute(), seconds);
}

string Time::operator()() const
{
	char buf[7];

	fromSeconds(buf);
	string time(buf);

	return time;
}

long Time::getValue(void) const
{
	return seconds;
}

Time& Time::operator++()
{
	++seconds;
	Update();
	return *this;
}

Time& Time::operator--()
{
	--seconds;
	Update();
	return *this;
}

Time& Time::operator+=(int val)
{
	seconds += val;
	Update();
	return *this;
}

Time& Time::operator-=(int val)
{
	seconds -= val;
	Update();
	return *this;
}

int Time::operator==(Time &t)
{
	return seconds == t.seconds;
}

int Time::operator!=(Time &t)
{
	return seconds != t.seconds;
}

int Time::operator<(Time &t)
{
	return seconds < t.seconds;
}

int Time::operator<=(Time &t)
{
	return seconds <= t.seconds;
}

int Time::operator>(Time &t)
{
	return seconds > t.seconds;
}

int Time::operator>=(Time &t)
{
	return seconds >= t.seconds;
}

void Time::toSeconds(int hour, int minute, int second)
{
	seconds = -1;
	
	if (hour > 23 ||minute > 59 ||second > 59) {
		if(getException() == Thread::throwObject) {
			throw this;
		}
		else if(getException() == Thread::throwException) {
			throw Exception("Time::toSeconds(): Invalid time.");
		}
		return;
	}

	seconds = 3600 * hour + 60 * minute + second;
}

void Time::fromSeconds(char *buffer) const
{
	ZNumber hour(buffer, 2);
	ZNumber minute(buffer + 2, 2);
	ZNumber second(buffer + 4, 2);

	hour = seconds / 3600;
	minute = (seconds - (3600 * hour())) / 60;
	second = seconds - (3600 * hour()) - (60 * minute());
	buffer[6] = '\0';
}

Time operator+(Time &time1, Time &time2)
{
	Time t;
	t.seconds = time1.seconds + time2.seconds;
	t.Update();
	return t;
}

Time operator-(Time &time1, Time &time2)
{
	Time t;
	t.seconds = time1.seconds - time2.seconds;
	t.Update();
	return t;
}

Time operator+(Time &time, int val)
{
	Time t = time;
	t.seconds += val;
	t.Update();
	return t;
}

Time operator+(int val, Time &time)
{
	Time t = time;
	t.seconds += val;
	t.Update();
	return t;
}

Time operator-(Time &time, int val)
{
	Time t = time;
	t.seconds -= val;
	t.Update();
	return t;
}

Time operator-(int val, Time &time)
{
	Time t = time;
	t.seconds -= val;
	t.Update();
	return t;
}

Datetime::Datetime(time_t tm)
{
	struct tm *dt = localtime(&tm);
	toJulian(dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday);
	toSeconds(dt->tm_hour, dt->tm_min, dt->tm_sec);
}

Datetime::Datetime(tm *dt) :
Date(dt), Time(dt)
{}

Datetime::Datetime(char *str, size_t size)
{
	char *timestr;

	if (!size)
		size = strlen(str);

// 00/00 00:00
	if (size ==  11)
	{
		timestr = str + 6;
		setDate(str, 5);
		setTime(timestr, 5);
	}
// 00/00/00 00:00
	else if (size == 14)
	{
		timestr = str + 9;
		setDate(str, 8);
		setTime(timestr,5);
	}
// 00/00/00 00:00:00
	else if (size == 17)
	{
		timestr = str + 9;
		setDate(str, 8);
		setTime(timestr,8);
	}
// 0000/00/00 00:00:00
	else if (size == 19)
	{
		timestr = str + 11;
		setDate(str, 10);
		setTime(timestr,8);
	}
	else if(getException() == Thread::throwObject)
	{
		throw this;
	}
	else if(getException() == Thread::throwException)
	{
		throw Exception("Datetime::Datetime(): Invalid time.");
	}	    	
}

Datetime::Datetime(int year, unsigned month, unsigned day,
		   int hour, int minute, int second) :
  Date(year, month, day), Time(hour, minute, second)
{}

Datetime::Datetime() : Date(), Time()
{
	time_t now;
	struct tm *dt;
	time(&now);
	dt = localtime(&now);

	toSeconds(dt->tm_hour, dt->tm_min, dt->tm_sec);
	toJulian(dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday);
}

bool Datetime::isValid(void) const
{
	return Date::isValid() && Time::isValid();
}

char *Datetime::getDatetime(char *buf) const
{
	fromJulian(buf);
	fromSeconds(buf+8);
	return buf;
}

time_t Datetime::getDatetime(void) const
{
	char buf[9];
	struct tm dt;
	memset(&dt, 0, sizeof(dt));

	fromJulian(buf);
	ZNumber nyear(buf, 4);
	ZNumber nmonth(buf + 4, 2);
	ZNumber nday(buf + 6, 2);
	dt.tm_year = nyear() - 1900;
	dt.tm_mon = nmonth() - 1;
	dt.tm_mday = nday();

	fromSeconds(buf);
	ZNumber nhour(buf, 2);
	ZNumber nminute(buf + 2, 2);
	ZNumber nsecond(buf + 4, 2);
	dt.tm_hour = nhour();
	dt.tm_min = nminute();
	dt.tm_sec = nsecond();
	dt.tm_isdst = -1;
	
	return mktime(&dt);
}

Datetime& Datetime::operator=(Datetime datetime)
{
	julian = datetime.julian;
	seconds = datetime.seconds;
	
	return *this;	
}

Datetime& Datetime::operator+=(Datetime &datetime)
{
	seconds += datetime.seconds;
	julian += datetime.julian; 
	Update();
	return *this;
}

Datetime& Datetime::operator-=(Datetime &datetime)
{
	seconds -= datetime.seconds;
	julian -= datetime.julian; 
	Update();
	return *this;
}

Datetime& Datetime::operator+=(Time &time)
{
	seconds += time.getValue();
	Update();
	return *this;
}

Datetime& Datetime::operator-=(Time &time)
{
	seconds -= time.getValue();
	Update();
	return *this;
}


int Datetime::operator==(Datetime &d)
{
	return (julian == d.julian) && (seconds == d.seconds);
}

int Datetime::operator!=(Datetime &d)
{
	return (julian != d.julian) || (seconds != d.seconds);
}

int Datetime::operator<(Datetime &d)
{
	if (julian != d.julian) {
		return (julian < d.julian);
	}
	else {
		return (seconds < d.seconds);
	}
}

int Datetime::operator<=(Datetime &d)
{
	if (julian != d.julian) {
		return (julian < d.julian);
	}
	else {
		return (seconds <= d.seconds);
	}
}

int Datetime::operator>(Datetime &d)
{
	if (julian != d.julian) {
		return (julian > d.julian);
	}
	else {
		return (seconds > d.seconds);
	}
}

int Datetime::operator>=(Datetime &d)
{
	if (julian != d.julian) {
		return (julian > d.julian);
	}
	else {
		return (seconds >= d.seconds);
	}
}

bool Datetime::operator!()
{
	return !(Date::isValid() && Time::isValid());
}


string Datetime::strftime(const char *format) const
{
	char buffer[64];
	int last;
	time_t t;
	tm *tbp;
	string retval;

	t = getDatetime();
        tbp = localtime(&t);
#ifdef	WIN32
	last = ::strftime(buffer, 64, format, tbp);
#else
	last = std::strftime(buffer, 64, format, tbp);
#endif

	buffer[last] = '\0';
	retval = buffer;
	return retval;

}

DateNumber::DateNumber(char *str) :
Number(str, 10), Date(str, 10)
{};

#ifdef	CCXX_NAMESPACES
};
#endif
