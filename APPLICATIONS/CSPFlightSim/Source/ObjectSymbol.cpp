#include "stdinc.h"

#include "ObjectSymbol.h"
#include "BaseObject.h"
#include "AirplaneObject.h"
#include "ObjectFactory.h"
#include "VirtualBattlefield.h"


extern ObjectFactory * g_pObjectFactory;
extern VirtualBattlefield * g_pBattlefield;

ObjectSymbol::ObjectSymbol()
{


}

std::string ObjectSymbol::SetCommand(std::deque<std::string> & args)
{
	std::string propString = args[0];
	std::string argString;

	if (propString == "POSITION" || propString == "POS")
	{
		float x,y,z;
         argString = args[1]; x = atof (argString.c_str() );
         argString = args[2]; y = atof (argString.c_str() );
         argString = args[3]; z = atof (argString.c_str() );
         BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);
         if (pObject)
             pObject->setGlobalPosition( x, y, z );
         else
         {
             return "Unable to find object " + m_Identifier;
         }

         return "OK";
 
	}
	else if ( propString == "VELOCITY" || propString == "VEL" )
	{
		float Vx, Vy, Vz;
		argString = args[1]; Vx = atof (argString.c_str() );
         argString = args[2]; Vy = atof (argString.c_str() );
         argString = args[3]; Vz = atof (argString.c_str() );
         BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);
         if (pObject)
             pObject->setVelocity( Vx, Vy, Vz );
         else
         {
             return "Unable to find object " + m_Identifier;
         }

         return "OK";
	}
	else if ( propString == "ORIENTATION" || propString == "ORI" )
	{
		float heading, pitch, roll;
		string nameString = args[1];
         argString = args[1]; heading = atof (argString.c_str() );
         argString = args[2]; pitch = atof (argString.c_str() );
         argString = args[3]; roll = atof (argString.c_str() );
         BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);
         if (pObject)
            pObject->setOrientation( heading, pitch, roll );
         else
         {
             return "Unable to find object " + m_Identifier;
         }

         return "OK";
	}
	else if (propString == "FREEZE" || propString == "SLEEP" )
	{
	  string argString = args[1];

      BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);

      if (pObject)
	  {
		if (argString == "ON")
		{
			pObject->setFreezeState(true);
		}
		else
		{
			pObject->setFreezeState(false);
		}

	  }
      else
      {
           return "Unable to find object " + m_Identifier;
      }

      return "OK";

	}
    else if ( propString == "COMPLEX_PHYSICS" )
	{
      BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);
	  // This currently only works on airplanes.
	  if (pObject)
	  {
	    if (pObject->getObjectType() == AIRPLANE_OBJECT_TYPE)
		{
			AirplaneObject * pAirplane = dynamic_cast<AirplaneObject*>(pObject);
	        string argString = args[1];

	        if (argString == "ON")
				pAirplane->setComplexPhysics(true);
			else
				pAirplane->setComplexPhysics(false);

        return "OK";
		}
	  }
      else
      {
           return "Unable to find object " + m_Identifier;
      }


	}




	return "Unknown property: " + propString;
}

std::string ObjectSymbol::GetCommand(std::deque<std::string> & args)
{
  	std::string propString = args[0];

	if (propString == "POSITION" || propString == "POS" )
    {
         BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);
         if (pObject)
		 {
             StandardVector3 pos = pObject->getGlobalPosition();
	         return pos.toString();
		 }
            
    }
	else if (propString == "VELOCITY" || propString == "VEL" )
	{
         BaseObject * pObject = g_pBattlefield->getObjectFromName(m_Identifier);
         if (pObject)
		 {
             StandardVector3 velocity = pObject->getVelocity();
	         return velocity.toString();
		 }
	}

	return "Unknown property: " + propString;

}

std::ostream & operator << (std::ostream & os, const ObjectSymbol& sym)
{
	os << "SymbolClass: " << typeid(sym).name();
	return os;

}

