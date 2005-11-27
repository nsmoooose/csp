/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Date.cpp
 * @brief Classes for date and time manipulation
 */

#include <csp/csplib/data/Date.h>

#include <ctime>
#include <ostream>


CSP_NAMESPACE


const Date::day_t Date::days_in_months[2][13] =
{	/* error, jan feb mar apr may jun jul aug sep oct nov dec */
	{  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{  0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } /* leap year */
};
const int Date::days_in_year[2][14] =
{	/* 0, jan feb mar apr may  jun  jul  aug  sep  oct  nov  dec */
	{  0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	{  0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

Date::Date(year_t year, month_t month, day_t day) {
	if (!validYMD(year, month, day)) throw InvalidDate();
	m_year = year;
	m_month = month;
	m_day = day;
	_updateJulian();
}

const char *Date::getMonthName() const {
	static const char* name[] = {"January", "February", "March", "April", "May", "June", "July",
	                             "August", "September", "October", "November", "December"};
	int idx = getMonth() - 1;
	assert(idx >= 0 && idx < 12);
	return name[idx];
}

const char *Date::getMonthShortName() const {
	static const char* name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
	                             "Aug", "Sep", "Oct", "Nov", "Dec"};
	int idx = getMonth() - 1;
	assert(idx >= 0 && idx < 12);
	return name[idx];
}

const char *Date::getWeekdayName() const {
	static const char* name[] = {"Monday", "Tuesday", "Wednesday",
	                             "Thursday", "Friday", "Saturday", "Sunday"};
	int idx = getWeekday() - 1;
	assert(idx >= 0 && idx < 7);
	return name[idx];
}

const char *Date::getWeekdayShortName() const {
	static const char* name[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
	int idx = getWeekday() - 1;
	assert(idx >= 0 && idx < 7);
	return name[idx];
}

int Date::getMondayWeekOfYear() const  {
	Date first(m_year, 1, 1);
	int wd = first.getWeekday() - 1;
	int day = getDayOfYear() - 1;
	return ((day + wd)/7U + (wd == 0 ? 1 : 0));
}

int Date::getSundayWeekOfYear() const {
	Date first(m_year, 1, 1);
	int wd = first.getWeekday();
	if (wd == 7) wd = 0;
	int day = getDayOfYear() - 1;
	return ((day + wd)/7U + (wd == 0 ? 1 : 0));
}

void Date::addMonths(int nmonths) {
	nmonths += m_month - 1;
	year_t years = static_cast<year_t>(nmonths / 12);
	month_t months = static_cast<month_t>(nmonths % 12);
	m_month = months + 1;
	m_year += years;
	int index = isLeap() ? 1 : 0;
	if (m_day > days_in_months[index][m_month]) {
		m_day = days_in_months[index][m_month];
	}
	_updateJulian();
}

void Date::subtractMonths(int nmonths) {
	year_t years = static_cast<year_t>(nmonths / 12);
	month_t months = static_cast<month_t>(nmonths % 12);
	m_year -= years;
	if (m_month > months) {
		m_month -= months;
	} else {
		months -= m_month;
		m_month = 12 - months;
		m_year -= 1;
	}
	int index = isLeap() ? 1 : 0;
	if (m_day > days_in_months[index][m_month]) {
		m_day = days_in_months[index][m_month];
	}
	_updateJulian();
}

void Date::addYears(int nyears) {
	m_year += nyears;
	if (m_month == 2 && m_day == 29) {
		if (!isLeap()) m_day = 28;
	}
	_updateJulian();
}

void Date::subtractYears(int nyears) {
	m_year -= nyears;
	if (m_month == 2 && m_day == 29) {
		if (!isLeap()) m_day = 28;
	}
	_updateJulian();
}

/**
 * Convert YMD to integer Julian date.
 *
 * Computes the julian date (JD) given a gregorian calendar date
 * (year,month,day).  Adapted from Fliegel and van Flandern (1968)
 */
void Date::_updateJulian()
{
	int y = m_year;
	int m = m_month;
	int d = m_day;
	m_julian = d-32075+1461*(y+4800+(m-14)/12)/4+367*(m-2-(m-14)/12*12)/12-3*((y+4900+(m-14)/12)/100)/4;
}


void Date::_updateYMD()
{
	/* Formula taken from the Calendar FAQ */
	unsigned int A = m_julian + 32045;
	unsigned int B = ( 4 *(A + 36524) )/ 146097 - 1;
	unsigned int C = A - (146097 * B)/4;
	unsigned int D = ( 4 * (C + 365) ) / 1461 - 1;
	unsigned int E = C - ((1461*D) / 4);
	unsigned int M = (5 * (E - 1) + 2)/153;

	month_t m = static_cast<month_t>(M + 3 - (12*(M/10)));
	day_t day = static_cast<day_t>(E - (153*M + 2)/5);
	year_t y = static_cast<year_t>(100 * B + D - 4800 + (M/10));

#ifdef ENABLE_DEBUG
	if (!validYMD(y, m, day)) {
		throw InvalidDate();
	}
#endif

	m_year = y;
	m_month = m;
	m_day = day;
}



int Date::getDaysInMonth(Date::month_t month, Date::year_t year) {
	if (!validYMD(year, month, 1)) throw InvalidDate();
	int index = Date::isLeap(year) ? 1 : 0;
	return days_in_months[index][month];
}

int Date::getWeeksInYear(Date::year_t year) {
	if (Date(year, 1, 1).getWeekday() == MONDAY) return 53;
	if (Date(year, 12, 31).getWeekday() == MONDAY) return 53;
	if (isLeap(year)) {
		if (Date(year, 1, 2).getWeekday() == MONDAY) return 53;
		if (Date(year, 12, 30).getWeekday() == MONDAY) return 53;
	}
	return 52;
}

int Date::getSundayWeeksInYear(Date::year_t year) {
	if (Date(year, 1, 1).getWeekday() == SUNDAY) return 53;
	if (Date(year, 12, 31).getWeekday() == SUNDAY) return 53;
	if (isLeap(year)) {
		if (Date(year, 1, 2).getWeekday() == SUNDAY) return 53;
		if (Date(year, 12, 30).getWeekday() == SUNDAY) return 53;
	}
	return 52;
}

int Date::compare(const Date &a, const Date &b) {
	julian_t ja = a.getJulian();
	julian_t jb = b.getJulian();
	if (ja > jb) return 1;
	if (ja < jb) return -1;
	return 0;
}

void Date::convert(struct tm *tm) const {
	/* zero all the irrelevant fields to be sure they're valid */
	/* On Linux and maybe other systems, there are weird non-POSIX
	* fields on the end of struct tm that choke strftime if they
	* contain garbage.  So we need to 0 the entire struct, not just the
	* fields we know to exist.
	*/
	memset (tm, 0x0, sizeof (struct tm));

	tm->tm_mday = m_day;
	tm->tm_mon  = m_month - 1; /* 0-11 goes in tm */
	tm->tm_year = ((int)m_year) - 1900; /* X/Open says tm_year can be negative */

	weekday_t day = getWeekday();
	if (day == 7) day = 0; /* struct tm wants days since Sunday, so Sunday is 0 */

	tm->tm_wday = (int)day;

	tm->tm_yday = getDayOfYear() - 1; /* 0 to 365 */
	tm->tm_isdst = -1; /* -1 means "information not available" */
}

std::string Date::formatString(const char *format) const {
	char buffer[128];
	struct tm tm;
	convert(&tm);
	int retval = ::strftime(buffer, 127, format, &tm);
	if (retval == 0) return "";
	return buffer;
}


int Zulu::reduce() {
	int days = 0;
	if (overflow() || m_time < 0.0) {
		days = (int) (m_time * 0.00001157407407407407);
		if (m_time < 0.0) days--;
		m_time -= days * 86400.0f;
	}
	return days;
}

void Zulu::convert(struct tm *tm, bool local) const {
	/* zero all the irrelevant fields to be sure they're valid */
	/* On Linux and maybe other systems, there are weird non-POSIX
	* fields on the end of struct tm that choke strftime if they
	* contain garbage.  So we need to 0 the entire struct, not just the
	* fields we know to exist.
	*/
	memset (tm, 0x0, sizeof (struct tm));
  	tm->tm_hour = getHour(local);
	tm->tm_min = getMinute();
	tm->tm_sec = (int) getSecond();
	tm->tm_isdst = -1; /* -1 means "information not available" */
}

std::string Zulu::formatString(const char *format, bool local) const {
	char buffer[128];
	struct tm tm;
	convert(&tm, local);
	int retval = ::strftime(buffer, 127, format, &tm);
	if (retval == 0) return "";
	return buffer;
}


void DateZulu::convert(struct tm *tm, bool local) const {
	/* zero all the irrelevant fields to be sure they're valid */
	/* On Linux and maybe other systems, there are weird non-POSIX
	* fields on the end of struct tm that choke strftime if they
	* contain garbage.  So we need to 0 the entire struct, not just the
	* fields we know to exist.
	*/
	memset (tm, 0x0, sizeof (struct tm));
  	tm->tm_hour = getHour(local);
	tm->tm_min = getMinute();
	tm->tm_sec = (int) getSecond();
	tm->tm_mday = getDay();
	tm->tm_mon  = getMonth() - 1; /* 0-11 goes in tm */
	tm->tm_year = getYear() - 1900; /* X/Open says tm_year can be negative */

	weekday_t day = getWeekday();
	if (day == 7) day = 0; /* struct tm wants days since Sunday, so Sunday is 0 */

	tm->tm_wday = (int)day;

	tm->tm_yday = getDayOfYear() - 1; /* 0 to 365 */
	tm->tm_isdst = -1; /* -1 means "information not available" */
}

std::string DateZulu::formatString(const char *format, bool local) const {
	char buffer[128];
	struct tm tm;
	convert(&tm, local);
	int retval = ::strftime(buffer, 127, format, &tm);
	if (retval == 0) return "";
	return buffer;
}

#define SEC2RAD 7.272205216643039903848712e-5L
#define EPOCH   2451545.0e+0L
#define DAYSEC  86400.0e+0L
#define CENDAY  36525.0e+0L
#define PI      3.141592653e+0L
#define COEFF0  24110.54841e+0L
#define COEFF1  8640184.812866e+0L
#define COEFF2  0.093104e+0L
#define COEFF3  -6.2e-6L

/* DEPRECATED
double DateZulu::getAccurateMST(radian_t longitude) const {
	return getMST(longitude);
}
*/

double DateZulu::getMST(radian_t longitude) const {
	double JD = getJulianDate();
	double T = (JD - EPOCH) * CSP_F1p0_36525p0;
	double F = static_cast<double>(DAYSEC * (JD - static_cast<int>(JD)));
	double GMST = static_cast<double>(COEFF0 - DAYSEC/2.0L + ((COEFF1 + (COEFF2 + COEFF3 * T) * T) * T) + F);
	GMST = GMST * SEC2RAD;
	int n = static_cast<int>(GMST * (0.5/PI));
	if (n < 0) n--;
	return GMST - n*(2.0*PI) + longitude;
}

const SimDate &SimDate::operator=(const SimDate &d) {
	DateZulu::operator=(d);
	paused = d.paused;
	last_update = d.last_update;
	pause_time = d.pause_time;
	reference = d.reference;
	return *this;
}

double SimDate::update() {
	calibrateRealTime();
	SimTime dt = 0.0;
	if (!paused) {
		SimTime t = getCalibratedRealTime();
		dt = t - last_update;
		last_update = t;
		setTime(t - reference);
	}
	return dt;
}

void SimDate::serialize(Reader &reader) {
	float time_;
	int julian_;
	reader >> time_;
	reader >> julian_;
	setTime(time_);
	setJulian(julian_);
}

void SimDate::serialize(Writer &writer) const {
	float time_ = static_cast<float>(getTime());
	int julian_ = getJulian();
	writer << time_;
	writer << julian_;
}

void SimDate::parseXML(const char* cdata) {
	int dy=0,dm=0,dd=0,th=0,tm=0;
	float ts=0.0;
	int ok = 0;
	std::string s(cdata);
	if (s.find(" ") != std::string::npos) {
		int n = sscanf(s.c_str(), "%d-%d-%d %d:%d:%f", &dy, &dm, &dd, &th, &tm, &ts);
		ok = (n == 6);
	} else {
	}
	if (!ok || dy < 0 || dy >9999 || dm < 1 || dm > 12 || dd < 1 || dd > 31 \
	     || th < 0 || th > 23 || tm < 0 || tm > 59 || ts < 0.0 || ts >= 60.0) {
		throw ParseException("SYNTAX ERROR: invalid date string");
	}
	try {
		*this = SimDate(static_cast<year_t>(dy), static_cast<month_t>(dm), static_cast<day_t>(dd), th, tm, ts);
	}
	catch (InvalidDate) {
		throw ParseException("SYNTAX ERROR: invalid date string");
	}
}


std::ostream &operator <<(std::ostream &o, Date const &d) { return o << d.asString(); }
std::ostream &operator <<(std::ostream &o, Zulu const &d) { return o << d.asString(); }
std::ostream &operator <<(std::ostream &o, DateZulu const &d) { return o << d.asString(); }
std::ostream &operator <<(std::ostream &o, SimDate const &d) { return o << d.asString(); }


CSP_NAMESPACE_END


