// MainManager.h: interface for the MainManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINMANAGER_H__A7D0BC73_EF86_4E03_A884_0836AAAA99B5__INCLUDED_)
#define AFX_MAINMANAGER_H__A7D0BC73_EF86_4E03_A884_0836AAAA99B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Task.h>

//#include <RenderKernel.h>
#include <SpriteManager.h>
#include <SpriteInstance.h>
#include <ResourceDatabase.h>
#include <ClearLayer.h>

#include <2DTypes.h>
#include <UI/Font.h>
#include <UI/InterfaceSystem.h>
#include <UI/OSDConsole.h>
#include <UI/OSDLine.h>

#include "MainInterface.h"

using namespace Jet;


class MainManager : public Task  
{
public:
	MainManager();
	virtual bool Init(OS *os);
	virtual void Done();
	virtual UpdateCode Update(float dt);

private:
	ResourceDatabase	*database;
	ClearLayer			*clearLayer;
	SpriteManager		*spriteManager;
	SpriteInstance		*splashScreen;
	MainInterface		*mainInterface;
};

#endif // !defined(AFX_MAINMANAGER_H__A7D0BC73_EF86_4E03_A884_0836AAAA99B5__INCLUDED_)
