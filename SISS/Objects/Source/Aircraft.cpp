#include <Aircraft.h>


Aircraft::Aircraft()
{

}

Aircraft::~Aircraft()
{

}

Aircraft::Aircraft(const Aircraft &)
{

}

Aircraft & Aircraft::operator=(const Aircraft &)
{
  return *this;
}

void Aircraft::dump()
{

}

void Aircraft::Initialize() 
{

}

void Aircraft::OnPhysics(unsigned int dt)
{

}

void Aircraft::OnController(unsigned int dt)
{

}


void Aircraft::OnUpdate(unsigned int dt)
{

}

void Aircraft::ReceiveUpdate(const ObjectData &)
{

}

void Aircraft::ReceiveMessage(const Message & )
{

}

void Aircraft::SetNamedValue(const char * name, const char * value)
{
  cout << "Platform::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle platform specific values
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      Platform::SetNamedValue(name, value);
    }

}

void Aircraft::SetNamedObject(const char * name, const BaseObject * value)
{
   cout << "Aircraft::SetNamedParameter Name: " << name <<  endl;

  // handle aircraft specific objects.
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      Platform::SetNamedObject(name, value);
    }
}

void Aircraft::SetValue(const char * value)
{

}

void Aircraft::SetTokenizedValue(int token, const char * value)
{
  cout << "Aircraft::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // aircraft specific values

      // call base class
    default:
      {
	Platform::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * Aircraft::CreateNamedObject(const char * name)
{
  cout << "Aircraft::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return Platform::CreateNamedObject(name);

}
