#include <SelfPropelledArtillery.h>


SelfPropelledArtillery::SelfPropelledArtillery()
{

}

SelfPropelledArtillery::~SelfPropelledArtillery()
{

}

SelfPropelledArtillery::SelfPropelledArtillery(const SelfPropelledArtillery &)
{

}

SelfPropelledArtillery & SelfPropelledArtillery::operator=(const SelfPropelledArtillery &)
{
  return *this;
}

void SelfPropelledArtillery::Initialize()
{

}

void SelfPropelledArtillery::dump()
{
  cout << "Dumping SelfPropelledArtillery" << endl;
  // TODO Dump SelfPropelledArtillery
  GroundVehicle::dump();
}

void SelfPropelledArtillery::OnPhysics(unsigned int dt)
{

}

void SelfPropelledArtillery::OnController(unsigned int dt)
{

}

void SelfPropelledArtillery::OnUpdate(unsigned int dt)
{

}

void SelfPropelledArtillery::ReceiveUpdate(const ObjectData &)
{

}

void SelfPropelledArtillery::ReceiveMessage(const Message & )
{

}

void SelfPropelledArtillery::SetNamedValue(const char * name, const char * value)
{
  cout << "SelfPropelledArtillery::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle SelfPropelledArtillery specific values
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

void SelfPropelledArtillery::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "SelfPropelledArtillery::SetNamedParameter Name: " << name <<  endl;

  // handle SelfPropelledArtillery specific objects.
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

void SelfPropelledArtillery::SetValue(const char * value)
{

}

void SelfPropelledArtillery::SetTokenizedValue(int token, const char * value)
{
  cout << "SelfPropelledArtillery::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // SelfPropelledArtillery specific values

      // call base class
    default:
      {
	GroundVehicle::SetTokenizedValue(token, value);
	break;
      }
    }

}

BaseObject * SelfPropelledArtillery::CreateNamedObject(const char * name)
{
  cout << "SelfPropelledArtillery::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return GroundVehicle::CreateNamedObject(name);
}
