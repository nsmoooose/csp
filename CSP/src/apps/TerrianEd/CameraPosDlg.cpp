// CameraPosDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TerrianEd.h"
#include "CameraPosDlg.h"
#include "EngineStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CEngineStatus	EngineStatus;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCameraPosDlg 


CCameraPosDlg::CCameraPosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraPosDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCameraPosDlg)
	m_iEdit_xpos = (int)EngineStatus.m_posx;
	m_iEdit_xrot = (int)EngineStatus.m_rotx;
	m_iEdit_ypos = (int)EngineStatus.m_posy;
	m_iEdit_yrot = (int)EngineStatus.m_roty;
	m_iEdit_zpos = (int)EngineStatus.m_posz;
	m_iEdit_zrot = 0;
	m_iEdit_camspeed = (int)EngineStatus.m_fSpeed;
	//}}AFX_DATA_INIT
}


void CCameraPosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCameraPosDlg)
	DDX_Text(pDX, IDC_EDIT_XPOS, m_iEdit_xpos);
	DDX_Text(pDX, IDC_EDIT_XROT, m_iEdit_xrot);
	DDV_MinMaxInt(pDX, m_iEdit_xrot, 0, 360);
	DDX_Text(pDX, IDC_EDIT_YPOS, m_iEdit_ypos);
	DDX_Text(pDX, IDC_EDIT_YROT, m_iEdit_yrot);
	DDV_MinMaxInt(pDX, m_iEdit_yrot, 0, 360);
	DDX_Text(pDX, IDC_EDIT_ZPOS, m_iEdit_zpos);
	DDX_Text(pDX, IDC_EDIT_ZROT, m_iEdit_zrot);
	DDV_MinMaxInt(pDX, m_iEdit_zrot, 0, 360);
	DDX_Text(pDX, IDC_EDIT_CAMSPEED, m_iEdit_camspeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCameraPosDlg, CDialog)
	//{{AFX_MSG_MAP(CCameraPosDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCameraPosDlg 

void CCameraPosDlg::OnOK() 
{
	CDialog::OnOK();
	EngineStatus.m_posx = (float)m_iEdit_xpos;
	EngineStatus.m_posy = (float)m_iEdit_ypos;
	EngineStatus.m_posz = (float)m_iEdit_zpos;
	EngineStatus.m_rotx = (float)m_iEdit_xrot;
	EngineStatus.m_roty = (float)m_iEdit_yrot;
//	EngineStatus.m_rotz = (float)m_iEdit_zrot;			/* not supported by engine yet */
	EngineStatus.m_fSpeed = (float)m_iEdit_camspeed;
}
