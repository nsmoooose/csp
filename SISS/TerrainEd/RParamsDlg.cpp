// RParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TerrianEd.h"
#include "RParamsDlg.h"
#include "EngineStatus.h"
#include "resource.h"

extern CMipMapRenderer		*m_renderer;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRParamsDlg dialog


CRParamsDlg::CRParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRParamsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRParamsDlg)
	m_iEdit_Max_Render_Distance = (int)m_renderer->GetMaxRenderDistance();
	m_iEdit_Detail_Level = (int)(m_renderer->GetDetailLevel());
	m_iEdit_Min_Render_Distance = (int)m_renderer->GetMinRenderDistance();
	m_iEdit_Render_Distance = (int)m_renderer->GetRenderDistance();
	m_iEdit_dz = (int)(m_renderer->GetDeltaZ() * 10);
	//}}AFX_DATA_INIT
}


void CRParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRParamsDlg)
	DDX_Text(pDX, IDC_EDIT_MAX_RENDER_DISTANCE, m_iEdit_Max_Render_Distance);
	DDX_Text(pDX, IDC_EDIT_DETAIL_LEVEL, m_iEdit_Detail_Level);
	DDX_Text(pDX, IDC_EDIT_MIN_RENDER_DISTANCE, m_iEdit_Min_Render_Distance);
	DDX_Text(pDX, IDC_EDIT_RENDER_DISTANCE, m_iEdit_Render_Distance);
	DDX_Text(pDX, IDC_EDIT_DXYZ, m_iEdit_dxyz);
	DDX_Text(pDX, IDC_EDIT_DZ, m_iEdit_dz);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRParamsDlg, CDialog)
	//{{AFX_MSG_MAP(CRParamsDlg)
	ON_EN_CHANGE(IDC_EDIT_MAX_RENDER_DISTANCE, OnChangeEditMaxRenderDistance)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERDXYZ, OnReleasedcaptureSliderdxyz)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERMXRD, OnReleasedcaptureSlidermxrd)
	ON_EN_CHANGE(IDC_EDIT_DXYZ, OnChangeEditDxyz)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERMIRD, OnReleasedcaptureSlidermird)
	ON_EN_CHANGE(IDC_EDIT_MIN_RENDER_DISTANCE, OnChangeEditMinRenderDistance)
	ON_EN_CHANGE(IDC_EDIT_DETAIL_LEVEL, OnChangeEditDetailLevel)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERDL, OnReleasedcaptureSliderdl)
	ON_EN_CHANGE(IDC_EDIT_DZ, OnChangeEditDz)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERDZ, OnReleasedcaptureSliderdz)
	ON_EN_CHANGE(IDC_EDIT_RENDER_DISTANCE, OnChangeEditRenderDistance)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERRD, OnReleasedcaptureSliderrd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRParamsDlg message handlers

void CRParamsDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_renderer->SetDeltaZ(((float)m_iEdit_dz) / 10);
//	m_renderer->SetQuadLength(SliderQL->GetPos());			// tweaking causes crashes! 
	m_renderer->SetDetailLevel((float)m_iEdit_Detail_Level);
	m_renderer->SetRenderDistance((float)m_iEdit_Render_Distance);
	m_renderer->SetMinRenderDistance((float)m_iEdit_Min_Render_Distance);
	m_renderer->SetMaxRenderDistance((float)m_iEdit_Max_Render_Distance);

	m_renderer->UpdateSettings();

	CDialog::OnOK();
}

BOOL CRParamsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SliderDXYZ	= (CSliderCtrl *) GetDlgItem(IDC_SLIDERDXYZ);
	SliderDZ	= (CSliderCtrl *) GetDlgItem(IDC_SLIDERDZ);
	SliderDL	= (CSliderCtrl *) GetDlgItem(IDC_SLIDERDL);
	SliderRD	= (CSliderCtrl *) GetDlgItem(IDC_SLIDERRD);
	SliderMIRD	= (CSliderCtrl *) GetDlgItem(IDC_SLIDERMIRD);
	SliderMXRD	= (CSliderCtrl *) GetDlgItem(IDC_SLIDERMXRD);

	SliderDXYZ->SetRange(1, 70);
	SliderDZ->SetRange(0, 20);
	SliderDL->SetRange(1, 60);
	SliderRD->SetRange(1, 5000);
	SliderMIRD->SetRange(1, 5000);
	SliderMXRD->SetRange(1, 5000);

	SliderDXYZ->SetTicFreq(5);
	SliderDZ->SetTicFreq(1);
	SliderDL->SetTicFreq(1);
	SliderRD->SetTicFreq(2);
	SliderMIRD->SetTicFreq(10);
	SliderMXRD->SetTicFreq(100);

	SliderDXYZ->SetPos(m_iEdit_dxyz);
	SliderDZ->SetPos(m_iEdit_dz);
	SliderDL->SetPos(m_iEdit_Detail_Level);
	SliderRD->SetPos(m_iEdit_Render_Distance);
	SliderMIRD->SetPos(m_iEdit_Min_Render_Distance);
	SliderMXRD->SetPos(m_iEdit_Max_Render_Distance);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRParamsDlg::OnChangeEditMaxRenderDistance() 
{
	UpdateData(TRUE);
	SliderMXRD->SetPos(m_iEdit_Max_Render_Distance);
	UpdateData(FALSE);
}

void CRParamsDlg::OnReleasedcaptureSlidermxrd(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iEdit_Max_Render_Distance = SliderMXRD->GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}

void CRParamsDlg::OnChangeEditMinRenderDistance() 
{
	UpdateData(TRUE);
	SliderMIRD->SetPos(m_iEdit_Min_Render_Distance);
	UpdateData(FALSE);
}

void CRParamsDlg::OnReleasedcaptureSlidermird(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iEdit_Min_Render_Distance = SliderMIRD->GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}

void CRParamsDlg::OnChangeEditDxyz() 
{
	UpdateData(TRUE);
	SliderMXRD->SetPos(m_iEdit_dxyz);
	UpdateData(FALSE);
}

void CRParamsDlg::OnReleasedcaptureSliderdxyz(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iEdit_dxyz = SliderDXYZ->GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}

void CRParamsDlg::OnChangeEditDetailLevel() 
{
	UpdateData(TRUE);
	SliderDL->SetPos(m_iEdit_Detail_Level);
	UpdateData(FALSE);
}

void CRParamsDlg::OnReleasedcaptureSliderdl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iEdit_Detail_Level = SliderDL->GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}

void CRParamsDlg::OnChangeEditDz() 
{
	UpdateData(TRUE);
	SliderDZ->SetPos(m_iEdit_dz);
	UpdateData(FALSE);
}

void CRParamsDlg::OnReleasedcaptureSliderdz(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iEdit_dz = SliderDZ->GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}

void CRParamsDlg::OnChangeEditRenderDistance() 
{
	UpdateData(TRUE);
	SliderRD->SetPos(m_iEdit_Render_Distance);
	UpdateData(FALSE);
}

void CRParamsDlg::OnReleasedcaptureSliderrd(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iEdit_Render_Distance = SliderRD->GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}
