// SwapInterval.h: Interface of the class CSwapInterval.
//
//////////////////////////////////////////////////////////////////////
//
// code taken from the xterminate-engine by Tim C. Schröder
// http://glvelocity.gamedev.net/
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_SWAPINTERVAL_H__A3808E8F_173C_4F12_93E7_404AFE010DC8__INCLUDED_)
#define AFX_SWAPINTERVAL_H__A3808E8F_173C_4F12_93E7_404AFE010DC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include <GL/gl.h>

#include "glext.h"
//#include "my_glext.h"
#include "WGLEXT.H"

#define ENABLE_VSYNC 1
#define DISABLE_VSYNC 0

class CSwapInterval  
{
public:
	void SetInterval(int iInterval);
	bool InitSwapIntervalExt();
	CSwapInterval();
	virtual ~CSwapInterval();

protected:
	static PFNWGLSWAPINTERVALEXTPROC m_pfnwglSwapIntervalEXT;
	static PFNWGLGETSWAPINTERVALEXTPROC m_pfnwglGetSwapIntervalEXT;
};

#endif // !defined(AFX_SWAPINTERVAL_H__A3808E8F_173C_4F12_93E7_404AFE010DC8__INCLUDED_)
