#include <Sensor.h>


Sensor::Sensor()
{

}

Sensor::~Sensor()
{

}

Sensor::Sensor(const Sensor &)
{

}

Sensor & Sensor::operator=(const Sensor &)
{
  return *this;
}

void Sensor::Initialize()
{

}

void Sensor::dump()
{

}

void Sensor::OnUpdate(unsigned int dt)
{

}

void Sensor::ReceiveUpdate(const ObjectData &)
{

}

void Sensor::ReceiveMessage(const Message & )
{

}

void Sensor::SetNamedValue(const char * name, const char * value)
{

}

void Sensor::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Sensor::SetValue(const char * value)
{

}

void Sensor::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Sensor::CreateNamedObject(const char * str)
{
  return NULL;
}
