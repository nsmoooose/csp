#include <Mortar.h>


Mortar::Mortar()
{

}

Mortar::~Mortar()
{

}

Mortar::Mortar(const Mortar &)
{

}

Mortar & Mortar::operator=(const Mortar &)
{
  return *this;
}

void Mortar::dump()
{
  cout << "Dumping Mortar" << endl;
  // TODO Dump Mortar
  GroundVehicle::dump();
}

void Mortar::Initialize() 
{

}

void Mortar::OnPhysics(unsigned int dt)
{

}

void Mortar::OnController(unsigned int dt)
{

}


void Mortar::OnUpdate(unsigned int dt)
{

}

void Mortar::ReceiveUpdate(const ObjectData &)
{

}

void Mortar::ReceiveMessage(const Message & )
{

}

void Mortar::SetNamedValue(const char * name, const char * value)
{
  cout << "Mortar::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle Mortar specific values
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      GroundVehicle::SetNamedValue(name, value);
    }

}

void Mortar::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "Mortar::SetNamedParameter Name: " << name <<  endl;

  // handle mortar specific objects.
  if (0)
    {
      //TODO
    }
  // call base class if not already handled
  else
    {
      GroundVehicle::SetNamedObject(name, value);
    }

}

void Mortar::SetValue(const char * value)
{

}

void Mortar::SetTokenizedValue(int token, const char * value)
{
  cout << "Mortar::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // Mortar specific values

      // call base class
    default:
      {
	GroundVehicle::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * Mortar::CreateNamedObject(const char * name)
{
  cout << "Mortar::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return GroundVehicle::CreateNamedObject(name);
}
