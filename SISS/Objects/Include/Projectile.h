#ifndef __PROJECTILEOBJECT_H__
#define __PROJECTILEOBJECT_H__

#include <Framework.h>
#include <BaseObject.h>
#include <BaseEntity.h>

class Projectile : public BaseEntity
{
 public:
  Projectile();
  virtual ~Projectile();

  Projectile(const Projectile &);
  Projectile & operator=(const Projectile &);

  virtual void dump();
  virtual void Initialize();

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
  virtual BaseObject * CreateNamedObject(const char * str);
  virtual void SetTokenizedValue(int token, const char * value);


  StandardVector3 position;
  StandardVector3 velocity;

 protected:
  void DoMovement(unsigned int dt);
  

};

#endif
