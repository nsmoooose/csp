#include <Munition.h>


Munition::Munition()
{

}

Munition::~Munition()
{

}

Munition::Munition(const Munition &)
{

}

Munition & Munition::operator=(const Munition &)
{
  return *this;
}

void Munition::dump()
{

}

void Munition::Initialize() 
{

}

void Munition::OnPhysics(unsigned int dt)
{

}

void Munition::OnController(unsigned int dt)
{

}


void Munition::OnUpdate(unsigned int dt)
{

}

void Munition::ReceiveUpdate(const ObjectData &)
{

}

void Munition::ReceiveMessage(const Message & )
{

}

void Munition::SetNamedValue(const char * name, const char * value)
{

}

void Munition::SetNamedObject(const char * name, const BaseObject * value)
{
 
}

void Munition::SetValue(const char * value)
{

}

void Munition::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Munition::CreateNamedObject(const char * str)
{
  return NULL;
}
