#ifndef __OBJECTSYMBOL_H__
#define __OBJECTSYMBOL_H__

#include "BaseSymbol.h"

class ObjectSymbol : public BaseSymbol
{
public:
	ObjectSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args);
	virtual std::string GetCommand(std::deque<std::string> & args);

	friend std::ostream & operator << (std::ostream & os, const ObjectSymbol& sym);


protected:

};

#endif
