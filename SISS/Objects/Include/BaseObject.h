#ifndef __BASEOBJECT_H__
#define __BASEOBJECT_H__

#include <string>
#include <Message.h>
#include <ObjectData.h>

class BaseObject
{

  // Methods
 public:

  BaseObject();
  virtual ~BaseObject();

  BaseObject(const BaseObject &);
  BaseObject & operator=(const BaseObject &);

  virtual void Initialize() = 0;
  virtual void dump();

  virtual void OnUpdate(unsigned int dt) = 0;
  virtual void ReceiveUpdate(const ObjectData &) = 0;
  virtual void ReceiveMessage(const Message & ) = 0;

  virtual void SetNamedValue(const char * name, const char * value);
  virtual void SetNamedObject(const char * name, const BaseObject * value);
  virtual void SetValue(const char * value);
  virtual void SetTokenizedValue(int token, const char * value);

  virtual BaseObject * CreateNamedObject(const char * str);


  unsigned long long GetObjectID() { return object_id; }
  unsigned int GetKindID() {return kind_id; }
  unsigned int GetDomainID() { return domain_id; }
  unsigned int GetCatagoryID() { return catagory_id; }
  unsigned int GetSubCatagoryID() { return subcatagory_id; }
  unsigned int GetSpecificID() { return specific_id; }
  unsigned int GetInstanceID() { return instance_id; }

  void SetObjectID(unsigned long long ID);

  void SetObjectID(unsigned int kind_id,
		   unsigned int domain_id,
		   unsigned int catagory_id,
		   unsigned int subcatagory_id,
		   unsigned int specific_id,
		   unsigned int instance_id);

  // Attributes
 public:
  unsigned int object_type;
  string object_name;
  bool deleteFlag;

  unsigned int kind_id;
  unsigned int domain_id;
  unsigned int catagory_id;
  unsigned int subcatagory_id;
  unsigned int specific_id;
  unsigned int instance_id;
  unsigned int country_id;

  string kind_str;
  string domain_str;
  string catagory_str;
  string subcatagory_str;
  string specific_str;
  string country_str;

  unsigned long long object_id;

  bool localFlag;


};

#endif

