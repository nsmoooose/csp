#include <csp/csplib/data/ObjectInterface.h>
#include <csp/cspsim/CarObject.h>

namespace csp {

CSP_XML_BEGIN(CarObject)
CSP_XML_END

DEFINE_INPUT_INTERFACE(CarObject)

CarObject::CarObject(): DynamicObject(TYPE_MUD_UNIT) {
}

CarObject::~CarObject() {
}

} /* namespace csp */
