#include "Networking.h"
#include "ClientNode.h"
#include <stdio.h>
#include <stdlib.h>
#include "Config.h"      

#include "Bus.h"

#include <SimData/Archive.h>
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

using bus::Kinetics;


ClientNode::ClientNode()
{

}

int ClientNode::run()
{

  int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
  csplog().setLogLevels(CSP_ALL, level);
  csplog().setOutput("ClientNode.log");
  
  printf("sizeof(int) = %d\n", sizeof(int));
  printf("sizeof(double) = %d\n", sizeof(double));
  printf("sizeof(simdata::Vector3) = %d\n", sizeof(simdata::Vector3));
  printf("sizeof(simdata::Quat) = %d\n", sizeof(simdata::Quat));
  printf("sizeof(simdata::SimTime) = %d\n", sizeof(simdata::SimTime));
  printf("sizeof(_Vector3Struct) = %d\n", sizeof(_Vector3Struct));
  printf("sizeof(_QuatStruct) = %d\n", sizeof(_QuatStruct));
  printf("sizeof(MessageHeader) = %d\n", sizeof(MessageHeader));
  printf("sizeof(NetworkMessage) = %d\n", sizeof(NetworkMessage));
  printf("sizeof(ObjectUpdateMessagePayload) = %d\n", sizeof(ObjectUpdateMessagePayload)); 
  
  MessageHeader header;
  header.dumpOffsets();
	
  printf("Network test client starting up...\n");
  Port localPort = g_Config.getInt("Networking", "LocalMessagePort", 10000, true);
  std::string localHost = g_Config.getString("Networking", "LocalMessageHost", "127.0.0.1", true);

  Port remotePort = (Port)g_Config.getInt("Networking", "RemoteMessagePort", 0, true);
  std::string remoteHost = g_Config.getString("Networking", "RemoteMessageHost", "127.0.0.1", true);
		   
  
  NetworkNode * remoteNode = new NetworkNode(1, remoteHost.c_str(), remotePort );
  NetworkNode * localNode =  new NetworkNode(1, localHost.c_str(), localPort);
  NetworkMessenger * networkMessenger = new NetworkMessenger(localNode);

  unsigned short messageType = 2;
  unsigned short payloadLen  = sizeof(int) + sizeof(double) + 3*sizeof(simdata::Vector3) +
	                       sizeof(simdata::Quat) /* + sizeof(simdata::Matrix3) + sizeof(double) */;

  NetworkMessage * message = networkMessenger->allocMessageBuffer(messageType, payloadLen);
  ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
  ptrPayload->dumpOffsets();
	   
//  NetworkMessage * message = networkMessenger->getMessageFromPool(1, 100);
//  memset(payloadPtr, 0 , 100);
//  strcpy(payloadPtr, "Hello From CSP Network Test Client!");
//  Port port = message->getOriginatorPort();

  simdata::uint32 id;
  simdata::uint32 type;
  float timestamp;
  DataChannel<simdata::Vector3>::Ref b_GlobalPosition;
  DataChannel<simdata::Vector3>::Ref b_AngularVelocity;
  DataChannel<simdata::Vector3>::Ref b_LinearVelocity;
  DataChannel<simdata::Quat>::Ref b_Attitude;

  b_GlobalPosition = DataChannel<simdata::Vector3>::newLocal(Kinetics::Position, simdata::Vector3::ZERO);
  b_AngularVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::AngularVelocity, simdata::Vector3::ZERO);
  b_LinearVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::Velocity, simdata::Vector3::ZERO);
  b_Attitude = DataChannel<simdata::Quat>::newLocal(Kinetics::Attitude, simdata::Quat::IDENTITY);

  id = 1;
  type = 1;
  timestamp = 0.0;
  b_GlobalPosition->value() = simdata::Vector3(1.0, 1.0, 1.0);
  b_AngularVelocity->value() = simdata::Vector3(1.0, 1.0, 1.0);
  b_LinearVelocity->value() = simdata::Vector3(1.0, 1.0, 1.0);
  b_Attitude->value() = simdata::Quat(1.0, 1.0, 1.0, 0.0);

 // ptrPayload->id = 1;
 // ptrPayload->timeStamp = 1.0;

  simdata::MemoryWriter writer((simdata::uint8 *)ptrPayload);
  writer << id;
  writer << type;
  writer << timestamp;
  
//  b_GlobalPosition->value().writeBinary((unsigned char *)&(ptrPayload->globalPosition),24);
//  b_LinearVelocity->value().writeBinary((unsigned char *)&(ptrPayload->linearVelocity),24);
//  b_AngularVelocity->value().writeBinary((unsigned char *)&(ptrPayload->angularVelocity),24);
//  b_Attitude->value().writeBinary((unsigned char *)&(ptrPayload->attitude),32);

  b_GlobalPosition->value().serialize(writer);
  b_LinearVelocity->value().serialize(writer);
  b_AngularVelocity->value().serialize(writer);
  b_Attitude->value().serialize(writer);
  
  ptrPayload->dump();
  
  networkMessenger->queueMessage(remoteNode, message);
  networkMessenger->sendQueuedMessages();
  
  return 0;
}



