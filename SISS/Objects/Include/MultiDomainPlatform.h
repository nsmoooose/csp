#ifndef __MULTIDOMAINPLATFORM_H__
#define __MULTIDOMAINPLATFORM_H__

#include <BaseObject.h>
#include <Platform.h>
#include <Message.h>
#include <ObjectData.h>

class MultiDomainPlatform : public Platform
{

  // Methods
 public:

  MultiDomainPlatform();
  virtual ~MultiDomainPlatform();

  MultiDomainPlatform(const MultiDomainPlatform &);
  MultiDomainPlatform & operator=(const MultiDomainPlatform &);

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
