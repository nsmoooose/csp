#ifndef __TERRAINSYMBOL_H__
#define __TERRAINSYMBOL_H__


#include "BaseSymbol.h"
#include "TerrainObject.h"


/**
 * class TerrainSymbol
 *
 * @author unknown
 */
class TerrainSymbol : public BaseSymbol
{
public:
	TerrainSymbol();
	virtual ~TerrainSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args);
	virtual std::string GetCommand(std::deque<std::string> & args);

	friend std::ostream & operator << (std::ostream & os, const TerrainSymbol& sym);


protected:
	TerrainObject * m_pTerrainObject;


};

#endif
