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

#include <SimData/Real.h>
#include <SimData/Random.h>
#include <SimData/Pack.h>

#include <sstream>


NAMESPACE_SIMDATA

// shared random number generator for Real instances
// XXX in multithreaded environments this *must* be replaced
// with a thread-safe generator. 
random::Taus2 Real::_rng;


// class Real

void Real::pack(Packer &p) const {
	p.pack(_mean);
	p.pack(_sigma);
}

void Real::unpack(UnPacker &p) {
	p.unpack(_mean);
	p.unpack(_sigma);
	regen();
}

void Real::parseXML(const char* cdata) {
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

Real::Real(float mean, float sigma) {
	set(mean, sigma);
	regen();
}

void Real::set(float mean, float sigma) {
	_mean = mean;
	_sigma = sigma;
	regen();
}

void Real::regen() { 
	if (_sigma <= 0.0) {
		_value = _mean;
	} else {
		_value = static_cast<float>(rd::BoxMueller(Real::_rng, _mean, _sigma));
		//box_muller(_mean, _sigma);
	}
}

float Real::getMean() const { return _mean; }

float Real::getSigma() const { return _sigma; }

float Real::getValue() const { return _value; }

std::string Real:: asString() const { 
	std::stringstream ss;
	ss << _value;
	return ss.str();
}



NAMESPACE_SIMDATA_END

