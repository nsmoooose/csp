///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
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
	if (m_iFrameCount==0) m_dwLastSecond = timeGetTime();
}

void CFrameCounter::EndFrame()
{

	m_iFrameCount++;											// increase variable each frame

	if (m_iFrameCount>=10)										// when 10 is reached, save the averaged result
	{
		m_iFrameCount = 0;
		DWORD dwElapsedTime = timeGetTime() - m_dwLastSecond;	// calculate elapsed time

		if (dwElapsedTime==0) dwElapsedTime = 1;				// if the computer is VERY fast ... ;)

		m_dFPS = 1000.0*10.0/(double)dwElapsedTime;				// save it to be accessed via GetFrameCount
	}
	
}
