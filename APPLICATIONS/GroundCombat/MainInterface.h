// MainInterface.h: interface for the MainInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAININTERFACE_H__EFBE8C70_8B98_4EF7_893D_8C3A46656FE8__INCLUDED_)
#define AFX_MAININTERFACE_H__EFBE8C70_8B98_4EF7_893D_8C3A46656FE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OS.h>
#include <Task.h>

#include <ResourceDatabase.h>
#include <ClearLayer.h>
#include <InputObserverSystem.h>
#include <UI/InterfaceSystem.h>
#include <UI/Dialog.h>

using namespace Jet;
using namespace UI;

class MainInterface : public Task  
{
public:
	MainInterface();
	virtual bool Init(OS *os);
	virtual void Done();
	virtual UpdateCode Update(float dt);
	virtual void Suspend() {};

private:
	void OnToggleFullscreen();
	ResourceDatabase	*database;
	InterfaceSystem		*interfaceSystem;
	Dialog				*inputForm;
	bool updating;
	bool ready;

	void Setup(const char *controlName, const char *callbackName, const CallBack &callback);
	void OnQuit();
};

#endif // !defined(AFX_MAININTERFACE_H__EFBE8C70_8B98_4EF7_893D_8C3A46656FE8__INCLUDED_)
