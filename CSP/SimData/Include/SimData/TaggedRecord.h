/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file TaggedRecord.h
 * @brief Base classes for tagged records.
 */


#ifndef __SIMDATA_TAGGED_RECORD_H__
#define __SIMDATA_TAGGED_RECORD_H__

#include <string>
#include <stack>
#include <vector>
#include <sstream>
#include <cassert>
#include <ostream>

#include <SimData/Ref.h>
#include <SimData/Archive.h>
#include <SimData/Namespace.h>


NAMESPACE_SIMDATA

// use our own, platform-neutral int64 representation
// TODO the base class (hasht) should probably renamed.
typedef hasht int64;


/**
 * Simple Writer class for serializing to a string buffer.
 */
class StringWriter: public Writer {
	std::string _buffer;
public:
	StringWriter() {
		_buffer.reserve(1024);
	}
	virtual Writer & operator<<(char const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(int16 const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(int32 const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(int64 const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(uint8 const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(uint16 const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(uint32 const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(float const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(double const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(bool const x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(hasht const &x) {
		_buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
		return *this;
	}
	virtual Writer & operator<<(char const *x) {
		assert(0);
		return *this;
	}
	virtual Writer & operator<<(BaseType const &x) {
		x.serialize(*this);
		return *this;
	}
	virtual Writer & operator<<(std::string const &x) {
		writeLength(x.size());
		_buffer.append(x);
		return *this;
	}
	std::string str() const {
		return _buffer;
	}
};

/**
 * Simple Reader class for serializing from a string buffer.
 */
class StringReader: public Reader {
	const unsigned char *_data;
	int _bytes;
public:
	StringReader(std::string const &buffer)
		: _data(reinterpret_cast<const unsigned char *>(buffer.data())),
			_bytes(buffer.size()) { }
	virtual Reader & operator>>(char &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<char const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(int16 &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<int16 const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(int32 &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<int32 const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(int64 &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<int64 const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(uint8 &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<uint8 const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(uint16 &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<uint16 const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(uint32 &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<uint32 const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(float &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<float const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(double &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<double const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(bool &x) {
		assert(_bytes >= sizeof(x));
		x = *(reinterpret_cast<bool const*>(_data));
		_data += sizeof(x);
		_bytes -= sizeof(x);
		return *this;
	}
	virtual Reader & operator>>(char * &x) {
		assert(0);
		return *this;
	}
	virtual Reader & operator>>(BaseType &x) {
		x.serialize(*this);
		return *this;
	}
	virtual Reader & operator>>(std::string &x) {
		int length = readLength();
		assert(_bytes >= length);
		x.clear();
		x.append(reinterpret_cast<const char*>(_data), length);
		_data += length;
		_bytes -= length;
		return *this;
	}
};


/**
 * Base class for TagWriter and TagReader which manages a stack
 * of nested records.
 */
class TagBase {
 protected:
	std::stack<int> _tagstack;
	enum { TAG_END = 0 };
	int _lasttag;
 public:
	TagBase(): _lasttag(0) { }
};


/**
 * Class for writing tagged records to wire format.
 */
class TagWriter: public TagBase {
 public:
	Writer &writer;
	TagWriter(Writer &writer_): TagBase(), writer(writer_) { }
	void writeTag(int tag) {
		int delta = tag - _lasttag;
		_lasttag = tag;
		if (delta < 128) {
			writer << static_cast<unsigned char>(delta);
		} else {
			unsigned char thi = static_cast<unsigned char>(delta >> 7);
			unsigned char tlo = static_cast<unsigned char>(0x80 | (delta & 0x7f));
			writer << tlo << thi;
		}
	}
	void beginCompound() {
		_tagstack.push(_lasttag);
		_lasttag = 0;
	}
	void endCompound() {
		writer << static_cast<unsigned char>(TAG_END);
		_lasttag = _tagstack.top();
		_tagstack.pop();
	}
	void writeLength(int x) {
		assert(x >= 0);
		while (x >= 128) {
			writer << static_cast<unsigned char>(0x80 | (x & 0x7f));
			x >>= 7;
		}
		writer << static_cast<unsigned char>(x);
	}
};


/**
 * Class for reading tagged records from wire format.
 */
class TagReader: public TagBase {
 public:
	Reader &reader;
	TagReader(Reader &reader_): TagBase(), reader(reader_) { }
	int nextTag() {
		uint8 ubyte;
		reader >> ubyte;
		if (ubyte == 0) { return 0; }
		int delta = ubyte & 0x7f;
		if (ubyte & 0x80) {
			reader >> ubyte;
			delta |= ubyte << 7;
		}
		_lasttag += delta;
		return _lasttag;
	}
	void beginCompound() {
		_tagstack.push(_lasttag);
		_lasttag = 0;
	}
	void endCompound() {
		_lasttag = _tagstack.top();
		_tagstack.pop();
	}
	int readLength() {
		int length = 0;
		int sh = 0;
		unsigned char x;
		do {
			reader >> x;
			length |= (x & 0x7f) << sh;
			sh += 7;
		} while (x & 0x80);
		return length;
	}
};


/**
 * Base class for auto-generated record classes that can be serialized
 * to a tagged binary format.  Subclasses are generated by compiling
 * record definitions with the TaggedRecordCompiler.
 */
class TaggedRecord: public Referenced {
public:
	typedef int64 Id;
	virtual void serialize(TagReader &reader) = 0;
	virtual void serialize(TagWriter &writer) const = 0;
	virtual Id getId() const=0;
	virtual int getVersion() const=0;
	virtual std::string getName() const=0;
	virtual void dump(std::ostream &, int indent=0) const=0;
protected:
	virtual ~TaggedRecord() {}
};


// dump a human-readable representation of a TaggedRecord instance
inline std::ostream & operator << (std::ostream &os, TaggedRecord const &tr) {
	tr.dump(os);
	return os;
}


/**
 * Small helper class for indenting TaggedRecord dumps.
 */
class Indent {
public:
	int level;
	Indent(int level_=0): level(level_) {}
	Indent & operator++() { ++level; return *this; }
	Indent & operator--() { --level; return *this; }
};

std::ostream & operator << (std::ostream &os, Indent const &indent) {
	for (int i = 0; i < indent.level; ++i) os << " ";
	return os;
}


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_TAGGED_RECORD_H__
