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
  Port clientPort = 20010;
  Port serverPort = 20011;
  MessageSocketDuplex * socket = new MessageSocketDuplex(serverPort);
  NetworkMessage message;
  message.initialize(1, 100);
  memset((void*)message.getPayloadPtr(), 0, 100);
  while(1)
  {
    int numreceived = socket->recvfrom(message, NULL, &clientPort);
    char * payloadPtr = (char*)message.getPayloadPtr();
    printf("Received Mesage From Client: %s\n", payloadPtr);
    memset((void*)message.getPayloadPtr(), 0, 100);
    if (numreceived == 0)
    {
//	::sleep(5);
    }
  }
 
  return 0;
}

#ifdef CSP_STANDALONE

int main(int argc, char * argv[])
{
  ServerNode node;
  return node.run();
}

#endif

