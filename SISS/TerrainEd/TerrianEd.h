// TerrianEd.h : main header file for the TERRIANED application
//

#if !defined(AFX_TERRIANED_H__8AF3C9B3_5DCD_4033_87CB_52E667C01D5D__INCLUDED_)
#define AFX_TERRIANED_H__8AF3C9B3_5DCD_4033_87CB_52E667C01D5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTerrianEdApp:
// See TerrianEd.cpp for the implementation of this class
//

class CTerrianEdApp : public CWinApp
{
public:
	CTerrianEdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrianEdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTerrianEdApp)
	afx_msg void OnAppAbout();
	afx_msg void OnViewRenderparams();
	afx_msg void OnViewMode();
	afx_msg void OnViewCamerapos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRIANED_H__8AF3C9B3_5DCD_4033_87CB_52E667C01D5D__INCLUDED_)
