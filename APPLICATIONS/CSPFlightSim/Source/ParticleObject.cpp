#include "stdinc.h"

#include "BaseObject.h"
#include "ParticleObject.h"
#include "BaseController.h"

// temporarily breaking the framework rules by using openGL directly.

ParticleObject::ParticleObject()
{
  num_particles = 25;
  num_redraws = 1;

  position_array = NULL;
  velocity_array = NULL;

  particle_life = 5;
  particle_time = 0;

  m_fBoundingSphereRadius = 0.001;

  m_bDeleteFlag = false;

}


ParticleObject::~ParticleObject()
{
  delete [] position_array;
  delete [] velocity_array;

}

void ParticleObject::initialize()
{
  position_array = new StandardVector3[num_particles];
  velocity_array = new StandardVector3[num_particles];

  int i;
  for (i=0;i<num_particles; i++)
  {
     position_array[i] = m_GlobalPosition;
     velocity_array[i].x = 100.0*rand()/RAND_MAX - 50.0;
     velocity_array[i].y = 100.0*rand()/RAND_MAX - 50.0;
     velocity_array[i].z = 100.0*rand()/RAND_MAX - 50.0;
  }

  particle_time = 0;


}



void ParticleObject::dump()
{

}

void ParticleObject::OnUpdate( double dt)
{

  particle_time += dt;

  if (particle_time > particle_life)
  {
    m_bDeleteFlag = true;
  }

  for (int i=0;i<num_particles; i++)
  {
    position_array[i] += dt * velocity_array[i];
  }
}

void ParticleObject::doMovement(double dt)
{

}


void ParticleObject::setNamedParameter(const char * name, const char * value)
{

}

unsigned int ParticleObject::OnRender()
{
  updateScene();
  return 0;
}

