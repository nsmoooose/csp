#ifndef __TAGHOLDER_H_
#define __TAGHOLDER_H_

#include <BaseObject.h>
#include <ObjectData.h>
#include <Message.h>

class TagHolder : public BaseObject
{
 public:
  TagHolder();
  virtual ~TagHolder();

  TagHolder(const TagHolder &);
  TagHolder(const char * tagName);
  TagHolder & operator=(const TagHolder &);

  virtual void Initialize();
  virtual void dump();

  virtual void OnUpdate(unsigned int dt);
  virtual void ReceiveUpdate(const ObjectData &) = 0;
  virtual void ReceiveMessage(const Message & ) = 0;

  virtual void SetNamedValue(const char * name, const char * value);
  virtual void SetNamedObject(const char * name, const BaseObject *);
  virtual void SetValue(const char * value);

  virtual BaseObject * CreateNamedObject(const char * str);



  string m_tag;


};


#endif
