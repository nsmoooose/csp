#include <ArmoredUtilityVehicle.h>


ArmoredUtilityVehicle::ArmoredUtilityVehicle()
{

}

ArmoredUtilityVehicle::~ArmoredUtilityVehicle()
{

}

ArmoredUtilityVehicle::ArmoredUtilityVehicle(const ArmoredUtilityVehicle &)
{

}

ArmoredUtilityVehicle & ArmoredUtilityVehicle::operator=(const ArmoredUtilityVehicle &)
{
  return *this;
}

void ArmoredUtilityVehicle::Initialize()
{

}

void ArmoredUtilityVehicle::dump()
{
  cout << "Dumping ArmoredUtilityVehicle" << endl;
  // TODO Dump ArmoredUtilityVehicle
  GroundVehicle::dump();
}

void ArmoredUtilityVehicle::OnPhysics(unsigned int dt)
{

}

void ArmoredUtilityVehicle::OnController(unsigned int dt)
{

}

void ArmoredUtilityVehicle::OnUpdate(unsigned int dt)
{

}

void ArmoredUtilityVehicle::ReceiveUpdate(const ObjectData &)
{

}

void ArmoredUtilityVehicle::ReceiveMessage(const Message & )
{

}

void ArmoredUtilityVehicle::SetNamedValue(const char * name, const char * value)
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

void ArmoredUtilityVehicle::SetNamedObject(const char * name, const BaseObject * value)
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

void ArmoredUtilityVehicle::SetTokenizedValue(int token, const char * value)
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

void ArmoredUtilityVehicle::SetValue(const char * value)
{

}

BaseObject * ArmoredUtilityVehicle::CreateNamedObject(const char * name)
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
