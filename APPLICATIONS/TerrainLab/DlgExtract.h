///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header of Class CDlgExtract
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	10/03/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(DLG_EXTRACT_H)
#define DLG_EXTRACT_H

#include <windows.h>
#include <assert.h>
#include <commdlg.h>
#include "resource.h"
#include "BaseCode.h"

class CDlgExtract
{
public:

	static friend LRESULT CALLBACK GlobalDlgExtractProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static CDlgExtract *pDlgExtract = 0;

		// Save the pointer 
		if (message == WM_INITDIALOG)
		{
			pDlgExtract= (CDlgExtract*) lParam;
			assert(!IsBadReadPtr(pDlgExtract, sizeof(CDlgExtract)));
		}
		return pDlgExtract->DlgExtractProc(hWnd, message, wParam, lParam);
	};

	CDlgExtract();
	~CDlgExtract();
	QueryParameters();

	char m_sSourceFile[MAX_STR_LENGTH];
	char m_sDestinationFile[MAX_STR_LENGTH]; 
	
protected:
	BOOL DlgExtractProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL BrowseSource(HWND hDlg);
	BOOL BrowseDestination(HWND hDlg);
	void InitDialog(HWND hDlg);
	BOOL Extract(HWND hDlg);

	int iDestWidth, iStartX, iStartY;
};

#endif

