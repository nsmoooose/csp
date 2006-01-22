/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
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

// TODO =============================================================
// pack tags; group them at the start of the record:
//   bit 7: packed (1) or offset (0)
//   packed:
//     bits 0-6 indicate which of the next 7 tags are present (1) or
//     absent (0)
//   offset:
//     bits 0-6 are the offset to the next tag, relative to the last
//     known tag. (1-127)
//       special cases:
//             0: reserved
//       101-126: reserved
//           127: offset is greater than 100, is contained in the next
//                two bytes


#ifndef __CSPLIB_NET_TAGGED_RECORD_H__
#define __CSPLIB_NET_TAGGED_RECORD_H__

#include <csp/csplib/data/Archive.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Namespace.h>

#include <string>
#include <stack>
#include <vector>
#include <sstream>
#include <cassert>
#include <ostream>


CSP_NAMESPACE


/** Simple Writer class for serializing to a memory buffer.
 *  @ingroup net
 */
class CSPLIB_EXPORT BufferWriter: public Writer {
	uint8 *_buffer;
	uint8 *_write;
	uint8 *_end;

protected:
	virtual void write(void const* data, uint32 bytes) {
		assert(_write + bytes <= _end);
		memcpy(_write, data, bytes);
		_write += bytes;
	}

public:
	BufferWriter(): Writer() { }
	BufferWriter(uint8 *buffer, uint32 size): Writer() {
		bind(buffer, size);
	}
	inline uint32 residual() const { return _end - _write; }
	inline uint8 *buffer() const { return _buffer; }
	inline bool full() const { return _write == _end; }
	inline uint32 length() const { return _write - _buffer; }
	void bind(uint8 *buffer, uint32 size) {
		_buffer = buffer;
		_write = buffer;
		_end = buffer + size;
	}
};

/** Simple Reader class for serializing from a memory buffer.
 *  @ingroup net
 */
class CSPLIB_EXPORT BufferReader: public Reader {
public:
	BufferReader(): Reader(0, 0) { }
	BufferReader(uint8 const *buffer, uint32 size): Reader(buffer, size) { }
	void bind(uint8 const *buffer, uint32 size) { Reader::bind(buffer, size); }
};

/** Simple Writer class for serializing to a string buffer.
 *  @ingroup net
 */
class CSPLIB_EXPORT StringWriter: public Writer {
	std::string _buffer;
protected:
	virtual void write(void const* data, uint32 bytes) {
		_buffer.append(reinterpret_cast<const char*>(data), bytes);
	}
public:
	StringWriter(): Writer() { _buffer.reserve(1024); }
	inline std::string const & str() const { return _buffer; }
};

/** Simple Reader class for serializing from a string buffer.
 *  @ingroup net
 */
class CSPLIB_EXPORT StringReader: public Reader {
public:
	StringReader(std::string const &buffer):
		Reader(reinterpret_cast<uint8 const*>(buffer.data()), buffer.size()) { }
};


class DumpWriter;
class TagReader;
class TagWriter;

/** Base class for objects supporting tagged serialization of data fields.
 *  @ingroup net
 */
class CSPLIB_EXPORT TaggedGroup : public Referenced {
public:
	virtual void dump(DumpWriter &, const char *name=0) const=0;
	virtual void serialize(TagReader &reader) = 0;
	virtual void serialize(TagWriter &writer) const = 0;
};


/** Base class for TagWriter and TagReader which manages a stack
 *  of nested records.
 *  @ingroup net
 */
class CSPLIB_EXPORT TagBase {
 protected:
	std::stack<int> _tagstack;
	enum { TAG_END = 0 };
	int _lasttag;
 public:
	TagBase(): _lasttag(0) { }
};


/** Class for writing tagged records to wire format.
 *  @ingroup net
 */
class CSPLIB_EXPORT TagWriter: public TagBase {
 public:
	Writer &writer;
	explicit TagWriter(Writer &writer_): TagBase(), writer(writer_) { }
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
	inline void writeLength(int x) {
		writer.writeLength(x);
	}
	inline TagWriter &operator << (TaggedGroup const &group) { group.serialize(*this); return *this; }
};


/** Class for reading tagged records from wire format.
 *  @ingroup net
 */
class CSPLIB_EXPORT TagReader: public TagBase {
 public:
	Reader &reader;
	explicit TagReader(Reader &reader_): TagBase(), reader(reader_) { }
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
	inline unsigned int readLength() {
		return reader.readLength();
	}
	template <typename T>
	inline TagReader &operator >> (T &t) { reader >> t; return *this; }
	inline TagReader &operator >> (TaggedGroup &group) { group.serialize(*this); return *this; }
};


class DumpLine;

/** Helper class for formatting indented text to an output stream.
 *  @ingroup net
 */
class CSPLIB_EXPORT DumpWriter {
	friend class DumpLine;
	std::string m_indent;
	mutable std::ostream &m_os;
	inline std::ostream &os() const { return m_os; }
	bool m_prefixed;
public:
	DumpWriter(std::ostream &os): m_os(os), m_prefixed(false) { }
	inline void indent() { m_indent += "  "; }
	inline void dedent() { if (m_indent.size() > 0) m_indent.erase(m_indent.size()-2); }
	inline DumpLine line();
	inline std::ostream & prefix() { m_prefixed = true; return os() << m_indent; }
};


/** Helper class for DumpWriter for formatting a single line of indented text.
 *  @ingroup net
 */
class CSPLIB_EXPORT DumpLine {
	mutable DumpWriter &m_writer;
public:
	inline std::ostream &os() const { return m_writer.os(); }
	inline DumpWriter &writer() const { return m_writer; }
	DumpLine(DumpWriter &writer): m_writer(writer) { }
	~DumpLine() { m_writer.os() << "\n"; }
	template <typename T>
	inline DumpLine const &operator<<(T const &t) const { os() << t; return *this; }
	inline DumpLine const &operator<<(TaggedGroup const &g) const { g.dump(writer()); return *this; }
};


inline DumpLine DumpWriter::line() {
	if (!m_prefixed) {
		os() << m_indent;
	} else {
		m_prefixed = false;
	}
	return DumpLine(*this);
}

inline std::ostream& operator <<(std::ostream& os, const TaggedGroup &group) {
	DumpWriter writer(os);
	group.dump(writer);
	return os;
}


/** Base class for auto-generated record classes that can be serialized
 *  to a tagged binary format.  Subclasses are generated by compiling
 *  record definitions with the TaggedRecordCompiler.
 *  @ingroup net
 */
class CSPLIB_EXPORT TaggedRecord: public TaggedGroup {
public:
	typedef uint64 Id;
	virtual Id getId() const=0;
	virtual int getVersion() const=0;
	virtual std::string getName() const=0;
	virtual int getCustomId() const=0;
	template <class TR>
	static inline Ref<TR> FastCast(Ref<TaggedRecord> const &record) {
		return (record->getId() == TR::_getId() ? static_cast<TR*>(record.get()) : 0);
	}
protected:
	virtual ~TaggedRecord() {}
};


CSP_NAMESPACE_END

#endif // __CSPLIB_NET_TAGGED_RECORD_H__

