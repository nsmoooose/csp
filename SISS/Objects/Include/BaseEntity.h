#ifndef __BASEENTITY_H_
#define __BASEENTITY_H_

#include <Framework.h>
#include <BaseObject.h>

class BaseEntity : public BaseObject
{
 public:
  BaseEntity();
  virtual ~BaseEntity();

  BaseEntity(const BaseEntity &);
  BaseEntity & operator=(const BaseEntity &);

  virtual void dump();
  virtual void Initialize() = 0;

  // BaseEntity specific methods
  virtual void OnPhysics(unsigned int dt) = 0;
  virtual void OnController(unsigned int dt) = 0;
  virtual void SetController(unsigned int id);

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

 protected:
  StandardVector3 m_position;
  StandardVector3 m_velocity;
  StandardVector3 m_acceleration;
  StandardVector3 m_angularVelocity;
  StandardMatrix3 m_orientation;

  bool frozenFlag;

  unsigned int controller_id;

  

};

#endif
