#ifndef __OBJECTRANGEINFO_H__
#define __OBJECTRANGEINFO_H__

#include "TypesMath.h"

/**
 * class ObjectRangeInfo
 *
 * @author unknown
 */
class ObjectRangeInfo
{

 public:

  ObjectRangeInfo();
  virtual ~ObjectRangeInfo();
  ObjectRangeInfo(const ObjectRangeInfo &);
  ObjectRangeInfo & operator=(const ObjectRangeInfo &);
  
  void dump();
  void clear();

  unsigned int getObjectID() { return object_id; }
  void setObjectID( unsigned int id ) { object_id = id; }

  float getRange() { return range; }
  void setRange( float dist ) { range = dist; }

  StandardVector3 & getDirection() { return direction; }
  void setDirection( StandardVector3 & dir ) { direction = dir; }

 protected:


  unsigned int object_id;
  float range;
  StandardVector3 direction;

};


#endif
