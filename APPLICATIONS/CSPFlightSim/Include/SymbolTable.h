#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "BaseSymbol.h"

class SymbolTable : public std::map<std::string, BaseSymbol *> 
{
public:
	SymbolTable();
	bool AddSymbol( std::string identifier, BaseSymbol * symbol );
	BaseSymbol * GetSymbol( std::string identifier );
	
	void dump();

protected:
//	std::map<std::string, BaseSymbol *> m_SymbolTable;

};

#endif