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
 * @file ObjectRangeInfo.cpp
 *
 **/

#include "stdinc.h"


#include "ObjectRangeInfo.h"


using namespace std;

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
       << direction.y << ", dir_z " << direction.z << std::endl;
}

void ObjectRangeInfo::clear()
{

}

