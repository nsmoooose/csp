#ifndef _WIN32
#include <typeinfo>
#endif

#include "global.h"
#include "Platform.h"
#include "GlobalCommands.h"
#include "VirtualBattlefield.h"
#include "AirplaneObject.h"
#include "AirplaneInput.h"
#include "ObjectFactory.h"
#include "CSPFlightSim.h"
#include "LogStream.h"
#include "SimTime.h"
#include "SymbolTable.h"
#include "ObjectTypeSymbol.h"
#include "ObjectSymbol.h"
#include "AeroParamSymbol.h"
#include "TerrainSymbol.h"

extern VirtualBattlefield * g_pBattlefield;
extern BaseObject * g_pPlayerObject;
extern BaseInput * g_pPlayerInput;
extern ObjectFactory * g_pObjectFactory;
extern CSPFlightSim * g_pCSPFlightSim;
extern SimTime * g_pSimTime;

using namespace std;

SymbolTable GlobalSymbolTable;

string ProcessCommandString(string & str)
{

    CSP_LOG(CSP_APP, CSP_DEBUG, "ProcessCommandString - str: " << str );
    string rtnStr;

    if (str.length() > 0)
    {

        StringTokenizer args(str);
        if (args.size() >= 1)
        {
            string commandString = args[0];
	    ConvertStringToUpper(commandString);
            args.pop_front();

            if (commandString == "ADD")
            {
                rtnStr = AddCommand(args);
            }
            else if (commandString == "DELETE")
            {
                rtnStr = DeleteCommand(args);
            }
            else if (commandString == "GET")
            {
                rtnStr = GetCommand(args);
            }
            else if (commandString == "SET")
            {
                rtnStr = SetCommand(args);
            }
            else if (commandString == "LIST")
            {
                rtnStr = ListCommand(args);
            }
            else if (commandString == "RUN")
            {
                rtnStr = RunCommand(args);
            }
            else if (commandString == "REM")
            {
                rtnStr = "Skipping comment";
            }
			else if (commandString == "QUIT")
			{
				rtnStr = QuitCommand(args);
			}
            else
            {
                rtnStr = "Warning Unknown command ";
                CSP_LOG(CSP_APP, CSP_WARN, rtnStr);
            }
        }

    }

    CSP_LOG(CSP_APP, CSP_DEBUG, "ProcessCommandString - Result: " << rtnStr );
    return rtnStr;

}

std::string RunCommand(deque<string> & args)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "RunCommand " );

    char buff[255];
    string path = args[0];
    
    if (!OSPath::isabs(path)) {
		string base = "../Data/Scripts";
		path = OSPath::join(base, path);
    }

    path = OSPath::filter(path);
    // open file
    ifstream file(path.c_str());
    if (!file)
    {
        return "Unable to find script file " + path;
    }

	// RUN Scriptfile.
    while (!file.eof() )
    {
        file.getline(buff, 255);
		std::string sbuff = std::string(buff);
		size_t idx = sbuff.find_first_of("\r\n");
		std::string s  = sbuff.substr(0, idx);
        ProcessCommandString(s);
    }

    file.close();

    return "OK";

}

string AddCommand( deque<string> & args)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "AddCommand " );

    string varString = args[0];
    ConvertStringToUpper(varString);
    // ADD OBJECT type_identifier identifer 
    if (varString == "OBJECT")
    {
        if ( args.size() != 3)
        {
            return "Usage: ADD OBJECT Type Name";
        }
        string typeString = args[1];
        string nameString = args[2];
		// Make sure symbol is not already in the symbol table.
		if ( !GlobalSymbolTable.GetSymbol(nameString) )
		{
			ObjectSymbol * pSymbol = new ObjectSymbol;
			pSymbol->setIdentifier( nameString );
			if (GlobalSymbolTable.AddSymbol(nameString, pSymbol) )
			{
				BaseObject * pObject = g_pObjectFactory->createNamedObject(typeString, nameString);
				if (pObject)
				{
					pObject->setObjectName(nameString);
					g_pBattlefield->addObject( pObject );
					// if the name is "PLAYER" set the global player object to this one.
					if (nameString == "PLAYER")
					{
						g_pPlayerObject = pObject;

                        g_pPlayerInput = new AirplaneInput;
	                    g_pPlayerInput->SetObject(g_pPlayerObject);

						// maybe this has to be moved on another place
						g_pPlayerObject->initialize();
					}

				}
				return "OK";
			}
		}
    }

    // ADD TYPE BASE identifier
    else if (varString == "TYPE")
    {
        if ( args.size() != 3)
        {
            return "Usage: ADD TYPE BASE Name";
        }
        string baseString = args[1];
        string nameString = args[2];

		// Make sure symbol is not already in the symbol table.
		if ( !GlobalSymbolTable.GetSymbol(nameString) )
		{
			ObjectTypeSymbol * pSymbol = new ObjectTypeSymbol;
			pSymbol->setIdentifier( nameString );
			if (GlobalSymbolTable.AddSymbol(nameString, pSymbol) )
			{
				g_pObjectFactory->createObjectType(nameString, baseString);
				return "OK";
			}
			else 
				return "Unable to add identifier.";
		}
		else 
			return "Unable to add because identifier already exists.";

    }
    else if (varString == "AERO")
	{
        string nameString = args[1];

		if ( !GlobalSymbolTable.GetSymbol(nameString) )
		{
			AeroParamSymbol * pSymbol = new AeroParamSymbol;
			pSymbol->setIdentifier( nameString );
			if (GlobalSymbolTable.AddSymbol(nameString, pSymbol) )
			{

			}


		}


		return "OK";
	}
	else if (varString == "TERRAIN")
	{
		string nameString = args[1];
		if ( !GlobalSymbolTable.GetSymbol(nameString) )
		{
			TerrainSymbol * pSymbol = new TerrainSymbol;
			pSymbol->setIdentifier( nameString );
			if (GlobalSymbolTable.AddSymbol(nameString, pSymbol) )
			{

			}


		}
		else
		{
			return "Symbol " + nameString + " already exists.";
		}


		return "OK";
	}


   	return string("Unknown Add command ") + varString;


}

string DeleteCommand(deque<string> & args)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "DeleteCommand  ");
    string varString = args[0];
    ConvertStringToUpper(varString);
	// DELETE OBJECT NAME
    if (varString == "OBJECT")
    {
        string nameString = args[1];
        BaseObject * pObject = g_pBattlefield->getObjectFromName(nameString);
        if (pObject)
        {
            pObject->setDeleteFlag(true);
            return "OK";

        }
        else 
        {
             return "Unable to find object " + nameString;
        }

    }
	// DELETE TYPE NAME
    else if (varString == "TYPE")
	{
        string nameString = args[1];
		g_pObjectFactory->deleteObjectType(nameString);
	}


    return "Unknown Delete command";
}

string ListCommand(deque<string> & args)
{
    return "List Command not yet Implemented";
}

string GetCommand(deque<string> & args)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "GetCommand  ");

    string identifier = args[0];

	BaseSymbol * pSymbol = GlobalSymbolTable.GetSymbol(identifier);
	if (pSymbol)
	{
		string className = typeid(*pSymbol).name();
		if (className == "class ObjectSymbol" )
		{
			args.pop_front();
			return pSymbol->GetCommand( args );
		}
		else if (className == "class ObjectTypeSymbol")
		{
			args.pop_front();
			return pSymbol->GetCommand( args );
		}
		else if (className == "class AeroParamSymbol")
		{
			args.pop_front();
			return pSymbol->GetCommand( args );
		}
	}

	// handle system variables
	else {
    		ConvertStringToUpper(identifier);
		if (identifier == "SIM_TIME" )
		{
			char buffer[32];
			float simTime = g_pSimTime->getSimTime();
		    sprintf(buffer, "SimTime %.3f]", simTime);
	
			return buffer;
		}
	}

   	return "Unknown Add command " + identifier;


}

// SET identifier ...
string SetCommand(deque<string> & args)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "SetCommand " );

    string identifier = args[0];
	BaseSymbol * pSymbol = GlobalSymbolTable.GetSymbol(identifier);
	if (pSymbol)
	{
		args.pop_front();
		return pSymbol->SetCommand( args );

	}

	// Handle system variables below

    ConvertStringToUpper(identifier);
    // set wireframe on|off
    if (identifier == "WIREFRAME")
    {
        if ( args.size() != 2)
        {
            return "Usage: SET WIREFRAME ON|OFF";
        }

        string argString = args[1];
        ConvertStringToUpper(argString);
        if (argString == "ON")
        {
            g_pBattlefield->setWireframeMode(true);
        }
        else
        {
            g_pBattlefield->setWireframeMode(false);
        }
        return "OK";
     }
      // set fog on|off
     else if (identifier == "FOG")
     {
        if ( args.size() != 2)
        {
            return "Usage: SET FOG ON|OFF";
        }

        string argString = args[1];
        ConvertStringToUpper(argString);
         if (argString == "ON")
         {
            g_pBattlefield->setFogMode(true);
         }
         else
         {
            g_pBattlefield->setFogMode(false);
         }
         return "OK";
      }
      // set fog_start value
      else if (identifier == "FOG_START")
      {   
          if ( args.size() != 2)
          {
            return "Usage: SET FOG_START VALUE";
          }

          string argString = args[1];
          float value = atof( argString.c_str() );
          g_pBattlefield->setFogStart(value);
          return "OK";
      }
      // SET FOG_END VALUE
      else if (identifier == "FOG_END")
      {
           if ( args.size() != 2)
           {
               return "Usage: SET FOG_END VALUE";
           }

           string argString = args[1];
           float value = atof( argString.c_str() );
           g_pBattlefield->setFogEnd(value);
           return "OK";
      }
      // SET VIEW_DISTANCE VALUE
      else if (identifier == "VIEW_DISTANCE")
      {
           if ( args.size() != 2)
           {
               return "Usage: SET VIEW_DISTANCE VALUE";
           }

           string argString = args[1];
           float value = atof( argString.c_str() );
           g_pBattlefield->setViewDistance(value);
           return "OK";

      }



	  return "Unknown SET command " + identifier;

}

string QuitCommand(deque<string> & args)
{
	return "Quit Command Not Yet Implemented.";
}

