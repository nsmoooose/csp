// F4Map2CSPDlg.h : Header-Datei
//

#if !defined(AFX_F4MAP2CSPDLG_H__055872B6_98AD_400E_9246_EC53E5B261D0__INCLUDED_)
#define AFX_F4MAP2CSPDLG_H__055872B6_98AD_400E_9246_EC53E5B261D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CF4Map2CSPDlg Dialogfeld

class CF4Map2CSPDlg : public CDialog
{
// Konstruktion
public:
	CF4Map2CSPDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CF4Map2CSPDlg)
	enum { IDD = IDD_F4MAP2CSP_DIALOG };
	CString	m_cFileNameSrc;
	CString	m_cFileNameDest;
	int		m_iLevel;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CF4Map2CSPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CF4Map2CSPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowseSrc();
	afx_msg void OnBrowseDest();
	afx_msg BOOL OnConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_F4MAP2CSPDLG_H__055872B6_98AD_400E_9246_EC53E5B261D0__INCLUDED_)
