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


#ifndef __REAL_H__
#define __REAL_H__

#include <string>
#include <math.h>
#include <SimData/BaseType.h>


NAMESPACE_SIMDATA


/**
 * class Real
 */
class SIMDATA_EXPORT Real: public BaseType {
	float _mean, _sigma;
	float _value;
public:
	Real(float mean=0.0, float sigma=0.0);
	void set(float mean, float sigma);
	void regen();
	float getMean();
	float getSigma();
	float getValue();

	virtual std::string  asString() const;

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

#ifndef SWIG
	inline operator float() { return _value; }
#endif

	virtual void pack(Packer &p) const;
	virtual void unpack(UnPacker &p);
	virtual void parseXML(const char* cdata);

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

NAMESPACE_END // namespace simdata

#endif //__REAL_H__
