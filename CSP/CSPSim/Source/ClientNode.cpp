


#include "Networking.h"
#include <stdio.h>
#include <stdlib.h>
#include "ClientNode.h"

ClientNode::ClientNode()
{

}

int ClientNode::run()
{
  printf("Network test client starting up...\n");
  Port clientPort = 20010;
  Port serverPort = 20011;
  MessageSocketDuplex * socketDuplex = new MessageSocketDuplex(clientPort);
  NetworkMessage message;
  message.initialize(1, 100);
  char * payloadPtr = (char*)message.getPayloadPtr();
  memset(payloadPtr, 0 , 100);
  strcpy(payloadPtr, "Hello From CSP Network Test Client!");
  ost::InetHostAddress * serverAddress = new ost::InetHostAddress("localhost");
  socketDuplex->sendto(message, serverAddress, &serverPort);
  
  return 0;
}

#ifdef CSP_STANDALONE

int main(int argc, char * argv[])
{
   ClientNode node;
   return node.run();
}

#endif


