// console_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "winnetwork.h"
#include "winsock2.h"

int main(int argc, char* argv[])
{
	WinNetwork* client;
	printf("Creating SOCKET object...\n");
	SOCKET clientsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	long  local_ip = htonl(inet_addr("127.0.0.1"));
	printf("Local ip set to: \t%d\n", local_ip);

	short local_port = 31337;
	printf("Local port set to: \t%d\n", local_port);

	long input_ip = htonl(inet_addr("127.0.0.1")); //0xD8A17A91 - 216.161.122.145
	printf("Remote IP set to: \t%d\n", input_ip);

	short server_port = 31336;
	printf("Remote port: \t\t%d\n", server_port);

	char buffer[256] = "Testing 123";
	client = new WinNetwork(clientsocket);
	if(!client) exit(1);
	client->Create(NETWORK_UDPIP, local_ip, local_port);
	while(1) {
		client->SendTo(buffer, 256, input_ip, server_port);
		//printf(".");
	}
	return 0;
}
