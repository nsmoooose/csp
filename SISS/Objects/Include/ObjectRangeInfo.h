#ifndef __OBJECTRANGEINFO_H__
#define __OBJECTRANGEINFO_H__

#include <Framework.h>

class ObjectRangeInfo
{

 public:

  ObjectRangeInfo();
  virtual ~ObjectRangeInfo();
  ObjectRangeInfo(const ObjectRangeInfo &);
  ObjectRangeInfo & operator=(const ObjectRangeInfo &);
  
  void dump();

  unsigned int object_id;
  float range;
  StandardVector3 direction;

};


#endif
