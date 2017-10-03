/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Real.cpp
 * @brief A guassian distributed floating-point value.
 */


#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Archive.h>
#include <csp/csplib/util/Random.h>

#include <sstream>


namespace csp {

// shared random number generator for Real instances
// XXX in multithreaded environments this *must* be replaced
// with a thread-safe generator.
random::Taus2 Real::_rng;


// class Real

void Real::serialize(Reader &reader) {
	reader >> _mean;
	reader >> _sigma;
	regen();
}

void Real::serialize(Writer &writer) const {
	writer << _mean;
	writer << _sigma;
}

void Real::parseXML(const char* cdata) {
	std::string s(cdata);
	float mean, sigma = 0.0;
	char separator;

	std::istringstream stream(cdata);
	stream.imbue(std::locale::classic());
	if (s.find(':') == std::string::npos) {
		stream >> mean;
	} else {
		stream >> mean >> separator >> sigma;
	}

	if (stream.fail()) {
		throw ParseException("SYNTAX ERROR: expect 'float' or 'float:float'");
	}

	set(mean, sigma);
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

float Real::sample() const {
	if (_sigma <= 0.0) return _mean;
	return static_cast<float>(rd::BoxMueller(Real::_rng, _mean, _sigma));
}

std::string Real::asString() const {
	std::ostringstream ss;
	ss << _value;
	return ss.str();
}

std::ostream &operator <<(std::ostream &o, Real const &r) { return o << r.asString(); }

} // namespace csp

