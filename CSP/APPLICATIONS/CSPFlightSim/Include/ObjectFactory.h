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
 * @file ObjectFactory.h
 *
 **/

#ifndef __OBJECTFACTORY_H__
#define __OBJECTFACTORY_H__

#include <list>
#include <string>

#include <osg/ref_ptr>
#include <osg/Node>

#include <AirplaneObject.h>
#include <BaseObject.h>
#include <TankObject.h>



/**
 * class ObjectType
 *
 * @author unknown
 */
struct ObjectType
{
	ObjectType() 
	{
		m_name = "";
		m_NodeName = "";
		m_AeroName = "";
		m_ObjectClass = UNKNOWN_TYPE;
	}

	std::string m_name;

	osg::ref_ptr<osg::Node> m_rpNode;
	std::string m_NodeName;
    unsigned int m_ObjectClass;
	std::string m_AeroName;

	virtual ~ObjectType();

};


/**
 * class ObjectFactory
 *
 * @author unknown
 */
class ObjectFactory
{
 public:
	 BaseObject * createNamedObject(const std::string & typeStr, const std::string & nameStr);
	 void createObjectType(const std::string & nameString, 
		 const std::string & baseString);
	 void deleteObjectType(const std::string & nameString);
	 void setObjectTypeModel(const std::string & nameString,
		 const std::string & modelString);
	 void setObjectTypeAero(const std::string & nameString,
	  const std::string & aeroString);

  void initialize();
  void Cleanup();

 protected:

	 bool isTypeNameUsed(const std::string & name);
	 ObjectType * getObjectTypeFromName(const std::string & name);

    std::list<ObjectType *> objectTypeList;

};
#endif // __OBJECTFACTORY_H__

