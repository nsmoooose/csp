#include <Spacecraft.h>


Spacecraft::Spacecraft()
{

}

Spacecraft::~Spacecraft()
{

}

Spacecraft::Spacecraft(const Spacecraft &)
{

}

Spacecraft & Spacecraft::operator=(const Spacecraft &)
{
  return *this;
}

void Spacecraft::Initialize()
{

}

void Spacecraft::dump()
{

}

void Spacecraft::OnPhysics(unsigned int dt)
{

}

void Spacecraft::OnController(unsigned int dt)
{

}

void Spacecraft::OnUpdate(unsigned int dt)
{

}

void Spacecraft::ReceiveUpdate(const ObjectData &)
{

}

void Spacecraft::ReceiveMessage(const Message & )
{

}

void Spacecraft::SetNamedValue(const char * name, const char * value)
{

}

void Spacecraft::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Spacecraft::SetValue(const char * value)
{

}

void Spacecraft::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Spacecraft::CreateNamedObject(const char * str)
{
  return NULL;
}
