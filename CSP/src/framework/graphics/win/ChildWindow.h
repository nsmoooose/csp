#ifndef __CHILDWINDOW_H_
#define __CHILDWINDOW_H_
#include "..\StandardGraphics.h"

class ChildWindow
{
public:
	ChildWindow();
	ChildWindow(StandardGraphics * );

protected:
	StandardGraphics * m_pParent;
};

#endif