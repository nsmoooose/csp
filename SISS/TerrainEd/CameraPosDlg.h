#if !defined(AFX_CAMERAPOSDLG_H__F3B508E0_1BC6_11D5_98AF_444553540000__INCLUDED_)
#define AFX_CAMERAPOSDLG_H__F3B508E0_1BC6_11D5_98AF_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CameraPosDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCameraPosDlg 

class CCameraPosDlg : public CDialog
{
// Konstruktion
public:
	CCameraPosDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CCameraPosDlg)
	enum { IDD = IDD_DIALOG_SETCAMERA };
	int		m_iEdit_xpos;
	int		m_iEdit_xrot;
	int		m_iEdit_ypos;
	int		m_iEdit_yrot;
	int		m_iEdit_zpos;
	int		m_iEdit_zrot;
	int		m_iEdit_camspeed;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCameraPosDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCameraPosDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CAMERAPOSDLG_H__F3B508E0_1BC6_11D5_98AF_444553540000__INCLUDED_
