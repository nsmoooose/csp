#ifndef __PARTICLEOBJECT_H__
#define __PARTICLEOBJECT_H__

#include <BaseController.h>

using namespace std;

class ParticleObject : public BaseObject
{
 public:

  ParticleObject();
  virtual ~ParticleObject();

  virtual void dump();

  virtual void OnUpdate( double dt);
  virtual void doMovement(double dt);

  virtual void initialize();
  virtual unsigned int OnRender();

  virtual void setNamedParameter(const char * name, const char * value);

 protected:

  int num_particles;

  StandardVector3 * position_array;
  StandardVector3 * velocity_array;

  int num_redraws;

  double particle_life;
  double particle_time;

};

#endif

