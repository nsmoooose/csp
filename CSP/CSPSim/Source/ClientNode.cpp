#include "Networking.h"
#include "ClientNode.h"
#include <stdio.h>
#include <stdlib.h>
#include "Config.h"      

#include <SimData/Types.h>
#include <SimData/ExceptionBase.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/FileUtility.h>
#include <SimData/GeoPos.h>

ClientNode::ClientNode()
{

}

int ClientNode::run()
{

  printf("sizeof(int) = %d\n", sizeof(int));
  printf("sizeof(double) = %d\n", sizeof(double));
  printf("sizeof(simdata::Vector3) = %d\n", sizeof(simdata::Vector3));
  printf("sizeof(simdata::Quat) = %d\n", sizeof(simdata::Quat));
  printf("sizeof(simdata::SimTime) = %d\n", sizeof(simdata::SimTime));
  
	
  printf("Network test client starting up...\n");
  Port localPort = g_Config.getInt("Networking", "LocalMessagePort", 10000, true);
  std::string localHost = g_Config.getString("Networking", "LocalMessageHost", "127.0.0.1", true);

  Port remotePort = (Port)g_Config.getInt("Networking", "RemoteMessagePort", 0, true);
  std::string remoteHost = g_Config.getString("Networking", "RemoteMessageHost", "127.0.0.1", true);
		   
  
  NetworkNode * remoteNode = new NetworkNode(1, remoteHost.c_str(), remotePort );
  NetworkNode * localNode =  new NetworkNode(1, localHost.c_str(), localPort);
  NetworkMessenger * networkMessenger = new NetworkMessenger(localNode);

  NetworkMessage * message = networkMessenger->getMessageFromPool(1, 100);
  char * payloadPtr = (char*)message->getPayloadPtr();
  memset(payloadPtr, 0 , 100);
  strcpy(payloadPtr, "Hello From CSP Network Test Client!");
  Port port = message->getOriginatorPort();
  networkMessenger->queueMessage(remoteNode, message);
  networkMessenger->sendMessages();
  
  return 0;
}



