

#include <Framework.h>

#include <BaseObject.h>
#include <TagHolder.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

TagHolder::TagHolder()
{
  cout << "TagHolder - Constructor" << endl;
}

TagHolder::TagHolder(const char * tagName)
{

}

TagHolder::~TagHolder()
{
  cout << "TagHolder - Destructor" << endl;
}

void TagHolder::SetNamedValue(const char * name, const char * value)
{
  cout << "TagHolder - SetNamedValue, Name: " << name << "Value: " << value << endl;
}

void TagHolder::SetNamedObject(const char * name, const BaseObject * value)
{
  cout << "TagHolder - SetNamedObject, Name: " << name << endl;
}

void TagHolder::SetValue(const char * value)
{
  cout << "TagHolder - SetValue " << value << endl;

}

void TagHolder::Initialize()
{

}

void TagHolder::dump()
{

}

void TagHolder::OnUpdate(unsigned int dt)
{

}
