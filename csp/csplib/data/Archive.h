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


/** Base class for serializing standard types and BaseTypes from a data source.
 */
class CSPLIB_EXPORT Reader {
	uint8 const *_buffer;
	uint8 const *_read;
	uint8 const *_end;
	DataArchive *_data_archive;
	bool _load_all;

protected:
	Reader(uint8 const *buffer, uint32 length, DataArchive *data_archive=0, bool load_all=false) :
		_buffer(buffer),
		_read(buffer),
		_end(buffer + length),
		_data_archive(data_archive),
		_load_all(load_all) { }

	void bind(uint8 const *buffer, uint32 length) {
		_buffer = buffer;
		_read = buffer;
		_end = buffer + length;
	}

public:
	virtual ~Reader() {}

	DataArchive* _getArchive() { return _data_archive; }
	bool _loadAll() const { return _load_all; }
	bool isComplete() const { return _read >= _end; }

#ifndef SWIG
	Reader& operator>>(char &x) {
		if (_read >= _end) throw DataUnderflow();
		x = *_read++;
		return *this;
	}

	Reader& operator>>(int8 &x) {
		if (_read >= _end) throw DataUnderflow();
		x = static_cast<int8>(*_read++);
		return *this;
	}

	Reader& operator>>(uint8 &x) {
		if (_read >= _end) throw DataUnderflow();
		x = static_cast<uint8>(*_read++);
		return *this;
	}

	Reader& operator>>(int16 &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		const int16 le = *(reinterpret_cast<int16 const*>(_read));
		x = CSP_INT16_FROM_LE(le);
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(uint16 &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		const uint16 le = *(reinterpret_cast<uint16 const*>(_read));
		x = CSP_UINT16_FROM_LE(le);
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(int32 &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		const int32 le = *(reinterpret_cast<int32 const*>(_read));
		x = CSP_INT32_FROM_LE(le);
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(uint32 &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		const uint32 le = *(reinterpret_cast<uint32 const*>(_read));
		x = CSP_UINT32_FROM_LE(le);
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(int64 &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		const int64 le = *(reinterpret_cast<int64 const*>(_read));
		x = CSP_INT64_FROM_LE(le);
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(uint64 &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		const uint64 le = *(reinterpret_cast<uint64 const*>(_read));
		x = CSP_UINT64_FROM_LE(le);
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(bool &x) {
		if (_read >= _end) throw DataUnderflow();
		char b = *_read++;
		x = (b != 0);
		return *this;
	}

	Reader& operator>>(float &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		x = *(reinterpret_cast<float const*>(_read));
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(double &x) {
		if (_read + sizeof(x) > _end) throw DataUnderflow();
		x = *(reinterpret_cast<double const*>(_read));
		_read += sizeof(x);
		return *this;
	}

	Reader& operator>>(hasht &x) {
		uint64 val;
		operator>>(val);
		x = hasht(static_cast<uint32>(val >> 32), static_cast<uint32>(val));
		return *this;
	}

	Reader& operator>>(std::string &x) {
		int32 n = readLength();
		if (_read + n > _end) throw DataUnderflow();
		x.assign(reinterpret_cast<char const*>(_read), n);
		_read += n;
		return *this;
	}
#endif // SWIG

	// old, fixed-width length implementation (DISABLED)
	// int32 _readLength() { int32 n; operator>>(n); return n; }

	int32 readLength() {
		if (_read >= _end) throw DataUnderflow();
		const uint32 bytes = (static_cast<uint32>(*_read) & 3) + 1;
		if (_read + bytes > _end) throw DataUnderflow();
		uint8 val[4] = {0,0,0,0};
		for (uint32 i=0; i < bytes; ++i) val[i] = *_read++;
		const uint32 le = *(reinterpret_cast<uint32*>(val));
		const uint32 length = CSP_UINT32_FROM_LE(le);
		return static_cast<int32>(length >> 2);
	}

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
	virtual ~Writer() {}

#ifndef SWIG
	Writer& operator<<(const char y) {
		write(&y, sizeof(y));
		return *this;
	}
	Writer& operator<<(const int8 y) {
		write(&y, sizeof(y));
		return *this;
	}
	Writer& operator<<(const uint8 y) {
		write(&y, sizeof(y));
		return *this;
	}
	Writer& operator<<(const int16 y) {
		const int16 le = CSP_INT16_TO_LE(y);
		write(&le, sizeof(le));
		return *this;
	}
	Writer& operator<<(const uint16 y) {
		const uint16 le = CSP_UINT16_TO_LE(y);
		write(&le, sizeof(le));
		return *this;
	}
	Writer& operator<<(const int32 y) {
		const int32 le = CSP_INT32_TO_LE(y);
		write(&le, sizeof(le));
		return *this;
	}
	Writer& operator<<(const uint32 y) {
		const uint32 le = CSP_UINT32_TO_LE(y);
		write(&le, sizeof(le));
		return *this;
	}
	Writer& operator<<(const int64 y) {
		const int64 le = CSP_INT64_TO_LE(y);
		write(&le, sizeof(le));
		return *this;
	}
	Writer& operator<<(const uint64 y) {
		const uint64 le = CSP_UINT64_TO_LE(y);
		write(&le, sizeof(le));
		return *this;
	}
	Writer& operator<<(const bool y) {
		char b = y ? 1 : 0;
		write(&b, sizeof(b));
		return *this;
	}
	Writer& operator<<(const float y) {
		write(&y, sizeof(y));
		return *this;
	}
	Writer& operator<<(const double y) {
		write(&y, sizeof(y));
		return *this;
	}

	Writer& operator<<(const hasht &y) {
		uint64 val = y.b;
		val = (val << 32) | y.a;
		return operator<<(val);
	}
	Writer& operator<<(const std::string &y) {
		const int32 n = y.length();
		writeLength(n);
		write(y.data(), n);
		return *this;
	}
#endif // SWIG

	void writeLength(int32 length) {
		assert(length >= 0 && length <= 1073741823);
		uint32 bf0 = static_cast<uint32>(length - 0x40) & 0x80000000;
		uint32 bf1 = static_cast<uint32>(length - 0x4000) & 0x80000000;
		// 0-63             : bf0=00b, 1 byte representation
		// 64-16383         : bf0=01b, 2 byte representation
		// 16384-1073741823 : bf0=11b, 4 byte representation
		bf0 = (((bf0 >> 1) | bf1) >> 30) ^ 3;
		uint32 output = static_cast<int32>((length << 2) | bf0);
		// store little-endian so the bits indicating the number of bytes
		// stored are in the first byte, and the length value is packed
		// toward the front of the stream.
		output = CSP_UINT32_TO_LE(output);
		write(&output, bf0 + 1);
	}

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
	FILE *_f;
	int32 _n;

protected:
	virtual void write(const void* x, uint32 n) {
		fwrite(x, n, 1, _f);
		_n += n;
	}

public:
	ArchiveWriter(PackFile f): Writer(), _n(0) {
		_f = static_cast<FILE*>(f);
		assert(_f != 0);
	}
	void resetCount() { _n = 0; }
	int32 getCount() { return _n; }
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
	ArchiveReader(const char* data, int32 n, DataArchive* archive=0, bool loadall=true):
		Reader(reinterpret_cast<const uint8*>(data), n, archive, loadall) { }
};

} // namespace csp


#endif // __CSPLIB_DATA_PACK_H__

