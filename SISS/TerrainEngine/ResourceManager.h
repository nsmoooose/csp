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
//	7/6/2001	implemented CResourceManager - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(RESOURCEMANAGER_H)
#define RESOURCEMANAGER_H

#pragma warning( disable : 4786 )	// disable warning about truncated characters in debug-info

#include <ctime>
#include <map>
#include <stack>

#include "global.h"
#include "BaseResource.h"


typedef unsigned int						RHANDLE;
typedef std::map<RHANDLE, CBaseResource*>	ResMap;
typedef ResMap::iterator					ResMapItor;
typedef ResMap::value_type					ResMapPair;

const RHANDLE INVALID_RHANDLE = 0xFFFFFFFF;

class CResourceManager
{
public:
	// Helper classes for map-usage
	template <class T> class pLess
	{
	public:
		inline bool operator()(T left, T right)
		{ return ((*left)< (*right)); }
	};

	template <class T> class pGreater
	{
	public:
		inline bool operator ()(T left, T right)
		{ return ((*left) < (*right)); }
	};

public:
	CResourceManager();
	virtual ~CResourceManager();

	void Clear();

	bool Create(uint uiMaxSize);
	void Destroy();
	
	inline void GotoBegin();
	inline CBaseResource* GetCurrentResource();
	inline bool GotoNext();
	inline bool IsValid();
	
	bool ReserveMemory(uint uiMem);

	bool InsertResource(RHANDLE* hUniqueID, CBaseResource* pResource);
	bool InsertResource(RHANDLE hUniqueID, CBaseResource* pResource);

	bool RemoveResource(CBaseResource* pResource);
	bool RemoveResource(RHANDLE hUniqueID);

	bool DestroyResource(CBaseResource* pResource);
	bool DestroyResource(RHANDLE hUniqueID);

	CBaseResource* GetResource(RHANDLE hUniqueID);

	CBaseResource* Lock(RHANDLE hUniqueID);
	int Unlock(RHANDLE hUniqueID);
	int Unlock(CBaseResource* pResource);

	RHANDLE FindResourceHandle(CBaseResource* pResource);

protected:
	inline void AddMemory(uint uiMem);
	inline void RemoveMemory(uint uiMem);
	uint GetNextResHandle();

	bool CheckForOverallocation();

	RHANDLE		m_hNextResHandle;
	uint		m_uiCurrentUsedMemory;
	uint		m_uiMaximumMemory;
	ResMapItor	m_CurrentResource;
	ResMap		m_ResourceMap;
	bool		m_bResourceReserved;
};

extern CResourceManager ResManager;

#endif
