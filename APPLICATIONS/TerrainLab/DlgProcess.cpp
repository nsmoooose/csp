///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Implementation of Class CDlgProcess
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/29/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include "DlgProcess.h"
#include "TerrainData.h"
#include "Terrain.h"
#include "Fractal.h"
#include "resource.h"
#include "math.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

extern CTerrainData *m_pTerrainData;
extern CTerrain		*m_pTerrain;
extern CFractal		 Fractal;
BOOL	bResult;
char textbuffer[256];

CDlgProcess::CDlgProcess()
{
}

CDlgProcess::~CDlgProcess()
{
}

BOOL CDlgProcess::BuildTerrain(HWND hDlg)
{
	int i, iQuadsWide, iQuadsHigh, iTargetFileSize, iTargetQuadSize, iTargetQuadWidth, iTargetQuadHeight;
	int iQuadX, iQuadY, x, y, iFile, iQuadBase;
	int iMsgBoxRet;
	short success;
	short *sBuffer;

	// check if there are valid source and destination files
	if(!m_sDestinationFile[0] || !m_sSourceFile[0])
	{
		MessageBox(hDlg, "Invalid Source or Destination File", "Invalid Files", MB_OK);
		return FALSE;
	}

	// save the name of the old mesh file
//	m_pTerrain->LoadRAWData(m_sSourceFile);
//	m_pTerrain->LoadDefaultMesh();
	iQuadsWide = iQuadsHigh = m_pTerrainData->m_iMeshWidth-1;
	
	// do the iterations and smooth the terrain
	for(i=0; i<iIterations; i++)
	{
		m_pTerrain->Iterate(m_pTerrainData->m_fFactorD);
		m_pTerrainData->m_fFactorD *= (float)pow(2, -1);
		SetDlgItemInt(hDlg, IDC_PROC_ITERATION, i+1, 1);

	}
	Fractal.SmoothStep(1.0f);

	// quad size is ((2^iterations)+1)^2 vertices
	// the bounding rows and columns of each quad are redundant, e.g.
	// the lowermost row of quad 1,1 is the same as the uppermost row of 1,2
	iTargetQuadWidth = iTargetQuadHeight = (int)pow(2, iIterations) + 1;
	iTargetQuadSize = (int)pow((pow(2, iIterations)+1),2);
	iTargetFileSize = iQuadsHigh * iQuadsWide * iTargetQuadSize * sizeof(short);
	SetDlgItemInt(hDlg, IDC_TARGETSIZE, iTargetFileSize, 1);
	sBuffer = new short[iTargetQuadSize];

	// check if file already exists
	iFile = _open(m_sDestinationFile, _O_RDONLY);
	if(iFile != -1)
	{
		iMsgBoxRet = MessageBox(hDlg, "Destination file already exists. Overwrite?", "File exists", MB_YESNO);
		if(iMsgBoxRet == IDNO)
		{
			_close(iFile);
			return FALSE;
		}
		_close(iFile);
	}
	
	// it doesn't exist, so create it.
	iFile = _open(m_sDestinationFile, _O_CREAT | _O_RDWR | _O_BINARY);
	if(iFile==-1)
	{	
		MessageBox(hDlg, "Unable to open destination file", "File error", MB_OK);
		return FALSE;
	}

	// save the quads to the target file
	for(iQuadY=0; iQuadY<iQuadsHigh; iQuadY++)
	{
		SetDlgItemInt(hDlg, IDC_QUADY, iQuadY, 1);
		for(iQuadX=0; iQuadX<iQuadsWide; iQuadX++)
		{
			SetDlgItemInt(hDlg, IDC_QUADX, iQuadX, 1);
			iQuadBase = iQuadY * (iTargetQuadHeight-1) * m_pTerrainData->m_iMeshWidth + 
						iQuadX * (iTargetQuadWidth-1);
			
			for(y=0; y<iTargetQuadWidth; y++)
				for(x=0; x<iTargetQuadWidth; x++)
				{
					sBuffer[y*iTargetQuadWidth+x] = (short)m_pTerrainData->
									 m_pHeightmap[iQuadBase + (y*m_pTerrainData->m_iMeshWidth) + x];
				}

			success = _write(iFile, sBuffer, iTargetQuadSize * sizeof(short));
			if(success != iTargetQuadSize * sizeof(short))
			{
				MessageBox(hDlg, "Error writing mesh", "File error", MB_OK);
				_close(iFile);
				delete []sBuffer;
				return FALSE;
			}
		}
	}

	// Ready.
	_chmod(m_sDestinationFile, _S_IREAD | _S_IWRITE);
	_close(iFile);
	MessageBox(hDlg, "Terrain's now on disc!", "Done", MB_OK);
	m_pTerrain->LoadDefaultMesh();
	if (sBuffer) delete []sBuffer;
	return TRUE;
}


BOOL CDlgProcess::BrowseSource(HWND hDlg)
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

BOOL CDlgProcess::BrowseDestination(HWND hDlg)
{
	BOOL success;
	static char szFilter[] = TEXT("CSP SCN\0*.scn\0\0");
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
	ofn.lpstrDefExt		= TEXT("scn");
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

void CDlgProcess::InitDialog(HWND hDlg)
{
	SetDlgItemText(hDlg, IDC_ITERATIONS, "5");
	iQuadWidth = (int)pow(2, 5) +1;
	itoa(iQuadWidth, textbuffer, 10);
	SetDlgItemText(hDlg, IDC_QUADWIDTH, textbuffer);
	SetDlgItemInt(hDlg, IDC_QUADX, 0, 1);
	SetDlgItemInt(hDlg, IDC_QUADY, 0, 1);
	SetDlgItemInt(hDlg, IDC_PROC_ITERATION, 0, 1);
	SetDlgItemInt(hDlg, IDC_TARGETSIZE, 0, 1);
	m_sSourceFile[0] = FALSE;
	m_sDestinationFile[0] = FALSE;
}

BOOL CDlgProcess::QueryParameters()
{
	HINSTANCE hInst = (HINSTANCE) GetWindowLong(GetActiveWindow(), GWL_HINSTANCE);

	// Invoke the dialog box
	if (DialogBoxParam(hInst, (LPCTSTR) IDD_DIALOG_GENERATE, GetDesktopWindow(), 
		(DLGPROC) GlobalDlgProcessProc, (LPARAM) this))
		return true;
	else 
		return false;	
}

BOOL CDlgProcess::DlgProcessProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			EndDialog(hDlg, 1);
			return true;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return false;

		case ID_BROWSE_SRC:
			BrowseSource(hDlg);
			return true;

		case ID_BROWSE_DEST:
			BrowseDestination(hDlg);
			return true;

		case IDC_ITERATIONS:
			GetDlgItemText(hDlg, IDC_ITERATIONS, textbuffer, 255);
			iIterations = atoi(textbuffer);
			iQuadWidth = (int)pow(2, iIterations) +1;
			itoa(iQuadWidth, textbuffer, 10);
			SetDlgItemText(hDlg, IDC_QUADWIDTH, textbuffer);
			return true;

		case ID_BUILD:
			bResult = BuildTerrain(hDlg);
			return true;

		}

	break;
	}
	return false;
}


