#ifndef __CSPSIM_CARENGINE_H__
#define __CSPSIM_CARENGINE_H__

#include <csp/cspsim/BaseDynamics.h>
#include <csp/cspsim/Engine.h>

namespace csp {

class CarEngine : public Engine {
public:
	CSP_DECLARE_OBJECT(CarEngine)

	CarEngine();
	virtual ~CarEngine();

private:
};

class CarEngineDynamics : public BaseDynamics {
public:
	CSP_DECLARE_OBJECT(CarEngineDynamics)

	CarEngineDynamics();
	virtual ~CarEngineDynamics();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

	virtual void computeForceAndMoment(double x);

private:
};

}

#endif
