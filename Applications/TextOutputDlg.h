#if !defined(AFX_TEXTOUTPUTDLG_H__88FEB378_C383_4B05_AE4A_B7806290E813__INCLUDED_)
#define AFX_TEXTOUTPUTDLG_H__88FEB378_C383_4B05_AE4A_B7806290E813__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextOutputDlg.h : Header-Datei
//

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
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEXTOUTPUTDLG_H__88FEB378_C383_4B05_AE4A_B7806290E813__INCLUDED_
