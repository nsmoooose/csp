///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CFrameCounter
//   presents a small "framecounter"
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	- 7/30/2000 included documentation - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(FRAME_COUNTER_H)
#define FRAME_COUNTER_H

#ifdef WIN32
#include "mmsystem.h"
#else
#include <sys/time.h>
#endif

class CFrameCounter  
{
public:
	// called when framerendering ends
	void EndFrame();
	// called when framerendering starts
	void StartFrame();
	// Return the framecount
	double GetFrameCount();
	CFrameCounter();
	virtual ~CFrameCounter();

private:
	unsigned int m_iFrameCount;
#ifdef WIN32	
	DWORD m_dwLastSecond;
#else
	long m_dwLastSecond;
#endif
	double m_dFPS;
};

#endif 
