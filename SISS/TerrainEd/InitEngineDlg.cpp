// InitEngineDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TerrianEd.h"
#include "InitEngineDlg.h"
#include "EngineStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInitEngineDlg 
extern CEngineStatus EngineStatus;


CInitEngineDlg::CInitEngineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInitEngineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInitEngineDlg)
	m_iDeltaH = (int)(EngineStatus.m_fEpsilonDeltaH * 1000);
	m_iNearClip = (int)(EngineStatus.m_fEpsilonNearClip * 1000);
	m_iTopCoord = (int)(EngineStatus.m_fEpsilonTopCoor * 1000);
	m_iQuadSize16 = 2;
//	m_cFileName = _T("");
	m_cFileName = "terrain2.raw";
	//}}AFX_DATA_INIT
}


void CInitEngineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitEngineDlg)
	DDX_Text(pDX, IDC_EDIT_DELTAH, m_iDeltaH);
	DDX_Text(pDX, IDC_EDIT_NEARCLIP, m_iNearClip);
	DDX_Text(pDX, IDC_EDIT_TOPCOORD, m_iTopCoord);
	DDX_Radio(pDX, IDC_RADIO1, m_iQuadSize16);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_cFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitEngineDlg, CDialog)
	//{{AFX_MSG_MAP(CInitEngineDlg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInitEngineDlg 

void CInitEngineDlg::OnOK() 
{
	// TODO: Zusätzliche Prüfung hier einfügen
	
	CDialog::OnOK();

	EngineStatus.m_fEpsilonDeltaH = (float)m_iDeltaH / 1000;
	EngineStatus.m_fEpsilonNearClip = (float)m_iNearClip / 1000;
	EngineStatus.m_fEpsilonTopCoor = (float)m_iTopCoord / 1000;
	strcpy(EngineStatus.m_sMapFileName, m_cFileName);

	switch(m_iQuadSize16)
	{
		case 0:
			EngineStatus.m_iQuadLength = 16;
			break;
		case 1:
			EngineStatus.m_iQuadLength = 32;
			break;
		case 2:
			EngineStatus.m_iQuadLength = 64;
			break;
		case 3:
			EngineStatus.m_iQuadLength = 128;
			break;
	}
}

void CInitEngineDlg::OnButtonBrowse() 
{
	CFileDialog	FileDialog(TRUE, NULL, "terrain2.raw",
						   OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
						   "RAW - 8-Bit RAW data|*.raw|MAP - Falcon 4.0 Terrain Map|*.map|");

	FileDialog.m_ofn.lpstrTitle="Select Map file";
	if (FileDialog.DoModal() == IDOK)
	{
		m_cFileName=FileDialog.GetPathName();
		UpdateData(FALSE);
	}
}
