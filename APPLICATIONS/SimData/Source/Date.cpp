/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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


#include <SimData/Date.h>

/////////////////////////////////////////////////////////////
//
// 'fast' timing routines (1-2 msec accuracy)
//
/////////////////////////////////////////////////////////////


#ifdef _WIN32
    #include <Windows.h>
    #include <ctime>
    static LARGE_INTEGER _tstart, _tend;
    static LARGE_INTEGER freq;

    void tstart(void)
    {
        static int first = 1;

        if(first) {
            QueryPerformanceFrequency(&freq);
            first = 0;
        }
        QueryPerformanceCounter(&_tstart);
    }
    void tend(void)
    {
        QueryPerformanceCounter(&_tend);
    }

    double tval()
    {
        return ((double)_tend.QuadPart -
                    (double)_tstart.QuadPart)/((double)freq.QuadPart);
    }

    timing_t get_realtime() {
	static double scale;
	static int first = 1;
	LARGE_INTEGER x;
	double now;
	if (first) {
		QueryPerformanceFrequency(&x);
		first = 0;
		scale = 1.0 / (double)x.QuadPart;
	}
	QueryPerformanceCounter(&x);
	now = (double)x.QuadPart;
	return (timing_t) (now * scale);
    }

#else

    #include <sys/time.h>
    #include <unistd.h>

    static struct timeval _tstart, _tend;
    static struct timezone tz;

    void tstart(void)
    {
        gettimeofday(&_tstart, &tz);
    }
    void tend(void)
    {
        gettimeofday(&_tend,&tz);
    }

    double tval()
    {
        double t1, t2;

        t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
        t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
        return t2-t1;
    }

    timing_t get_realtime() {
	struct timezone tz;
	struct timeval now;
	gettimeofday(&now, &tz);
	return (timing_t) ((double)now.tv_sec + (double)now.tv_usec*1.0e-6);
    }

#endif

/////////////////////////////////////////////////////////////
// end of timing routines
/////////////////////////////////////////////////////////////

NAMESPACE_SIMDATA



const Date::day_t Date::days_in_months[2][13] = 
{  /* error, jan feb mar apr may jun jul aug sep oct nov dec */
  {  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, 
  {  0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } /* leap year */
};
const int Date::days_in_year[2][14] =
{  /* 0, jan feb mar apr may  jun  jul  aug  sep  oct  nov  dec */
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
	static const char* name[] = {"January", "February", "March", 
	                             "April", "May", "June", "July", 
				     "August", "September", "October",
				     "November", "December"};
	int idx = getMonth() - 1;
	assert(idx >= 0 && idx < 12);
	return name[idx];
}

const char *Date::getMonthShortName() const {
	static const char* name[] = {"Jan", "Feb", "Mar", 
	                             "Apr", "May", "Jun", "Jul", 
				     "Aug", "Sep", "Oct",
				     "Nov", "Dec"};
	int idx = getMonth() - 1;
	assert(idx >= 0 && idx < 12);
	return name[idx];
}

const char *Date::getWeekdayName() const {
	static const char* name[] = {"Monday", "Tuesday", "Wednesday",
	                             "Thursday", "Friday", "Saturday",
				     "Sunday"};
	int idx = getWeekday() - 1;
	assert(idx >= 0 && idx < 7);
	return name[idx];
}

const char *Date::getWeekdayShortName() const {
	static const char* name[] = {"Mon", "Tue", "Wed",
	                             "Thu", "Fri", "Sat",
				     "Sun"};
	int idx = getWeekday() - 1;
	assert(idx >= 0 && idx < 7);
	return name[idx];
}

int Date::getMondayWeekOfYear() const  {
	weekday_t wd;
	day_t day;
	Date first(m_year, 1, 1);
	wd = first.getWeekday() - 1;
	day = getDayOfYear() - 1;
	return ((day + wd)/7U + (wd == 0 ? 1 : 0));
}

int Date::getSundayWeekOfYear() const {
	weekday_t wd;
	day_t day;
	Date first(m_year, 1, 1);
	wd = first.getWeekday();
	if (wd == 7) wd = 0;
	day = getDayOfYear() - 1;
	return ((day + wd)/7U + (wd == 0 ? 1 : 0));
}

void Date::addMonths(int nmonths) {
	int years, months;
	int index;
	nmonths += m_month - 1;
	years = nmonths/12;
	months = nmonths%12;
	m_month = months + 1;
	m_year += years;
	index = isLeap() ? 1 : 0;
	if (m_day > days_in_months[index][m_month]) {
		m_day = days_in_months[index][m_month];
	}
	_updateJulian();
}

void Date::subtractMonths(int nmonths) {
	int years, months;
	int index;
	years = nmonths/12;
	months = nmonths%12;
	m_year -= years;
	if (m_month > months) {
		m_month -= months;
	} else {
		months -= m_month;
		m_month = 12 - months;
		m_year -= 1;
	}
	index = isLeap() ? 1 : 0;
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
  year_t y;
  month_t m;
  day_t day;
  unsigned int A, B, C, D, E, M;
  
  /* Formula taken from the Calendar FAQ */
  
  A = m_julian + 32045;
  B = ( 4 *(A + 36524) )/ 146097 - 1;
  C = A - (146097 * B)/4;
  D = ( 4 * (C + 365) ) / 1461 - 1;
  E = C - ((1461*D) / 4);
  M = (5 * (E - 1) + 2)/153;
  
  m = M + 3 - (12*(M/10));
  day = E - (153*M + 2)/5;
  y = 100 * B + D - 4800 + (M/10);
  
#ifdef ENABLE_DEBUG
  if (!validYMD(y, m, day)) 
    {
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
	if (overflow()) {
		days = (int) (m_time * 0.00001162790697674418);
		m_time -= days * 86000.0f;
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
#define PI	3.141592653e+0L
#define COEFF0	24110.54841e+0L
#define COEFF1  8640184.812866e+0L
#define COEFF2  0.093104e+0L
#define COEFF3  -6.2e-6L

double DateZulu::getAccurateMST(radian_t longitude) const {  
	double JD = getJulianDate();
	double T = (JD - EPOCH) * F1p0_36525p0;
	double F = DAYSEC * (JD - (int) JD);
	double GMST = COEFF0 - DAYSEC/2.0L
	               + ((COEFF1 + (COEFF2 + COEFF3 * T) * T) * T) + F;
	GMST = GMST * SEC2RAD;
	int n = (int) (GMST * (0.5/PI));
	if (n < 0) n--;
	return GMST - n*(2.0*PI) + longitude;
}

double DateZulu::getMST(radian_t longitude) const {  
	double JD = getJulianDate();
	double T = (JD - EPOCH) * F1p0_36525p0;
	double F = DAYSEC * (JD - (int) JD);
	double GMST = COEFF0 - DAYSEC/2.0L
	               + COEFF1 * T + F;
	GMST = GMST * SEC2RAD;
	int n = (int) (GMST * (0.5/PI));
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
	SimTime dt = 0.0;
	if (!paused) {
		SimTime t = get_realtime();
		dt = t - last_update;
		last_update = t;
		setTime(t - reference);
	}
	return dt;
}

void SimDate::pack(Packer &p) const {
	p.pack(((float)getTime()));
	p.pack((int)getJulian());
}

void SimDate::unpack(UnPacker &p) {
	float time_;
	int julian_;
	p.unpack(time_);
	p.unpack(julian_);
	setTime(time_);
	setJulian(julian_);
}

void SimDate::parseXML(const char* cdata) {
	int dy,dm,dd,th=0,tm=0;
	float ts=0.0;
	int ok;
	std::string s(cdata);
	if (s.find(" ") >= 0) {
		int n = sscanf(s.c_str(), "%d-%d-%d %d:%d:%f", &dy, &dm, &dd, &th, &tm, &ts);
		ok = (n == 6);
	} else {
	}
	if (!ok || dy < 0 || dy >9999 || dm < 1 || dm > 12 || dd < 1 || dd > 31 \
	     || th < 0 || th > 23 || tm < 0 || tm > 59 || ts < 0.0 || ts >= 60.0) {
		throw ParseException("SYNTAX ERROR: invalid date string");
	}
	try {
		*this = SimDate(dy, dm, dd, th, tm, ts);
	} 
	catch (InvalidDate) {
		throw ParseException("SYNTAX ERROR: invalid date string");
	}
}

NAMESPACE_END // namespace simdata


/*
USING_SIMDATA
main() {
	SimDate d(2002, 12, 14, 22, 50, 0.0);
	cout << d.asString() << endl;
	cout << d.getLMST(-2.1345) << endl;
	cout << d.getMonthName() << endl;
	cout << d.getWeekdayName() << endl;
	cout << d.getJulianDate() << endl;
	cout << d.getTime() << endl;
	printf("%.3lf\n", d.getJulianDate());
	Date t(1970, 1, 1);
	cout << t.getJulian() << endl;
	Date i(t.getJulian());
	cout << i.strftime("%D") << endl;
	SimDate s(1877, 8, 11, 7, 30, 0.0);
	printf("%.3lf\n", s.getJulianDate());
	cout << "=====================" << endl;
	SimDate x(2002,12,15,11,6,47);
	printf("%.9lf\n", x.getAccurateGMST());
	printf("%.9lf\n", x.getGMST());
	return 0;
}
*/


