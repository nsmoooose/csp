#ifndef __BALLISTICMUNITION_H_
#define __BALLISTICMUNITION_H_

#include <BaseObject.h>
#include <Munition.h>
#include <Message.h>
#include <ObjectData.h>

class BallisticMunition : public Munition
{
 public:

  // Methods
 public:

  BallisticMunition();
  virtual ~BallisticMunition();

  BallisticMunition(const BallisticMunition &);
  BallisticMunition & operator=(const BallisticMunition &);

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

};

#endif
