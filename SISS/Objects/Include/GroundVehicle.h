#ifndef __GROUNDVEHICLE_H__
#define __GROUNDVEHICLE_H__

#include <BaseObject.h>
#include <Platform.h>
#include <ObjectData.h>
#include <Message.h>

class GroundVehicle : public Platform
{


  // Methods
 public:

  GroundVehicle();
  virtual ~GroundVehicle();

  GroundVehicle(const GroundVehicle &);
  GroundVehicle & operator=(const GroundVehicle &);

  virtual void Initialize();
  virtual void dump();

  // BaseEntity specific methods
  virtual void OnPhysics(unsigned int dt);
  virtual void OnController(unsigned int dt);

  // Object communication methods
  virtual void OnUpdate(unsigned int dt);
  virtual void ReceiveUpdate(const ObjectData &);
  virtual void ReceiveMessage(const Message & );

  // XML handler methods
  virtual void SetNamedValue(const char * name, const char * value);
  virtual void SetNamedObject(const char * name, const BaseObject * value);
  virtual void SetValue(const char * value);
  virtual void SetTokenizedValue(int token, const char * value);

  virtual BaseObject * CreateNamedObject(const char * str);

  // Attributes

};

#endif
