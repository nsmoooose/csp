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


#include <SimData/Noise.h>
#include <SimData/Random.h>


NAMESPACE_SIMDATA


Perlin1D::Perlin1D(double persistence, int octaves, Interpolation interpolation) {
	m_persistence = persistence;
	m_octaves = octaves;
	m_interpolation = interpolation;
	randomize();
}


void Perlin1D::setParameters(double persistence, int octaves) {
	m_persistence = persistence;
	m_octaves = octaves;
} 


void Perlin1D::setInterpolation(Interpolation interpolation) {
	m_interpolation = interpolation;
}


void Perlin1D::setOffset(int idx) {
	m_offset = idx;
}


void Perlin1D::randomize() {
	m_offset = int(g_Random.newRand()*1.0e+9);
}


double Perlin1D::_getInterpolatedNoise(double x) {
	int integer_X = int(x);
	double fractional_X = x - integer_X;

	double v1 = _getSmoothedNoise(integer_X);
	double v2 = _getSmoothedNoise(integer_X + 1);

	if (m_interpolation == LINEAR) {
		return _linearInterpolate(v1, v2, fractional_X);
	} else
	if (m_interpolation == COSINE) {
		return _cosineInterpolate(v1, v2, fractional_X);
	}
	
	double v0 = _getSmoothedNoise(integer_X - 1);
	double v3 = _getSmoothedNoise(integer_X + 2);

	return _cubicInterpolate(v0, v1, v2, v3, fractional_X);
} 

double Perlin1D::getValue(double x) {
	double total = 0.0;
	double amplitude = 1.0;
	double frequency = 1.0;
	for (int i = 0; i < m_octaves; i++) {
		amplitude *= m_persistence;
		total += _getInterpolatedNoise(x * frequency + m_offset) * amplitude;
		frequency *= 2.0;
	} 
	return total;
}

std::vector<float> Perlin1D::generate(int n, bool periodic, double timescale, double amplitude, double offset) {
	std::vector<float> buffer(n);
	if (n > 0) {
		int i;
		double s = timescale/n;
		for (i=0; i<n; i++) {
			double f = i*s;
			buffer[i] = static_cast<float>(getValue(f)*amplitude + offset);
		}
		if (periodic) {
			s = 1.0 / n;
			for (i=0; i<n/2; i++) {
				float f = static_cast<float>(0.5 + i*s);
				float a = buffer[i];
				float b = buffer[n-i-1];
				buffer[i] = f*a+(1-f)*b;
				buffer[n-i-1] = f*b+(1-f)*a;
			}
		}
	}
	return buffer;
}


NAMESPACE_END // namespace simdata


