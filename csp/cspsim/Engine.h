#pragma once
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Real.h>
#include <csp/cspsim/Bus.h>

namespace csp {

class Engine: public Object {
public:
	Engine();
	virtual ~Engine();

	CSP_DECLARE_OBJECT(Engine)

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

	void setThrottle(double throttle) { m_Throttle = throttle; }
	double getThrottle() const { return m_Throttle; }

protected:
	double m_Throttle;
	double m_StartRPM;

	Real m_IdleRPM;
	Real m_IdleFuelConsumption;

	DataChannel<double>::RefT b_RPM;

private:
};

}
