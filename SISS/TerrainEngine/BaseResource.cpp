///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CBaseResource
//   an base class for all resources handled by the resource manager
//
//	 Portions Copyright (C) James Boer, 2000 (from "Game Programming Gems")
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	7/3/2001	implemented CBaseResource - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include "BaseResource.h"


CBaseResource::CBaseResource()
{
	Clear();
}

CBaseResource::~CBaseResource()
{
	Destroy();
}

bool CBaseResource::Create()
{
	return false;
}

void CBaseResource::Destroy()
{
}

void CBaseResource::Clear()
{
	m_Priority = RES_MED_PRIORITY;
	m_uiRefCount = 0;
	m_tLastAccess = 0;
}

bool CBaseResource::operator < (CBaseResource& oContainer)
{
	if(GetPriority() < oContainer.GetPriority())
		return true;
	else if(GetPriority() > oContainer.GetPriority())
		return false;
	else
	{
		if(m_tLastAccess < oContainer.GetLastAccess())
			return true;
		else if(m_tLastAccess > oContainer.GetLastAccess())
			return false;
		else
		{
			if(GetSize() < oContainer.GetSize())
				return true;
			else
				return false;
		}
	}
	return false;
}
