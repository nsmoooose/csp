#if !defined(AFX_CLIENTSERVERCOM_H__08D33321_0045_11D5_9E76_00E07D798D7D__INCLUDED_)
#define AFX_CLIENTSERVERCOM_H__08D33321_0045_11D5_9E76_00E07D798D7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <net/StandardNetwork.h>

// clientServerCom.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// clientServerCom dialog

class clientServerCom : public CDialog
{
// Construction
public:
	void Setup(StandardNetwork *_s,int _protocolType,int _mode);
	clientServerCom(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(clientServerCom)
	enum { IDD = IDD_DIALOG1 };
	CString	m_Sends;
	CString	m_Receives;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(clientServerCom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int clientListLen;
	StandardNetwork ** clientList;
	int mode;
	int protocolType;
	StandardNetwork * network;

	// Generated message map functions
	//{{AFX_MSG(clientServerCom)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSendMessages();
	afx_msg void OnSend();
	afx_msg void OnCloseIt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSERVERCOM_H__08D33321_0045_11D5_9E76_00E07D798D7D__INCLUDED_)
