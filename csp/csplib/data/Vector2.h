#ifndef __CSPLIB_DATA_VECTOR2_H__
#define __CSPLIB_DATA_VECTOR2_H__

#include <csp/csplib/data/BaseType.h>

namespace csp {

/** A two-dimensional vector class using double-precision.
 *
 *  @ingroup BaseTypes
 */
class CSPLIB_EXPORT Vector2 {
public:
	Vector2(): _x(0.0), _y(0.0) {}
	Vector2(double x_, double y_): _x(x_), _y(y_) {}
	Vector2(const Vector2& v): _x(v._x), _y(v._y) {}

	Vector2& operator=(const Vector2&)=default;

	std::string asString() const;
	std::string typeString() const { return "type::Vector2"; }
	void serialize(Reader&);
	void serialize(Writer&) const;
	void parseXML(const char*);
	void convertXML() {}

	double x() const { return _x; }
	double y() const { return _y; }

private:
	double _x, _y;
};

CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, Vector2 const &v);

}

#endif
