///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CResourceManager
//   describes an resource manager for all types of data which can be
//	 loaded and deleted dynamically
//
//	 Portions Copyright (C) James Boer, 2000 (from "Game Programming Gems")
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	7/12/2001	implemented CResourceManager - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include <queue>

#include "ResourceManager.h"

using namespace std;



CResourceManager ResManager;		// for global access



CResourceManager::CResourceManager()
{
	Clear();
}

CResourceManager::~CResourceManager()
{
	Destroy();
	return;
}

void CResourceManager::Clear()
{
	m_ResourceMap.clear();
	m_hNextResHandle = INVALID_RHANDLE;
	m_uiCurrentUsedMemory = 0;
	m_uiMaximumMemory = 0;
	m_bResourceReserved = false;
	m_CurrentResource = m_ResourceMap.end();
}

void CResourceManager::Destroy()
{
	for(ResMapItor itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
	{
		if (!itor->second->IsLocked())
		{
			delete itor->second;
		}
	}
	m_ResourceMap.clear();
	Clear();
}

void CResourceManager::GotoBegin()
{
	m_CurrentResource = m_ResourceMap.begin();
}

CBaseResource* CResourceManager::GetCurrentResource()
{
	return m_CurrentResource->second;
}

bool CResourceManager::GotoNext()
{
	m_CurrentResource++;
	return IsValid();
}

bool CResourceManager::IsValid()
{
	return (m_CurrentResource != m_ResourceMap.end());
}

bool CResourceManager::ReserveMemory(uint uiMem)
{
	AddMemory(uiMem);
	if (!CheckForOverallocation()) return false;
	m_bResourceReserved = true;
	return true;
}

bool CResourceManager::InsertResource(RHANDLE* hUniqueID, CBaseResource* pResource)
{
	// Get the next unique ID for this catalog
	*hUniqueID = GetNextResHandle();
	// Insert the resource into the current catalog's map
	m_ResourceMap.insert(ResMapPair(*hUniqueID, pResource));
	// Get the memory and add it to the catalog total.  Note that we only have
	// to check for memory overallocation if we haven't preallocated memory
	if (!m_bResourceReserved)
	{
		AddMemory(pResource->GetSize());
		// check to see if any overallocation has taken place
		if (!CheckForOverallocation()) return false;
	}
	else
		m_bResourceReserved = false;
	// return success
	return true;
}

bool CResourceManager::InsertResource(RHANDLE hUniqueID, CBaseResource* pResource)
{
	ResMapItor itor = m_ResourceMap.find(hUniqueID);
	if (itor != m_ResourceMap.end())
		// ID has already been allocated as a resource
		return false;
	// Insert the resource into the current catalog's map
	m_ResourceMap.insert(ResMapPair(hUniqueID, pResource));
	// Get the memory and add it to the catalog total.  Note that we only have
	// to check for memory overallocation if we haven't preallocated memory
	if (!m_bResourceReserved)
	{
		AddMemory(pResource->GetSize());
		// check to see if any overallocation has taken place
		if (!CheckForOverallocation()) return false;
	}
	else
		m_bResourceReserved = false;

	// return the id to the user for their use and return success
	return true;
}

bool CResourceManager::RemoveResource(CBaseResource* pResource)
{
	// try to find the resource with the specified resource
    ResMapItor itor;

	for(itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
	{
		if (itor->second == pResource) break;
	}
	if (itor == m_ResourceMap.end())
		// Could not find resource to remove. 
		return false;
	// if the resource was found, check to see that it's not locked
	if (itor->second->IsLocked())
		// Can't remove a locked resource
		return false;
	// Get the memory and subtract it from the manager total
	RemoveMemory(pResource->GetSize());
	// remove the requested resource
	m_ResourceMap.erase(itor);

	return true;
}

bool CResourceManager::RemoveResource(RHANDLE hUniqueID)
{
	// try to find the resource with the specified id
	ResMapItor itor = m_ResourceMap.find(hUniqueID);
	if (itor == m_ResourceMap.end())
		// Could not find resource to remove
		return false;
	// if the resource was found, check to see that it's not locked
	if (((*itor).second)->IsLocked())
		// Can't remove a locked resource
		return false;
	// Get the memory and subtract it from the manager total
	RemoveMemory(((*itor).second)->GetSize());
	// remove the requested resource
	m_ResourceMap.erase(itor);

	return true;
}

bool CResourceManager::DestroyResource(CBaseResource* pResource)
{
	if (!RemoveResource(pResource)) return false;
	delete pResource;
	return true;
}

bool CResourceManager::DestroyResource(RHANDLE hUniqueID)
{
	CBaseResource* pResource = GetResource(hUniqueID);
	if (!RemoveResource(hUniqueID)) return false;
	delete pResource;
	return true;
}

CBaseResource* CResourceManager::GetResource(RHANDLE hUniqueID)
{
	ResMapItor itor = m_ResourceMap.find(hUniqueID);

	if (itor == m_ResourceMap.end()) return NULL;
	
	// you may need to add your own OS dependent method of getting
	// the current time to set your resource access time

	// set the current time as the last time the object was accessed
	itor->second->SetLastAccessTime(time(0));

	// recreate the object before giving it to the application
	if (itor->second->IsDisposed())
	{
		itor->second->Recreate();
		AddMemory(itor->second->GetSize());

		// check to see if any overallocation has taken place, but
		// make sure we don't swap out the same resource.
		Lock(hUniqueID);
		CheckForOverallocation();
		Unlock(hUniqueID);
	}

 
	// return the object pointer
	return itor->second;
}

CBaseResource* CResourceManager::Lock(RHANDLE hUniqueID)
{
	ResMapItor itor = m_ResourceMap.find(hUniqueID);
	if (itor == m_ResourceMap.end()) return NULL;
	
	// increment the object's count
	itor->second->SetReferenceCount(itor->second->GetReferenceCount() + 1);

	// recreate the object before giving it to the application
	if (itor->second->IsDisposed())
	{
		itor->second->Recreate();
		AddMemory(itor->second->GetSize());
		// check to see if any overallocation has taken place
		CheckForOverallocation();
	}

	// return the object pointer
	return itor->second;
}

int CResourceManager::Unlock(RHANDLE hUniqueID)
{
	ResMapItor itor = m_ResourceMap.find(hUniqueID);
	if (itor == m_ResourceMap.end()) return -1;
	
	// decrement the object's count
	if (itor->second->GetReferenceCount() > 0)
		itor->second->SetReferenceCount(itor->second->GetReferenceCount() - 1);

	return itor->second->GetReferenceCount();
}

int CResourceManager::Unlock(CBaseResource* pResource)
{
	RHANDLE hResource = FindResourceHandle(pResource);
	if (hResource == INVALID_RHANDLE) return -1;
	return Unlock(hResource);
}

RHANDLE CResourceManager::FindResourceHandle(CBaseResource* pResource)
{
	// try to find the resource with the specified resource
    ResMapItor itor;
	for(itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
	{
		if(itor->second == pResource) break;
	}
	if(itor == m_ResourceMap.end()) return INVALID_RHANDLE;
	return itor->first;
}

bool CResourceManager::CheckForOverallocation()
{
	if (m_uiCurrentUsedMemory > m_uiMaximumMemory)
	{
		// Attempt to remove iMemToPurge bytes from the managed resource
		int iMemToPurge = m_uiCurrentUsedMemory - m_uiMaximumMemory;

		// create a temporary priority queue to store the managed items
		priority_queue<CBaseResource*, vector<CBaseResource*>, pGreater<CBaseResource*> > PriQueue;

		// insert copies of all the resource pointers into the priority queue, but
		// exclude those that are current disposed or are locked
		for (ResMapItor itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
		{
			if(!itor->second->IsDisposed() && !itor->second->IsLocked()) PriQueue.push(itor->second);
		}

		while ((!PriQueue.empty()) && (m_uiCurrentUsedMemory > m_uiMaximumMemory))
		{
			uint uiDisposalSize = PriQueue.top()->GetSize();
			PriQueue.top()->Dispose();
			if (PriQueue.top()->IsDisposed())
				RemoveMemory(uiDisposalSize);
			PriQueue.pop();
		}

		// If the resource queue is empty and we still have too much memory allocated,
		// then we return failure.  This could happen if too many resources were locked
		// or if a resource larger than the requested maximum memory was inserted.
		if (PriQueue.empty() && (m_uiCurrentUsedMemory > m_uiMaximumMemory)) return false;
	}
	return true;
}

void CResourceManager::AddMemory(uint uiMem)
{
	m_uiCurrentUsedMemory += uiMem;  
}

void CResourceManager::RemoveMemory(uint uiMem)
{
	m_uiCurrentUsedMemory -= uiMem;
}

uint CResourceManager::GetNextResHandle()
{
	return --m_hNextResHandle;
}
