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

#include <SimData/Spread.h>
#include <SimData/Random.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA


// class Spread

void Spread::pack(Packer &p) const {
	p.pack(_mean);
	p.pack(_sigma);
}

void Spread::unpack(UnPacker &p) {
	p.unpack(_mean);
	p.unpack(_sigma);
	regen();
}

void Spread::parseXML(const char* cdata) {
	std::string s(cdata);
	float mean, sigma = 0.0;
	bool ok = true;
	std::string::size_type pos = s.find(':');
	if (pos == std::string::npos) {
		int n = sscanf(s.c_str(), "%f", &mean);
		if (n != 1) ok = false;
	} else {
		int n = sscanf(s.c_str(), "%f:%f", &mean, &sigma);
		if (n != 2) ok = false;
	}
	set(mean, sigma);
	if (!ok) throw ParseException("SYNTAX ERROR: expect 'float' or 'float:float'");
}

Spread::Spread(float mean, float sigma) {
	set(mean, sigma);
	regen();
}

void Spread::set(float mean, float sigma) {
	_mean = mean;
	_sigma = sigma;
	regen();
}

void Spread::regen() { 
	if (_sigma <= 0.0)
		_value = _mean;
	else
		_value = box_muller(_mean, _sigma);
}

float Spread::getMean() { return _mean; }

float Spread::getSigma() { return _sigma; }

float Spread::getValue() { return _value; }

std::string Spread:: asString() const { 
	char fmt[128];
	snprintf(fmt, 128, "%f", _value);
	return std::string(fmt);
}

float Spread::__neg__() { return -_value; }
float Spread::__pos__() { return _value; }
float Spread::__abs__() { return static_cast<float>(fabs(_value)); }
int Spread::__nonzero__() { return _value != 0.0; }
float Spread::__float__() { return _value; }
int Spread::__int__() { return (int)_value; }
long Spread::__long__() { return (long)_value; }
float Spread::__add__(float v) { return v + _value; }
float Spread::__radd__(float v) { return v + _value; }
float Spread::__sub__(float v) { return _value - v; }
float Spread::__rsub__(float v) { return v - _value; }
float Spread::__mul__(float v) { return v * _value; }
float Spread::__rmul__(float v) { return v * _value; }
float Spread::__div__(float v) { assert(v != 0.0); return _value / v; }
float Spread::__rdiv__(float v) { assert(_value != 0.0); return v / _value; }
float Spread::__rpow__(float v) { return static_cast<float>(pow(v, _value)); }
float Spread::__pow__(float v) { return static_cast<float>(pow(_value, v)); }


NAMESPACE_END // namespace simdata

