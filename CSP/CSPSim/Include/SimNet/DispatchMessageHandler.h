#ifndef __DISPATCH_MESSAGE_HANDLER_H__
#define __DISPATCH_MESSAGE_HANDLER_H__

#include <SimNet/NetworkMessageHandler.h>

class DispatchMessageHandler : public NetworkMessageHandler
{
  public:
    DispatchMessageHandler();
    virtual void process(NetworkMessage * message);
    virtual ~DispatchMessageHandler();
    
    void setLocalAddress(unsigned int addr) { _addr = addr; }
    unsigned int getLocalAddress() { return _addr; }
    
    void setLocalPort(unsigned short port) { _port = port; }
    unsigned short getLocalPort() { return _port; }
    
    void setVirtualBattlefield(VirtualBattlefield * battlefield) { _virtualBattlefield = battlefield; }
    VirtualBattlefield * getVirtualBattlefield() { return _virtualBattlefield; }
    
    void setDataManager(simdata::DataManager & dataManager) { _dataManager = dataManager; }
    simdata::DataManager getDataManager() { return _dataManager; }
		
    
  protected:
    bool isLocal(unsigned int addr, unsigned short port);
    simdata::Ref<DynamicObject> addRemoteObject( NetworkMessage * message );
    
  private:
	  
    RemoteObjectTable m_RemoteObjectTable;
    unsigned int _addr;
    unsigned short _port; 
    VirtualBattlefield * _virtualBattlefield;
    simdata::DataManager _dataManager;

};


#endif
