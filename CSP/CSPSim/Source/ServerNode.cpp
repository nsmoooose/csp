#include "Networking.h"
#include "ServerNode.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "Config.h"
#include "Log.h"

#include <SimData/Ref.h>
#include <SimData/Date.h>
#include <SimData/DataManager.h>

#include <KineticsChannels.h>

#include <SimData/Types.h>
#include <SimData/ExceptionBase.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/FileUtility.h>
#include <SimData/GeoPos.h>
#include <SimData/Timing.h>

using bus::Kinetics;

ServerNode::ServerNode()
{

}

int ServerNode::run()
{
  int count = 0;
  
  int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
  csplog().setLogLevels(CSP_ALL, level);
  csplog().setOutput("ServerNode.log");
  
  printf("Network test server starting up...\n");
  
  Port remotePort = g_Config.getInt("Networking", "LocalMessagePort", 10000, true);
  std::string remoteHost = g_Config.getString("Networking", "LocalMessageHost", "127.0.0.1", true);

  Port localPort = (Port)g_Config.getInt("Networking", "RemoteMessagePort", 0, true);
  std::string localHost = g_Config.getString("Networking", "RemoteMessageHost", "127.0.0.1", true);
  
  NetworkNode * remoteNode = new NetworkNode(1, remoteHost.c_str(), remotePort );
  NetworkNode * localNode =  new NetworkNode(1, localHost.c_str(), localPort);
  
  NetworkMessenger * networkMessenger = new NetworkMessenger(localNode);
  PrintMessageHandler * printMessageHandler = new PrintMessageHandler();
  printMessageHandler->setFrequency(1);
  networkMessenger->registerReceiveHandler(printMessageHandler);

  EchoMessageHandler * echoMessageHandler = new EchoMessageHandler();
  echoMessageHandler->setMessenger(networkMessenger);
  networkMessenger->registerReceiveHandler(echoMessageHandler);
  
  //MessageSocketDuplex * socketDuplex = new MessageSocketDuplex(localPort);
  NetworkMessage * message=NULL;
  
  while(1)
  {
    networkMessenger->receiveMessages();
    int count = networkMessenger->getSendQueueCount();
    printf("SendQueueCount: %d\n", count);
    networkMessenger->sendQueuedMessages();

    simdata::tstart();

    simdata::tend();
    double etime;
    etime = simdata::tval();
    while((etime = simdata::tval()) < 3.0 ) {
      simdata::tend();
    }
    
  }
 
  return 0;
}


