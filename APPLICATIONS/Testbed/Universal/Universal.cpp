// Universal.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "winnetwork.h"
#include "winsock2.h"
#include "resource.h"

WinNetwork* server;
WinNetwork* client;
long  ip_local_server = INADDR_ANY; //address of the local server
long  ip_local_client = INADDR_ANY;//htonl(inet_addr("127.0.0.1")); //interface address of the local client
short port_local_server = 31336;
short port_local_client = 31337;

char buffer[256];

SOCKET serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
SOCKET clientsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

BOOL CALLBACK DialogProc(HWND hDlg, UINT MSG, WPARAM wParam, LPARAM lParam)
{
	switch(MSG)
	{
	case WM_INITDIALOG:
		//Create a server object
		if(server->Create(NETWORK_UDPIP, ip_local_server, port_local_server) != 0) {
			MessageBox(NULL, "Error creating WinNetwork server.","Error", MB_OK);
			return false;
		}
		//Create a client object
		if(client->Create(NETWORK_UDPIP, ip_local_client, port_local_client) != 0) {
			MessageBox(NULL, "Error creating WinNetwork client.", "Error", MB_OK);
			return false;
		}
		//Set up the server object so we get a Windows Message when a packet is to be received.
		server->SetupEvent(hDlg);
		break;
	case WM_COMMAND:
	{
		switch(wParam) {
		case IDC_SEND:
			{
				char input_ip[256];
				GetDlgItemText(hDlg, IDC_SENDTEXT, buffer, 256);
				GetDlgItemText(hDlg, IDC_DEST_IP, input_ip, 256);
				SetDlgItemText(hDlg, IDC_SENDTEXT, "");
				long ip = client->ResolveHostname(input_ip);
				int nRet = client->SendTo(buffer, 256, htonl(ip), port_local_server);
				if(nRet != 0) {
						MessageBox(hDlg, "SendTo: Error.", "Error.", MB_OK);
				}
			}
			break;
		case IDC_CANCEL: 
			EndDialog(hDlg, 0); 
			break;
		}
	}break;
	case WM_SOCKETREAD:
		{
			memset(buffer, '\0', 256);
			server->Receive(buffer, (short*)256, 1);
			SetDlgItemText(hDlg, IDC_RECEIVEDTEXT, buffer);
			break;
		}
	default:
		break;

	}
	return(0);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	server = new WinNetwork(serversocket);
	client = new WinNetwork(clientsocket);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

	return 0;
}



