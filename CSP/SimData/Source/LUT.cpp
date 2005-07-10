/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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


#include <SimData/LUT.h>

#include <sstream>

#ifndef __SIMDATA_NO_LUT__


NAMESPACE_SIMDATA


template class LUT<1, float>;
template class LUT<2, float>;
template class LUT<3, float>;


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


/**
 * @brief A helper class for storing second-derivatives.
 *
 * A helper class for storing second-derivatives used
 * in spline interpolation.  The structure mirrors that
 * of LUT<N,X>, with nested vectors of progressively
 * lower dimensional Curvature instances.
 *
 * You should never need to use this class directly.
 */
template <int N, class X>
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
 * @brief Partial specialization of Curvature for one-dimension.
 *
 * This is essentially just a std::vector<X>.  See Curvature
 * (N>1) for details.
 */
template <class X>
class Curvature<1, X> {
	typedef std::vector<X> CurveVector;
	CurveVector m_Curve;

// ideally, these methods should be private, but g++-3.2
// doesn't accept partially specialized template friends.
// so for now they must be declare public.
public:
//friend class LUT<1,X>;

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
 * @brief Stub to prevent zero-dimensional Curvatures.
 */
template <class X>
class Curvature<0, X> {
	Curvature();
};



/**
 */
template <typename X>
void InterpolationType<X>::postInterpolation(X x0, X x1, int n) {
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


/**
 * N-dimensional lookup table.
 *
 * Converts (partially) irregularly spaced data to a regularly spaced
 * lookup table using linear or spline interpolation.  The table is
 * then accessed by fast linear interpolation.
 */
template <int N, class X>
void LUT<N,X>::createData(int n) {
	deref();
	m_Data = new DataVector(n);
	m_Ref = new int(0);
	ref();
}

template <int N, class X>
void LUT<N,X>::postInterpolation(TableVector *_table) {
	X x0 = data(0).first;
	X x1 = data(dataSize()-1).first;
	substitute(_table);
	int n = tableSize()-1;
	InterpolationType<X>::postInterpolation(x0, x1, n);
}

template <int N, class X>
void LUT<N,X>::interpolateLinear(int n, TableVector *_table) {
	int i;
	X x0 = data(0).first;
	X x1 = data(dataSize()-1).first;
	X dx = (x1 - x0) / (n-1);
	typename DataVector::iterator c1 = m_Data->begin();
	typename DataVector::iterator c0 = c1++;
	X s = static_cast<X>(1.0) / (c1->first - c0->first);
	for (i = 0; i < n; ++i) {
		X x = x0 + i * dx;
		while (x > c1->first && c1+1 != m_Data->end()) {
			c0 = c1++;
			if (c1->first <= c0->first) {
				this->throwBreakpointOrder();
			}
			s = static_cast<X>(1.0) / (c1->first - c0->first);
		}
		X f = (x - c0->first) * s;
		c0->second.__interpolate(f, c1->second, (*_table)[i]);
	}
}

template <int N, class X>
void LUT<N,X>::computeCurvature(Curvature<N,X> &c) {
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

template <int N, class X>
void LUT<N,X>::interpolateSpline(int n, TableVector *_table) {
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
		this->throwBreakpointOrder();
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
				this->throwBreakpointOrder();
			}
			s = static_cast<X>(1.0) / h;
		}
		X f = (x - xa) * s;
		Y::__splineInterpolate(f, h, data(index-1).second, data(index).second, c.curve(index-1), c.curve(index), (*_table)[i]);
	}
}


// spline interpolation
template <int N, class X>
void LUT<N,X>::__splineInterpolate(X x, X h, LUT<N,X> const &y0, LUT<N,X> const &y1, Curvature<N,X> const &c0, Curvature<N,X> const &c1, LUT<N,X> &out) {
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
template <int N, class X>
void LUT<N,X>::__interpolate(X x, LUT<N,X> const &next, LUT<N,X> &out) {
	int n = dataSize();
	assert(next.dataSize() == n);
	out.createData(n);
	for (int i = 0; i < n; i++) {
		out.data(i).first = data(i).first;
		data(i).second.__interpolate(x, next.data(i).second, out.data(i).second);
	}
}

template <int N, class X>
X LUT<N,X>::__getElement(int n) {
	int i = n % dataSize();
	return data(i).second.__getElement((n-i)/dataSize());
}

template <int N, class X>
void LUT<N,X>::__getDimension(std::vector<int> &dim) {
	dim.push_back(dataSize());
	data(0).second.__getDimension(dim);
}


template <int N, class X>
LUT<N,X>::LUT(): m_Data(0), m_Ref(0) { }

template <int N, class X>
LUT<N,X>::LUT(LUT<N, X> const &copy): InterpolationType<X>(copy), m_Data(copy.m_Data), m_Ref(copy.m_Ref) {
	ref();
}

template <int N, class X>
LUT<N, X> const &LUT<N,X>::operator=(LUT<N, X> const &copy) {
	InterpolationType<X>::operator=(copy);
	deref();
	m_Data = copy.m_Data;
	m_Ref = copy.m_Ref;
	ref();
	return *this;
}

template <int N, class X>
LUT<N,X>::~LUT() {
	deref();
}

template <int N, class X>
void LUT<N,X>::interpolate(Dim const &dim, Interpolation::Modes mode) {
	this->checkNotInterpolated();
	int n = dim[0];
	assert(n >= 2 && m_Data && dataSize() >= 2);
	TableVector *_table = new TableVector(n);
	switch (mode) {
		case Interpolation::LINEAR:
			interpolateLinear(n, _table);
			break;
		case Interpolation::SPLINE:
			interpolateSpline(n, _table);
			break;
		default:
			this->throwInterpolationMode();
	}
	for (int i = 0; i < n; ++i) {
		(*_table)[i].interpolate(dim.rest(), mode);
	}
	postInterpolation(_table);
}

template <int N, class X>
void LUT<N,X>::interpolate(std::vector<int> const &dim, Interpolation::Modes mode) {
	interpolate(Dim(dim), mode);
}

template <int N, class X>
X LUT<N,X>::getValue(Vec const &v) const {
	this->checkInterpolated();
	X x = v[0];
	int i;
	X f;
	find(x, i, f);
	X value = table(i).getValue(v.rest()) * (static_cast<X>(1.0)-f) + table(i+1).getValue(v.rest()) * f;
	return value;
}

template <int N, class X>
void LUT<N,X>::load(std::vector<X> const &values, Breaks const &breaks, int *index) {
	this->checkNotInterpolated();
	std::vector<X> &br = breaks[0];
	int n = br.size(), index0 = 0;
	if (!index) index = &index0;
	createData(n);
	for (int i = 0; i < n; i++) {
		data(i).first = br[i];
		data(i).second.load(values, breaks.rest(), index);
	}
}

template <int N, class X>
void LUT<N,X>::load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks) {
	if (breaks.size() != N) {
		throw InterpolationIndex("Incorrect number of breakpoint arrays");
	}
	load(values, Breaks(breaks));
}

template <int N, class X>
void LUT<N,X>::serialize(Reader &reader) {
	this->checkNotInterpolated();
	X x0, x1;
	int dim, n;
	reader >> dim;
	if (dim != N) {
		std::ostringstream msg;
		msg << "LUT<" << N << ">::serialize table of dimension " << dim;
		throw InterpolationUnpackMismatch(msg.str());
	}
	reader >> x0;
	reader >> x1;
	reader >> n;
	TableVector *_table = new TableVector(n);
	for (int i = 0; i < n; i++) {
		reader >> ((*_table)[i]);
	}
	substitute(_table);
	InterpolationType<X>::postInterpolation(x0, x1, n-1);
}

template <int N, class X>
void LUT<N,X>::serialize(Writer &writer) const {
	this->checkInterpolated();
	int n = tableSize();
	int dim = N;
	writer << dim;
	writer << this->m_X0;
	writer << this->m_X1;
	writer << n;
	for (int i = 0; i < n; ++i) {
		table(i).serialize(writer);
	}
}

template <int N, class X>
std::string LUT<N,X>::asString() const {
	std::ostringstream ss;
	ss << "<simdata::" << N << "D Lookup Table>";
	return ss.str();
}

template <int N, class X>
std::string LUT<N,X>::typeString() const {
	std::ostringstream ss;
	ss << "type::LUT" << N << ">";
	return ss.str();
}


template <typename X>
void LUT<1,X>::createData(int n) {
	//DataVector *old = m_Data;
	deref();
	m_Data = new DataVector(n);
	m_Ref = new int(0);
	ref();
}

template <typename X>
void LUT<1,X>::postInterpolation(TableVector *_table) {
	X x0 = data(0).first;
	X x1 = data(dataSize()-1).first;
	substitute(_table);
	int n = tableSize()-1;
	InterpolationType<X>::postInterpolation(x0, x1, n);
}

template <typename X>
void LUT<1,X>::interpolateLinear(int n, TableVector *_table) {
	X x0 = data(0).first;
	X x1 = data(dataSize()-1).first;
	X dx = (x1 - x0) / (n-1);
	typename DataVector::iterator c1 = m_Data->begin();
	typename DataVector::iterator c0 = c1++;
	X s = static_cast<X>(1.0) / (c1->first - c0->first);
	for (int i = 0; i < n; ++i) {
		X x = x0 + i * dx;
		while (x > c1->first && c1+1 != m_Data->end()) {
			c0 = c1++;
			if (c1->first <= c0->first) {
				this->throwBreakpointOrder();
			}
			s = static_cast<X>(1.0) / (c1->first - c0->first);
		}
		X f = (x - c0->first) * s;
		(*_table)[i] = (static_cast<X>(1.0) - f) * c0->second + f * c1->second;
	}
}

template <typename X>
void LUT<1,X>::interpolateSpline(int n, TableVector *_table) {
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
	X s = static_cast<X>(1.0) / h;
	X d2s = h * h * static_cast<X>(0.166666667);
	for (int i = 0; i < n; ++i) {
		X x = x0 + i * dx;
		while (x > xb && index < n0-1) {
			++index;
			xa = xb;
			xb = data(index).first;
			ya = yb;
			yb = data(index).second;
			h = xb - xa;
			s = static_cast<X>(1.0) / h;
			d2s = h * h * static_cast<X>(0.166666667);
		}
		X f = (x - xa) * s;
		X g = static_cast<X>(1.0) - f;
		(*_table)[i] = g * ya + f * yb + ((g*g-static_cast<X>(1.0))*g*uv[index-1] + (f*f-static_cast<X>(1.0))*f*uv[index]) * d2s;
	}
}


// linear interpolation between this and next
template <typename X>
void LUT<1,X>::__interpolate(X x, LUT<1,X> const &next, LUT<1,X> &out) {
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
template <typename X>
void LUT<1,X>::__splineInterpolate(
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
		out.data(i).second =
			y * y0.data(i).second + x * y1.data(i).second +
			((y * y - static_cast<X>(1.0)) * y * c0.curve(i) +
			(x * x - static_cast<X>(1.0)) * x * c1.curve(i)) * d2s;
	}
}


template <typename X>
LUT<1,X>::LUT(): m_Data(0), m_Ref(0) { }

template <typename X>
LUT<1,X>::~LUT() {
	deref();
}

template <typename X>
LUT<1,X>::LUT(LUT<1, X> const &copy): InterpolationType<X>(copy), m_Data(copy.m_Data), m_Ref(copy.m_Ref) {
	ref();
}

template <typename X>
LUT<1, X> const &LUT<1,X>::operator=(LUT<1, X> const &copy) {
	InterpolationType<X>::operator=(copy);
	deref();
	m_Data = copy.m_Data;
	m_Ref = copy.m_Ref;
	ref();
	return *this;
}

template <typename X>
void LUT<1,X>::interpolate(Dim const &dim, Interpolation::Modes mode) {
	this->checkNotInterpolated();
	int n = dim[0];
	assert(n >= 2 && dataSize() >= 2);
	TableVector *_table = new TableVector(n);
	assert(_table != 0);
	switch (mode) {
		case Interpolation::LINEAR:
			interpolateLinear(n, _table);
			break;
		case Interpolation::SPLINE:
			interpolateSpline(n, _table);
			break;
		default:
			this->throwInterpolationMode();
	}
	postInterpolation(_table);
}

template <typename X>
void LUT<1,X>::interpolate(std::vector<int> const &dim, Interpolation::Modes mode) {
	interpolate(Dim(dim), mode);
}

template <typename X>
X LUT<1,X>::getValue(Vec const &v) const {
	this->checkInterpolated();
	X x = v[0];
	int i;
	X f;
	find(x, i, f);
	X value = table(i) * (static_cast<X>(1.0)-f) + table(i+1) * f;
	return value;
}

template <typename X>
void LUT<1,X>::load(std::vector<X> const &values, Breaks const &breaks, int *index) {
	this->checkNotInterpolated();
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

template <typename X>
void LUT<1,X>::load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks) {
	if (breaks.size() != 1) {
		throw InterpolationIndex("Incorrect number of breakpoint arrays");
	}
	load(values, Breaks(breaks));
}

template <typename X>
void LUT<1,X>::serialize(Reader &reader) {
	int dim = 1;
	reader >> dim;
	this->checkNotInterpolated();
	X x0, x1;
	int n;
	if (dim != 1) {
		std::ostringstream msg;
		msg << "LUT<1>::serialize table of dimension " << dim;
		throw InterpolationUnpackMismatch(msg.str());
	}
	reader >> x0;
	reader >> x1;
	reader >> n;
	TableVector *_table = new TableVector(n);
	for (int i = 0; i < n; i++) {
		reader >> ((*_table)[i]);
	}
	substitute(_table);
	InterpolationType<X>::postInterpolation(x0, x1, n-1);
}

template <typename X>
void LUT<1,X>::serialize(Writer &writer) const {
	int dim = 1;
	writer << dim;
	this->checkInterpolated();
	int n = tableSize();
	writer << this->m_X0;
	writer << this->m_X1;
	writer << n;
	for (int i = 0; i < n; i++) {
		writer << table(i);
	}
}

template <typename X>
std::string LUT<1,X>::asString() const {
	return "<simdata::1D Lookup Table>";
}

template <typename X>
std::string LUT<1,X>::typeString() const {
	return "type::LUT<1>";
}


std::ostream &operator <<(std::ostream &o, Table1 const &t) { return o << t.asString(); }
std::ostream &operator <<(std::ostream &o, Table2 const &t) { return o << t.asString(); }
std::ostream &operator <<(std::ostream &o, Table3 const &t) { return o << t.asString(); }


NAMESPACE_SIMDATA_END // simdata


#endif // __SIMDATA_NO_LUT__

