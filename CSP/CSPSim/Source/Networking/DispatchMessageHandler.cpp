#include "Networking.h"
#include "DynamicObject.h"

#include "Config.h"
#include <SimData/FileUtility.h>

extern SimpleConfig g_Config;
DispatchMessageHandler::DispatchMessageHandler()
{
  _addr = 0;
  _port = 0;
}

DispatchMessageHandler::~DispatchMessageHandler()
{

}

void DispatchMessageHandler::process(NetworkMessage * message, NetworkMessenger * messenger)
{
  CSP_LOG(APP, DEBUG, "DispatchMessageHandler::process()");
  NetworkNode * node = message->getOriginatorNode();
  unsigned int addr = node->getAddress().getAddress().s_addr;
  unsigned short port = node->getPort();
  if (isLocal(addr, port))
	  return;
  simdata::uint16 messageType = message->getType();

  void * ptr = message->getPayloadPtr();
  ObjectUpdateMessagePayload * messagePayload = (ObjectUpdateMessagePayload*)ptr;

  unsigned int objectID = messagePayload->id;
  unsigned int objectType = messagePayload->id;

  RemoteObjectWrapper * remoteObjectWrapper = m_RemoteObjectTable.getRemoteObject(
		  addr, port, objectID );

  if (remoteObjectWrapper == NULL)
  {
        CSP_LOG(APP, DEBUG, "DispatchMessageHandler::process() - object not found in database so adding it");
	// TODO create new object and add it to table.
	//
	// set remoteObjectWrapper to value of new object.
        simdata::Ref<DynamicObject> obj = addRemoteObject(message);	
        CSP_LOG(APP, DEBUG, "DispatchMessageHandler::process() - wrapping new object");
	RemoteObjectWrapper * remoteObjectWrapper = new RemoteObjectWrapper(obj);
	// TODO add new object to wrapper object
        CSP_LOG(APP, DEBUG, "DispatchMessageHandler::process() - putting new object in remote object table");
	m_RemoteObjectTable.putRemoteObject( addr, port, objectID, remoteObjectWrapper );
  }
  simdata::Ref<DynamicObject> object = remoteObjectWrapper->getWrappedObject();
  object->putUpdateMessage(message);
	  

}

bool DispatchMessageHandler::isLocal(unsigned int addr, unsigned short port)
{
  return ((_addr == addr) && (_port == port));
}


simdata::Ref<DynamicObject> DispatchMessageHandler::addRemoteObject( NetworkMessage * message )
{
  CSP_LOG(APP, DEBUG, "DispatchMessageHandler::addRemoteObject() - adding new object");

  // use hard coded mirage for now.
  std::string vehicle = g_Config.getPath("Testing", "Vehicle", "sim:vehicles.aircraft.m2k", false);
  CSP_LOG(APP, DEBUG, "DispatchMessageHandler::addRemoteObject() - calling dataManager using path: " 
		  << vehicle.c_str());
  
  simdata::Ref<DynamicObject> ao = _dataManager.getObject(vehicle.c_str());
  assert(ao.valid());


  CSP_LOG(APP, DEBUG, "DispatchMessageHandler::addRemoteObject() - sending update message to new object");
  ao->putUpdateMessage(message);

  CSP_LOG(APP, DEBUG, "DispatchMessageHandler::addRemoteObject() - adding new object to battlefield");
  _virtualBattlefield->addUnit(ao);
  return ao;
}
