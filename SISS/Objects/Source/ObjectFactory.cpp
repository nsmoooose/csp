#include <stdlib.h>
#include <ObjectFactory.h>
#include <BaseObject.h>
#include <Tank.h>
#include <ArmoredUtilityVehicle.h>
#include <ArmoredFightingVehicle.h>
#include <SelfPropelledArtillery.h>
#include <TowedArtillery.h>
#include <Airplane.h>
#include <Mortar.h>
#include <Helicopter.h>
#include <SurfaceVessel.h>
#include <SubmersibleVessel.h>
#include <Spacecraft.h>
#include <GuidedMunition.h>
#include <FixedMunition.h>
#include <BallisticMunition.h>
#include <VectorHolder.h>
#include <string.h>
#include <iostream>

BaseObject *
ObjectFactory::createTankObject()
{
  return NULL;
}

BaseObject *
ObjectFactory::createAirplaneObject()
{
  return NULL;
}

BaseObject * ObjectFactory::CreateObjectFromID(char * str)
{
  unsigned long long id = atoll(str);
  unsigned long long buf;

  unsigned long long answer, remainder;
  answer = id / 1000000000000;
  remainder = id % 1000000000000;

  unsigned int kindID = answer;

  answer = remainder / 10000000000;
  remainder = remainder % 10000000000;

  unsigned int domainID = answer;

  answer = remainder / 10000000;
  remainder = remainder % 10000000;

  unsigned int countryID = answer;

  answer = remainder / 100000;
  remainder = remainder % 100000;

  unsigned int catID = answer;

  answer = remainder / 100;
  remainder = remainder % 100;

  unsigned int scatID = answer;

  unsigned int specID = remainder;

  cout << "ObjectFactory::CreateObjectFromID - " << str << endl;
  cout << "ObjectFactory::CreateObjectFromID - KindID: " << kindID << 
    ", domainID: " << domainID << 
    ", countryID: " << countryID << 
    ", catID " << catID << 
    ", scatID: " << scatID << 
    ", specID: " << specID <<
    endl;

  BaseObject * object = NULL;

  // Handle kindID==1, Platforms
  if (kindID == 1)
    {
      // Handle domainID==1, Ground Units
      if ( domainID == 1)
	{
	  if ( catID == 1 )
	    { 
	      object = new Tank;
	    }
	  else if ( catID == 2)
	    {
	      object = new ArmoredFightingVehicle;
	    }
	  else if ( catID == 3)
	    {
	      object = new ArmoredUtilityVehicle;
	    }
	  else if ( catID == 4)
	    {
	      object = new SelfPropelledArtillery;
	    }
	  else if ( catID == 5)
	    {
	      object = new TowedArtillery;
	    }
	  else if (catID == 10)
	    {
	      object = new Mortar;
	    }
	  // all other ground units
	  else
	    {
	      object = new GroundVehicle;
	    }
	}
      // handle domainID == 2, aircraft
      else if ( domainID == 2)
	{
	  // handle catID 1 through 7 airplanes
	  if ( catID >= 1 && catID <= 7)
	    {
	      object = new Airplane;
	    }
	  // handle catID 20 and greater Helicopters
	  else if ( catID >= 20 )
	    {
	      object = new Helicopter;
	    }
	}
      // handle domainID==3, SurfaceVessels
      else if (kindID == 1 && domainID == 3)
	{
	  object = new SurfaceVessel;
	}
      // handle domainID==4, SubmersibleVessels
      else if (kindID == 1 && domainID == 4)
	{
	  object = new SubmersibleVessel;
	}
      // handle domainID==5, spacecraft
      else if (kindID == 1 && domainID == 5)
	{
	  object = new Spacecraft;
	}
    }
  

  if(object)
    {
      cout << "ObjectFactory::CreateObjectFromID - setting global property id's" << endl;
      object->kind_id = kindID;
      object->domain_id = domainID;
      object->catagory_id =  catID;
      object->country_id = countryID;
      object->subcatagory_id = scatID;
      object->specific_id = specID;
      object->object_id = id;
    }

  cout << "ObjectFactory::CreateObjectFromID - Finished" << endl;
  return object;

}

BaseObject * ObjectFactory::CreateNamedObject(char * str)
{
  cout << "ObjectFactory::CreateNamedObject" << endl;

  //  if (strcmp(str, "ObjectID") == 0)
  //    {
      //      BaseObject * object = new BaseObject();
  //  return object;      
  //    }
  //  else
      return NULL;

  if (strcmp(str, "TANK") == 0)
    {
      cout << "ObjectFactory::CreateNamedObject - Creating tank object" << endl;
      BaseObject * object = new Tank();
      return object;
    }
  else if (strcmp(str, "AIRPLANE") == 0)
    {
      cout << "ObjectFactory::CreateNamedObject - Create airplane object" << endl;
      BaseObject * object = new Airplane();
      return object;
    }
  else if (strcmp(str, "POSITION_VECTOR") == 0)
    {
      cout << "ObjectFactor::CreateNamedObject - Create position object" << endl;
      BaseObject * object = new VectorHolder();
      return object;
    }
  else if (strcmp(str, "VELOCITY_VECTOR") == 0)
    {
      cout << "ObjectFactor::CreateNamedObject - Create velocity object" << endl;
      BaseObject * object = new VectorHolder();
      return object;
    }
  else if (strcmp(str, "DIRECTION_VECTOR") == 0)
    {
      cout << "ObjectFactor::CreateNamedObject - Create direction object" << endl;
      BaseObject * object = new VectorHolder();
      return object;
    }

  else if (strcmp(str, "VECTOR") == 0)
    {
      cout << "ObjectFactory::CreateNamedObject - Create vector object" << endl;
      BaseObject * object = new VectorHolder();
      return object;
    }

  else
    {
      cout << "ObjectFactory::CreateNamedObject - No object created returning NULL" << endl;
      return NULL;
    }
}
