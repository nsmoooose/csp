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
 * @file StaticObject.h
 *
 **/

#ifndef __STATICOBJECT_H__
#define __STATCIOBJECT_H__


#include "SimObject.h"


/**
 * class StaticObject - Base class for all fixed objects in the simulation.
 *
 */
class StaticObject: public SimObject
{
public:

	StaticObject();
	virtual ~StaticObject();

	
//	EXTEND_SIMDATA_XML_INTERFACE(StaticObject, SimObject)
//		SIMDATA_XML("direction", StaticObject::m_Direction, false)
//	END_SIMDATA_XML_INTERFACE

	SIMDATA_OBJECT(StaticObject, 0, 0)

protected:
	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void dump() {}
	virtual void onUpdate(double) {}
	virtual void initialize() {}
	virtual unsigned int onRender() { return 0; }
};


#endif // __STATCIOBJECT_H__

