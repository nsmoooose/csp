// TextOutputDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "terrianed.h"
#include "TextOutputDlg.h"
#include "EngineStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextOutputDlg 


CTextOutputDlg::CTextOutputDlg(CWnd* pParent)
	: CDialog(CTextOutputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextOutputDlg)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CTextOutputDlg::OnOK() 
{
	// TODO: Zusätzliche Prüfung hier einfügen
	
	CDialog::OnOK();
}
