#include <csp/csplib/data/ObjectInterface.h>
#include <csp/cspsim/Engine.h>

namespace csp {

CSP_XML_BEGIN(Engine)
CSP_XML_END

Engine::Engine() : m_Throttle(0.0) {
}

Engine::~Engine() {
}

}
