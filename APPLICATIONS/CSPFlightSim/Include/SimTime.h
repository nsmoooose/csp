#ifndef __SIMTIME_H__
#define __SIMTIME_H__

/**
 * class SimTime
 *
 * @author unknown
 */
class SimTime
{
public:
    SimTime();
    void updateSimTime(float dt);
    float getSimTime();


    protected:

    float sim_time;

};








#endif
