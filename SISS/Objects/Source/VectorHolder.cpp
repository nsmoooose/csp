

#include <Framework.h>

#include <BaseObject.h>
#include <VectorHolder.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

VectorHolder::VectorHolder()
{
  cout << "VectorHolder - Constructor" << endl;
}

VectorHolder::~VectorHolder()
{
  cout << "VectorHolder - Destructor" << endl;
}

void VectorHolder::SetNamedValue(const char * name, const char * value)
{
  cout << "VectorHolder - SetNamedValue, Name: " << name << "Value: " << value << endl;
}

void VectorHolder::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "VectorHolder - SetNamedObject, Name: " << name << endl;
}

void VectorHolder::SetValue(const char * value)
{
  cout << "VectorHolder - SetValue " << value << endl;
  char * ptr; char buff[256];
  memset(buff, 0, 256);
  ptr = strchr(value, ',');
  int count = 0;
  while (value != ptr)
    {
    buff[count] = *value;
    count++; value++;
    }
  float x = atof(buff);
  value++;
  ptr = strchr(value, ',');
  count = 0;
  while (value != ptr)
    {
      buff[count] = *value;
      count++; value++;
    }
  float y = atof(buff);
  count = 0;
  value++;
  ptr = strchr(value, '\0');
  while (value != ptr)
    {
      buff[count] = *value;
      value++; count++;
    }
  float z = atof(buff);
  cout << "VectorHolder - SetValue, X: " << x << ", Y: " << y << ", Z:" << z << endl;
  m_Vector = StandardVector3(x,y,z);
  

}

BaseObject * VectorHolder::CreateNamedObject(const char * str)
{

  return NULL;
}



void VectorHolder::Initialize()
{

}

void VectorHolder::dump()
{

}

void VectorHolder::OnUpdate(unsigned int dt)
{

}

void VectorHolder::ReceiveUpdate(const ObjectData &)
{

}

void VectorHolder::ReceiveMessage(const Message & )
{

}
