#ifndef __OBJECTTYPESYMBOL_H__
#define __OBJECTTYPESYMBOL_H__

#include "BaseSymbol.h"

class ObjectTypeSymbol : public BaseSymbol
{
public:
	ObjectTypeSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args);
	virtual std::string GetCommand(std::deque<std::string> & args);

	friend std::ostream & operator << (std::ostream & os, const ObjectTypeSymbol& sym);


protected:

};

#endif
