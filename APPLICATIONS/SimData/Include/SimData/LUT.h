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


#ifndef __SIMDATA_LUT_H__
#define __SIMDATA_LUT_H__


/**
 * @file LUT.h
 *
 * Interpolated lookup tables of arbitrary dimension.
 *
 * TODO
 * 	this code is still in the form of a .cpp file used
 * 	during testing/development
 *
 * 	need to integrate with simdata:
 * 	     inherit from basetype
 * 	     implement pack/unpack/asString, etc.
 * 	     (see the original simdata::Curve and simdata::Table 
 * 	      classes for guidance)
 *           xml parser needs to understand these classes
 *           swig interface
 *
 *      check/validate code under vc.net
 */


#include <vector>
#include <iostream>
#include <istream>
#include <cstdio>


#include <SimData/Date.h>
#include <SimData/Exception.h>


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


/**
 *   Second derivative calculation for natural cubic spline
 *
 *   Adapted from the GNU Scientific Library
 *   Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002 Gerard Jungman
 */
template<typename X>
void cspline(std::vector<X> const &x, std::vector<X> const &y, std::vector<X> &out)
{
	int n = x.size();
	// construct the linear (tridiagonal) system
	std::vector<X> D(n), b(n), L(n);
	X dx0, dx1 = x[1] - x[0];
	X dy0, dy1 = y[1] - y[0];
	for (int i = 0, j = n - 2; i < j; i++) { 
		dx0 = dx1;
		dy0 = dy1;
		dx1 = x[i+2] - x[i+1];
		dy1 = y[i+2] - y[i+1];
		D[i] = static_cast<X>(2.0) * (dx0 + dx1);
		b[i] = static_cast<X>(6.0) * (dy1/dx1 - dy0/dx0);
		L[i] = dx1;
	}
	// solve it by Cholesky decomposition
	out[0] = out[n-1] = 0.0; // natural spline
	X lp = L[0];
	L[0] /= D[0];
	for (int i = 1, j = n - 2; i < j; ++i) {
		D[i] -= lp * L[i-1];
		lp = L[i];
		L[i] /= D[i];
	}
	for (int i = 1, j = n - 1; i < j; ++i) {
		b[i] -= L[i-1] * b[i-1];
		out[i] = b[i-1] / D[i-1];
	}
	// back substitute
	for (int i = n - 3; i > 0; --i) {
		out[i] -= L[i-1] * out[i+1];
	}
}


// forward declaration for lookup table templates
template <int N, typename X>
class LUT;


/**
 * General purpose, fixed-dimension vectors for
 * use with LUT templates.  Used for passing table
 * dimensions and lookup coordinates.  Not the
 * cleanest class ever, but its fast and designed
 * to work with recursive algorithms (which the
 * LUT classes use extensively).
 */
template <int N, typename T>
class VEC {
	T m_Vec[N];
	int m_N;
	T* m_Vp;

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
 * One-dimensional VEC partial specialization.
 * 
 * The methods here are pretty straightforward, see VEC<N, T>
 * for details.
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
 * No zero-dimensional vectors allowed.
 */
template <typename T>
class VEC<0, T> {
	VEC() {}
};


/**
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
 */
template <int N, typename T>
class WRAP {
	LUT<N,T> *m_Bind;
	VEC<N,T> m_Vec;
	int m_N;
	T m_Value;

friend class LUT<N,T>;

	/**
	 * Only LUT<N, T> should create WRAP<N, T> instances.
	 */
	WRAP(LUT<N,T> *bind, T x): m_Bind(bind), m_Vec(x), m_N(1) {
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
 * Base class for interpolated lookup tables.
 */
class Interpolation: public BaseType {
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
 * Interpolation base class for a specific data type.
 * Manages lookup indexing for the interpolated table.
 */
template <typename X>
class InterpolationType: public Interpolation {
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
	void postInterpolation(X x0, X x1, int n) {
		m_Limit = n;
		m_X0 = x0;
		m_X1 = x1;
		if (m_X0 == m_X1) {
			m_XS = 0.0;
		} else {
			m_XS = static_cast<X>(n) / (m_X1 - m_X0);
		}
		m_Interpolated = true;
	}

};


/**
 * A helper class for storing second-derivatives used
 * in spline interpolation.  The structure mirrors that
 * of LUT<N,X>, with nested vectors of progressively
 * lower dimensional Curvature instances.
 */
template <int N, class X=float>
class Curvature {
	typedef Curvature<N-1,X> Y;
	typedef std::vector<Y> CurveVector;
	CurveVector m_Curve;

friend class LUT<N,X>;

	inline int size() const { return m_Curve.size(); }

	/**
	 * Const accessor for lower-dimension (N-1) Curvature
	 */
	inline Y const &curve(int n) const { return m_Curve[n]; }

	/**
	 * Accessor for lower-dimension (N-1) Curvature
	 */
	inline Y &curve(int n) { return m_Curve[n]; }

public:
	Curvature() {}

	/**
	 * Set a specific subelement indexed by a single
	 * integer.  The indexing mirrors the LUT method 
	 * getElement().
	 * 
	 * For example:
	 *     if N = 3 and the dimensions are 10, 5, and 20,
	 *     then n = 834 would correspond to the indices
	 *     (4, 3, 16) --- (16*5 + 3)*10 + 4 = 834
	 */
	void setElement(int n, X const v) {
		int i = n % size();
		curve(i).setElement((n-i)/size(), v);
	}

	/**
	 * Initialize the curvature structure, allocating
	 * lower-dimensional subcomponents of the requested size.
	 */
	void init(VEC<N,int> const &dim) {
		int n = dim[0];
		m_Curve.resize(n);
		for (int i = 0; i < n; i++) {
			m_Curve[i].init(dim.rest());
		}
	}
};

/**
 * Partial specialization of Curvature for one-dimension.
 *
 * This is essentially just a std::vector<X>.  See Curvature
 * (N>1) for details.
 */
template <class X>
class Curvature<1, X> {
	typedef std::vector<X> CurveVector;
	CurveVector m_Curve;
friend class LUT<1,X>;
	inline int size() const { return m_Curve.size(); }
	inline X const &curve(int n) const { return m_Curve[n]; }
	inline X &curve(int n) { return m_Curve[n]; }
public:
	Curvature() {}
	inline void setElement(int n, X const v) { curve(n) = v; }
	void init(VEC<1,int> const &dim) {
		int n = dim[0];
		m_Curve.resize(n);
	}
};

/**
 * Zero-dimensional Curvatures are not allowed.
 */
template <class X>
class Curvature<0, X> {
	Curvature() {}
};




/**
 * N-dimensional lookup table.
 *
 * Converts (partially) irregularly spaced data to a regularly spaced 
 * lookup table using linear or spline interpolation.  The table is
 * then accessed by fast linear interpolation.
 */
template <int N, class X=float>
class LUT: public InterpolationType<X> {
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
		if (*m_Ref <= 0) {
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

	void createData(int n) {
		deref();
		m_Data = new DataVector(n);
		__LUTLOG("NEW-N " << int(m_Data));
		m_Ref = new int(0);
		ref();
	}

	void postInterpolation(TableVector *_table) {
		X x0 = data(0).first;
		X x1 = data(dataSize()-1).first;
		substitute(_table);
		int n = tableSize()-1;
		InterpolationType<X>::postInterpolation(x0, x1, n);
	}

	void interpolateLinear(int n, TableVector *_table) {
		int i, j = 0;
		X x0 = data(0).first;
		X x1 = data(dataSize()-1).first;
		X dx = (x1 - x0) / (n-1);
		typename DataVector::iterator c1 = m_Data->begin();
		typename DataVector::iterator c0 = c1++;
		X s = 1.0 / (c1->first - c0->first);
		for (i = 0; i < n; ++i) {
			X x = x0 + i * dx;
			while (x > c1->first && c1+1 != m_Data->end()) {
				c0 = c1++;
				if (c1->first <= c0->first) {
					throwBreakpointOrder();
				}
				s = 1.0 / (c1->first - c0->first);
			}
			X f = (x - c0->first) * s;
			c0->second.__interpolate(f, c1->second, (*_table)[i]);
		}
	}

	void computeCurvature(Curvature<N,X> &c) {
		std::vector<int> dim;
		__getDimension(dim);
		c.init(Dim(dim));
		int n = dataSize();
		std::vector<X> xv(n), yv(n), uv(n);
		int elements = 1;
		for (int i = 1; i < N; i++) {
			elements *= dim[i];
		}
		for (int i = 0; i < n; i++) {
			xv[i] = data(i).first;
		}
		for (int element = 0; element < elements; ++element) {
			for (int i = 0; i < n; i++) {
				yv[i] = data(i).second.__getElement(element);
			}
			cspline<X>(xv, yv, uv);
			for (int i = 0; i < n; i++) {
				c.curve(i).setElement(element, uv[i]);
			}
		}
	}

	void interpolateSpline(int n, TableVector *_table) {
		// first compute second derivatives with respect to
		// the current dimension.
		Curvature<N,X> c;
		computeCurvature(c);

		// now interpolate along the current dimension
		int n0 = dataSize();
		X x0 = data(0).first;
		X xn = data(n0-1).first;
		X dx = (xn - x0) / (n-1);
		int index = 1;
		X xa = data(0).first;
		X xb = data(1).first;
		X h = xb - xa;
		if (xb <= xa) {
			throwBreakpointOrder();
		}
		X s = static_cast<X>(1.0) / h;
		for (int i = 0; i < n; ++i) {
			X x = x0 + i * dx;
			while (x > xb && index < n0-1) {
				++index;
				xa = xb;
				xb = data(index).first;
				h = xb - xa;
				if (xb <= xa) {
					throwBreakpointOrder();
				}
				s = static_cast<X>(1.0) / h;
			}
			X f = (x - xa) * s;
			Y::__splineInterpolate(f, h, data(index-1).second, data(index).second, c.curve(index-1), c.curve(index), (*_table)[i]);
		}
	}


friend class LUT<N+1,X>;
private:
	// spline interpolation
	static void __splineInterpolate(X x, X h, LUT<N,X> const &y0, LUT<N,X> const &y1, 
	              Curvature<N,X> const &c0, 
	              Curvature<N,X> const &c1, 
	              LUT<N,X> &out) {
		int n = y0.dataSize();
		assert(y1.dataSize() == n);
		out.createData(n);
		for (int i = 0; i < n; i++) {
			out.data(i).first = y0.data(i).first;
			Y::__splineInterpolate(
				x, h, y0.data(i).second, y1.data(i).second,
				c0.curve(i), c1.curve(i),
				out.data(i).second
			);
		}
	}

	// 2 LUT parameter version does linear interpolation
	void __interpolate(X x, LUT<N,X> const &next, LUT<N,X> &out) {
		int n = dataSize();
		assert(next.dataSize() == n);
		out.createData(n);
		for (int i = 0; i < n; i++) {
			out.data(i).first = data(i).first;
			data(i).second.__interpolate(x, next.data(i).second, out.data(i).second);
		}
	}
	
	X __getElement(int n) {
		int i = n % dataSize();
		return data(i).second.__getElement((n-i)/dataSize());
	}

	void __getDimension(std::vector<int> &dim) {
		dim.push_back(dataSize());
		data(0).second.__getDimension(dim);
	}


public:
	typedef VEC<N, int> Dim;
	typedef VEC<N, X> Vec;
	typedef VEC<N, std::vector<X> > Breaks;

	LUT(): m_Data(0), m_Ref(0) { }

	LUT(LUT<N, X> const &copy): InterpolationType<X>(copy), m_Data(copy.m_Data), m_Ref(copy.m_Ref) {
		ref();
	}

	LUT<N, X> const &operator=(LUT<N, X> const &copy) {
		InterpolationType<X>::operator=(copy);
		deref();
		m_Data = copy.m_Data;
		m_Ref = copy.m_Ref;
		ref();
	}

	~LUT() {
		deref();
	}

	void interpolate(Dim const &dim, Interpolation::Modes mode) {
		checkNotInterpolated();
		int n = dim[0];
		assert(n >= 2 && m_Data && dataSize() >= 2);
		TableVector *_table = new TableVector(n);
		__LUTLOG("NEWDN " << int(_table));
		switch (mode) {
			case LINEAR:
				interpolateLinear(n, _table);
				break;
			case SPLINE:
				interpolateSpline(n, _table);
				break;
			default:
				throwInterpolationMode();
		}
		for (int i = 0; i < n; ++i) {
			__LUTLOG("SUB INTERP " << i << " of " << n);
			(*_table)[i].interpolate(dim.rest(), mode);
		}
		postInterpolation(_table);
	}

	void interpolate(std::vector<int> const &dim, Interpolation::Modes mode) {
		interpolate(Dim(dim), mode);
	}

	X getValue(Vec const &v) const {
		checkInterpolated();
		X x = v[0];
		int i;
		X f;
		find(x, i, f);
		X value = table(i).getValue(v.rest()) * (1.0-f) + table(i+1).getValue(v.rest()) * f;
		__LUTLOG("getValue[N]: " << x << " " << i << " " << f << " = " << value);
		return value;
	}

	inline X getValue(std::vector<X> const &x) const {
		return getValue(Vec(x));
	}

	inline WRAP<N,X> operator[](X x) const {
		checkInterpolated();
		return WRAP<N,X>(this, x);
	}

	void load(std::vector<X> const &values, Breaks const &breaks, int *index = 0) {
		checkNotInterpolated();
		std::vector<X> &br = breaks[0];
		int n = br.size(), index0 = 0;
		if (!index) index = &index0;
		createData(n);
		for (int i = 0; i < n; i++) {
			data(i).first = br[i];
			data(i).second.load(values, breaks.rest(), index);
		}
	}

	void load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks) {
		if (breaks.size() != N) {
			throw InterpolationIndex("Incorrect number of breakpoint arrays");
		}
		load(values, Breaks(breaks));
	}

	/**
	 * Serialize an object to a data archive.
	 */
	virtual void pack(Packer& p) const {
		checkInterpolated();
		int n = tableSize();
		p.pack(int(N));
		p.pack(m_X0);
		p.pack(m_X1);
		p.pack(n);
		for (int i = 0; i < n; i++) {
			table(i).pack(p);
		}
	}
	
	/**
	 * Deserialize an object from a data archive.
	 */
	virtual void unpack(UnPacker& p) {
		checkNotInterpolated();
		X x0, x1;
		int dim, n;
		p.unpack(dim);
		if (dim != N) {
			char msg[128];
			sprintf(msg, "LUT<%d>::unpack table of dimension %d", N, dim);
			throw InterpolationUnpackMismatch(msg);
		}
		p.unpack(x0);
		p.unpack(x1);
		p.unpack(n);
		TableVector *_table = new TableVector(n);
		for (int i = 0; i < n; i++) {
			p.unpack((*_table)[i]);
		}
		substitute(_table);
		InterpolationType<X>::postInterpolation(x0, x1, n-1);
	}

	/**
	 * Return strig representation of type.
	 */
	virtual std::string asString() const {
		char buff[128];
		sprintf(buff, "<%d-Dimensional Lookup Table>", N);
		return buff;
	}
};

template <typename X>
class LUT<1, X>: public InterpolationType<X> {
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
		if (*m_Ref <= 0) {
			if (m_Data) {
				__LUTLOG("~DATA " << int(m_Data));
				delete m_Data;
			}
			m_Data = 0;
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

	void createData(int n) {
		DataVector *old = m_Data;
		deref();
		m_Data = new DataVector(n);
		__LUTLOG("NEW-1 " << int(m_Data) << " (was " << int(old) << ")");
		m_Ref = new int(0);
		ref();
	}

	void postInterpolation(TableVector *_table) {
		X x0 = data(0).first;
		X x1 = data(dataSize()-1).first;
		substitute(_table);
		int n = tableSize()-1;
		InterpolationType<X>::postInterpolation(x0, x1, n);
	}

	void interpolateLinear(int n, TableVector *_table) {
		int j = 0;
		X x0 = data(0).first;
		X x1 = data(dataSize()-1).first;
		X dx = (x1 - x0) / (n-1);
		typename DataVector::iterator c1 = m_Data->begin();
		typename DataVector::iterator c0 = c1++;
		X s = 1.0 / (c1->first - c0->first);
		for (int i = 0; i < n; ++i) {
			X x = x0 + i * dx;
			while (x > c1->first && c1+1 != m_Data->end()) {
				c0 = c1++;
				if (c1->first <= c0->first) {
					throwBreakpointOrder();
				}
				s = 1.0 / (c1->first - c0->first);
			}
			X f = (x - c0->first) * s;
			(*_table)[i] = (1.0 - f) * c0->second + f * c1->second;
		}
	}

	void interpolateSpline(int n, TableVector *_table) {
		int n0 = dataSize();
		std::vector<X> xv(n0), yv(n0), uv(n0);
		for (int i = 0; i < n0; i++) {
			xv[i] = data(i).first;
			yv[i] = data(i).second;
		}
		cspline<X>(xv, yv, uv);
		X x0 = data(0).first;
		X x1 = data(n0-1).first;
		X dx = (x1 - x0) / (n-1);
		int index = 1;
		X xa = data(0).first;
		X ya = data(0).second;
		X xb = data(1).first;
		X yb = data(1).second;
		X h = xb - xa;
		X s = 1.0 / h;
		X d2s = h * h * 0.166666667;
		for (int i = 0; i < n; ++i) {
			X x = x0 + i * dx;
			while (x > xb && index < n0-1) {
				++index;
				xa = xb;
				xb = data(index).first;
				ya = yb;
				yb = data(index).second;
				h = xb - xa;
				s = 1.0 / h;
				d2s = h * h * 0.166666667;
			}
			X f = (x - xa) * s;
			X g = 1.0 - f;
			(*_table)[i] = g * ya +
				       f * yb +
				       ((g*g-1.0)*g*uv[index-1] +
					(f*f-1.0)*f*uv[index]) * d2s;
		}
	}

friend class LUT<2,X>;
private:

	inline X __getElement(int n) {
		return data(n).second;
	}

	inline void __getDimension(std::vector<int> &dim) {
		dim.push_back(dataSize());
	}

	// linear interpolation between this and next
	void __interpolate(X x, LUT<1,X> const &next, LUT<1,X> &out) {
		int n = dataSize();
		assert(next.dataSize() == n);
		out.createData(n);
		X y = static_cast<X>(1.0) - x;
		for (int i = 0; i < n; i++) {
			out.data(i).first = data(i).first;
			out.data(i).second = y * data(i).second + x * next.data(i).second;
		}
	}

	// spline interpolation 
	static void __splineInterpolate(
			X x, X h, 
			LUT<1,X> const &y0, 
			LUT<1,X> const &y1, 
			Curvature<1,X> const &c0, 
			Curvature<1,X> const &c1, 
			LUT<1,X> &out) 
	{
		int n = y0.dataSize();
		assert(y1.dataSize() == n);
		out.createData(n);
		X y = static_cast<X>(1.0) - x;
		X d2s = h * h * static_cast<X>(0.1666666666667);
		for (int i = 0; i < n; i++) {
			out.data(i).first = y0.data(i).first;
			out.data(i).second = y * y0.data(i).second + 
			                     x * y1.data(i).second +
			                     ((y * y - 1.0) * y * c0.curve(i) +
			                      (x * x - 1.0) * x * c1.curve(i)) * d2s;
		}
	}

public:
	typedef VEC<1, int> Dim;
	typedef VEC<1, X> Vec;
	typedef VEC<1, std::vector<X> > Breaks;

	X __get(int n) { return data(n).second; }

	LUT(): m_Data(0), m_Ref(0) { }

	~LUT() {
		deref();
	}

	LUT(LUT<1, X> const &copy): InterpolationType<X>(copy), m_Data(copy.m_Data), m_Ref(copy.m_Ref) {
		__LUTLOG("COPY IMP " << int(m_Data));
		ref();
	}

	LUT<1, X> const &operator=(LUT<1, X> const &copy) {
		__LUTLOG("COPY EXP " << int(m_Data) << " " << int(copy.m_Data));
		InterpolationType<X>::operator=(copy);
		deref();
		m_Data = copy.m_Data;
		m_Ref = copy.m_Ref;
		ref();
	}

	void interpolate(Dim const &dim, Interpolation::Modes mode) {
		checkNotInterpolated();
		int n = dim[0];
		assert(n >= 2 && dataSize() >= 2);
		TableVector *_table = new TableVector(n);
		assert(_table != 0);
		__LUTLOG("NEWD1 " << int(_table));
		switch (mode) {
			case LINEAR:
				interpolateLinear(n, _table);
				break;
			case SPLINE:
				interpolateSpline(n, _table);
				break;
			default:
				throwInterpolationMode();
		}
		postInterpolation(_table);
	}

	void interpolate(std::vector<int> const &dim, Interpolation::Modes mode) {
		interpolate(Dim(dim), mode);
	}

	X getValue(Vec const &v) const {
		checkInterpolated();
		X x = v[0];
		int i;
		X f;
		find(x, i, f);
		X value = table(i) * (1.0-f) + table(i+1) * f;
		__LUTLOG("getValue[1]: " << x << " " << i << " " << f << " = " << value);
		return value;
	}

	inline X getValue(std::vector<X> const &x) const {
		return getValue(Vec(x));
	}

	inline X operator[](Vec const &v) const {
		return getValue(v);
	}

	void load(std::vector<X> const &values, Breaks const &breaks, int *index = 0) {
		checkNotInterpolated();
		std::vector<X> const &br = breaks[0];
		int n = br.size(), index0 = 0;
		if (!index) index = &index0;
		createData(n);
		for (int i = 0; i < n; i++) {
			data(i).first = br[i];
			data(i).second = values[*index];
			++(*index);
		}
	}

	void load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks) {
		if (breaks.size() != 1) {
			throw InterpolationIndex("Incorrect number of breakpoint arrays");
		}
		load(values, Breaks(breaks));
	}

	/**
	 * Serialize an object to a data archive.
	 */
	virtual void pack(Packer& p) const {
		checkInterpolated();
		int n = tableSize();
		p.pack(int(1));
		p.pack(m_X0);
		p.pack(m_X1);
		p.pack(n);
		for (int i = 0; i < n; i++) {
			p.pack(table(i));
		}
	}
	
	/**
	 * Deserialize an object from a data archive.
	 */
	virtual void unpack(UnPacker& p) {
		checkNotInterpolated();
		X x0, x1;
		int dim, n;
		p.unpack(dim);
		if (dim != 1) {
			char msg[128];
			sprintf(msg, "LUT<1>::unpack table of dimension %d", dim);
			throw InterpolationUnpackMismatch(msg);
		}
		p.unpack(x0);
		p.unpack(x1);
		p.unpack(n);
		TableVector *_table = new TableVector(n);
		for (int i = 0; i < n; i++) {
			p.unpack((*_table)[i]);
		}
		substitute(_table);
		InterpolationType<X>::postInterpolation(x0, x1, n-1);
	}

	/**
	 * Return strig representation of type.
	 */
	virtual std::string asString() const {
		return "<1-Dimensional Lookup Table>";
	}
};

/**
 * No zero-dimensional LUTs
 */
template <typename X>
class LUT<0, X>: public Interpolation {
	LUT() {}
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
 * Define a few common LUT types.
 */
typedef LUT<1,float> Table1;
typedef LUT<2,float> Table2;
typedef LUT<3,float> Table3;


NAMESPACE_END // simdata


#if 0
void load2(simdata::Table2 &t) {
	float f;
	int a, b, n;
	std::cin >> a;
	std::cin >> b;
	n = a*b;
	std::vector<float> x(a), y(b);
	std::vector<float> values(n);
	for (int i = 0; i < a; i++) std::cin >> x[i];
	for (int i = 0; i < b; i++) std::cin >> y[i];
	for (int i = 0; i < n; i++) std::cin >> values[i];
	// data in 'thrust' input file is transposed relative to
	// the standard indexing [M][ALT].   either switich to
	// [ALT][M], change the input file, or transpose on the
	// fly as we do here:
	std::vector<float> transpose(n);
	for (int i = 0; i < b; i++) {
		for (int j = 0; j < a; j++) {
			transpose[j*b+i] = values[i*a+j];
		}
	}
	t.load(transpose, simdata::Table2::Breaks(x)(y));
}


void test() {
	simdata::Table2 t;
	load2(t);
	simdata::SimTime t0 = simdata::SimDate::getSystemTime();
	t.interpolate(simdata::Table2::Dim(100)(40), simdata::Interpolation::SPLINE);
	//t.interpolate(simdata::Table2::Dim(40)(20), simdata::Interpolation::LINEAR);
	simdata::SimTime t1 = simdata::SimDate::getSystemTime();
	std::cout << (t1-t0)*1000000.0 <<  " us\n";
	std::cout << t[0][0] << " = 0,0\n";
	std::cout << t[0.2][0] << " = 0.2,0\n";
	std::cout << t[0.4][0] << " = 0.4,0\n";
	std::cout << t[2.2][0] << " = 2.2,0\n";
	std::cout << t[0.5][100] << " = 0.5,100\n";
	std::cout << t[0.0][3048] << " = 0.0,3k\n";
	std::cout << t[0.0][6096] << " = 0.0,6k\n";
	std::cout << t[0.0][30480] << " = 0.0,30k\n";
	std::cout << t[1.0][12192] << " = 1.0,12k\n";
	std::cout << t[2.2][30480] << " = 2.2,30k\n";

	std::cerr << "P2 100 100 255\n";
	for (int i = 0; i < 100; i++) {
	for (int j = 0; j < 100; j++) {
		//std::cerr << int(t[i*0.01*2.2][j*0.01*30480]*0.002+128) << " ";
		std::cerr << int(t[i*0.01*2.2][j*0.01*30480]*100+128) << " ";
	}
	}

	t0 = simdata::SimDate::getSystemTime();
	float x = 0.0;
	for (int i = 100000; i > 0; --i) {
		x += t[2.1][30400]; 
	}
	t1 = simdata::SimDate::getSystemTime();
	std::cout << (t1-t0)*10.0 << " us; " << x <<  "\n";

	/*
	simdata::Table3 t3;
	t0 = simdata::SimDate::getSystemTime();
	// XXX must set data before interpolating
	t3.interpolate(simdata::Table3::Dim(100)(40)(20), simdata::Interpolation::SPLINE);
	t1 = simdata::SimDate::getSystemTime();
	std::cout << (t1-t0) << " s\n";
	*/
}

int main() try { 
	test();
	return 0; 
} catch(...) { }

#endif

#endif // __SIMDATA_LUT_H__

