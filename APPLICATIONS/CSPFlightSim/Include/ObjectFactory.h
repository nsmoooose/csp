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

#include <BaseObject.h>
#include <TankObject.h>
#include <AirplaneObject.h>
#include <osg/ref_ptr>
#include <osg/Node>

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

    string m_name;

	osg::ref_ptr<osg::Node> m_rpNode;
	string m_NodeName;
    unsigned int m_ObjectClass;
	string m_AeroName;

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
  BaseObject * createNamedObject(const string & typeStr, const string & nameStr);
  void createObjectType(const string & nameString, 
             const string & baseString);
  void deleteObjectType(const string & nameString);
  void setObjectTypeModel(const string & nameString,
									   const string & modelString);
  void setObjectTypeAero(const string & nameString,
									const string & aeroString);

  void initialize();
  void Cleanup();

 protected:

    bool isTypeNameUsed(const string & name);
    ObjectType * getObjectTypeFromName(const string & name);

    std::list<ObjectType *> objectTypeList;

};
#endif // __OBJECTFACTORY_H__

