#include <Supplies.h>


Supplies::Supplies()
{

}

Supplies::~Supplies()
{

}

Supplies::Supplies(const Supplies &)
{

}

Supplies & Supplies::operator=(const Supplies &)
{
  return *this;
}

void Supplies::Initialize()
{

}

void Supplies::dump()
{

}

void Supplies::OnUpdate(unsigned int dt)
{

}

void Supplies::ReceiveUpdate(const ObjectData &)
{

}

void Supplies::ReceiveMessage(const Message & )
{

}

void Supplies::SetNamedValue(const char * name, const char * value)
{

}

void Supplies::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Supplies::SetValue(const char * value)
{

}

void Supplies::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Supplies::CreateNamedObject(const char * str)
{
  return NULL;
}
