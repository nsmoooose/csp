#ifndef __INSTANTACTIONMANAGER_H__
#define __INSTANTACTIONMANAGER_H__

#include <BaseController.h>
#include <AirplaneObject.h>

using namespace std;

class InstantActionController;

class InstantActionManager : public BaseObject
{
 public:

  friend InstantActionController;

  InstantActionManager();
  virtual ~InstantActionManager();

  virtual void dump();

  virtual void OnUpdate(float dt);

  virtual void Initialize();
  virtual unsigned int OnRender();
  virtual void SetNamedParameter(const char * name, const char * value);

  void SetPlayerPlane(AirplaneObject * pPlane);

 protected:
     AirplaneObject * p_PlayerPlane;
    float m_planeRange;
    float m_tankRange;

};

#endif

