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

Reader::Reader(uint8_t const *buffer, uint32_t length, DataArchive *data_archive, bool load_all) :
	_buffer(buffer),
	_read(buffer),
	_end(buffer + length),
	_data_archive(data_archive),
	_load_all(load_all) { 
}

void Reader::bind(uint8_t const *buffer, uint32_t length) {
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

Reader& Reader::operator>>(int8_t &x) {
	if (_read >= _end) throw DataUnderflow();
	x = static_cast<int8_t>(*_read++);
	return *this;
}

Reader& Reader::operator>>(uint8_t &x) {
	if (_read >= _end) throw DataUnderflow();
	x = static_cast<uint8_t>(*_read++);
	return *this;
}

Reader& Reader::operator>>(int16_t &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const int16_t le = *(reinterpret_cast<int16_t const*>(_read));
	x = CSP_INT16_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(uint16_t &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const uint16_t le = *(reinterpret_cast<uint16_t const*>(_read));
	x = CSP_UINT16_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(int32_t &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const int32_t le = *(reinterpret_cast<int32_t const*>(_read));
	x = CSP_INT32_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(uint32_t &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const uint32_t le = *(reinterpret_cast<uint32_t const*>(_read));
	x = CSP_UINT32_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(int64_t &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const int64_t le = *(reinterpret_cast<int64_t const*>(_read));
	x = CSP_INT64_FROM_LE(le);
	_read += sizeof(x);
	return *this;
}

Reader& Reader::operator>>(uint64_t &x) {
	if (_read + sizeof(x) > _end) throw DataUnderflow();
	const uint64_t le = *(reinterpret_cast<uint64_t const*>(_read));
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
	uint64_t val;
	operator>>(val);
	x = hasht(static_cast<uint32_t>(val >> 32), static_cast<uint32_t>(val));
	return *this;
}

Reader& Reader::operator>>(std::string &x) {
	int32_t n = readLength();
	if (_read + n > _end) throw DataUnderflow();
	x.assign(reinterpret_cast<char const*>(_read), n);
	_read += n;
	return *this;
}

int32_t Reader::readLength() {
	if (_read >= _end) throw DataUnderflow();
	const uint32_t bytes = (static_cast<uint32_t>(*_read) & 3) + 1;
	if (_read + bytes > _end) throw DataUnderflow();
	uint8_t val[4] = {0,0,0,0};
	for (uint32_t i=0; i < bytes; ++i) val[i] = *_read++;
	const uint32_t le = *(reinterpret_cast<uint32_t*>(val));
	const uint32_t length = CSP_UINT32_FROM_LE(le);
	return static_cast<int32_t>(length >> 2);
}

Writer::~Writer() {
}

Writer& Writer::operator<<(const char y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const int8_t y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const uint8_t y) {
	write(&y, sizeof(y));
	return *this;
}

Writer& Writer::operator<<(const int16_t y) {
	const int16_t le = CSP_INT16_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const uint16_t y) {
	const uint16_t le = CSP_UINT16_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const int32_t y) {
	const int32_t le = CSP_INT32_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const uint32_t y) {
	const uint32_t le = CSP_UINT32_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const int64_t y) {
	const int64_t le = CSP_INT64_TO_LE(y);
	write(&le, sizeof(le));
	return *this;
}

Writer& Writer::operator<<(const uint64_t y) {
	const uint64_t le = CSP_UINT64_TO_LE(y);
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
	uint64_t val = y.b;
	val = (val << 32) | y.a;
	return operator<<(val);
}

Writer& Writer::operator<<(const std::string &y) {
	const int32_t n = y.length();
	writeLength(n);
	write(y.data(), n);
	return *this;
}

void Writer::writeLength(int32_t length) {
	assert(length >= 0 && length <= 1073741823);
	uint32_t bf0 = static_cast<uint32_t>(length - 0x40) & 0x80000000;
	uint32_t bf1 = static_cast<uint32_t>(length - 0x4000) & 0x80000000;
	// 0-63             : bf0=00b, 1 byte representation
	// 64-16383         : bf0=01b, 2 byte representation
	// 16384-1073741823 : bf0=11b, 4 byte representation
	bf0 = (((bf0 >> 1) | bf1) >> 30) ^ 3;
	uint32_t output = static_cast<int32_t>((length << 2) | bf0);
	// store little-endian so the bits indicating the number of bytes
	// stored are in the first byte, and the length value is packed
	// toward the front of the stream.
	output = CSP_UINT32_TO_LE(output);
	write(&output, bf0 + 1);
}

void ArchiveWriter::write(const void* x, uint32_t n) {
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

int32_t ArchiveWriter::getCount() { 
	return _n; 
}

ArchiveReader::ArchiveReader(const char* data, int32_t n, DataArchive* archive, bool loadall) :
	Reader(reinterpret_cast<const uint8_t*>(data), n, archive, loadall) { 
}

} // namespace csp
