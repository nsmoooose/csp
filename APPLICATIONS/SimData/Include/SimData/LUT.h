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


#include <SimData/PTS.h>

#ifdef __PTS_SIM__
	// old versions of msvc can't do partial template specialization
	#define __SIMDATA_NO_LUT__
	#define __SIMDATA_LUT_H__
#endif


#ifndef __SIMDATA_LUT_H__
#define __SIMDATA_LUT_H__


/**
 * @file LUT.h
 *
 * Interpolated lookup tables of arbitrary dimension.
 *
 * TODO 	
 * 	* check/validate code under vc.net
 */


#include <vector>
#include <iostream>
#include <istream>
#include <cstdio>
#include <cmath>


#include <SimData/Pack.h>
#include <SimData/Exception.h>
#include <SimData/BaseType.h>


NAMESPACE_SIMDATA

SIMDATA_EXCEPTION(InterpolationInput);
SIMDATA_EXCEPTION(InterpolationError);
SIMDATA_EXCEPTION(InterpolationIndex);
SIMDATA_EXCEPTION(InterpolationUnpackMismatch);


// some simple debugging messages used during initial development
// LUTLOG() can be safely removed when no longer needed
#if 0
#  define __LUTLOG(msg) std::cout << msg << "\n";
#else
#  define __LUTLOG(msg)
#endif




// forward declaration for lookup table templates
template <int N, typename X>
class LUT;


/**
 * @brief A fixed-dimension vector for LUT parameters.
 *
 * General purpose, fixed-dimension vectors for
 * use with LUT templates.  Used for passing table
 * dimensions and lookup coordinates.  Not the
 * cleanest class ever, but its fast and designed
 * to work with recursive algorithms (which the
 * LUT classes use extensively).
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <int N, typename T>
class VEC {
	T m_Vec[N];
	T* m_Vp;
	int m_N;

	friend class VEC<N+1, T>;

	/**
	 * Constructor used by rest() to create subvectors.
	 */
	VEC(T* d): m_Vp(d), m_N(N) {}

public:
	/**
	 * Convenience constructor, initialize from std::vector
	 */
	VEC(std::vector<T> const &v): m_Vp(m_Vec), m_N(N) {
		if (v.size() != N) throw InterpolationIndex("VEC::VEC() dimension mismatch");
		for (int i = 0; i < N; i++) {
			m_Vec[i] = v[i];
		}
	}

	/**
	 * Partial constructor.  See operator() for details.
	 */
	VEC(T d): m_Vp(m_Vec), m_N(1) { m_Vec[0] = d; }

	/**
	 * Default constructor.  The new VEC must be initialized
	 * before use.  See set().
	 */
	VEC(): m_Vp(m_Vec), m_N(0) {}

	/**
	 * Set a new value for the vector.  For N>1 this
	 * creates an "incomplete" vector that must be
	 * finished by additional calls to operator(),
	 * as in:
	 *
	 *   VEC<3,int> s;
	 *   s.set(10)(100)(1000);
	 */
	VEC<N,T> &set(T d) {
		m_N = 1;
		m_Vp = m_Vec;
		m_Vec[0] = d;
		return *this;
	}

	/**
	 * Syntactic sugar for constructing vectors
	 * as expressions.
	 *
	 * e.g. VEC<3,int>(10)(100)(1000)
	 */
	inline VEC<N,T> &operator()(T d) {
		if (m_N >= N) {
			throw InterpolationIndex("VEC() too many coordinates specified");
		}
		m_Vec[m_N++] = d;
		return *this;
	}
	/** 
	 * Return an indexed element of the vector.
	 * Generally only called with n = 0.  Use
	 * rest() to access other members in recursive
	 * methods.
	 */
	inline T &operator[](int n) const { 
		if (n < 0 || n >= m_N) {
			throw InterpolationIndex("VEC[] index out of range");
		}
		return m_Vp[n]; 
	}

	/**
	 * Return a lower dimensional vector containing
	 * all but the first element of the original
	 * vector.
	 */
	inline VEC<N-1, T> rest() const {
		if (m_N != N) {
			throw InterpolationIndex("VEC.rest() called for incomplete VEC");
		}
		return VEC<N-1, T>(m_Vp+1);
	}
};

/**
 * @brief One-dimensional VEC partial specialization.
 * 
 * The methods here are pretty straightforward, see VEC<N, T>
 * for details.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <typename T>
class VEC<1, T> {
friend class VEC<2, T>;
	T m_Vec;
	VEC(T *d): m_Vec(*d) {}
public:
	/**
	 * Convenience constructor, initialize from std::vector
	 */
	VEC(std::vector<T> const &v) {
		if (v.size() != 1) throw InterpolationIndex("VEC::VEC() dimension mismatch");
		m_Vec = v[0];
	}
	VEC(T d): m_Vec(d) {}
	inline void set(T d) { m_Vec = d; }
	inline T operator[](int n) const { 
		if (n != 0) {
			throw InterpolationIndex("VEC[] index out of range");
		}
		return m_Vec; 
	}
};

/**
 * @brief Stub to prevent zero-dimensional vectors. 
 *
 * No zero-dimensional vectors allowed.
 */
template <typename T>
class VEC<0, T> {
	VEC(); // private ctor, not defined
};


/**
 * @brief Helper class for LUT parameters.
 *
 * Helper class templates for improved syntax when accessing 
 * tables.  LUT::operator[] returns a WRAP object which can
 * be further dereferenced with operator[].  When N coordinates
 * have been specified, WRAP queries the LUT to get the 
 * interpolated value.  WRAP defines (operator T) to act as
 * a variable of type T to return this value.  In practice,
 * here's what it looks like:
 *
 *   LUT<3,float> table;
 *   ...
 *   float x = table[10.2][3.3][1.4];
 *
 * Here table[10.2] returns a WRAP<3,float>.  The next two
 * dereferences build up the internal VEC<3, float>, finally
 * calling table.getValue().  The righthand-side expression
 * is still a WRAP<3, float> at this point, but is implicitly
 * coerced to a float to assign the interpolated value to 'x'.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <int N, typename T>
class WRAP {
	LUT<N,T> const *m_Bind;
	VEC<N,T> m_Vec;
	int m_N;
	T m_Value;

friend class LUT<N,T>;

	/**
	 * Only LUT<N, T> should create WRAP<N, T> instances.
	 */
	WRAP(LUT<N,T> const *bind, T x): m_Bind(bind), m_Vec(x), m_N(1) {
		m_Value = static_cast<T>(0.0);
	}

public:
	/**
	 * Specify another coordinate.
	 */
	inline WRAP<N,T> &operator[](T x);

	/**
	 * Coerce the return value.
	 */
	inline operator T() const { return m_Value; }
};


/**
 * @brief Base class for interpolated lookup tables.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class SIMDATA_EXPORT Interpolation: public BaseType {
public:
	typedef enum { LINEAR, SPLINE } Modes;

	/**
	 * Return true if the interpolated table is ready
	 * for use.
	 */
	inline bool isInterpolated() const { return m_Interpolated; }

protected:
	/**
	 * Bare Interpolation instances should not be created
	 */
	Interpolation(): m_Interpolated(false) {}

	inline void checkInterpolated() const {
		if (!isInterpolated()) {
			throw InterpolationError("LUT not interpolated");
		}
	}

	inline void checkNotInterpolated() const {
		if (isInterpolated()) {
			throw InterpolationError("LUT already interpolated");
		}
	}

	void throwBreakpointOrder() const {
		throw InterpolationInput("Breakpoints must increase monotonically");
	}

	void throwInterpolationMode() const {
		throw InterpolationError("Unknown interpolation mode");
	}

	bool m_Interpolated;
};


/**
 * @brief Interpolation base class for a specific data type.
 *
 * Manages lookup indexing for the interpolated table.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <typename X>
class SIMDATA_EXPORT InterpolationType: public Interpolation {
protected:
	X m_X0, m_X1, m_XS;
	int m_Limit;

	InterpolationType(): m_X0(0.0), m_X1(0.0), m_XS(0.0), m_Limit(1) {}

	/**
	 * Find the index and interpolation parameter for
	 * a given coordinate.
	 */
	inline void find(X x, int &i, X &f) const {
		x = (x - m_X0) * m_XS;
		i = static_cast<int>(floor(x));	
		if (i >= m_Limit) {
			i = m_Limit-1;
			f = 1.0;
		} else
		if (i < 0) {
			i = 0;
			f = 0.0;
		} else {
			f = x - i;
		}
	}

	/**
	 * Compute a few parameters needed for fast lookups.
	 * Called after a LUT has been interpolated.
	 */
	void postInterpolation(X x0, X x1, int n);
};


/**
 * @brief A helper class for storing second-derivatives.
 *
 * A helper class for storing second-derivatives used in 
 * spline interpolation.  The structure mirrors that
 * of LUT<N,X>, with nested vectors of progressively
 * lower dimensional Curvature instances.
 *
 * You should never need to use this class directly.
 */
template <int N, class X=float>
class Curvature;



/**
 * @brief N-dimensional lookup table.
 *
 * Converts (partially) irregularly spaced data to a regularly spaced 
 * lookup table using linear or spline interpolation.  The table is
 * then accessed by fast linear interpolation.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
template <int N, class X=float>
class SIMDATA_EXPORT LUT: public InterpolationType<X> {
	typedef LUT<N-1, X> Y;
	typedef std::vector< std::pair<X, Y> > DataVector;
	typedef std::vector<Y> TableVector;

	union {
		DataVector *m_Data;
		TableVector *m_Table;
	};

	int *m_Ref;

	inline std::pair<X, Y> &data(int n) {
		return (*m_Data)[n];
	}

	inline std::pair<X, Y> const &data(int n) const {
		return (*m_Data)[n];
	}

	inline int dataSize() const { return m_Data->size(); }

	inline Y &table(int n) {
		return (*m_Table)[n];
	}

	inline Y const &table(int n) const {
		return (*m_Table)[n];
	}

	inline int tableSize() const { return m_Table->size(); }

	inline void ref() {
		bool inc = false;
		if (isInterpolated()) {
			inc = m_Table != NULL;
		} else {
			inc = m_Data != NULL;
		}
		if (inc) {
			assert(m_Ref);
			*m_Ref += 1;
		}
	}

	inline void deref() {
		if (!m_Ref) return;
		*m_Ref -= 1;
		assert(*m_Ref >= 0);
		if (*m_Ref == 0) {
			if (m_Interpolated) {
				assert(m_Table);
				__LUTLOG("~TABLE " << int(m_Table));
				delete m_Table;
				m_Table = 0;
			} else {
				assert(m_Data);
				__LUTLOG("~DATA " << int(m_Data));
				delete m_Data;
				m_Data = 0;
			}
			delete m_Ref;
			m_Ref = 0;
		}
	}

	inline void substitute(TableVector *_table) {
		assert(!isInterpolated() && _table);
		deref();
		m_Ref = new int(0);
		m_Table = _table;
		ref();
	}

	void createData(int n);
	void postInterpolation(TableVector *_table);
	void interpolateLinear(int n, TableVector *_table);
	void computeCurvature(Curvature<N,X> &c);
	void interpolateSpline(int n, TableVector *_table);

friend class LUT<N+1,X>;
private:
	// spline interpolation
	static void __splineInterpolate(X x, X h, LUT<N,X> const &y0, LUT<N,X> const &y1, 
	              Curvature<N,X> const &c0, 
	              Curvature<N,X> const &c1, 
	              LUT<N,X> &out);

	// 2 LUT parameter version does linear interpolation
	void __interpolate(X x, LUT<N,X> const &next, LUT<N,X> &out);
	
	X __getElement(int n);
	void __getDimension(std::vector<int> &dim);

public:
	/** An integer array type for dimensioning the table */
	typedef VEC<N, int> Dim;
	/** A floating point array for specifying table coordinates */
	typedef VEC<N, X> Vec;
	/** An array of breakpoint sets for specifying the input data coordinates */
	typedef VEC<N, std::vector<X> > Breaks;

	/** Default constructor.
	 */
	LUT();

	/** Copy constructor.
	 */
	LUT(LUT<N, X> const &copy);

	/** Destructor.
	 */
	virtual ~LUT();

	/** Copy operator
	 */
	LUT<N, X> const &operator=(LUT<N, X> const &copy);

	/** Interpolate the input data.
	 *
	 *  The resulting table will have uniformly spaced data points,
	 *  with the specified number of points in each dimension.
	 *
	 *  @param dim The number of (evenly-spaced) points in each table
	 *             dimension after interpolation.
	 *  @param mode The interpolation mode (e.g. linear)
	 */
	void interpolate(Dim const &dim, Interpolation::Modes mode);

	/** Interpolate the input data.
	 *
	 *  The resulting table will have uniformly spaced data points,
	 *  with the specified number of points in each dimension.
	 *
	 *  @param dim The number of (evenly-spaced) points in each table
	 *             dimension after interpolation.
	 *  @param mode The interpolation mode (e.g. linear)
	 */
	void interpolate(std::vector<int> const &dim, Interpolation::Modes mode);

	/** Lookup the value at a given point in the table.
	 *
	 *  The value is calculated by linear interpolation of the evenly-
	 *  spaced table values, which in turn were generated from the
	 *  source data by the interpolate() method.
	 *
	 *  @param v The coordinates to sample.
	 */
	X getValue(Vec const &v) const;

	/** Lookup the value at a given point in the table.
	 *
	 *  The value is calculated by linear interpolation of the evenly-
	 *  spaced table values, which in turn were generated from the
	 *  source data by the interpolate() method.
	 *
	 *  @param x The coordinates to sample.
	 */
	inline X getValue(std::vector<X> const &x) const {
		return getValue(Vec(x));
	}


	/** Lookup the value at a given point in the table.
	 *
	 *  This method returns a WRAP instance which allows
	 *  further coordinates to be specified by indexing.
	 *  The result is equivalent to the getValue() methods,
	 *  but the syntax is different: @b
	 *  <tt>table[c0][c1][c2] == table.getValue(Vec(c0)(c1)(c2))</tt>
	 *
	 *  @param x The first coordinate to sample.
	 */
	inline WRAP<N,X> operator[](X x) const {
		checkInterpolated();
		return WRAP<N,X>(this, x);
	}

	/** Load the initial data value and breakpoints.
	 *  
	 *  The order of table values is last index first.  So
	 *  for a 3x3 table, the order would be [0][0], [0][1],
	 *  [0][2], [1][0], [1][1], etc.
	 *
	 *  @param values the table values.
	 *  @param breaks the breakpoints for each dimension.
	 *  @param index for internal use only.
	 */
	void load(std::vector<X> const &values, Breaks const &breaks, int *index = 0);
	void load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks);

	/** Serialize an object to a data archive.
	 */
	virtual void pack(Packer& p) const;
	
	/** Deserialize an object from a data archive.
	 */
	virtual void unpack(UnPacker& p);

	/** Return a string representation of LUT.
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const;
};

/**
 * @brief One-dimensional lookup table specialization.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <typename X>
class SIMDATA_EXPORT LUT<1, X>: public InterpolationType<X> {
	typedef std::vector< std::pair<X, X> > DataVector;
	typedef std::vector<X> TableVector;

	union {
		DataVector *m_Data;
		TableVector *m_Table;
	};
	int *m_Ref;

	inline std::pair<X, X> &data(int n) {
		return (*m_Data)[n];
	}

	inline std::pair<X, X> const &data(int n) const {
		return (*m_Data)[n];
	}

	inline int dataSize() const { return m_Data->size(); }

	inline X &table(int n) {
		return (*m_Table)[n];
	}

	inline X const &table(int n) const {
		return (*m_Table)[n];
	}

	inline int tableSize() const { return m_Table->size(); }

	inline void ref() {
		if (m_Data) {
			assert(m_Ref);
			*m_Ref += 1;
		}
	}

	inline void deref() {
		if (!m_Ref) return;
		assert(m_Data);
		*m_Ref -= 1;
		assert(*m_Ref >= 0);
		if (*m_Ref == 0) {
			if (m_Interpolated) {
				assert(m_Table);
				__LUTLOG("~TABLE " << int(m_Table));
				delete m_Table;
				m_Table = 0;
			} else {
				assert(m_Data);
				__LUTLOG("~DATA " << int(m_Data));
				delete m_Data;
				m_Data = 0;
			}
			delete m_Ref;
			m_Ref = 0;
		}
	}

	inline void substitute(TableVector *_table) {
		assert(!isInterpolated() && _table);
		deref();
		m_Ref = new int(0);
		m_Table = _table;
		ref();
	}

	void createData(int n);
	void postInterpolation(TableVector *_table);
	void interpolateLinear(int n, TableVector *_table);
	void interpolateSpline(int n, TableVector *_table);

friend class LUT<2,X>;
private:

	inline X __getElement(int n) {
		return data(n).second;
	}

	inline void __getDimension(std::vector<int> &dim) {
		dim.push_back(dataSize());
	}

	// linear interpolation between this and next
	void __interpolate(X x, LUT<1,X> const &next, LUT<1,X> &out);

	// spline interpolation 
	static void __splineInterpolate(
			X x, X h, 
			LUT<1,X> const &y0, 
			LUT<1,X> const &y1, 
			Curvature<1,X> const &c0, 
			Curvature<1,X> const &c1, 
			LUT<1,X> &out);

public:
	typedef VEC<1, int> Dim;
	typedef VEC<1, X> Vec;
	typedef VEC<1, std::vector<X> > Breaks;

	inline X __get(int n) const { return data(n).second; }

	LUT();
	virtual ~LUT();
	LUT(LUT<1, X> const &copy);
	LUT<1, X> const &operator=(LUT<1, X> const &copy);

	void interpolate(Dim const &dim, Interpolation::Modes mode);
	void interpolate(std::vector<int> const &dim, Interpolation::Modes mode);

	X getValue(Vec const &v) const;

	inline X getValue(std::vector<X> const &x) const {
		return getValue(Vec(x));
	}

	inline X operator[](Vec const &v) const {
		return getValue(v);
	}

	void load(std::vector<X> const &values, Breaks const &breaks, int *index = 0);
	void load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks);

	/** Serialize an object to a data archive.
	 */
	virtual void pack(Packer& p) const;
	
	/** Deserialize an object from a data archive.
	 */
	virtual void unpack(UnPacker& p);

	/** Return a string representation of LUT.
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 */
	virtual std::string typeString() const;
};

/**
 * @brief Stub to prevent zero-dimensional LUTs
 */
template <typename X>
class SIMDATA_EXPORT LUT<0, X>: public Interpolation {
	LUT();
};


/**
 * Specify an additional lookup coordinate, and
 * get the tabulated value once all the coordinates
 * are set.
 */
template <int N, typename X>
inline WRAP<N,X> &WRAP<N,X>::operator[](X x) {
	m_Vec(x);
	if (++m_N == N && m_Bind) {
		m_Value = m_Bind->getValue(m_Vec);
	}
	return *this;
}


/**
 * @brief A one-dimensional interpolated lookup table using single-precision floats.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
typedef LUT<1,float> Table1;

/**
 * @brief A two-dimensional interpolated lookup table using single-precision floats.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
typedef LUT<2,float> Table2;

/**
 * @brief A three-dimensional interpolated lookup table using single-precision floats.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 * @ingroup BaseTypes
 */
typedef LUT<3,float> Table3;


NAMESPACE_SIMDATA_END // simdata


#endif // __SIMDATA_LUT_H__

