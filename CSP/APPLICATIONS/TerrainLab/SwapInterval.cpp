// SwapInterval.cpp: Interface of the class CSwapInterval.
//
//////////////////////////////////////////////////////////////////////
//
// code taken from the xterminate-engine by Tim C. Schröder
// http://glvelocity.gamedev.net/
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SwapInterval.h"

// Static member variables
PFNWGLSWAPINTERVALEXTPROC CSwapInterval::m_pfnwglSwapIntervalEXT = 0;
PFNWGLGETSWAPINTERVALEXTPROC CSwapInterval::m_pfnwglGetSwapIntervalEXT = 0;


//////////////////////////////////////////////////////////////////////
// Construction / destruction
//////////////////////////////////////////////////////////////////////

CSwapInterval::CSwapInterval()
{

}

CSwapInterval::~CSwapInterval()
{

}

bool CSwapInterval::InitSwapIntervalExt()
{
	////////////////////////////////////////////////////////////////////////
	// Get the extension function pointers
	////////////////////////////////////////////////////////////////////////

	// Get the function pointers
	m_pfnwglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) 
		wglGetProcAddress("wglSwapIntervalEXT");
	m_pfnwglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) 
		wglGetProcAddress("wglGetSwapIntervalEXT");

	// Success ?
	if (m_pfnwglSwapIntervalEXT && m_pfnwglGetSwapIntervalEXT)
		return true;
	else
		return false;
}

void CSwapInterval::SetInterval(int iInterval)
{
	////////////////////////////////////////////////////////////////////////
	// Set the interval
	////////////////////////////////////////////////////////////////////////

	if (!m_pfnwglSwapIntervalEXT)
	{
		assert(m_pfnwglSwapIntervalEXT);
		return;
	}

	
	(* m_pfnwglSwapIntervalEXT) (iInterval);
}
