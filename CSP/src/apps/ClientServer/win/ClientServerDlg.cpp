// ClientServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientServer.h"
#include "ClientServerDlg.h"
//#include <net/Windows/WindowsNetwork.h>
#include <net/StandardNetwork.h>
#include <net/FactoryNetwork.h>
#include "clientServerCom.h"
//#include <winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TCP

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientServerDlg dialog

CClientServerDlg::CClientServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientServerDlg)
	m_Message = _T("");
	m_PortServer = 0;
	m_ClientNr = 0;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientServerDlg)
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddressServer);
	DDX_Control(pDX, IDC_IPADDRESS2, m_IPAddressClient);
	DDX_Text(pDX, IDC_Message, m_Message);
	DDX_Text(pDX, IDC_Port, m_PortServer);
	DDX_Text(pDX, IDC_ClientNr, m_ClientNr);
	DDV_MinMaxUInt(pDX, m_ClientNr, 1, 15);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientServerDlg, CDialog)
	//{{AFX_MSG_MAP(CClientServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnStartServer)
	ON_BN_CLICKED(IDC_BUTTON3, OnStartClient)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, OnChangeIPAddress)
	ON_EN_CHANGE(IDC_Port, OnChangePort)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS2, OnChangeIPAddressClient)
	ON_BN_CLICKED(IDC_UseUDPIP, OnUseUDPIP)
	ON_BN_CLICKED(IDC_UseTCPIP, OnUseTCPIP)
	ON_EN_CHANGE(IDC_ClientNr, OnChangeClientNr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientServerDlg message handlers
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CClientServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
/*TODO	struct hostent *h = WindowsNetwork::GetHostByName("localhost");
	h = WindowsNetwork::GetHostByName(h->h_name);
	
	if(!h->h_addr_list[0]) // has computer an IP address????
	{
*/		this->m_IPAddressServer.SetAddress(127,0,0,1);//192,168,1,227);//
		this->m_IPAddressClient.SetAddress(127,0,0,1);
/*	}
	else 
	{
		this->m_IPAddressServer.SetAddress(h->h_addr_list[0][0],h->h_addr_list[0][1],
										h->h_addr_list[0][2],h->h_addr_list[0][3]);
		this->m_IPAddressClient.SetAddress(h->h_addr_list[0][0],h->h_addr_list[0][1],
										h->h_addr_list[0][2],h->h_addr_list[0][3]);
	}
*/
	this->m_PortServer = 1256;
	this->protocolType = NETWORK_TCPIP;
	this->m_ClientNr = 1;

	CButton *radio1=(CButton *)GetDlgItem(IDC_UseUDPIP);
	radio1->SetCheck(0);

	CButton *radio2=(CButton *)GetDlgItem(IDC_UseTCPIP);
	radio2->SetCheck(1);

	CEdit *edit=(CEdit *)GetDlgItem(IDC_ClientNr);
	edit->SetReadOnly(FALSE);

	UpdateData(FALSE);	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
HCURSOR CClientServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnStartServer() 
{
	char str[512];
	short strLen = 512;
	BYTE b1,b2,b3,b4;
	long ip;
	
	this->m_IPAddressServer.GetAddress(b1,b2,b3,b4);
	ip = (b1<<24) + (b2<<16) + (b3<<8) + b4; 

	FactoryNetwork *netFac = new FactoryNetwork();
	printf("%d mögliche network-modes\n",netFac->GetCount());
	netFac->Create(_NETWORK,&(this->socket));
	delete netFac;

	if(this->socket->Create(this->protocolType,ip,(short &)this->m_PortServer)==NETWORK_RET_ERROR)
	{
		this->m_Message.Format("Error occured while socket creation.");
		UpdateData(FALSE);	
		return;
	}
	this->socketIsSet = true;

	if(this->protocolType == NETWORK_TCPIP)
		this->socket->Listen(this->m_ClientNr);

	//	this->socket->GetHostByName("localhost");

	sprintf(str,"Server on \"%d.%d.%d.%d\" port %d initialised.",((unsigned long)ip & (255 << 24)) >> 24,
		(ip & (255 << 16)) >> 16,(ip & (255 << 8)) >> 8,(ip & 255),this->m_PortServer);
	MessageBox(str);

	clientServerCom com;	
	com.Setup(this->socket,this->protocolType,1);
	com.DoModal();

	this->socket->Close();	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnStartClient() 
{
	char str[512];
	int counter = 1;
	BYTE b1,b2,b3,b4;
	long ipClient,ipServer;
	short portClient = 0;

	this->m_IPAddressClient.GetAddress(b1,b2,b3,b4);
	ipClient = (b1<<24) + (b2<<16) + (b3<<8) + b4; 

	this->m_IPAddressServer.GetAddress(b1,b2,b3,b4);
	ipServer = (b1<<24) + (b2<<16) + (b3<<8) + b4; 

	FactoryNetwork *netFac = new FactoryNetwork();
	printf("%d mögliche network-modes\n",netFac->GetCount());
	netFac->Create(_NETWORK,&(this->socket));
	delete netFac;

	if(this->socket->Create(this->protocolType,ipClient,portClient)==NETWORK_RET_ERROR)
	{
		this->m_Message.Format("Error occured while socket creation.");
		UpdateData(FALSE);	
		return;
	}

	this->socketIsSet = true;

	sprintf(str,"Client on \"%d.%d.%d.%d\" port %d initialised.",((unsigned long)ipClient & (255 << 24)) >> 24,
		(ipClient & (255 << 16)) >> 16,(ipClient & (255 << 8)) >> 8,(ipClient & 255),portClient);
	MessageBox(str);

	if(this->socket->Connect(ipServer,this->m_PortServer) == NETWORK_RET_OK)
	{
		clientServerCom com;
		com.Setup(this->socket,this->protocolType,0);
		com.DoModal();
	}
	this->socket->Close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnChangeIPAddress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(false);
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnChangePort() 
{
	UpdateData(true);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnChangeIPAddressClient(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(false);	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnUseUDPIP() 
{
	this->protocolType = NETWORK_UDPIP;

	CEdit *edit=(CEdit *)GetDlgItem(IDC_ClientNr);
	edit->SetReadOnly(TRUE);

	UpdateData(false);			
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnUseTCPIP() 
{
	this->protocolType = NETWORK_TCPIP;

	CEdit *edit=(CEdit *)GetDlgItem(IDC_ClientNr);
	edit->SetReadOnly(FALSE);

	UpdateData(false);				
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientServerDlg::OnChangeClientNr() 
{
	UpdateData(true);
}
