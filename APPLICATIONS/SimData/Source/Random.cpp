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

#include <SimData/Random.h>


NAMESPACE_SIMDATA

Random g_Random;


float box_muller(float mean, float sigma) {
	double x1, x2, w, y1;
	static double y2;
	static int use_last = 0;
	if (use_last) {
		y1 = y2;
		use_last = 0;
	}
	else {
		do {
			x1 = 2.0 * g_Random.newRand() - 1.0;
			x2 = 2.0 * g_Random.newRand() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );
		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	return static_cast<float>( mean + y1 * sigma );
}


/*
 * This random number generator is from William H. Press, et al.,
 * _Numerical Recipes in C_, Second Ed. with corrections (1994), 
 * p. 282.  This excellent book is available through the
 * WWW at http://nr.harvard.edu/nr/bookc.html.
 * The specific section concerning ran2, Section 7.1, is in
 * http://cfatab.harvard.edu/nr/bookc/c7-1.ps
 *
 */


float Random::ran2(long &idum) {
	const long IM1=2147483563;
	const long IM2=2147483399;
	const double AM=(1.0/IM1);
	const long IMM1=(IM1-1);
	const long IA1=40014;
	const long IA2=40692;
	const long IQ1=53668;
	const long IQ2=52774;
	const long IR1=12211;
	const long IR2=3791;
	const long NTAB=32;
	const long NDIV=(1+IMM1/NTAB);
	const double EPS=1.2e-7;
	const double RNMX=(1.0-EPS);

	int j;
	long k;
	static long idum2=123456789;
	static long iy=0;
	static long iv[NTAB];
	float temp;

	if(idum <= 0) {
		if(-(idum) < 1)
			idum = 1;
		else
			idum = -(idum);
		idum2 = (idum);
		for(j=NTAB+7;j >= 0;j--) {
			k=(idum)/IQ1;
			idum=IA1*(idum-k*IQ1)-k*IR1;
			if(idum < 0)
				idum+=IM1;
			if(j < NTAB)
	  			iv[j] =idum;
		}

		iy = iv[0];
	}

	k = idum/IQ1;

	idum = IA1*(idum - k*IQ1) - k*IR1;
	if(idum < 0)
		idum += IM1;

	k = idum2/IQ2;
	idum2 = IA2*(idum2-k*IQ2)-k*IR2;
	if(idum2 < 0)
		idum2 += IM2;
	j=iy/NDIV;
	iy=iv[j]-idum2;
	iv[j] = idum;
	if (iy < 1)
		iy += IMM1;
	if ((temp = static_cast<float>(AM*iy)) > RNMX)
		return static_cast<float>(RNMX);
	else
		return temp;
}


float Gauss::box_muller(float mean, float sigma) {
	double x1, x2, w, y1;
	static double y2;
	static int use_last = 0;
	if (use_last) {
		y1 = y2;
		use_last = 0;
	}
	else {
		do {
			x1 = 2.0 * newRand() - 1.0;
			x2 = 2.0 * newRand() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );
		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	return static_cast<float>( mean + y1 * sigma );
}


Gauss::Gauss(float mean, float sigma) {
	_mean = mean;
	_sigma = sigma;
	_g = box_muller(_mean, _sigma);
}

float Gauss::newGauss() {
	_g = box_muller(_mean, _sigma);
	return _g;
}


NAMESPACE_END // namespace simdata

