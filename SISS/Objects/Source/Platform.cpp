#include <Platform.h>


Platform::Platform()
{

}

Platform::~Platform()
{

}

Platform::Platform(const Platform &)
{

}

Platform & Platform::operator=(const Platform &)
{
  return *this;
}

void Platform::Initialize()
{

}

void Platform::dump()
{
  cout << "Dumping Platform" << endl;
  PhysicalEntity::dump();
}

void Platform::OnUpdate(unsigned int dt)
{

}

void Platform::ReceiveUpdate(const ObjectData &)
{

}

void Platform::ReceiveMessage(const Message & )
{

}

void Platform::SetNamedValue(const char * name, const char * value)
{
  cout << "Platform::SetNamedValue - name: " << name << ", value " << value << endl;
  // handle Platform specific values


  // otherwise call base class
  PhysicalEntity::SetNamedValue(name, value);
}

void Platform::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "Platform::SetNamedObject - name: " << name << ", value " << value << endl;
  // handle platform specific objects


  // otherwise call base class
  PhysicalEntity::SetNamedObject(name, value);

}

void Platform::SetValue(const char * value)
{

}

void Platform::SetTokenizedValue(int token, const char * value)
{
  cout << "Platform::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // handle Platform specific values


      // otherwise call base classes
    default:
      PhysicalEntity::SetTokenizedValue(token, value);
      break;
    }
}

BaseObject * Platform::CreateNamedObject(const char * str)
{
  return NULL;
}
