///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Implementation of Class CDlgExtract
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	10/03/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include "DlgExtract.h"
#include "TerrainData.h"
#include "Terrain.h"
#include "resource.h"
#include "math.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

extern CTerrainData *m_pTerrainData;
extern CTerrain		*m_pTerrain;

CDlgExtract::CDlgExtract()
{
}

CDlgExtract::~CDlgExtract()
{
}

CDlgExtract::Extract(HWND hDlg)
{
	int success;
	long lsize, iFile;
	unsigned short *buffer;
	float fwidth;
	unsigned short *aiHeightmap, *dest_map;

	int iWidth, iHeight;

	// Load RAW data and save to internal structure
	iFile = _open(m_sSourceFile, _O_BINARY);
	if(iFile==-1)
	{	
		MessageBox(hDlg, "Unable to open source file", "File error", MB_OK);
		return FALSE;
	}

	lsize = _filelength(iFile);
	lsize /= 2;									// RAW uses 16bit height values, so we have to divide by 2
												// to get the real number of height map values
	fwidth = (float)sqrt((double)lsize);		// compute terrain width from size of file
	iWidth = (int)fwidth;						
	iHeight = (int)fwidth;						

	aiHeightmap = new unsigned short[lsize];
	buffer = new unsigned short[lsize];
	dest_map = new unsigned short[iDestWidth * iDestWidth];

	success = _read(iFile, buffer, (lsize*sizeof(unsigned short)));

	for (int y = iStartY; y<iStartY + iDestWidth; y++)					
		for (int x = iStartX; x<iStartX + iDestWidth; x++)
		{
			dest_map[(y-iStartY) * iDestWidth + x - iStartX] =  (unsigned short)buffer[y*iHeight+x];
		} 

	_close(iFile);
	delete []buffer;

	// Write the destination file
	iFile = _open(m_sDestinationFile, _O_CREAT | _O_RDWR | _O_BINARY);
	if(iFile==-1)
	{	
		MessageBox(hDlg, "Unable to open destination file", "File error", MB_OK);
		return FALSE;
	}

	success = _write(iFile, dest_map, iDestWidth * iDestWidth * sizeof(unsigned short));
	_chmod(m_sDestinationFile, _S_IREAD | _S_IWRITE);
	_close(iFile);

	return TRUE;
}

BOOL CDlgExtract::BrowseSource(HWND hDlg)
{
	BOOL success;
	static char szFilter[] = TEXT("RAW - 16bit RAW file\0*.raw\0\0");
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
		strcpy(m_sSourceFile, ofn.lpstrFile);
		SetDlgItemText(hDlg, IDC_SRC_FILENAME, ofn.lpstrFile);
		return true;
	}
	else
	{
		m_sSourceFile[0] = FALSE;
		return false;
	}
}

BOOL CDlgExtract::BrowseDestination(HWND hDlg)
{
	BOOL success;
	static char szFilter[] = TEXT("RAW - 16bit RAW file\0*.raw\0\0");
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
	if(success = GetSaveFileName(&ofn))
	{
		strlwr(ofn.lpstrFile);
		strcpy(m_sDestinationFile, ofn.lpstrFile);
		SetDlgItemText(hDlg, IDC_DEST_FILENAME, ofn.lpstrFile);
		return true;
	}
	else
	{
		m_sDestinationFile[0] = FALSE;
		return false;
	}
}

void CDlgExtract::InitDialog(HWND hDlg)
{
	SetDlgItemText(hDlg, IDC_ITERATIONS, "5");
	m_sSourceFile[0] = FALSE;
	m_sDestinationFile[0] = FALSE;
}

BOOL CDlgExtract::QueryParameters()
{
	HINSTANCE hInst = (HINSTANCE) GetWindowLong(GetActiveWindow(), GWL_HINSTANCE);

	// Invoke the dialog box
	if (DialogBoxParam(hInst, (LPCTSTR) IDD_DIALOG_EXTRACT, GetDesktopWindow(), 
		(DLGPROC) GlobalDlgExtractProc, (LPARAM) this))
		return true;
	else 
		return false;	
}

BOOL CDlgExtract::DlgExtractProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static char tbuffer[256];

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
			Extract(hDlg);
			EndDialog(hDlg, 1);
			return true;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return false;

		case IDC_SETX:
			GetDlgItemText(hDlg, IDC_SETX, tbuffer, 255);
			iStartX = atoi(tbuffer);
			return true;

		case IDC_SETY:
			GetDlgItemText(hDlg, IDC_SETY, tbuffer, 255);
			iStartY = atoi(tbuffer);
			return true;

		case IDC_DESTWIDTH:
			GetDlgItemText(hDlg, IDC_DESTWIDTH, tbuffer, 255);
			iDestWidth = atoi(tbuffer);
			return true;

		case ID_BROWSE_SRC:
			BrowseSource(hDlg);
			return true;

		case ID_BROWSE_DEST:
			BrowseDestination(hDlg);
			return true;

		}

	break;
	}
	return false;
}


