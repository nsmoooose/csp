#ifndef __CLIENTNODE_H__
#define __CLIENTNODE_H__

class Battlefield;

class ClientNode
{

  public:
    ClientNode();
    int run();
  protected:

    void init();
    void dumpSizes();
    void initNetworking();

    Battlefield * m_battlefield;
    simdata::Ref<simdata::DataManager> m_DataManager;
    NetworkMessenger * m_networkMessenger;
    NetworkNode * m_remoteNode;
    NetworkNode * m_localNode;
    Port m_localPort;
    std::string m_localHost;
    Port m_remotePort;
    std::string m_remoteHost;

};

#endif

