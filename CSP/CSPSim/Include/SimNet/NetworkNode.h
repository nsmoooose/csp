
#ifndef __NETWORKNODE_H__
#define __NETWORKNODE_H__

class NetworkNode
{
  private:
    ost::InetHostAddress m_addr;
    Port m_port;
    //uint16 m_node_id;
    
  public:
    NetworkNode();
//    NetworkNode(int node_id, ost::InetHostAddress addr, Port port);
//    NetworkNode(int node_id, simdata::uint32 addr, Port port);
//    NetworkNode(int node_id, const char * hostname, Port port);

    NetworkNode(ost::InetHostAddress addr, Port port);
    NetworkNode(simdata::uint32 addr, Port port);
    NetworkNode(const char * hostname, Port port);

    void setAddress(ost::InetHostAddress addr);
    void setPort(Port port);
//    void setId(short node_id);

//    short getId();
    Port getPort();
    ost::InetHostAddress getAddress();
    const char * getHostname();
    
};

#endif 
