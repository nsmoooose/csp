#include <PhysicalEntity.h>


PhysicalEntity::PhysicalEntity()
{

}

PhysicalEntity::~PhysicalEntity()
{

}

PhysicalEntity::PhysicalEntity(const PhysicalEntity &)
{

}

PhysicalEntity & PhysicalEntity::operator=(const PhysicalEntity &)
{
  return *this;
}


void PhysicalEntity::SetNamedValue(const char * name, const char * value)
{
  cout << "PhysicalEntity::SetNamedValue - name: " << name << ", value " << value << endl;
  // handle PhysicalEntity specific values
  if (0)
    {
      // TODO
    }
  else
    {
      // otherwise call base class
      BaseEntity::SetNamedValue(name, value);
    }
}

void PhysicalEntity::SetNamedObject(const char * name, const BaseObject * value)
{
   cout << "PhysicalEntity::SetNamedObject - name: " << name << ", value " << value << endl;
   // handle PhysicalEntity specific objects
   if (0)
     {
       // TODO
     }
   else
     {
   
       // otherwise call base class
       BaseEntity::SetNamedObject(name, value);
     }

}

void PhysicalEntity::SetValue(const char * value)
{

}

void PhysicalEntity::SetTokenizedValue(int token, const char * value)
{
  cout << "PhysicalEntity::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // handle PhysicalEntity specific values


      // otherwise call base class
    default:
      BaseEntity::SetTokenizedValue(token, value);
      break;
    }
}

BaseObject * PhysicalEntity::CreateNamedObject(const char * str)
{
  return NULL;
}

void PhysicalEntity::dump()
{
  cout << "Dumping PhysicalEntity" << endl;
  BaseEntity::dump();
}
