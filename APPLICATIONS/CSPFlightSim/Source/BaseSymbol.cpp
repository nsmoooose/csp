#include "stdinc.h"

#ifdef _WIN32
#include <typeinfo.h>
#endif
#include <typeinfo>

#include "BaseSymbol.h"

BaseSymbol::BaseSymbol()
{


}

void BaseSymbol::setIdentifier(std::string _Identifier) 
{ 
	m_Identifier = _Identifier; 
}

std::ostream & operator << (std::ostream & os, const BaseSymbol& sym)
{
	os << "SymbolClass: " << typeid ( sym ).name();
	return os;

}
