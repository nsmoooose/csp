#include "BaseObject.h"
#include "InstantActionManager.h"
#include "BaseController.h"
#include "iostream"
#include "VirtualBattlefield.h"

extern VirtualBattlefield * g_pBattlefield;

InstantActionManager::InstantActionManager()
{
  m_iObjectType = INSTACTMAN_OBJECT_TYPE;
  m_iObjectID = g_pBattlefield->getNewObjectID();
  m_iControllerID = 0;
  m_sObjectName = "INSTANTACTIONMANAGER";
  m_bDeleteFlag = false;
  m_pController = NULL;

  p_PlayerPlane = NULL;

  m_planeRange=15000;
  m_tankRange=10000;

  m_fBoundingSphereRadius = 0.001;
}

InstantActionManager::~InstantActionManager()
{

}

void InstantActionManager::dump()
{

}

void InstantActionManager::OnUpdate(float dt)
{
  if (m_pController)
    m_pController->OnUpdate(dt);

}

void InstantActionManager::Initialize()
{

}

void InstantActionManager::SetNamedParameter(const char * name, const char * value)
{

}

void InstantActionManager::SetPlayerPlane(AirplaneObject * pPlane)
{
   p_PlayerPlane = pPlane;
}
