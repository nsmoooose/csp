#include <EnvironmentalEntity.h>


EnvironmentalEntity::EnvironmentalEntity()
{

}

EnvironmentalEntity::~EnvironmentalEntity()
{

}

EnvironmentalEntity::EnvironmentalEntity(const EnvironmentalEntity &)
{

}

EnvironmentalEntity & EnvironmentalEntity::operator=(const EnvironmentalEntity &)
{
  return *this;
}

void EnvironmentalEntity::Initialize()
{

}

void EnvironmentalEntity::dump()
{

}

void EnvironmentalEntity::OnUpdate(unsigned int dt)
{

}

void EnvironmentalEntity::ReceiveUpdate(const ObjectData &)
{

}

void EnvironmentalEntity::ReceiveMessage(const Message & )
{

}

void EnvironmentalEntity::SetNamedValue(const char * name, const char * value)
{

}

void EnvironmentalEntity::SetNamedObject(const char * name, const BaseObject * value)
{

}

void EnvironmentalEntity::SetValue(const char * value)
{

}

void EnvironmentalEntity::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * EnvironmentalEntity::CreateNamedObject(const char * str)
{
  return NULL;
}
