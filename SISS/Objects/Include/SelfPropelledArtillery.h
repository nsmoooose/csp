#ifndef _SELFPROPELLEDARTILLERY_H__
#define _SELFPROPELLEDARTILLERY_H__

#include <BaseObject.h>
#include <GroundVehicle.h>
#include <ObjectData.h>
#include <Message.h>


class SelfPropelledArtillery : public GroundVehicle
{

  // Methods
 public:

  SelfPropelledArtillery();
  virtual ~SelfPropelledArtillery();

  SelfPropelledArtillery(const SelfPropelledArtillery &);
  SelfPropelledArtillery & operator=(const SelfPropelledArtillery &);

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
