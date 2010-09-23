#ifndef __CSPSIM_ENGINE_H__
#define __CSPSIM_ENGINE_H__

#include <csp/csplib/data/Object.h>

namespace csp {

class Bus;

class Engine: public Object {
public:
	Engine();
	virtual ~Engine();

	CSP_DECLARE_OBJECT(Engine)

	virtual void registerChannels(Bus*) { }
	virtual void importChannels(Bus*) { }

	void setThrottle(double throttle) { m_Throttle = throttle; }
	double getThrottle() const { return m_Throttle; }

protected:
	double m_Throttle;

private:
};

}

#endif
