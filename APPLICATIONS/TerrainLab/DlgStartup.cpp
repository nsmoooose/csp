///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Implementation of Class CDlgStartup  
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include "DlgStartup.h"
#include "TerrainData.h"
#include "SwapInterval.h"
#include "BaseCode.h"
#include "resource.h"

extern CTerrainData *m_pTerrainData;
extern int iMode;

CDlgStartup::CDlgStartup()
{
}

CDlgStartup::~CDlgStartup()
{
}

BOOL CDlgStartup::BrowseFile(HWND hDlg)
{
	BOOL success;
	static char szFilter[] = TEXT("16bit RAW file\0*.raw\0\0");
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static OPENFILENAME ofn;

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hDlg;
	ofn.hInstance		= 0;
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter	= 0;
	ofn.nFilterIndex	= 0;
	ofn.lpstrFile		= szFileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrFileTitle	= szTitleName;
	ofn.nMaxFileTitle	= MAX_PATH;
	ofn.lpstrInitialDir	= "Data";
	ofn.lpstrTitle		= NULL;
	ofn.Flags			= OFN_CREATEPROMPT;
	ofn.nFileOffset		= 0;
	ofn.nFileExtension	= 0;
	ofn.lpstrDefExt		= TEXT("raw");
	ofn.lCustData		= 0L;
	ofn.lpfnHook		= NULL;
	ofn.lpTemplateName	= NULL;

	strcpy(ofn.lpstrFile,"\0");
	if(success = GetOpenFileName(&ofn))
	{
		strlwr(ofn.lpstrFile);
		strcpy(m_pTerrainData->m_sMeshFileName, ofn.lpstrFile);
		SetDlgItemText(hDlg, IDC_EDIT_FILENAME, ofn.lpstrFile);
		return true;
	}
	else
		return false;
}

void CDlgStartup::InitDialog(HWND hDlg)
{
	SetDlgItemText(hDlg, IDC_GRID_DIST, "500");
	SetDlgItemText(hDlg, IDC_DELTAZ, "1.0");
	SendDlgItemMessage(hDlg, IDC_SET_VSYNC, BM_SETCHECK, 1, 0);			
	SendDlgItemMessage(hDlg, IDC_LOADDEFMESH, BM_SETCHECK, 1, 0);			
}

BOOL CDlgStartup::QueryParameters()
{
	HINSTANCE hInst = (HINSTANCE) GetWindowLong(GetActiveWindow(), GWL_HINSTANCE);

	// Invoke the dialog box
	if (DialogBoxParam(hInst, (LPCTSTR) IDD_DIALOG_STARTUP, GetDesktopWindow(), 
		(DLGPROC) GlobalDlgStartupProc, (LPARAM) this))
		return true;
	else 
		return false;	
}

void CDlgStartup::SaveSettings(HWND hDlg)
{
	char textbuffer[256], extension[10];

	if(SendDlgItemMessage(hDlg, IDC_LOADDEFMESH, BM_GETCHECK, 0, 0))
	{
		m_pTerrainData->m_bStartupAction = DLG_LOAD_DEFAULT;
		iMode = MODE_FRACTAL;
	}

	m_pTerrainData->m_iMeshGridDistance = GetDlgItemInt(hDlg, IDC_GRID_DIST, NULL, TRUE);
	m_pTerrainData->m_iMeshGridDistanceBasis = m_pTerrainData->m_iMeshGridDistance;

	GetDlgItemText(hDlg, IDC_DELTAZ, textbuffer, 255);
	m_pTerrainData->m_fDeltaZ = (float)atof(textbuffer);
	
	if(SendDlgItemMessage(hDlg, IDC_LOADNEWMESH, BM_GETCHECK, 0, 0))
	{
		m_pTerrainData->m_bStartupAction = DLG_LOAD_NEW;

		// get the extension of the filename and convert it to lower case
		strcpy(extension, m_pTerrainData->m_sMeshFileName+strlen(m_pTerrainData->m_sMeshFileName)-3);
		strlwr(extension);

/*		if (strcmp(extension, "scn") == 0)
		{
			// scn file found!
			m_pTerrainData->m_bMapType = MAP_TYPE_SCN;
		} */
		if (strcmp(extension, "raw") == 0)
		{
			// scn file found!
			iMode = MODE_FRACTAL;
		}
	}	

	if(SendDlgItemMessage(hDlg, IDC_SET_VSYNC, BM_GETCHECK, 0, 0))				
		m_pTerrainData->m_bActivateVSync = ENABLE_VSYNC;
	else
		m_pTerrainData->m_bActivateVSync = DISABLE_VSYNC;

	return;
}

BOOL CDlgStartup::DlgStartupProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	/////////////////////////////////////////////////////
	// Message handling for the Startup Dialog
	/////////////////////////////////////////////////////
	switch(message)
	{
	case WM_INITDIALOG:
		InitDialog(hDlg);
		return true;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			SaveSettings(hDlg);
			EndDialog(hDlg, 1);
			return true;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return false;

		case IDBROWSE:
			BrowseFile(hDlg);
			SendDlgItemMessage(hDlg, IDC_LOADNEWMESH, BM_SETCHECK, 1, 0);
			return true;
		}

	break;
	}
	return false;
}


