#include <ArmoredFightingVehicle.h>


ArmoredFightingVehicle::ArmoredFightingVehicle()
{

}

ArmoredFightingVehicle::~ArmoredFightingVehicle()
{

}

ArmoredFightingVehicle::ArmoredFightingVehicle(const ArmoredFightingVehicle &)
{

}

ArmoredFightingVehicle & ArmoredFightingVehicle::operator=(const ArmoredFightingVehicle &)
{
  return *this;
}

void ArmoredFightingVehicle::Initialize()
{

}

void ArmoredFightingVehicle::dump()
{
  cout << "Dumping ArmoredFightingVehicle" << endl;
  // TODO Dump ArmoredFightingVehicle
  GroundVehicle::dump();
}

void ArmoredFightingVehicle::OnPhysics(unsigned int dt)
{

}

void ArmoredFightingVehicle::OnController(unsigned int dt)
{

}

void ArmoredFightingVehicle::OnUpdate(unsigned int dt)
{

}

void ArmoredFightingVehicle::ReceiveUpdate(const ObjectData &)
{

}

void ArmoredFightingVehicle::ReceiveMessage(const Message & )
{

}

void ArmoredFightingVehicle::SetNamedValue(const char * name, const char * value)
{
  cout << "ArmoredFightingVehicle::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle ArmoredFightingVehicle specific values
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

void ArmoredFightingVehicle::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "ArmoredFightingVehicle::SetNamedParameter Name: " << name <<  endl;

  // handle tank specific objects.
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

void ArmoredFightingVehicle::SetValue(const char * value)
{

}

void ArmoredFightingVehicle::SetTokenizedValue(int token, const char * value)
{
  cout << "ArmoredFightingVehicle::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // tank specific values

      // call base class
    default:
      {
	GroundVehicle::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * ArmoredFightingVehicle::CreateNamedObject(const char * name)
{
  return NULL;
  cout << "ArmoredFightingVehicle::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return GroundVehicle::CreateNamedObject(name);
}
