#if !defined(AFX_TEXTOUTPUTDLG_H__27D9FC40_2AA7_11D5_98B0_00E07D8D0E76__INCLUDED_)
#define AFX_TEXTOUTPUTDLG_H__27D9FC40_2AA7_11D5_98B0_00E07D8D0E76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextOutputDlg.h : Header-Datei
//

#ifndef IDD_DIALOG_TEXTOUTPUT
#define IDD_DIALOG_TEXTOUTPUT 137
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextOutputDlg 

class CTextOutputDlg : public CDialog
{
// Konstruktion
public:
	CTextOutputDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTextOutputDlg)
	enum { IDD = IDD_DIALOG_TEXTOUTPUT };
	CString	m_sOutputText;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTextOutputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTextOutputDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEXTOUTPUTDLG_H__27D9FC40_2AA7_11D5_98B0_00E07D8D0E76__INCLUDED_
