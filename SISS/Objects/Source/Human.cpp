#include <Human.h>


Human::Human()
{

}

Human::~Human()
{

}

Human::Human(const Human &)
{

}

Human & Human::operator=(const Human &)
{
  return *this;
}

void Human::Initialize()
{

}

void Human::dump()
{

}

void Human::OnUpdate(unsigned int dt)
{

}

void Human::ReceiveUpdate(const ObjectData &)
{

}

void Human::ReceiveMessage(const Message & )
{

}

void Human::SetNamedValue(const char * name, const char * value)
{

}

void Human::SetNamedObject(const char * name, const BaseObject * value)
{

}

void Human::SetValue(const char * value)
{

}

void Human::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Human::CreateNamedObject(const char * str)
{
  return NULL;
}
