#include "Networking.h"
#include "ServerNode.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif

ServerNode::ServerNode()
{

}

int ServerNode::run()
{
  printf("Network test client starting up...\n");
  Port remotePort = 3150;
  Port localPort = 3160;
  unsigned short messageLen = 512;
  NetworkNode * remoteNode;
  NetworkNode localNode(1, "localhost", localPort);

  MessageSocketDuplex * socket = new MessageSocketDuplex(localPort);
  NetworkMessage * message=NULL;
  while(1)
  {
    int numreceived = socket->recvfrom(&message);
    if (numreceived > 0)
    {
	Port port = message->getOriginatorPort();
        printf("Received Data From Client:\n");
	printf("Client port: %d\n", port);
    }
    else
    {
#ifndef WIN32
	::sleep(1);
#endif
    }
  }
 
  return 0;
}


