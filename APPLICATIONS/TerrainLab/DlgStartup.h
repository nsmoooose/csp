///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header of Class CDlgStartup  
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(DLG_STARTUP_H)
#define DLG_STARTUP_H

#include <windows.h>
#include <assert.h>
#include <commdlg.h>
#include "resource.h"

class CDlgStartup
{
public:

	static friend LRESULT CALLBACK GlobalDlgStartupProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static CDlgStartup *pDlgStartup = 0;

		// Save the pointer to CSetupDialog
		if (message == WM_INITDIALOG)
		{
			pDlgStartup = (CDlgStartup *) lParam;
			assert(!IsBadReadPtr(pDlgStartup, sizeof(CDlgStartup)));
		}
		return pDlgStartup->DlgStartupProc(hWnd, message, wParam, lParam);
	};

	CDlgStartup();
	~CDlgStartup();
	QueryParameters();
	
protected:
	BOOL DlgStartupProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL BrowseFile(HWND hDlg);
	void InitDialog(HWND hDlg);
	void SaveSettings(HWND hDlg);
};

#endif

