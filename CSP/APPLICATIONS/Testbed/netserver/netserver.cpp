// netserver.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "winnetwork.h"

WinNetwork* server;
long  local_ip = INADDR_ANY;
short local_port = 31336;

char buffer[256];

SOCKET serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

BOOL CALLBACK DialogProc(HWND hDlg, UINT MSG, WPARAM wParam, LPARAM lParam)
{
	switch(MSG)
	{
	case WM_INITDIALOG:
		server->SetupEvent(hDlg);
		break;
	case WM_COMMAND:
	{
		switch(wParam) {
			case IDC_CANCEL: 
				EndDialog(hDlg, 0); 
				break;
		}
	}break;
	case WM_SOCKETREAD:
		{
			memset(buffer, '\0', 256);
			server->Receive(buffer, (short*)256, 1);
			SetDlgItemText(hDlg, IDC_MESSAGE, buffer);
			break;
		}
	default:
		break;

	}
	return(0);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	server = new WinNetwork(serversocket);
	if(server->Create(NETWORK_UDPIP, local_ip, local_port) != 0) {
		MessageBox(NULL, "Error creating WinNetwork.","Error", MB_OK);
		return false;
	}
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

	return 0;
}



