// TextOutputDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "F4Map2CSP.h"
#include "TextOutputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextOutputDlg 


CTextOutputDlg::CTextOutputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextOutputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextOutputDlg)
	m_sOutputText = _T("");
	//}}AFX_DATA_INIT
}


void CTextOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextOutputDlg)
	DDX_Text(pDX, IDC_EDIT_TEXTOUTPUT, m_sOutputText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextOutputDlg, CDialog)
	//{{AFX_MSG_MAP(CTextOutputDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTextOutputDlg 
