#if !defined(AFX_INITENGINEDLG_H__E41C3220_2060_11D5_98AF_444553540000__INCLUDED_)
#define AFX_INITENGINEDLG_H__E41C3220_2060_11D5_98AF_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InitEngineDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInitEngineDlg 

class CInitEngineDlg : public CDialog
{
// Konstruktion
public:
	CInitEngineDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CInitEngineDlg)
	enum { IDD = IDD_INITENGINE };
	int		m_iDeltaH;
	int		m_iNearClip;
	int		m_iTopCoord;
	int		m_iQuadSize16;
	CString	m_cFileName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CInitEngineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CInitEngineDlg)
	virtual void OnOK();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INITENGINEDLG_H__E41C3220_2060_11D5_98AF_444553540000__INCLUDED_
