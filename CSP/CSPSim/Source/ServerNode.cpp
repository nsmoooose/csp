#include "Networking.h"
#include "ServerNode.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "Config.h"

ServerNode::ServerNode()
{

}

int ServerNode::run()
{
  int count = 0;
  printf("Network test server starting up...\n");
  Port remotePort = g_Config.getInt("Networking", "LocalMessagePort", 10000, true);
  std::string remoteHost = g_Config.getString("Networking", "LocalMessageHost", "127.0.0.1", true);

  Port localPort = (Port)g_Config.getInt("Networking", "RemoteMessagePort", 0, true);
  std::string localHost = g_Config.getString("Networking", "RemoteMessageHost", "127.0.0.1", true);
  
  NetworkNode * remoteNode = new NetworkNode(1, remoteHost.c_str(), remotePort );
  NetworkNode * localNode =  new NetworkNode(1, localHost.c_str(), localPort);
  
  MessageSocketDuplex * socketDuplex = new MessageSocketDuplex(localPort);
  NetworkMessage * message=NULL;
  while(1)
  {
    int numreceived = socketDuplex->recvfrom(&message);
    if (numreceived > 0)
    {
	if (count % 100 == 0)
	{
	  NetworkNode * node = message->getOriginatorNode();
	  MessageHeader * header = (MessageHeader*)message;
          printf("Received Data From Client:\n");
	  printf("Client addr: %s\n", node->getHostname());
	  printf("Client port: %d\n", node->getPort());

	  printf("MagicNumber: 0x%x\n", header->m_magicNumber);
	  printf("PayloadLen: %u\n", header->m_payloadLen);
	  printf("MessageType: %u\n", header->m_messageType);
	  printf("IPAddr: 0x%x\n", header->m_ipaddr);
	  printf("Port: %u\n", header->m_port);
	  printf("ID: %u\n", header->m_id);
			  
          ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
          printf("ID: %u\n", ptrPayload->id);
	  printf("TimeStamp: %u\n", ptrPayload->timeStamp);
	  printf("PositionX: %f, PositionY: %f, PositionZ: %f\n", 
			ptrPayload->globalPosition.x,
			ptrPayload->globalPosition.y,
			ptrPayload->globalPosition.z);
	}
	count++;
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


