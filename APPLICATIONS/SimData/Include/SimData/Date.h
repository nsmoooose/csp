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


/**
 * @file Date.h
 *
 * Classes for date and time manipulation.
 *
 * 	Date: 
 * 	      Generic date class storing YMD and julian days
 * 	
 * 	Zulu: 
 * 	      Very simple time class storing seconds since midnight as
 * 	      a float.  Rollover functions are provided to integrate with
 * 	      the Date class.  Time is ZULU (UTC), with an optional TZ
 * 	      field that can be used to adjust for the local timezone.
 * 	      
 * 	DateZulu:
 *
 * 	      This class simply combines Date and Zulu, implementing
 * 	      date/time rollover.  Some astronical time routines, such
 * 	      as MST (GMST/LMST) are implemented.
 *
 * 	SimTime:
 *
 * 	       This is nothing more than a typedef to the internal time
 * 	       storage type used by Zulu (currently float).
 *
 * 	SimDate:
 *
 * 	       This class combines DateZulu and BaseType so that date
 * 	       and time can be serialized to and from data archives.  A
 * 	       simple parsing function for XML cdata streams is also
 * 	       implemented.
 * 	
 */


#ifndef __SIMDATA_DATE_H__
#define __SIMDATA_DATE_H__

#include <SimData/Pack.h>
#include <SimData/Exception.h>
#include <SimData/BaseType.h>



/////////////////////////////////////////////////////////////
//
// 'fast' timing routines (1-2 msec accuracy)
//
/////////////////////////////////////////////////////////////


typedef double timing_t;

/**
 * Start timer.
 */
void tstart(void);

/**
 * Stop timer.
 */
void tend(void);

/**
 * Get time interval (in seconds) between last tstart() and tend().
 * Precision should be 1-2 ms on most platforms.
 */
double tval();

/**
 * Return the current time in seconds.  The offset is platform 
 * dependent, so do not use for absolute time.
 */
timing_t get_realtime();


/////////////////////////////////////////////////////////////
// end of timing routines
/////////////////////////////////////////////////////////////





#define F1p0_60p0      0.016666666666666667L
#define F1p0_24p0      0.041666666666666667L
#define F1p0_36525p0   0.000027378507871321L
#define F1p0_86000p0   0.000011627906976744L


typedef double radian_t;
typedef double degree_t;


NAMESPACE_SIMDATA



// TODO: please merge these with global type definitions
typedef int sint32;
typedef short sint16;
typedef unsigned char uint08;


/**
 * Exception thrown by the Data class when invalid dates are
 * detected.
 */
SIMDATA_EXCEPTION(InvalidDate);

/**
 * Date class for YMD and julian dates.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @credits Many of these routines borrow heavily from the GDate
 *          implementation in GLib
 */
class SIMDATA_EXPORT Date {
public:
	typedef sint32 julian_t;
	typedef uint08 day_t;
	typedef uint08 weekday_t;
	typedef uint08 month_t;
	typedef sint16 year_t;

	typedef enum {
		MONDAY       = 1,
		TUESDAY      = 2,
		WEDNESDAY    = 3,
		THURSDAY     = 4,
		FRIDAY       = 5,
		SATURDAY     = 6,
		SUNDAY       = 7
	} WEEKDAY;

	typedef enum {
		JANUARY	  = 1,
		FEBRUARY  = 2,
		MARCH     = 3,
		APRIL     = 4,
		MAY       = 5,
		JUNE      = 6,
		JULY      = 7,
		AUGUST    = 8,
		SEPTEMBER = 9,
		OCTOBER   = 10,
		NOVEMBER  = 11,
		DECEMBER  = 12
	} MONTH;

	/**
	 * Default constructor, sets date to Julian day 0 (January 1, 4713 BCE)
	 */
	Date() {
		m_julian = 0;
		_updateYMD();
	}

	/**
	 * Construct a Date from year, month, and day.
	 */
	Date(year_t year, month_t month, day_t day);
	
	/**
	 * Construct a Date from a Julian day.
	 */
	Date(julian_t julian) {
		m_julian = julian;
		_updateYMD();
	}

	/**
	 * Copy constructor.
	 */
	Date(const Date &d) {
		*this = d;
	}

#ifndef SWIG
	/**
	 * Assignment.
	 */
	const Date &operator=(const Date &d) {
		m_year = d.m_year;
		m_month = d.m_month;
		m_day = d.m_day;
		m_julian = d.m_julian;
		return *this;
	}
#endif // SWIG

	/**
	 * Check if a given (year, month, day) exists.
	 */
	static bool validYMD(year_t year, month_t month, day_t day) {
		if (month < 1 || month > 12 || day < 1) return false;
		return (day <= (isLeap(year) ? 
		           days_in_months[1][month] : days_in_months[0][month]));
	}

	/**
	 * Returns true if this is a leap year.
	 */
	bool isLeap() const {
		return isLeap(m_year);
	}

	/**
	 * Returns true if the year is a leap year.
	 */
	static bool isLeap(year_t year) {
  		return ( (((year % 4) == 0) && ((year % 100) != 0)) || (year % 400) == 0 );
	}
	
	/**
	 * Get the day of the week as an integer (1=MONDAY..7=SUNDAY)
	 */
	weekday_t getWeekday() const {
		return ((m_julian) % 7) + 1;
	}
	
	/**
	 * Get the month of the year as an integer (1..12).
	 */
	month_t getMonth() const {
		return m_month;
	}
	
	/**
	 * Get the day of the month (1..).
	 */
	month_t getDay() const {
		return m_day;
	}
	
	/**
	 * Get the year.
	 */
	year_t getYear() const {
		return m_year;
	}

	/**
	 * Get the Julian day (as an integer).
	 */
	julian_t getJulian() const {
		return m_julian;
	}

	/**
	 * Get the full month name (e.g. "January").
	 */
	const char *getMonthName() const;

	/**
	 * Get the abbreviated month name (e.g. "Jan").
	 */
	const char *getMonthShortName() const;
	
	/**
	 * Get the full day of the week name (e.g. "Monday").
	 */
	const char *getWeekdayName() const;
	
	/**
	 * Get the abbreviated day of the week name (e.g. "Mon").
	 */
	const char *getWeekdayShortName() const;

	/**
	 * Set the Julian day.
	 */
	void setJulian(julian_t j) {
		*this = Date(j);
	}

	/**
	 * Get the day of the year (1..).
	 */
	int getDayOfYear() const {
  		int index;
  		index = isLeap() ? 1 : 0;
  		return (days_in_year[index][m_month] + m_day);
	}

	/**
	 * Get the week of the year for weeks starting on Monday.
	 */
	int getMondayWeekOfYear() const;

	/**
	 * Get the week of the year for weeks starting on Sunday.
	 */
	int getSundayWeekOfYear() const;
	
	/**
	 * Returns true if it is the first day of the month (i.e. day == 1).
	 */
	bool isFirstOfMonth() const {
		return (m_day == 1);
	}

	/**
	 * Returns true if it is the last day of the month.
	 */
	bool isLastOfMonth() const {
  		int index = isLeap() ? 1 : 0;
  		return m_day == days_in_months[index][m_month];
	}
	
	/**
	 * Add a number of days to the date.
	 */
	void addDays(int ndays) {
 		m_julian += ndays;
		_updateYMD();
	}
	
	/**
	 * Subtract a number of days from the date.
	 */
	void subtractDays(int ndays) {
 		m_julian -= ndays;
		_updateYMD();
	}

	/**
	 * Add a number of months to the date.  The day will be
	 * truncated to fit within the new month.
	 */
	void addMonths(int nmonths);
	
	/**
	 * Subtract a number of months to the date.  The day will
	 * be truncated to fit within the new month.
	 */
	void subtractMonths(int nmonths);

	/**
	 * Add a number of years to the date.  February 29 change to
	 * February 28 for non-leap years.
	 */
	void addYears(int nyears);

	/**
	 * Subtract a number of years to the date.  February 29 change 
	 * to February 28 for non-leap years.
	 */
	void subtractYears(int nyears);

	/**
	 * Get the number of days in a month.
	 */
	static int getDaysInMonth(month_t month, year_t year);

	/**
	 * Get the number of weeks in a year (for weeks starting on Monday).
	 */
	static int getWeeksInYear(year_t year);
	
	/**
	 * Get the number of weeks in a year (for weeks starting on Sunday).
	 */
	static int getSundayWeeksInYear(year_t year);

	/**
	 * Compare two dates.  Returns +1 if a > b, -1 if a < b, or 0 if a == b.
	 */
	static int compare(const Date &a, const Date &b);
	
	/**
	 * Convert the date to a struct tm (time fields are set to zero).
	 */
	void convert(struct tm *tm) const;

	/**
	 * Convert the date to a string using strftime formatting.
	 */
	std::string formatString(const char *format) const;
	virtual std::string asString() const {
		return formatString("%Y/%m/%d");
	}
	
private:
	static const day_t days_in_months[2][13];
	static const int days_in_year[2][14];

	julian_t m_julian;
	day_t m_day;
	month_t m_month;
	year_t m_year;

	void _updateJulian();
	void _updateYMD();

	bool validYMD() const {
		return validYMD(m_year, m_month, m_day);
	}
};



/**
 * class Zulu
 * 
 * This class represents the time of day in "zulu" time (i.e. UTC
 * or Greenwich Mean).
 * 
 * Times are stored as doubles in seconds since midnight.  This
 * makes time based calculations relatively easy.  When the second
 * count exceeds 86400, the day is incremented and the clock reset.
 * Resetting the clock can cause problems with calculations involving
 * changes in time, so care must be taken.  The advantage of keeping
 * the clock count small is that precision is not lost for small time
 * intervals.  In computing time intervals that should be small,
 * use the SimDate::interval function.
 *
 * A timezone parameter and options for operating on "local" times
 * are provided.  Care must be taken when using these methods in 
 * conjunction with dates, since the date may differ depending on 
 * whether local or zulu time is used.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

class SIMDATA_EXPORT Zulu {

public:
	typedef double time_t;

	/**
	 * Default constructor.
	 *
	 * Time set to midnight, Greenwich mean.
	 */
	Zulu() {
		m_time = 0;
		m_tz = 0;
	}
	
	/**
	 * Construct a Zulu from the hour, minute, and second.
	 *
	 * @param hour hour (0-23)
	 * @param minute minute (0-59)
	 * @param second second (0.0-60.0)
	 * @param tz timezone (defaults to Greenwich mean)
	 */
	Zulu(int hour, int minute, time_t second, int tz=0) {
		m_time = hour*3600.0 + minute*60.0 + second;
		m_tz = tz;
	}
	
	/**
	 * Construct a Zulu from the seconds since midnight.
	 * 
	 * @param second seconds since midnight (0.0-86400.0)
	 * @param tz timezone (default to Greenwich mean)
	 */
	Zulu(time_t second, int tz=0) {
		m_time = second;
		m_tz = tz;
	}
	
	/**
	 * Copy constructor
	 */
	Zulu(const Zulu &z) {
		*this = z;
	}
	
#ifndef SWIG
	/**
	 * Copy operator.
	 */
	const Zulu &operator=(const Zulu &z) {
		m_time = z.m_time;
		m_tz = z.m_tz;
		return *this;
	}
#endif // SWIG

	/**
	 * Set the timezone (only effects the local time)
	 *
	 * @param tz timezone (hour offset from Greenwich mean)
	 */
	void setTZ(int tz) {
		m_tz = tz;
	}

	/**
	 * Get the timezone.
	 */
	int getTZ() const {
		return m_tz;
	}
	
	/**
	 * Wrap time to 24 hour period.
	 *
	 * Reduces the seconds since midnight to the range 0-86400.
	 *
	 * @returns the number of days subtracted to bring the time into range.
	 */
	int reduce();
	
	/**
	 * Get the curret time in seconds since midnight.
	 *
	 * @param local adjust the result to reflect the timezone.
	 */
	time_t getTime(bool local=false) const { 
		return m_time + (local ? (m_tz * 3600.0) : 0.0);
	}
	
	/**
	 * Test if the time accumulator exceeds 24 hours.
	 */
	bool overflow() const {
		return m_time >= 86400.0f;
	}
	
	/**
	 * Get the hour.
	 *
	 * @param local adjust the result to reflect the timezone.
	 */
	int getHour(bool local=false) const {
		int adjust = local ? m_tz : 0;
		return ((((int)m_time) / 3600) + adjust) % 24;
	}
	
	/**
	 * Get the minute.
	 */
	int getMinute() const {
		return (((int)m_time) / 60) % 60;
	}
	
	/**
	 * Get the second.
	 */
	int getSecond() const {
		return ((int)m_time) % 60;
	}
	
	/**
	 * Advance the time.
	 *
	 * @param dt the number of seconds to add.
	 * @return true if the time exceeds 24 hours (zulu)
	 */
	bool addTime(time_t dt) {
		m_time += dt;
		return overflow();
	}
	
	/**
	 * Set the time.
	 *
	 * @param t the number of seconds since midnight.
	 * @param local if true, t represents the local time (not zulu).
	 */
	void setTime(time_t t, bool local=false) {
		if (local) t -= m_tz * 3600.0;
		m_time = t;
	}

	/**
	 * Extract time from a C tm structure.
	 *
	 * @param tm a time structure returned by gmtime() and localtime()
	 * @param local if true, tm represents the local time (not zulu).
	 */
	void convert(struct tm *tm, bool local=false) const;
	
	/**
	 * Format the time as a string.
	 *
	 * @param format a format string: see the standard C function strftime() for details.
	 * @param local if true, format the local time (not zulu).
	 */
	std::string formatString(const char *format, bool local=false) const;
	
	
	/**
	 * Return the time as a string in the form "HH:MM::SSz"
	 */
	virtual std::string asString() const {
		return formatString("%H:%M:%Sz");
	}

private:
	time_t m_time;
	int m_tz;
};



/**
 * class DateZulu
 *
 * This class combines time and date operations into a single object.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

class SIMDATA_EXPORT DateZulu: public Date, public Zulu {


public:

	DateZulu(): Date(), Zulu() {}

	DateZulu(year_t year, month_t month, day_t day, int hour, int minute, time_t second):
		Date(year, month, day),
		Zulu(hour, minute, second) {
		addDays(reduce());
	}

	DateZulu(julian_t julian, int hour, int minute, time_t second):
		Date(julian),
		Zulu(hour, minute, second) {
		addDays(reduce());
	}

	DateZulu(const DateZulu &dz): Date(dz), Zulu(dz) { }
#ifndef SWIG
	const DateZulu &operator=(const DateZulu &dz) {
		Date::operator=(dz);
		Zulu::operator=(dz);
		return *this;
	}
#endif // SWIG
		
	
	void convert(struct tm *tm, bool local=false) const;
	std::string formatString(const char *format, bool local=false) const;
	virtual std::string asString() const {
		return formatString("%Y/%m/%d %H:%M:%Sz");
	}
	
	/**
	 * Increment the current time, with date rollover.
	 *
	 * @param dt The time interval (in seconds)
	 */
	void addTime(time_t dt) {
		Zulu::addTime(dt);
		addDays(reduce());
	}
	void setTime(time_t t) {
		Zulu::setTime(t);
		addDays(reduce());
	}

	double getJulianDate() const {
		double j = getJulian();
		double t = getTime() * F1p0_86000p0 + 0.5;
		return j + t;
	}

	double getAccurateMST(radian_t longitude=0.0L) const;
	double getMST(radian_t longitude=0.0L) const;

};



/**
 * SimTime is not actually a class, but just a floating point
 * type that stores seconds since midnight.
 */
typedef DateZulu::time_t SimTime;

/**
 * Class for representing dates and times within the simulation.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT SimDate: public DateZulu, public BaseType {

	SimTime reference;
	SimTime pause_time;
	SimTime last_update;
	bool paused;

public:
	
	SimDate(): DateZulu(), BaseType() {
		paused = false;
		last_update = get_realtime();
		setReferenceTime(getTime());
	}

	SimDate(year_t year, month_t month, day_t day, int hour, int minute, time_t second):
		DateZulu(year, month, day, hour, minute, second),
		BaseType() {
		paused = false;
		last_update = get_realtime();
		setReferenceTime(getTime());
	}
		
	SimDate(julian_t julian, int hour, int minute, time_t second):
		DateZulu(julian, hour, minute, second),
		BaseType() {
		paused = false;
		last_update = get_realtime();
		setReferenceTime(getTime());
	}
		
	SimDate(const SimDate &d): DateZulu(d) {
		paused = false;
		last_update = get_realtime();
		setReferenceTime(getTime());
	}
	
#ifndef SWIG
	const SimDate &operator=(const SimDate &d);
#endif // SWIG	

	virtual std::string asString() const {
		return formatString("%Y/%m/%d %H:%M:%Sz");
	}
	
	/* The difference between two times.  Both times
	 * should be in the range [0, 86000).
	 *
	 * @param a A time value.
	 * @param b A time value.
	 */
	static SimTime interval(SimTime a, SimTime b) {
		a -= b;
		if (a < 0.0) return a + 86000.0;
		return a;
	}

	/**
	 * Return the current system clock (~ 3 millisecond accurate)
	 * value in seconds.
	 */
	static SimTime getSystemTime() {
		return get_realtime();
	}

	/**
	 * Called by the simulation loop to update the current
	 * simulation time using the system clock.  Time rolls
	 * over past midnight and updates the date.  Don't go
	 * for more than 24 hrs without calling this or you
	 * may lose a day of simulation time.
	 *
	 * @return the elapsed time since the last call in
	 * seconds.
	 */
	double update();

	/**
	 * Set the time reference to produce the desired target time
	 *
	 * @param target The desired simulation time, right NOW! (seconds: 0-86000)
	 */
	void setReferenceTime(SimTime target) {
		reference = get_realtime() - target;
	}

	/**
	 * Pause further time updates.
	 */
	void pause() {
		paused = true;
		pause_time = get_realtime();
	}

	/**
	 * Restore time updates.  Time will continue from the point at
	 * which setPause() was called.
	 */
	void unpause() {
		paused = false;
		setReferenceTime(pause_time - reference);
	}

	bool isPaused() {
		return paused;
	}

	virtual void pack(Packer &p) const;

	virtual void unpack(UnPacker &p);

	/**
	 * Parse date/time string from XML cdata.
	 *
	 * The format is either:
  	 *    <Date name='...'>yyyy-mm-dd hh-mm-ss.ms</Date>
         * where '.ms' is optional, or:
         *    <Date name='...'>yyyy-mm-dd</Date>
	 * in which case the time is set to zero.
         */
	virtual void parseXML(const char* cdata);
	
}; 

NAMESPACE_END // namespace simdata

#endif // __SIMDATA_DATE_H__

