#ifndef __TANKPHYSICS_H__
#define __TANKPHYSICS_H__

#include "BasePhysics.h"

class TankPhysics : public BasePhysics
{

public:
    TankPhysics();
	
    virtual void Initialize();
    virtual void DoSimStep(double dt);






};




#endif

