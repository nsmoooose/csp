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


#ifndef __INTERPOLATE_H__
#define __INTERPOLATE_H__

#include <string>
#include <vector>
#include <cstdio>

#include <SimData/Enum.h>
#include <SimData/Export.h>



/*
#define value_t	float
#define vector_t std::vector<value_t>
#define vector_it std::vector<value_t>::iterator
*/


NAMESPACE_SIMDATA

class Packer;
class UnPacker;

/**
 * Base class for interpolated data.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT InterpolatedData: public BaseType {

	/*
	
	Base class for lookup tables.
	
	Derived classes must implement the following methods:
		precess_second_derivatives()
		getValue()
		getPrecise()
	
	*/

public:

	typedef float value_t;
	typedef std::vector<value_t> vector_t;
	typedef std::vector<value_t>::iterator vector_it;
	typedef std::vector<value_t>::const_iterator vector_cit;

	static const Enumeration Method;;
	Enum method;

	InterpolatedData();
	virtual ~InterpolatedData();
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	virtual value_t getValue(value_t, value_t) const;
	virtual value_t getPrecise(value_t, value_t) const;
	vector_t compute_second_derivatives(const vector_t& breaks, const vector_t& data);
	int find(vector_t b, value_t v) const;
};


/**
 * A simple one-dimensional bicubic-interpolated look up table.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class Curve: public InterpolatedData {

	vector_t _breaks, _data, _table, _sd;
	value_t _range, _spacing, _min;
	int _i_n;

	// A one dimensional bicubic-interpolated LUT.
	
public:

	Curve();
	Curve(const Curve &);
	virtual ~Curve();
#ifndef SWIG
	const Curve &operator=(const Curve &);
#endif // SWIG
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	vector_t getBreaks();
	void setBreaks(const vector_t& breaks);
	void setData(const vector_t& data);
	void interpolate(value_t spacing);
	virtual void _compute_second_derivatives();
	value_t getPrecise(value_t v) const;
	value_t getValue(value_t p) const;
	void dumpCurve(FILE* f) const;
	virtual std::string asString() const;
};



/**
 * A simple two-dimensional bicubic-interpolated look up table.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Table: public InterpolatedData {

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
	virtual void _compute_second_derivatives();
	void toPGM(FILE *fp) const;
	void dumpTable(FILE *fp) const;
	void dumpDRows(FILE *fp) const;
	void dumpDCols(FILE *fp) const;
	virtual std::string asString() const;

};


NAMESPACE_END // namespace simdata


#endif // __INTERPOLATE_H__

