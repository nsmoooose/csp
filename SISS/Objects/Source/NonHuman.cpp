#include <NonHuman.h>


NonHuman::NonHuman()
{

}

NonHuman::~NonHuman()
{

}

NonHuman::NonHuman(const NonHuman &)
{

}

NonHuman & NonHuman::operator=(const NonHuman &)
{
  return *this;
}

void NonHuman::Initialize()
{

}

void NonHuman::dump()
{

}

void NonHuman::OnUpdate(unsigned int dt)
{

}

void NonHuman::ReceiveUpdate(const ObjectData &)
{

}

void NonHuman::ReceiveMessage(const Message & )
{

}

void NonHuman::SetNamedValue(const char * name, const char * value)
{

}

void NonHuman::SetNamedObject(const char * name, const BaseObject * value)
{

}

void NonHuman::SetValue(const char * value)
{

}

void NonHuman::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * NonHuman::CreateNamedObject(const char * str)
{
  return NULL;
}
