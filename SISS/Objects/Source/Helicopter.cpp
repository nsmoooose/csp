#include <Helicopter.h>


Helicopter::Helicopter()
{

}

Helicopter::~Helicopter()
{

}

Helicopter::Helicopter(const Helicopter &)
{

}

Helicopter & Helicopter::operator=(const Helicopter &)
{
  return *this;
}

void Helicopter::Initialize()
{

}

void Helicopter::dump()
{
  cout << "Dumping Helicopter" << endl;
  // TODO Dump Helicopter
  Aircraft::dump();
}

void Helicopter::OnPhysics(unsigned int dt)
{

}

void Helicopter::OnController(unsigned int dt)
{

}


void Helicopter::OnUpdate(unsigned int dt)
{

}

void Helicopter::ReceiveUpdate(const ObjectData &)
{

}

void Helicopter::ReceiveMessage(const Message & )
{

}

void Helicopter::SetNamedValue(const char * name, const char * value)
{
  cout << "Helicopter::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle Helicopter specific values
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      Aircraft::SetNamedValue(name, value);
    }
}

void Helicopter::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "Helicopter::SetNamedParameter Name: " << name <<  endl;

  // handle helicopter specific objects.
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      Aircraft::SetNamedObject(name, value);
    }
}

void Helicopter::SetValue(const char * value)
{

}

void Helicopter::SetTokenizedValue(int token, const char * value)
{
  cout << "Helicopter::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // Helicopter specific values

      // call base class
    default:
      {
	Aircraft::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * Helicopter::CreateNamedObject(const char * name)
{
  cout << "Helicopter::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return Aircraft::CreateNamedObject(name);
}
