/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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


class DataArchive;


SIMDATA_EXCEPTION(DataUnderflow);
SIMDATA_EXCEPTION(ConstViolation);
SIMDATA_EXCEPTION(SerializeError);


/** A trivial FILE * wrapper to provide a uniform file interface for both
 *  C++ and Python.
 */
class SIMDATA_EXPORT PackFile {
	FILE *_f;
	bool _open;
public:

#ifndef SWIG
	/** Retrieve the underlying FILE pointer.
	 */
	operator FILE*() { return _f; }

	/** Wrap an existing (open) FILE pointer.
	 */
	PackFile(FILE* f): _f(f), _open(false) {
		_open = (_f != 0);
	}
#endif

	/** Open a new file (fopen interface)
	 */
	PackFile(const char *fn, const char *mode) {
		_f = (FILE*) fopen(fn, mode);
		assert(_f); // XXX add error handling
		_open = (_f != 0);
	}

	/** Close the current file, if open.
	 */
	void close() {
		if (_open) {
			if (_f) fclose(_f);
			_f = 0;
			_open = false;
		}
	}
};


/**
 * Abstract base class for serializing standard types and BaseTypes
 * from a data source.
 */
class SIMDATA_EXPORT Reader {
public:
	virtual ~Reader() {}

	virtual DataArchive* _getArchive() { return 0; }
	virtual bool _loadAll() const { return false; }

	virtual Reader& operator>>(char &x)=0;
	virtual Reader& operator>>(int16 &x)=0;
	virtual Reader& operator>>(int32 &x)=0;
	virtual Reader& operator>>(uint8 &x)=0;
	virtual Reader& operator>>(uint16 &x)=0;
	virtual Reader& operator>>(uint32 &x)=0;
	virtual Reader& operator>>(bool &x)=0;
	virtual Reader& operator>>(float &x)=0;
	virtual Reader& operator>>(double &x)=0;
	virtual Reader& operator>>(char* &x)=0;
	virtual Reader& operator>>(BaseType &x)=0;
	virtual Reader& operator>>(hasht &x)=0;
	virtual Reader& operator>>(std::string &x)=0;
	virtual int32 readLength() { int32 n; operator>>(n); return n; }

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
	int32 _int32() {
		SIMDATA(int32) y;
		(*self) >> y;
		return y;
	}
	int16 _int16() {
		SIMDATA(int16) y;
		(*self) >> y;
		return y;
	}
	char _char() {
		char y;
		(*self) >> y;
		return y;
	}
	uint8 _uint8() {
		SIMDATA(uint8) y;
		(*self) >> y;
		return y;
	}
	uint16 _uint16() {
		SIMDATA(uint16) y;
		(*self) >> y;
		return y;
	}
	uint32 _uint32() {
		SIMDATA(uint32) y;
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
	int32 n;
	reader >> n;
	y.resize(n);
	typename std::vector<T>::iterator i = y.begin();
	while (n-- > 0) reader >> (*i++);
	return reader;
}


/**
 * Abstract base class for serializing standard types and BaseTypes
 * to a data source.
 */
class SIMDATA_EXPORT Writer {
public:
	virtual ~Writer() {}

	virtual Writer& operator<<(const char)=0;
	virtual Writer& operator<<(const int16)=0;
	virtual Writer& operator<<(const int32)=0;
	virtual Writer& operator<<(const uint8)=0;
	virtual Writer& operator<<(const uint16)=0;
	virtual Writer& operator<<(const uint32)=0;
	virtual Writer& operator<<(const bool)=0;
	virtual Writer& operator<<(const float)=0;
	virtual Writer& operator<<(const double)=0;
	virtual Writer& operator<<(const char*)=0;
	virtual Writer& operator<<(const BaseType &x)=0;
	virtual Writer& operator<<(const hasht &x)=0;
	virtual Writer& operator<<(const std::string &x)=0;
	virtual void writeLength(int32 n) { operator<<(n); }

	// explicit packing (use from python)

#ifdef SWIG
%extend {
	void _double(double x) { (*self) << x; }
	void _float(float x) { (*self) << x; }
	void _bool(bool x) { (*self) << x; }
	void _int32(int32 x) { (*self) << x; }
	void _int16(int16 x) { (*self) << x; }
	void _char(char x) { (*self) << x; }
	void _uint8(uint8 x) { (*self) << x; }
	void _uint16(uint16 x) { (*self) << x; }
	void _uint32(uint32 x) { (*self) << x; }
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
 *  @author Mark Rose <mkrose@users.sourceforge.net>
 */
class SIMDATA_EXPORT ArchiveWriter: public Writer {
	FILE *_f;
	int32 _n;
	void write(const void* x, int32 n) {
		fwrite(x, n, 1, _f);
	}
public:
	ArchiveWriter(PackFile f): Writer(), _n(0) {
		_f = static_cast<FILE*>(f);
		assert(_f != 0);
	}
	void resetCount() { _n = 0; }
	int32 getCount() { return _n; }

	Writer& operator<<(const char x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const int16 x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const int32 x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const uint8 x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const uint16 x) {
		write(&x, sizeof(x)); _n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const uint32 x) {
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
		int32 n = strlen(x);
		writeLength(n);
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
 *  @author Mark Rose <mkrose@users.sourceforge.net>
 */
class SIMDATA_EXPORT ArchiveReader: public Reader {
	const char* _d;
	int32 _n;
	DataArchive* _archive;
	bool _loadall;
	
public:
	DataArchive* _getArchive() { return _archive; }
	bool _loadAll() const { return _loadall; }

	ArchiveReader(const char* data, int32 n, DataArchive* archive=0, bool loadall = true):
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
	Reader& operator>>(int32 &y) {
		_n -= sizeof(int);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(int));
		_d += sizeof(int);
		return *this;
	}
	Reader& operator>>(uint32 &y) {
		_n -= sizeof(unsigned int);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(unsigned int));
		_d += sizeof(unsigned int);
		return *this;
	}
	Reader& operator>>(bool &y) {
		char x;
		operator>>(x);
		y = (x != 0);
		return *this;
	}
	Reader& operator>>(int16 &y) {
		_n -= sizeof(int16);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(int16));
		_d += sizeof(int16);
		return *this;
	}
	Reader& operator>>(uint16 &y) {
		_n -= sizeof(uint16);
		if (_n < 0) throw DataUnderflow();
		memcpy(&y, _d, sizeof(uint16));
		_d += sizeof(uint16);
		return *this;
	}
	Reader& operator>>(char &y) {
		_n -= sizeof(char);
		if (_n < 0) throw DataUnderflow();
		y = static_cast<char>(*_d++);
		return *this;
	}
	Reader& operator>>(uint8 &y) {
		_n -= sizeof(uint8);
		if (_n < 0) throw DataUnderflow();
		y = static_cast<uint8>(*_d++);
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
		int32 n = readLength();
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


/** Writer class for serializing data to a memory buffer.
 *
 *  This class currently uses a fixed size memory buffer, that
 *  must be preallocated with sufficient space to store all
 *  data that is serialized.
 */
class SIMDATA_EXPORT MemoryWriter: public Writer {
	uint8 * _ptr;
	int _n;
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
	Writer& operator<<(const int16 x) {
		memcpy(_ptr+_n, &x, sizeof(x));
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const int32 x) {
		memcpy(_ptr+_n, &x, sizeof(x));
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const uint8 x) {
		memcpy(_ptr+_n, &x, sizeof(x));
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const uint16 x) {
		memcpy(_ptr+_n, &x, sizeof(x));
		_n += sizeof(x);
		return *this;
	}
	Writer& operator<<(const uint32 x) {
		memcpy(_ptr+_n, &x, sizeof(x));
		_n += sizeof(x);
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
		int32 n = strlen(x);
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


/** Reader class for serializing data from a memory buffer.
 *
 *  The current implementation does not check for serialization
 *  underflows or overflows.
 */
class SIMDATA_EXPORT MemoryReader: public Reader {
	uint8 * _ptr;
	int _n;
public:
	MemoryReader(uint8 * ptr): Reader(), _n(0) {
		_ptr = ptr;
		assert(_ptr != 0);
	}
	void resetCount() { _n = 0; }
	int getCount() { return _n; }

	Reader& operator>>(char &x) {
		memcpy(&x, _ptr+_n, sizeof(char));
		_n += sizeof(char);
		return *this;
	}
	Reader& operator>>(int16 &x) {
		memcpy(&x, _ptr+_n, sizeof(int16));
		_n += sizeof(int16);
		return *this;
	}
	Reader& operator>>(int32 &x) {
		memcpy(&x, _ptr+_n, sizeof(int32)); 
		_n += sizeof(int32);
		return *this;
	}
	Reader& operator>>(unsigned char &x) {
		memcpy(&x, _ptr+_n, sizeof(unsigned char));
		_n += sizeof(unsigned char);
		return *this;
	}
	Reader& operator>>(uint16 &x) {
		memcpy(&x, _ptr+_n, sizeof(uint16));
		_n += sizeof(uint16);
		return *this;
	}
	Reader& operator>>(uint32 &x) {
		memcpy(&x, _ptr+_n, sizeof(uint32));
		_n += sizeof(uint32);
		return *this;
	}
	Reader& operator>>(bool &y) {
		char x;
		operator>>(x);
		y = (x != 0);
		_n += sizeof(char);
		return *this;
	}
	Reader& operator>>(float &x) {
		memcpy(&x, _ptr+_n, sizeof(x)); 
		_n += sizeof(x);
		return *this;
	}
	Reader& operator>>(double &x) {
		memcpy(&x, _ptr+_n, sizeof(x)); 
		_n += sizeof(x);
		return *this;
	}
	Reader& operator>>(char* &x) {
		int32 n = strlen(x);
		operator>>(n);
		memcpy(x, _ptr+_n, n);
		_n += n;
		return *this;
	}
	Reader& operator>>(BaseType &x) {
		x.serialize(*this);
		return *this;
	}
	Reader& operator>>(hasht &x) {
		memcpy(&x, _ptr+_n, sizeof(x)); 
		_n += sizeof(x);
		return *this;
	}
	Reader& operator>>(std::string &y) {
		char* c;
		operator>>(c);
		y.assign(c);
		free(c);
		return *this;
	}
};

NAMESPACE_SIMDATA_END


#endif //__SIMDATA_PACK_H__

