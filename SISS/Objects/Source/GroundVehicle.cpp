#include <GroundVehicle.h>


GroundVehicle::GroundVehicle()
{
  cout << "GroundVehicle::GroundVehicle() - creating Ground Vehicle" << endl;
}

GroundVehicle::~GroundVehicle()
{

}

GroundVehicle::GroundVehicle(const GroundVehicle &)
{

}

GroundVehicle & GroundVehicle::operator=(const GroundVehicle &)
{
  return *this;
}

void GroundVehicle::Initialize()
{

}

void GroundVehicle::dump()
{
  cout << "Dumping GroundVehicle" << endl;
  Platform::dump();
}

void GroundVehicle::OnPhysics(unsigned int dt)
{

}

void GroundVehicle::OnController(unsigned int dt)
{

}


void GroundVehicle::OnUpdate(unsigned int dt)
{

}

void GroundVehicle::ReceiveUpdate(const ObjectData &)
{

}

void GroundVehicle::ReceiveMessage(const Message & )
{

}

void GroundVehicle::SetNamedValue(const char * name, const char * value)
{

  cout << "GroundVehicle::SetNamedValue - name: " << name << ", value " << value << endl;

  // handle GroundVehicle specific values

  
  // otherwise call base class
  Platform::SetNamedValue(name, value);

}

void GroundVehicle::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "GroundVehicle::SetNamedObject - name: " << name << ", value " << value << endl;

  // handle GroundVehicle specific objects
  

  // otherwise call base class
  Platform::SetNamedObject(name, value);

}

void GroundVehicle::SetValue(const char * value)
{

}

void GroundVehicle::SetTokenizedValue(int token, const char * value)
{
  cout << "GroundVehicle::SetTokekizedValue token: " << token << ", value: " << value << endl;

  switch(token)
    {
      // handle GroundVehicle specific cases


      // otherwise call base class
    default:
      Platform::SetTokenizedValue(token, value);
      break;
    }
}

BaseObject * GroundVehicle::CreateNamedObject(const char * str)
{
  return NULL;
}
