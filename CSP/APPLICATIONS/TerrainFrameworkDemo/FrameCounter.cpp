///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of class CFrameCounter
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


#ifdef WIN32
#include "stdafx.h"
#endif

#include "FrameCounter.h"


CFrameCounter::CFrameCounter()
{
	// Init member variables
	m_iFrameCount  = 0;
}

CFrameCounter::~CFrameCounter()
{

}

double CFrameCounter::GetFrameCount()
{
	// Return the last framecount
	return m_dFPS;
}

void CFrameCounter::StartFrame()
{
	// Begin a new frame
#ifdef WIN32
	if (m_iFrameCount==0) m_dwLastSecond = timeGetTime();
#else
	if (m_iFrameCount==0)
	  {
	    struct timeval time_val;
	    gettimeofday(&time_val, 0);
	    m_dwLastSecond = time_val.tv_usec;
	  }
	
#endif

}

void CFrameCounter::EndFrame()
{

	m_iFrameCount++;											// increase variable each frame

	if (m_iFrameCount>=10)										// when 10 is reached, save the averaged result
	{
		m_iFrameCount = 0;
#ifdef WIN32
		DWORD dwElapsedTime = timeGetTime() - m_dwLastSecond;	// calculate elapsed time
#else
		struct timeval time_val;
		gettimeofday(&time_val, 0);
		long dwElapsedTime = time_val.tv_usec - m_dwLastSecond;	// calculate elapsed time
#endif

		if (dwElapsedTime==0) dwElapsedTime = 1;				// if the computer is VERY fast ... ;)

		m_dFPS = 1000.0*10.0/(double)dwElapsedTime;				// save it to be accessed via GetFrameCount
	}
	
}
