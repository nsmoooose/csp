///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header of Class CDlgProcess
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/29/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(DLG_PROCESS_H)
#define DLG_PROCESS_H

#include <windows.h>
#include <assert.h>
#include <commdlg.h>
#include "resource.h"
#include "BaseCode.h"

class CDlgProcess
{
public:

	static friend LRESULT CALLBACK GlobalDlgProcessProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static CDlgProcess *pDlgProcess = 0;

		// Save the pointer to CProcessDialog
		if (message == WM_INITDIALOG)
		{
			pDlgProcess = (CDlgProcess *) lParam;
			assert(!IsBadReadPtr(pDlgProcess, sizeof(CDlgProcess)));
		}
		return pDlgProcess->DlgProcessProc(hWnd, message, wParam, lParam);
	};

	CDlgProcess();
	~CDlgProcess();
	QueryParameters();

	char m_sSourceFile[MAX_STR_LENGTH];
	char m_sDestinationFile[MAX_STR_LENGTH]; 
	
protected:
	BOOL DlgProcessProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL BrowseSource(HWND hDlg);
	BOOL BrowseDestination(HWND hDlg);
	void InitDialog(HWND hDlg);
	BOOL BuildTerrain(HWND hDlg);

	int iQuadWidth, iIterations, iSize;
};

#endif

