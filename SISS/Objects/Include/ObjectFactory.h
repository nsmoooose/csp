#ifndef __OBJECTFACTORY_H_
#define __OBJECTFACTORY_H_


#include <BaseObject.h>
#include <Tank.h>
#include <Airplane.h>

class ObjectFactory
{
 public:
  static BaseObject * createTankObject();
  static BaseObject * createAirplaneObject();
  static BaseObject * CreateNamedObject(char * str);

  static BaseObject * CreateObjectFromID(char * str);
};


#endif
