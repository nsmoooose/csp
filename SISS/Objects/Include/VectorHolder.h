#ifndef __VECTORHOLDER_H_
#define __VECTORHOLDER_H_

#include <BaseObject.h>
#include <ObjectData.h>
#include <Message.h>

class VectorHolder : public BaseObject
{
 public:
  VectorHolder();
  virtual ~VectorHolder();

  VectorHolder(const VectorHolder &);
  VectorHolder & operator=(const VectorHolder &);

  virtual void Initialize();
  virtual void dump();

  virtual void OnUpdate(unsigned int dt);
  virtual void ReceiveUpdate(const ObjectData &);
  virtual void ReceiveMessage(const Message & );

  virtual void SetNamedValue(const char * name, const char * value);
  virtual void SetNamedObject(const char * name, const BaseObject *);
  virtual void SetValue(const char * value);

  virtual BaseObject * CreateNamedObject(const char * str);



  StandardVector3 m_Vector;


};


#endif
