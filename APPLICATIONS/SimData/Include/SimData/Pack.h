/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
 * @file Pack.h
 */


#ifndef __SIMDATA_PACK_H__
#define __SIMDATA_PACK_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <SimData/BaseType.h>
#include <SimData/HashUtility.h>
#include <SimData/Exception.h>
#include <SimData/Namespace.h>


NAMESPACE_SIMDATA


class Packer;
class UnPacker;
class DataArchive;


SIMDATA_EXCEPTION(DataUnderflow);



/**
 * @brief Utility class for writing raw data to an object archive.
 *
 * Packer instances are created by the DataArchive class when an
 * object is being serialized.  The instance stores a FILE object
 * and provides methods to write variables of various types to
 * the file in a standard format.
 *
 * @author Mark Rose <tm2@stm.lbl.gov>
 */
class Packer {
	FILE *_f;
	int _n;
	void write(const void* x, int n) {
		fwrite(x, n, 1, _f);
	}
public:
	Packer(FILE* f) {
		_f = f;
		resetCount();
	}
	void resetCount() { _n = 0; }
	int getCount() { return _n; }

	// overloaded packing
	void pack(char x) {
		write(&x, sizeof(char));
		_n += sizeof(char);
	}
	void pack(short x) {
		write(&x, sizeof(short));
		_n += sizeof(short);
	}
	void pack(int x) {
		write(&x, sizeof(int));
		_n += sizeof(int);
	}
	void pack(double x) {
		write(&x, sizeof(double));
		_n += sizeof(double);
	}
	void pack(const char* x) {
		int n = strlen(x);
		pack(n);
		write(x, n);
		_n += n;
	}
	void pack(const BaseType& x) {
		x.pack(*this);
	}
	void pack(float x) {
		write(&x, sizeof(float));
		_n += sizeof(float);
	}
	void pack(hasht x) {
		write(&x, sizeof(hasht));
		_n += sizeof(hasht);
	}
	void pack(const std::string& x) {
		pack(x.c_str());
	}
	template<typename T>
	void pack(const std::vector<T>& x) {
		pack((int)x.size());
		typename std::vector<T>::const_iterator i = x.begin();
		while (i != x.end()) pack(*i++);
	}
	/*
	void pack(const std::vector<double>& x) {
		pack((int)x.size());
		std::vector<double>::const_iterator i = x.begin();
		while (i != x.end()) pack(*i++);
	}
	void pack(const std::vector<float>& x) {
		pack((int)x.size());
		std::vector<float>::const_iterator i = x.begin();
		while (i != x.end()) pack(*i++);
	}
	void pack(const std::vector<int>& x) {
		pack((int)x.size());
		std::vector<int>::const_iterator i = x.begin();
		while (i != x.end()) pack(*i++);
	}
	*/

	// explicit packing
	void pack_double(double x) { pack(x); }
	void pack_float(float x) { pack(x); }
	void pack_int(int x) { pack(x); }
	void pack_hasht(hasht x) { pack(x); }
	void pack_string(const std::string& x) { pack(x); }
	void pack_basetype(BaseType &x) { pack(x); }

};


/**
 * @brief Utility class for extracting raw data from an object archive.
 *
 * UnPacker instances are created by the DataArchive class when an
 * object is being deserialized.  The instance stores all the data
 * needed to reconstruct the object, and provides access methods
 * for translating the raw bytes into variables of various types.
 *
 * @author Mark Rose <tm2@stm.lbl.gov>
 */
class UnPacker {
	const char* _d;
	int _n;
	DataArchive* _archive;
	bool _loadall;
	
public:
	DataArchive* _getArchive() { return _archive; }
	bool _loadAll() const { return _loadall; }

	UnPacker(const char* data, int n, DataArchive* archive=0, bool loadall = true) {
		_n = n;
		_d = data;
		_archive = archive;
		_loadall = loadall;
	}

	bool isComplete() const {
		return _n == 0;
	}
	
	// overloaded unpacking
	void unpack(double& y) {
		_n -= sizeof(double);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(double));
		_d += sizeof(double);
	}
	void unpack(float& y) {
		_n -= sizeof(float);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(float));
		_d += sizeof(float);
	}
	void unpack(int& y) {
		_n -= sizeof(int);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(int));
		_d += sizeof(int);
	}
	void unpack(bool& y) {
		int x;
		unpack(x);
		y = (x != 0);
	}
	void unpack(short& y) {
		_n -= sizeof(short);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(short));
		_d += sizeof(short);
	}
	void unpack(char& y) {
		_n -= sizeof(char);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(char));
		_d += sizeof(char);
	}
	void unpack(hasht& y) {
		_n -= sizeof(hasht);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(hasht));
		_d += sizeof(hasht);
	}
	void unpack(char*& y) {
		int n;
		unpack(n);
		// XXX this not really a data underflow
		if (n < 0) throw DataUnderflow();
		_n -= n;
		if (_n < 0) throw DataUnderflow();
		y = (char*) malloc(sizeof(char)*(n+1));
		assert(y != 0); // XXX should throw a memory exception
		memcpy(y, _d, sizeof(char)*n);
		y[n] = 0;
		_d += n;
	}
	void unpack(std::string& y) {
		char* c;
		unpack(c);
		y.assign(c);
		free(c);
	}
	template<typename T>
	void unpack(std::vector<T>& y) {
		int n;
		unpack(n);
		y.resize(n);
		typename std::vector<T>::iterator i = y.begin();
		while (n-- > 0) unpack(*i++);
	}
	void unpack(BaseType& y) {
		y.unpack(*this);
	}


	// explicit unpacking (use from python)
	double unpack_double() {
		double y;
		unpack(y);
		return y;
	}
	float unpack_float() {
		float y;
		unpack(y);
		return y;
	}
	int unpack_int() {
		int y;
		unpack(y);
		return y;
	}
	hasht unpack_hasht() {
		hasht y;
		unpack(y);
		return y;
	}
	std::string unpack_string() {
		std::string y;
		unpack(y);
		return y;
	}
	BaseType &unpack_basetype(BaseType &y) {
		unpack(y);
		return y;
	}

#ifdef XXX_SWIG
	// more explicit methods for python that instantiate,
	// unpack, and return BaseType data objects.
%insert("shadow") %{
	def unpack_Date(self):
		return self.unpack_basetype(Date())
	def unpack_Matrix(self):
		return self.unpack_basetype(Matrix3())
	def unpack_Vector(self):
		return self.unpack_basetype(Vector3())
	def unpack_Spread(self):
		return self.unpack_basetype(Spread())
	def unpack_Table(self):
		return self.unpack_basetype(Table())
	def unpack_Curve(self):
		return self.unpack_basetype(Curve())
	def unpack_Table1(self):
		return self.unpack_basetype(Table1())
	def unpack_Table2(self):
		return self.unpack_basetype(Table2())
	def unpack_Table3(self):
		return self.unpack_basetype(Table3())
	def unpack_Path(self):
		return self.unpack_basetype(Path())
	def unpack_External(self):
		return self.unpack_basetype(External())
	def unpack_Key(self):
		return self.unpack_basetype(Key())
	def unpack_List(self, unpack):
		return self.unpack_basetype(List())
	def unpack_Pointer(self):
		raise "FIXME: NOT IMPLMENTED"
		return self.unpack_basetype(Pointer())
	def unpack_Enum(self, y):
		self.unpack_basetype(y)
	def unpack(self):
		raise "Use explicit unpack_* methods from Python"
%}
#endif // SWIG
	
};

/*
		n = self.unpack_int()
		items = SimData.List()
		for idx in range(n):
			items.append(unpack())
		return items
*/
NAMESPACE_SIMDATA_END


#endif //__SIMDATA_PACK_H__

