#include "stdinc.h"

#include "global.h"

#include <ObjectFactory.h>
#include <BaseObject.h>
#include <TankObject.h>
#include <AirplaneObject.h>
#include <MissileObject.h>
#include <ParticleObject.h>
#include "BaseSymbol.h"
#include "SymbolTable.h"
#include "AeroParamSymbol.h"

#include <osg/CullFace>
#include <osg/Node>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

extern SymbolTable GlobalSymbolTable;

ObjectFactory * g_pObjectFactory = new ObjectFactory;

ObjectType::~ObjectType()
{
}

void ObjectFactory::setObjectTypeAero(const string & nameString,
									const string & aeroString)
{
	std::list<ObjectType *>::iterator i;
	for (i = objectTypeList.begin(); i != objectTypeList.end(); ++i)
	{

		ObjectType *pType = *i;
		if (pType->m_name == nameString)
		{
			pType->m_AeroName = aeroString;
			return;
		}
	}

}


void ObjectFactory::setObjectTypeModel(const string & nameString,
									   const string & modelString)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectFactory::setObjectTypeModel name: "
		<< nameString << ", model: " << modelString );

    osg::Node * pNode = osgDB::readNodeFile(modelString);

	if (pNode)
	{
		CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectFactory::setObjectTypeModel: readNodeFile() succeeded, objectTypeList.size() = " << objectTypeList.size());
		std::list<ObjectType *>::iterator i;
		for (i = objectTypeList.begin(); i != objectTypeList.end(); ++i)
		{

			ObjectType *pType = *i;
			CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectFactory::setObjectTypeModel: searching " << pType->m_name);
			if (pType->m_name == nameString)
			{
				pType->m_rpNode = pNode;
				pType->m_NodeName = modelString;
				return;
			}
		}
	} else {
		CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectFactory::setObjectTypeModel: readNodeFile() failed.");
	}
}

void ObjectFactory::createObjectType(const string & nameString, 
                                  //   const string & modelString ,
                                     const string & baseString)
{

    CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectFactory::createObjectType name: "
        << nameString <<  ", base: " << baseString );
    //osgDB:: setDataFilePathList("Models");

//    osg::Node * pNode = osgDB::readNodeFile(modelString);
    bool bUsed = isTypeNameUsed(nameString);
    unsigned int type = BaseObject::getClassTypeFromName(baseString);
    if ( /* pNode && */ !bUsed && type)
    {
        ObjectType * pObjectType = new ObjectType;
//		pObjectType->m_rpNode = pNode;
        pObjectType->m_name = nameString;
//		pObjectType->m_NodeName = modelString;
        pObjectType->m_ObjectClass = type;
        objectTypeList.push_front(pObjectType);
        CSP_LOG(CSP_APP, CSP_INFO, "Added Object Type " << nameString );
    }
    else
    {
        CSP_LOG(CSP_APP, CSP_WARN, "Unable to create new object type.");
    }

}

void ObjectFactory::deleteObjectType(const string & nameString)
{
	CSP_LOG(CSP_APP, CSP_INFO, "Deleting Object Type " << nameString );
    std::list<ObjectType *>::iterator i;
    for (i = objectTypeList.begin(); i != objectTypeList.end(); ++i)
	{
        ObjectType *pType = *i;
        if (pType->m_name == nameString)
		{
			i = objectTypeList.erase(i);
			delete pType;
			return;
		}
	}
	CSP_LOG(CSP_APP, CSP_WARN, "ObjectFactory::deleteObjectType - Unable to find type " <<
		nameString );
}


bool ObjectFactory::isTypeNameUsed(const string & name)
{
    std::list<ObjectType *>::iterator i;
    for (i = objectTypeList.begin(); i != objectTypeList.end(); ++i)
    {
        ObjectType *pType = *i;
        if (pType->m_name == name)
            return true;
    }
    return false;

}

ObjectType * ObjectFactory::getObjectTypeFromName(const string & name)
{
    std::list<ObjectType *>::iterator i;
    for (i = objectTypeList.begin(); i != objectTypeList.end(); ++i)
    {
        ObjectType *pType = *i;

        if (pType->m_name == name)
            return *i;
    }
    return NULL;

}

void ObjectFactory::initialize()
{
    CSP_LOG(CSP_APP, CSP_INFO, "ObjectFactory::initialize() " );
    //osgDB:: setDataFilePathList("Models");
}


void ObjectFactory::Cleanup()
{
  CSP_LOG(CSP_APP, CSP_INFO, "ObjectFactory::Cleanup() - Removing all types.");
  while(!objectTypeList.empty())
  {
	ObjectType *pType = objectTypeList.front();
	objectTypeList.pop_front();
	delete pType;
  }
}


BaseObject * ObjectFactory::createNamedObject(const string & inputStr, const string & nameStr)
{
  CSP_LOG(CSP_APP, CSP_DEBUG, "ObjectFactory - Creating object of type " << inputStr);
  string str = inputStr;
  ConvertStringToUpper(str);

    ObjectType * pType = getObjectTypeFromName(inputStr);
    if (pType)
    {
        BaseObject * object = BaseObject::createObject(pType->m_ObjectClass);
		if ( pType->m_AeroName != "")
		{
			AirplaneObject * pAirplane = dynamic_cast<AirplaneObject*>(object);
			if (pAirplane)
			{
				BaseSymbol * pSymbol = GlobalSymbolTable.GetSymbol(pType->m_AeroName);
				{
					if (pSymbol)
					{
						AeroParamSymbol * pAero = dynamic_cast<AeroParamSymbol*>(pSymbol);
						AeroParam * pAeroParam = pAero->getAeroParam();

						if (pAero)
						{
							pAirplane->setAero( pAeroParam );
						}
					}
				}

			}
		}
		object->setNode( pType->m_rpNode.get() );
		object->setObjectName( nameStr );
        object->addToScene();
        return object;
    }
    else 
    {
        CSP_LOG(CSP_APP, CSP_WARN, "ObjectFactory - Unable to create object of type " << inputStr);
        return NULL;
    }


  cout << "ObjectFactory::CreateNamedObject - No object created returning NULL" << endl;
  return NULL;

}
