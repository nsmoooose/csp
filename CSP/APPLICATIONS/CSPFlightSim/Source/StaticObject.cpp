#include "stdinc.h"

#include "BaseObject.h"
#include "StaticObject.h"


StaticObject::StaticObject()
{

}


StaticObject::~StaticObject()
{

}

void StaticObject::dump()
{

}

void StaticObject::OnUpdate(double dt)
{

}

void StaticObject::initialize()
{

}

unsigned int StaticObject::OnRender()
{
    return 0;
}

void StaticObject::doMovement(double dt)
{

}

int StaticObject::updateScene()
{
	BaseObject::updateScene();
	return 0;
}
