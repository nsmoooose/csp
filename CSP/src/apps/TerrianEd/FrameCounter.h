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

#if !defined(FRAME_COUNTER_H)
#define FRAME_COUNTER_H


#include "mmsystem.h"

class CFrameCounter  
{
public:
	void EndFrame();
	void StartFrame();
	// Return the framecount
	unsigned int GetFrameCount();
	CFrameCounter();
	virtual ~CFrameCounter();

private:
	unsigned int m_iFrameCount;
	DWORD m_dwLastSecond;
};

#endif 
