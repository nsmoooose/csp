#include <BaseScreen.h>

BaseScreen::BaseScreen(): m_Interface(0) 
{
}

BaseScreen::~BaseScreen() 
{
}

VirtualHID *BaseScreen::getInterface()
{
	return m_Interface;
}

