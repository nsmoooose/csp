// Combat Simulator Project
// Copyright (C) 2002, 2004 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file StringTools.cpp
 *
 **/


#include <csp/csplib/util/StringTools.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <ctype.h>
#include <iterator>
#include <limits>
#include <sstream>
#include <vector>

#if !defined(__GNUC__) && !defined(snprintf)
#define snprintf _snprintf
#endif

#include <iostream> // XXX
namespace csp {

void ConvertStringToUpper(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void ConvertStringToLower(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::string TrimString(std::string const &str, std::string const &chars) {
	const std::string::size_type begin = str.find_first_not_of(chars);
	const std::string::size_type end = str.find_last_not_of(chars);
	return begin == std::string::npos ? "" : str.substr(begin, (1 + end) - begin);
}

std::string LeftTrimString(std::string const &str, std::string const &chars) {
	const std::string::size_type begin = str.find_first_not_of(chars);
	return begin == std::string::npos ? "" : str.substr(begin);
}

std::string RightTrimString(std::string const &str, std::string const &chars) {
	const std::string::size_type end = str.find_last_not_of(chars);
	return end == std::string::npos ? "" : str.substr(0, end + 1);
}

std::vector<std::string> SplitString(const std::string& s) {
	std::istringstream is(s);
	return std::vector<std::string>(std::istream_iterator<std::string>(is), std::istream_iterator<std::string>());
}

/** Parse an integer value from a c-string.  Returns the value as an unsigned
 *  64-bit integer.  The sign is returned separately.  Accepts numerical values
 *  in standard C octal, decimal, and hex notation.  Returns true if the input
 *  is a valid numeric string.
 *
 *  This is essentially scanf("%{hh,h,,ll}i") with support for unsigned types
 *  and better portability.
 */
bool _parseInt(const char *str, uint64 &value, int &sign) {
	value = 0;
	sign = 1;
	int base = 10;
	str = skipWhitespace(str);
	char c = *str++;
	if (!c) return false;
	if (c == '-') {
		sign = -1;
		c = *str++;
	} else if (c == '+') {
		c = *str++;
	}
	if (c == '0') {
		c = *str++;
		if (!c) return true;
		if (c == 'x' || c == 'X') {
			base = 16;
			c = *str++;
		} else base = 8;
	}
	if (!c) return false;
	while (c) {
		int v;
		if (c < '0') break;
		if (base == 10) {
			if (c > '9') return false;
			v = c - '0';
		} else if (base == 16) {
			c = c | 32;
			if (c <= '9') {
				v = c - '0';
			} else if (c >= 'a' && c <= 'f') {
				v = c - 'a' + 10;
			} else return false;
		} else {  // base == 8
			if (c > '7') return false;
			v = c - '0';
		}
		uint64 next = value * base + v;
		if (next < value) return false;
		value = next;
		c = *str++;
	}
	if (c != 0) c = *skipWhitespace(str);
	return c == 0;
}

/* A helper template for various parseInt specializations.  Parses an integer
 * value of the template type from a c-string, returning true if it is a valid
 * numeric string and is within the limits of the integral type.  The limit
 * argument should be the largest positive value of the type.
 */
template <typename T>
bool _parseSignedInt(const char *s, T &x) {
	const uint64 limit = static_cast<uint64>(std::numeric_limits<T>::max());
	uint64 value;
	int sign;
	if (!_parseInt(s, value, sign)) return false;
	if (value <= limit) {
		x = static_cast<T>(value) * static_cast<T>(sign);
		return true;
	} else if (sign < 0 && value == limit + 1) {
		x = -static_cast<T>(limit) - 1;
		return true;
	}
	return false;
}

template <typename T>
bool _parseUnsignedInt(const char *s, T &x) {
	const uint64 limit = static_cast<uint64>(std::numeric_limits<T>::max());
	uint64 value;
	int sign;
	if (!_parseInt(s, value, sign)) return false;
	if (sign < 0) return false;
	if (value <= limit) {
		x = static_cast<T>(value);
		return true;
	}
	return false;
}

bool parseInt(const char *s, int64 &x) { return _parseSignedInt(s, x); }
bool parseInt(const char *s, uint64 &x) { return _parseUnsignedInt(s, x); }
bool parseInt(const char *s, int32 &x) { return _parseSignedInt(s, x); }
bool parseInt(const char *s, uint32 &x) { return _parseUnsignedInt(s, x); }
bool parseInt(const char *s, int16 &x) { return _parseSignedInt(s, x); }
bool parseInt(const char *s, uint16 &x) { return _parseUnsignedInt(s, x); }
bool parseInt(const char *s, int8 &x) { return _parseSignedInt(s, x); }
bool parseInt(const char *s, uint8 &x) { return _parseUnsignedInt(s, x); }
bool parseDouble(const char *s, double &x) {
	size_t len = strlen(s);

	// Very basic error checking of values.
	for(unsigned int i = 0;i < len;++i) {
		char value = s[i];
		if((value < '0' || value > '9') && value != '.' && value != '-')
			return false;
	}

	std::stringstream stream(s);
	stream.imbue(std::locale::classic());
	stream >> x;
	return !stream.fail();
}

// Helper class for stringprintf and friends.  Provides a fast, append-only string
// buffer that uses the stack for small string.
class FormatArg::stringbuf {
public:
	stringbuf(): buf(fixed), end(fixed), reserved(sizeof(fixed)) { }
	~stringbuf() { if (buf != fixed) free(buf); }
	std::string str() const { return std::string(buf, end); }
	void reserve(size_t n) {  // incremental, not total space
		if (reserved < static_cast<size_t>(end + n - buf)) {
			size_t len = end - buf;
			reserved += std::max(reserved, n);
			buf = (char*) realloc((buf == fixed) ? NULL : buf, reserved);
			strncpy(buf, fixed, len);
			end = buf + len;
		}
	}
	void append(const char *src) {
		const char *stop = buf + reserved;
		while (*src) {
			if (end == stop) {
				reserve(reserved);
				stop = buf + reserved;
			}
			*end++ = *src++;
		}
	}
	void append(const char *src, size_t n) {
		reserve(n);
		strncpy(end, src, n);
		end += n;
	}
	inline void append(char x) {
		reserve(1);
		*end++ = x;
	}
	inline void append(char x, size_t n) {
		reserve(n);
		while (n--) *end++ = x;
	}
private:
	char *buf;
	char *end;
	size_t reserved;
	char fixed[128];
};

struct FormatArg::formatspec {
	bool space;
	bool plus;
	bool left;
	bool zero;
	int width;
	int precision;
	char format;
	bool prefix;
};

bool parseFormat(const char *&fmt, FormatArg::formatspec &spec) {
	if (!*fmt) return false;
	spec.space = false;
	spec.plus = false;
	spec.left = false;
	spec.zero = false;
	spec.prefix = false;
	for (; ; ++fmt) {
		switch (*fmt) {
			case 0: return false;
			case '0': spec.zero = true; continue;
			case '-': spec.left = true; continue;
			case '+': spec.plus = true; continue;
			case ' ': spec.space = true; continue;
			case '#': spec.prefix = true; continue;
		}
		break;
	}
	int width = 0;
	for (; *fmt >= '0' && *fmt <= '9'; ++fmt) {
		width = width * 10 + (*fmt - '0');
		if (width > 4096) return false;
	}

	int precision = 6;
	if (*fmt == '.') {
		precision = 0;
		for (++fmt; *fmt >= '0' && *fmt <= '9'; ++fmt) {
			precision = precision * 10 + (*fmt - '0');
			if (precision > 128) return false;
		}
	}

	spec.width = width;
	spec.precision = precision;

	if (*fmt == '*') {
		spec.width = -1;
		++fmt;
	}

	// skip length modifiers; we already know the argument type.
	for (; ; ++fmt) {
		switch (*fmt) {
			case 0: return false;
			case 'h':
			case 'l':
			case 'L':
			case 'j':
			case 'z':
			case 't':
				continue;
		}
		break;
	}

	spec.format = *fmt++;
	return true;
}


template <typename INT>
bool formatIntegerType(FormatArg::stringbuf &out, FormatArg::formatspec const &spec, INT value, bool neg, int base, bool lower=true) {
	assert(base > 1 && base <= 16);
	int width = spec.width;
	INT scale = 1;
	for (INT r = value; ; ) {
		--width;
		r /= INT(base);
		if (!r) break;
		scale *= base;
	}
	if (spec.prefix) width -= ((base == 8) ? 1 : ((base == 16) ? 2 : 0));
	if (spec.plus || spec.space) --width;
	out.reserve(((width > 0) ? width : 0) + 22);
	if (!spec.left && !spec.zero && width > 0) out.append(' ', width);
	if (neg) {
		out.append('-');
	} else if (spec.plus) {
		out.append('+');
	} else if (spec.space) {
		out.append(' ');
	} else if (spec.prefix) {
		if (base == 16) {
			out.append(lower ? "0x" : "0X", 2);
		} else if (base == 8) {
			out.append('0');
		}
	}
	if (!spec.left && spec.zero && width > 0) out.append('0', width);
	if (value == 0) {
		out.append('0');
	} else {
		const char *set = lower ? "0123456789abcdef" : "0123456789ABCDEF";
		while (scale > 0) {
			int digit = static_cast<int>(value / scale);
			value -= digit * scale;
			assert(digit >= 0 && digit < base);
			out.append(set[digit]);
			scale /= base;
		}
	}
	if (spec.left && width > 0) out.append(' ', width);
	return true;
}

bool FormatArg::formatInt(stringbuf &out, formatspec const &spec, int base, bool lower) const {
	switch (x_type) {
		case TYPE_INT: {
			bool neg = x_val.i < 0;
			int x = (neg ? -x_val.i : x_val.i);
			if (neg && base != 10) {
				return formatIntegerType<unsigned>(out, spec, static_cast<unsigned>(-x), false, base, lower);
			} else {
				return formatIntegerType<int>(out, spec, x, neg, base, lower);
			}
		}
		case TYPE_UINT: {
			return formatIntegerType<unsigned>(out, spec, x_val.ui, false, base, lower);
		}
		case TYPE_INT64: {
			bool neg = x_val.i64 < 0;
			int64 x = (neg ? -x_val.i64 : x_val.i64);
			if (neg && base != 10) {
				return formatIntegerType<uint64>(out, spec, static_cast<uint64>(-x), false, base, lower);
			} else {
				return formatIntegerType<int64>(out, spec, x, neg, base, lower);
			}
		}
		case TYPE_UINT64: {
			return formatIntegerType<uint64>(out, spec, x_val.ui64, false, base, lower);
		}
		case TYPE_PTR: {
			uintptr_t val = alias_cast<uintptr_t>(x_val.p);
			return formatIntegerType<uintptr_t>(out, spec, val, false, base, lower);
		}
		default:
			return false;
	}
	return false;
}

bool FormatArg::formatFloat(stringbuf &out, formatspec const &spec, double value, char style) const {
	const int precision = spec.precision;
	int width = spec.width;
	const bool plus = spec.plus;
	const bool space = spec.space;
	const bool left = spec.left;
	const bool zero = spec.zero;

	bool neg = value < 0;
	if (neg) value = -value;
	assert(precision < 1000);
	// numeric formatting is just too hairy to implement from scratch, so we let snprintf do the
	// heavy lifting using a scrubbed format string.
	char fmt[20];
	char *fill = fmt;
	*fill++ = '%';
	*fill++ = '.';
	if (precision > 100) *fill++ = '0' + static_cast<char>((precision / 100) % 10);
	if (precision > 10) *fill++ = '0' + static_cast<char>((precision / 10) % 10);
	*fill++ = '0' + static_cast<char>(precision % 10);
	*fill++ = style;
	*fill = 0;
	// 128 bytes should be more than enough since we are doing alignment by hand.
	// in most cases this should be nearly as fast or faster than preallocating
	// space in the stringbuf and writing directly to its internal buffer.
	char buffer[128];
	int len = snprintf(buffer, sizeof(buffer), fmt, value);
	width -= len + (neg || plus || space ? 1 : 0);
	out.reserve(((width > 0) ? width + 1 : 1) + len);
	if (!left && !zero && width > 0) out.append(' ', width);
	if (neg) {
		out.append('-');
	} else if (plus) {
		out.append('+');
	} else if (space) {
		out.append(' ');
	}
	if (!left && zero && width > 0) out.append('0', width);
	out.append(buffer, len);
	if (left && width > 0) out.append(' ', width);
	return true;
}

bool FormatArg::formatChar(stringbuf &out, formatspec const &spec,  char c) const {
	if (!spec.left && spec.width > 1) out.append(' ', spec.width - 1);
	out.append(c);
	if (spec.left && spec.width > 1) out.append(' ', spec.width - 1);
	return true;
}

bool FormatArg::formatString(stringbuf &out, formatspec const &spec, const char *s, int len) const {
	int width = spec.width;
	if (width > 0) {
		if (len < 0) len = strlen(s);
		width -= len;
		out.reserve((width > 0 ? width : 0) + len);
	}
	if (!spec.left && width > 0) out.append(' ', width);
	out.append(s);
	if (spec.left && width > 0) out.append(' ', width);
	return true;
}

const FormatArg FormatArg::nil(0);

bool FormatArg::getWidth(formatspec &spec) const {
	assert(spec.width < 0);
	switch (x_type) {
		case TYPE_INT: spec.width = x_val.i; break;
		case TYPE_UINT: spec.width = static_cast<int>(x_val.ui); break;
		case TYPE_INT64: spec.width = static_cast<int>(x_val.i64); break;
		case TYPE_UINT64: spec.width = static_cast<int>(x_val.ui64); break;
		default: return false;
	}
	if (spec.width < 0) {
		spec.left = true;
		spec.width = -spec.width;
	}
	if (spec.width > 4096) spec.width = 0;
	return true;
}

std::string stringprintf(const char *fmt, FormatArg const &a0, FormatArg const &a1, FormatArg const &a2, FormatArg const &a3, FormatArg const &a4, FormatArg const &a5) {
	static const int maxarg = 5;
	FormatArg const *arg[maxarg + 1] = {0};
	if (&a0 != &FormatArg::nil) {
		arg[0] = &a0;
		if (&a1 != &FormatArg::nil) {
			arg[1] = &a1;
			if (&a2 != &FormatArg::nil) {
				arg[2] = &a2;
				if (&a3 != &FormatArg::nil) {
					arg[3] = &a3;
					if (&a4 != &FormatArg::nil) {
						arg[4] = &a4;
						if (&a5 != &FormatArg::nil) {
							arg[5] = &a5;
						}
					}
				}
			}
		}
	}

	FormatArg::stringbuf out;
	out.reserve(128);
	assert(fmt != NULL);
	const char *x = fmt;
	int index = -1;
	for (; *x; ++x) {
		if (*x == '%') {
			if (x > fmt) out.append(fmt, x - fmt);
			if (*++x == '%') {
				out.append('%');
				fmt = x + 1;
			} else {
				FormatArg::formatspec spec;
				if (!parseFormat(x, spec)) return out.str();
				if (spec.width == -1) {
					if (++index > maxarg || !arg[index] || !arg[index]->getWidth(spec)) return out.str();
				}
				if (++index > maxarg || !arg[index] || !arg[index]->format(spec, out)) return out.str();
				fmt = x--;
			}
		}
	}
	if (*fmt) out.append(fmt, x - fmt);
	return out.str();
}


bool FormatArg::format(formatspec &spec, stringbuf &out) const {
	switch (spec.format) {
		case 'd':
		case 'i':
		case 'u':
			return formatInt(out, spec, 10);
		case 'o':
			spec.space = spec.plus = false;
			return formatInt(out, spec, 8);
		case 'p':
			spec.prefix = true;
		case 'x':
		case 'X':
			spec.space = spec.plus = false;
			return formatInt(out, spec, 16, spec.format != 'X');
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
			if (x_type != TYPE_DOUBLE) return false;
			return formatFloat(out, spec, x_val.d, spec.format);
		case 'c':
			if (x_type != TYPE_CHAR) return false;
			return formatChar(out, spec, x_val.c);
		case 's':
			if (x_type != TYPE_STRING) return false;
			return formatString(out, spec, x_val.s, x_len);
		case '%':
			out.append('%');
		default:
			return false;
	}
	return true;
}

} // namespace csp

