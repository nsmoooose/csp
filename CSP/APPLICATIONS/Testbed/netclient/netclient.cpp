// netclient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "winnetwork.h"
#include "winsock2.h"

WinNetwork* client;

long  local_ip = htonl(inet_addr("127.0.0.1"));
short local_port = 31337;

short server_port = 31336;

BOOL CALLBACK DialogProc(HWND hDlg, UINT MSG, WPARAM wParam, LPARAM lParam)
{
	switch(MSG)
	{
		case WM_COMMAND:
		{
			switch(wParam) {
				case IDC_SEND:
				{
					char buffer[256];
					char input_ip[256];
					GetDlgItemText(hDlg, IDC_MESSAGE, buffer, 256);
					GetDlgItemText(hDlg, IDC_SERVERIP, input_ip, 256);
					int nRet = client->SendTo(buffer, 256, htonl(inet_addr(input_ip)), server_port);
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
	}
	return(0);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SOCKET clientsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	client = new WinNetwork(clientsocket);
	client->Create(NETWORK_UDPIP, local_ip, local_port);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

	return 0;
}



