#ifndef __BASEPHYSICS_H__
#define __BASEPHYSICS_H__

class BasePhysics 
{
	public:
		BasePhysics() { ;}
	
    virtual void Initialize() = 0;
    virtual void DoSimStep(double dt) = 0;

};


#endif
