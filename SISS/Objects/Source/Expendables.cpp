#include <Expendables.h>


Expendables::Expendables()
{

}

Expendables::~Expendables()
{

}

Expendables::Expendables(const Expendables &)
{

}

Expendables & Expendables::operator=(const Expendables &)
{
  return *this;
}

void Expendables::Initialize()
{

}

void Expendables::dump()
{

}

void Expendables::OnUpdate(unsigned int dt)
{

}

void Expendables::ReceiveUpdate(const ObjectData &)
{

}

void Expendables::ReceiveMessage(const Message & )
{

}

void Expendables::SetNamedValue(const char * name, const char * value)
{

}

void Expendables::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Expendables::SetValue(const char * value)
{

}

void Expendables::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Expendables::CreateNamedObject(const char * str)
{
  return NULL;
}
