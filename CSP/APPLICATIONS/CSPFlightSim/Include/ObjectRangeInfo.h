// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file ObjectRangeInfo.h
 *
 **/

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


#endif // __OBJECTRANGEINFO_H__

