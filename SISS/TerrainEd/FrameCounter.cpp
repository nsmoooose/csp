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

unsigned int CFrameCounter::GetFrameCount()
{
	// Return the last framecount
	return m_iFrameCount;
}

void CFrameCounter::StartFrame()
{
	// Begin a new frame
	m_dwLastSecond = timeGetTime();
}

void CFrameCounter::EndFrame()
{
	// Watch division by zero. This occurs when the
	// FPS exceeds 1000, this really happens ;-)

	DWORD dwElapsedTime;

	// Get the time and correct zero values
	dwElapsedTime = timeGetTime() - m_dwLastSecond;

	if (dwElapsedTime == 0)
		dwElapsedTime = 1;

	// Finish the frame and save the averaged frame count
	m_iFrameCount += 1000 / dwElapsedTime;
	m_iFrameCount /= 2;
}
