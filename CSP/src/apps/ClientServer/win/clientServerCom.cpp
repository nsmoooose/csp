// clientServerCom.cpp : implementation file
//

#include "stdafx.h"
#include "ClientServer.h"
#include "clientServerCom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// clientServerCom dialog


clientServerCom::clientServerCom(CWnd* pParent /*=NULL*/)
	: CDialog(clientServerCom::IDD, pParent)
{
	//{{AFX_DATA_INIT(clientServerCom)
	m_Sends = _T("");
	m_Receives = _T("");
	//}}AFX_DATA_INIT
}


void clientServerCom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(clientServerCom)
	DDX_Text(pDX, IDC_EDIT1, m_Sends);
	DDX_LBString(pDX, IDC_Receives, m_Receives);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(clientServerCom, CDialog)
	//{{AFX_MSG_MAP(clientServerCom)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT1, OnSendMessages)
	ON_BN_CLICKED(IDC_BUTTON1, OnSend)
	ON_BN_CLICKED(IDC_CloseIt, OnCloseIt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// clientServerCom message handlers

BOOL clientServerCom::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->SetTimer(1,100,NULL);

	this->clientList = NULL;
	this->clientListLen = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void clientServerCom::OnOK()
{
}


void clientServerCom::Setup(StandardNetwork *_s, int _protocolType, int _mode)
{
	this->network = _s;
	this->protocolType = _protocolType;
	this->mode = _mode;
}

void clientServerCom::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	char str[1024];
	char str2[1024];
	short strLen = 1024;
	long ip;
	short port;
	StandardNetwork *newClient;
	

	if(this->mode == 0) // client
	{
		if(this->network->Receive(str,&strLen,10)==NETWORK_RET_OK)
		{
			CListBox *list=(CListBox *)GetDlgItem(IDC_Receives);
			list->AddString(str);
			UpdateData(FALSE);
		}
	}
	else
	{
		short ret = this->network->Accept(&newClient,10);
		if(ret == NETWORK_RET_OK)
		{
			for(int i=0;i<this->clientListLen;i++)
			{
				if(!this->clientList[i])
					break;
			}

			if(i<this->clientListLen)
			{
				this->clientList[i] = newClient;	
			}
			else
			{
				i=this->clientListLen;
				this->clientList = (StandardNetwork **)realloc(this->clientList,
									sizeof(StandardNetwork *)*(this->clientListLen+1));
				this->clientList[this->clientListLen] = newClient;
				this->clientListLen++;
			}

			this->clientList[i]->GetPeerAddress(ip,port);
			sprintf(str2,"new client (%d.) on %d.%d.%d.%d port %d",i,
				((unsigned long)ip & (255 << 24)) >> 24,(ip & (255 << 16)) >> 16,(ip & (255 << 8)) >> 8,(ip & 255),port);
			CListBox *list=(CListBox *)GetDlgItem(IDC_Receives);
			list->AddString(str2);
			UpdateData(FALSE);
		}

		for(int i=0;i<this->clientListLen;i++)
		{
			if(this->clientList[i])
			{
				short strLen = 1024;
				if((this->clientList[i]->Receive(str,&strLen,10) == NETWORK_RET_OK))
				{
					this->clientList[i]->GetPeerAddress(ip,port);
					sprintf(str2,"client %2d: %s",i+1,str);
					CListBox *list=(CListBox *)GetDlgItem(IDC_Receives);
					list->AddString(str2);
					UpdateData(FALSE);
	
					for(int j=0;j<this->clientListLen;j++)
					{
						if(this->clientList[j] && (j!=i))
							this->clientList[j]->Send(str2,strlen(str2)+1);											
					}
				}
				else if(strLen == 0)
				{
					this->clientList[i]->Close();
					delete this->clientList[i];
					this->clientList[i]=NULL;
					
					sprintf(str2,"client %2d disconnected",i+1);
					CListBox *list=(CListBox *)GetDlgItem(IDC_Receives);
					list->AddString(str2);
					UpdateData(FALSE);				
				}
			}
		}

	}
	CDialog::OnTimer(nIDEvent);
}


void clientServerCom::OnSendMessages() 
{
	UpdateData(true);
}

void clientServerCom::OnSend() 
{
	char str[1024];

	// TODO: Add your control notification handler code here
	sprintf(str,"send    : %s",(LPCTSTR)this->m_Sends);
	CListBox *list=(CListBox *)GetDlgItem(IDC_Receives);
	list->AddString(str);
	UpdateData(FALSE);

	if(this->mode == 0) // kein Server
	{
		if(this->network->Send((LPCTSTR)this->m_Sends,this->m_Sends.GetLength()+1) == NETWORK_RET_ERROR)
		{
			MessageBox("Server has disconnected -> shut client down");
			OnCloseIt();
		}
		else
		{
			this->m_Sends.Format("");
			UpdateData(false);
		}
	}else if(this->protocolType == NETWORK_TCPIP)
	{
		CString tmp("Server : ");
		tmp+=this->m_Sends;

		for(int i=0;i<this->clientListLen;i++)
			if(this->clientList[i])
				this->clientList[i]->Send((LPCTSTR)tmp,tmp.GetLength()+1);					
		this->m_Sends.Format("");
		UpdateData(false);
	}

}

void clientServerCom::OnCloseIt() 
{
	// TODO: Add your control notification handler code here
	for(int i=0;i<this->clientListLen;i++)
	{
		if(this->clientList[i])
		{
			this->clientList[i]->Close();
			delete this->clientList[i];
		}
	}
	if(this->clientList)
	{
		free(this->clientList);
		this->clientList = NULL;
	}
	CDialog::OnOK();
}
