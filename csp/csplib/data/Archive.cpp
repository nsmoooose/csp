#include <csp/csplib/data/Archive.h>

namespace csp {

PackFile::PackFile(const char *fn, const char *mode) {
	_f = (FILE*) fopen(fn, mode);
	assert(_f); // XXX add error handling
	_open = (_f != 0);
}

void PackFile::close() {
	if (_open) {
		if (_f) fclose(_f);
		_f = 0;
		_open = false;
	}
}

Reader::Reader(uint8 const *buffer, uint32 length, DataArchive *data_archive, bool load_all) :
	_buffer(buffer),
	_read(buffer),
	_end(buffer + length),
	_data_archive(data_archive),
	_load_all(load_all) { 
}

void Reader::bind(uint8 const *buffer, uint32 length) {
	_buffer = buffer;
	_read = buffer;
	_end = buffer + length;
}

Reader::~Reader() {
}

DataArchive* Reader::_getArchive() { 
	return _data_archive; 
}

bool Reader::_loadAll() const { 
	return _load_all; 
}

bool Reader::isComplete() const { 
	return _read >= _end; 
}

Reader& Reader::operator>>(char &x) {
	if (_read >= _end) throw DataUnderflow();
	x = *_read++;
	return *this;
}

Reader& Reader::operator>>(int8 &x) {
	if (_read >= _end) throw DataUnderflow();
	x = static_cast<int8>(*_read++);
	return *this;
}

Reader& Reader::operator>>(uint8 &x) {
	if (_read >= _end) throw DataUnderflow();
	x = static_cast<uint8>(*_read++);
	return *this;
}

Reader& Reader::operator>>(int16 &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const int16 le = *(reinterpret_cast<int16 const*>(_read));
	x = CSP_INT16_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(uint16 &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const uint16 le = *(reinterpret_cast<uint16 const*>(_read));
	x = CSP_UINT16_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(int32 &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const int32 le = *(reinterpret_cast<int32 const*>(_read));
	x = CSP_INT32_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(uint32 &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const uint32 le = *(reinterpret_cast<uint32 const*>(_read));
	x = CSP_UINT32_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(int64 &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const int64 le = *(reinterpret_cast<int64 const*>(_read));
	x = CSP_INT64_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(uint64 &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const uint64 le = *(reinterpret_cast<uint64 const*>(_read));
	x = CSP_UINT64_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(bool &x) {
	if (_read >= _end) throw DataUnderflow();
	char b = *_read++;
	x = (b != 0);
	return *this;
}

Reader& Reader::operator>>(float &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	x = *(reinterpret_cast<float const*>(_read));
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(double &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	x = *(reinterpret_cast<double const*>(_read));
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(hasht &x) {
	uint64 val;
	operator>>(val);
	x = hasht(static_cast<uint32>(val >> 32), static_cast<uint32>(val));
	return *this;
}

Reader& Reader::operator>>(std::string &x) {
	int32 n = readLength();
	if (_read + n > _end) throw DataUnderflow();
	x.assign(reinterpret_cast<char const*>(_read), n);
	_read += n;
	return *this;
}

int32 Reader::readLength() {
	if (_read >= _end) throw DataUnderflow();
	const uint32 bytes = (static_cast<uint32>(*_read) & 3) + 1;
	if (_read + bytes > _end) throw DataUnderflow();
	uint8 val[4] = {0,0,0,0};
	for (uint32 i=0; i < bytes; ++i) val[i] = *_read++;
	const uint32 le = *(reinterpret_cast<uint32*>(val));
	const uint32 length = CSP_UINT32_FROM_LE(le);
	return static_cast<int32>(length >> 2);
}

Writer::~Writer() {
}

Writer& Writer::operator<<(const char y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const int8 y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const uint8 y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const int16 y) {
	const int16 le = CSP_INT16_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const uint16 y) {
	const uint16 le = CSP_UINT16_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const int32 y) {
	const int32 le = CSP_INT32_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const uint32 y) {
	const uint32 le = CSP_UINT32_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const int64 y) {
	const int64 le = CSP_INT64_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const uint64 y) {
	const uint64 le = CSP_UINT64_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const bool y) {
	char b = y ? 1 : 0;
	write(&b, sizeof(b));
	return *this;
}

Writer& Writer::operator<<(const float y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const double y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const hasht &y) {
	uint64 val = y.b;
	val = (val << 32) | y.a;
	return operator<<(val);
}

Writer& Writer::operator<<(const std::string &y) {
	const int32 n = y.length();
	writeLength(n);
	write(y.data(), n);
	return *this;
}

void Writer::writeLength(int32 length) {
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

void ArchiveWriter::write(const void* x, uint32 n) {
	fwrite(x, n, 1, _f);
	_n += n;
}

ArchiveWriter::ArchiveWriter(PackFile f): Writer(), _n(0) {
	_f = static_cast<FILE*>(f);
	assert(_f != 0);
}

void ArchiveWriter::resetCount() { 
	_n = 0; 
}

int32 ArchiveWriter::getCount() { 
	return _n; 
}

ArchiveReader::ArchiveReader(const char* data, int32 n, DataArchive* archive, bool loadall) :
	Reader(reinterpret_cast<const uint8*>(data), n, archive, loadall) { 
}

} // namespace csp
