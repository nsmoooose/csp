#ifndef __AIRCRAFT_H__
#define __AIRCRAFT_H__

#include <BaseObject.h>
#include <Platform.h>

class Aircraft : public Platform
{
  // Methods
 public:

  Aircraft();
  virtual ~Aircraft();

  Aircraft(const Aircraft &);
  Aircraft & operator=(const Aircraft &);

  virtual void dump();
  virtual void Initialize();

  // BaseEntity specific methods
  virtual void OnPhysics(unsigned int dt) = 0;
  virtual void OnController(unsigned int dt) = 0;

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
