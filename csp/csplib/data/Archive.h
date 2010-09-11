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
 * @file Archive.h
 * @brief Classes for storing and retrieving data from data archives.
 */

#ifndef __CSPLIB_DATA_ARCHIVE_H__
#define __CSPLIB_DATA_ARCHIVE_H__

#include <csp/csplib/util/Endian.h>
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Namespace.h>

#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cassert>

namespace csp {

class DataArchive;

CSP_EXCEPTION(DataUnderflow)
CSP_EXCEPTION(ConstViolation)
CSP_EXCEPTION(SerializeError)

/** A trivial FILE * wrapper to provide a uniform file interface for both C++
 *  and Python.
 */
class CSPLIB_EXPORT PackFile {
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
	PackFile(const char *fn, const char *mode);

	/** Close the current file, if open.
	 */
	void close();
};


/** Base class for serializing standard types and BaseTypes from a data source.
 */
class CSPLIB_EXPORT Reader {
	uint8 const *_buffer;
	uint8 const *_read;
	uint8 const *_end;
	DataArchive *_data_archive;
	bool _load_all;

protected:
	Reader(uint8 const *buffer, uint32 length, DataArchive *data_archive=0, bool load_all=false);

	void bind(uint8 const *buffer, uint32 length);

public:
	virtual ~Reader();

	DataArchive* _getArchive();
	bool _loadAll() const;
	bool isComplete() const;

#ifndef SWIG
	Reader& operator>>(char &x);
	Reader& operator>>(int8 &x);
	Reader& operator>>(uint8 &x);
	Reader& operator>>(int16 &x);
	Reader& operator>>(uint16 &x);
	Reader& operator>>(int32 &x);
	Reader& operator>>(uint32 &x);
	Reader& operator>>(int64 &x);
	Reader& operator>>(uint64 &x);
	Reader& operator>>(bool &x);
	Reader& operator>>(float &x);
	Reader& operator>>(double &x);
	Reader& operator>>(hasht &x);
	Reader& operator>>(std::string &x);
#endif // SWIG

	int32 readLength();

	// explicit methods for use from Python

#ifdef SWIG
%extend {

	// more explicit methods for python that instantiate,
	// unpack, and return BaseType data objects.
	
#define __CSP_ARCHIVE(T, NAME) T NAME() { \
		T x; (*self) >> x; return x; \
	}

	__CSP_ARCHIVE(double, _double)
	__CSP_ARCHIVE(float, _float)
	__CSP_ARCHIVE(bool, _bool)
	__CSP_ARCHIVE(char, _char)
	__CSP_ARCHIVE(std::string, _string)
	__CSP_ARCHIVE(CSP(int64), _int64)
	__CSP_ARCHIVE(CSP(int32), _int32)
	__CSP_ARCHIVE(CSP(int16), _int16)
	__CSP_ARCHIVE(CSP(int8), _int8)
	__CSP_ARCHIVE(CSP(uint64), _uint64)
	__CSP_ARCHIVE(CSP(uint32), _uint32)
	__CSP_ARCHIVE(CSP(uint16), _uint16)
	__CSP_ARCHIVE(CSP(uint8), _uint8)
	__CSP_ARCHIVE(CSP(hasht), _hasht)
	__CSP_ARCHIVE(CSP(SimDate), _SimDate);
	__CSP_ARCHIVE(CSP(Matrix3), _Matrix3);
	__CSP_ARCHIVE(CSP(Vector2), _Vector2);
	__CSP_ARCHIVE(CSP(Vector3), _Vector3);
	__CSP_ARCHIVE(CSP(Quat), _Quat);
	__CSP_ARCHIVE(CSP(Real), _Real);
	__CSP_ARCHIVE(CSP(Table1), _Table1);
	__CSP_ARCHIVE(CSP(Table2), _Table2);
	__CSP_ARCHIVE(CSP(Table3), _Table3);
	__CSP_ARCHIVE(CSP(LLA), _LLA);
	__CSP_ARCHIVE(CSP(UTM), _UTM);
	__CSP_ARCHIVE(CSP(ECEF), _ECEF);
	__CSP_ARCHIVE(CSP(Path), _Path);
	__CSP_ARCHIVE(CSP(Key), _Key);

	// TODO List;
	// TODO Enum
	// TODO Pointer

#undef __CSP_ARCHIVE
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

template <class BASETYPE>
inline Reader& operator>>(Reader &reader, BASETYPE &x) {
	x.serialize(reader);
	return reader;
}


template<typename T>
inline Reader& operator>>(Reader& reader, std::vector<T> &y) {
	int32 n = reader.readLength();
	y.resize(n);
	typename std::vector<T>::iterator i = y.begin();
	while (n-- > 0) reader >> (*i++);
	return reader;
}


/**
 * Abstract base class for serializing standard types and BaseTypes
 * to a data source.
 */
class CSPLIB_EXPORT Writer {
protected:
	virtual void write(void const* data, uint32 bytes)=0;

public:
	virtual ~Writer();

#ifndef SWIG
	Writer& operator<<(const char y);
	Writer& operator<<(const int8 y);
	Writer& operator<<(const uint8 y);
	Writer& operator<<(const int16 y);
	Writer& operator<<(const uint16 y);
	Writer& operator<<(const int32 y);
	Writer& operator<<(const uint32 y);
	Writer& operator<<(const int64 y);
	Writer& operator<<(const uint64 y);
	Writer& operator<<(const bool y);
	Writer& operator<<(const float y);
	Writer& operator<<(const double y);
	Writer& operator<<(const hasht &y);
	Writer& operator<<(const std::string &y);
#endif // SWIG

	void writeLength(int32 length);

	// explicit packing (use from python)

#ifdef SWIG
%extend {
	inline void _double(double x) { (*self) << x; }
	inline void _float(float x) { (*self) << x; }
	inline void _bool(bool x) { (*self) << x; }
	inline void _int64(int64 x) { (*self) << x; }
	inline void _int32(int32 x) { (*self) << x; }
	inline void _int16(int16 x) { (*self) << x; }
	inline void _int8(int8 x) { (*self) << x; }
	inline void _char(char x) { (*self) << x; }
	inline void _uint8(uint8 x) { (*self) << x; }
	inline void _uint16(uint16 x) { (*self) << x; }
	inline void _uint32(uint32 x) { (*self) << x; }
	inline void _uint64(uint64 x) { (*self) << x; }
	inline void _hasht(hasht const &x) { (*self) << x; }
	inline void _string(std::string const &x) { (*self) << x; }
	inline void _Object(Object const &x) { (*self) << x; }
	inline void _ECEF(ECEF const &x) { (*self) << x; }
	inline void _LLA(LLA const &x) { (*self) << x; }
	inline void _Key(Key const &x) { (*self) << x; }
	inline void _UTM(UTM const &x) { (*self) << x; }
	inline void _Matrix3(Matrix3 const &x) { (*self) << x; }
	inline void _Path(Path const &x) { (*self) << x; }
	inline void _Quat(Quat const &x) { (*self) << x; }
	inline void _Real(Real const &x) { (*self) << x; }
	inline void _SimDate(SimDate const &x) { (*self) << x; }
	inline void _Table1(Table1 const &x) { (*self) << x; }
	inline void _Table2(Table2 const &x) { (*self) << x; }
	inline void _Table3(Table3 const &x) { (*self) << x; }
	inline void _Vector2(Vector2 const &x) { (*self) << x; }
	inline void _Vector3(Vector3 const &x) { (*self) << x; }
}
#endif // SWIG

};


template <class BASETYPE>
inline Writer& operator<<(Writer &writer, BASETYPE const &x) {
	x.serialize(writer);
	return writer;
}

template<typename T>
inline Writer& operator<<(Writer &writer, std::vector<T> const &x) {
	writer.writeLength(x.size());
	typename std::vector<T>::const_iterator i = x.begin();
	for (; i != x.end(); ++i) writer << (*i);
	return writer;
}


/** Utility class for writing raw data to an object archive.
 *
 *  ArchiveWriter instances are created by the DataArchive class when an
 *  object is being serialized.  The instance stores a FILE object and
 *  provides methods to write variables of various types to the file in
 *  a standard format.
 *
 *  @author Mark Rose <mkrose@users.sourceforge.net>
 */
class CSPLIB_EXPORT ArchiveWriter: public Writer {
public:
	ArchiveWriter(PackFile f);

	void resetCount();

	int32 getCount();

protected:
	virtual void write(const void* x, uint32 n);

private:
	FILE *_f;
	int32 _n;
};


/** Utility class for extracting raw data from an object archive.
 *
 *  ArchiveReader instances are created by the DataArchive class when an
 *  object is being deserialized.  The instance stores all the data needed
 *  to reconstruct the object, and provides access methods for translating
 *  the raw bytes into variables of various types.
 *
 *  @author Mark Rose <mkrose@users.sourceforge.net>
 */
class CSPLIB_EXPORT ArchiveReader: public Reader {
public:
	ArchiveReader(const char* data, int32 n, DataArchive* archive=0, bool loadall=true);
};

} // namespace csp

#endif // __CSPLIB_DATA_ARCHIVE_H__
