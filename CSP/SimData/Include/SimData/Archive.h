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
#include <SimData/ExceptionBase.h>
#include <SimData/Namespace.h>


NAMESPACE_SIMDATA


class Packer;
class UnPacker;
class DataArchive;


SIMDATA_EXCEPTION(DataUnderflow);
SIMDATA_EXCEPTION(ConstViolation);
SIMDATA_EXCEPTION(SerializeError);


class SIMDATA_EXPORT PackFile {
	FILE *_f;
	bool _open;
public:
#ifndef SWIG
	operator FILE*() { return _f; }
	PackFile(FILE* f): _f(f), _open(false) {}
#endif
	PackFile(const char *fn, const char *mode) {
		_f = (FILE*) fopen(fn, mode);
		assert(_f); // XXX add error handling
		_open = (_f != 0);
	}
	void close() {
		if (_open) {
			fclose(_f);
			_open = false;
		}
	}
};



class SIMDATA_EXPORT Reader {
public:
	virtual ~Reader() {}

	virtual DataArchive* _getArchive() { return 0; }
	virtual bool _loadAll() const { return false; }

	virtual Reader& operator>>(char &x)=0;
	virtual Reader& operator>>(short &x)=0;
	virtual Reader& operator>>(int &x)=0;
	virtual Reader& operator>>(bool &x)=0;
	virtual Reader& operator>>(float &x)=0;
	virtual Reader& operator>>(double &x)=0;
	virtual Reader& operator>>(char* &x)=0;
	virtual Reader& operator>>(BaseType &x)=0;
	virtual Reader& operator>>(hasht &x)=0;
	virtual Reader& operator>>(std::string &x)=0;

	// explicit methods for use from Python

#ifdef SWIG
%extend {
	double _double() {
		double y;
		(*self) >> y;
		return y;
	}
	float _float() {
		float y;
		(*self) >> y;
		return y;
	}
	bool _bool() {
		bool y;
		(*self) >> y;
		return y;
	}
	int _int() {
		int y;
		(*self) >> y;
		return y;
	}
	short _short() {
		short y;
		(*self) >> y;
		return y;
	}
	char _char() {
		char y;
		(*self) >> y;
		return y;
	}
	hasht _hasht() {
		simdata::hasht y;
		(*self) >> y;
		return y;
	}
	std::string _string() {
		std::string y;
		(*self) >> y;
		return y;
	}
	BaseType &_basetype(BaseType &y) {
		(*self) >> y;
		return y;
	}

	// more explicit methods for python that instantiate,
	// unpack, and return BaseType data objects.
	
#define __SIMDATA_ARCHIVE(T) SIMDATA(T) _##T() { \
		SIMDATA(T) x; (*self) >> x; return x; \
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
%insert("shadow") %{
	def _Pointer(self):
		raise "FIXME: NOT IMPLEMENTED"
	def _Enum(self):
		raise "FIXME: NOT IMPLEMENTED"
	def _List(self):
		raise "FIXME: NOT IMPLEMENTED"
	def __call__(self, *args):
		raise "Use explicit _* methods from Python"
%}
#endif // SWIG

};


template<typename T>
Reader& operator>>(Reader& reader, std::vector<T> &y) {
	int n;
	reader >> n;
	y.resize(n);
	typename std::vector<T>::iterator i = y.begin();
	while (n-- > 0) reader >> (*i++);
	return reader;
}


/**
 *
 */
class SIMDATA_EXPORT Writer {
public:
	virtual ~Writer() {}

	virtual Writer& operator<<(const char)=0;
	virtual Writer& operator<<(const short)=0;
	virtual Writer& operator<<(const int)=0;
	virtual Writer& operator<<(const bool)=0;
	virtual Writer& operator<<(const float)=0;
	virtual Writer& operator<<(const double)=0;
	virtual Writer& operator<<(const char*)=0;
	virtual Writer& operator<<(const BaseType &x)=0;
	virtual Writer& operator<<(const hasht &x)=0;
	virtual Writer& operator<<(const std::string &x)=0;

	// explicit packing (use from python)

#ifdef SWIG
%extend {
	void _double(double x) { (*self) << x; }
	void _float(float x) { (*self) << x; }
	void _bool(bool x) { (*self) << x; }
	void _int(int x) { (*self) << x; }
	void _short(short x) { (*self) << x; }
	void _char(char x) { (*self) << x; }
	void _hasht(hasht const &x) { (*self) << x; }
	void _string(std::string const &x) { (*self) << x; }
	void _basetype(BaseType const &x) { (*self) << x; }
}
#endif // SWIG

};


template<typename T>
Writer& operator<<(Writer& writer, const std::vector<T> &x) {
	writer << static_cast<int>(x.size());
	typename std::vector<T>::const_iterator i = x.begin();
	while (i != x.end()) writer << (*i++);
	return writer;
}


/** Utility class for writing raw data to an object archive.
 *
 *  Packer instances are created by the DataArchive class when an
 *  object is being serialized.  The instance stores a FILE object
 *  and provides methods to write variables of various types to
 *  the file in a standard format.
 *
 *  @author Mark Rose <tm2@stm.lbl.gov>
 */
class SIMDATA_EXPORT ArchiveWriter: public Writer {
	FILE *_f;
	int _n;
	void write(const void* x, int n) {
		fwrite(x, n, 1, _f);
	}
public:
	ArchiveWriter(PackFile f): Writer(), _n(0) {
		_f = static_cast<FILE*>(f);
		assert(_f != 0);
	}
	void resetCount() { _n = 0; }
	int getCount() { return _n; }

	Writer& operator<<(const char x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const short x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const int x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const bool x) {
		const char c = x ? 1:0;
		operator<<(c);
		return *this;
	}
	Writer& operator<<(const float x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const double x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const char* x) {
		int n = strlen(x);
		operator<<(n);
		write(x, n);
		_n += n;
		return *this;
	}
	Writer& operator<<(const BaseType &x) {
		x.serialize(*this);
		return *this;
	}
	Writer& operator<<(const hasht &x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const std::string &x) {
		operator<<(x.c_str());
		return *this;
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
class SIMDATA_EXPORT ArchiveReader: public Reader {
	const char* _d;
	int _n;
	DataArchive* _archive;
	bool _loadall;
	
public:
	DataArchive* _getArchive() { return _archive; }
	bool _loadAll() const { return _loadall; }

	ArchiveReader(const char* data, int n, DataArchive* archive=0, bool loadall = true):
		Reader(), _d(data), _n(n), _archive(archive), _loadall(loadall) { }

	bool isComplete() const { return _n == 0; }
	
	Reader& operator>>(double &y) {
		_n -= sizeof(double);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(double));
		_d += sizeof(double);
		return *this;
	}
	Reader& operator>>(float &y) {
		_n -= sizeof(float);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(float));
		_d += sizeof(float);
		return *this;
	}
	Reader& operator>>(int &y) {
		_n -= sizeof(int);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(int));
		_d += sizeof(int);
		return *this;
	}
	Reader& operator>>(bool &y) {
		char x;
		operator>>(x);
		y = (x != 0);
		return *this;
	}
	Reader& operator>>(short &y) {
		_n -= sizeof(short);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(short));
		_d += sizeof(short);
		return *this;
	}
	Reader& operator>>(char &y) {
		_n -= sizeof(char);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(char));
		_d += sizeof(char);
		return *this;
	}
	Reader& operator>>(hasht &y) {
		_n -= sizeof(hasht);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(hasht));
		_d += sizeof(hasht);
		return *this;
	}
	Reader& operator>>(char* &y) {
		int n;
		operator>>(n);
		// XXX this not really a data underflow
		if (n < 0) throw DataUnderflow();
		_n -= n;
		if (_n < 0) throw DataUnderflow();
		// XXX UGLY! this leaks memory unless the caller calls free()!!
		y = (char*) malloc(sizeof(char)*(n+1));
		assert(y != 0); // XXX should throw a memory exception
		memcpy(y, _d, sizeof(char)*n);
		y[n] = 0;
		_d += n;
		return *this;
	}
	Reader& operator>>(std::string &y) {
		char* c;
		operator>>(c);
		y.assign(c);
		free(c);
		return *this;
	}
	Reader& operator>>(BaseType &y) {
		y.serialize(*this);
		return *this;
	}
};


class SIMDATA_EXPORT MemoryWriter: public Writer {
	uint8 * _ptr;
	int _n;
	//void write(const void* x, int n) {
	//	fwrite(x, n, 1, _f);
	//}
public:
	MemoryWriter(uint8 * ptr): Writer(), _n(0) {
		_ptr = ptr;
		assert(_ptr != 0);
	}
	void resetCount() { _n = 0; }
	int getCount() { return _n; }

	Writer& operator<<(const char x) {
		memcpy(_ptr+_n, &x, sizeof(char));
		_n += sizeof(char);
		return *this;
	}
	Writer& operator<<(const short x) {
		memcpy(_ptr+_n, &x, sizeof(short));
		_n += sizeof(short);
		return *this;
	}
	Writer& operator<<(const int x) {
		memcpy(_ptr+_n, &x, sizeof(x)); 
		_n += sizeof(int);
		return *this;
	}
	Writer& operator<<(const bool x) {
		const char c = x ? 1:0;
		operator<<(c);
		return *this;
	}
	Writer& operator<<(const float x) {
		memcpy(_ptr+_n, &x, sizeof(x)); 
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const double x) {
		memcpy(_ptr+_n, &x, sizeof(x)); 
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const char* x) {
		int n = strlen(x);
		operator<<(n);
		memcpy(_ptr+_n, x, n);
		_n += n;
		return *this;
	}
	Writer& operator<<(const BaseType &x) {
		x.serialize(*this);
		return *this;
	}
	Writer& operator<<(const hasht &x) {
		memcpy(_ptr+_n, &x, sizeof(x)); 
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const std::string &x) {
		operator<<(x.c_str());
		return *this;
	}
};
NAMESPACE_SIMDATA_END


#endif //__SIMDATA_PACK_H__

