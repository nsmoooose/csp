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
 * @file Archive.h
 * @brief Classes for storing and retrieving data from data archives.
 */


#ifndef __SIMDATA_ARCHIVE_H__
#define __SIMDATA_ARCHIVE_H__

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
SIMDATA_EXCEPTION(ConstViolation);

class Archive {
	bool __loading;
public:
	Archive(bool loading): __loading(loading) {}
	virtual ~Archive() {}

	virtual DataArchive* _getArchive() { return 0; }
	virtual bool _loadAll() const { return false; }
	bool isLoading() { return __loading; }

	virtual void operator()(char &x) = 0;
	virtual void operator()(short &x)=0;
	virtual void operator()(int &x)=0;
	virtual void operator()(bool &x)=0;
	virtual void operator()(float &x)=0;
	virtual void operator()(double &x)=0;
	virtual void operator()(char* &x)=0;
	virtual void operator()(BaseType &x)=0;
	virtual void operator()(hasht &x)=0;
	virtual void operator()(std::string &x)=0;
	template<typename T>
	void operator()(std::vector<T> &x);

	// explicit packing (use from python)

#ifdef SWIG
%extend {
	double _double(double x=0.0) { 
		double y=x; 
		self->operator()(y); 
		return y; 
	}
	float _float(double x=0.0f) { 
		float y=x; 
		self->operator()(y); 
		return y; 
	}
	bool _bool(bool x=false) { 
		bool y=x; 
		self->operator()(y); 
		return y; 
	}
	int _int(int x=0) { 
		int y=x; 
		self->operator()(y); 
		return y; 
	}
	short _short(short x=0) { 
		short y=x; 
		self->operator()(y); 
		return y; 
	}
	char _char(char x=0) { 
		char y=x; 
		self->operator()(y); 
		return y; 
	}
	hasht _hasht(hasht x=0) { 
		simdata::hasht y=x; 
		self->operator()(y); 
		return y; 
	}
	std::string _string(std::string const &x="") { 
		std::string y=x; 
		self->operator()(y); 
		return y; 
	}
	BaseType &_basetype(BaseType &x) { 
		self->operator()(x); 
		return x; 
	}
#define __SIMDATA_ARCHIVE(T) SIMDATA(T) _##T() { \
		SIMDATA(T) x; self->operator()(x); return x; \
	}
	__SIMDATA_ARCHIVE(SimDate);
	__SIMDATA_ARCHIVE(Matrix3);
	__SIMDATA_ARCHIVE(Vector3);
	__SIMDATA_ARCHIVE(Quat);
	__SIMDATA_ARCHIVE(Real);
	__SIMDATA_ARCHIVE(Curve);
	__SIMDATA_ARCHIVE(Table);
	__SIMDATA_ARCHIVE(Table1);
	__SIMDATA_ARCHIVE(Table2);
	__SIMDATA_ARCHIVE(Table3);
	__SIMDATA_ARCHIVE(LLA);
	__SIMDATA_ARCHIVE(UTM);
	__SIMDATA_ARCHIVE(ECEF);
	__SIMDATA_ARCHIVE(GeoPos);
	__SIMDATA_ARCHIVE(Path);
	__SIMDATA_ARCHIVE(External);
	__SIMDATA_ARCHIVE(Key);
	// TODO List;
	// TODO Enum
	// TODO Pointer
#undef __SIMDATA_ARCHIVE
}
	// more explicit methods for python that instantiate,
	// unpack, and return BaseType data objects.
%insert("shadow") %{
	def _Pointer(self):
		raise "FIXME: NOT IMPLMENTED"
	def _Enum(self):
		raise "FIXME: NOT IMPLMENTED"
	def _List(self):
		raise "FIXME: NOT IMPLMENTED"
	def __call__(self, *args):
		raise "Use explicit _* methods from Python"
%}
#endif // SWIG
	

};

/** Utility class for writing raw data to an object archive.
 *
 *  Packer instances are created by the DataArchive class when an
 *  object is being serialized.  The instance stores a FILE object
 *  and provides methods to write variables of various types to
 *  the file in a standard format.
 *
 *  @author Mark Rose <tm2@stm.lbl.gov>
 */
class Packer: public Archive {
	FILE *_f;
	int _n;
	void write(const void* x, int n) {
		fwrite(x, n, 1, _f);
	}
public:
	Packer(FILE* f): Archive(false) {
		_f = f;
		resetCount();
	}
	void resetCount() { _n = 0; }
	int getCount() { return _n; }

	void operator()(char &x) {
		write(&x, sizeof(char));
		_n += sizeof(char);
	}
	void operator()(short &x) {
		write(&x, sizeof(short));
		_n += sizeof(short);
	}
	void operator()(int &x) {
		write(&x, sizeof(int));
		_n += sizeof(int);
	}
	void operator()(bool &x) {
		char c = x ? 1:0;
		operator()(c);
	}
	void operator()(double &x) {
		write(&x, sizeof(double));
		_n += sizeof(double);
	}
	void operator()(char* &x) {
		int n = strlen(x);
		operator()(n);
		write(x, n);
		_n += n;
	}
	void operator()(BaseType &x) {
		x.serialize(*this);
	}
	void operator()(float &x) {
		write(&x, sizeof(float));
		_n += sizeof(float);
	}
	void operator()(hasht &x) {
		write(&x, sizeof(hasht));
		_n += sizeof(hasht);
	}
	void operator()(std::string &x) {
		char *str = const_cast<char*>(x.c_str());
		operator()(str);
	}
	template<typename T>
	void operator()(std::vector<T> &x) {
		int n = static_cast<int>(x.size());
		operator()(n);
		typename std::vector<T>::iterator i = x.begin();
		while (i != x.end()) operator()(*i++);
	}
};


/** Utility class for extracting raw data from an object archive.
 *
 *  UnPacker instances are created by the DataArchive class when an
 *  object is being deserialized.  The instance stores all the data
 *  needed to reconstruct the object, and provides access methods
 *  for translating the raw bytes into variables of various types.
 *
 *  @author Mark Rose <tm2@stm.lbl.gov>
 */
class UnPacker: public Archive {
	const char* _d;
	int _n;
	DataArchive* _archive;
	bool _loadall;
	
public:
	DataArchive* _getArchive() { return _archive; }
	bool _loadAll() const { return _loadall; }

	UnPacker(const char* data, int n, DataArchive* archive=0, bool loadall = true):
		Archive(true) {
		_n = n;
		_d = data;
		_archive = archive;
		_loadall = loadall;
	}

	bool isComplete() const { return _n == 0; }
	
	void operator()(double &y) {
		_n -= sizeof(double);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(double));
		_d += sizeof(double);
	}
	void operator()(float &y) {
		_n -= sizeof(float);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(float));
		_d += sizeof(float);
	}
	void operator()(int &y) {
		_n -= sizeof(int);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(int));
		_d += sizeof(int);
	}
	void operator()(bool &y) {
		char x;
		operator()(x);
		y = (x != 0);
	}
	void operator()(short &y) {
		_n -= sizeof(short);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(short));
		_d += sizeof(short);
	}
	void operator()(char &y) {
		_n -= sizeof(char);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(char));
		_d += sizeof(char);
	}
	void operator()(hasht &y) {
		_n -= sizeof(hasht);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(hasht));
		_d += sizeof(hasht);
	}
	void operator()(char* &y) {
		int n;
		operator()(n);
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
	void operator()(std::string &y) {
		char* c;
		operator()(c);
		y.assign(c);
		free(c);
	}
	template<typename T>
	void operator()(std::vector<T> &y) {
		int n;
		operator()(n);
		y.resize(n);
		typename std::vector<T>::iterator i = y.begin();
		while (n-- > 0) operator()(*i++);
	}
	void operator()(BaseType &y) {
		y.serialize(*this);
	}
};

/*
 *  Need to do explicit casting in this case, since there's
 *  no way to have a virtual template method.
 */
template<typename T>
void Archive::operator()(std::vector<T> &x) {
	Packer *packer = dynamic_cast<Packer*>(this);
	if (packer != 0) { 
		packer->operator()(x);
		return;
	}
	UnPacker *unpacker = dynamic_cast<UnPacker*>(this);
	assert(unpacker != 0);
	unpacker->operator()(x);
}

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_PACK_H__

