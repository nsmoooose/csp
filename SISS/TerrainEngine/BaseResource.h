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
//	7/2/2001	implemented CBaseResource  - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(BASERESOURCE_H)
#define BASERESOURCE_H

#include <time.h>

#include "global.h"


class CBaseResource
{
public:
	enum PriorityType
	{
		RES_LOW_PRIORITY = 0,
		RES_MED_PRIORITY,
		RES_HIGH_PRIORITY
	};

	CBaseResource();
	virtual ~CBaseResource();

	/// Clears class data
	virtual void Clear();

	// Create and destroy functions.  Note that the Create() function of the
	// derived class does not have to exactly match the base class.  No assumptions
	// are made regarding parameters.
	virtual bool Create();
	virtual void Destroy();

	// Dispose and recreate must be able to discard and then completely recreate
	// the data contained in the class with no additional parameters
	virtual bool Recreate() = 0;
	virtual void Dispose() = 0;

	// GetSize() must return the size of the data inside the class, and IsDisposed()
	// lets the manager know if the data exists.
	virtual size_t GetSize() = 0;
	virtual bool IsDisposed() = 0;

	// These functions set the parameters by which the sorting operator determines
	// in what order resources are discarded
	inline void SetPriority(PriorityType priority) { m_Priority = priority; }
	inline PriorityType GetPriority() { return m_Priority; }

	inline void SetReferenceCount(uint uiCount) { m_uiRefCount = uiCount; }
	inline uint GetReferenceCount() { return m_uiRefCount; }

	inline bool IsLocked() { return (m_uiRefCount > 0); }
	
	inline void SetLastAccessTime(time_t tLastAccess) { m_tLastAccess = tLastAccess; }
	inline time_t GetLastAccess() { return m_tLastAccess; }

	virtual bool operator < (CBaseResource& oContainer);

protected:
	PriorityType	m_Priority;
	uint			m_uiRefCount;
	time_t			m_tLastAccess;
};

#endif
