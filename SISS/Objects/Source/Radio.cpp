#include <Radio.h>


Radio::Radio()
{

}

Radio::~Radio()
{

}

Radio::Radio(const Radio &)
{

}

Radio & Radio::operator=(const Radio &)
{
  return *this;
}

void Radio::Initialize()
{

}

void Radio::dump()
{

}

void Radio::OnUpdate(unsigned int dt)
{

}

void Radio::ReceiveUpdate(const ObjectData &)
{

}

void Radio::ReceiveMessage(const Message & )
{

}

void Radio::SetNamedValue(const char * name, const char * value)
{

}

void Radio::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Radio::SetValue(const char * value)
{

}

void Radio::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Radio::CreateNamedObject(const char * str)
{
  return NULL;
}
