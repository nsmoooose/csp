#include <Lifeform.h>


Lifeform::Lifeform()
{

}

Lifeform::~Lifeform()
{

}

Lifeform::Lifeform(const Lifeform &)
{

}

Lifeform & Lifeform::operator=(const Lifeform &)
{
  return *this;
}

void Lifeform::Initialize()
{

}

void Lifeform::dump()
{

}

void Lifeform::OnUpdate(unsigned int dt)
{

}

void Lifeform::ReceiveUpdate(const ObjectData &)
{

}

void Lifeform::ReceiveMessage(const Message & )
{

}

void Lifeform::SetNamedValue(const char * name, const char * value)
{

}

void Lifeform::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Lifeform::SetValue(const char * value)
{

}

void Lifeform::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Lifeform::CreateNamedObject(const char * str)
{
  return NULL;
}
