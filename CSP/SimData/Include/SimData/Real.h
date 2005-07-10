/* SimData: Data Infrastructure for Simulations
 * Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Real.h
 * @brief A guassian distributed floating-point value.
 */


#ifndef __SIMDATA_REAL_H__
#define __SIMDATA_REAL_H__

#include <SimData/BaseType.h>
#include <SimData/Random.h>

#include <string>
#include <cmath>


NAMESPACE_SIMDATA


/**
 * @brief A guassian distributed floating-point value.
 *
 * This class represents a "real-world" value distributed according
 * to a normal distribution.  Reals are specified by a median value
 * and a standard deviation.  When constructed the actual value is
 * generated from this distribution.
 *
 * @author Mark Rose <mkrose@users.sf.net>
 * @ingroup BaseTypes
 */
class SIMDATA_EXPORT Real {
	static random::Taus2 _rng;
	float _mean, _sigma;
	float _value;

public: // BaseType

	/// String representation.
	std::string asString() const;

	/// Type representation.
	std::string typeString() const { return "type::Real"; }

	/// Serialize from a Reader.
	void serialize(Reader&);

	/// Serialize to a Writer.
	void serialize(Writer&) const;

	/** Parse the character data from an XML \<Real\> tag.
	 *  The format is "mean:sigma"
	 */
	void parseXML(const char*);

	/// XML post processing.
	void convertXML() {}

public:
	/**
	 * Create a new real with the specified distribution.
	 *
	 * @param mean the mean value of the normal distribution.
	 * @param sigma the standard deviation of the distribution.
	 */
	Real(float mean=0.0, float sigma=0.0);

	/**
	 * Set the distribution paramaters.
	 *
	 * @param mean the mean value of the normal distribution.
	 * @param sigma the standard deviation of the distribution.
	 */
	void set(float mean, float sigma);

	/**
	 * Choose a new value from the distribution.
	 */
	void regen();

	/**
	 * Get the mean value of the distribution.
	 */
	float getMean() const;
	
	/**
	 * Get the standard deviation of the distribution.
	 */
	float getSigma() const;
	
	/**
	 * Get the actual value.
	 */
	float getValue() const;

	/*
    	float __neg__();
	float __pos__();
	float __abs__();
    	int __nonzero__();
	float __float__();
	int __int__();
	long __long__();
	float __add__(float v);
	float __radd__(float v);
	float __sub__(float v);
	float __rsub__(float v);
	float __mul__(float v);
	float __rmul__(float v);
	float __div__(float v);
	float __rdiv__(float v);
	float __rpow__(float v);
	float __pow__(float v);
	*/

#ifndef SWIG
	/**
	 * Allow implicit coersion to a floating point value.
	 */
	inline operator float() const { return _value; }
#endif

// insert Python shadow class code to emulate "operator float()"
#ifdef SWIG______NONO
%insert("shadow") %{
    def __coerce__(self, other):
        if isinstance(other, Real):
            return (self.getValue(), other.getValue())
        return (self.getValue(), other)
%}
#endif // SWIG

};

#ifdef SWIG
%extend Real {
	float __neg__() { return -(*self); }
	float __pos__() { return (*self); }
	float __abs__() { return static_cast<float>(fabs(*self)); }
	float __add__(float v) { return v + (*self); }
	float __radd__(float v) { return v + (*self); }
	float __sub__(float v) { return (*self) - v; }
	float __rsub__(float v) { return v - (*self); }
	float __mul__(float v) { return v * (*self); }
	float __rmul__(float v) { return v * (*self); }
	float __div__(float v) { assert(v != 0.0); return (*self) / v; }
	float __rdiv__(float v) { assert((*self) != 0.0); return v / (*self); }
	float __rpow__(float v) { return static_cast<float>(pow(v, (*self))); }
	float __pow__(float v) { return static_cast<float>(pow((*self), v)); }
	int __int__() { return (int)(*self); }
	long __long__() { return (long)(*self); }
	float __float__() { return (*self); }
	int __nonzero__() { return (*self) != 0.0; }
}
#endif // SWIG


SIMDATA_EXPORT std::ostream &operator <<(std::ostream &o, Real const &r);


NAMESPACE_SIMDATA_END

#endif //__SIMDATA_REAL_H__
