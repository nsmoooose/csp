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
#endif
