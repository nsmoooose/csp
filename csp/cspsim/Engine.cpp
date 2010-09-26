#include <csp/csplib/data/ObjectInterface.h>
#include <csp/cspsim/Engine.h>

namespace csp {

CSP_XML_BEGIN(Engine)
	CSP_DEF("idle_rpm", m_IdleRPM, false)
	CSP_DEF("idle_fuel_consumption", m_IdleFuelConsumption, false)
CSP_XML_END

Engine::Engine() :
	m_Throttle(0.0),
	m_StartRPM(0.0)
{
}

Engine::~Engine() {
}

void Engine::registerChannels(Bus* bus) {
	b_RPM = bus->registerLocalDataChannel<double>("Engine.RPM", 0.0);
}

void Engine::importChannels(Bus*) {
}

}
