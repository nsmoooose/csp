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


#include <SimData/Interpolate.h>
#include <SimData/Pack.h>
#include <SimData/Version.h>


NAMESPACE_SIMDATA

#define min(a, b) (((a)<(b)) ? (a) : (b))
#define max(a, b) (((a)>(b)) ? (a) : (b))

template <typename T>
const Enumeration InterpolatedData<T>::Method("LINEAR SPLINE");

//static load_check check_interpolate;



// class InterpolatedData

template <typename T>
InterpolatedData<T>::InterpolatedData(): method(InterpolatedData<T>::Method) {
}
	
template <typename T>
InterpolatedData<T>::~InterpolatedData() {
}

template <typename T>
void InterpolatedData<T>::pack(Packer& p) const {
	p.pack(method);
}

template <typename T>
void InterpolatedData<T>::unpack(UnPacker& p) {
	p.unpack(method);
}

template <typename T>
typename InterpolatedData<T>::value_t InterpolatedData<T>::getValue(value_t, value_t) const { 
	return 0.0; 
}

template <typename T>
typename InterpolatedData<T>::value_t InterpolatedData<T>::getPrecise(value_t, value_t) const { 
	return 0.0; 
}

template <typename T>
typename InterpolatedData<T>::vector_t InterpolatedData<T>::_compute_second_derivatives(const vector_t& breaks, const vector_t& data) {
	int n = breaks.size();
	vector_t z(n,0.0), u(n,0.0);
	int i;
	for (i=1; i<n-1; i++) {
		value_t d = breaks[i+1] - breaks[i-1];
		value_t sig = (breaks[i] - breaks[i-1]) / d;
		value_t p = static_cast<value_t>(2.0 + sig * z[i-1]);
		z[i] = static_cast<value_t>((sig - 1.0) / p);
		value_t a = (data[i+1] - data[i]) / (breaks[i+1] - breaks[i]);
		value_t b = (data[i] - data[i-1]) / (breaks[i] - breaks[i-1]);
		u[i] = static_cast<value_t>((6.0 * (a - b) / d - sig * u[i-1]) / p);
	}
	z[n-1] = 0.0;
	for (i=n-2; i>=0; i--)
		z[i] = z[i] * z[i+1] + u[i];
	return z;
}

template <typename T>
int InterpolatedData<T>::find(vector_t b, value_t v) const {
	int lo = 0;
	int hi = b.size()-1;
	int i;
	while (hi - lo > 1) {
		i = (hi + lo) / 2;
		if (b[i] > v) hi = i;
		else lo = i;
	}
	return lo;
}



// class Curve

Curve::Curve() {
}

Curve::~Curve() {
}

Curve::Curve(const Curve &c): InterpolatedData<float>(c) {
	*this = c;
}

const Curve & Curve::operator=(const Curve &c) {
	_breaks = c._breaks;
	_data = c._data;
	_table = c._table;
	_sd = c._sd;
	_spacing = c._spacing;
	_range = c._range;
	_min = c._min;
	_i_n = c._i_n;
	return *this;
}

void Curve::pack(Packer& p) const {
	InterpolatedData<value_t>::pack(p);
	p.pack(_breaks);
	p.pack(_data);
	p.pack(_spacing);
}

void Curve::unpack(UnPacker& p) {
	InterpolatedData<value_t>::unpack(p);
	vector_t breaks;
	p.unpack(breaks);
	setBreaks(breaks);
	vector_t data;
	p.unpack(data);
	setData(data);
	value_t spacing;
	p.unpack(spacing);
	interpolate(spacing);
}

Curve::vector_t Curve::getBreaks() {
	return _breaks;
}

void Curve::setBreaks(const vector_t& breaks) {
	_breaks = breaks;
}

void Curve::setData(const vector_t& data) {
	_data = data;
}

// TODO set flags to ensure a consistant state
void Curve::interpolate(value_t spacing) {
	_compute_second_derivatives();
	_spacing = spacing;
	int n = _breaks.size();
	_min = _breaks[0];
	double max = _breaks[n-1];
	value_t _range = static_cast<value_t>(max - _min);
	_i_n = int(_range / spacing) + 1;
	_table.resize(_i_n);
	vector_it element = _table.begin();
	int i;
	for (i=0; i<_i_n; i++) {
		value_t x = _min + _range * i / (_i_n-1);
		*element++ = getPrecise(x);
	}
}

void Curve::_compute_second_derivatives() {
	_sd = InterpolatedData<value_t>::_compute_second_derivatives(_breaks, _data);
}

Curve::value_t Curve::getPrecise(Curve::value_t v) const {
	int i = find(_breaks, v);
	double h = _breaks[i+1] - _breaks[i];
	double f2 = (v - _breaks[i]) / h;
	double f1 = 1.0 - f2;
	double d1 = _sd[i];
	double d2 = _sd[i+1];
	double y1 = _data[i];
	double y2 = _data[i+1];
	double s = h * h / 6.0;
	v = static_cast<value_t>(y1*f1 + y2*f2 + (f1*(f1*f1 - 1.0)*d1 + f2*(f2*f2 - 1.0)*d2) * s);
	return v;
}

Curve::value_t Curve::getValue(Curve::value_t p) const {
	double i = (p - _min) / _range;
	if (i < 0.0) i = 0.0;
	else if (i > 1.0) i = 1.0;
	i = i * (_i_n-1);
	int idx = (int) i;
	double f = i - idx;
	double y1 = _table[idx];
	double y2 = _table[idx+1];
	value_t z = static_cast<value_t>(f * y2 + (1.0 - f) * y1);
	return z;
}

void Curve::dumpCurve(FILE* f) const {
	double x = _min;
	double d = _range / _table.size();
	vector_cit element = _table.begin();
	while (element != _table.end()) {
		fprintf(f, "%.4f %.4f\n", x, *element++);
		x += d;
	}
}

std::string Curve::asString() const {
	return std::string("<simdata::Curve>");
}

std::string Curve::typeString() const {
	return std::string("type::Curve");
}


// class Table

Table::Table() {
	invalidate();
}

Table::~Table() {
}

Table::Table(const Table &t): InterpolatedData<float>(t) {
	*this = t;
}

const Table & Table::operator=(const Table &t) {
	_x_label = t._x_label;
	_y_label = t._y_label;
	_x_breaks = t._x_breaks;
	_y_breaks =  t._y_breaks;
	_data = t._data;
	_table = t._table;
	_dcols = t._dcols;
	_drows = t._drows;
	_x_spacing = t._x_spacing;
	_y_spacing = t._y_spacing;
	_x_min = t._x_min;
	_y_min = t._y_min;
	_x_range = t._x_range;
	_y_range = t._y_range;
	_x_in = t._x_in;
	_y_in = t._y_in;
	_x_n = t._x_n;
	_y_n = t._y_n;
	_valid = t._valid;
	return *this;
}

void Table::invalidate() { 
	_valid = 0; 
}

int Table::isValid() const {
	return _valid; 
}

void Table::pack(Packer &p) const {
	InterpolatedData<value_t>::pack(p);
	p.pack(_x_breaks);
	p.pack(_y_breaks);
	p.pack(_data);
	p.pack(_x_spacing);
	p.pack(_y_spacing);
}

void Table::unpack(UnPacker &p) {
	InterpolatedData<value_t>::unpack(p);
	vector_t breaks, data;
	p.unpack(breaks);
	setXBreaks(breaks);
	p.unpack(breaks);
	setYBreaks(breaks);
	p.unpack(data);
	setData(data);
	value_t spacing;
	p.unpack(spacing);
	setXSpacing(spacing);
	p.unpack(spacing);
	setYSpacing(spacing);
	interpolate();
}

Table::vector_t Table::getXBreaks() const {
	return _x_breaks;
}

Table::vector_t Table::getYBreaks() const {
	return _y_breaks;
}

void Table::setXBreaks(const Table::vector_t& breaks) {
	_x_breaks = breaks;
	_x_n = _x_breaks.size();
	invalidate();
}

void Table::setYBreaks(const Table::vector_t& breaks) {
	_y_breaks = breaks;
	_y_n = _y_breaks.size();
	invalidate();
}

void Table::setBreaks(const Table::vector_t& x_breaks, const Table::vector_t& y_breaks) {
	setXBreaks(x_breaks);
	setYBreaks(y_breaks);
}

void Table::setXSpacing(Table::value_t spacing) {
	_x_spacing = spacing;
	invalidate();
}

void Table::setYSpacing(Table::value_t spacing) {
	_y_spacing = spacing;
	invalidate();
}

void Table::setSpacing(Table::value_t x_spacing, Table::value_t y_spacing) {
	setXSpacing(x_spacing);
	setYSpacing(y_spacing);
}

Table::value_t Table::getXSpacing() const {
	return _x_spacing;
}

Table::value_t Table::getYSpacing() const {
	return _y_spacing;
}

void Table::setData(const Table::vector_t& data) {
	_data = data;
	invalidate();
}

void Table::interpolate() {
	assert(_x_n > 0 && _y_n > 0);
	assert(_x_spacing > 0.0 && _y_spacing > 0.0);
	assert(_x_n == (int) _x_breaks.size());
	assert(_y_n == (int) _y_breaks.size());
	assert((int) _data.size() == _x_n*_y_n);
	_compute_second_derivatives();
	_x_min = _x_breaks[0];
	value_t x_max = _x_breaks[_x_n-1];
	_y_min = _y_breaks[0];
	value_t y_max = _y_breaks[_y_n-1];
	value_t x_d, y_d;
	_x_range = x_d = x_max - _x_min;
	_y_range = y_d = y_max - _y_min;
	_x_in = int(x_d / _x_spacing) + 1;
	_y_in = int(y_d / _y_spacing) + 1;
	_table.resize(_x_in * _y_in, 0.0);
	int i, j;
	vector_it element = _table.begin();
	for (j=0; j<_y_in; j++) {
		value_t y_v = _y_min + y_d * j / (_y_in - 1);
		for (i=0; i<_x_in; i++) {
			value_t x_v = _x_min + x_d * i / (_x_in - 1);
			*element++ = getPrecise(x_v, y_v);
		}
	}
}

Table::value_t Table::getPrecise(Table::value_t x, Table::value_t y) const {
	assert(isValid());
	int x_i = find(_x_breaks, x);
	int y_i = find(_y_breaks, y);
	value_t x_h = _x_breaks[x_i+1] - _x_breaks[x_i];
	value_t y_h = _y_breaks[y_i+1] - _y_breaks[y_i];
	double f11 = 1.0 - (x - _x_breaks[x_i]) / x_h;
	double f12 = 1.0 - (_x_breaks[x_i+1] - x) / x_h;
	double f21 = 1.0 - (y - _y_breaks[y_i]) / y_h;
	double f22 = 1.0 - (_y_breaks[y_i+1] - y) / y_h;
	int idx;
	idx = y_i * _x_n + x_i;
	double d11 = _drows[idx++];
	double d12 = _drows[idx--];
	idx += _x_n;
	double d21 = _drows[idx++];
	double d22 = _drows[idx--];
	idx = y_i * _x_n + x_i;
	double v11 = _data[idx++];
	double v12 = _data[idx--];
	idx += _x_n;
	double v21 = _data[idx++];
	double v22 = _data[idx--];
	double x_s = x_h * x_h / 6.0;
	double y_s = y_h * y_h / 6.0;
	double mid21 = v11*f11 + v12*f12 + (f11*(f11*f11 - 1.0)*d11 + f12*(f12*f12 - 1.0)*d12) * x_s;
	double mid22 = v21*f11 + v22*f12 + (f11*(f11*f11 - 1.0)*d21 + f12*(f12*f12 - 1.0)*d22) * x_s;
	idx = x_i * _y_n + y_i;
	double d1, d2;
	d1 = _dcols[idx] * f11;
	idx += _y_n;
	d1 += _dcols[idx] * f12;
	idx = x_i * _y_n + y_i + 1;
	d2 = _dcols[idx] * f11;
	idx += _y_n;
	d2 += _dcols[idx] * f12;
	double v = mid21*f21 + mid22*f22 + (f21*(f21*f21 - 1.0)*d1  + f22*(f22*f22 - 1.0)*d2) * y_s;
	return ((value_t) v);
}

Table::value_t Table::getValue(Table::value_t x, Table::value_t y) const {
	assert(isValid());
	double x_i = (x - _x_min) / _x_range;
	double y_i = (y - _y_min) / _y_range;
	x_i = min(max(x_i, 0.0), 1.0) * (_x_in - 1);
	y_i = min(max(y_i, 0.0), 1.0) * (_y_in - 1);
	int x_ii = int(x_i);
	int y_ii = int(y_i);
	double x_f = x_i - x_ii;
	double y_f = y_i - y_ii;
	double v1, v2, z1, z2, z;
	int idx;
	idx = y_ii * _x_in + x_ii;
	v1 = _table[idx++];
	v2 = _table[idx--];
	idx += _x_in;
	z1 = x_f * v2 + (1.0 - x_f) * v1;
	v1 = _table[idx++];
	v2 = _table[idx--];
	z2 = x_f * v2 + (1.0 - x_f) * v1;
	z = z2 * y_f + (1.0 - y_f) * z1;
	return ((value_t) z);
}

void Table::_compute_second_derivatives() {
	_drows.resize(0, 0.0);
	_dcols.resize(0, 0.0);
	vector_it row, row_end;
	int i, j;
	row = _data.begin();
	row_end = row + _x_n;
	for (j=0; j<_y_n; j++) {
		vector_t row_data = vector_t(row, row_end);
		vector_t d = InterpolatedData<value_t>::_compute_second_derivatives(_x_breaks, row_data);
		_drows.insert(_drows.end(), d.begin(), d.end());
		row = row_end;
		row_end += _x_n;
	}
	for (i=0; i<_x_n; i++) {
		vector_t col_data(_y_n);
		int idx = i;
		for (j=0; j<_y_n; j++) {
			col_data[j] = _data[idx];
			idx += _x_n;
		}
		vector_t d = InterpolatedData<value_t>::_compute_second_derivatives(_y_breaks, col_data);
		_dcols.insert(_dcols.end(), d.begin(), d.end());
	}
	_valid = 1;
}

void Table::toPGM(FILE *fp) const {
	fprintf(fp, "P2 %d %d 255\n", _x_in, _y_in);
	int i, j;
	vector_cit element = _table.begin();
	double min=1e+10, max=-1e+10, v;
	for (i=_x_in*_y_in; i>0; i--) {
		v = *element++;
		if (min > v) min = v;
		if (max < v) max = v;
	}
	double scale = max-min;
	if (scale > 0) scale = 255.0/scale;
	element = _table.begin();
	for (j=0; j<_y_in; j++) {
		for (i=0; i<_x_in; i++) {
			v = *element++;
			v = (v-min)*scale;
			fprintf(fp, "%4d", (int) v);
		}
		fprintf(fp, "\n");
	}
}

void Table::dumpTable(FILE *fp) const {
	int i, j;
	vector_cit element = _table.begin();
	for (j=0; j<_y_in; j++) {
		for (i=0; i<_x_in; i++) {
			fprintf(fp, "%0.2f ", (float) *element++);
		}
		fprintf(fp, "\n");
	}
}

void Table::dumpDRows(FILE *fp) const {
	int i, j;
	vector_cit element = _drows.begin();
	for (j=0; j<_y_n; j++) {
		for (i=0; i<_x_n; i++) {
			fprintf(fp, "%0.2f ", (float) *element++);
		}
		fprintf(fp, "\n");
	}
}

void Table::dumpDCols(FILE *fp) const {
	int i, j;
	vector_cit element = _dcols.begin();
	for (j=0; j<_y_n; j++) {
		for (i=0; i<_x_n; i++) {
			fprintf(fp, "%0.2f ", (float) *element++);
		}
		fprintf(fp, "\n");
	}
}

std::string Table::asString() const {
	return std::string("<simdata::Table>");
}

std::string Table::typeString() const {
	return std::string("type::Table");
}


NAMESPACE_SIMDATA_END


