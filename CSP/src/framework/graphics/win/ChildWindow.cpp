#include "ChildWindow.h"
#include "..\StandardGraphics.h"

ChildWindow::ChildWindow()
{
	m_pParent = NULL;
}

ChildWindow::ChildWindow(StandardGraphics * parent)
{
	m_pParent = parent;

}