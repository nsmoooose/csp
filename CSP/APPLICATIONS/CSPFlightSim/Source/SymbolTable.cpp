#include "stdinc.h"


#include "SymbolTable.h"

SymbolTable::SymbolTable()
{


}

/**
  bool SymbolTable::AddSymbol(std::string identifier identifier, BaseSymbol)

  Add a new symbol to this symbol table. 
  If successful return true.
  The symbol must not already be in the table.
  */
bool SymbolTable::AddSymbol(std::string identifier, BaseSymbol * symbol)
{
	std::map<std::string, BaseSymbol *>::iterator pos;
	pos = find(identifier);

	// if the identifier already exists return false
	if (pos != end() )
	{
		return false;

	}
	else
	{
		insert( std::pair<std::string, BaseSymbol *>(identifier, symbol) );
		return true;
	}
}

/**
  BaseSymbol * SymbolTable::GetSymbol(std::string identifier)

  Returns the symbol associated with the identifier or null if
  it does not exist.
  */
BaseSymbol * SymbolTable::GetSymbol(std::string identifier)
{
	std::map<std::string, BaseSymbol *>::iterator pos;
	pos = find(identifier);
	if (pos == end() )
		return NULL;

	BaseSymbol * pSymbol = pos->second;

	return pSymbol;
}

void SymbolTable::dump()
{
	std::map<std::string, BaseSymbol *>::iterator pos;
	CSP_LOG(CSP_APP, CSP_INFO, "Dumping Symbol Table");
	for (pos = begin(); pos != end(); ++pos)
	{
		BaseSymbol * pSymbol = pos->second;
		CSP_LOG(CSP_APP, CSP_INFO, "Symbol Identifier: " << pos->first 
			<< ", Symbol Info: " << *pSymbol) ;
	}
	CSP_LOG(CSP_APP, CSP_INFO, "Finished Dumbing Symbol Table");
}
