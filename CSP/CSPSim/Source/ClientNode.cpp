#include "Networking.h"
#include "ClientNode.h"
#include <stdio.h>
#include <stdlib.h>

ClientNode::ClientNode()
{

}

int ClientNode::run()
{
  printf("Network test client starting up...\n");
  Port localPort = 3150;
  Port remotePort = 3160;
  NetworkNode localNode(1,  "localhost", localPort);
  NetworkNode remoteNode(1, "localhost", remotePort);
  NetworkMessenger * messenger = new NetworkMessenger(&localNode);
  NetworkMessage * message = messenger->getMessageFromPool(1, 100);
  char * payloadPtr = (char*)message->getPayloadPtr();
  memset(payloadPtr, 0 , 100);
  strcpy(payloadPtr, "Hello From CSP Network Test Client!");
  Port port = message->getOriginatorPort();
  messenger->queueMessage(&remoteNode, message);
  messenger->sendMessages();
  
  return 0;
}



