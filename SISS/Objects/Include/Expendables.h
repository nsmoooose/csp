#ifndef __EXPENDABLES_H__
#define __EXPENDABLES_H__

#include <BaseObject.h>
#include <Munition.h>
#include <ObjectData.h>
#include <Message.h>

class Expendables : public Munition
{
  // Methods
 public:

  Expendables();
  virtual ~Expendables();

  Expendables(const Expendables &);
  Expendables & operator=(const Expendables &);

  virtual void Initialize() = 0;
  virtual void dump() = 0;

  // BaseEntity specific methods
  virtual void OnPhysics(unsigned int dt) = 0;
  virtual void OnController(unsigned int dt) = 0;

  // Object communication methods
  virtual void OnUpdate(unsigned int dt) = 0;
  virtual void ReceiveUpdate(const ObjectData &) = 0;
  virtual void ReceiveMessage(const Message & ) = 0;

  // XML handler methods
  virtual void SetNamedValue(const char * name, const char * value);
  virtual void SetNamedObject(const char * name, const BaseObject * value);
  virtual void SetValue(const char * value);
  virtual void SetTokenizedValue(int token, const char * value);

  virtual BaseObject * CreateNamedObject(const char * str);

  // Attributes

};

#endif
