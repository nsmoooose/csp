// console_server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "winnetwork.h"
#include "typesnetwork.h"

int main(int argc, char* argv[])
{
	WinNetwork* server;

	printf("Creating SOCKET object...\n");
	SOCKET serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	long  local_ip = INADDR_ANY;//;htonl(inet_addr("127.0.0.1"))
	short local_port = 31336;

	char buffer[MAX_DGRAM_SIZE];

	server = new WinNetwork(serversocket);

	server->Create(NETWORK_UDPIP, local_ip, local_port);

	short size = 1;

	while(1) {
		memset(buffer, '\0', MAX_DGRAM_SIZE);
		server->Receive(buffer, (short*)MAX_DGRAM_SIZE, 10);
		
		Sleep(500);
		printf("Received: %s\n", buffer);
	}

	return 0;
}
