#if !defined(AFX_RPARAMSDLG_H__B6604981_D1A9_4F44_BE2A_0F291EE6B497__INCLUDED_)
#define AFX_RPARAMSDLG_H__B6604981_D1A9_4F44_BE2A_0F291EE6B497__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RParamsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRParamsDlg dialog

class CRParamsDlg : public CDialog
{
// Construction
public:
	CRParamsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRParamsDlg)
	enum { IDD = IDD_RPARAMS };
	int		m_iEdit_Max_Render_Distance;
	int		m_iEdit_Detail_Level;
	int		m_iEdit_Min_Render_Distance;
	int		m_iEdit_Render_Distance;
	int		m_iEdit_dxyz;
	int		m_iEdit_dz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRParamsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRParamsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditMaxRenderDistance();
	afx_msg void OnReleasedcaptureSliderdxyz(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlidermxrd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditDxyz();
	afx_msg void OnReleasedcaptureSlidermird(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditMinRenderDistance();
	afx_msg void OnChangeEditDetailLevel();
	afx_msg void OnReleasedcaptureSliderdl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditDz();
	afx_msg void OnReleasedcaptureSliderdz(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditRenderDistance();
	afx_msg void OnReleasedcaptureSliderrd(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl* SliderDXYZ;
	CSliderCtrl* SliderDZ;
	CSliderCtrl* SliderDL;
	CSliderCtrl* SliderRD;
	CSliderCtrl* SliderMIRD;
	CSliderCtrl* SliderMXRD;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RPARAMSDLG_H__B6604981_D1A9_4F44_BE2A_0F291EE6B497__INCLUDED_)
