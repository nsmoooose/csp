#include "stdinc.h"

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