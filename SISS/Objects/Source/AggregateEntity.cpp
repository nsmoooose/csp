#include <AggregateEntity.h>

AggregateEntity::AggregateEntity()
{

}

AggregateEntity::~AggregateEntity()
{

}

AggregateEntity::AggregateEntity(const AggregateEntity &)
{

}

AggregateEntity & AggregateEntity::operator=(AggregateEntity &)
{

  return *this;
}

void AggregateEntity::Initialize()
{

}

void AggregateEntity::dump()
{
  cout << "Dumping AggregateEntity" << endl;
  // TODO Dump AggregateEntity
  BaseEntity::dump();
}

void AggregateEntity::OnUpdate(unsigned int dt)
{

}

void AggregateEntity::ReceiveUpdate(const ObjectData &)
{

}

void AggregateEntity::ReceiveMessage(const Message & )
{

}

void AggregateEntity::SetNamedValue(const char * name, const char * value)
{

}

void AggregateEntity::SetNamedObject(const char * name, const BaseObject * value)
{
 
}

void AggregateEntity::SetTokenizedValue(int token, const char * value)
{

}


void AggregateEntity::SetValue(const char * value)
{

}

BaseObject * AggregateEntity::CreateNamedObject(const char * str)
{
  return NULL;
}
