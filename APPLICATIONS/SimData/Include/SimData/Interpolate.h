/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimData.
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
 * @file Interpolate.h
 * 
 * Lookup table classes with bicubic interpolation.
 *
 * The Table classes read raw parameterized data from a input file.
 * The raw data is interpolated at a specified granularity using a 
 * bicubic approximation and stored for fast lookup.  Lookups then
 * do linear interpolation of these stored values.
 *
 * classes:
 *	Table1D          LUT for F(x)
 *	Table2D          LUT for F(x,y)
 */


#ifndef __SIMDATA_INTERPOLATE_H__
#define __SIMDATA_INTERPOLATE_H__

#include <string>
#include <vector>
#include <cstdio>

#include <SimData/Enum.h>
#include <SimData/Export.h>




NAMESPACE_SIMDATA

class Packer;
class UnPacker;

/**
 * @brief Base class for interpolated data.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <typename T>
class SIMDATA_EXPORT InterpolatedData: public BaseType {

	/*
	
	Base class for lookup tables.
	
	Derived classes must implement the following methods:
		precess_second_derivatives()
		getValue()
		getPrecise()
	
	*/

public:

	typedef T value_t;
	typedef typename std::vector<value_t> vector_t;
	typedef typename std::vector<value_t>::iterator vector_it;
	typedef typename std::vector<value_t>::const_iterator vector_cit;

	static const Enumeration Method;
	Enum<InterpolatedData<T>::Method> method;

	InterpolatedData();
	virtual ~InterpolatedData();
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	virtual value_t getValue(value_t, value_t) const;
	virtual value_t getPrecise(value_t, value_t) const;
	int find(vector_t b, value_t v) const;

protected:
	vector_t _compute_second_derivatives(const vector_t& breaks, const vector_t& data);
	virtual void _compute_second_derivatives() = 0;
};


#ifdef SWIG
%template(InterpolatedFloat) InterpolatedData<float>;
#endif


/**
 * @brief A one-dimensional bicubic-interpolated look-up table.
 *
 * The curve input is a discrete set of (x, y) pairs sampled from a
 * continuous curve y = F(x).  A smooth curve is formed from these
 * points using bicubic interpolation, and then resampled at a 
 * specified number of evenly spaced values of x.  The resampled
 * values can then be used as a fast lookup table (with linear 
 * interpolation) to find y(x).
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 * @deprecated Use Table1 instead.
 */
class SIMDATA_EXPORT Curve: public InterpolatedData<float> {

	vector_t _breaks, _data, _table, _sd;
	value_t _range, _spacing, _min;
	int _i_n;

	// A one dimensional bicubic-interpolated LUT.
	
public:

	/**
	 * Default constructor.
	 */
	Curve();
	
	/**
	 * Copy constructor.
	 */
	Curve(const Curve &);

	/**
	 * Destructor.
	 */
	virtual ~Curve();

#ifndef SWIG
	/**
	 * Assignment operator.
	 */
	const Curve &operator=(const Curve &);
#endif // SWIG

	/**
	 * Serialize to a data archive.
	 */
	virtual void pack(Packer& p) const;
	
	/**
	 * Deserialize from a data archive.
	 */
	virtual void unpack(UnPacker& p);

	/**
	 * Get a list of the x values of the source data set.
	 */
	vector_t getBreaks();

	/**
	 * Set the x values of the source data set.
	 */
	void setBreaks(const vector_t& breaks);

	/**
	 * Set the y values of the source data set.
	 */
	void setData(const vector_t& data);

	/**
	 * Interpolate the source data set and resample at
	 * at equaly spaced values of x to form the lookup table.
	 *
	 * @param spacing the spacing between resampled values of X
	 */
	void interpolate(value_t spacing);
	
	/**
	 * Get the "precise" value y(x) from bicubic interpolation.
	 */
	value_t getPrecise(value_t x) const;

	/**
	 * Get the "quick and dirty" value of y(x) from linear interpolation
	 * of the resampled lookup table.
	 */
	value_t getValue(value_t x) const;

	/**
	 * Dump the y(x) lookup table to an output file.
	 */
	void dumpCurve(FILE* f) const;

	/**
	 * String representation.
	 */
	virtual std::string asString() const;

protected:
	/**
	 * Compute the second derivative for bicubic interpolation.
	 */
	virtual void _compute_second_derivatives();
};



/**
 * @brief A two-dimensional bicubic-interpolated look up table.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 * @deprecated Use Table2 instead.
 */
class SIMDATA_EXPORT Table: public InterpolatedData<float> {

public:
	std::string _x_label, _y_label;
	vector_t _x_breaks, _y_breaks, _data, _table, _dcols, _drows;
	value_t _x_spacing, _y_spacing;
	value_t _x_min, _y_min, _x_range, _y_range;
	int _x_in, _y_in, _x_n, _y_n;
	int _valid;
	
public:
	Table();
	Table(const Table &);
	virtual ~Table();
#ifndef SWIG
	const Table &operator=(const Table &);
#endif // SWIG
	void invalidate();
	int isValid() const;
	virtual void pack(Packer &p) const;
	virtual void unpack(UnPacker &p);
	vector_t getXBreaks() const;
	vector_t getYBreaks() const;
	void setXBreaks(const vector_t& breaks);
	void setYBreaks(const vector_t& breaks);
	void setBreaks(const vector_t& x_breaks, const vector_t& y_breaks);
	void setXSpacing(value_t spacing);
	void setYSpacing(value_t spacing);
	void setSpacing(value_t x_spacing, value_t y_spacing);
	value_t getXSpacing() const;
	value_t getYSpacing() const;
	void setData(const vector_t& data);
	void interpolate();
	value_t getPrecise(value_t x, value_t y) const;
	value_t getValue(value_t x, value_t y) const;
	void toPGM(FILE *fp) const;
	void dumpTable(FILE *fp) const;
	void dumpDRows(FILE *fp) const;
	void dumpDCols(FILE *fp) const;
	virtual std::string asString() const;

protected:
	virtual void _compute_second_derivatives();

};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_INTERPOLATE_H__

