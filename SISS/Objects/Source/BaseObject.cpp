#include "BaseObject.h"
#include "iostream.h"
#include <PropertyMap.h>
#include <PropertyTokens.h>


BaseObject::BaseObject()
{
  cout << "BaseObject::BaseObject()" << endl;

  kind_id = 0;
  domain_id = 0;
  catagory_id = 0;
  subcatagory_id = 0;
  specific_id = 0;
  instance_id = 0;

  object_id = 0;

  localFlag = true;

}

BaseObject::~BaseObject()
{
  cout << "BaseObject::~BaseObject()" << endl;
}

BaseObject::BaseObject(const BaseObject & rhs)
{
  object_id = rhs.object_id;
  object_type = rhs.object_type;
  object_name = rhs.object_name;

}

BaseObject & BaseObject::operator=(const BaseObject & rhs)
{
  object_id = rhs.object_id;
  object_type = rhs.object_type;
  object_name = rhs.object_name;

  return *this;
}


void BaseObject::SetNamedValue(const char * name, const char * value)
{
  cout << "BaseObject::SetNamedValue - name: " << name << ", value " << value << endl;

  if (strcmp(name, "KIND") == 0)
    {
      kind_str = value;
    }
}

void BaseObject::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "BaseObject::SetNameObject - Name: " << name << endl;
}

void BaseObject::SetValue(const char * value)
{
  cout << "BaseObject::SetValue" << endl;
}

void BaseObject::SetTokenizedValue(int token, const char * value)
{
  cout << "BaseObject::SetTokekizedValue token: " << token << ", value: " << value << endl;
  switch(token)
    {
      // handle BaseObject specific values
    case KIND_TOK:
      kind_str = value;
      break;

    case DOMAIN_TOK:
      domain_str = value;
      break;

    case CATAGORY_TOK:
      catagory_str = value;
      break;

    case SUBCATAGORY_TOK:
      subcatagory_str = value;
      break;

    case COUNTRY_TOK:
      country_str = value;
      break;

    case SPECIFIC_TOK:
      specific_str = value;
      break;

    case COUNTRYID_TOK:
      country_id = atoi(value);
      break;


      // otherwise 
    default:
      cout << "Unable to handle this token" << endl;
    }

}

BaseObject * BaseObject::CreateNamedObject(const char * str)
{
  return NULL;
}

void BaseObject::dump()
{
  cout << "Dumping BaseObject" << endl;
  cout << "object_type: " << object_type << endl;
  cout << "object_name: " << object_name << endl;
  cout << "kind_id: " << kind_id << endl;
  cout << "domain_id: " << domain_id << endl;
  cout << "catagory_id: " << catagory_id << endl;
  cout << "subcatagory_id: " << subcatagory_id << endl;
  cout << "specific_id: " << specific_id << endl;
  cout << "instance_id: " << instance_id << endl;
  cout << "country_id: " << country_id << endl;
  cout << "kind_str: " << kind_str << endl;
  cout << "domain_str: " << domain_str << endl;
  cout << "catagory_str: " << catagory_str << endl;
  cout << "subcatagory_str: " << subcatagory_str << endl;
  cout << "specific_str: " << specific_str << endl;
  cout << "country_str: " << country_str << endl;
  cout << "object_id: " << object_id << endl;
  cout << "localFlag: " << localFlag << endl;
}
