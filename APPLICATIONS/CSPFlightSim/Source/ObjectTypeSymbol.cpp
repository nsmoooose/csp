#include "stdinc.h"

#include "ObjectTypeSymbol.h"
#include "ObjectFactory.h"
#include "global.h"

extern ObjectFactory * g_pObjectFactory;

ObjectTypeSymbol::ObjectTypeSymbol()
{


}

std::string ObjectTypeSymbol::SetCommand(std::deque<std::string> & args)
{
	std::string propString = args[0];
	
       	ConvertStringToUpper(propString);
	if (propString == "MODEL")
	{
		std::string valueString = args[1];
		g_pObjectFactory->setObjectTypeModel( m_Identifier, valueString );
	}
	else if (propString == "AERO")
	{
		std::string valueString = args[1];
		g_pObjectFactory->setObjectTypeAero( m_Identifier, valueString );	
	}
	return "OK";
}

std::string ObjectTypeSymbol::GetCommand(std::deque<std::string> & args)
{
	return "Not Yet Implemented:";
}

std::ostream & operator << (std::ostream & os, const ObjectTypeSymbol& sym)
{
	os << "SymbolClass: " << typeid(sym).name();
	return os;

}

