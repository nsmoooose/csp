#include <SurfaceVessel.h>


SurfaceVessel::SurfaceVessel()
{

}

SurfaceVessel::~SurfaceVessel()
{

}

SurfaceVessel::SurfaceVessel(const SurfaceVessel &)
{

}

SurfaceVessel & SurfaceVessel::operator=(const SurfaceVessel &)
{
  return *this;
}

void SurfaceVessel::Initialize()
{

}

void SurfaceVessel::dump()
{
  cout << "Dumping SurfaceVessel" << endl;
  // TODO Dump Surface
  Platform::dump();
}

void SurfaceVessel::OnPhysics(unsigned int dt)
{

}

void SurfaceVessel::OnController(unsigned int dt)
{

}

void SurfaceVessel::OnUpdate(unsigned int dt)
{

}

void SurfaceVessel::ReceiveUpdate(const ObjectData &)
{

}

void SurfaceVessel::ReceiveMessage(const Message & )
{

}

void SurfaceVessel::SetNamedValue(const char * name, const char * value)
{
  cout << "SurfaceVessel::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle SurfaceVessel specific values
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

void SurfaceVessel::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "Surface::SetNamedParameter Name: " << name <<  endl;

  // handle SurfaceVessel specific objects.
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

void SurfaceVessel::SetValue(const char * value)
{

}

void SurfaceVessel::SetTokenizedValue(int token, const char * value)
{
  cout << "SurfaceVessel::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // SurfaceVessel specific values

      // call base class
    default:
      {
	Platform::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * SurfaceVessel::CreateNamedObject(const char * name)
{
  cout << "SurfaceVessel::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return Platform::CreateNamedObject(name); 
}
