#include "ObjectRangeInfo.h"
#include "iostream.h"


ObjectRangeInfo::ObjectRangeInfo()
{

}

ObjectRangeInfo::~ObjectRangeInfo()
{

}

ObjectRangeInfo::ObjectRangeInfo(const ObjectRangeInfo & rhs)
{
  object_id    = rhs.object_id;
  range        = rhs.range;
  direction    = rhs.direction;

}

ObjectRangeInfo & ObjectRangeInfo::operator=(const ObjectRangeInfo & rhs)
{
  object_id    = rhs.object_id;
  range        = rhs.range;
  direction    = rhs.direction;

  return *this;
}

void ObjectRangeInfo::dump()
{
  cout << "Object_id: " << object_id << ", range: " 
       << range << ", dir_x " << direction.x << " dir_y " 
       << direction.y << ", dir_z " << direction.z << endl;
}
