#include <SubmersibleVessel.h>


SubmersibleVessel::SubmersibleVessel()
{

}

SubmersibleVessel::~SubmersibleVessel()
{

}

SubmersibleVessel::SubmersibleVessel(const SubmersibleVessel &)
{

}

SubmersibleVessel & SubmersibleVessel::operator=(const SubmersibleVessel &)
{
  return *this;
}

void SubmersibleVessel::Initialize()
{

}

void SubmersibleVessel::dump()
{
  cout << "Dumping SubmersibleVessel" << endl;
  // TODO Dump SubmersibleVessel
  Platform::dump();
}

void SubmersibleVessel::OnPhysics(unsigned int dt)
{

}
 
void SubmersibleVessel::OnController(unsigned int dt)
{

}


void SubmersibleVessel::OnUpdate(unsigned int dt)
{

}

void SubmersibleVessel::ReceiveUpdate(const ObjectData &)
{

}

void SubmersibleVessel::ReceiveMessage(const Message & )
{

}

void SubmersibleVessel::SetNamedValue(const char * name, const char * value)
{
  cout << "SubmersibleVessel::SetNamedParameter Name: " << name << ", Value: " << value << endl;
  // handle Submersible specific values
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

void SubmersibleVessel::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "SubmersibleVessel::SetNamedParameter Name: " << name <<  endl;

  // handle SubmersibleVessel specific objects.
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

void SubmersibleVessel::SetValue(const char * value)
{

}

void SubmersibleVessel::SetTokenizedValue(int token, const char * value)
{
  cout << "Submersible::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // SubmersibleVessel specific values

      // call base class
    default:
      {
	Platform::SetTokenizedValue(token, value);
	break;
      }
    }
}

BaseObject * SubmersibleVessel::CreateNamedObject(const char * name)
{
  cout << "SubmersibleVessel::CreateNamedParameter Name: " << name <<  endl;
  if (0)
    {
      //TODO
    }
  else
    return Platform::CreateNamedObject(name);
}
