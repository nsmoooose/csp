#include "stdinc.h"

#include "SimTime.h"



SimTime * g_pSimTime = new SimTime;

SimTime::SimTime()
{
    sim_time = 0;
}

void SimTime::updateSimTime(float dt)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "SimTime::updateSimTime() time: " 
		<< sim_time << ", dt: " << dt);

    sim_time += dt;
}

float SimTime::getSimTime()
{
    return sim_time;
}

