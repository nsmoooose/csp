// ClientServerDlg.h : header file
//

#if !defined(AFX_CLIENTSERVERDLG_H__30DC8CD4_51DF_4A2E_9B36_7A17DF9BCC90__INCLUDED_)
#define AFX_CLIENTSERVERDLG_H__30DC8CD4_51DF_4A2E_9B36_7A17DF9BCC90__INCLUDED_

#include <net/StandardNetwork.h>	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientServerDlg dialog

class CClientServerDlg : public CDialog
{
// Construction
public:
	CClientServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClientServerDlg)
	enum { IDD = IDD_CLIENTSERVER_DIALOG };
	CIPAddressCtrl	m_IPAddressServer;
	CIPAddressCtrl	m_IPAddressClient;
	CString	m_Message;
	UINT	m_PortServer;
	UINT	m_ClientNr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int protocolType;
	bool socketIsSet;
	StandardNetwork *socket;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClientServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartServer();
	afx_msg void OnStartClient();
	afx_msg void OnChangeIPAddress(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangePort();
	afx_msg void OnChangeIPAddressClient(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUseUDPIP();
	afx_msg void OnUseTCPIP();
	afx_msg void OnChangeClientNr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSERVERDLG_H__30DC8CD4_51DF_4A2E_9B36_7A17DF9BCC90__INCLUDED_)
