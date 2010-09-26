#include <csp/csplib/data/ObjectInterface.h>
#include <csp/cspsim/CarEngine.h>

namespace csp {

CSP_XML_BEGIN(CarEngine)
CSP_XML_END

CSP_XML_BEGIN(CarEngineDynamics)
CSP_DEF("engine", m_Engine, true)
CSP_XML_END

CarEngine::CarEngine() {
}

CarEngine::~CarEngine() {
}

CarEngineDynamics::CarEngineDynamics() {
}

CarEngineDynamics::~CarEngineDynamics() {
}

void CarEngineDynamics::registerChannels(Bus*) {
}

void CarEngineDynamics::importChannels(Bus*) {
}

void CarEngineDynamics::computeForceAndMoment(double) {
}

}
