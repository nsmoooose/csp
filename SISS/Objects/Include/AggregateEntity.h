#ifndef __AGGREGATEENTITY_H__
#define __AGGREGATEENTITY_H__

#include <BaseObject.h>
#include <BaseEntity.h>
#include <ObjectData.h>
#include <Message.h>

class AggregateEntity : public BaseEntity
{

 public:
  AggregateEntity();
  virtual ~AggregateEntity();

  AggregateEntity(const AggregateEntity &);
  AggregateEntity & operator=(AggregateEntity &);

  virtual void Initialize();
  virtual void dump();  

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
  virtual BaseObject * CreateNamedObject(const char * str);
  virtual void SetTokenizedValue(int token, const char * value);


};

#endif
