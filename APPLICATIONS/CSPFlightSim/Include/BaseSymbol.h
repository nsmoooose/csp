#ifndef __BASESYMBOL_H__
#define __BASESYMBOL_H__


/**
 * class BaseSymbol - Describe me!
 *
 * @author unknown
 */
class BaseSymbol
{
public:
	BaseSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args) = 0;
	virtual std::string GetCommand(std::deque<std::string> & args) = 0;

	friend std::ostream & operator << (std::ostream & os, const BaseSymbol& sym);

	void setIdentifier(std::string _Identifier);

protected:
	std::string m_Identifier;
};




#endif