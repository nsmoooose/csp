#include <csp/csplib/data/Vector2.h>
#include <csp/csplib/data/Archive.h>
#include <csp/csplib/util/StringTools.h>

#include <iomanip>
#include <sstream>

namespace csp {

std::string Vector2::asString() const {
	std::ostringstream os;
	os << "[" << std::setw(8) << x() << " "
	   << std::setw(8) << y() << "]";
	return os.str();
}

void Vector2::serialize(Reader& reader) {
	reader >> _x >> _y;
}

void Vector2::serialize(Writer& writer) const {
	writer << _x << _y;
}

void Vector2::parseXML(const char* cdata) {
	std::vector<std::string> values = SplitString(cdata);

	// Make sure that there is enough values to parse.
	if(values.size() != 2) {
		throw ParseException("SYNTAX ERROR: expecting 2 floats.");
	}

	// Parse each value.
	if(!parseDouble(values[0].c_str(), _x) ||
	   !parseDouble(values[1].c_str(), _y)) {
		throw ParseException("SYNTAX ERROR: expecting 2 floats in correct format.");
	}
}

std::ostream &operator <<(std::ostream &o, Vector2 const &v) { return o << v.asString(); }

}
