#include <TowedArtillery.h>


TowedArtillery::TowedArtillery()
{

}

TowedArtillery::~TowedArtillery()
{

}

TowedArtillery::TowedArtillery(const TowedArtillery &)
{

}

TowedArtillery & TowedArtillery::operator=(const TowedArtillery &)
{
  return *this;
}

void TowedArtillery::Initialize()
{

}

void TowedArtillery::dump()
{
  cout << "Dumping TowedArtillery" << endl;
  // TODO Dump TowedArtillery
  GroundVehicle::dump();
}

void TowedArtillery::OnPhysics(unsigned int dt)
{

}
 
void TowedArtillery::OnController(unsigned int dt)
{

}


void TowedArtillery::OnUpdate(unsigned int dt)
{

}

void TowedArtillery::ReceiveUpdate(const ObjectData &)
{

}

void TowedArtillery::ReceiveMessage(const Message & )
{

}

void TowedArtillery::SetNamedValue(const char * name, const char * value)
{
  cout << "TowedArtillery::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle TowedArtillery specific values
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

void TowedArtillery::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "TowedArtillery::SetNamedParameter Name: " << name <<  endl;

  // handle TowedArtillery specific objects.
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

void TowedArtillery::SetValue(const char * value)
{

}

void TowedArtillery::SetTokenizedValue(int token, const char * value)
{
  cout << "TowedArtillery::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // TowedArtillery specific values

      // call base class
    default:
      {
	GroundVehicle::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * TowedArtillery::CreateNamedObject(const char * name)
{
  cout << "TowedArtillery::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return GroundVehicle::CreateNamedObject(name);
}
